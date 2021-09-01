
#include "lasanga/utility/specialize.h"
#include "lasanga/utility/type_tree.h"

#include <iostream>
#include <string>

std::string constructed;

template<typename...>
struct type_list;

template<template<typename...> class>
struct wrapped_tt;

namespace alias
{
    struct A;
    struct B;
    struct C;
    struct D;
}   // namespace alias

template<typename A, typename B, typename C>
struct RootA
{
    RootA()
    {
        constructed += "RootA";
        constructed += ' ';
    }

    A a;
    B b;
    C c;
};

template<typename A, typename B>
struct RootB
{
    RootB()
    {
        constructed += "RootB";
        constructed += ' ';
    }

    A a;
    B b;
};

template<>
struct eld::util::get_alias_list<RootA>
{
    using type = type_list<alias::A, alias::B, alias::C>;
};

template<>
struct eld::util::get_alias_list<RootB>
{
    using type = type_list<alias::A, alias::B>;
};

template<typename... T>
struct GenericRoot : T...
{
};

template<const char L, size_t Index = 0>
struct Letter
{
    Letter() { constructed += L + std::to_string(Index) + ' '; }
};

template<typename AliasT, typename TypeT>
struct descriptor_t
{
    using name_tag = AliasT;
    using value_type = TypeT;
};

int main()
{
    using namespace eld::util;

    GenericRoot<Letter<'A'>, Letter<'B'>, Letter<'C'>, Letter<'D'>>();
    std::cout << constructed << std::endl;
    constructed.clear();

    specialize_tree<
        type_node<GenericRoot, Letter<'A'>, Letter<'B'>, Letter<'C'>, Letter<'D'>>>::type();
    std::cout << constructed << std::endl;
    constructed.clear();

    using nested_type = specialize_tree<
        type_node<GenericRoot,
                  type_node<GenericRoot,
                            Letter<'A', 1>,
                            type_node<GenericRoot, type_node<GenericRoot, Letter<'B', 1>>>>,
                  Letter<'A'>,
                  Letter<'B'>>>::type;

    nested_type();
    std::cout << constructed << std::endl;
    constructed.clear();

//    using descriptor_a = descriptor_t<alias::A, Letter<'A'>>;
//    using descriptor_b = descriptor_t<alias::B, Letter<'B'>>;
//    using descriptor_c = descriptor_t<alias::C, wrapped_tt<RootB>>;

//    using found_type =
//        typename detail::find_type_by_alias<alias::B, type_list<descriptor_a, descriptor_b>>::type;
//    static_assert(std::is_same_v<found_type, Letter<'B'>>);
//
//    specialize_tree<construct_tree_old<RootB, type_list<descriptor_a, descriptor_b>>::type>::type();
//    std::cout << constructed << std::endl;
//    constructed.clear();
//
//    using nested_type_2 = specialize_tree<
//        construct_tree_old<RootA, type_list<descriptor_a, descriptor_b, descriptor_c>>::type>::type;
//
//    [[maybe_unused]] nested_type_2 nestedType2{};
//
//    std::cout << constructed << std::endl;
//    constructed.clear();

    return 0;
}
