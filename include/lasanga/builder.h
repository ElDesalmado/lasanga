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
        template<typename NameTag, typename Callable>
        class designated_factory
        {
            static_assert(is_callable_factory<Callable>(),
                          "Factory type must be a callable object!");

        public:
            using type = NameTag;
            using callable_type = Callable;

            template<typename... ArgsT>
            designated_factory(ArgsT &&...args)   //
              : factory(std::forward<ArgsT>(args)...)
            {
            }

        private:
            Callable factory;
        };

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

    }   // namespace detail

    template<typename NameTag, typename Callable>
    detail::designated_factory<NameTag, std::decay_t<Callable>> wrap_factory(Callable &&callable)
    {
        return detail::designated_factory<NameTag, std::decay_t<Callable>>(
            std::forward<Callable>(callable));
    }

    template<typename... DesignatedFactories>
    class builder
    {
    public:
        // builder must have overloadings for operator()(tag<NameTag>)

    private:
        std::tuple<DesignatedFactories...> designatedFactories_;
    };

    template<typename... ArgsT>
    constexpr auto make_builder(ArgsT &&.../*args*/)
    {
        // TODO: implement
    }

}   // namespace eld
