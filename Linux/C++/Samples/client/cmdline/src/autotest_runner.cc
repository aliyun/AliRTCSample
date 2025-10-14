#include "autotest_runner.h"
#include <random>
#include "autotest_tcp_client.h"
#include "tlog/file_path.h"
#include "tlog/tlog.h"
#include "tlog/utils.h"
#include "app_options.h"

// static const char* kDefaultRemoteTcpIP = "127.0.0.1";
// static const int kDefaultRemoteTcpPort = 9999;

static std::string GenerateDeviceId() {
    return std::to_string(std::random_device()());
}

AutoTestRunner::AutoTestRunner(AppOptions *options)
    : TestingRunner(options) {
    TLOGI("AutoTestRunner ctor");
    device_id_ = options->device_id;
    if (device_id_.empty()) {
        device_id_ = GenerateDeviceId();
    }
    TLOGI("device_id_: %s", device_id_.c_str());
}

AutoTestRunner::~AutoTestRunner() {
    TLOGI("AutoTestRunner dtor");
}

// --- implements TestingRunner ---
bool AutoTestRunner::Start() {
    TLOGI("AutoTestRunner::Start");
    tcp_client_.reset(new AutoTestTcpClient());
    tcp_client_->SetUrl(app_options_->remote_url);
    tcp_client_->SetDeviceId(device_id_);
    tcp_client_->SetRtcLogDir(GetLogDir());
    if (tcp_client_->Start() != 0) {
        return false;
    }
    return true;
}
void AutoTestRunner::Stop() {
    TLOGI("AutoTestRunner::Stop");
    tcp_client_->Stop();
}

std::string AutoTestRunner::GetDefaultLogDir() {
    if (log_dir_.empty()) {
        tlog::FilePath exe_path(tlog::GetExecutablePath());
        const std::string log_dir_name("DingRtcAutoTestLog");
        std::string ymd = tlog::DateTimeFormat("%Y-%m-%d");
        std::string hms = tlog::DateTimeFormat("%H-%M-%S");
        // log dir: ${path_to_onertc_demo}/../DingRtcAutoTestLog/YYYY-mm-dd/HH-MM-SS_${device_id}
        log_dir_ = exe_path.DirName().DirName().Append(log_dir_name).Append(ymd).Append(hms + "_linux_" + device_id_).value();
        // TLOGI("log dir default: %s", log_dir_.c_str());
    }
    return log_dir_;
}