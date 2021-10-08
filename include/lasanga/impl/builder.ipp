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

// resolve_generic_class specializations both for impl::builder and for
// generic::builder<impl::builder>
namespace eld::generic
{
    template<template<typename...> class TGenericClassT,
             typename... ModifiersT,
             typename... DesignatedFactoriesT>
    struct resolve_generic_class<impl::builder<DesignatedFactoriesT...>,
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

    template<template<typename...> class TGenericClassT,
             typename... ModifiersT,
             typename... DesignatedFactoriesT>
    struct resolve_generic_class<builder<impl::builder<DesignatedFactoriesT...>>,
                                 TGenericClassT,
                                 ModifiersT...> :
      resolve_generic_class<impl::builder<DesignatedFactoriesT...>, TGenericClassT, ModifiersT...>
    {
    };
}   // namespace eld::generic

namespace eld::impl
{
    // standard case when factory::type is resolved
    template<typename... DesignatedFactoriesT>
    template<typename FactoryT, typename>
    struct builder<DesignatedFactoriesT...>::construct
    {
        constexpr explicit construct(builder &b)   //
          : b(b)
        {
        }

        template<typename... ArgsT>
        decltype(auto) operator()(ArgsT &&...args)
        {
            auto &factory = std::get<FactoryT>(b.factories_);
            return factory(std::forward<ArgsT>(args)...);
        }

        builder &b;
    };

    // recursive specialization for multiple layers
    template<typename... DesignatedFactoriesT>
    template<typename FactoryT,
             template<typename...>
             class TGenericClassT,
             template<template<typename...> class>
             class TWrapperTT>
    struct builder<DesignatedFactoriesT...>::construct<FactoryT, TWrapperTT<TGenericClassT>>
    {
        constexpr explicit construct(builder &b)   //
          : b(b)
        {
        }

        template<typename... ArgsT>
        decltype(auto) operator()(ArgsT &&...args)
        {
            return generic::make_lasanga<generic::resolve_generic_class, TGenericClassT>(
                b,
                std::forward<ArgsT>(args)...);
        }

        builder &b;
    };

}   // namespace eld::impl
