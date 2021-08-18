#pragma once

#include "lasanga/traits.h"

#include <type_traits>
#include <utility>

namespace eld
{
    template<typename...>
    struct type_list;

    namespace detail
    {
        template<typename WrappedPredicateT, typename T>
        struct apply_predicate;

        template<template<typename...> class Predicate,
                 typename T,
                 typename... Modifiers,
                 template<template<typename...> class, typename...>
                 class PredicateWrapper>
        struct apply_predicate<PredicateWrapper<Predicate, Modifiers...>, T>
        {
            constexpr static auto value = Predicate<T, Modifiers...>::value;
        };

        template<typename T, typename... WrappedPredicates>
        struct apply_predicates
        {
            constexpr static auto value =
                std::conjunction_v<apply_predicate<WrappedPredicates, T>...>;
        };

        template<typename TupleT,
                 typename ListWrappedPredicatesT,
                 typename = decltype(std::make_index_sequence<std::tuple_size_v<TupleT>>())>
        struct map_tuple_index_sequence;

        template<template<typename...> class TupleT,
                 typename... Types,
                 template<typename...>
                 class TypeListT,
                 typename... WrappedPredicatesT,
                 size_t... Indx>
        struct map_tuple_index_sequence<TupleT<Types...>,
                                        TypeListT<WrappedPredicatesT...>,
                                        std::index_sequence<Indx...>>
        {
        private:
            using tuple_input = TupleT<Types...>;

            using tuple_indices = decltype(std::tuple_cat(
                std::conditional_t<apply_predicates<Types, WrappedPredicatesT...>::value,
                                   std::tuple<std::integral_constant<size_t, Indx>>,
                                   std::tuple<>>()...));

            template<typename TupleI>
            struct sequence_from_tuple;

            template<template<typename...> class TupleIC, size_t... IC>
            struct sequence_from_tuple<TupleIC<std::integral_constant<size_t, IC>...>>
            {
                using type = std::index_sequence<IC...>;
            };

        public:
            using type = typename sequence_from_tuple<tuple_indices>::type;
        };

        template<typename TupleT,
                 typename ListWrappedPredicatesT,
                 typename = typename map_tuple_index_sequence<TupleT, ListWrappedPredicatesT>::type>
        struct map_tuple;

        template<template<typename...> class TupleT,
                 typename... T,
                 typename ListWrappedPredicatesT,
                 size_t... Indx>
        struct map_tuple<TupleT<T...>, ListWrappedPredicatesT, std::index_sequence<Indx...>>
        {
            constexpr auto operator()(TupleT<T...> &tuple)
            {
                return TupleT<std::tuple_element_t<Indx, std::decay_t<decltype(tuple)>> &...>(
                    std::get<Indx>(tuple)...);
            }

            constexpr auto operator()(const TupleT<T...> &tuple)
            {
                return TupleT<std::tuple_element_t<Indx, std::decay_t<decltype(tuple)>> &...>(
                    std::get<Indx>(tuple)...);
            }
        };

    }   // namespace detail

    template<template<typename...> class, typename...>
    struct wrapped_predicate;

    template<typename... WrappedPredicates, typename TupleT>
    constexpr auto map_tuple(TupleT &&tuple)
    {
        return detail::map_tuple<std::decay_t<TupleT>, eld::type_list<WrappedPredicates...>>{}(
            tuple);
    }

    namespace traits
    {
        template<typename>
        struct type_list_size;

        template<typename... Types>
        struct type_list_size<type_list<Types...>>
        {
            constexpr static size_t value = sizeof...(Types);
        };

        template<size_t, typename>
        struct element_type;

        template<size_t Indx, typename... Types>
        struct element_type<Indx, type_list<Types...>>
        {
            using type = std::tuple_element_t<Indx, std::tuple<Types...>>;
        };

        // TODO: Should I allow this?
        template<size_t Indx>
        struct element_type<Indx, type_list<>>
        {
            using type = not_found_t;
        };

    }   // namespace traits

    /**
     * Wrapper type to store unspecialized template class (for example, within a type_list)
     */
    template<template<typename...> class>
    struct type_tt
    {
    };

