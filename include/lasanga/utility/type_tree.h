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

#include "lasanga/utility/map_type_list.h"
#include "lasanga/utility/traits.h"

#include <type_traits>
#include <utility>

namespace eld::util
{
    //    namespace detail
    //    {
    //        template<typename TDescriptorT, typename TNameTagT>
    //        struct is_same_name_tag : std::is_same<typename TDescriptorT::name_tag, TNameTagT>
    //        {
    //        };
    //
    //        // TODO: add depends_on_type, unnamed_tt by default
    //        template<typename AliasT,
    //                 typename TTypeListT,
    //                 typename = typename map_type_list_conjunction<
    //                     TTypeListT,
    //                     util::wrapped_predicate<is_same_name_tag, AliasT>>::type>
    //        struct find_type_by_alias;
    //
    //        // TODO: add depends_on_type, unnamed_tt by default
    //        template<typename AliasT,
    //                 template<typename...>
    //                 class TTypeListT,
    //                 typename... TypesT,
    //                 typename... FoundTypesT>
    //        struct find_type_by_alias<AliasT, TTypeListT<TypesT...>, TTypeListT<FoundTypesT...>>
    //        {
    //            static_assert(sizeof...(FoundTypesT) != 0,
    //                          "Failed to find type by alias, no descriptors found!");
    //            static_assert(sizeof...(FoundTypesT) <= 1,
    //                          "Failed to find type by alias, multiple descriptors found!");
    //        };
    //
    //        template<typename AliasT,
    //                 template<typename...>
    //                 class TTypeListT,
    //                 typename... TypesT,
    //                 typename FoundTypeT>
    //        struct find_type_by_alias<AliasT, TTypeListT<TypesT...>, TTypeListT<FoundTypeT>>
    //        {
    //            using type = typename FoundTypeT::value_type;
    //        };
    //
    //        template<template<template<typename...> class, typename...> class TTGetAliasListT,
    //                 typename TWrappedRootT,
    //                 typename... ModifiersT>
    //        struct get_alias_list_from_wrapped
    //        {
    //            // just to allow compilation. This type is not valid
    //            using type = void;
    //        };
    //
    //        template<template<template<typename...> class, typename...> class TTGetAliasListT,
    //                 template<template<typename...> class>
    //                 class TTRootWrapperT,
    //                 template<typename...>
    //                 class TRootTypeT,
    //                 typename... ModifiersT>
    //        struct get_alias_list_from_wrapped<TTGetAliasListT,
    //                                           TTRootWrapperT<TRootTypeT>,
    //                                           ModifiersT...>
    //        {
    //            using type = typename TTGetAliasListT<TRootTypeT, ModifiersT...>::type;
    //        };
    //
    //        template<template<template<typename...> class, typename...> class TTTreeNodeT,
    //                 template<template<typename...> class, typename...>
    //                 class TTGetAliasListT,
    //                 typename TWrappedRootT,
    //                 typename TAliasListT,
    //                 typename TTypeMapListT,
    //                 typename... ModifiersT>
    //        struct construct_tree_recursive
    //        {
    //            // just to allow compilation. This type is not valid
    //            using type = void;
    //        };
    //
    //        template<template<template<typename...> class, typename...> class TTTreeNodeT,
    //                 template<template<typename...> class, typename...>
    //                 class TTGetAliasListT,
    //                 template<template<typename...> class>
    //                 class TTRootWrapperT,
    //                 template<typename...>
    //                 class TRootTypeT,
    //                 template<typename...>
    //                 class TTypeListT,
    //                 typename... AliasesT,
    //                 typename TTypeMapListT,
    //                 typename... ModifiersT>
    //        struct construct_tree_recursive<TTTreeNodeT,
    //                                        TTGetAliasListT,
    //                                        TTRootWrapperT<TRootTypeT>,
    //                                        TTypeListT<AliasesT...>,
    //                                        TTypeMapListT,
    //                                        ModifiersT...>
    //        {
    //            // TODO: Get Aliases List from TRootTypeT !here!
    //            template<typename AliasT>
    //            using find_type_t = typename detail::find_type_by_alias<AliasT,
    //            TTypeMapListT>::type;
    //
    //            using type = TTTreeNodeT<
    //                TRootTypeT,
    //                std::conditional_t<traits::is_template_wrapper<find_type_t<AliasesT>>::value,
    //                                   typename construct_tree_recursive<
    //                                       TTTreeNodeT,
    //                                       TTGetAliasListT,
    //                                       find_type_t<AliasesT>,
    //                                       typename get_alias_list_from_wrapped<TTGetAliasListT,
    //                                                                            find_type_t<AliasesT>,
    //                                                                            ModifiersT...>::type,
    //                                       TTypeMapListT,
    //                                       ModifiersT...>::type,
    //                                   find_type_t<AliasesT>>...>;
    //        };
    //
    //        template<template<typename...> class, typename...>
    //        struct wrapped_tt;
    //
    //    }   // namespace detail
    //
    //    template<template<template<typename...> class, typename...> class TTTreeNodeT,
    //             template<template<typename...> class, typename...>
    //             class TTGetAliasListT,
    //             template<typename...>
    //             class TGenericRootT,
    //             typename TTypeMapListT,
    //             typename... ModifiersT>
    //    struct construct_tree_generic_old
    //    {
    //        using type = typename detail::construct_tree_recursive<
    //            TTTreeNodeT,
    //            TTGetAliasListT,
    //            detail::wrapped_tt<TGenericRootT>,
    //            typename TTGetAliasListT<TGenericRootT, ModifiersT...>::type,
    //            TTypeMapListT,
    //            ModifiersT...>::type;
    //    };

