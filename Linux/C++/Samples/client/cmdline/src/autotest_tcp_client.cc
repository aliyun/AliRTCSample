#include "autotest_tcp_client.h"

#include "nlohmann/json.hpp"
#include "tlog/tlog.h"
#include "tlog/utils.h"
#include "autotest_rtcsdk_client.h"

using namespace tlog;
using json = nlohmann::json;


static const int kMsgHeaderLength = 4;
static const int kMsgTypeLength = 16;
static const int kMaxMsgLength = 1024 * 1024 * 2;
static const int kDefaultMsgLength = 1024 * 2;

static const char *os_type() {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#ifdef _WIN64
    return "windows";
#else
    return "windows";
#endif
#elif __APPLE__
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR
    return "ios-simulator";
#elif TARGET_OS_IPHONE
    return "ios";
#elif TARGET_OS_MAC
    return "mac";
#else
    return "apple";
#endif
#elif __ANDROID__
    return "android";
#elif __linux__
    return "linux";
#elif __unix__  // all unices not caught above
    return "unix";
#elif defined(_POSIX_VERSION)
    return "posix";
#else
#error "Unknown compiler"
    return "unknown";
#endif
}

AutoTestTcpClient::AutoTestTcpClient() {
    SetCallback(this);
}

AutoTestTcpClient::~AutoTestTcpClient() {}

void AutoTestTcpClient::SetRtcLogDir(const std::string &log_dir) {
    rtc_log_dir_ = log_dir;
}

void AutoTestTcpClient::SetDeviceId(const std::string &device_id) {
    device_id_ = device_id;
}

int AutoTestTcpClient::Start() {
    TLOGI("AutoTestTcpClient::Start");
    rtc_client_.reset(new AutoTestRtcSdkClient(this));
    rtc_client_->SetRtcLogDir(rtc_log_dir_);
    read_buf_.EnsureCapacity(kDefaultMsgLength);
    return TcpClient::Start();
}

void AutoTestTcpClient::Stop() {
    TLOGI("AutoTestTcpClient::Stop");
    TcpClient::Stop();
    rtc_client_.reset(nullptr);
    read_buf_.Clear();
}

void AutoTestTcpClient::OnConnected() {
    TLOGI("AutoTestTcpClient::OnConnected");

    std::string sdkVersion =
        std::string(ding::rtc::RtcEngine::GetSDKVersion()) + "." + ding::rtc::RtcEngine::GetSDKBuild();

    json j_meta = {
        {"sdkVersion", sdkVersion},
        {"os", os_type()},
        {"osVersion", OsVersion()},
        {"deviceID", device_id_},
        {"clientTime", CurrentSystemTimeMillis()}
    };

    SendMetaMsg(j_meta.dump());
}

void AutoTestTcpClient::OnDisconnect() {
    TLOGI("AutoTestTcpClient::OnDisconnect");
}

void AutoTestTcpClient::OnError(const char *desc) {
    TLOGI("AutoTestTcpClient::OnError, desc: %s", desc?desc:"");
}

void AutoTestTcpClient::OnRecvData(const uint8_t *data, int size) {
    TLOGI("AutoTestTcpClient::OnRecvData, data: %p, size: %d", data, size);

    int pos = 0;
    while (pos < size) {
        // read header
        if (read_buf_.size() < kMsgHeaderLength) {
            int to_read = MIN(size - pos, kMsgHeaderLength - read_buf_.size());
            TLOGV("bufsize: %d, pos: %d, size: %d, to_read: %d", (int)read_buf_.size(), pos, size, to_read);
            if (to_read > 0) {
                read_buf_.AppendData(data + pos, to_read);
                pos += to_read;
            }
        }

        // read body
        int body_length = 0;
        if (read_buf_.size() >= kMsgHeaderLength) {
            body_length = ReadU32LE(read_buf_.data());
            assert(body_length + kMsgHeaderLength < kMaxMsgLength);
            assert(body_length + kMsgHeaderLength >= read_buf_.size());
            int to_read = MIN(size - pos, body_length + kMsgHeaderLength - read_buf_.size());
            TLOGV("bufsize: %d, body_length: %d, pos: %d, size: %d, to_read: %d", (int)read_buf_.size(), body_length, pos, size, to_read);
            if (to_read > 0) {
                read_buf_.AppendData(data + pos, to_read);
                pos += to_read;
            }
        }

        // read complete
        if (read_buf_.size() == body_length + kMsgHeaderLength) {
            TLOGI("read completed, size: %d", (int)read_buf_.size());
            OnRecvOneMessage(read_buf_.data() + kMsgHeaderLength, body_length);
            read_buf_.Clear();
        }
    }
}

void AutoTestTcpClient::OnRecvOneMessage(const uint8_t *body, int len) {
    // mg_hexdump(body, kMsgTypeLength);
    if (strncmp((const char *)body, "command", 7) == 0) {
        assert(len > sizeof(uint64_t));
        if (len < sizeof(uint64_t)) {
            TLOGE("invalid command message length: %d", len);
            return;
        }
        uint64_t invoke_id = ReadU64BE(body + kMsgTypeLength);
        std::string cmd((const char *)(body + kMsgTypeLength + sizeof(uint64_t)),
                        len - kMsgTypeLength - sizeof(uint64_t));
        if (rtc_client_) {
            std::string result = rtc_client_->OnRecvCommand(invoke_id, cmd);
            if (!result.empty()) {
                TLOGI("send command result: %s", result.c_str());
                SendCommandResultMsg(invoke_id, result);
            }
        }
    } else if (strncmp((const char *)body, "ping", 4) == 0) {
        SendPongMsg(body + kMsgTypeLength, len - kMsgTypeLength);
    } else if (strncmp((const char *)body, "event-ack", 9) == 0) {
        assert(len > sizeof(uint64_t));
        if (len < sizeof(uint64_t)) {
            TLOGE("invalid event-ack message length: %d", len);
            return;
        }
        uint64_t invoke_id = ReadU64BE(body + kMsgTypeLength);
        TLOGI("recv event-ack message, invoke_id: %lld", invoke_id);
    }
}

