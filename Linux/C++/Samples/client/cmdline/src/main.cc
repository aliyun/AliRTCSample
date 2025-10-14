#include <stdio.h>
#include <string.h>
#include <signal.h>

#include <string>
#include <thread>
#include <chrono>
#include <memory>

#include "tlog/file_path.h"
#include "tlog/tlog.h"
#include "tlog/utils.h"
#include "tlog/check.h"
#include "app_options.h"
#include "basictest_runner.h"
#include "autotest_runner.h"
#include "httpserver_test_runner.h"
#include "capi_test_runner.h"
#include "multiengine_test_runner.h"

using namespace tlog;

static void ShowUsage() {
    printf("Usage: onertc_demo [options]\n"
           "Options:\n"
           "    --help                          Show this help message and exit\n"
           "    --basic-test                    Start a basic test, reading config from `app_config.json`\n"
           "    --autotest-client               Start a autotest tcp client, receiving command from server\n"
           "    --http-server                   Start a http server, serving a webpage for testing\n"
           "    --capi-test                     Start a basic test for c api\n"
           "    --multiengine-test              Start a multiple rtc engine instances test\n"
           "    --remote-url=url                Set remote server url, such as tcp://host:port\n"
           "    --app-config=app_config.json    Specified app config file\n"
           "    --log-dir=dir                   Specified app and rtcsdk log storage directory\n"
           "    --device-id=device_id           Specified device id, used for autotest client\n"
           "\n");
}

static int g_signum = 0;

static bool ParseOptions(AppOptions& options, int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        TLOGI("User options, argv[%d]: %s", i, argv[i]);
        if (strcmp(argv[i], "--help") == 0) {
            options.help = true;
        } else if (strcmp(argv[i], "--basic-test") == 0) {
            options.basic_test = true;
        } else if (strcmp(argv[i], "--autotest-client") == 0) {
            options.autotest_client = true;
        } else if (strncmp(argv[i], "--remote-url", strlen("--remote-url")) == 0) {
            std::string remote_url_arg = argv[i];
            auto pos = remote_url_arg.find("=");
            if (pos != std::string::npos) {
                options.remote_url = remote_url_arg.substr(pos + 1);
            }
        } else if (strncmp(argv[i], "--app-config", strlen("--app-config")) == 0) {
            std::string app_config_filepath_arg = argv[i];
            auto pos = app_config_filepath_arg.find("=");
            if (pos != std::string::npos) {
                options.app_config_filepath = app_config_filepath_arg.substr(pos + 1);
            } else {
                TLOGW("--app-config empty value?");
            }
        } else if (strncmp(argv[i], "--log-dir", strlen("--log-dir")) == 0) {
            std::string log_dir_arg(argv[i]);
            auto pos = log_dir_arg.find("=");
            if (pos != std::string::npos) {
                options.log_dir = log_dir_arg.substr(pos+1);
            }
        } else if (strncmp(argv[i], "--device-id", strlen("--device-id")) == 0) {
            std::string device_id_arg(argv[i]);
            auto pos = device_id_arg.find("=");
            if (pos != std::string::npos) {
                options.device_id = device_id_arg.substr(pos+1);
            }
        } else if (strcmp(argv[i], "--http-server") == 0) {
            options.http_server_test = true;
        } else if (strcmp(argv[i], "--capi-test") == 0) {
            options.capi_test = true;
        } else if (strcmp(argv[i], "--multiengine-test") == 0) {
            options.multiengine_test = true;
        } else {
            TLOGE("Invalid option: %s\n", argv[i]);
        }
    }
    return true;
}

static void SignalHandler(int signum) {
    switch (signum) {
        case SIGINT:
            TLOGI("Signal SIGINT (%d) caught", signum);
            break;
        case SIGTERM:
            TLOGI("Signal SIGTERM (%d) caught", signum);
            break;
        default:
            TLOGI("Signal (%d) caught", signum);
    }
    g_signum = signum;
}

static void InitLogging(const std::string &log_dir) {
    if (log_dir.empty()) return;
#if !defined(NDEBUG) || defined(ENABLE_LOG_DETAILS)
    tlog::Logger::Instance().SetLogLevel(LOG_LEVEL_VERBOSE);
#else
    tlog::Logger::Instance().SetLogLevel(LOG_LEVEL_INFO);
#endif
    tlog::Logger::Instance().EnableConsoleOutput(true);

    std::string log_file_path = log_dir + "/onertc_demo_" + DateTimeFormat() + ".log";
    TLOGI("log_file_path: %s", log_file_path.c_str());
    tlog::Logger::Instance().SetLogFilePath(log_file_path.c_str());
}

static std::string GetDefaultLogDir() {
    tlog::FilePath cwd(tlog::CurrentWorkingDirectory());
    return cwd.Append("onertc_demo_log").value();
}

int main(int argc, char *argv[]) {
    std::string cwd = CurrentWorkingDirectory();
    TLOGI("App started from workdir: %s, argc: %d", cwd.c_str(), argc);

    AppOptions app_options;
    ParseOptions(app_options, argc, argv);
    if (argc < 2 || app_options.help) {
        ShowUsage();
        return 0;
    }

    signal(SIGINT, SignalHandler);
    signal(SIGTERM, SignalHandler);

    std::unique_ptr<TestingRunner> testing_runner;
    if (app_options.autotest_client) {
        if (app_options.remote_url.empty()) {
            app_options.remote_url = "tcp://127.0.0.1:9999";
        }
        testing_runner.reset(new AutoTestRunner(&app_options));
    } else if (app_options.http_server_test) {
        testing_runner.reset(new HttpServerTestRunner(&app_options));
    } else if (app_options.basic_test) {
        if (app_options.app_config_filepath.empty()) {
            // default app config file assumed in current working directory
            app_options.app_config_filepath = cwd + "/app_config.json";
        }
        testing_runner.reset(new BasicTestRunner(&app_options));
    } else if (app_options.capi_test) {
        if (app_options.app_config_filepath.empty()) {
            app_options.app_config_filepath = cwd + "/app_config.json";
        }
        testing_runner.reset(new CApiTestRunner(&app_options));
    } else if (app_options.multiengine_test) {
        if (app_options.app_config_filepath.empty()) {
            app_options.app_config_filepath = cwd + "/multi_app_config.json";
        }
        testing_runner.reset(new MultiEngineTestRunner(&app_options));
    } else {
        TLOGE("nothing to run");
        return 1;
    }

    // Init logging path
    std::string log_dir = testing_runner->GetLogDir();
    InitLogging(log_dir);
    
    if (!testing_runner->Start()) {
        TLOGE("testing runner start failed");
        return -1;
    }

    while (true) {
        if (g_signum) {
            TLOGI("signaled break out, %d", g_signum);
            break;
        }
        if (testing_runner->IsCompleted()) {
            TLOGI("testing runner completed break out");
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    testing_runner->Stop();

    TLOGI("App exit ...");
    return 0;
}
