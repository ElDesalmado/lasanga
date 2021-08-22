#pragma once

/**
 * Type tree utility.
 * Type tree is a type_tree<GenericClass, Types...>.
 *
 * Type tree traits: First argument is a template template parameter for the generic class. Type
 * tree class can also have a list of modifiers designated by a nested typename "modifiers".
 *
 *
 */

#include <type_traits>
#include <utility>

namespace eld
{
    namespace traits
    {
        template<typename>
        struct is_type_tree : std::false_type
        {
        };

        template<template<template<typename...> class, typename...> class TTNodeTypeT,
                 template<typename...>
                 class TGenericClassT,
                 typename... SpecT>
        struct is_type_tree<TTNodeTypeT<TGenericClassT, SpecT...>> : std::true_type
        {
        };

    }   // namespace traits

    namespace util
    {
        template<template<typename...> class TGenericClassT, typename... SpecTypesT>
        struct type_node
        {
        };

        template<typename TNodeTypeT>
        struct specialize_tree
        {
            using type = TNodeTypeT;
        };

        template<template<template<typename...> class, typename...> class TTNodeTypeT,
                 template<typename...>
                 class TGenericClassT,
                 typename... SpecTypesT>
        struct specialize_tree<TTNodeTypeT<TGenericClassT, SpecTypesT...>>
        {
            using type =
                TGenericClassT<std::conditional_t<traits::is_type_tree<SpecTypesT>::value,
                                                  typename specialize_tree<SpecTypesT>::type,
                                                  SpecTypesT>...>;
        };
    }   // namespace util
}   // namespace eld
