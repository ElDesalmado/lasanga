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
        // TODO: resolve factory

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

            /*
             * TODO: get a type from filtered list of factories
             *  filter: same Alias, DependsOnT, ModifiersT
             */

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
            auto &factory = [](ArgsT &&...) { return 42; };
        }

    private:
        std::tuple<DesignatedFactoriesT...> factories_;
    };
}   // namespace eld::util

// TODO: eld::make_builder for util::builder_impl

#include "lasanga/generic/builder.h"

namespace eld
{
    template<typename... FactoriesT>
    struct specialize_builder_impl<util::builder_impl, FactoriesT...>
    {
        // TODO: handle deduction of real factories list
        using type = util::builder_impl<FactoriesT...>;
    };
}   // namespace eld

#include "lasanga/generic/resolve_generic_class.h"

namespace eld::generic
{
    template<template<typename...> class TGenericClassT,
             typename... ModifiersT,
             typename... DesignatedFactoriesT>
    struct resolve_generic_class<builder<util::builder_impl<DesignatedFactoriesT...>>,
                                 TGenericClassT,
                                 ModifiersT...>
    {
        using builder_type = builder<util::builder_impl<DesignatedFactoriesT...>>;
        using type = typename builder_type::template resolve_generic_class<TGenericClassT,
                                                                           ModifiersT...>::type;
    };
}   // namespace eld::generic