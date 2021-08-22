#pragma once

#include "lasanga/builder.h"
#include "lasanga/custom_override.h"
#include "lasanga/designated_factory.h"
#include "lasanga/layer.h"
#include "lasanga/traits.h"

namespace eld
{
    namespace detail
    {
        template<typename FactoryT,
                 typename NameTagT,
                 typename FactoryNameTagT = typename FactoryT::name_tag>
        struct is_same_name_tag : std::is_same<FactoryNameTagT, NameTagT>
        {
        };

        template<typename FactoryT,
                 template<template<typename...> class>
                 class TNameTagWrapperT,
                 template<typename...>
                 class TNameTagT,
                 template<template<typename...> class>
                 class TFactoryNameTagWrapperT,
                 template<typename...>
                 class TFactoryNameTagT>
        struct is_same_name_tag<FactoryT,
                                TNameTagWrapperT<TNameTagT>,
                                TFactoryNameTagWrapperT<TFactoryNameTagT>> :
          traits::is_same_tt<TNameTagT, TFactoryNameTagT>
        {
        };

        template<typename FactoryT,
                 typename DependsOnT,
                 typename FactoryDependsOnT = typename FactoryT::depends_on_type>
        struct is_same_depends_on : std::is_same<FactoryDependsOnT, DependsOnT>
        {
        };

        template<typename FactoryT,
                 template<template<typename...> class>
                 class TDependsOnTWrapperT,
                 template<typename...>
                 class TDependsOnT,
                 template<template<typename...> class>
                 class TFactoryDependsOnTWrapperT,
                 template<typename...>
                 class TFactoryDependsOnT>
        struct is_same_depends_on<FactoryT,
                                  TDependsOnTWrapperT<TDependsOnT>,
                                  TFactoryDependsOnTWrapperT<TFactoryDependsOnT>> :
          traits::is_same_tt<TDependsOnT, TFactoryDependsOnT>
        {
        };




        template<template<typename...> class TGenericClassT,
                 typename NameTagT,
                 typename TFactoriesListT>
        struct find_factory
        {
            /**
             * - search for named factory
             * - if not found any search unnamed
             */
            // TODO: map type list: same depends_on_type and name_tag or same name_tag
            using type_list = util::map_type_list_disjunction<TFactoriesListT>;

            static_assert(util::type_list_size<type_list>::value != 0,
                          "Failed to find a factory by name tag!");
            static_assert(
                util::type_list_size<type_list>::value <= 1,
                "Failed to resolve a factory by name tag: multiple factories were found!");

            using type = typename traits::element_type<0, type_list>::type;
        };

        template<template<typename...> class TGenericClassT,
                 typename TNameListT,
                 typename TFactoriesListT>
        struct resolve_factories_by_names;

        template<template<typename...> class TGenericClassT,
                 template<typename...>
                 class TNameListT,
                 typename... NameTagsT,
                 template<typename...>
                 class TFactoriesListT,
                 typename... FactoriesT>
        struct resolve_factories_by_names<TGenericClassT,
                                          TNameListT<NameTagsT...>,
                                          TFactoriesListT<FactoriesT...>>
        {
            /**
             * For each NameTag find one and only FactoryT which satisfies either
             *      FactoryT::DependsOnT == GenericT && FactoryT::NameTagT == NameTagsT
             *          or
             *      FactoryT::NameTagT == NameTagsT
             */
            using type =
                TFactoriesListT<typename find_factory<TGenericClassT,
                                                      NameTagsT,
                                                      TFactoriesListT<FactoriesT...>>::type...>;
        };

        template<typename TFactoriesListT>
        struct get_types_from_factories;

        template<template<typename...> class TFactoriesListT, typename... FactoriesT>
        struct get_types_from_factories<TFactoriesListT<FactoriesT...>>
        {
            using type = TFactoriesListT<typename FactoriesT::value_type...>;
        };

        template <template <typename...> class TGenericRootT, typename TNameListT,
        typename TAliasTypeMapT>
        struct resolve_types_by_names;

    }   // namespace detail

    /**
     * Create an object of a fully specialized class template GenericClass with specialization types
     * deduced from BuilderT using a user-defined specialization for `get_type_list<GenericClass,
     * Modifiers...>`.
     * @tparam TGenericClassT
     * @tparam GetNameList Customization for a type to get a name list from. Using get_name_list by
     * default
     * @tparam Modifiers
     * @tparam BuilderT
     * @param builder
     * @return
     *
     * \todo Try to deduce name list from GenericClass
     */
    template<template<typename...> class TGenericClassT,
             template<template<typename...> class, typename...> class GetNameList = get_name_list,
             typename... Modifiers,
             typename BuilderT>
    constexpr auto make_lasanga(BuilderT &&builder)
    {
        static_assert(traits::is_complete<GetNameList<TGenericClassT, Modifiers...>>::value,
                      "Name list has not been defined for GenericClass");

        /**
         * - Get Names List
         * - Get Factories List
         * - Resolve Types from Names List
         */

        // TODO: deduce name list from unspecialized GenericClass
        using name_list = typename GetNameList<TGenericClassT, Modifiers...>::type;
        using factories_list = typename std::decay_t<BuilderT>::factories_list;
        using resolved_factories = typename detail::
            resolve_factories_by_names<TGenericClassT, name_list, factories_list>::type;
        using type_list = typename detail::get_types_from_factories<resolved_factories>::type;

        return typename detail::specialize_t<TGenericClassT, type_list>::type(builder);
    }
}   // namespace eld