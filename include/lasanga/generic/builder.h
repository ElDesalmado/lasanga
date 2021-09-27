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

            template<template<typename...> class TGenericClassT, typename... ModifiersT>
            struct resolve_generic_class
            {
                using type = typename implementation_type::
                    template resolve_generic_class<TGenericClassT, ModifiersT...>::type;
            };

            template<typename NameTagT,
                     typename DependsOnT,
                     typename... Modifiers,
                     typename... ArgsT>
            decltype(auto) operator()(d_name_t<NameTagT, DependsOnT, Modifiers...> dNameTag,
                                      ArgsT &&...args)
            {
                return impl_.construct(dNameTag, std::forward<ArgsT>(args)...);
            }

            template<template<typename...> class TAliasTagT,
                     typename DependsOnT,
                     typename... Modifiers,
                     typename... ArgsT>
            decltype(auto) operator()(eld::d_name_tt<TAliasTagT, DependsOnT, Modifiers...>,
                                      ArgsT &&...args)
            {
                return (*this)(d_name_t<wrapped_tt<TAliasTagT>, DependsOnT, Modifiers...>(),
                               std::forward<ArgsT>(args)...);
            }

            template<typename AliasTagT,
                     template<typename...>
                     class TDependsOnT,
                     typename... ModifiersT,
                     typename... ArgsT>
            decltype(auto) operator()(eld::dt_name_t<AliasTagT, TDependsOnT, ModifiersT...>,
                                      ArgsT &&...args)
            {
                return (*this)(d_name_t<AliasTagT, wrapped_tt<TDependsOnT>, ModifiersT...>(),
                               std::forward<ArgsT>(args)...);
            }

            template<template<typename...> class TAliasTagT,
                     template<typename...>
                     class TDependsOnT,
                     typename... ModifiersT,
                     typename... ArgsT>
            decltype(auto) operator()(eld::dt_name_tt<TAliasTagT, TDependsOnT, ModifiersT...>,
                                      ArgsT &&...args)
            {
                return (*this)(
                    d_name_t<wrapped_tt<TAliasTagT>, wrapped_tt<TDependsOnT>, ModifiersT...>(),
                    std::forward<ArgsT>(args)...);
            }

        private:
            implementation_type impl_;
        };
    }   // namespace generic

    template <typename ImplT, typename ... ArgsT>
    constexpr auto make_builder(ArgsT &&... args)
    {
        return generic::builder<ImplT>(std::forward<ArgsT>(args)...);
    }

}   // namespace eld