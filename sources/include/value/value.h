#ifndef rabbit_value_h
#define rabbit_value_h

#include <interval/interval.h>

#include <boost/leaf.hpp>

namespace rabbit
{
namespace leaf = boost::leaf;

template <class T>
using result = leaf::result<T>;

enum class value_error
{
    less_than_min = 1,
    more_than_max
};

template <typename MinT, typename MaxT, auto DefaultValue>
class Value;

template <typename T>
result<T> make_value(typename T::value_type aValue) noexcept;

template <typename T, T MinV, T MaxV, T DefaultV>
class Value<interval::Min<MinV>, interval::Max<MaxV>, DefaultV>
{
   public:
    using ValueT = Value<interval::Min<MinV>, interval::Max<MaxV>, DefaultV>;
    using value_type = T;
    using interval_type =
        interval::Interval<interval::Min<MinV>, interval::Max<MaxV>>;

   private:
    constexpr static T defaultValue() noexcept
    {
        static_assert(DefaultV >= MinV, "DefaultV must be not less than MinV.");
        static_assert(DefaultV <= MaxV, "DefaultV must be not more than MaxV.");
        return DefaultV;
    }

   public:
    static constexpr T kDefault = defaultValue();
    static constexpr T kMin = MinV;
    static constexpr T kMax = MaxV;

    static result<void> validate(value_type aValue) noexcept
    {
        const auto kLocation = interval_type::location(aValue);

        if (kLocation == interval::eIntervalLocation::kInside)
        {
            return {};
        }

        if (kLocation == interval::eIntervalLocation::kBelow)
        {
            return leaf::new_error(value_error::less_than_min);
        }

        return leaf::new_error(value_error::more_than_max);
    }

    friend result<ValueT> make_value<>(value_type aValue) noexcept
    {
        BOOST_LEAF_CHECK(validate(aValue));
        return Value(aValue);
    }

    Value() = default;

    result<void> set(value_type aValue) noexcept
    {
        BOOST_LEAF_CHECK(validate(aValue));
        value_ = aValue;
        return {};
    }

    constexpr value_type get() const noexcept { return value_; }

   private:
    constexpr Value(value_type aValue) noexcept : value_(aValue) {}
    value_type value_{kDefault};
};
}  // namespace rabbit

#endif /* rabbit_value_h */
