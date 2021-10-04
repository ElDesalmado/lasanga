#pragma once

#include <tuple>
#include <type_traits>
#include <utility>

namespace eld::util
{
    namespace detail
    {
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
    }

    template<template<typename...> class TLogicalT,
             typename TTypeListT,
             template<typename>
             class... TPredicatesTT>
    struct filter_type_list;

    template<template<typename...> class TLogicalT,
             typename... TypesT,
             template<typename...>
             class TTypeListTT,
             template<typename>
             class... TPredicatesTT>
    struct filter_type_list<TLogicalT, TTypeListTT<TypesT...>, TPredicatesTT...>
    {
        template <typename T>
        using logical_value_type = TLogicalT<TPredicatesTT<T>...>;

        using type = typename detail::convert_type_list<
            decltype(std::tuple_cat(
                std::declval<std::conditional_t<logical_value_type<TypesT>::value,
                                                std::tuple<TypesT>,
                                                std::tuple<>>>()...)),
            TTypeListTT>::type;
    };

    template<typename TTypeListT, template<typename> class... TPredicatesTT>
    using filter_type_list_conjunction =
        filter_type_list<std::conjunction, TTypeListT, TPredicatesTT...>;

    template<typename TTypeListT, template<typename> class... TPredicatesTT>
    using filter_type_list_disjunction =
        filter_type_list<std::disjunction, TTypeListT, TPredicatesTT...>;

}   // namespace eld::util