    /**
     * Builder tag. Used to construct an object by NameTag
     */
    template<typename>
    struct name_t
    {
    };

    /**
     * Builder tag. Used to construct an object by unspecialized template NameTag. Template NameTag
     * can be used to overload a get<NameTag> function
     */
    template<template<typename...> class>
    struct name_tt
    {
    };

    /**
     * Helper function to deduce name_t or name_tt
     */
    template<typename NonT>
    constexpr name_t<NonT> name_tag()
    {
        return {};
    }

    /**
     * Helper function to deduce name_t or name_tt
     */
    template<template<typename...> class TT>
    constexpr name_tt<TT> name_tag()
    {
        return {};
    }

    /**
     * Build tag. Using DependentName to specialize an object to build by NameTag.
     * @tparam DependsOnT NameTag or a typename of a class that an object depends on.
     * @tparam NameTagT
     */
    template<typename NameTagT, typename DependsOnT, typename...>
    struct d_name_t
    {
    };

    template<template<typename...> class TNameTagT, typename DependsOnT, typename... Modifiers>
    struct d_name_tt
    {
    };

    /**
     * Builder tag. Used to construct an object within a GenericClass by NameTag and a set of
     * Modifiers
     * @tparam TDependsOnT Unspecialized template class that owns an object to be constructed.
     * @tparam NameTagT Tag designated a name for a type.
     * @tparam Modifiers A set of modifiers. Can be used to held in distinguishing between different
     * contexts and TDependsOnT'es
     */
    template<typename NameTagT, template<typename...> class TDependsOnT, typename... Modifiers>
    struct dt_name_t
    {
    };

    template<template<typename...> class TNameTagT,
             template<typename...>
             class TDependsOnT,
             typename... Modifiers>
    struct dt_name_tt
    {
    };

    /**
     * Helper function to resolve between typename and template DependentName
     * @tparam DependsOnT
     * @tparam NameTag
     * @tparam Modifiers
     * @return
     */
    template<typename NameTagT, typename DependsOnT, typename...>
    constexpr d_name_t<NameTagT, DependsOnT> d_name_tag()
    {
        return {};
    }

    template<template<typename...> class TNameTagT, typename DependsOnT, typename... Modifiers>
    constexpr d_name_tt<TNameTagT, DependsOnT, Modifiers...> d_name_tag()
    {
        return {};
    }

    /**
     * Helper function to resolve between typename and template DependentName
     * @tparam TDependsOnT
     * @tparam NameTagT
     * @tparam Modifiers
     * @return
     */
    template<typename NameTagT, template<typename...> class TDependsOnT, typename... Modifiers>
    constexpr dt_name_t<NameTagT, TDependsOnT, Modifiers...> d_name_tag()
    {
        return {};
    }

    template<template<typename...> class TNameTagT,
             template<typename...>
             class TDependsOnT,
             typename... Modifiers>
    constexpr dt_name_tt<TNameTagT, TDependsOnT, Modifiers...> d_name_tag()
    {
        return {};
    }

    /**
     * Build tag. Used to construct an object by its type.
     */
    template<typename>
    struct build_t
    {
    };

    /**
     * Build tag. Used to construct an object of a specialization (Spec) type for a class template
     * Type<Spec>
     * @tparam Type
     * @tparam Modifiers
     */
    template<template<typename...> class Type, typename... Modifiers>
    struct build_tt
    {
    };

    /**
     * Helper function to deduce name_t or name_tt
     */
    template<typename NonT>
    constexpr build_t<NonT> build_tag()
    {
        return {};
    }

    /**
     * Helper function to deduce name_t or name_tt
     */
    template<template<typename...> class TT>
    constexpr build_tt<TT> build_tag()
    {
        return {};
    }

    /**
     * Helper Mapping function to provide a list of names for a given unspecialized class template
     * GenericClass using a set of Modifiers.
     * When implementing a specialization, one needs to define a member typename `type =
     * type_list<...>`
     * @tparam GenericClass
     * @tparam Modifiers
     */
    template<template<typename...> class GenericClass, typename... Modifiers>
    struct get_name_list;

    namespace traits
    {
        template<typename /*BuilderT*/, typename /*NameList*/>
        struct get_type_list;

