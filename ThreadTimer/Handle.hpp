#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <algorithm>


namespace internal
{

    template <class T>
    concept WinAPIHandle = requires(T a)
    {
        std::is_same_v<T, HANDLE>;
        std::is_same_v<T, SC_HANDLE>;
        std::is_same_v<T, HWND>;
        std::is_same_v<T, HMODULE>;
    };

    template<class THandle>
    requires WinAPIHandle<THandle>
    void CloseSmartHandle(THandle object)
    {
        if constexpr (std::is_same_v<THandle, HANDLE>)
        {
            CloseHandle(object);
        }
        else if constexpr (std::is_same_v<THandle, SC_HANDLE>)
        {
            CloseServiceHandle(object);
        }
        else if constexpr (std::is_same_v<THandle, HWND>)
        {
            CloseWindow(object);
        }
        else if constexpr (std::is_same_v<THandle, HMODULE>)
        {
            FreeLibrary(object);
        }
    }

} // internal


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