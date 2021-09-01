#pragma once

#include "lasanga/utility/traits.h"

#include <type_traits>
#include <utility>

namespace eld::util
{
    /**
     * Fully specialize TGenericClassT class template given a Type tree with TTNodeTypeT root
     * node. Helper generic class template specialization to allow recursion.
     * @tparam TNodeTypeT
     */
    template<typename TNodeTypeT>
    struct specialize_tree
    {
        using type = TNodeTypeT;
    };

    /**
     * Fully specialize TGenericClassT class template given a Type tree with TTNodeTypeT root node.
     * @tparam TTNodeTypeT Root node type.
     * @tparam TGenericClassT Generic class of a root node
     * @tparam SpecTypesT List of types to specialize TGenericClassT with.
     */
    template<template<template<typename...> class, typename...> class TTNodeTypeT,
             template<typename...>
             class TGenericClassT,
             typename... SpecTypesT>
    struct specialize_tree<TTNodeTypeT<TGenericClassT, SpecTypesT...>>
    {
        using type = TGenericClassT<std::conditional_t<eld::traits::is_type_tree<SpecTypesT>::value,
                                                       typename specialize_tree<SpecTypesT>::type,
                                                       SpecTypesT>...>;
    };
}   // namespace eld::util
