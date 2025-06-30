#pragma once
#include <concepts>
#include <functional>
#include <string>
#include <type_traits>
#include <utility>

namespace MEngine
{
template <typename T, typename Func>
concept GetterFunc = std::invocable<Func, const T &> && std::same_as<std::invoke_result_t<Func, const T &>, T>;

template <typename T, typename Func>
concept SetterFunc = std::invocable<Func, T &, const T &>;

template <typename T> inline constexpr auto DefaultGetter = [](const T &v) { return v; };
template <typename T> inline constexpr auto DefaultSetter = [](T &v, const T &value) { v = value; };

template <typename T> class Property
{
  private:
    T mValue;
    std::function<T(const T &)> mGetter;
    std::function<void(T &, const T &)> mSetter;

  public:
    Property() : mValue(T{}), mGetter(DefaultGetter<T>), mSetter(DefaultSetter<T>)
    {
    }
    template <typename TGetter = decltype(DefaultGetter<T>), typename TSetter = decltype(DefaultSetter<T>)>
        requires(GetterFunc<T, TGetter> && SetterFunc<T, TSetter> && !std::is_same_v<std::decay_t<TGetter>, T>)
    Property(const T &value, TGetter &&getter = {}, TSetter &&setter = {})
        : mGetter(std::forward<TGetter>(getter)), mSetter(std::forward<TSetter>(setter))
    {
        Set(value);
    }
    template <typename TGetter = decltype(DefaultGetter<T>), typename TSetter = decltype(DefaultSetter<T>)>
        requires(GetterFunc<T, TGetter> && SetterFunc<T, TSetter>)
    Property(T &&value, TGetter &&getter = {}, TSetter &&setter = {})
        : mGetter(std::forward<TGetter>(getter)), mSetter(std::forward<TSetter>(setter))
    {
        Set(std::move(value));
    }
    template <typename TGetter = decltype(DefaultGetter<T>), typename TSetter = decltype(DefaultSetter<T>)>
        requires GetterFunc<T, TGetter> && SetterFunc<T, TSetter>
    Property(TGetter &&getter, TSetter &&setter)
        : mValue(T{}), mGetter(std::forward<TGetter>(getter)), mSetter(std::forward<TSetter>(setter))
    {
    }
    Property(const Property &other) : mValue(other.mValue), mGetter(other.mGetter), mSetter(other.mSetter)
    {
    }
    Property(Property &&other) noexcept
        : mValue(std::move(other.mValue)), mGetter(std::move(other.mGetter)), mSetter(std::move(other.mSetter))
    {
        other.mValue = T{};
        other.mGetter = DefaultGetter<T>;
        other.mSetter = DefaultSetter<T>;
    }
    Property &operator=(const Property &other)
    {
        if (this != &other)
        {
            mValue = other.mValue;
            mGetter = other.mGetter;
            mSetter = other.mSetter;
        }
        return *this;
    }
    Property &operator=(Property &&other) noexcept
    {
        if (this != &other)
        {
            mValue = std::move(other.mValue);
            other.mValue = T{};
            mGetter = std::move(other.mGetter);
            other.mGetter = DefaultGetter<T>;
            mSetter = std::move(other.mSetter);
            other.mSetter = DefaultSetter<T>;
        }
        return *this;
    }
    ~Property() = default;
    T Get() const
    {
        return mGetter(mValue);
    }
    void Set(const T &value)
    {
        mSetter(mValue, value);
    }
    void Set(T &&value)
    {
        mSetter(mValue, std::move(value));
    }
    operator T() const
    {
        return Get();
    }
    Property &operator=(const T &value)
    {
        Set(value);
        return *this;
    }
    Property &operator=(T &&value)
    {
        Set(std::move(value));
        return *this;
    }
};
template <> class Property<std::string>
{
  private:
    std::string mValue;
    std::function<std::string(const std::string &)> mGetter;
    std::function<void(std::string &, const std::string &)> mSetter;

  public:
    Property() : mValue(std::string{}), mGetter(DefaultGetter<std::string>), mSetter(DefaultSetter<std::string>)
    {
    }
    template <typename TGetter = decltype(DefaultGetter<std::string>),
              typename TSetter = decltype(DefaultSetter<std::string>)>
        requires(GetterFunc<std::string, TGetter> && SetterFunc<std::string, TSetter> &&
                 !std::is_same_v<std::decay_t<TGetter>, std::string>)
    Property(std::string &value, TGetter &&getter = {}, TSetter &&setter = {})
        : mGetter(std::forward<TGetter>(getter)), mSetter(std::forward<TSetter>(setter))
    {
        Set(value);
    }
    template <typename TGetter = decltype(DefaultGetter<std::string>),
              typename TSetter = decltype(DefaultSetter<std::string>)>
        requires(GetterFunc<std::string, TGetter> && SetterFunc<std::string, TSetter>)
    Property(std::string &&value, TGetter &&getter = {}, TSetter &&setter = {})
        : mGetter(std::forward<TGetter>(getter)), mSetter(std::forward<TSetter>(setter))
    {
        Set(std::move(value));
    }
    template <typename TGetter = decltype(DefaultGetter<std::string>),
              typename TSetter = decltype(DefaultSetter<std::string>)>
        requires(GetterFunc<std::string, TGetter> && SetterFunc<std::string, TSetter>)
    Property(const char *value, TGetter &&getter = {}, TSetter &&setter = {})
        : mGetter(std::forward<TGetter>(getter)), mSetter(std::forward<TSetter>(setter))
    {
        Set(std::move(std::string(value)));
    }
    template <typename TGetter = decltype(DefaultGetter<std::string>)>
        requires GetterFunc<std::string, TGetter>
    Property(TGetter &&getter)
        : mValue(std::string{}), mGetter(std::forward<TGetter>(getter)), mSetter(DefaultSetter<std::string>)
    {
    }
    template <typename TSetter = decltype(DefaultSetter<std::string>)>
        requires SetterFunc<std::string, TSetter>
    Property(TSetter &&setter)
        : mValue(std::string{}), mGetter(DefaultGetter<std::string>), mSetter(std::forward<TSetter>(setter))
    {
    }
    template <typename TGetter = decltype(DefaultGetter<std::string>),
              typename TSetter = decltype(DefaultSetter<std::string>)>
        requires GetterFunc<std::string, TGetter> && SetterFunc<std::string, TSetter>
    Property(TGetter &&getter, TSetter &&setter)
        : mValue(std::string{}), mGetter(std::forward<TGetter>(getter)), mSetter(std::forward<TSetter>(setter))
    {
    }
    Property(const Property &other) : mValue(other.mValue), mGetter(other.mGetter), mSetter(other.mSetter)
    {
    }
    Property(Property &&other) noexcept
        : mValue(std::move(other.mValue)), mGetter(std::move(other.mGetter)), mSetter(std::move(other.mSetter))
    {
        other.mValue = std::string{};
        other.mGetter = DefaultGetter<std::string>;
        other.mSetter = DefaultSetter<std::string>;
    }
    Property &operator=(const Property &other)
    {
        if (this != &other)
        {
            mValue = other.mValue;
            mGetter = other.mGetter;
            mSetter = other.mSetter;
        }
        return *this;
    }
    Property &operator=(Property &&other) noexcept
    {
        if (this != &other)
        {
            mValue = std::move(other.mValue);
            other.mValue = std::string{};
            mGetter = std::move(other.mGetter);
            other.mGetter = DefaultGetter<std::string>;
            mSetter = std::move(other.mSetter);
            other.mSetter = DefaultSetter<std::string>;
        }
        return *this;
    }
    ~Property() = default;
    std::string Get() const
    {
        return mGetter(mValue);
    }
    void Set(const std::string &value)
    {
        mSetter(mValue, value);
    }
    void Set(std::string &&value)
    {
        mSetter(mValue, std::move(value));
    }
    void Set(const char *value)
    {
        mSetter(mValue, std::move(std::string(value)));
    }
    operator std::string() const
    {
        return Get();
    }
    Property &operator=(const std::string &value)
    {
        Set(value);
        return *this;
    }
    Property &operator=(std::string &&value)
    {
        Set(std::move(value));
        return *this;
    }
    Property &operator=(const char *value)
    {
        Set(value);
        return *this;
    }
};
using Int = Property<int>;
using Long = Property<long>;
using ULong = Property<unsigned long>;
using Float = Property<float>;
using Bool = Property<bool>;
using String = Property<std::string>;
using Char = Property<char>;
using UInt = Property<unsigned int>;
using UChar = Property<unsigned char>;
using Short = Property<short>;
using UShort = Property<unsigned short>;
using Int8 = Property<int8_t>;
using Int16 = Property<int16_t>;
using UInt8 = Property<uint8_t>;
using Uint32 = Property<uint32_t>;
using Uint64 = Property<uint64_t>;
using Int32 = Property<int32_t>;
using Int64 = Property<int64_t>;
using Double = Property<double>;
} // namespace MEngine