#include "tcp_client.h"

#include "mongoose/mongoose.h"
#include "tlog/tlog.h"
#include "tlog/utils.h"


#define NETWORK_IO_DEBUG 1


TcpClient::TcpClient() {}
TcpClient::~TcpClient() {}

void TcpClient::SetCallback(TcpClientCallback *callback) {
    callback_ = callback;
}

void TcpClient::SetUrl(const std::string &url) {
    TLOGI("TcpClient::SetUrl, url: %s", url.c_str());
    url_ = url;
}

int TcpClient::Start() {
    TLOGI("TcpClient::Start");

    if (!VerifyUrl()) {
        TLOGE("verify url failed");
        return -1;
    }

#if !defined(NDEBUG)
    mg_log_set(MG_LL_VERBOSE);
#else
    mg_log_set(MG_LL_INFO);
#endif

#if NETWORK_IO_DEBUG
    mg_log_set(MG_LL_VERBOSE);
#endif

    if (!mgr_) {
        mgr_ = (struct mg_mgr *)calloc(1, sizeof(struct mg_mgr));
        mg_mgr_init(mgr_);
        if (auto_reconnect_) {
            mg_timer_add(mgr_, timer_interval_ms_, MG_TIMER_REPEAT | MG_TIMER_RUN_NOW, &TcpClient::MgTimerFunc, this);
        }
    }

    thread_.reset(new tlog::Thread());
    thread_->Start();
    thread_->PostTask([this] {
        conn_ = mg_connect(mgr_, url_.c_str(), &TcpClient::MgEventHandler, this);
        RunLoop();
    });

    return 0;
}

void TcpClient::RunLoop() {
    mg_mgr_poll(mgr_, poll_interval_ms_);
    thread_->PostDelayedTask([this]{
        RunLoop();
    }, poll_interval_ms_);
}

void TcpClient::Stop() {
    TLOGI("TcpClient::Stop");
    thread_.reset(nullptr);
    if (mgr_) {
        mg_mgr_free(mgr_);
        mgr_ = nullptr;
    }
    conn_ = nullptr;
}

bool TcpClient::SendDataUnlocked(const uint8_t *data, int size) {
#if NETWORK_IO_DEBUG
    TLOGI("SendData, size: %d", size);
    mg_hexdump(data, size);
#endif
    if (!conn_) {
        TLOGW("no data will be sent, conn_: %p", conn_);
        return false;
    }
    if (!mg_send(conn_, data, size)) {
        TLOGE("mg_send error");
        return false;
    }
    return true;
}

bool TcpClient::SendData(const uint8_t *data, int size) {
    if (thread_) {
        return thread_->SendTask([this, data, size] {
            return SendDataUnlocked(data, size);
        });
    }
    return false;
}

void TcpClient::MgEventHandler(struct mg_connection *c, int ev, void *ev_data) {
    TcpClient *p_this = (TcpClient *)c->fn_data;
    if (p_this) {
        p_this->OnMgEvent(c, ev, ev_data);
    }
}

void TcpClient::OnMgEvent(struct mg_connection *c, int ev, void *ev_data) {
    if (ev == MG_EV_OPEN) {
        TLOGV("CLIENT created");
    } else if (ev == MG_EV_CONNECT) {
        TLOGI("CLIENT connected");
        if (callback_) {
            callback_->OnConnected();
        }
    } else if (ev == MG_EV_READ) {
        struct mg_iobuf *r = &c->recv;

    #if NETWORK_IO_DEBUG
        TLOGI("CLIENT got data len: %zu", r->len);
        mg_hexdump(r->buf, r->len);
    #endif

        if (callback_) {
            callback_->OnRecvData(r->buf, (int)r->len);
        }

        r->len = 0;  // Tell Mongoose we've consumed data
    } else if (ev == MG_EV_CLOSE) {
        TLOGI("CLIENT disconnected");
        conn_ = nullptr;
        if (callback_) {
            callback_->OnDisconnect();
        }
    } else if (ev == MG_EV_ERROR) {
        TLOGW("CLIENT got error: %s", (char *)ev_data);
        if (callback_) {
            callback_->OnError((const char *)ev_data);
        }
    } else if (ev == MG_EV_POLL) {
        // TLOGV("CLIENT poll");
    } else {
        TLOGV("ClientEventHandler, ev: %d", ev);
    }
}

void TcpClient::MgTimerFunc(void *arg) {
    TcpClient *p_this = (TcpClient*)arg;
    TLOGV("MgTimerFunc, tick... this: %p", p_this);
    if (p_this) {
        p_this->OnTimer();
    }
}

void TcpClient::OnTimer() {
    if (!conn_ && auto_reconnect_) {
        conn_ = mg_connect(mgr_, url_.c_str(), &TcpClient::MgEventHandler, this);
        TLOGI("try to reconnect, conn_: %p", conn_);
    }
}

bool TcpClient::VerifyUrl() {
    // valid format: tcp://host:port
    
    auto pos = url_.find("tcp://");
    if (pos == std::string::npos || pos != 0) {
        TLOGE("invalid url, pos: %d", (int)pos);
        return false;
    }
    auto host_port = tlog::SplitString(url_.substr(6), ':');
    if (host_port.size() != 2 || host_port[0].empty() || host_port[1].empty()) {
        TLOGE("Invalid host:port");
        return false;
    }
    // std::string ip = ip_port[0];
    // int port = std::stoi(ip_port[1]);
    // if (!IsValidIPv4Address(ip_port[0].c_str())) {
    //     TLOGE("Invalid ip: %s", ip.c_str());
    //     return false;
    // }
    // if (!IsValidPort(port)) {
    //     TLOGE("Invalid port: %d", port);
    //     return false;
    // }
    return true;
}
