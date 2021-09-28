#pragma once

// generic headers
#include "lasanga/generic/builder.h"
#include "lasanga/generic/lasanga.h"

// default implementations
#include "lasanga/alias_list.h"
#include "lasanga/utility/builder_impl.h"

//#include "lasanga/builder.h"
//#include "lasanga/custom_override.h"
//#include "lasanga/designated_factory.h"
//#include "lasanga/generic/traits.h"
//#include "lasanga/layer.h"

namespace eld
{
    //    namespace detail
    //    {
    //        template<typename FactoryT,
    //                 typename AliasTagT,
    //                 typename FactoryNameTagT = typename FactoryT::alias_tag>
    //        struct is_same_name_tag : std::is_same<FactoryNameTagT, AliasTagT>
    //        {
    //        };
    //
    //        template<typename FactoryT,
    //                 template<template<typename...> class>
    //                 class TNameTagWrapperT,
    //                 template<typename...>
    //                 class TNameTagT,
    //                 template<template<typename...> class>
    //                 class TFactoryNameTagWrapperT,
    //                 template<typename...>
    //                 class TFactoryNameTagT>
    //        struct is_same_name_tag<FactoryT,
    //                                TNameTagWrapperT<TNameTagT>,
    //                                TFactoryNameTagWrapperT<TFactoryNameTagT>> :
    //          traits::is_same_tt<TNameTagT, TFactoryNameTagT>
    //        {
    //        };
    //
    //        template<typename FactoryT,
    //                 typename DependsOnT,
    //                 typename FactoryDependsOnT = typename FactoryT::depends_on_type>
    //        struct is_same_depends_on : std::is_same<FactoryDependsOnT, DependsOnT>
    //        {
    //        };
    //
    //        template<typename FactoryT,
    //                 template<template<typename...> class>
    //                 class TDependsOnTWrapperT,
    //                 template<typename...>
    //                 class TDependsOnT,
    //                 template<template<typename...> class>
    //                 class TFactoryDependsOnTWrapperT,
    //                 template<typename...>
    //                 class TFactoryDependsOnT>
    //        struct is_same_depends_on<FactoryT,
    //                                  TDependsOnTWrapperT<TDependsOnT>,
    //                                  TFactoryDependsOnTWrapperT<TFactoryDependsOnT>> :
    //          traits::is_same_tt<TDependsOnT, TFactoryDependsOnT>
    //        {
    //        };
    //
    //
    //
    //
    //        template<template<typename...> class TGenericClassT,
    //                 typename AliasTagT,
    //                 typename TFactoriesListT>
    //        struct find_factory
    //        {
    //            /**
    //             * - search for named factory
    //             * - if not found any search unnamed
    //             */
    //            // TODO: map type list: same depends_on_type and alias_tag or same alias_tag
    //            using type_list = util::map_type_list_disjunction<TFactoriesListT>;
    //
    //            static_assert(util::type_list_size<type_list>::value != 0,
    //                          "Failed to find a factory by name tag!");
    //            static_assert(
    //                util::type_list_size<type_list>::value <= 1,
    //                "Failed to resolve a factory by name tag: multiple factories were found!");
    //
    //            using type = typename traits::element_type<0, type_list>::type;
    //        };
    //
    //        template<template<typename...> class TGenericClassT,
    //                 typename TNameListT,
    //                 typename TFactoriesListT>
    //        struct resolve_factories_by_names;
    //
    //        template<template<typename...> class TGenericClassT,
    //                 template<typename...>
    //                 class TNameListT,
    //                 typename... NameTagsT,
    //                 template<typename...>
    //                 class TFactoriesListT,
    //                 typename... FactoriesT>
    //        struct resolve_factories_by_names<TGenericClassT,
    //                                          TNameListT<NameTagsT...>,
    //                                          TFactoriesListT<FactoriesT...>>
    //        {
    //            /**
    //             * For each NameTag find one and only FactoryT which satisfies either
    //             *      FactoryT::DependsOnT == GenericT && FactoryT::AliasTagT == NameTagsT
    //             *          or
    //             *      FactoryT::AliasTagT == NameTagsT
    //             */
    //            using type =
    //                TFactoriesListT<typename find_factory<TGenericClassT,
    //                                                      NameTagsT,
    //                                                      TFactoriesListT<FactoriesT...>>::type...>;
    //        };
    //
    //        template<typename TFactoriesListT>
    //        struct get_types_from_factories;
    //
    //        template<template<typename...> class TFactoriesListT, typename... FactoriesT>
    //        struct get_types_from_factories<TFactoriesListT<FactoriesT...>>
    //        {
    //            using type = TFactoriesListT<typename FactoriesT::value_type...>;
    //        };
    //
    //        template <template <typename...> class TGenericRootT, typename TNameListT,
    //        typename TAliasTypeMapT>
    //        struct resolve_types_by_names;
    //
    //    }   // namespace detail


    template<template<typename...> class TGenericClassT, typename... ModifiersT, typename BuilderT>
    constexpr auto make_lasanga(BuilderT &&builder)
    {
        return generic::make_lasanga<generic::resolve_generic_class, TGenericClassT, ModifiersT...>(
            builder);
    }
}   // namespace eld