        template<typename BuilderT, typename... NameTags>
        struct get_type_list<BuilderT, type_list<NameTags...>>
        {
            using type = type_list<typename BuilderT::template type_by_name<NameTags>...>;
        };
    }   // namespace traits

    namespace detail
    {
        template<template<typename...> class /*GenericClass*/, typename /*TypeList*/>
        struct specialize_t;

        /**
         * Specialize an unspecialized class template GenericClass using a type_list of Types
         * @tparam GenericClass
         * @tparam Types Types to be used for specialization.
         * \todo: support template tree types?
         */
        template<template<typename...> class GenericClass, typename... Types>
        struct specialize_t<GenericClass, type_list<Types...>>
        {
            using type = GenericClass<Types...>;
        };
    }   // namespace detail

    /**
     * Create an object of a fully specialized class template GenericClass with specialization types
     * deduced from BuilderT using a user-defined specialization for `get_type_list<GenericClass,
     * Modifiers...>`.
     * @tparam GenericClass
     * @tparam GetNameList Customization for a type to get a name list from. Using get_name_list by
     * default
     * @tparam Modifiers
     * @tparam BuilderT
     * @param builder
     * @return
     *
     * \todo Try to deduce name list from GenericClass
     */
    template<template<typename...> class GenericClass,
             template<template<typename...> class, typename...> class GetNameList = get_name_list,
             typename... Modifiers,
             typename BuilderT>
    constexpr auto make_lasanga(BuilderT &&builder)
    {
        static_assert(traits::is_complete<GetNameList<GenericClass, Modifiers...>>::value,
                      "Name list has not been defined for GenericClass");

        using name_list = typename GetNameList<GenericClass, Modifiers...>::type;
        using type_list_t = typename traits::get_type_list<std::decay_t<BuilderT>, name_list>::type;

        return typename detail::specialize_t<GenericClass, type_list_t>::type(builder);
    }

    /**
     * Create an object of a class T using a Builder.
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

    namespace detail
    {
        template<typename NameTag>
        struct is_unnamed : std::is_same<NameTag, unnamed>
        {
        };

        template<template<typename...> class TNameTag>
        struct is_unnamed<type_tt<TNameTag>> : traits::is_same_tt<TNameTag, unnamed_tt>
        {
        };
    }   // namespace detail

    namespace traits
    {
        template<typename DFactoryT>
        using is_unnamed = eld::detail::is_unnamed<typename DFactoryT::name_tag>;

        template<typename DFactoryT>
        using is_named = std::negation<is_unnamed<DFactoryT>>;

        template<typename DFactoryT>
        using name_tag = typename DFactoryT::name_tag;

        template<typename DFactoryT>
        using value_type = typename DFactoryT::value_type;

        template<typename DFactoryT>
        using depends_on_type = typename DFactoryT::depends_on_type;

        template<typename DFactoryT>
        using is_dependent = std::negation<eld::detail::is_unnamed<depends_on_type<DFactoryT>>>;

    }   // namespace traits

    // TODO: remove/refactor this
    namespace detail
    {
        template<typename Tuple>
        struct type_list_from_tuple;

        template<typename... Types>
        struct type_list_from_tuple<std::tuple<Types...>>
        {
            using type = type_list<std::decay_t<Types>...>;
        };

        // TODO: add usage of GenericContextType to implementation
        template<typename NameTag, typename... DesignatedFactories>
        struct map_factories
        {
            using type = typename type_list_from_tuple<decltype(std::tuple_cat(
                std::declval<std::conditional_t<
                    std::is_same_v<NameTag, typename DesignatedFactories::name_tag>,
                    std::tuple<DesignatedFactories>,
                    std::tuple<>>>()...))>::type;
        };

        template<typename NameTag, typename... DesignatedFactories>
        struct get_type_by_name
        {
            static_assert(!std::is_same_v<NameTag, unnamed>, "NameTag must not be unnamed");
            using list = typename map_factories<NameTag, DesignatedFactories...>::type;

            static_assert(traits::type_list_size<list>::value <= 1, "Several NameTags found!");

            using type = typename traits::element_type<0, list>::type::value_type;
        };

        // implement search by NameTag
    }   // namespace detail

    /**
     * Generic Builder class.
     * \details Building:
     * One tag type (build, name, d_name) specialization allows construction of only one type of
     * object.
     * - using build_t: will build an object of typename
     * - using name_t and name_tt: will build an object that is registered with the given alias
     * - using d_name_t and d_name_tt: will try to build an object using a dependent name. If no
     * dependent name is registered, will fall back to name_t or name_tt
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
                map_tuple<wrapped_predicate<traits::is_unnamed>, wrapped_predicate<same_type, T>>(
                    designatedFactories_);

            return construct(buildTag, mappedTuple);
        }

        template<typename NameTagT>
        decltype(auto) operator()(eld::name_t<NameTagT>)
        {
            auto mappedTuple =
                map_tuple<wrapped_predicate<traits::is_named>,
                          wrapped_predicate<same_name_tag, NameTagT>>(designatedFactories_);
            return construct(mappedTuple);
        }

        /**
         * Build using template NameTag
         * @tparam TNameTagT
         * @return
         */
        template<template<typename...> class TNameTagT>
        decltype(auto) operator()(eld::name_tt<TNameTagT>)
        {
            return (*this)(eld::name_t<eld::type_tt<TNameTagT>>());
        }

