#pragma once

#include "traits.h"

#include <type_traits>
#include <utility>

namespace eld::util
{
    /**
     * Fully specialize TGenericClassT class template given a Type tree with TTNodeTypeT root
     * node. Helper generic class template specialization to allow recursion.
     * @tparam NonTemplateClass
     */
    template<typename NonTemplateClass>
    struct specialize_tree
    {
        using type = NonTemplateClass;
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
        using type = TGenericClassT<std::conditional_t<traits::is_type_tree<SpecTypesT>::value,
                                                       typename specialize_tree<SpecTypesT>::type,
                                                       SpecTypesT>...>;
    };

    template <typename TGenericClassT>
    using specialize_generic_class = specialize_tree<TGenericClassT>;
}   // namespace eld::util
