#pragma once

#include "lasanga/utility/filter_type_list.h"
#include "lasanga/utility/traits.h"

#include <type_traits>

namespace eld::util
{
    namespace detail
    {
        template<typename AT, typename BT>
        struct same_wrapped_tt : std::is_same<AT, BT>
        {
        };

        template<template<typename...> class TAT,
                 template<typename...>
                 class TBT,
                 template<template<typename...> class>
                 class TWrappedATT,
                 template<template<typename...> class>
                 class TWrappedBTT>
        struct same_wrapped_tt<TWrappedATT<TAT>, TWrappedBTT<TBT>> : traits::is_same_tt<TAT, TBT>
        {
        };

        /*
         * If Dependent:
         *      - search for same alias and same context
         *      - search for same alias without context
         * Else:
         *      - search for same alias without context
         *      - search for same alias and same context
         */

        template<typename FactoryT>
        using is_independent = std::is_same<eld::tag::unnamed, typename FactoryT::depends_on_type>;

        template<typename AliasT,
                 typename DependsOnT,
                 typename ListFactoriesT,
                 typename ListModifiersT>
        struct resolve_impl
        {
            template<typename FactoryT>
            using same_alias = detail::same_wrapped_tt<typename FactoryT::alias_tag, AliasT>;

            template<typename FactoryT>
            using same_depends_on_type =
                detail::same_wrapped_tt<typename FactoryT::depends_on_type, DependsOnT>;

            using resolved_first =
                typename filter_type_list_conjunction<ListFactoriesT,
                                                      same_alias,
                                                      same_depends_on_type>::type;
            static_assert(traits::type_list_size<resolved_first>::value <= 1,
                          "Multiple designated factories found");

            using resolved_second =
                std::conditional_t<!traits::type_list_size<resolved_first>::value,
                                   typename filter_type_list_conjunction<ListFactoriesT,
                                                                         same_alias,
                                                                         is_independent>::type,
                                   resolved_first>;
            static_assert(traits::type_list_size<resolved_second>::value <= 1,
                          "Multiple designated factories found");
            static_assert(traits::type_list_size<resolved_second>::value != 0,
                          "No designated factories found");

            using type = typename traits::element_type<0, resolved_second>::type;
        };

        template<typename AliasT, typename ListFactoriesT, typename ListModifiersT>
        struct resolve_impl<AliasT, tag::unnamed, ListFactoriesT, ListModifiersT>
        {
            template<typename FactoryT>
            using same_alias = detail::same_wrapped_tt<typename FactoryT::alias_tag, AliasT>;

            using resolved_first = typename filter_type_list_conjunction<ListFactoriesT,
                                                                         same_alias,
                                                                         is_independent>::type;
            static_assert(traits::type_list_size<resolved_first>::value <= 1,
                          "Multiple designated factories found");

            using resolved_second = std::conditional_t<
                !traits::type_list_size<resolved_first>::value,
                typename filter_type_list_conjunction<ListFactoriesT, same_alias>::type,
                resolved_first>;
            static_assert(traits::type_list_size<resolved_second>::value <= 1,
                          "Multiple designated factories found");
            static_assert(traits::type_list_size<resolved_second>::value != 0,
                          "No designated factories found");

            using type = typename traits::element_type<0, resolved_second>::type;
        };

    }   // namespace detail

    /**
     * Resolve a designated factory by Alias, DependsOnT and a set of modifiers.
     * If DependsOnT is not specified, will first try to find a unique DesignatedFactory with the
     * same AliasT. If none have been found, will try to find a unique DesignatedFactory with any
     * DependsOnT specified and the same AliasT. If none or multiple factories were found, a
     * compile-time error will be generated.
     *
     * If DependsOnT is specified, will search among named DesignatedFactories with the same
     * alias and DependOnT. Then will search among independent factories. This is necessary due to
     * resolve_generic_class using unspecialized TGenericClassT template argument.
     *
     * @tparam AliasT
     * @tparam DependsOnT
     * @tparam ListFactoriesT
     * @tparam ListModifiersT
     */
    template<typename AliasT, typename DependsOnT, typename ListFactoriesT, typename ListModifiersT>
    struct resolve_factory
    {
        using type =
            typename detail::resolve_impl<AliasT, DependsOnT, ListFactoriesT, ListModifiersT>::type;
    };

}   // namespace eld::util
