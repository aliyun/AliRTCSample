#ifndef __TCP_CLIENT_H__
#define __TCP_CLIENT_H__

#include <stdint.h>
#include <memory>
#include <mutex>
#include <string>

#include "tlog/thread.h"

class TcpClientCallback {
public:
    TcpClientCallback() {}
    virtual ~TcpClientCallback() {}

    virtual void OnConnected() = 0;
    virtual void OnDisconnect() = 0;
    virtual void OnError(const char *desc) = 0;
    virtual void OnRecvData(const uint8_t *data, int len) = 0;
};

struct mg_mgr;
struct mg_connection;

class TcpClient {
public:
    TcpClient();
    virtual ~TcpClient();

    void SetCallback(TcpClientCallback *callback);
    // url: tcp:://host:port
    void SetUrl(const std::string &url);
    virtual int Start();
    virtual void Stop();
    bool SendData(const uint8_t *data, int size);
    bool SendDataUnlocked(const uint8_t *data, int size);
    void EnableAutoReconnect(bool auto_reconnect, int timer_interval_ms) {
        auto_reconnect_ = auto_reconnect;
        timer_interval_ms_ = timer_interval_ms;
    }

protected:
    static void MgEventHandler(struct mg_connection *c, int ev, void *ev_data);
    static void MgTimerFunc(void *arg);

    void OnMgEvent(struct mg_connection *c, int ev, void *ev_data);
    void OnTimer();

    bool VerifyUrl();
    void RunLoop();

protected:
    TcpClientCallback *callback_{nullptr};
    struct mg_mgr *mgr_{nullptr};
    struct mg_connection *conn_{nullptr};
    std::string url_;
    int poll_interval_ms_{50};
    int timer_interval_ms_{5000};
    bool auto_reconnect_{true};
    std::mutex lock_;
    std::unique_ptr<tlog::Thread> thread_;
};

#endif // __TCP_CLIENT_H__
