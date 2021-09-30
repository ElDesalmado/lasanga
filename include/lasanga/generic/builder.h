#pragma once

#include "lasanga/tags.h"

#include <tuple>
#include <type_traits>
#include <utility>

namespace eld
{
    namespace generic
    {
        /**
         * Generic class template for builder's public API.
         * @tparam ImplT
         */
        template<typename ImplT>
        class builder
        {
        public:
            using implementation_type = ImplT;

            template<typename... ArgsT,
                     typename std::enable_if_t<sizeof...(ArgsT) ||
                                               std::is_default_constructible_v<implementation_type>>
                         * = nullptr>
            constexpr explicit builder(ArgsT &&...args)   //
              : impl_(std::forward<ArgsT>(args)...)
            {
            }

            template<typename NameTagT,
                     template <typename...> class TDependsOnT,
                     typename... Modifiers,
                     typename... ArgsT>
            decltype(auto) operator()(d_alias_t<NameTagT, TDependsOnT, Modifiers...> dNameTag,
                                      ArgsT &&...args)
            {
                return impl_(dNameTag, std::forward<ArgsT>(args)...);
            }

            template<template<typename...> class TAliasTagT,
                     template<typename...>
                     class TDependsOnT,
                     typename... ModifiersT,
                     typename... ArgsT>
            decltype(auto) operator()(eld::d_alias_tt<TAliasTagT, TDependsOnT, ModifiersT...>,
                                      ArgsT &&...args)
            {
                return (*this)(d_alias_t<wrapped_tt<TAliasTagT>, TDependsOnT, ModifiersT...>(),
                    std::forward<ArgsT>(args)...);
            }

        private:
            implementation_type impl_;
        };
    }   // namespace generic

    template<typename ImplT, typename... ArgsT>
    constexpr auto make_builder(ArgsT &&...args)
    {
        return generic::builder<ImplT>(std::forward<ArgsT>(args)...);
    }

    /**
     * Customization point to specialize TGenericImplT from a set of ArgsT types
     * @tparam TGenericImplT
     * @tparam ArgsT
     */
    template<template<typename...> class TGenericImplT, typename... ArgsT>
    struct specialize_builder_impl;

    template<template<typename...> class TImplT, typename... ArgsT>
    constexpr auto make_builder(ArgsT &&...args)
    {
        using impl_type = typename specialize_builder_impl<TImplT, ArgsT...>::type;
        return make_builder<impl_type>(std::forward<ArgsT>(args)...);
    }

}   // namespace eld