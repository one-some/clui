// TODO: Destruct
#pragma once
#include "Claire/Assert.h"

template<typename T>
class Optional {
public:
    Optional() { }

    template <typename... Args>
    Optional(std::in_place_t, Args&&... args) {
        construct(std::forward<Args>(args)...);
    }

    Optional(T& value) {
        construct(value);
    }

    Optional(T&& value) {
        construct(std::move(value));
    }

    T* get_ptr() {
        ASSERT(m_full, "Not full");
        return (T*)&m_storage;
    }

    T* operator->() {
        return get_ptr();
    }

    T& operator*() {
        return *get_ptr();
    }

    bool full() { return m_full; }
    operator bool() { return m_full; }

private:
    alignas(alignof(T)) char m_storage[sizeof(T)];
    bool m_full = false;

    template<typename... Args>
    void construct(Args&&... args) {
        new (&m_storage) T(std::forward<Args>(args)...);
        m_full = true;
    }
};