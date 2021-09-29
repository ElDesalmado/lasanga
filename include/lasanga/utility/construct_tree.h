#pragma once

/**
 * Type tree utility.
 * Type tree is an n-ary type tree with node = tree_node<TGenericClassT, Types...>.
 *
 * Type tree can be constructed from a type map, which is implemented as a list of
 * key,modifiers...:value pairs.
 *
 * Type tree traits: First argument is a template template parameter for the generic class. Type
 * tree class can also have a list of modifiers designated by a nested typename "modifiers".
 *
 *
 */

#include "traits.h"

#include <type_traits>
#include <utility>

namespace eld::util
{
    namespace detail
    {
        template <typename...>
        struct type_list;

        template<template<typename...> class>
        struct wrapped_generic_t;

        template<template<template<typename...> class, typename...> class TGetAliasListTT>
        struct wrapped_get_alias_list;

        template<typename WrappedAliasT, typename... Modifiers>
        struct wrapped_resolve_alias_type;

        template<template<typename...> class TGenericClassT, typename... SpecsT>
        struct tree_node;

        template<typename WrappedGenericClassTT,
                 typename ModifiersListT,
                 template<typename /*WrappedGenericClassTT*/, typename /*ModifiersListT*/>
                 class TGetAliasListTT,
                 template<typename /*WAliasT*/,
                          template<typename...>
                          class /*TGenericClassT*/,
                          typename... /*ModifiersT*/>
                 class TResolveAliasTypeTT,
                 typename AliasListT =
                     typename TGetAliasListTT<WrappedGenericClassTT, ModifiersListT>::type>
        struct construct_tree
        {
            using type = WrappedGenericClassTT;
        };

        template<template<typename...> class TGenericClassT,
                 typename... ModifiersT,
                 typename... AliasesT,
                 template<template<typename...> class>
                 class TWrapperT,
                 template<typename, typename>
                 class TGetAliasListTT,
                 template<typename /*WAliasT*/,
                          template<typename...>
                          class /*TGenericClassT*/,
                          typename... /*ModifiersT*/>
                 class TResolveAliasTypeTT>
        struct construct_tree<TWrapperT<TGenericClassT>,
                              detail::type_list<ModifiersT...>,
                              TGetAliasListTT,
                              TResolveAliasTypeTT,
                              detail::type_list<AliasesT...>>
        {
            template<typename XTWrappedAliasT>
            using resolve_type =
                typename TResolveAliasTypeTT<XTWrappedAliasT, TGenericClassT, ModifiersT...>::type;

            template<typename TWrappedGenericClassT>
            using construct_recursive = typename construct_tree<TWrappedGenericClassT,
                                                                detail::type_list<ModifiersT...>,
                                                                TGetAliasListTT,
                                                                TResolveAliasTypeTT>::type;
            using type =
                tree_node<TGenericClassT,
                          std::conditional_t<
                              traits::is_wrapped_generic_class_t<resolve_type<AliasesT>>::value,
                              construct_recursive<resolve_type<AliasesT>>,
                              resolve_type<AliasesT>>...>;
        };

        template<template<typename...> class>
        struct wrapped_tt;

    }   // namespace detail

    template<template<template<typename...> class /*TGenericClassT*/, typename... /*TModifiersT*/>
             class TGetAliasListTT,
             template<typename /*AliasT*/,
                      template<typename...>
                      class /*TGenericClassT*/,
                      typename... /*TModifiersT*/>
             class TResolveAliasTypeTT,
             template<typename...>
             class TGenericClassT,
             typename... ModifiersT>
    class construct_tree
    {
        template<typename, typename>
        struct wrapp_get_alias_list;

        template<template<typename...> class XTGenericClassT, typename... XModifiersT>
        struct wrapp_get_alias_list<detail::wrapped_tt<XTGenericClassT>,
                                    detail::type_list<XModifiersT...>>
        {
            using type = typename TGetAliasListTT<XTGenericClassT, XModifiersT...>::type;
        };

    public:
        using type = typename detail::construct_tree<detail::wrapped_tt<TGenericClassT>,
                                                     detail::type_list<ModifiersT...>,
                                                     wrapp_get_alias_list,
                                                     TResolveAliasTypeTT>::type;
    };

}   // namespace eld::generic
