#pragma once

/**
 * This is a default implementation to resolve_generic_class
 */

namespace eld::impl
{
    namespace detail
    {
        template<template<typename...> class>
        struct wrapped_tt;

        template<template<typename...> class TGenericT, typename ListT>
        struct apply_template_arg_list;

        template<template<typename...> class TGenericT,
                 template<typename...>
                 class TListT,
                 typename... ArgsT>
        struct apply_template_arg_list<TGenericT, TListT<ArgsT...>>
        {
            using type = TGenericT<ArgsT...>;
        };

        template<typename...>
        struct type_list;

        struct alias_dummy;

        template<template<typename...> class TGenericClassT,
                 typename AliasListT,
                 template<typename, template<typename...> class>
                 class TResolveAliasTypeTT>
        struct resolve_alias_list;

        template<template<typename...> class TGenericClassT,
                 typename... AliasT,
                 template<typename...>
                 class AliasListT,
                 template<typename, template<typename...> class>
                 class TResolveAliasTypeTT>
        struct resolve_alias_list<TGenericClassT, AliasListT<AliasT...>, TResolveAliasTypeTT>
        {
            using type = type_list<typename TResolveAliasTypeTT<AliasT, TGenericClassT>::type...>;
        };

        template<typename ResolvedT,
                 template<template<typename...> class>
                 class TGetAliasListTT,
                 template<typename, template<typename...> class>
                 class TResolveAliasTypeTT>
        struct resolve_generic_class
        {
            using type = ResolvedT;
        };

        template<template<typename...> class TGenericT,
                 template<template<typename...> class>
                 class TWrapperTT,
                 template<template<typename...> class>
                 class TGetAliasListTT,
                 template<typename, template<typename...> class>
                 class TResolveAliasTypeTT>
        struct resolve_generic_class<TWrapperTT<TGenericT>, TGetAliasListTT, TResolveAliasTypeTT>
        {
            // get alias list that corresponds to the unspecialized generic class template
            using alias_list = typename TGetAliasListTT<TGenericT>::type;

            // resolve type by an alias and TGenericT for each alias from the alias_list
            using type_list =
                typename resolve_alias_list<TGenericT, alias_list, TResolveAliasTypeTT>::type;
            using speciazed_generic = typename apply_template_arg_list<TGenericT, type_list>::type;

            using type = typename resolve_generic_class<speciazed_generic,
                                                        TGetAliasListTT,
                                                        TResolveAliasTypeTT>::type;
        };
    }   // namespace detail

    template<template<typename...> class TGenericClassT,
             template<template<typename...> class>
             class TGetAliasListTT,
             template<typename, template<typename...> class>
             class TResolveAliasTypeTT>
    struct resolve_generic_class
    {
        using type = typename detail::resolve_generic_class<detail::wrapped_tt<TGenericClassT>,
                                                            TGetAliasListTT,
                                                            TResolveAliasTypeTT>::type;
    };
}   // namespace eld::impl