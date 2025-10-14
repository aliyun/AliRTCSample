#ifndef TLOG_THREAD_H_
#define TLOG_THREAD_H_

#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

namespace tlog {

using Task = std::function<void(void)>;

class Thread {
public:
    Thread();
    ~Thread();

    // Set the thread name for debugging. Must be called before Start().
    // If `obj` is non-null, its value is appended to `name`.
    bool SetName(const std::string& name, const void* obj = nullptr);
    const std::string& name() const { return name_; }

    // Start the  execution of the thread.
    bool Start();

    // Tell the thread to stop and wait until it is joined.
    // Never call Stop on the current thread, instead use Quit.
    void Stop();

    // Tell the thread to exit without terminating the underlying OS thread.
    void Quit();
    void Restart();

    // Blocks the calling thread until this thread has terminated.
    void Join();

    bool IsCurrent() const;

    // Convenience method to invoke a functor on another thread.
    // Blocks the current thread until execution is complete.
    void SendTask(Task&& task);

    template <typename Functor,
              typename ReturnT = decltype(std::declval<Functor>()()),
              typename = typename std::enable_if<!std::is_void<ReturnT>::value>::type>
    ReturnT SendTask(Functor&& functor) {
        ReturnT result;
        SendTask([&] { result = std::forward<Functor>(functor)(); });
        return result;
    }

    // Schedules a `task` to execute. Tasks are executed in FIFO order.
    void PostTask(Task&& task);
    // Schedules a `task` to execute with a specified `delay`.
    void PostDelayedTask(Task&& task, int delay_ms);


protected:
    void Run();

    // Return true if th thread is currently running.
    bool IsRunning();

    bool IsQuitting();

    // ProcessMessages will process I/O and dispatch messages until:
    //  1) cms milliseconds have elapsed (returns true)
    //  2) Stop() is called (returns false)
    bool ProcessMessages(int cms);

    // Get() will process I/O until:
    //  1) A task is available (returns it)
    //  2) cmsWait seconds have elapsed (returns empty task)
    //  3) Stop() is called (returns empty task)
    Task Get(int cmsWait);
    void Dispatch(Task&& task);

    void SetDispatchWarningMs(int deadline);

    void WakeUp();

private:
    struct DelayedMessage {
        bool operator<(const DelayedMessage& dmsg) const {
            return (dmsg.run_time_ms < run_time_ms) ||
                   ((dmsg.run_time_ms == run_time_ms) && (dmsg.message_number < message_number));
        }

        int64_t delay_ms;
        int64_t run_time_ms;
        uint32_t message_number;
        Task functor;
    };

    std::string name_;

    std::mutex msg_lock_;
    std::mutex event_lock_;
    std::condition_variable event_cond_;

    std::atomic<int> stop_;

    std::unique_ptr<std::thread> thread_;

    std::queue<Task> messages_;
    std::priority_queue<DelayedMessage> delayed_messages_;
    uint32_t delayed_next_num_;

    static const int kSlowDispatchLoggingThreshold = 50;  // 50 ms
    int dispatch_warning_ms_ = kSlowDispatchLoggingThreshold;
};

} // namespace tlog

#endif // TLOG_THREAD_H_
