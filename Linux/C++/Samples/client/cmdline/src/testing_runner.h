#ifndef __TESTING_RUNNER_H__
#define __TESTING_RUNNER_H__

#include "app_options.h"
#include "tlog/utils.h"

class TestingRunner {
public:
    TestingRunner(AppOptions *options) : app_options_(options) {}
    virtual ~TestingRunner() {}

    virtual bool Start() = 0;
    virtual void Stop() = 0;
    bool IsCompleted() { return is_completed_; }
    virtual std::string GetLogDir() {
        return (app_options_ && !app_options_->log_dir.empty()) ? app_options_->log_dir : GetDefaultLogDir();
    }

protected:
    AppOptions *app_options_;
    volatile bool is_completed_{false};
    
    virtual void SetCompleted() { is_completed_ = true; }
    void ResetCompleted() { is_completed_ = false; }
    virtual std::string GetDefaultLogDir() {
        return tlog::CurrentWorkingDirectory() + "/onertc_demo_log";
    }
};

#endif  // __TESTING_RUNNER_H__
