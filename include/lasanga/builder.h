#pragma once

#include "lasanga/traits.h"

namespace eld
{
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

    /**
     * For building non-template types
     * @tparam ...
     */
    template<typename...>
    struct build_t
    {
    };

    /**
     * For building specializations for template-template types.
     *
     * When a builder has one specialization for generic_class<T> (i.e. generic_class<Foo>),
     * Builder must be invoked without specializations: builder(build_tt<generic_class>())
     * Otherwise, a specialization must be provided: builder(build_tt<generic_class, Foo>())
     * @tparam GenericClass
     * @tparam SpecAndModifiers
     */
    template<template<typename...> class GenericClass, typename... SpecAndModifiers>
    struct build_tt
    {
    };

    template<typename...>
    struct type_list;

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
        template<typename, typename = void>
        struct is_callable_factory : std::false_type
        {
        };

        template<typename T>
        struct is_callable_factory<T, std::void_t<decltype(std::declval<T>()())>> : std::true_type
        {
        };

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
         *      - explicitly provided via wrap_factory<Type>(CallableReturnTuple)
         *      - if constructing a tuple - wrap_factory<std::tuple> must be used
         *
         * - or/and operator()(build_tt<GenericType, ...>)
         * - specialization for default_generic<GenericType,...>
         *
         * designated_factory has 2 specializations:
         * - non-template type
         * - template type
         */

        template<typename Callable, typename T, typename...>
        class designated_factory
        {
            static_assert(is_callable_factory<Callable>(),
                          "Factory type must be a callable object!");

        public:
            using type = typename traits::get_type<T>::type;
            using name = typename traits::get_name<T>::type;

            template<bool DefaultConstructible = std::is_default_constructible_v<Callable>,
                     typename std::enable_if_t<DefaultConstructible, int> * = nullptr>
            designated_factory()   //
              : factory_()
            {
            }

            designated_factory(designated_factory &&) = default;

            template<typename CallableT>
            designated_factory(CallableT &&callable)   //
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
        class designated_factory<Callable, GenericClass<Specialization>, Modifiers...>
        {
        public:
            using type = typename traits::get_type<Specialization>::type;
            using name = typename traits::get_name<Specialization>::type;

            template<bool DefaultConstructible = std::is_default_constructible_v<Callable>,
                     typename std::enable_if_t<DefaultConstructible, int> * = nullptr>
            designated_factory()   //
              : factory_()
            {
            }

            designated_factory(designated_factory &&) = default;

            template<typename CallableT>
            designated_factory(CallableT &&callable)   //
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
    constexpr auto wrap_factory()
    {
        using constructed_type = decltype(std::declval<Callable>()());
        static_assert(!traits::is_tuple<constructed_type>(),
                      "Factories that construct tuples are not allowed. Use special overload and "
                      "explicitly specify constructed type");
        return detail::designated_factory<Callable, constructed_type>();
    }

    template<template<typename...> class GenericClass, typename Callable>
    constexpr auto wrap_factory()
    {
        using constructed_type = decltype(std::declval<Callable>()());
        static_assert(!traits::is_tuple<constructed_type>(),
                      "Factories that construct tuples are not allowed. Use special overload and "
                      "explicitly specify constructed type");
        return detail::designated_factory<Callable, GenericClass<constructed_type>>();
    }

    template<typename ConstructedT>
    constexpr auto wrap_factory(ConstructedT (*ptr)())
    {
        return detail::designated_factory<decltype(ptr), ConstructedT>(ptr);
    }

    template<typename Callable>   //, typename std::enable_if_t<, int>* = nullptr>
    constexpr auto wrap_factory(Callable &&callable)
    {
        using callable_type = std::decay_t<Callable>;
        using constructed_type = decltype(std::declval<callable_type>()());
        static_assert(!traits::is_tuple<constructed_type>(),
                      "Factories that construct tuples are not allowed. Use special overload and "
                      "explicitly specify constructed type");
        return detail::designated_factory<callable_type, constructed_type>(
            std::forward<Callable>(callable));
    }

    template<typename NameTag, typename Callable>
    detail::designated_factory<NameTag, std::decay_t<Callable>> wrap_factory(Callable &&callable)
    {
        return detail::designated_factory<NameTag, std::decay_t<Callable>>(
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
