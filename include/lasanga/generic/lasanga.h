#pragma once

#include "lasanga/generic/resolve_generic_class.h"
#include "lasanga/generic/specialize_tree.h"

#include <type_traits>

namespace eld::generic
{
    /**
     *
     * @tparam TGetAliasListTT meta function to get a list of aliases of TGenericClassT that are
     * used to fully specialize it
     * @tparam TResolveAliasTT
     * @tparam TGenericClassT
     * @tparam ModifiersT
     * @tparam BuilderT
     * @return
     */
    template<template<template<typename...> class, typename...> class TGetAliasListTT,
             template<typename...>
             class TGenericClassT,
             typename... ModifiersT,
             typename BuilderT>
    constexpr auto make_lasanga(BuilderT &&builder)
    {
        using builder_type = std::decay_t<BuilderT>;
        using resolved_type =
            typename resolve_generic_class<builder_type, TGenericClassT, ModifiersT...>::type;

        return typename specialize_generic_class<resolved_type>::type(builder);

        // TODO: encapsulate tree construction withing resolve_generic_class
        //        using alias_resolver_type = typename
        //        traits::get_alias_type_resolver<builder_type>::type;
        //
        //        using tree_type = typename construct_tree<TGetAliasListTT,
        //                                                  alias_resolver_type::template
        //                                                  resolve_type, TGenericClassT,
        //                                                  ModifiersT...>::type;
        //
        //        return typename util::specialize_tree<tree_type>::type(builder);
    }
}   // namespace eld::generic