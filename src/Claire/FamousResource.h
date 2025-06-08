#pragma once
#include <mutex>

namespace FamousLocking {
    struct None {
        void lock()    { }
        void unlock()  { }
    };

    struct Mutex {
        std::mutex mutex;
        void lock()    { mutex.lock(); }
        void unlock()  { mutex.unlock(); }
    };
}

// Remember to manually notify for anything in place. Or die
template <typename T, typename LockingPolicy = FamousLocking::None>
class FamousResource {
public:
    template<typename... Args>
    FamousResource(Args&&... args)
        : resource(std::make_shared<T>(std::forward<Args>(args)...)) { }

    std::shared_ptr<const T> get() const {
        std::lock_guard<LockingPolicy> guard(locking_policy);
        return resource;
    }

    template<typename Func>
    void perform_on(Func&& operation) {
        // TODO: Modifier proxy?
        // TODO: Split notification and operation lock critical sections?

        auto indiana_jones_idol_swap = std::make_shared<T>(*get());
        // Do the operation out of the critical section because we're baller
        operation(*indiana_jones_idol_swap);

        std::vector<std::function<void(std::shared_ptr<const T>)>> safe_subscribers;
        std::shared_ptr<const T> snapshot;

        {
            std::lock_guard<LockingPolicy> guard(locking_policy);
            resource = indiana_jones_idol_swap;
            safe_subscribers = subscribers;
            snapshot = resource;
        }

        for (auto& callback : safe_subscribers) {
            callback(snapshot);
        }
    }

    void subscribe(std::function<void(std::shared_ptr<const T>)> callback) {
        std::shared_ptr<const T> snapshot;

        {
            std::lock_guard<LockingPolicy> guard(locking_policy);
            subscribers.push_back(callback);
            snapshot = resource;
        }

        callback(snapshot);
    }

private:
    std::shared_ptr<T> resource;
    mutable LockingPolicy locking_policy;
    std::vector<std::function<void(std::shared_ptr<const T>)>> subscribers;
};