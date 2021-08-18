#pragma once

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

namespace eld
{
    namespace util
    {
        template<typename>
        struct type_list_size;

        template<template<typename...> class TTypeListT, typename... Types>
        struct type_list_size<TTypeListT<Types...>>
        {
            constexpr static size_t value = sizeof...(Types);
        };
    }   // namespace util

    namespace detail
    {
        template<typename WrappedPredicateT, typename T>
        struct apply_predicate;

        template<template<typename...> class Predicate,
                 typename T,
                 typename... Modifiers,
                 template<template<typename...> class, typename...>
                 class PredicateWrapper>
        struct apply_predicate<PredicateWrapper<Predicate, Modifiers...>, T>
        {
            constexpr static auto value = Predicate<T, Modifiers...>::value;
        };

        template<typename T, typename... WrappedPredicates>
        struct apply_predicates
        {
            constexpr static auto value =
                std::conjunction_v<apply_predicate<WrappedPredicates, T>...>;
        };

        template<typename TupleT,
                 typename ListWrappedPredicatesT,
                 typename = decltype(std::make_index_sequence<std::tuple_size_v<TupleT>>())>
        struct map_tuple_index_sequence;

        template<template<typename...> class TupleT,
                 typename... Types,
                 template<typename...>
                 class TypeListT,
                 typename... WrappedPredicatesT,
                 size_t... Indx>
        struct map_tuple_index_sequence<TupleT<Types...>,
                                        TypeListT<WrappedPredicatesT...>,
                                        std::index_sequence<Indx...>>
        {
        private:
            using tuple_input = TupleT<Types...>;

            using tuple_indices = decltype(std::tuple_cat(
                std::conditional_t<apply_predicates<Types, WrappedPredicatesT...>::value,
                                   std::tuple<std::integral_constant<size_t, Indx>>,
                                   std::tuple<>>()...));

            template<typename TupleI>
            struct sequence_from_tuple;

            template<template<typename...> class TupleIC, size_t... IC>
            struct sequence_from_tuple<TupleIC<std::integral_constant<size_t, IC>...>>
            {
                using type = std::index_sequence<IC...>;
            };

        public:
            using type = typename sequence_from_tuple<tuple_indices>::type;
        };

        template<typename TupleT,
                 typename ListWrappedPredicatesT,
                 typename = typename map_tuple_index_sequence<TupleT, ListWrappedPredicatesT>::type>
        struct map_tuple;

        template<template<typename...> class TupleT,
                 typename... T,
                 typename ListWrappedPredicatesT,
                 size_t... Indx>
        struct map_tuple<TupleT<T...>, ListWrappedPredicatesT, std::index_sequence<Indx...>>
        {
            constexpr auto operator()(TupleT<T...> &tuple)
            {
                return TupleT<std::tuple_element_t<Indx, std::decay_t<decltype(tuple)>> &...>(
                    std::get<Indx>(tuple)...);
            }

            constexpr auto operator()(const TupleT<T...> &tuple)
            {
                return TupleT<std::tuple_element_t<Indx, std::decay_t<decltype(tuple)>> &...>(
                    std::get<Indx>(tuple)...);
            }
        };

        template<template<typename...> class /*GenericClass*/, typename /*TypeList*/>
        struct specialize_t;

        /**
         * Specialize an unspecialized class template GenericClass using a type_list of Types
         * @tparam GenericClass
         * @tparam Types Types to be used for specialization.
         * \todo: support template tree types?
         */
        template<template<typename...> class GenericClass,
                 template<typename...>
                 class TTypeListT,
                 typename... Types>
        struct specialize_t<GenericClass, TTypeListT<Types...>>
        {
            // TODO: understandable compile time error
            using type = GenericClass<Types...>;
        };
    }   // namespace detail

    namespace util
    {
        template<typename...>
        struct type_list;

        template<typename FromT, template<typename...> class TToT>
        struct convert_type_list;

        template<template<typename...> class TFromT,
                 typename... TypesT,
                 template<typename...>
                 class TToT>
        struct convert_type_list<TFromT<TypesT...>, TToT>
        {
            using type = TToT<TypesT...>;
        };

        template<template<typename...> class, typename...>
        struct wrapped_predicate;

        template<typename... WrappedPredicates, typename TupleT>
        constexpr auto map_tuple(TupleT &&tuple)
        {
            return detail::map_tuple<std::decay_t<TupleT>, type_list<WrappedPredicates...>>{}(
                tuple);
        }
    }   // namespace util
}   // namespace eld
