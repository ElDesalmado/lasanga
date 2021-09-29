#pragma once

#include "lasanga/alias_list.h"
#include "lasanga/tags.h"
#include "lasanga/utility/construct_tree.h"
#include "lasanga/utility/resolve_factory.h"
#include "lasanga/utility/specialize_tree.h"

#include "lasanga/utility.h"

#include <tuple>

namespace eld::util
{
    namespace detail
    {
        template<template<typename...> class TGenericClassT, typename FactoriesList>
        struct resolve_alias_type;

        template<template<typename...> class TGenericClassT,
                 typename... DesignatedFactoriesT,
                 template<typename...>
                 class TListT>
        struct resolve_alias_type<TGenericClassT, TListT<DesignatedFactoriesT...>>
        {
            template<typename XAliasT,
                     template<typename...>
                     class XTGenericClassT,
                     typename... XModifiersT>
            using type = typename util::resolve_factory<XAliasT,
                                                        XTGenericClassT,
                                                        TListT<DesignatedFactoriesT...>,
                                                        TListT<XModifiersT...>>::type;
        };
    }   // namespace detail

    template<typename... DesignatedFactoriesT>
    class builder_impl
    {
    public:
        /**
         * Resolver template that uses a list of Designated factories.
         * @tparam TGenericClassT
         * @tparam ModifiersT
         */
        template<template<typename...> class TGenericClassT, typename... ModifiersT>
        struct resolve_generic_class
        {
            template<typename XAliasT,
                     template<typename...>
                     class XTGenericClassT,
                     typename... XModifiersT>
            using resolve_alias_type =
                typename detail::resolve_alias_type<TGenericClassT,
                                                    util::type_list<DesignatedFactoriesT...>>::
                    template type<XAliasT, XTGenericClassT, ModifiersT...>::type;

            using type_tree = typename util::construct_tree<eld::get_alias_list,
                                                            resolve_alias_type,
                                                            TGenericClassT,
                                                            ModifiersT...>::type;

            using type = typename util::specialize_tree<type_tree>::type;
        };

        constexpr explicit builder_impl(DesignatedFactoriesT &&...factories)
          : factories_(std::move(factories)...)
        {
        }

        template<typename AliasTagT,
                 template<typename...>
                 class TDependsOnT,
                 typename... Modifiers,
                 typename... ArgsT>
        decltype(auto) construct(d_alias_t<AliasTagT, TDependsOnT, Modifiers...>, ArgsT &&...args)
        {
            using resolved_factory =
                typename util::resolve_factory<AliasTagT,
                                               TDependsOnT,
                                               util::type_list<DesignatedFactoriesT...>,
                                               util::type_list<Modifiers...>>::type;

            // TODO: handle "factory" with wrapped generic class template as a value_type for deep
            //  composition

            auto &factory = std::get<resolved_factory>(factories_);
            return factory(std::forward<ArgsT>(args)...);
        }

    private:
        // all factories' types are unique
        std::tuple<DesignatedFactoriesT...> factories_;
    };
}   // namespace eld::util

#include "lasanga/generic/builder.h"

namespace eld
{
    template<typename... FactoriesT>
    struct specialize_builder_impl<util::builder_impl, FactoriesT...>
    {
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
        using builder_type = util::builder_impl<DesignatedFactoriesT...>;
        using type = typename builder_type::template resolve_generic_class<TGenericClassT,
                                                                           ModifiersT...>::type;
    };
}   // namespace eld::generic