#include <gtest/gtest.h>

#include "rabbit/value.h"

namespace leaf = boost::leaf;

template <class T>
using result = leaf::result<T>;

template <typename E>
constexpr
    typename std::enable_if_t<std::is_enum_v<E>, std::underlying_type_t<E>>
    to_underlying(E e) noexcept
{
    return static_cast<typename std::underlying_type_t<E>>(e);
}

namespace test_utils
{
template <typename CallableT>
int execute(CallableT &&aArg) noexcept
{
    const auto r = leaf::try_handle_all(
        aArg,
        [](leaf::match<rabbit::value_error, rabbit::value_error::less_than_min>)
        { return to_underlying(rabbit::value_error::less_than_min); },
        [](leaf::match<rabbit::value_error, rabbit::value_error::more_than_max>)
        { return to_underlying(rabbit::value_error::more_than_max); },
        [](leaf::error_info const &unmatched)
        {
            std::cerr << "Unknown failure detected: " << unmatched << std::endl;
            return -1;
        });
    return r;
}
}  // namespace test_utils

template <typename MinT, typename MaxT, auto DefaultValue>
class ValueTest;

template <typename T, T MinV, T MaxV, T DefaultV>
class ValueTest<interval::Min<MinV>, interval::Max<MaxV>, DefaultV>
    : public ::testing::Test
{
   public:
    ValueTest() = default;
    using ValueT =
        rabbit::Value<interval::Min<MinV>, interval::Max<MaxV>, DefaultV>;

   protected:
    ValueT value_;
};

using TestValueInRange10To20 =
    ValueTest<interval::Min<10>, interval::Max<20>, 14>;
using ValueInRange10_20 =
    rabbit::Value<interval::Min<10>, interval::Max<20>, 14>;

#ifdef BOOST_LEAF_NO_EXCEPTIONS

namespace boost
{
[[noreturn]] void throw_exception(std::exception const &e)
{
    std::cerr
        << "Terminating due to a C++ exception under BOOST_LEAF_NO_EXCEPTIONS: "
        << e.what();
    std::terminate();
}

struct source_location;
[[noreturn]] void throw_exception(std::exception const &e,
                                  boost::source_location const &)
{
    throw_exception(e);
}
}  // namespace boost

#endif

TEST(ValueTest, MakeValidValue)
{
    result<ValueInRange10_20> r = rabbit::make_value<ValueInRange10_20>(15);
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r.value().get(), 15);
}

TEST(ValueTest, MakeInvalidValue1)
{
    const auto r = test_utils::execute(
        [&]() -> result<int>
        {
            result<ValueInRange10_20> value =
                rabbit::make_value<ValueInRange10_20>(5);
            return value.has_value() ? result<int>{0} : value.error();
        });
    ASSERT_EQ(r, to_underlying(rabbit::value_error::less_than_min));
}

TEST(ValueTest, MakeInvalidValue2)
{
    const auto r = test_utils::execute(
        [&]() -> result<int>
        {
            result<ValueInRange10_20> value =
                rabbit::make_value<ValueInRange10_20>(21);
            return value.has_value() ? result<int>{0} : value.error();
        });
    ASSERT_EQ(r, to_underlying(rabbit::value_error::more_than_max));
}

TEST_F(TestValueInRange10To20, SetValidValue)
{
    result<void> sr = value_.set(12);
    ASSERT_TRUE(static_cast<bool>(sr));
    ASSERT_EQ(value_.get(), 12);
}

TEST_F(TestValueInRange10To20, SetValueLessThanMin)
{
    const auto r = test_utils::execute(
        [&]() -> result<int>
        {
            result<void> sr = value_.set(9);
            return sr ? result<int>{0} : sr.error();
        });
    ASSERT_EQ(r, to_underlying(rabbit::value_error::less_than_min));
    ASSERT_EQ(value_.get(), ValueT::kDefault);
}

TEST_F(TestValueInRange10To20, SetValueMoreThanMax)
{
    const auto r = test_utils::execute(
        [&]() -> result<int>
        {
            result<void> sr = value_.set(25);
            return sr ? result<int>{0} : sr.error();
        });
    ASSERT_EQ(r, to_underlying(rabbit::value_error::more_than_max));
    ASSERT_EQ(value_.get(), ValueT::kDefault);
}

TEST_F(TestValueInRange10To20, MinMaxDefault)
{
    static_assert(ValueT::kMin == 10, "Invalid kMin");
    static_assert(ValueT::kMax == 20, "Invalid kMax");
    static_assert(ValueT::kDefault == 14, "Invalid kDefault");
}
