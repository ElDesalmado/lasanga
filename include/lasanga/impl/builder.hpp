#pragma once

#include "lasanga/get_alias_list.h"
#include "lasanga/impl/resolve_generic_class.h"
#include "lasanga/utility/resolve_factory.h"

#include "lasanga/utility.h"

#include <tuple>
#include <utility>

/**
 * This is a default implementation for the generic::builder.
 */

namespace eld::impl
{
    template<typename... DesignatedFactoriesT>
    class builder
    {
    public:
        using factories_list = util::type_list<DesignatedFactoriesT...>;

        template<typename... XDesignatedFactoriesT>
        constexpr explicit builder(XDesignatedFactoriesT &&...factories)
          : factories_(std::forward<XDesignatedFactoriesT>(factories)...)
        {
        }

        template<typename AliasTagT,
                 template<typename...>
                 class TDependsOnT,
                 typename... Modifiers,
                 typename... ArgsT>
        decltype(auto) operator()(d_alias_t<AliasTagT, TDependsOnT, Modifiers...>, ArgsT &&...args)
        {
            using resolved_factory =
                typename util::resolve_factory<AliasTagT,
                                               wrapped_tt<TDependsOnT>,
                                               factories_list,
                                               util::type_list<Modifiers...>>::type;

            return construct<resolved_factory>(*this)(std::forward<ArgsT>(args)...);
        }

        template<template<typename...> class TAliasTagT,
                 template<typename...>
                 class TDependsOnT,
                 typename... ModifiersT,
                 typename... ArgsT>
        decltype(auto) operator()(eld::d_alias_tt<TAliasTagT, TDependsOnT, ModifiersT...>,
                                  ArgsT &&...args)
        {
            return (*this)(eld::d_alias_tag<wrapped_tt<TAliasTagT>, TDependsOnT, ModifiersT...>(),
                           std::forward<ArgsT>(args)...);
        }

        template<typename AliasTagT, typename... ModifiersT, typename... ArgsT>
        decltype(auto) operator()(alias_t<AliasTagT>, ArgsT &&...args)
        {
            using resolved_factory =
                typename util::resolve_factory<AliasTagT,
                                               tag::unnamed,
                                               factories_list,
                                               util::type_list<ModifiersT...>>::type;

            return construct<resolved_factory>(*this)(std::forward<ArgsT>(args)...);
        }

        template<template<typename...> class TAliasTagT, typename... ArgsT>
        decltype(auto) operator()(eld::alias_tt<TAliasTagT>, ArgsT &&...args)
        {
            return (*this)(alias_t<wrapped_tt<TAliasTagT>>(), std::forward<ArgsT>(args)...);
        }

    private:
        template<typename FactoryT, typename = typename std::decay_t<FactoryT>::type>
        struct construct;

    private:
        std::tuple<DesignatedFactoriesT...> factories_;
    };

}   // namespace eld::impl

#include "lasanga/impl/builder.ipp"