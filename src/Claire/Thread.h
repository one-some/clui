#pragma once

#include <pthread.h>
#include <functional>

#include "Claire/Assert.h"

inline void* _thread_trampoline(void* std_func) {
    auto* func = (std::function<void()>*)std_func;
    (*func)();

    delete func;
    return nullptr;
}

class Thread {
public:
    template<typename F>
    Thread(F&& target) {
        auto* func = new std::function<void()>(std::forward<F>(target));
        this->target = func;
    }

    ~Thread() {
        ASSERT(!joinable, "Woah please stop before dying");
    }

    Thread(const Thread&) = delete;
    Thread& operator=(const Thread&) = delete;

    void start() {
        ASSERT(!joinable, "Man we're busy here");

        int ret = pthread_create(&thread, nullptr, _thread_trampoline, target);
        ASSERT(ret == 0, "Error creating thread");

        joinable = true;
    }

    void join() {
        ASSERT(joinable, "Okay how do you plan on joining a dead thread");
        int ret = pthread_join(thread, nullptr);
        ASSERT(ret == 0, "Error joining thread");
        joinable = false;
    }

    void daemonify() {
        ASSERT(joinable, "Thread not joinable");
        int ret = pthread_detach(thread);
        ASSERT(ret == 0, "Error detaching thread");
        joinable = false;
    }

    

private:
    std::function<void()>* target;
    pthread_t thread;

    bool joinable = false;
};