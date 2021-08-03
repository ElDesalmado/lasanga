
#include "lasanga/builder.h"

#include <type_traits>
#include <utility>

template<typename...>
struct type_list;

// wrapper to store unspecialized template class within a type_list
template<template<typename...> class>
struct type_tt
{
};

template<template<typename...> class /*GenericClass*/, typename /*TypeList*/>
struct specialize_t;

template<template<typename...> class GenericClass, typename... Types>
struct specialize_t<GenericClass, type_list<Types...>>
{
    using type = GenericClass<Types...>;
};

// TODO: use builder traits (?)
template<typename BuilderT, typename NameTag>
using type_by_name = typename BuilderT::template type_by_name<NameTag>::type;

template<typename /*BuilderT*/, typename /*NameList*/>
struct get_type_list;

template<typename BuilderT, typename... NameTags>
struct get_type_list<BuilderT, type_list<NameTags...>>
{
    using type = type_list<type_by_name<BuilderT, NameTags>...>;
};

template<template<typename...> class GenericClass, typename... Modifiers>
struct get_name_list;

template<template<typename...> class GenericClass,
         template<template<typename...> class, typename...> class GetNameList = get_name_list,
         typename... Modifiers,
         typename BuilderT>
constexpr auto make_lasanga(BuilderT &&builder)
{
    using name_list = typename GetNameList<GenericClass, Modifiers...>::type;
    using type_list_t = typename get_type_list<std::decay_t<BuilderT>, name_list>::type;

    return typename specialize_t<GenericClass, type_list_t>::type(builder);
}

// build by NameTag
template<typename>
struct name_t
{
};

// build by template NameTag. Template NameTag can be used to overload a get<NameTag> function
template<template<typename...> class>
struct name_tt
{
};

// build by dependent NameTag and a set of Modifiers. NameTag of a type used within a template
// GenericClass
template<template<typename...> class GenericClass, typename NameTag, typename... Modifiers>
struct dname_t
{
};

template<typename>
struct build_t
{
};

// (? do I need this ?) tag to build a specialization(implementation) for class template Type<Spec>
template<template<typename...> class Type, typename... Modifiers>
struct build_tt
{
};

///////// Example

#include <iostream>

struct NonTemplate
{
    void speak() { std::cout << "NonTemplate" << std::endl; }
};

// This part should be defined in user's generic/layer.h header

namespace name_tags
{
    struct A;

    template<typename...>
    struct B;

    struct C;
}   // namespace name_tags

// TODO: function build(Builder, tag<NameOrType>) to deduce return type? (Object or Tuple)

template<typename A, typename B, typename C>
class Layer
{
public:
    template<typename BuilderT>
    Layer(BuilderT &&builder)
      : a_(builder(name_t<name_tags::A>())),
        b_(builder(name_tt<name_tags::B>())),
        c_(builder(dname_t<Layer, name_tags::C>())),
        nt_(builder(build_t<NonTemplate>()))
    {
    }

    void speak_all()
    {
        a_.speak();
        b_.speak();
        c_.speak();
        nt_.speak();
    }

private:
    A a_;
    B b_;
    C c_;
    NonTemplate nt_;
};

template<>
struct get_name_list<Layer>
{
    using type = type_list<name_tags::A, type_tt<name_tags::B>, name_tags::C>;
};

// end of user-defined header

struct Person
{
    void speak() { std::cout << "Person" << std::endl; }
};

struct Dog
{
    void speak() { std::cout << "Dog" << std::endl; }
};

struct Cat
{
    void speak() { std::cout << "Cat" << std::endl; }
};

template<typename...>
struct unspecified_tt;

namespace traits
{
    template<template<typename...> class ATT, template<typename...> class BTT>
    struct is_same_tt : std::false_type
    {
    };

    template<template<typename...> class TT>
    struct is_same_tt<TT, TT> : std::true_type
    {
    };

    template<typename T, typename From>
    struct is_constructible : std::is_constructible<T, From>
    {
    };

    template<typename T, typename... Args>
    struct is_constructible<T, std::tuple<Args...>> : std::is_constructible<T, Args...>
    {
    };

    template<template<typename...> class>
    struct is_unspecified : std::false_type
    {
    };

    template<>
    struct is_unspecified<unspecified_tt> : std::true_type
    {
    };
}   // namespace traits

namespace detail
{
}

// TODO: variadic list for parameters other than Type?
template<typename Callable,
         typename Type,
         typename NameTag = Type,
         template<typename...> class GenericContextClass = unspecified_tt,
         template<typename...> class GenericClass = unspecified_tt>
class designated_factory
{
public:
    using type = Type;
    using name_tag = NameTag;

    // TODO: refactor this?
    static_assert(std::is_same_v<type, decltype(std::declval<Callable>()())> ||
                      traits::is_constructible<type, decltype(std::declval<Callable>()())>::value,
                  "Callable can't be used to construct an object or Type");

    template<typename... ArgsT,
             typename std::enable_if_t<sizeof...(ArgsT) ||   //
                                           std::is_default_constructible_v<Callable>,
                                       int> * = nullptr>
    explicit designated_factory(ArgsT &&...args)   //
      : factory_(std::forward<ArgsT>(args)...)
    {
    }

    decltype(auto) operator()(build_t<type>) { return factory_(); }

    template<bool Specified = !traits::is_unspecified<GenericClass>::value,
             typename std::enable_if_t<Specified, int> * = nullptr>
    decltype(auto) operator()(build_tt<GenericClass>)
    {
        return operator()(build_t<type>());
    }

    // TODO: conditional to use name_tt<type> if name_tag is a template
    decltype(auto) operator()(name_t<name_tag>) { return operator()(build_t<type>()); }

    template<bool Specified = !traits::is_unspecified<GenericContextClass>::value,
             typename std::enable_if_t<Specified, int> * = nullptr>
    decltype(auto) operator()(dname_t<GenericContextClass, name_tag>)
    {
        return operator()(build_t<type>());
    }

private:
    Callable factory_;
};

// TODO: create Builder using make_builder function
// TODO: make builder using name tags and designated factories
struct Builder
{
    // these operators must be generated automatically within designated factories

    NonTemplate operator()(build_t<NonTemplate>) { return {}; }

    Person operator()(name_t<name_tags::A>) { return {}; }

    Dog operator()(name_tt<name_tags::B>) { return {}; }

    Cat operator()(name_t<name_tags::C>) { return {}; }

    template<template<typename...> class NotSpecified>
    Cat operator()(dname_t<NotSpecified, name_tags::C>)
    {
        return {};
    }

    // if this is not declared within the Builder, name_t must be used
    //    Cat operator()(dname_t<Layer, name_tags::C>) { return {}; }

    template<typename NameTag>
    struct type_by_name;
};

// This must be automatically generated for the Builder in case NameTag was bound
template<>
struct Builder::type_by_name<name_tags::A>
{
    using type = Person;
};
template<>
struct Builder::type_by_name<type_tt<name_tags::B>>
{
    using type = Dog;
};
template<>
struct Builder::type_by_name<name_tags::C>
{
    using type = Cat;
};

int main(int, char **)
{
    auto builder =   // make_builder(
        Builder();

    auto deducedLayer = make_lasanga<Layer>(builder);
    deducedLayer.speak_all();

    //
    return 0;
}
