#pragma once

#include <type_traits>

namespace eld
{
    namespace detail
    {
        template<typename...>
        struct always_false : std::false_type
        {
        };
    }   // namespace detail

    /**
     * Helper Mapping function to provide a list of names for a given unspecialized class template
     * TGenericClassT using a set of ModifiersT.
     * When implementing a specialization, one needs to define a member typename `type =
     * type_list<...>`.
     *
     * Example for specialization:
     * template<>
       struct eld::get_alias_list<Crowd>
       {
           using type = util::type_list<alias::A, wrapped_tt<alias::B>, alias::C>;
       };
     * @tparam TGenericClassT unspecialized template Generic class template to be used as a key to
     * get a list of aliases
     * @tparam ModifiersT optional list of modifier types to specialize the search
     */
    template<template<typename...> class TGenericClassT, typename... ModifiersT>
    struct get_alias_list
    {
        static_assert(detail::always_false<ModifiersT...>(),
                      "Type list for template generic template class is not specialized.");
        using type = void;
    };

    template <typename...>
    struct alias_list;

}   // namespace eld
