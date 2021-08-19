
#include <lasanga/utility/apply_predicates.h>

#include <gtest/gtest.h>

#include <type_traits>

namespace stub
{
    template<template<typename...> class Predicate, typename... Modifiers>
    struct predicate_wrapper;
}

TEST(apply_predicate, is_same)
{
    namespace util = eld::util;

    using int_t = int;
    constexpr auto res = util::apply_predicate<stub::predicate_wrapper<std::is_same, int>, int_t>();
    EXPECT_TRUE(res);
}

TEST(apply_predicate, is_constructible)
{
    namespace util = eld::util;

    struct foo
    {
        foo(int, double) {}
    };

    constexpr auto resTrue =
        util::apply_predicate<stub::predicate_wrapper<std::is_constructible, int, double>, foo>();
    EXPECT_TRUE(resTrue);

    constexpr auto resFalse =
        util::apply_predicate<stub::predicate_wrapper<std::is_constructible, void>, foo>();
    EXPECT_FALSE(resFalse);

    constexpr auto resFalse2 =
        util::apply_predicate<stub::predicate_wrapper<std::is_constructible>, foo>();
    EXPECT_FALSE(resFalse2);
}



int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
