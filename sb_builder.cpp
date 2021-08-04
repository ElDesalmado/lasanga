
#include "lasanga/builder.h"

#include <type_traits>
#include <utility>

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
      : a_(builder(eld::name_t<name_tags::A>())),
        b_(builder(eld::name_tt<name_tags::B>())),
        c_(builder(eld::dname_t<Layer, name_tags::C>())),
        nt_(builder(eld::build_t<NonTemplate>()))
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
struct eld::get_name_list<Layer>
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



namespace traits
{



}   // namespace traits

namespace detail
{
}

// TODO: variadic list for parameters other than Type?


// TODO: create Builder using make_builder function
// TODO: make builder using name tags and designated factories
struct Builder
{
    // these operators must be generated automatically within designated factories

    NonTemplate operator()(eld::build_t<NonTemplate>) { return {}; }

    Person operator()(eld::name_t<name_tags::A>) { return {}; }

    Dog operator()(eld::name_tt<name_tags::B>) { return {}; }

    Cat operator()(eld::name_t<name_tags::C>) { return {}; }

    template<template<typename...> class NotSpecified>
    Cat operator()(eld::dname_t<NotSpecified, name_tags::C>)
    {
        return {};
    }

    // if this is not declared within the Builder, name_t must be used
    //    Cat operator()(dname_t<Layer, name_tags::C>) { return {}; }

    template<typename NameTag, template<typename...> class = eld::unspecified_tt>
    struct type_by_name;
};

// This must be automatically generated for the Builder in case NameTag was bound
template<>
struct Builder::type_by_name<name_tags::A>
{
    using type = Person;
};
template<>
struct Builder::type_by_name<eld::type_tt<name_tags::B>>
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

    auto deducedLayer = eld::make_lasanga<Layer>(builder);
    deducedLayer.speak_all();

    //
    return 0;
}
