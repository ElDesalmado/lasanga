
#include <lasanga/utility/resolve_factory.h>

#include <gtest/gtest.h>

#include <tuple>
#include <type_traits>

namespace stubs
{
    template<typename...>
    struct type_list;

    template<template<typename...> class>
    struct wrapped_tt;

    template<typename ValueTypeT, typename AliasTagT, typename TDependsOnT>
    struct designated_factory
    {
        using value_type = ValueTypeT;
        using alias_tag = AliasTagT;
        using depends_on_type = TDependsOnT;
    };

}   // namespace stubs

namespace nested
{
    namespace alias
    {
        template<typename...>
        struct A;
        struct B;
        struct C;
    }   // namespace alias

    template<typename...>
    struct generic
    {
    };
}   // namespace nested

// TODO: how to test expected failures?

TEST(resolve_factory, success)
{
    using factory_a = stubs::designated_factory<int,
                                                stubs::wrapped_tt<nested::alias::A>,
                                                stubs::wrapped_tt<nested::generic>>;
    using factory_b =
        stubs::designated_factory<int, nested::alias::B, stubs::wrapped_tt<nested::generic>>;
    using factory_c =
        stubs::designated_factory<int, nested::alias::C, stubs::wrapped_tt<nested::generic>>;

    using factories = std::tuple<factory_a, factory_b, factory_c>;

    using resolved_a = typename eld::util::resolve_factory<stubs::wrapped_tt<nested::alias::A>,
                                                           stubs::wrapped_tt<nested::generic>,
                                                           factories,
                                                           stubs::type_list<>>::type;

    constexpr bool value_a =
        std::is_same_v<typename factory_a::value_type, typename resolved_a::value_type>;
    static_assert(value_a, "Failed to resolve A");
    EXPECT_TRUE(value_a);

    using resolved_b = typename eld::util::resolve_factory<nested::alias::B,
                                                           stubs::wrapped_tt<nested::generic>,
                                                           factories,
                                                           stubs::type_list<>>::type;

    constexpr bool value_b =
        std::is_same_v<typename factory_b::value_type, typename resolved_b::value_type>;
    static_assert(value_b, "Failed to resolve B");
    EXPECT_TRUE(value_b);

    using resolved_c = typename eld::util::resolve_factory<nested::alias::C,
                                                           stubs::wrapped_tt<nested::generic>,
                                                           factories,
                                                           stubs::type_list<>>::type;

    constexpr bool value_c =
        std::is_same_v<typename factory_c::value_type, typename resolved_c::value_type>;
    static_assert(value_c, "Failed to resolve C");
    EXPECT_TRUE(value_c);
}

namespace alias
{
    struct A;
    struct B;
    struct C;
}   // namespace alias

// TODO: make work
template<typename... /*typename AT, typename BT, typename CT*/>
struct layer
{
};

TEST(resolve_factory, success2)
{
    using factory_a = stubs::designated_factory<int, alias::A, stubs::wrapped_tt<layer>>;
    using factory_b = stubs::designated_factory<int, alias::B, stubs::wrapped_tt<layer>>;
    using factory_c = stubs::designated_factory<int, alias::C, stubs::wrapped_tt<layer>>;

    using factories = std::tuple<factory_a, factory_b, factory_c>;

    using resolved_a = typename eld::util::
        resolve_factory<alias::A, stubs::wrapped_tt<layer>, factories, stubs::type_list<>>::type;

    constexpr bool value_a =
        std::is_same_v<typename factory_a::value_type, typename resolved_a::value_type>;
    static_assert(value_a, "Failed to resolve A");
    EXPECT_TRUE(value_a);

    using resolved_b = typename eld::util::
        resolve_factory<alias::B, stubs::wrapped_tt<layer>, factories, stubs::type_list<>>::type;

    constexpr bool value_b =
        std::is_same_v<typename factory_b::value_type, typename resolved_b::value_type>;
    static_assert(value_b, "Failed to resolve B");
    EXPECT_TRUE(value_b);

    using resolved_c = typename eld::util::
        resolve_factory<alias::C, stubs::wrapped_tt<layer>, factories, stubs::type_list<>>::type;

    constexpr bool value_c =
        std::is_same_v<typename factory_c::value_type, typename resolved_c::value_type>;
    static_assert(value_c, "Failed to resolve C");
    EXPECT_TRUE(value_c);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
