#ifndef __APP_OPTIONS_H__
#define __APP_OPTIONS_H__

#include <string>

struct AppOptions {
    bool help = false;
    bool basic_test = false;
    bool autotest_client = false;
    bool http_server_test = false;
    bool capi_test = false;
    bool multiengine_test = false;
    std::string remote_url;
    std::string app_config_filepath;
    std::string log_dir;
    std::string device_id;
};

#endif // __APP_OPTIONS_H__
