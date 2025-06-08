#include <mutex>

template <typename T>
class LockedPointer {
public:
    LockedPointer(T& resource, std::mutex& mutex) : resource(resource), guard(mutex) { }

    T* operator->() { return &resource; }
    T& operator*() { return resource; }

private:
    T& resource;
    std::lock_guard<std::mutex> guard;
};

template <typename T>
class LockedResource {
public:
    template<typename... Args>
    LockedResource(Args&&... args) : resource(std::forward<Args>(args)...) { }

    LockedPointer<T> borrow() { return LockedPointer<T>(resource, mutex); }

    // TODO: with_lock(std::function<void(T& resource)>)

private:
    T resource;
    mutable std::mutex mutex;
};