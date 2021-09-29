#pragma once

#include "lasanga/utility/construct_tree.h"
#include "lasanga/utility/convert_type_list.h"
#include "lasanga/utility/filter_type_list.h"

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

namespace eld
{
//    namespace detail
//    {
//
//        template<typename TupleT,
//                 typename ListWrappedPredicatesT,
//                 typename = decltype(std::make_index_sequence<std::tuple_size_v<TupleT>>())>
//        struct map_tuple_index_sequence;
//
//        template<template<typename...> class TupleT,
//                 typename... Types,
//                 template<typename...>
//                 class TypeListT,
//                 typename... WrappedPredicatesT,
//                 size_t... Indx>
//        struct map_tuple_index_sequence<TupleT<Types...>,
//                                        TypeListT<WrappedPredicatesT...>,
//                                        std::index_sequence<Indx...>>
//        {
//        private:
//            using tuple_input = TupleT<Types...>;
//
//            using tuple_indices = decltype(std::tuple_cat(
//                std::conditional_t<util::apply_predicates<Types, WrappedPredicatesT...>::value,
//                                   std::tuple<std::integral_constant<size_t, Indx>>,
//                                   std::tuple<>>()...));
//
//            template<typename TupleI>
//            struct sequence_from_tuple;
//
//            template<template<typename...> class TupleIC, size_t... IC>
//            struct sequence_from_tuple<TupleIC<std::integral_constant<size_t, IC>...>>
//            {
//                using type = std::index_sequence<IC...>;
//            };
//
//        public:
//            using type = typename sequence_from_tuple<tuple_indices>::type;
//        };
//
//        template<typename TupleT,
//                 typename ListWrappedPredicatesT,
//                 typename = typename map_tuple_index_sequence<TupleT, ListWrappedPredicatesT>::type>
//        struct map_tuple;
//
//        template<template<typename...> class TupleT,
//                 typename... TGenericClassT,
//                 typename ListWrappedPredicatesT,
//                 size_t... Indx>
//        struct map_tuple<TupleT<TGenericClassT...>, ListWrappedPredicatesT, std::index_sequence<Indx...>>
//        {
//            constexpr auto operator()(TupleT<TGenericClassT...> &tuple)
//            {
//                return TupleT<std::tuple_element_t<Indx, std::decay_t<decltype(tuple)>> &...>(
//                    std::get<Indx>(tuple)...);
//            }
//
//            constexpr auto operator()(const TupleT<TGenericClassT...> &tuple)
//            {
//                return TupleT<std::tuple_element_t<Indx, std::decay_t<decltype(tuple)>> &...>(
//                    std::get<Indx>(tuple)...);
//            }
//        };
//
//        template<template<typename...> class TGenericClassT, typename TypeList, typename = void>
//        struct can_specialize : std::false_type
//        {
//        };
//
//        template<template<typename...> class TGenericClassT,
//                 template<typename...>
//                 class TTypeListT,
//                 typename... TypesT>
//        struct can_specialize<TGenericClassT,
//                              TTypeListT<TypesT...>,
//                              std::void_t<TGenericClassT<TypesT...>>> : std::true_type
//        {
//        };
//
//        template<template<typename...> class /*TGenericClassT*/, typename /*TypeList*/>
//        struct specialize_t;
//
//        /**
//         * Specialize an unspecialized class template TGenericClassT using a type_list of Types
//         * @tparam TGenericClassT
//         * @tparam Types Types to be used for specialization.
//         * \todo: support template tree types?
//         */
//        template<template<typename...> class TGenericClassT,
//                 template<typename...>
//                 class TTypeListT,
//                 typename... Types>
//        struct specialize_t<TGenericClassT, TTypeListT<Types...>>
//        {
//            static_assert(can_specialize<TGenericClassT, TTypeListT<Types...>>::value,
//                          "Can not specialize TGenericClassT from a given Type List");
//            using type = TGenericClassT<Types...>;
//        };
//
//        template<template<typename...> class TOutputListT, typename NameT, typename... FactoriesT>
//        struct map_type_by_name
//        {
//            using type = typename filter_type_list<TOutputListT, NameT, FactoriesT...>::type;
//        };
//
//        template<template<typename...> class TOutputListT, typename NameT, typename... FactoriesT>
//        struct resolve_type_by_name
//        {
//            // TODO: recursively specialize Template Types from factories
//            using type = typename map_type_by_name<TOutputListT, NameT, FactoriesT...>::type;
//
//            static_assert(util::type_list_size<type>::value != 0,
//                          "Failed to resolve type from NameTag");
//            static_assert(util::type_list_size<type>::value <= 1,
//                          "NameTag resolves to multiple types");
//        };
//
//    }   // namespace detail

    namespace util
    {
        template<typename...>
        struct type_list;

        template<template<typename...> class, typename...>
        struct wrapped_predicate;

//        template<typename... WrappedPredicates, typename TupleT>
//        constexpr auto map_tuple(TupleT &&tuple)
//        {
//            return eld::detail::map_tuple<std::decay_t<TupleT>, type_list<WrappedPredicates...>>{}(
//                tuple);
//        }

//        template<typename NameListT,
//                 typename FactoryListT>
//        struct resolve_name_list;
//
//        template<template<typename...> class TNameListT,
//                 typename... NamesT,
//                 template<typename...>
//                 class TFactoryListT,
//                 typename... FactoriesT>
//        struct resolve_name_list<TNameListT<NamesT...>, TFactoryListT<FactoriesT...>>
//        {
//            using type =
//                TFactoryListT<typename eld::detail::resolve_type_by_name<TFactoryListT,
//                                                                        NamesT,
//                                                                        FactoriesT...>::type...>;
//        };

    }   // namespace util
}   // namespace eld
