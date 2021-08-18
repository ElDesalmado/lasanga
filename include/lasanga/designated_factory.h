#pragma once

#include "lasanga/traits.h"

namespace eld
{
    // TODO: designated factory with multiple callable types?
    template<typename CallableT,
             typename ValueTypeT,
             typename NameTagT = eld::tag::unnamed,
             typename DependsOnT = eld::tag::unnamed>
    class designated_factory
    {
    public:
        using value_type = ValueTypeT;
        using name_tag = NameTagT;
        using depends_on_type = DependsOnT;

        template<
            bool DefaultConstructible = std::is_destructible_v<CallableT>,
            typename... ArgsT,
            typename std::enable_if_t<sizeof...(ArgsT) || DefaultConstructible, int> * = nullptr>
        constexpr explicit designated_factory(ArgsT &&...argsT)   //
          : factory_(std::forward<ArgsT>(argsT)...)
        {
        }

        template<bool MoveConstructible = std::is_move_constructible_v<CallableT>,
                 typename std::enable_if_t<MoveConstructible, int> * = nullptr>
        explicit designated_factory(designated_factory &&other) noexcept   //
          : factory_(std::move(other.factory_))
        {
        }

        decltype(auto) operator()() { return factory_(); }

    private:
        CallableT factory_;
    };

    template<typename Callable>
    constexpr auto wrap_factory(Callable &&callable)
    {
        using callable_type = std::decay_t<Callable>;
        using constructed_type = decltype(std::declval<callable_type>()());
        static_assert(!traits::is_tuple<constructed_type>(),
                      "Factories that construct tuples are not allowed. Use special overload and "
                      "explicitly specify constructed type");
        return eld::designated_factory<std::decay_t<Callable>, constructed_type>(
            std::forward<Callable>(callable));
    }

    template<typename Callable>
    constexpr auto wrap_factory()
    {
        return wrap_factory<Callable>(Callable());
    }

    template<typename Constructed>
    constexpr auto wrap_factory(Constructed (*fPtr)())
    {
        return wrap_factory<std::decay_t<decltype(fPtr)>>(std::move(fPtr));
    }

    template<typename NameTag, typename Callable>
    constexpr auto named_factory(Callable &&callable)
    {
        static_assert(!std::is_same_v<NameTag, tag::unnamed>, "NamedTag must not be unnamed.");

        using callable_type = std::decay_t<Callable>;
        using constructed_type = decltype(std::declval<callable_type>()());
        static_assert(!traits::is_tuple<constructed_type>(),
                      "Factories that construct tuples are not allowed. Use special overload and "
                      "explicitly specify constructed type");
        return eld::designated_factory<std::decay_t<Callable>, constructed_type, NameTag>(
            std::forward<Callable>(callable));
    }

    template<typename NameTag, typename Callable>
    constexpr auto named_factory()
    {
        return named_factory<NameTag>(Callable());
    }

    template<template<typename...> class NameTagT, typename Callable>
    constexpr auto named_factory(Callable &&callable)
    {
        static_assert(!traits::is_same_tt<NameTagT, tag::unnamed_t>::value,
                      "Template NamedTag must not be unspecified.");

        return named_factory<type_tt<NameTagT>, std::decay_t<Callable>>(
            std::forward<Callable>(callable));
    }

    template<template<typename...> class NameTagT, typename Callable>
    constexpr auto named_factory()
    {
        return named_factory<NameTagT>(Callable());
    }

    template<typename NameTag, typename Constructed>
    constexpr auto named_factory(Constructed (*fPtr)())
    {
        return named_factory<NameTag, std::decay_t<decltype(fPtr)>>(std::move(fPtr));
    }

    template<template<typename...> class NameTagT, typename Constructed>
    constexpr auto named_factory(Constructed (*fPtr)())
    {
        return named_factory<NameTagT, std::decay_t<decltype(fPtr)>>(std::move(fPtr));
    }

    template<typename NameTagT, typename DependsOnT, typename Callable>
    constexpr auto d_named_factory(Callable &&callable)
    {
        static_assert(!std::is_same_v<NameTagT, tag::unnamed>, "NamedTag must not be unnamed.");
        // TODO: check DependOnT is valid

        using callable_type = std::decay_t<Callable>;
        using constructed_type = decltype(std::declval<callable_type>()());
        static_assert(!traits::is_tuple<constructed_type>(),
                      "Factories that construct tuples are not allowed. Use special overload and "
                      "explicitly specify constructed type");
        return eld::designated_factory<callable_type, constructed_type, NameTagT, DependsOnT>(
            std::forward<Callable>(callable));
    }

    template<typename NameTagT, typename DependsOnT, typename Callable>
    constexpr auto d_named_factory()
    {
        return d_named_factory<NameTagT, DependsOnT>(Callable());
    }

    template<typename NameTagT, typename DependsOnT, typename ConstructedT>
    constexpr auto d_named_factory(ConstructedT (*pFunction)())
    {
        return d_named_factory<NameTagT, DependsOnT, std::decay_t<decltype(pFunction)>>(
            std::move(pFunction));
    }

    template<template<typename...> class TNameTagT, typename DependsOnT, typename Callable>
    constexpr auto d_named_factory(Callable &&callable)
    {
        return d_named_factory<type_tt<TNameTagT>, DependsOnT>(std::forward<Callable>(callable));
    }

    template<template<typename...> class TNameTagT, typename DependsOnT, typename Callable>
    constexpr auto d_named_factory()
    {
        return d_named_factory<TNameTagT, DependsOnT>(Callable());
    }

    template<template<typename...> class TNameTagT, typename DependsOnT, typename ConstructedT>
    constexpr auto d_named_factory(ConstructedT (*pFunction)())
    {
        return d_named_factory<TNameTagT, DependsOnT, std::decay_t<decltype(pFunction)>>(
            std::move(pFunction));
    }

    template<typename NameTagT, template<typename...> class TDependsOnT, typename Callable>
    constexpr auto d_named_factory(Callable &&callable)
    {
        return d_named_factory<NameTagT, type_tt<TDependsOnT>>(std::forward<Callable>(callable));
    }

    template<typename NameTagT, template<typename...> class TDependsOnT, typename Callable>
    constexpr auto d_named_factory()
    {
        return d_named_factory<NameTagT, TDependsOnT>(Callable());
    }

    template<typename NameTagT, template<typename...> class TDependsOnT, typename ConstructedT>
    constexpr auto d_named_factory(ConstructedT (*pFunction)())
    {
        return d_named_factory<NameTagT, TDependsOnT, std::decay_t<decltype(pFunction)>>(
            std::move(pFunction));
    }

    template<template<typename...> class TNameTagT,
             template<typename...>
             class TDependsOnT,
             typename Callable>
    constexpr auto d_named_factory(Callable &&callable)
    {
        return d_named_factory<type_tt<TNameTagT>, type_tt<TDependsOnT>>(
            std::forward<Callable>(callable));
    }

    template<template<typename...> class TNameTagT,
             template<typename...>
             class TDependsOnT,
             typename Callable>
    constexpr auto d_named_factory()
    {
        return d_named_factory<TNameTagT, TDependsOnT>(Callable());
    }

    template<template<typename...> class TNameTagT,
             template<typename...>
             class TDependsOnT,
             typename ConstructedT>
    constexpr auto d_named_factory(ConstructedT (*pFunction)())
    {
        return d_named_factory<TNameTagT, TDependsOnT, std::decay_t<decltype(pFunction)>>(
            std::move(pFunction));
    }

}   // namespace eld
