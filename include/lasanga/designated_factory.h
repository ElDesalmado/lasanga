#pragma once

#include "lasanga/tags.h"
#include "lasanga/utility/traits.h"

#include <type_traits>
#include <utility>

namespace eld
{
    // TODO: designated factory with multiple callables (unique types)?
    template<typename CallableT,
             typename ValueTypeT,
             typename AliasTagT = eld::tag::unnamed,
             typename DependsOnT = eld::tag::unnamed>
    class designated_factory
    {
    public:
        // TODO: clarify factory traits
        using value_type = ValueTypeT;
        using type = value_type;
        using alias_tag = AliasTagT;

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

        template<typename... ArgsT>
        decltype(auto) operator()(ArgsT &&...args)
        {
            return factory_(std::forward<ArgsT>(args)...);
        }

    private:
        CallableT factory_;
    };

    /*
     * TODO: designated_factory for unspecialized generic class template
     *      - new class or add functionality?
     *      - value_type = wrapped_tt<TGenericClassT>
     *      - must specialized class be builder-aware?
     */

    template<typename Callable>
    constexpr auto wrap_factory(Callable &&callable)
    {
        using callable_type = std::decay_t<Callable>;
        using constructed_type = decltype(std::declval<callable_type>()());
        static_assert(!util::traits::is_tuple<constructed_type>(),
                      "Factories that construct tuples are not allowed. Use special overload and "
                      "explicitly specify constructed type");
        return designated_factory<std::decay_t<Callable>, constructed_type>(
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

    // TODO: only one signature to create named_factory? (remove d_named_factory)

    template<typename NameTagT, typename Callable>
    constexpr auto named_factory(Callable &&callable)
    {
        static_assert(!std::is_same_v<NameTagT, tag::unnamed>, "NamedTag must not be unnamed.");

        using callable_type = std::decay_t<Callable>;
        using constructed_type = decltype(std::declval<callable_type>()());
        static_assert(!util::traits::is_tuple<constructed_type>(),
                      "Factories that construct tuples are not allowed. Use special overload and "
                      "explicitly specify constructed type");
        return designated_factory<std::decay_t<Callable>, constructed_type, NameTagT>(
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
        static_assert(!util::traits::is_same_tt<NameTagT, tag::unnamed_t>::value,
                      "Template NamedTag must not be unspecified.");

        return named_factory<wrapped_tt<NameTagT>, std::decay_t<Callable>>(
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

    // TODO: return DependsOnT as a template (non-template) argument

    template<typename NameTagT, template<typename...> class TDependsOnT, typename Callable>
    constexpr auto d_named_factory(Callable &&callable)
    {
        static_assert(!std::is_same_v<NameTagT, tag::unnamed>, "NamedTag must not be unnamed.");

        using callable_type = std::decay_t<Callable>;
        using constructed_type = decltype(std::declval<callable_type>()());
        static_assert(!util::traits::is_tuple<constructed_type>(),
                      "Factories that construct tuples are not allowed. Use special overload and "
                      "explicitly specify constructed type");
        return designated_factory<callable_type, constructed_type, NameTagT, wrapped_tt<TDependsOnT>>(
            std::forward<Callable>(callable));
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
        return d_named_factory<wrapped_tt<TNameTagT>, TDependsOnT>(
            std::forward<Callable>(callable));
    }

    template<template<typename...> class TNameTagT, template<typename...>
                                                    class TDependsOnT, typename Callable>
    constexpr auto d_named_factory()
    {
        return d_named_factory<TNameTagT, TDependsOnT>(Callable());
    }

    template<template<typename...> class TNameTagT, template<typename...>
                                                    class TDependsOnT, typename ConstructedT>
    constexpr auto d_named_factory(ConstructedT (*pFunction)())
    {
        return d_named_factory<TNameTagT, TDependsOnT, std::decay_t<decltype(pFunction)>>(
            std::move(pFunction));
    }

}   // namespace eld
