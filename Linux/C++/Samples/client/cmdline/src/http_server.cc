#include "http_server.h"
#include <string>
#include <unordered_map>
#include <utility>
#include "mongoose/mongoose.h"
#include "tlog/tlog.h"

static const char *s_root_dir = "web_root";

HttpServer::HttpServer() {

}

HttpServer::~HttpServer() {

}

void HttpServer::SetCallback(HttpServerCallback *callback) {
    callback_ = callback;
}

void HttpServer::SetUrl(const std::string &url) {
    TLOGI("HttpServer::SetUrl, url: %s", url.c_str());
    url_ = url;
}

bool HttpServer::Start() {
    TLOGI("HttpServer::Start");

#if !defined(NDEBUG)
    mg_log_set(MG_LL_VERBOSE);
#else
    mg_log_set(MG_LL_INFO);
    // mg_log_set(MG_LL_VERBOSE);
#endif

    if (!mgr_) {
        mgr_ = (struct mg_mgr *)calloc(1, sizeof(struct mg_mgr));
        mg_mgr_init(mgr_);
    }

    thread_ = std::thread([this] {
        mg_http_listen(mgr_, url_.c_str(), &HttpServer::MgEventHandler, this);  // Create HTTP listener
        while (!quit_) {
            mg_mgr_poll(mgr_, poll_interval_ms_);
        }
    });
    
    return true;
}

void HttpServer::Stop() {
    quit_ = true;
    if (thread_.joinable()) {
        thread_.join();
    }
    quit_ = false;
    if (mgr_) {
        mg_mgr_free(mgr_);
        mgr_ = nullptr;
    }
    http_conn_ = nullptr;
    ws_conn_ = nullptr;
}

void HttpServer::MgEventHandler(struct mg_connection *c, int ev, void *ev_data) {
    HttpServer *p_this = (HttpServer *)c->fn_data;
    if (p_this) {
        p_this->OnMgEvent(c, ev, ev_data);
    }
}

void HttpServer::OnMgEvent(struct mg_connection *c, int ev, void *ev_data) {
    if (ev == MG_EV_OPEN && c->is_listening) {
        TLOGI("HttpServer::OnMgEvent, MG_EV_OPEN, c: %p", (void *)c);
    } else if (ev == MG_EV_HTTP_MSG) {
        http_conn_ = c;
        struct mg_http_message *hm = (struct mg_http_message *)ev_data;
        std::string method(hm->method.ptr, hm->method.len);
        std::string uri(hm->uri.ptr, hm->uri.len);
        std::string query(hm->query.ptr, hm->query.len);
        std::string proto(hm->proto.ptr, hm->proto.len);
        std::string body(hm->body.ptr, hm->body.len);
        std::string head(hm->head.ptr, hm->head.len);
        // std::string message(hm->message.ptr, hm->message.len);

        std::unordered_map<std::string, std::string> headers;
        int max = sizeof(hm->headers) / sizeof(hm->headers[0]);
        for (int i = 0; i < max; i++) {
            headers.insert(std::make_pair(std::string(hm->headers[i].name.ptr, hm->headers[i].name.len),
                                          std::string(hm->headers[i].value.ptr, hm->headers[i].value.len)));
        }

        TLOGI("HttpServer::OnMgEvent, MG_EV_HTTP_MSG, c: %p", (void *)c);
        TLOGI("method: %s", method.c_str());
        TLOGI("uri: %s", uri.c_str());
        TLOGI("query: %s", query.c_str());
        TLOGI("proto: %s", proto.c_str());
        TLOGI("body: %s", body.c_str());
        TLOGI("head: %s", head.c_str());
        // TLOGI("message: %s", message.c_str());

        struct mg_str *upgrade = mg_http_get_header(hm, "Upgrade");
        if (upgrade && mg_strcmp(*upgrade, mg_str("websocket")) == 0) {
            mg_ws_upgrade(c, hm, NULL);  // Upgrade HTTP to Websocket
        } else if (mg_http_match_uri(hm, "/") || mg_http_match_uri(hm, "/index.html") ||
            mg_http_match_uri(hm, "/app.js") || mg_http_match_uri(hm, "/style.css") || mg_http_match_uri(hm, "/favicon.ico")) {
            struct mg_http_serve_opts opts = {.root_dir = s_root_dir};
            mg_http_serve_dir(c, hm, &opts);
        } else {
            if (callback_) {
                callback_->OnHttpRequest(method, uri, headers, body);
            }
        }
    } else if (ev == MG_EV_WS_OPEN) {
        TLOGI("HttpServer::OnMgEvent, MG_EV_WS_OPEN, c: %p", (void *)c);
        ws_conn_ = c;
        if (callback_) {
            callback_->OnWsConnected();
        }
    } else if (ev == MG_EV_WS_MSG) {
        // Got websocket frame. Received data is wm->data. Echo it back!
        struct mg_ws_message *wm = (struct mg_ws_message *) ev_data;
        // TLOGI("HttpServer::OnMgEvent, MG_EV_WS_MSG, len:%d, data:%s", wm->data.len, wm->data.ptr);
        // mg_ws_send(c, wm->data.ptr, wm->data.len, WEBSOCKET_OP_TEXT);
        if (callback_) {
            callback_->OnWsMessage(std::string(wm->data.ptr, wm->data.len));
        }
    } else if (ev == MG_EV_CLOSE) {
        TLOGI("HttpServer::OnMgEvent, MG_EV_CLOSE, c: %p", (void *)c);
        if (callback_) {
            callback_->OnConnectionClose();
        }
        ws_conn_ = nullptr;
        http_conn_ = nullptr;
    } else {
        // TLOGI("HttpServer::OnMgEvent, ev: %d", ev);
    }
}

static const char *s_json_header =
    "Content-Type: application/json\r\n"
    "Cache-Control: no-cache\r\n";
void HttpServer::SendHttpResponse(int code, const std::string &response) {
    if (http_conn_) {
        mg_http_reply(http_conn_, code, s_json_header, "%s", response.c_str());
    }
}

void HttpServer::SendWsMessage(const std::string &msg) {
    if (ws_conn_) {
        TLOGI("HttpServer::SendWsMessage, msg: %s", msg.c_str());
        mg_ws_send(ws_conn_, msg.data(), msg.size(), WEBSOCKET_OP_TEXT);
    }
}