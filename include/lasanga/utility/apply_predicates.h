#pragma once

#include <type_traits>

namespace eld::util
{
    /**
     * Apply Wrapped Predicate to typename TGenericClassT
     * @tparam T
     * @tparam WrappedPredicateT
     */
    template<typename T, typename WrappedPredicateT>
    struct apply_predicate;

    /**
     * Apply TPredicateT to typename TGenericClassT. TPredicateT is instantiated with TGenericClassT as a first template
     * parameter, and ModifiersT as trailing template parameters.
     * @tparam T
     * @tparam TPredicateT
     * @tparam ModifiersT
     * @tparam TTPredicateWrapperT
     */
    template<typename T,
             template<typename...>
             class TPredicateT,
             typename... ModifiersT,
             template<template<typename...> class, typename...>
             class TTPredicateWrapperT>
    struct apply_predicate<T, TTPredicateWrapperT<TPredicateT, ModifiersT...>> :
      TPredicateT<T, ModifiersT...>
    {
        // TODO: static_assert(is_valid_predicate) ??
    };

    /**
     * Applies several wrapped predicates to type TGenericClassT, applying TLogicalT function to results
     * @tparam TLogicalT
     * @tparam T
     * @tparam WrappedPredicates
     */
    template<template<typename...> class TLogicalT, typename T, typename... WrappedPredicates>
    struct apply_predicates : TLogicalT<apply_predicate<T, WrappedPredicates>...>
    {
    };

    /**
     * Applies several wrapped predicates to type TGenericClassT, applying conjunction function to results
     * @tparam T
     * @tparam WrappedPredicates
     */
    template<typename T, typename... WrappedPredicates>
    using apply_predicates_conjunction =
        apply_predicates<std::conjunction, T, WrappedPredicates...>;

    template<typename T, typename... WrappedPredicates>
    constexpr inline bool apply_predicates_conjunction_v =
        apply_predicates_conjunction<T, WrappedPredicates...>::value;

    /**
     * Applies several wrapped predicates to type TGenericClassT, applying disjunction function to results
     * @tparam T
     * @tparam WrappedPredicates
     */
    template<typename T, typename... WrappedPredicates>
    using apply_predicates_disjunction =
        apply_predicates<std::disjunction, T, WrappedPredicates...>;

    template<typename T, typename... WrappedPredicates>
    constexpr inline bool apply_predicates_disjunction_v =
        apply_predicates_disjunction<T, WrappedPredicates...>::value;

    template <template<typename...> class, typename...>
    struct wrapped_predicate;

}   // namespace eld::util