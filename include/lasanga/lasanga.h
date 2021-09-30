#pragma once

// generic headers
#include "lasanga/generic/builder.h"
#include "lasanga/generic/lasanga.h"
#include "lasanga/get_alias_list.h"
#include "lasanga/designated_factory.h"
#include "lasanga/tags.h"

// default implementations
#include "lasanga/impl/builder.h"

#include "lasanga/utility.h"


namespace eld
{
    /**
     * Create an object of a class TGenericClassT using a Builder.
     * @tparam T
     * @tparam Builder
     * @param builder
     * @return
     */
    template<typename T, typename Builder>
    constexpr auto make_lasanga(Builder &&builder)
    {
        return T(builder);
    }

    template<template<typename...> class TGenericClassT, typename... ModifiersT, typename BuilderT>
    constexpr auto make_lasanga(BuilderT &&builder)
    {
        return generic::make_lasanga<generic::resolve_generic_class, TGenericClassT, ModifiersT...>(
            builder);
    }

    template <typename ... FactoriesT>
    constexpr auto make_default_builder(FactoriesT &&... factories)
    {
        return make_builder<impl::builder>(std::forward<FactoriesT>(factories)...);
    }

}   // namespace eld