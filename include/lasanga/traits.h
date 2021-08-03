#pragma once

#include <tuple>
#include <type_traits>
#include <cstddef>
#include <utility>

namespace eld
{
    namespace tag
    {
        // placeholder for template template tag
        struct any;

        template<typename...>
        struct build
        {
        };

        struct default_call
        {
        };

    }   // namespace tag

    constexpr tag::default_call default_call_tag;

    struct unnamed;

    template<typename Name, typename T, typename...>
    struct descriptor_t;

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

        template<typename>
        struct is_composition : std::false_type
        {
        };

        template<typename... T>
        struct is_composition<std::tuple<T...>> : std::true_type
        {
        };

        template<typename... T>
        struct is_composition<std::tuple<const T...>> : std::true_type
        {
        };

        template<typename>
        struct is_aggregation : std::false_type
        {
        };

        template<typename... T>
        struct is_aggregation<std::tuple<T &...>> : std::true_type
        {
        };

        template<typename... T>
        struct is_aggregation<std::tuple<const T &...>> : std::true_type
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
            using type = eld::unnamed;
        };

        template<template<typename...> class Tag, typename Name, typename T, typename... U>
        struct get_name<Tag<Name, T, U...>>
        {
            using type = Name;
        };

    }   // namespace traits

    struct not_found;

    template<typename T>
    struct value_type
    {
        using type = T;
    };

    template<typename>
    struct map_key_type : value_type<not_found>
    {
    };

    template<typename KeyT>
    using find_type = typename map_key_type<KeyT>::type;

}   // namespace eld
