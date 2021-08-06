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

            using type = typename traits::element_type<0, list>::type::type;
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
        using type_by_name = typename detail::
            get_type_by_name<NameTag, GenericContextType, DesignatedFactories...>::type;

        // TODO: resolve dname_t to name_t in case no DependsOnT template types were found

    private:
    };

    template<typename... DesignatedFactories>
    constexpr auto make_builder(DesignatedFactories &&...factories)
    {
        return builder<std::decay_t<DesignatedFactories>...>(
            std::forward<DesignatedFactories>(factories)...);
    }

    // TODO: function to get template tree from builder?

    namespace detail
    {
        template<typename NameTag>
        struct is_unnamed : std::is_same<NameTag, unnamed>
        {
        };

        template<template<typename...> class TNameTag>
        struct is_unnamed<type_tt<TNameTag>> : traits::is_same_tt<TNameTag, unspecified_tt>
        {
        };

        template<typename NameTag>
        struct get_name_t
        {
            using type = name_t<NameTag>;
        };

        template<template<typename...> class TNameTag>
        struct get_name_t<type_tt<TNameTag>>
        {
            using type = name_tt<TNameTag>;
        };

        template<typename Callable,
                 typename Type,
                 typename NameTag = unnamed,
                 template<typename...> class DependsOnT = unspecified_tt,
                 template<typename...> class GenericClass = unspecified_tt>
        class designated_factory
        {
        public:
            using type = Type;
            using name_tag = NameTag;

            // TODO: checks if name_tag is a wrapped template tag -> type_tt<TNameTag>

            // TODO: refactor this?
            static_assert(
                std::is_same_v<type, decltype(std::declval<Callable>()())> or
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

            template<bool Specified = !traits::is_same_tt<GenericClass, unspecified_tt>::value,
                     typename std::enable_if_t<Specified, int> * = nullptr>
            decltype(auto) operator()(eld::build_tt<GenericClass>)
            {
                return operator()(eld::build_t<type>());
            }

            template<bool Named = !detail::is_unnamed<name_tag>::value,
                     typename std::enable_if_t<Named, int> * = nullptr>
            decltype(auto) operator()(typename detail::get_name_t<name_tag>::type)
            {
                return operator()(eld::build_t<type>());
            }

            // TODO: generic version for dname_t and dname_tt (?)
            template<bool Named = !detail::is_unnamed<name_tag>::value,
                     bool Specified = !traits::is_same_tt<DependsOnT, unspecified_tt>::value,
                     typename std::enable_if_t<Named and Specified, int> * = nullptr>
            decltype(auto) operator()(eld::dname_t<DependsOnT, name_tag>)
            {
                return operator()(eld::build_t<type>());
            }

        private:
            Callable factory_;
        };
    }   // namespace detail

    template<typename Callable>
    constexpr auto wrap_factory(Callable &&callable)
    {
        using callable_type = std::decay_t<Callable>;
        using constructed_type = decltype(std::declval<callable_type>()());
        static_assert(!traits::is_tuple<constructed_type>(),
                      "Factories that construct tuples are not allowed. Use special overload and "
                      "explicitly specify constructed type");
        return detail::designated_factory<std::decay_t<Callable>, constructed_type>(
            std::forward<Callable>(callable));
    }

    template<typename Callable>
    constexpr auto wrap_factory()
    {
        return wrap_factory<Callable>(Callable());
    }

    template<typename Constructed>
    constexpr auto wrap_factory(Constructed (*fPtr)())
    {
        return wrap_factory<std::decay_t<decltype(fPtr)>>(std::move(fPtr));
    }

    template<typename NameTag,
             template<typename...> class DependsOnT = unspecified_tt,
             typename Callable>
    constexpr auto named_factory(Callable &&callable)
    {
        static_assert(!std::is_same_v<NameTag, unnamed>, "NamedTag must not be unnamed.");

        using callable_type = std::decay_t<Callable>;
        using constructed_type = decltype(std::declval<callable_type>()());
        static_assert(!traits::is_tuple<constructed_type>(),
                      "Factories that construct tuples are not allowed. Use special overload and "
                      "explicitly specify constructed type");
        return detail::
            designated_factory<std::decay_t<Callable>, constructed_type, NameTag, DependsOnT>(
                std::forward<Callable>(callable));
    }

    template<template<typename...> class NameTagT,
             template<typename...> class DependsOnT = unspecified_tt,
             typename Callable>
    constexpr auto named_factory(Callable &&callable)
    {
        static_assert(!traits::is_same_tt<NameTagT, unspecified_tt>::value,
                      "Template NamedTag must not be unspecified.");

        return named_factory<type_tt<NameTagT>, DependsOnT, std::decay_t<Callable>>(
            std::forward<Callable>(callable));
    }

    template<typename NameTag,
             typename Callable,
             template<typename...> class DependsOnT = unspecified_tt>
    constexpr auto named_factory()
    {
        return named_factory<NameTag, DependsOnT, Callable>(Callable());
    }

    template<template<typename...> class NameTagT,
             typename Callable,
             template<typename...> class DependsOnT = unspecified_tt>
    constexpr auto named_factory()
    {
        return named_factory<NameTagT, DependsOnT, Callable>(Callable());
    }

    template<typename NameTag,
             template<typename...> class DependsOnT = unspecified_tt,
             typename Constructed>
    constexpr auto named_factory(Constructed (*fPtr)())
    {
        return named_factory<NameTag, DependsOnT, std::decay_t<decltype(fPtr)>>(std::move(fPtr));
    }

    template<template<typename...> class NameTagT,
             template<typename...> class DependsOnT = unspecified_tt,
             typename Constructed>
    constexpr auto named_factory(Constructed (*fPtr)())
    {
        return named_factory<NameTagT, DependsOnT, std::decay_t<decltype(fPtr)>>(std::move(fPtr));
    }

}   // namespace eld
