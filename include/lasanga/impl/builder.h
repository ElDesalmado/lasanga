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

            // TODO: handle "factory" with wrapped generic class template as a value_type for deep
            //  composition

            auto &factory = std::get<resolved_factory>(factories_);
            return factory(std::forward<ArgsT>(args)...);
        }

    private:
        std::tuple<DesignatedFactoriesT...> factories_;
    };

}   // namespace eld::impl

#include "lasanga/generic/builder.h"

namespace eld
{
    template<typename... FactoriesT>
    struct specialize_builder_impl<impl::builder, FactoriesT...>
    {
        using type = impl::builder<FactoriesT...>;
    };
}   // namespace eld

#include "lasanga/generic/resolve_generic_class.h"

namespace eld::generic
{
    template<template<typename...> class TGenericClassT,
             typename... ModifiersT,
             typename... DesignatedFactoriesT>
    struct resolve_generic_class<builder<impl::builder<DesignatedFactoriesT...>>,
                                 TGenericClassT,
                                 ModifiersT...>
    {
        // adapter to resolve from factories
        template<typename XAliasT, template<typename...> class XTGenericClassT>
        using resolve_alias_type =
            typename eld::util::resolve_factory<XAliasT,
                                                wrapped_tt<XTGenericClassT>,
                                                util::type_list<DesignatedFactoriesT...>,
                                                util::type_list<ModifiersT...>>::type;

        using type = typename eld::impl::
            resolve_generic_class<TGenericClassT, eld::get_alias_list, resolve_alias_type>::type;
    };
}   // namespace eld::generic