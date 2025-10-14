#ifndef __MULTIENGINE_TEST_RUNNER_H__
#define __MULTIENGINE_TEST_RUNNER_H__

#include <stdio.h>
#include <string>
#include <memory>
#include <unordered_map>
#include "tlog/thread.h"
#include "app_config.h"
#include "testing_runner.h"
#include "external_source_pusher.h"
#include "engine_interface.h"

class EngineCallback;
class MultiEngineTestRunner;

struct EngineContext {
    AppConfig *app_config = nullptr;
    ding::rtc::RtcEngineAuthInfo auth_info;
    ding::rtc::RtcEngine *rtc_engine = nullptr;
    int engine_id = 0;
    EngineCallback *callback = nullptr;
    ExternalSourcePusher *external_source_pusher = nullptr;
    FILE *playback_audio_dump_file = nullptr;
    FILE *remote_video_dump_file = nullptr;
    MultiEngineTestRunner *runner = nullptr;
};

class MultiEngineTestRunner : public TestingRunner {
public:
    MultiEngineTestRunner(AppOptions *options);
    virtual ~MultiEngineTestRunner();

    bool Start() override;
    void Stop() override;

protected:
    void OnJoinChannelResult(EngineContext *context, int result, const char *channel, const char *userId, int elapsed);
    
private:
    void TestingEntry();

    EngineContext* CreateEngineContext(AppConfig *app_config);
    void FreeEngineContext(EngineContext *context);

    int JoinChannel(EngineContext *context);
    void LeaveChannel(EngineContext *context);
    
    void CheckPushExternalStatus(EngineContext *context);

private:
    static int s_engine_id_;

    std::unique_ptr<tlog::Thread> thread_;
    std::vector<AppConfig> app_configs_;
    std::vector<EngineContext *> contexts_;

    friend class EngineCallback;
};

#endif // __MULTIENGINE_TEST_RUNNER_H__
