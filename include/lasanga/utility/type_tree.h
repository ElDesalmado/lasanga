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

        template<typename>
        struct is_template_wrapper : std::false_type
        {
        };

        template<template<template<typename...> class, typename...> class TTWrapperTypeT,
                 template<typename...>
                 class TGenericTypeT>
        struct is_template_wrapper<TTWrapperTypeT<TGenericTypeT>> : std::true_type
        {
        };

    }   // namespace traits

    namespace util
    {
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

        namespace detail
        {
            template<typename TDescriptorT, typename TNameTagT>
            struct is_same_name_tag : std::is_same<typename TDescriptorT::name_tag, TNameTagT>
            {
            };

            template<typename AliasT,
                     typename TTypeListT,
                     typename = typename map_type_list_conjunction<
                         TTypeListT,
                         util::wrapped_predicate<is_same_name_tag, AliasT>>::type>
            struct find_type_by_alias;

            template<typename AliasT,
                     template<typename...>
                     class TTypeListT,
                     typename... TypesT,
                     typename... FoundTypesT>
            struct find_type_by_alias<AliasT, TTypeListT<TypesT...>, TTypeListT<FoundTypesT...>>
            {
                static_assert(sizeof...(FoundTypesT) != 0,
                              "Failed to find type by alias, no descriptors found!");
                static_assert(sizeof...(FoundTypesT) <= 1,
                              "Failed to find type by alias, multiple descriptors found!");
            };

            template<typename AliasT,
                     template<typename...>
                     class TTypeListT,
                     typename... TypesT,
                     typename FoundTypeT>
            struct find_type_by_alias<AliasT, TTypeListT<TypesT...>, TTypeListT<FoundTypeT>>
            {
                using type = typename FoundTypeT::value_type;
            };

            template<template<template<typename...> class, typename...> class TTTreeNodeT,
                     typename TWrappedRootT,
                     typename TAliasListT,
                     typename TTypeMapListT,
                     typename... ModifiersT>
            struct construct_tree
            {
                // just to allow compilation. This type is not valid
                using type = void;
            };

            template<template<template<typename...> class, typename...> class TTTreeNodeT,
                     template<template<typename...> class>
                     class TTRootWrapperT,
                     template<typename...>
                     class TRootTypeT,
                     template<typename...>
                     class TTypeListT,
                     typename... AliasesT,
                     typename TTypeMapListT,
                     typename... ModifiersT>
            struct construct_tree<TTTreeNodeT,
                                  TTRootWrapperT<TRootTypeT>,
                                  TTypeListT<AliasesT...>,
                                  TTypeMapListT,
                                  ModifiersT...>
            {
                // TODO: Get Aliases List from TRootTypeT !here!
                template<typename AliasT>
                using find_type_t =
                    typename detail::find_type_by_alias<AliasT, TTypeMapListT>::type;

                using type = TTTreeNodeT<
                    TRootTypeT,
                    std::conditional_t<traits::is_template_wrapper<find_type_t<AliasesT>>::value,
                                       typename construct_tree<TTTreeNodeT,
                                                               find_type_t<AliasesT>,
                                                               TTypeListT<AliasesT...>,
                                                               TTypeMapListT,
                                                               ModifiersT...>::type,
                                       find_type_t<AliasesT>>...>;
            };

        }   // namespace detail
            //
            //        template<template<template<typename...> class, typename...> class TTTreeNodeT,
            //                 template<typename...>
            //                 class TRootTypeT,
            //                 typename TAliasListT,
            //                 typename TTypeMapListT,
            //                 typename... ModifiersT>
            //        struct construct_tree;
            //
            //        template<template<template<typename...> class, typename...> class TTTreeNodeT,
            //                 template<typename...>
            //                 class TRootTypeT,
            //                 template<typename...>
            //                 class TTypeListT,
            //                 typename... AliasTypesT,
            //                 typename... AliasToTypesT,
            //                 typename... ModifiersT>
            //        struct construct_tree<TTTreeNodeT,
            //                              TRootTypeT,
            //                              TTypeListT<AliasTypesT...>,
            //                              TTypeListT<AliasToTypesT...>,
            //                              ModifiersT...>
            //        {
            //            template<typename AliasT>
            //            using find_type_t =
            //                typename detail::find_type_by_alias<AliasT,
            //                TTypeListT<AliasToTypesT...>>::type;
            //            // TODO: implement
            //            /**
            //             * for each AliasType find a corresponding type.
            //             * If a corresponding type is a template typename, call construct_tree
            //             recursively
            //             */
            //            using type = TTTreeNodeT<
            //                TRootTypeT,
        //                std::conditional_t<traits::is_template_wrapper<find_type_t<AliasTypesT>>::value,
        //                                   typename construct_tree<>::type,
        //                                   find_type_t<AliasTypesT>>...>;
        //        };

        /**
         * Default type for a template tree type node.
         * @tparam TGenericClassT
         * @tparam SpecTypesT
         */
        template<template<typename...> class TGenericClassT, typename... SpecTypesT>
        struct type_node
        {
        };

    }   // namespace util
}   // namespace eld
