#pragma once

#include "lasanga/traits.h"

#include <type_traits>
#include <utility>

namespace eld
{
    template<typename...>
    struct type_list;

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
     * Builder tag. Used to construct an object within a GenericClass by NameTag and a set of
     * Modifiers
     * @tparam GenericClass Unspecialized template class that owns an object to be constructed.
     * @tparam NameTag Tag designated a name for a type.
     * @tparam Modifiers A set of modifiers. Can be used to held in distinguishing between different
     * contexts and GenericClass'es
     */
    template<template<typename...> class GenericClass, typename NameTag, typename... Modifiers>
    struct dname_t
    {
    };

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
        namespace detail
        {
            // TODO: implement.
            template<typename BuilderT,
                     typename NameTag,
                     template<typename...> class GenericContextClass = unspecified_tt>
            struct type_by_name
            {
                using type =
                    typename BuilderT::template type_by_name<NameTag, GenericContextClass>::type;
                static_assert(!std::is_same_v<type, not_found_t>,
                              "NameTag is not registered within the BuilderT");
            };
        }   // namespace detail

        template<typename BuilderT, typename NameTag>
        using type_by_name = typename detail::type_by_name<BuilderT, NameTag>::type;

        template<typename /*BuilderT*/, typename /*NameList*/>
        struct get_type_list;

        template<typename BuilderT, typename... NameTags>
        struct get_type_list<BuilderT, type_list<NameTags...>>
        {
            using type = type_list<type_by_name<BuilderT, NameTags>...>;
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
     */
    template<template<typename...> class GenericClass,
             template<template<typename...> class, typename...> class GetNameList = get_name_list,
             typename... Modifiers,
             typename BuilderT>
    constexpr auto make_lasanga(BuilderT &&builder)
    {
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

        // TODO: add usage of GenericContextType to implementation
        template<typename NameTag,
                 template<typename...>
                 class GenericContextClass,
                 typename... DesignatedFactories>
        struct get_type_by_name
        {
            static_assert(!std::is_same_v<NameTag, unnamed>, "NameTag must not be unnamed");
            using list = typename map_factories<NameTag, DesignatedFactories...>::type;

            static_assert(traits::type_list_size<list>::value <= 1, "Several NameTags found!");

            using type = typename traits::element_type<0, list>::type;
        };

        // implement search by NameTag
    }   // namespace detail

    template<typename... DesignatedFactories>
    class builder : DesignatedFactories...
    {
    public:
        using DesignatedFactories::operator()...;

        template<typename... FactoriesT>
        constexpr builder(FactoriesT &&...factories)
          : DesignatedFactories(std::forward<FactoriesT>(factories))...
        {
        }

        template<typename NameTag, template<typename...> class GenericContextType = unspecified_tt>
        using type_by_name =
            detail::get_type_by_name<NameTag, GenericContextType, DesignatedFactories...>;

    private:
    };

    // TODO: function to get template tree from builder?

    namespace detail
    {
        template<typename Callable,
                 typename Type,
                 typename NameTag = unnamed,
                 template<typename...> class GenericContextClass = unspecified_tt,
                 template<typename...> class GenericClass = unspecified_tt>
        class designated_factory
        {
        public:
            using type = Type;
            using name_tag = NameTag;

            // TODO: refactor this?
            static_assert(
                std::is_same_v<type, decltype(std::declval<Callable>()())> ||
                    traits::is_constructible<type, decltype(std::declval<Callable>()())>::value,
                "Callable can't be used to construct an object or Type");

            template<typename... ArgsT,
                     typename std::enable_if_t<sizeof...(ArgsT) ||   //
                                                   std::is_default_constructible_v<Callable>,
                                               int> * = nullptr>
            constexpr explicit designated_factory(ArgsT &&...args)   //
              : factory_(std::forward<ArgsT>(args)...)
            {
            }

            template<bool MoveConstructible = std::is_move_constructible_v<Callable>,
                     typename std::enable_if_t<MoveConstructible, int> * = nullptr>
            explicit designated_factory(designated_factory &&other) noexcept   //
              : factory_(std::move(other.factory_))
            {
            }

            decltype(auto) operator()(eld::build_t<type>) { return factory_(); }

            template<bool Specified = !traits::is_unspecified<GenericClass>::value,
                     typename std::enable_if_t<Specified, int> * = nullptr>
            decltype(auto) operator()(eld::build_tt<GenericClass>)
            {
                return operator()(eld::build_t<type>());
            }

            // TODO: (?) conditional to use name_tt<type> if name_tag is a template
            template<bool Named = !std::is_same_v<name_tag, unnamed>,
                     typename std::enable_if_t<Named, int> * = nullptr>
            decltype(auto) operator()(eld::name_t<name_tag>)
            {
                return operator()(eld::build_t<type>());
            }

            template<bool Named = !std::is_same_v<name_tag, unnamed>,
                     bool Specified = !traits::is_unspecified<GenericContextClass>::value,
                     typename std::enable_if_t<Named and Specified, int> * = nullptr>
            decltype(auto) operator()(eld::dname_t<GenericContextClass, name_tag>)
            {
                return operator()(eld::build_t<type>());
            }

        private:
            Callable factory_;
        };
    }   // namespace detail

    /**
     * TODO:
     * - make_builder utility function
     *      - resolve function pointers that return objects
     *      - resolve wrapped with tags callables
     *
     * - generic builder class with tags
     * - move "build" function here?
     * - builder should be able to have custom template tags with any placeholder
     */

