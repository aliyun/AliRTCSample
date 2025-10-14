#include "thread.h"

#include <stdio.h>
#include <inttypes.h>

#include "utils.h"
#include "tlog.h"

namespace tlog {

static const int kForever = -1;

// ---------- Thread ------------
Thread::Thread()
    : stop_(0)
    , delayed_next_num_(0) {
    SetName("Thread", this); // default name
}

Thread::~Thread() {
    Stop();
}

bool Thread::SetName(const std::string& name, const void* obj) {
    if (IsRunning()) {
        TLOGE("Must SetName before the thread Start.\n");
        return false;
    }

    name_ = std::string(name);
    if (obj) {
        // The %p specifier typically produce at most 16 hex digits, possibly with a
        // 0x prefix. But format is implementation defined, so add some margin.
        char buf[30];
        snprintf(buf, sizeof(buf), "-%p", obj);
        name_ += buf;
    }
    return true;
}

bool Thread::Start() {
    if (IsRunning()) {
        TLOGV("Ignore call Start, the thread is already running.");
        return false;
    }

    Restart();
    thread_.reset(new std::thread(&Thread::Run, this));
    return true;
}

void Thread::Stop() {
    Quit();
    Join();
}

void Thread::Quit() {
    stop_.store(1, std::memory_order_release);
    WakeUp();
}

void Thread::Join() {
    if (thread_ && thread_->joinable()) {
        thread_->join();
    }
    thread_.reset(nullptr);
}

bool Thread::IsCurrent() const {
    return thread_ && thread_->get_id() == std::this_thread::get_id();
}

bool Thread::IsRunning() {
    return thread_ != nullptr;
}

bool Thread::IsQuitting() {
    return stop_.load(std::memory_order_acquire) != 0;
}

void Thread::Restart() {
    stop_.store(0, std::memory_order_release);
}

void Thread::Run() {
    // TODO: set thread name?
    ProcessMessages(kForever);
}

void Thread::WakeUp() {
    std::unique_lock<std::mutex> lk(event_lock_);
    event_cond_.notify_all();
}

bool Thread::ProcessMessages(int cmsLoop) {
    int64_t msEnd = (kForever == cmsLoop) ? 0 : TimeAfter(cmsLoop);
    int cmsNext = cmsLoop;

    while (true) {
        Task task = Get(cmsNext);
        if (!task)
            return !IsQuitting();
        Dispatch(std::move(task));

        if (cmsLoop != kForever) {
            cmsNext = static_cast<int>(TimeUntil(msEnd));
            if (cmsNext < 0)
                return true;
        }
    }
}

Task Thread::Get(int cmsWait) {
    int64_t cmsTotal = cmsWait;
    int64_t cmsElapsed = 0;
    int64_t msStart = TimeMillis();
    int64_t msCurrent = msStart;
    
    while (true) {
        // Check for posted events
        int64_t cmsDelayNext = kForever;
        {
            // All queue operations need to be locked, but nothing else in this loop
            // can happen while holding the `mutex_`.
            std::lock_guard<std::mutex> lock(msg_lock_);
            // Check for delayed messages that have been triggered and calculate the
            // next trigger time.
            while (!delayed_messages_.empty()) {
                if (msCurrent < delayed_messages_.top().run_time_ms) {
                    cmsDelayNext = TimeDiff(delayed_messages_.top().run_time_ms, msCurrent);
                    break;
                }
                messages_.push(std::move(delayed_messages_.top().functor));
                delayed_messages_.pop();
            }
            // Pull a message off the message queue, if available.
            if (!messages_.empty()) {
                Task task = std::move(messages_.front());
                messages_.pop();
                return task;
            }
        }

        if (IsQuitting()) {
            printf("%s is quitting\n", name_.c_str());
            break;
        }

        // Which is shorter, the delay wait or the asked wait?

        int64_t cmsNext;
        if (cmsWait == kForever) {
            cmsNext = cmsDelayNext;
        } else {
            cmsNext = std::max<int64_t>(0, cmsTotal - cmsElapsed);
            if ((cmsDelayNext != kForever) && (cmsDelayNext < cmsNext)) cmsNext = cmsDelayNext;
        }

        {
            std::unique_lock<std::mutex> lk(event_lock_);
            if (cmsNext == kForever) {
                event_cond_.wait(lk);
            } else {
                event_cond_.wait_for(lk, std::chrono::milliseconds(cmsNext));
            }
        }

        // If the specified timeout expired, return

        msCurrent = TimeMillis();
        cmsElapsed = TimeDiff(msCurrent, msStart);
        if (cmsWait != kForever) {
            if (cmsElapsed >= cmsWait) {
                return nullptr;
            }
        }
    }
    return nullptr;
}

void Thread::Dispatch(Task&& task) {
    //   MT_DCHECK_RUN_ON(this);
    int64_t start_time = TimeMillis();
    std::move(task)();
    int64_t end_time = TimeMillis();
    int64_t diff = TimeDiff(end_time, start_time);
    if (diff >= dispatch_warning_ms_) {
        fprintf(stderr, "Message to %s took %" PRId64 " ms to dispatch.\n", name().c_str(), diff);
        // To avoid log spew, move the warning limit to only give warning
        // for delays that are larger than the one observed.
        dispatch_warning_ms_ = diff + 1;
    }
}

void Thread::SetDispatchWarningMs(int deadline) {
    if (!IsCurrent()) {
        PostTask([this, deadline]() { SetDispatchWarningMs(deadline); });
        return;
    }
    // MT_DCHECK_RUN_ON(this);
    dispatch_warning_ms_ = deadline;
}

void Thread::SendTask(Task&& task) {
    if (IsQuitting())
        return;

    if (IsCurrent()) {
        task();
        return;
    }

    std::mutex m;
    std::condition_variable cv;
    bool completed = false;
    auto task_sync = [&] {
        task();
        {
            std::unique_lock<std::mutex> lk(m);
            completed = true;
            cv.notify_one();
        }
    };

    PostTask(std::move(task_sync));

    {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [&completed] { return completed; });
    }
}

void Thread::PostTask(Task&& task) {
    if (IsQuitting()) {
        return;
    }

    // Keep thread safe
    // Add the message to the end of the queue
    // Signal for the multiplexer to return

    {
        std::lock_guard<std::mutex> lock(msg_lock_);
        messages_.push(std::move(task));
    }
    WakeUp();
}

void Thread::PostDelayedTask(Task&& task, int delay_ms) {
    if (IsQuitting()) {
        return;
    }

    // Keep thread safe
    // Add to the priority queue. Gets sorted soonest first.
    // Signal for the multiplexer to return.

    int64_t run_time_ms = TimeAfter(delay_ms);
    {
        std::lock_guard<std::mutex> lock(msg_lock_);
        delayed_messages_.push({.delay_ms = delay_ms,
                                .run_time_ms = run_time_ms,
                                .message_number = delayed_next_num_,
                                .functor = std::move(task)});
        // If this message queue processes 1 message every millisecond for 50 days,
        // we will wrap this number.  Even then, only messages with identical times
        // will be misordered, and then only briefly.  This is probably ok.
        ++delayed_next_num_;
        // MT_DCHECK_NE(0, delayed_next_num_);
    }
    WakeUp();
}



} // namespace tlog