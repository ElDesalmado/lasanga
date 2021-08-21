
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
    using namespace eld::util;

    using int_t = int;
    constexpr auto res = apply_predicate<int_t, stub::predicate_wrapper<std::is_same, int>>();
    EXPECT_TRUE(res);
}

TEST(apply_predicate, is_constructible)
{
    using namespace eld::util;

    struct foo
    {
        foo(int, double) {}
    };

    constexpr auto resTrue =
        apply_predicate<foo, stub::predicate_wrapper<std::is_constructible, int, double>>();
    EXPECT_TRUE(resTrue);

    constexpr auto resFalse =
        apply_predicate<foo, stub::predicate_wrapper<std::is_constructible, void>>();
    EXPECT_FALSE(resFalse);

    constexpr auto resFalse2 =
        apply_predicate<foo, stub::predicate_wrapper<std::is_constructible>>();
    EXPECT_FALSE(resFalse2);
}

TEST(apply_predicates_conjunction, same_and_integral)
{
    using namespace eld::util;
    struct foo
    {
        foo(int, double) {}
    };

    using int_t = int;

    constexpr auto resTrue =
        apply_predicates_conjunction<int_t,
                                     stub::predicate_wrapper<std::is_same, int>,
                                     stub::predicate_wrapper<std::is_integral>>();
    EXPECT_TRUE(resTrue);

    constexpr auto resFalse =
        apply_predicates_conjunction<foo,
                                     stub::predicate_wrapper<std::is_same, foo>,
                                     stub::predicate_wrapper<std::is_integral>>();
    EXPECT_FALSE(resFalse);
}

TEST(apply_predicates_conjunction, is_integral_signed)
{
    using namespace eld::util;

    using int_t = int;
    constexpr bool resTrue =
        apply_predicates_conjunction_v<int_t,
                                       stub::predicate_wrapper<std::is_integral>,
                                       stub::predicate_wrapper<std::is_signed>>;

    EXPECT_TRUE(resTrue);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
