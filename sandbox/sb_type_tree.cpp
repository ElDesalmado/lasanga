
#include "lasanga/utility/type_tree.h"

#include <iostream>
#include <string>

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

std::string constructed;

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

    using descriptor_a = descriptor_t<alias::A, Letter<'A'>>;
    using descriptor_b = descriptor_t<alias::B, Letter<'B'>>;
    using descriptor_generic = descriptor_t<alias::C, wrapped_tt<GenericRoot>>;
//    using descriptor_d = descriptor_t<alias::D,


    using found_type =
        typename detail::find_type_by_alias<alias::B, type_list<descriptor_a, descriptor_b>>::type;
    static_assert(std::is_same_v<found_type, Letter<'B'>>);

    using constructed_t =
        typename detail::construct_tree<type_node,
                                        wrapped_tt<GenericRoot>,
                                        type_list<alias::A, alias::B>,
                                        type_list<descriptor_a, descriptor_b>>::type;
    specialize_tree<constructed_t>::type();
    std::cout << constructed << std::endl;
    constructed.clear();

    return 0;
}
