#pragma once

namespace eld
{
    namespace tag
    {
        // placeholder for template template tag
        struct any;

        struct not_found
        {
        };

        template<typename...>
        struct build
        {
        };

        struct default_call
        {
        };

        struct unnamed;

        template<typename...>
        struct unnamed_t;
    }

    constexpr inline tag::not_found not_found;
    constexpr tag::default_call default_call_tag;


    /**
     * Wrapper type to store unspecialized template class (for example, within a type_list)
     */
    template<template<typename...> class>
    struct wrapped_tt
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
     * Builder tag. Used to construct an object within a TGenericClassT by NameTag and a set of
     * ModifiersT
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
}
