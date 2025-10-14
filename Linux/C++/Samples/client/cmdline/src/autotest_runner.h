#ifndef __AUTOTEST_RUNNER_H__
#define __AUTOTEST_RUNNER_H__

#include <memory>
#include "testing_runner.h"

class AutoTestTcpClient;

class AutoTestRunner : public TestingRunner {
public:
    AutoTestRunner(AppOptions *options);
    virtual ~AutoTestRunner();

    // --- implements TestingRunner ---
    bool Start() override;
    void Stop() override;
    std::string GetDefaultLogDir() override;
    
private:
    std::unique_ptr<AutoTestTcpClient> tcp_client_;
    std::string device_id_;
    std::string log_dir_;
};


#endif // __AUTOTEST_RUNNER_H__
