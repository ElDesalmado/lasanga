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
    // recursive specialization for multiple layers
    template<typename... DesignatedFactoriesT>
    template<typename FactoryT,
             template<typename...>
             class TGenericClassT,
             template<template<typename...> class>
             class TWrapperTT>
    struct builder<DesignatedFactoriesT...>::construct<FactoryT, TWrapperTT<TGenericClassT>>
    {
        construct(builder &b) : b(b) {}

        template<typename... ArgsT>
        decltype(auto) operator()(ArgsT &&...args)
        {
            return generic::make_lasanga<generic::resolve_generic_class, TGenericClassT>(
                b,
                std::forward<ArgsT>(args)...);
        }

        builder &b;
    };

    namespace detail
    {
        template<typename AT, typename BT>
        struct same_wrapped_tt : std::is_same<AT, BT>
        {
        };

        template<template<typename...> class TAT,
                 template<typename...>
                 class TBT,
                 template<template<typename...> class>
                 class TWrapperATT,
                 template<template<typename...> class>
                 class TWrapperBTT>
        struct same_wrapped_tt<TWrapperATT<TAT>, TWrapperBTT<TBT>> :
          util::traits::is_same_tt<TAT, TBT>
        {
        };
    }   // namespace detail

    template<typename... DesignatedFactoriesT>
    template<typename AliasT>
    struct builder<DesignatedFactoriesT...>::factory_by_alias
    {
        template<typename FactoryT>
        using same_alias = detail::same_wrapped_tt<AliasT, typename FactoryT::alias_tag>;

        template<typename FactoryT>
        using is_independent = std::is_same<eld::tag::unnamed, typename FactoryT::depends_on_type>;

        template<typename FactoryT>
        using is_dependent = std::bool_constant<!is_independent<FactoryT>::value>;

        template<typename TypeListT>
        struct get_factory;

        template<typename DesignatedFactoryT, template<typename...> class TTypeListT>
        struct get_factory<TTypeListT<DesignatedFactoryT>>
        {
            decltype(auto) operator()(std::tuple<DesignatedFactoriesT...> &factories)
            {
                return std::get<DesignatedFactoryT>(factories);
            }
        };

        template<typename... XDesignatedFactoriesT, template<typename...> class TTypeListT>
        struct get_factory<TTypeListT<XDesignatedFactoriesT...>>
        {
            decltype(auto) operator()(std::tuple<DesignatedFactoriesT...> &factories)
            {
                using filtered_dependent = typename util::filter_type_list_conjunction<
                    util::type_list<DesignatedFactoriesT...>,
                    same_alias,
                    is_dependent>::type;
                static_assert(util::traits::type_list_size<filtered_dependent>::value <= 1,
                              "Multiple dependent designated factories found");
                static_assert(util::traits::type_list_size<filtered_dependent>::value != 0,
                              "No designated factories found");

                return std::get<typename util::traits::element_type<0, filtered_dependent>::type>(
                    factories);
            }
        };

        decltype(auto) operator()(std::tuple<DesignatedFactoriesT...> &factories)
        {
            using filtered_independent = typename util::filter_type_list_conjunction<
                util::type_list<DesignatedFactoriesT...>,
                same_alias,
                is_independent>::type;

            static_assert(util::traits::type_list_size<filtered_independent>::value <= 1,
                          "Multiple independent designated factories found");

            return get_factory<filtered_independent>{}(factories);
        }
    };
}   // namespace eld::impl
