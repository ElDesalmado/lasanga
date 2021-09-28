
#include "lasanga/generic/construct_tree.h"
#include "lasanga/generic/specialize_tree.h"

#include "lasanga/generic/traits.h"
#include "lasanga/lasanga.h"

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
struct eld::get_alias_list<RootA>
{
    using type = type_list<alias::A, alias::B, alias::C>;
};

template<>
struct eld::get_alias_list<RootB>
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

template<template<typename...> class, typename...>
struct type_node;

int main()
{
    using namespace eld::util;

    GenericRoot<Letter<'A'>, Letter<'B'>, Letter<'C'>, Letter<'D'>>();
    std::cout << constructed << std::endl;
    constructed.clear();

    eld::generic::specialize_tree<
        type_node<GenericRoot, Letter<'A'>, Letter<'B'>, Letter<'C'>, Letter<'D'>>>::type();
    std::cout << constructed << std::endl;
    constructed.clear();

    using nested_type = eld::generic::specialize_tree<
        type_node<GenericRoot,
                  type_node<GenericRoot,
                            Letter<'A', 1>,
                            type_node<GenericRoot, type_node<GenericRoot, Letter<'B', 1>>>>,
                  Letter<'A'>,
                  Letter<'B'>>>::type;

    nested_type();
    std::cout << constructed << std::endl;
    constructed.clear();

    return 0;
}
