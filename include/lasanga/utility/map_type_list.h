#pragma once

#include "lasanga/utility/apply_predicates.h"
#include "lasanga/utility/convert_type_list.h"

#include <tuple>
#include <type_traits>
#include <utility>

namespace eld::util
{
    template<template<typename...> class TLogicalT,
             typename TypeListT,
             typename... WrappedPredicatesT>
    struct map_type_list;

    template<template<typename...> class TLogicalT,
             template<typename...>
             class TTypeListT,
             typename... TypesT,
             typename... WrappedPredicatesT>
    struct map_type_list<TLogicalT, TTypeListT<TypesT...>, WrappedPredicatesT...>
    {
        using type = typename convert_type_list<
            decltype(std::tuple_cat(
                std::declval<std::conditional_t<
                    apply_predicates<TLogicalT, TypesT, WrappedPredicatesT...>::value,
                    std::tuple<TypesT>,
                    std::tuple<>>>()...)),
            TTypeListT>::type;
    };
}   // namespace eld::util