
#include "lasanga/utility/filter_type_list.h"

#include <gtest/gtest.h>

template<template<typename...> class, typename...>
struct wrapped_tt;

template<typename...>
struct type_list;

TEST(filter_type_list, is_integral)
{
    using fundamental_types = type_list<bool, float, double, int, unsigned long>;

    using result_list =
        typename eld::util::filter_type_list_conjunction<fundamental_types, std::is_integral>::type;

    constexpr bool res = std::is_same_v<result_list, type_list<bool, int, unsigned long>>;

    EXPECT_TRUE(res);
}

TEST(filter_type_list, is_signed_integral)
{
    using fundamental_types = type_list<bool, float, double, int, unsigned long>;

    using result_list = typename eld::util::
        filter_type_list_conjunction<fundamental_types, std::is_integral, std::is_signed>::type;

    constexpr bool resTrue = std::is_same_v<result_list, type_list<int>>;
    static_assert(resTrue);
    EXPECT_TRUE(resTrue);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
