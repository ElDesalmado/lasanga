#pragma once

#include "lasanga/alias_list.h"
#include "lasanga/generic/construct_tree.h"
#include "lasanga/tags.h"

#include "lasanga/utility.h"

#include <tuple>

namespace eld::util
{
    namespace detail
    {
        template<template<typename...> class TGenericClassT,
                 typename ModifiersList,
                 typename FactoriesList>
        struct resolve_alias_type;

        template<template<typename...> class TGenericClassT,
                 typename... ModifiersT,
                 typename... DesignatedFactoriesT,
                 template<typename...>
                 class TListT>
        struct resolve_alias_type<TGenericClassT,
                                  TListT<ModifiersT...>,
                                  TListT<DesignatedFactoriesT...>>
        {
            struct stub_resolved
            {
                using type = void;
            };

            template<typename XAliasT,
                     template<typename...>
                     class XTGenericClassT,
                     typename... XModifiersT>
            using type = stub_resolved;   // TODO: implement
        };
    }   // namespace detail

    template<typename... DesignatedFactoriesT>
    class builder_impl
    {
    public:
        template<template<typename...> class TGenericClassT, typename... ModifiersT>
        struct resolve_generic_class
        {
            template<typename XAliasT,
                     template<typename...>
                     class XTGenericClassT,
                     typename... XModifiersT>
            using resolve_alias_type =
                typename detail::resolve_alias_type<TGenericClassT,
                                                    util::type_list<ModifiersT...>,
                                                    util::type_list<DesignatedFactoriesT...>>::
                    template type<XAliasT, XTGenericClassT, ModifiersT...>::type;

            // TODO: implement
            using type = typename eld::generic::construct_tree<eld::get_alias_list,
                                                               resolve_alias_type,
                                                               TGenericClassT,
                                                               ModifiersT...>::type;
        };

        constexpr explicit builder_impl(DesignatedFactoriesT &&...factories)
          : factories_(std::move(factories)...)
        {
            // TODO: specialize factories from generic branches
        }

        template<typename NameTagT, typename DependsOnT, typename... Modifiers, typename... ArgsT>
        decltype(auto) construct(d_name_t<NameTagT, DependsOnT, Modifiers...>, ArgsT &&...args)
        {
            // TODO: implement
            // TODO: find factory (filter by NameTagT, DependsOnT and is invocable with ArgsT)
            auto &factory = [](ArgsT && ...)
            {
                return 42;
            };
        }

    private:
        std::tuple<DesignatedFactoriesT...> factories_;
    };
}   // namespace eld::util

#include "lasanga/generic/builder.h"

namespace eld
{
    template<template<typename...> class, typename... ArgsT>
    constexpr auto make_builder(ArgsT &&...args);

}