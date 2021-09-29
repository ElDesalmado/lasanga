#pragma once

#include "lasanga/generic/resolve_generic_class.h"
#include "lasanga/tags.h"
#include "lasanga/utility.h"
#include "lasanga/utility/traits.h"

#include <type_traits>
#include <utility>

namespace eld
{

    /**
     * Generic Builder class.
     * \details Building:
     * One tag type (build, name, d_name) specialization allows construction of only one type of
     * object.
     * - using build_t: will build an object of typename
     * - using alias_t and alias_tt: will build an object that is registered with the given alias
     * - using d_name_t and d_name_tt: will try to build an object using a dependent name. If no
     * dependent name is registered, will fall back to alias_t or alias_tt
     *
     * Type tree for specialization:
     * Given a root template class, builder will use root's name_list to recursively create a type
     * tree that can be used to fully specialize the given template class.
     * @tparam DesignatedFactories
     */
    template<typename... DesignatedFactories>
    class builder
    {
    public:
        using factories_list = util::type_list<DesignatedFactories...>;

        template<typename... DFactoriesT>
        constexpr explicit builder(DFactoriesT &&...designatedFactories)
          : designatedFactories_(std::forward<DFactoriesT>(designatedFactories)...)
        {
        }

        /**
         * Build using build_t tag.
         * \details First will try to find a unique unnamed designated factory.
         * Then will try to find a unique named designated factory.
         * If other than one factories are suitable, will throw a compile time error.
         * @tparam T
         * @return
         */
        template<typename T>
        decltype(auto) operator()(eld::build_t<T> buildTag)
        {
            auto mappedTuple =
                util::map_tuple<util::wrapped_predicate<util::traits::is_unnamed>,
                                util::wrapped_predicate<same_type, T>>(designatedFactories_);

            return construct(buildTag, mappedTuple);
        }

        template<typename NameTagT>
        decltype(auto) operator()(eld::alias_t<NameTagT>)
        {
            auto mappedTuple = util::map_tuple<util::wrapped_predicate<util::traits::is_named_factory>,
                                               util::wrapped_predicate<same_name_tag, NameTagT>>(
                designatedFactories_);
            return construct(mappedTuple);
        }

        /**
         * Build using template NameTag
         * @tparam TNameTagT
         * @return
         */
        template<template<typename...> class TNameTagT>
        decltype(auto) operator()(eld::alias_tt<TNameTagT>)
        {
            return (*this)(eld::alias_t<eld::wrapped_tt<TNameTagT>>());
        }

        /**
         * Build using dependent name tag. Will first try to find a designated factory with
         * requested DependsOnT. Will fall back to alias_t if not found.
         * @tparam DependsOnT
         * @tparam NameTagT
         * @tparam ...
         * @param dNameTag
         * @return
         */
        template<typename NameTagT, typename DependsOnT, typename...>
        decltype(auto) operator()(eld::d_name_t<NameTagT, DependsOnT> dNameTag)
        {
            auto mappedTupleFactories =
                util::map_tuple<util::wrapped_predicate<util::traits::is_dependent>,
                                util::wrapped_predicate<same_depends_on, DependsOnT>>(
                    designatedFactories_);

            return construct(dNameTag, mappedTupleFactories);
        }

        template<template<typename...> class TNameTagT, typename DependsOnT, typename... Modifiers>
        decltype(auto) operator()(eld::d_name_tt<TNameTagT, DependsOnT, Modifiers...>)
        {
            // TODO: this is invalid! (?)
            return (*this)(eld::d_name_t<eld::wrapped_tt<TNameTagT>, DependsOnT, Modifiers...>());
        }

        template<typename NameTagT, template<typename...> class TDependsOnT, typename... Modifiers>
        decltype(auto) operator()(eld::d_alias_t<NameTagT, TDependsOnT, Modifiers...>)
        {
            return (*this)(eld::d_name_t<NameTagT, eld::wrapped_tt<TDependsOnT>, Modifiers...>());
        }

        template<template<typename...> class TNameTagT,
                 template<typename...>
                 class TDependsOnT,
                 typename... Modifiers>
        decltype(auto) operator()(eld::d_alias_tt<TNameTagT, TDependsOnT, Modifiers...>)
        {
            return (*this)(
                eld::d_name_t<eld::wrapped_tt<TNameTagT>, eld::wrapped_tt<TDependsOnT>, Modifiers...>());
        }

    private:
        template<typename FactoryT, typename NameTag>
        using same_name_tag = std::is_same<util::traits::name_tag<FactoryT>, NameTag>;

        template<typename FactoryT, typename Type>
        using same_type = std::is_same<traits::value_type<FactoryT>, Type>;

        template<typename FactoryT, typename DependsOnT>
        using same_depends_on = std::is_same<util::traits::depends_on_type<FactoryT>, DependsOnT>;

        template<typename T, typename... Factories>
        decltype(auto) construct(eld::build_t<T>, std::tuple<Factories &...> tupleFactories)
        {
            return construct(tupleFactories);
        }

        template<typename T>
        decltype(auto) construct(eld::build_t<T>, std::tuple<>)
        {
            auto mappedTuple =
                util::map_tuple<util::wrapped_predicate<util::traits::is_named_factory>,
                                util::wrapped_predicate<same_type, T>>(designatedFactories_);
            return construct(mappedTuple);
        }

        template<typename NameTagT, typename DependsOnT, typename... FactoriesT>
        decltype(auto) construct(eld::d_name_t<NameTagT, DependsOnT>,
                                 std::tuple<FactoriesT &...> tupleFactories)
        {
            return construct(tupleFactories);
        }

        template<typename NameTagT, typename DependsOnT>
        decltype(auto) construct(eld::d_name_t<NameTagT, DependsOnT>, std::tuple<>)
        {
            return (*this)(eld::alias_t<NameTagT>());
        }

        template<typename... FoundFactories>
        decltype(auto) construct(std::tuple<FoundFactories &...> factories)
        {
            constexpr size_t tupleSize = std::tuple_size_v<decltype(factories)>;
            static_assert(tupleSize != 0, "No designated factories have been found.");
            static_assert(
                tupleSize <= 1,
                "Can't resolve designated factory. Several suitable factories were registered");

            return std::get<0>(factories)();
        }

    private:
        std::tuple<DesignatedFactories...> designatedFactories_;
    };

    template<typename... DesignatedFactories>
    constexpr auto make_builder(DesignatedFactories &&...factories)
    {
        // TODO: combine named with unnamed if type is the same?
        return builder<std::decay_t<DesignatedFactories>...>(
            std::forward<DesignatedFactories>(factories)...);
    }

}   // namespace eld
