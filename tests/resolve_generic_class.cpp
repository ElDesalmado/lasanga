
#include <lasanga/impl/resolve_generic_class.h>

#include <iostream>

template <template <typename...> class>
struct wrapped_tt;

template <typename...>
struct type_list;

template <template <typename...> class>
struct get_alias_list;

namespace alias
{
    template <typename...>
    struct A;

    template <typename...>
    struct B;
}

template <typename AT, typename BT>
struct Layer0
{
    AT a;
    BT b;

    void print()
    {
        std::cout << "Layer0\n";
        a.print();
        b.print();
    }
};

template <>
struct get_alias_list<Layer0>
{
    using type = type_list<wrapped_tt<alias::A>,
        wrapped_tt<alias::B>>;
};

template <typename AT>
struct Layer1
{
    AT a;

    void print()
    {
        std::cout << "Layer1\n";
        a.print();
    }
};

template <>
struct get_alias_list<Layer1>
{
    using type = type_list<wrapped_tt<alias::A>>;
};

template <const char C>
struct dummy
{
    constexpr static const char value = C;

    void print()
    {
        std::cout << C << '\n';
    }
};

template <typename AliasT, template <typename...> class>
struct resolve_alias_type;

template <>
struct resolve_alias_type<wrapped_tt<alias::B>,
    Layer0>
{
    using type = dummy<'B'>;
};

template <>
struct resolve_alias_type<wrapped_tt<alias::A>,
                          Layer0>
{
//     using type = dummy<'A'>;
    using type = wrapped_tt<Layer1>;
};

template <>
struct resolve_alias_type<wrapped_tt<alias::A>,
                          Layer1>
{
     using type = dummy<'A'>;
};

#include <gtest/gtest.h>

TEST(resolve_generic_class, 2_layers)
{
    // TODO: check resolved tree output
    using resolved_type = typename eld::impl::resolve_generic_class<Layer0,
          get_alias_list,
          resolve_alias_type>::type;

    resolved_type().print();
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
