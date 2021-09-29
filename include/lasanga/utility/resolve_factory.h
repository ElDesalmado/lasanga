#pragma once

#include "lasanga/utility/filter_type_list.h"
#include "lasanga/utility/traits.h"

#include <type_traits>

namespace eld::util
{
    namespace detail
    {
        template<typename AliasAT, typename AliasBT>
        struct same_alias : std::is_same<AliasAT, AliasBT>
        {
        };

        template<template<typename...> class TAliasATT,
                 template<typename...>
                 class TAliasBTT,
                 template<template<typename...> class>
                 class TWrapperATT,
                 template<template<typename...> class>
                 class TWrapperBTT>
        struct same_alias<TWrapperATT<TAliasATT>, TWrapperBTT<TAliasBTT>> :
          traits::is_same_tt<TAliasATT, TAliasBTT>
        {
        };

        template<template<typename...> class TDependsOnAT, template<typename...> class TDependsOnBT>
        struct same_depends_on : traits::is_same_tt<TDependsOnAT, TDependsOnBT>
        {
        };
    }   // namespace detail

    /**
         * Resolve a designated factory by Alias, TGenericClassT and a set of modifiers.
         * @tparam AliasT
         * @tparam TGenericClassT
         * @tparam ListFactoriesT
         * @tparam ListModifiersT
     */
    template<typename AliasT,
             template<typename...>
             class TGenericClassT,
             typename ListFactoriesT,
             typename ListModifiersT>
    class resolve_factory
    {
        template<typename FactoryT>
        using same_alias = detail::same_alias<typename FactoryT::alias_tag, AliasT>;

        template<typename FactoryT>
        using same_generic_class =
            detail::same_depends_on<TGenericClassT, FactoryT::template depends_on_type>;

        // TODO: implement
        using filtered_type_list =
            typename filter_type_list_conjunction<ListFactoriesT,
                                                  same_alias,
                                                  same_generic_class>::type;

        static_assert(traits::type_list_size<filtered_type_list>::value <= 1,
                      "Multiple designated factories resolved from Alias and GenericClass");
        static_assert(traits::type_list_size<filtered_type_list>::value > 0,
                      "No designated factories resolved from Alias and GenericClass");

    public:
        using type = typename traits::element_type<0, filtered_type_list>::type;
    };

}   // namespace eld::util