    //    template<template<typename...> class TGenericRootT,
    //             typename TTypeMapListT,
    //             typename... ModifiersT>
    //    using construct_tree_old = construct_tree_generic_old<type_node,
    //                                                          get_alias_list,
    //                                                          TGenericRootT,
    //                                                          TTypeMapListT,
    //                                                          ModifiersT...>;

    /**
     * TODO: construct tree: Given a generic root class and modifiers, recursively get aliases and
     *  corresponding types:
     *  - get aliases from GenericRoot
     *  - get types for each alias
     *  - if type is a generic root, get aliases
     */

    namespace detail
    {
        template<template<template<typename...> class, typename...> class TTGetAliasListT,
                 typename TTGenericRootWithModifiersT>
        struct get_alias_list_wrapped;

        template<template<template<typename...> class, typename...> class TTGetAliasListT,
                 template<template<typename...> class, typename...>
                 class TTWrapperT,
                 template<typename...>
                 class TGenericRootT,
                 typename... ModifiersT>
        struct get_alias_list_wrapped<TTGetAliasListT, TTWrapperT<TGenericRootT, ModifiersT...>>
        {
            using type = typename TTGetAliasListT<TGenericRootT, ModifiersT...>::type;
        };

        template<template<template<typename...> class, typename...> class TTGetAliasListT,
                 typename TTGenericRootWithModifiersT>
        using get_alias_list_t =
            typename get_alias_list_wrapped<TTGetAliasListT, TTGenericRootWithModifiersT>::type;

        template<typename>
        struct is_generic_root : std::false_type
        {
        };

        template<template<typename...> class TGenericRootT,
                 typename... M,
                 template<template<typename...> class, typename...>
                 class TTWrapperT>
        struct is_generic_root<TTWrapperT<TGenericRootT, M...>> : std::true_type
        {
        };

    }   // namespace detail

    template<template<template<typename...> class, typename...> class TTGetAliasListT,
             template<typename, template<typename...> class, typename...>
             class TTGetTypeFromAliasT,
             typename TMapAliasToTypeT,
             typename TTGenericRootWithModifiersT,
             typename TAliasListT =
                 detail::get_alias_list_t<TTGetAliasListT, TTGenericRootWithModifiersT>>
    struct construct_tree_generic;

    /**
     * Given a TGenericRootT, list of ModifiersT and meta functions TTGetAliasListT and
     * TTGetTypeFromAliasT, recursively construct a type tree that can be used to fully specialize
     * TGenericRootT class template.
     * @tparam TTGetAliasListT Meta function to get a list of aliases from TGenericRootT with
     * ModifiersT
     * @tparam TTGetTypeFromAliasT Meta function to get a corresponding to an alias type from
     * TMapAliasToTypeT using TGenericRootT with ModifiersT
     * @tparam TMapAliasToTypeT
     * @tparam TGenericRootT
     * @tparam ModifiersT
     * @tparam AliasesT
     * @tparam TTGenericListT
     * @tparam TTypeListT
     */
    template<template<template<typename...> class, typename...> class TTGetAliasListT,
             template<typename, template<typename...> class, typename...>
             class TTGetTypeFromAliasT,
             typename TMapAliasToTypeT,
             template<typename...>
             class TGenericRootT,
             typename... ModifiersT,
             typename... AliasesT,
             template<template<typename...> class, typename...>
             class TTGenericListT,
             template<typename...>
             class TTypeListT>
    struct construct_tree_generic<TTGetAliasListT,
                                  TTGetTypeFromAliasT,
                                  TMapAliasToTypeT,
                                  TTGenericListT<TGenericRootT, ModifiersT...>,
                                  TTypeListT<AliasesT...>>
    {
        template<typename A>
        using type_from_alias =
            typename TTGetTypeFromAliasT<TMapAliasToTypeT, TGenericRootT, A, ModifiersT...>::type;

        using type = TTGenericListT<
            TGenericRootT,
            std::conditional_t<detail::is_generic_root<type_from_alias<AliasesT>>::value,
                               typename construct_tree_generic<TTGetAliasListT,
                                                               TTGetTypeFromAliasT,
                                                               TMapAliasToTypeT,
                                                               type_from_alias<AliasesT>>::type,
                               type_from_alias<AliasesT>>...>;
    };

    /**
     * Get a corresponding to AliasT type from TMapAliasToTypeT using TGenericClassT and a list of
     * ModifiersT.
     * \details First will try to find a type registered with TGenericClassT and ModifiersT. If
     * fails, will try to find a common case with TGenericClassT = unspecified_tt. Alias must be
     * resolved to a single type, otherwise a compile time error will occur.
     * @tparam TMapAliasToTypeT
     * @tparam TGenericClassT
     * @tparam AliasT
     * @tparam ModifiersT
     */
    template<typename TMapAliasToTypeT,
             template<typename...>
             class TGenericClassT,
             typename AliasT,
             typename... ModifiersT>
    class get_type_from_alias
    {
    public:
        // TODO: implement
        using type = void;
    };

    /**
     * Default type for a template tree type node.
     * @tparam TGenericClassT
     * @tparam SpecTypesT
     */
    template<template<typename...> class TGenericClassT, typename... SpecTypesT>
    struct type_node
    {
    };

    /**
     * Generic template for mapping alias lists to TGenericClassT with ModifiersT.
     * User must provide a specialization that defines a typename
     * "using type = type_list<AliasesT...>"
     * @tparam TGenericClassT
     * @tparam ModifiersT
     */
    template<template<typename...> class TGenericClassT, typename... ModifiersT>
    struct get_alias_list;

}   // namespace eld::util
