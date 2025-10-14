#ifndef __AUTOTEST_TCP_CLIENT_H__
#define __AUTOTEST_TCP_CLIENT_H__

#include <memory>
#include "tcp_client.h"
#include "tlog/buffer.h"

class AutoTestRtcSdkClient;

class AutoTestTcpClient : public TcpClient, public TcpClientCallback {
public:
    AutoTestTcpClient();
    virtual ~AutoTestTcpClient();
    
    void SetRtcLogDir(const std::string &log_dir);
    void SetDeviceId(const std::string &device_id);
    int Start() override;
    void Stop() override;

    // --- implements TcpClientCallback ---
    void OnConnected() override;
    void OnDisconnect() override;
    void OnError(const char *desc) override;
    void OnRecvData(const uint8_t *data, int size) override;

protected:
    friend class AutoTestRtcSdkClient;
    typedef enum {
		MSG_UNKNOWN = 0,
        MSG_C2S_META = 1,
		MSG_S2C_COMMAND = 3,
		MSG_S2C_PING = 5,
		MSG_C2S_PONG = 6,
		MSG_C2S_COMMND_RETURN = 8,
		MSG_C2S_EVENT = 9,
        MSG_S2C_EVENT_ACK = 10,
    } MsgType;
    const char *MsgTypeString(MsgType type);

    void OnRecvOneMessage(const uint8_t *body, int len);

    // void SendMsg(MsgType type, const uint8_t *payload, int len);
    void SendMetaMsg(const std::string &meta);
    void SendPongMsg(const uint8_t *ping_msg, int len);
    void SendCommandResultMsg(uint64_t invoke_id, const std::string &result);
    void SendEventMsg(const std::string &event);

    tlog::Buffer read_buf_;
    std::unique_ptr<AutoTestRtcSdkClient> rtc_client_;
    std::string rtc_log_dir_;
    std::string device_id_;
    uint64_t event_invoke_id_ = 100;
};


// class MsgData {
// public:
//     MsgData(size_t size)
//         : size_(size)
//         , data_(size > 0 ? new uint8_t[size] : nullptr) {}

//     uint8_t* data() { return data_.get(); }
//     size_t size() { return size_; }

//     template <typename U>
//     void append(const U *data, size_t size) {
//         memcpy(data_.get() + write_pos_, data, size * sizeof(U));
//         write_pos_ += size;
//     }

//     template <typename U, size_t N>
//     void append(const U (&array)[N]) {
//         append(array, N);
//     }

//     template <typename U>
//     void append(const U &item) {
//         append(&item, 1);
//     }

// private:
//     std::unique_ptr<uint8_t[]> data_;
//     size_t size_{0};
//     size_t write_pos_{0};
// };

#endif  // __AUTOTEST_TCP_CLIENT_H__
