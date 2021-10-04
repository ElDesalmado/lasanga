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
    }   // namespace tag

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
    struct alias_t
    {
    };

    /**
     * Builder tag. Used to construct an object by unspecialized template NameTag. Template NameTag
     * can be used to overload a get<NameTag> function
     */
    template<template<typename...> class>
    struct alias_tt
    {
    };

    /**
     * Helper function to deduce alias_t or alias_tt
     */
    template<typename NonT>
    constexpr alias_t<NonT> alias_tag()
    {
        return {};
    }

    /**
     * Helper function to deduce alias_t or alias_tt
     */
    template<template<typename...> class TT>
    constexpr alias_tt<TT> alias_tag()
    {
        return {};
    }

    /**
     * Builder tag. Used to construct an object within a TGenericClassT by NameTag and a set of
     * ModifiersT
     * @tparam TDependsOnT Unspecialized template class that owns an object to be constructed.
     * @tparam AliasTagT Tag designated a name for a type.
     * @tparam Modifiers A set of modifiers. Can be used to held in distinguishing between different
     * contexts and TDependsOnT'es
     */
    template<typename AliasTagT, template<typename...> class TDependsOnT, typename... Modifiers>
    struct d_alias_t
    {
    };

    template<template<typename...> class TNameTagT,
             template<typename...>
             class TDependsOnT,
             typename... Modifiers>
    struct d_alias_tt
    {
    };

    /**
     * Helper function to resolve between typename and template DependentName
     * @tparam TDependsOnT
     * @tparam AliasTagT
     * @tparam Modifiers
     * @return
     */
    template<typename AliasTagT, template<typename...> class TDependsOnT, typename... Modifiers>
    constexpr d_alias_t<AliasTagT, TDependsOnT, Modifiers...> d_alias_tag()
    {
        return {};
    }

    template<template<typename...> class TAliasTagT,
             template<typename...>
             class TDependsOnT,
             typename... Modifiers>
    constexpr d_alias_tt<TAliasTagT, TDependsOnT, Modifiers...> d_alias_tag()
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
     * Helper function to deduce alias_t or alias_tt
     */
    template<typename NonT>
    constexpr build_t<NonT> build_tag()
    {
        return {};
    }

    /**
     * Helper function to deduce alias_t or alias_tt
     */
    template<template<typename...> class TT>
    constexpr build_tt<TT> build_tag()
    {
        return {};
    }
}   // namespace eld
