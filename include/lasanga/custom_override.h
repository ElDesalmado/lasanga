#pragma once

#include "lasanga/traits.h"

namespace eld
{
    namespace detail
    {
        template<typename SignatureTag, typename T, typename... ArgsT>
        constexpr decltype(auto) invoke_by_signature(T &&obj, ArgsT &&...args)
        {
            return SignatureTag()(obj, std::forward<ArgsT>(args)...);
        }

        // indx is 1 less than tuple size -> callable is the last argument
        template<typename SignatureTag, typename T, typename TupleArgs, size_t... indx>
        constexpr decltype(auto) invoke_default_call(T &&obj,
                                                     TupleArgs &&tupleArgs,
                                                     std::index_sequence<indx...>,
                                                     std::true_type /*is_complete<SignatureTag>*/)
        {
            static_assert(std::tuple_size_v<TupleArgs> >= 1, "Callable is not provided");
            static_assert(std::tuple_size_v<TupleArgs> - sizeof...(indx) == 1,
                          "Callable must be the last argument!");

            return invoke_by_signature<SignatureTag>(
                obj,
                std::forward<std::decay_t<std::tuple_element_t<indx, TupleArgs>>>(
                    std::get<indx>(tupleArgs))...);
        }

        // indx is 1 less than tuple size -> callable is the last argument
        template<typename SignatureTag, typename T, typename TupleArgs, size_t... indx>
        constexpr decltype(auto) invoke_default_call(T &&obj,
                                                     TupleArgs &&tupleArgs,
                                                     std::index_sequence<indx...>,
                                                     std::false_type /*is_complete<SignatureTag>*/)
        {
            static_assert(std::tuple_size_v<TupleArgs> >= 1, "Callable is not provided");
            static_assert(std::tuple_size_v<TupleArgs> - sizeof...(indx) == 1,
                          "Callable must be the last argument!");

            constexpr auto callable_index = sizeof...(indx);
            using callable_type = std::tuple_element_t<callable_index, TupleArgs>;

            return std::forward<callable_type>(std::get<callable_index>(tupleArgs))(
                obj,
                std::forward<std::decay_t<std::tuple_element_t<indx, TupleArgs>>>(
                    std::get<indx>(tupleArgs))...);
        }

    }   // namespace detail

    template<template<typename...> class SignatureTag, typename T, typename... U, typename... ArgsT>
    constexpr decltype(auto) invoke(tag::default_call, T &&obj, ArgsT &&...args)
    {
        using signature_type = SignatureTag<std::decay_t<T>, U...>;

        return detail::invoke_default_call<signature_type>(
            obj,
            std::forward_as_tuple(std::forward<ArgsT>(args)...),
            std::make_index_sequence<sizeof...(ArgsT) - 1>(),   // do not index callable
            traits::is_complete<signature_type>());
    }

    template<template<typename...> class NameTag, typename T, typename... U, typename... ArgsT>
    constexpr decltype(auto) invoke(T &&obj, ArgsT &&...args)
    {
        using signature_type = NameTag<std::decay_t<T>, U...>;
        static_assert(traits::is_complete<signature_type>(), "Signature tag is not implemented!");

        return detail::invoke_by_signature<signature_type>(obj, std::forward<ArgsT>(args)...);
    }

    namespace detail
    {
        template<typename NameTag, typename T>
        constexpr decltype(auto) get_tuple_element(T &&tuple, std::true_type /*is_tuple*/)
        {
            using found_type = eld::find_type<NameTag>;
            static_assert(!std::is_same_v<found_type, not_found>,
                          "NameTag does not have a type assigned to it");

            return std::get<found_type>(tuple);
        }

        template<typename NameTag, typename T>
        constexpr decltype(auto) get_tuple_element(T &&obj, std::false_type /*is_tuple*/)
        {
            static_assert(traits::is_tuple<T>(), "No default getter has been specified!");
        }

        template<typename NameTag, typename T>
        constexpr decltype(auto) get_invoke_name_tag(T &&obj,
                                                     std::true_type /*is_complete<NameTag>*/)
        {
            return NameTag()(obj);
        }

        template<typename NameTag, typename T>
        constexpr decltype(auto) get_invoke_name_tag(T &&obj,
                                                     std::false_type /*is_complete<NameTag>*/)
        {
            // TODO: implement
        }

        template<typename NameTag, typename T, typename Callable>
        constexpr decltype(auto) get_invoke_name_tag(tag::default_call,
                                                     T &&obj,
                                                     Callable &&,
                                                     std::true_type /*is_complete<NameTag>*/)
        {
            return NameTag()(obj);
        }

        template<typename NameTag, typename T, typename Callable>
        constexpr decltype(auto) get_invoke_name_tag(tag::default_call,
                                                     T &&obj,
                                                     Callable &&callable,
                                                     std::false_type /*is_complete<NameTag>*/)
        {
            return std::forward<Callable>(callable)(obj);
        }

    }   // namespace detail

    template<template<typename...> class NameTag, typename... U, typename T>
    constexpr decltype(auto) get(T &&obj)
    {
        using name_tag_t = NameTag<std::decay_t<T>, U...>;
        return detail::get_invoke_name_tag<name_tag_t>(obj, traits::is_complete<name_tag_t>());
    }

    template<template<typename...> class NameTag, typename... U, typename T, typename Callable>
    constexpr decltype(auto) get(T &&obj, Callable &&defaultCall)
    {
        using name_tag_t = NameTag<std::decay_t<T>, U...>;
        return detail::get_invoke_name_tag<name_tag_t>(default_call_tag,
                                                       obj,
                                                       std::forward<Callable>(defaultCall),
                                                       traits::is_complete<name_tag_t>());
    }

}   // namespace eld
