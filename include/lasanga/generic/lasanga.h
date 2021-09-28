#pragma once

#include "lasanga/generic/resolve_generic_class.h"
#include "lasanga/generic/specialize_tree.h"

#include <type_traits>

namespace eld::generic
{
    /**
     * Creates an object of fully specialized TGenericClassT. Function provides template
     * functionality similar to deduction guides, that is its argument (Builder type) is used to
     * deduce template arguments.
     * @tparam TResolveGenericClassTT meta template to resolve unspecialized TGenericClassT
     * template's type using BuilderT type and a set of ModifiersT
     * @tparam TGenericClassT
     * @tparam ModifiersT
     * @tparam BuilderT
     * @return
     */
    template<template<typename /*BuilderT*/,
                      template<typename...>
                      class /*TGenericClassT*/,
                      typename... /*ModifiersT*/>
             class TResolveGenericClassTT,
             template<typename...>
             class TGenericClassT,
             typename... ModifiersT,
             typename BuilderT>
    constexpr auto make_lasanga(BuilderT &&builder)
    {
        using builder_type = std::decay_t<BuilderT>;
        using resolved_type =
            typename TResolveGenericClassTT<builder_type, TGenericClassT, ModifiersT...>::type;

        return typename specialize_generic_class<resolved_type>::type(builder);
    }
}   // namespace eld::generic