        /**
         * Build using dependent name tag. Will first try to find a designated factory with
         * requested DependsOnT. Will fall back to name_t if not found.
         * @tparam DependsOnT
         * @tparam NameTagT
         * @tparam ...
         * @param dNameTag
         * @return
         */
        template<typename NameTagT, typename DependsOnT, typename...>
        decltype(auto) operator()(eld::d_name_t<NameTagT, DependsOnT> dNameTag)
        {
            auto mappedTuple =
                map_tuple<wrapped_predicate<traits::is_dependent>,
                          wrapped_predicate<same_depends_on, DependsOnT>>(designatedFactories_);

            return construct(dNameTag, mappedTuple);
        }

        template<template<typename...> class TNameTagT, typename DependsOnT, typename... Modifiers>
        decltype(auto) operator()(eld::d_name_tt<TNameTagT, DependsOnT, Modifiers...>)
        {
            // TODO: this is invalid! (?)
            return (*this)(eld::d_name_t<eld::type_tt<TNameTagT>, DependsOnT, Modifiers...>());
        }

        template<typename NameTagT, template<typename...> class TDependsOnT, typename... Modifiers>
        decltype(auto) operator()(eld::dt_name_t<NameTagT, TDependsOnT, Modifiers...>)
        {
            return (*this)(eld::d_name_t<NameTagT, eld::type_tt<TDependsOnT>, Modifiers...>());
        }

        template<template<typename...> class TNameTagT,
                 template<typename...>
                 class TDependsOnT,
                 typename... Modifiers>
        decltype(auto) operator()(eld::dt_name_tt<TNameTagT, TDependsOnT, Modifiers...>)
        {
            return (*this)(
                eld::d_name_t<eld::type_tt<TNameTagT>, eld::type_tt<TDependsOnT>, Modifiers...>());
        }

        template<typename NameTag>
        using type_by_name =
            typename detail::get_type_by_name<NameTag, DesignatedFactories...>::type;

    private:
        template<typename FactoryT, typename NameTag>
        using same_name_tag = std::is_same<traits::name_tag<FactoryT>, NameTag>;

        template<typename FactoryT, typename Type>
        using same_type = std::is_same<traits::value_type<FactoryT>, Type>;

        template<typename FactoryT, typename DependsOnT>
        using same_depends_on = std::is_same<traits::depends_on_type<FactoryT>, DependsOnT>;

        template<typename T, typename... Factories>
        decltype(auto) construct(eld::build_t<T>, std::tuple<Factories &...> tupleFactories)
        {
            return construct(tupleFactories);
        }

        template<typename T>
        decltype(auto) construct(eld::build_t<T>, std::tuple<>)
        {
            auto mappedTuple =
                map_tuple<wrapped_predicate<traits::is_named>, wrapped_predicate<same_type, T>>(
                    designatedFactories_);
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
            return (*this)(eld::name_t<NameTagT>());
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
