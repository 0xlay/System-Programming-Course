#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <cassert>
#include <algorithm>


namespace internal
{
    template <typename Handle>
    void CloseSmartHandle(Handle object)
    {
        static_assert(false, "Type isn't supported!");
    }

    template <>
    inline void CloseSmartHandle(HANDLE object)
    {
        CloseHandle(object);
    }

    template <>
    inline void CloseSmartHandle(SC_HANDLE object)
    {
        CloseServiceHandle(object);
    }

    template <>
    inline void CloseSmartHandle(HWND object)
    {
        CloseWindow(object);
    }

    template <>
    inline void CloseSmartHandle(HMODULE object)
    {
        FreeLibrary(object);
    }
}


template <class Handle>
class SmartHandle final
{
public:
    SmartHandle() noexcept;
    SmartHandle(Handle object) noexcept;
    ~SmartHandle() noexcept;

    SmartHandle(SmartHandle&& other) noexcept;
    SmartHandle& operator=(SmartHandle&& other) noexcept;

    SmartHandle(const SmartHandle&) = delete;
    SmartHandle& operator=(const SmartHandle&) = delete;

    operator Handle();

private:
    Handle object_;
};


template <class Handle>
SmartHandle<Handle>::SmartHandle() noexcept
    : object_(nullptr)
{ }


template <class Handle>
SmartHandle<Handle>::SmartHandle(Handle object) noexcept
    : object_(object)
{ }


template<class Handle>
SmartHandle<Handle>::SmartHandle(SmartHandle<Handle>&& other) noexcept
{
    object_ = other.object_;
    other.object_ = nullptr;
}


template <class Handle>
SmartHandle<Handle>::~SmartHandle() noexcept
{
    if (object_)
    {
        internal::CloseSmartHandle(object_);
    }
}


template <class Handle>
SmartHandle<Handle>& SmartHandle<Handle>::operator=(SmartHandle<Handle>&& other) noexcept
{
    if (this != &other)
    {
        std::swap(object_, other.object_);
    }
    return *this;
}


template <class Handle>
SmartHandle<Handle>::operator Handle()
{
    return object_;
}
