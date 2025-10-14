#ifndef __HTTP_SERVER_H__
#define __HTTP_SERVER_H__

#include <thread>
#include <string>
#include <unordered_map>

struct mg_mgr;
struct mg_connection;

class HttpServerCallback {
public:
    HttpServerCallback() {}
    virtual ~HttpServerCallback() {}

    virtual void OnHttpRequest(const std::string &method,
                               const std::string &uri,
                               const std::unordered_map<std::string, std::string> &headers,
                               const std::string &body) = 0;
    virtual void OnWsConnected() = 0;
    virtual void OnWsMessage(const std::string &msg) = 0;
    virtual void OnConnectionClose() = 0;
};

class HttpServer {
public:
    HttpServer();
    ~HttpServer();
    
    void SetCallback(HttpServerCallback *callback);
    void SetUrl(const std::string &url);
    bool Start();
    void Stop();
    bool IsRunning();

    void SendHttpResponse(int code, const std::string &response);
    void SendWsMessage(const std::string &msg);

protected:
    static void MgEventHandler(struct mg_connection *c, int ev, void *ev_data);
    void OnMgEvent(struct mg_connection *c, int ev, void *ev_data);

    struct mg_mgr *mgr_{nullptr};
    struct mg_connection *http_conn_{nullptr};
    struct mg_connection *ws_conn_{nullptr};
    int poll_interval_ms_{100};

    std::string url_;
    std::thread thread_;
    bool quit_{false};

    HttpServerCallback *callback_{nullptr};
};

#endif // __HTTP_SERVER_H__
