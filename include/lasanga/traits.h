#pragma once

#include "lasanga/utility.h"

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

namespace eld
{
    namespace tag
    {
        // placeholder for template template tag
        struct any;

        struct not_found
        {
        };

        template<typename...>
        struct build
        {
        };

        struct default_call
        {
        };

        struct unnamed;

        template<typename...>
        struct unnamed_t;

    }   // namespace tag

    constexpr inline tag::not_found not_found;
    constexpr tag::default_call default_call_tag;

    namespace traits
    {
        template<typename T>
        struct is_tuple : std::false_type
        {
        };

        template<typename... T>
        struct is_tuple<std::tuple<T...>> : std::true_type
        {
        };

        template<typename, typename = void>
        struct is_complete : std::false_type
        {
        };

        template<typename T>
        struct is_complete<T, std::void_t<decltype(sizeof(T))>> : std::true_type
        {
        };

        template<typename T, typename BuilderT, typename = void>
        struct is_buildable : std::false_type
        {
        };

        template<typename T, typename BuilderT>
        struct is_buildable<
            T,
            BuilderT,
            std::void_t<decltype(std::declval<BuilderT>().operator()(tag::build<T>()))>> :
          std::true_type
        {
        };

        template<typename T>
        struct get_type
        {
            using type = T;
        };

        template<template<typename...> class Tag, typename Name, typename T, typename... U>
        struct get_type<Tag<Name, T, U...>>
        {
            using type = T;
        };

        template<typename T>
        struct get_name
        {
            using type = eld::tag::unnamed;
        };

        template<template<typename...> class Tag, typename Name, typename T, typename... U>
        struct get_name<Tag<Name, T, U...>>
        {
            using type = Name;
        };

        template<template<typename...> class ATT, template<typename...> class BTT>
        struct is_same_tt : std::false_type
        {
        };

        template<template<typename...> class TT>
        struct is_same_tt<TT, TT> : std::true_type
        {
        };

        template<typename T, typename From>
        struct is_constructible : std::is_constructible<T, From>
        {
        };

        template<typename T, typename... Args>
        struct is_constructible<T, std::tuple<Args...>> : std::is_constructible<T, Args...>
        {
        };

        template<template<typename...> class>
        struct is_unspecified : std::false_type
        {
        };

        template<>
        struct is_unspecified<tag::unnamed_t> : std::true_type
        {
        };

    }   // namespace traits

    template<typename T>
    struct value_type
    {
        using type = T;
    };

    template<typename>
    struct map_key_type : value_type<tag::not_found>
    {
    };

    template<typename KeyT>
    using find_type = typename map_key_type<KeyT>::type;

}   // namespace eld
