
#include <type_traits>

namespace eld::util
{
    template<typename WrappedPredicateT, typename T>
    struct apply_predicate;

    template<template<typename...> class Predicate,
             typename T,
             typename... Modifiers,
             template<template<typename...> class, typename...>
             class PredicateWrapper>
    struct apply_predicate<PredicateWrapper<Predicate, Modifiers...>, T> :
      Predicate<T, Modifiers...>
    {
        // TODO: static_assert(is_valid_predicate) ??
    };

    template<typename T, typename... WrappedPredicates>
    struct apply_predicates
    {
        constexpr static auto value = std::conjunction_v<apply_predicate<WrappedPredicates, T>...>;
    };

    template<typename T, typename... WrappedPredicates>
    constexpr inline bool apply_predicates_v = apply_predicates<T, WrappedPredicates...>::value;

}   // namespace eld::util