    namespace traits
    {
        template<typename BuilderT>
        struct builder
        {
            using type = BuilderT;

            template<template<typename...> class GenericClass, typename... Modifiers>
            using default_specialization =
                typename type::template default_specialization<GenericClass, Modifiers...>::type;
        };
    }   // namespace traits

    namespace detail
    {
        /**
         *
         * @tparam NameTag May be a type of an object to build, may be a tag descriptor_t<NameTag,
         * Type>
         * @tparam Callable Callable that constructs an object of type, or provides a tuple of
         * arguments to construct an object from
         */

        /**
         * Designated_factory must provide:
         * - operator()(build_t<Type>)
         * - type: type of a constructed object
         *      -  deduced from return type of a callable object if not a tuple
         *      - explicitly provided via wrap_factory_old<Type>(CallableReturnTuple)
         *      - if constructing a tuple - wrap_factory_old<std::tuple> must be used
         *
         * - or/and operator()(build_tt<GenericType, ...>)
         * - specialization for default_generic<GenericType,...>
         *
         * designated_factory has 2 specializations:
         * - non-template type
         * - template type
         */

        template<typename Callable, typename T, typename...>
        class designated_factory_old
        {
        public:
            using type = typename traits::get_type<T>::type;
            using name = typename traits::get_name<T>::type;

            template<bool DefaultConstructible = std::is_default_constructible_v<Callable>,
                     typename std::enable_if_t<DefaultConstructible, int> * = nullptr>
            designated_factory_old()   //
              : factory_()
            {
            }

            designated_factory_old(designated_factory_old &&) = default;

            template<typename CallableT>
            designated_factory_old(CallableT &&callable)   //
              : factory_(std::forward<CallableT>(callable))
            {
            }

            constexpr decltype(auto) operator()(build_t<type>) { return factory_(); }

            template<bool IsUnnamed = std::is_same_v<name, unnamed>,
                     typename std::enable_if_t<!IsUnnamed, int> * = nullptr>
            constexpr decltype(auto) operator()(build_t<name>)
            {
                return operator()(build_t<type>());
            }

        private:
            Callable factory_;
        };

        // specialization to create implementation type for GenericClass<>
        template<template<typename> class GenericClass,
                 typename Specialization,
                 typename... Modifiers,
                 typename Callable>
        class designated_factory_old<Callable, GenericClass<Specialization>, Modifiers...>
        {
        public:
            using type = typename traits::get_type<Specialization>::type;
            using name = typename traits::get_name<Specialization>::type;

            template<bool DefaultConstructible = std::is_default_constructible_v<Callable>,
                     typename std::enable_if_t<DefaultConstructible, int> * = nullptr>
            designated_factory_old()   //
              : factory_()
            {
            }

            designated_factory_old(designated_factory_old &&) = default;

            template<typename CallableT>
            designated_factory_old(CallableT &&callable)   //
              : factory_(std::forward<CallableT>(callable))
            {
            }

            constexpr decltype(auto) operator()(build_tt<GenericClass>) { return factory_(); }

        private:
            Callable factory_;
        };

        template<typename... T>
        class builder : protected T...
        {
        public:
            template<typename... ArgsT>
            explicit builder(ArgsT &&...args)   //
              : T(std::forward<ArgsT>(args))...
            {
            }

            using T::operator()...;
        };

    }   // namespace detail

    template<typename Callable>
    constexpr auto wrap_factory_old()
    {
        using constructed_type = decltype(std::declval<Callable>()());
        static_assert(!traits::is_tuple<constructed_type>(),
                      "Factories that construct tuples are not allowed. Use special overload and "
                      "explicitly specify constructed type");
        return detail::designated_factory_old<Callable, constructed_type>();
    }

    template<template<typename...> class GenericClass, typename Callable>
    constexpr auto wrap_factory_old()
    {
        using constructed_type = decltype(std::declval<Callable>()());
        static_assert(!traits::is_tuple<constructed_type>(),
                      "Factories that construct tuples are not allowed. Use special overload and "
                      "explicitly specify constructed type");
        return detail::designated_factory_old<Callable, GenericClass<constructed_type>>();
    }

    template<typename ConstructedT>
    constexpr auto wrap_factory_old(ConstructedT (*ptr)())
    {
        return detail::designated_factory_old<decltype(ptr), ConstructedT>(ptr);
    }

    template<typename Callable>   //, typename std::enable_if_t<, int>* = nullptr>
    constexpr auto wrap_factory_old(Callable &&callable)
    {
        using callable_type = std::decay_t<Callable>;
        using constructed_type = decltype(std::declval<callable_type>()());
        static_assert(!traits::is_tuple<constructed_type>(),
                      "Factories that construct tuples are not allowed. Use special overload and "
                      "explicitly specify constructed type");
        return detail::designated_factory_old<callable_type, constructed_type>(
            std::forward<Callable>(callable));
    }

    template<typename NameTag, typename Callable>
    detail::designated_factory<NameTag, std::decay_t<Callable>> wrap_factory_old(
        Callable &&callable)
    {
        return detail::designated_factory_old<NameTag, std::decay_t<Callable>>(
            std::forward<Callable>(callable));
    }

    /**
     *
     * @tparam ArgsT list of designated factories
     * @return
     */
    template<typename... ArgsT>
    constexpr auto make_builder(ArgsT &&...args)
    {
        return detail::builder<std::decay_t<ArgsT>...>(std::forward<ArgsT>(args)...);
    }

}   // namespace eld