// void AutoTestTcpClient::SendMsg(MsgType type, const uint8_t *payload, int len) {
//     assert(len <= kMaxMsgLength);

//     size_t buf_size = kMsgHeaderLength + kMsgTypeLength + len;
//     std::unique_ptr<uint8_t[]> buf(new uint8_t[buf_size]);

//     // write header
//     uint32_t body_length = kMsgTypeLength + len;
//     WriteU32LE(buf.get(), body_length);
    
//     // write type
//     strcpy(buf.get() + kMsgHeaderLength, MsgTypeString());

//     // write payload
//     memcpy(buf.get() + kMsgHeaderLength + kMsgTypeLength, payload, len);
    
//     TLOGV("send msg type: %s, size: %d", MsgTypeString(), buf_size);

//     // mg_hexdump(buf.get(), buf_size);

//     SendData(buf.get(), buf_size);
// }

void AutoTestTcpClient::SendMetaMsg(const std::string &meta) {
    char type[kMsgTypeLength] = "meta";
    uint32_t body_length = kMsgTypeLength + meta.size();
    
    Buffer buf(0, kMsgHeaderLength + body_length);
    buf.AppendData((const uint8_t *)&body_length, sizeof(uint32_t));
    buf.AppendData(type);
    buf.AppendData(meta.data(), meta.size());

    TLOGV("SendMeta, buf size: %lld", buf.size());

    SendDataUnlocked(buf.data(), buf.size());
}

void AutoTestTcpClient::SendPongMsg(const uint8_t *ping_msg, int len) {
    char type[kMsgTypeLength] = "pong";
    std::string timestamp = std::to_string(CurrentSystemTimeMillis());
    uint32_t body_length = kMsgTypeLength + len + timestamp.length() + 1;
    
    Buffer buf(0, kMsgHeaderLength + body_length);
    buf.AppendData((const uint8_t *)&body_length, sizeof(uint32_t));
    buf.AppendData(type);
    buf.AppendData(ping_msg, len);
    buf.AppendData(' ');
    buf.AppendData(timestamp.data(), timestamp.size());

    TLOGV("SendPong, buf size: %lld", buf.size());

    SendDataUnlocked(buf.data(), buf.size());

    // MsgData msg(kMsgHeaderLength + kMsgTypeLength + len + timestamp.length() + 1);
    // msg.append((uint32_t)(msg.size - kMsgHeaderLength));
    // msg.append(type);
    // msg.append(payload, len);
    // msg.append(' ');
    // msg.append(timestamp.data(), timestamp.size());
}

void AutoTestTcpClient::SendCommandResultMsg(uint64_t invoke_id, const std::string& result) {
    char type[kMsgTypeLength] = "return";
    uint32_t body_length = kMsgTypeLength + sizeof(uint64_t) + result.size();
    
    Buffer buf(0, kMsgHeaderLength + body_length);
    buf.AppendData((const uint8_t *)&body_length, sizeof(uint32_t));
    buf.AppendData(type);
    uint64_t invoke_id_be;
    WriteU64BE((uint8_t *)&invoke_id_be, invoke_id);
    buf.AppendData((const uint8_t *)&invoke_id_be, sizeof(uint64_t));
    buf.AppendData(result.data(), result.size());

    TLOGV("SendCommandResult, buf size: %lld", buf.size());

    SendDataUnlocked(buf.data(), buf.size());
}

void AutoTestTcpClient::SendEventMsg(const std::string &event) {
    char type[kMsgTypeLength] = "event";
    uint32_t body_length = kMsgTypeLength + sizeof(uint64_t) + event.size();

    Buffer buf(0, kMsgHeaderLength + body_length);
    buf.AppendData((const uint8_t *)&body_length, sizeof(uint32_t));
    buf.AppendData(type);
    uint64_t invoke_id_be;
    WriteU64BE((uint8_t *)&invoke_id_be, event_invoke_id_++);
    buf.AppendData((const uint8_t *)&invoke_id_be, sizeof(uint64_t));
    buf.AppendData(event.data(), event.size());

    TLOGV("SendEventMsg, buf size: %lld", buf.size());

    SendData(buf.data(), buf.size());
}

const char* AutoTestTcpClient::MsgTypeString(MsgType type) {
    switch (type) {
        case MSG_C2S_META:
            return "meta";
        case MSG_S2C_COMMAND:
            return "command";
        case MSG_S2C_PING:
            return "ping";
        case MSG_C2S_PONG:
            return "pong";
        case MSG_C2S_COMMND_RETURN:
            return "return";
        case MSG_C2S_EVENT:
            return "event";
        case MSG_S2C_EVENT_ACK:
            return "event-ack";
        case MSG_UNKNOWN:
            return "unknown";
        default:
            return "";
    }
}
