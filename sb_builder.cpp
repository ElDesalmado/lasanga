
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

namespace alias
{
    struct A;

    template<typename...>
    struct B;

    struct C;
}   // namespace alias

// TODO: function build(Builder, tag<NameOrType>) to deduce return type? (Object or Tuple)

template<typename A, typename B, typename C>
class Layer
{
public:
    template<typename BuilderT>
    Layer(BuilderT &&builder)
      : a_(builder(eld::name_tag<alias::A>())),
        b_(builder(eld::name_tag<alias::B>())),
        c_(builder(eld::dname_t<Layer, alias::C>())),
        nt_(builder(eld::build_tag<NonTemplate>()))
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
    using type = type_list<alias::A, type_tt<alias::B>, alias::C>;
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

    Person operator()(eld::name_t<alias::A>) { return {}; }

    Dog operator()(eld::name_tt<alias::B>) { return {}; }

    Cat operator()(eld::name_t<alias::C>) { return {}; }

    template<template<typename...> class NotSpecified>
    Cat operator()(eld::dname_t<NotSpecified, alias::C>)
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
struct Builder::type_by_name<alias::A>
{
    using type = Person;
};
template<>
struct Builder::type_by_name<eld::type_tt<alias::B>>
{
    using type = Dog;
};
template<>
struct Builder::type_by_name<alias::C>
{
    using type = Cat;
};

// factories

struct create_cat
{
    Cat operator()() { return {}; }
};

Person create_person() { return {}; }

int main(int, char **)
{
    namespace detail = eld::detail;

    eld::wrap_factory (&create_person)(eld::build_tag<Person>()).speak();
    eld::wrap_factory([]() { return Dog(); })(eld::build_tag<Dog>()).speak();
    eld::wrap_factory<create_cat>()(eld::build_tag<Cat>()).speak();

    // named
    eld::named_factory<alias::A> (&create_person)(eld::name_tag<alias::A>()).speak();
    eld::named_factory<alias::B>([]() { return Dog(); })(eld::name_tag<alias::B>()).speak();
    eld::named_factory<alias::C, create_cat, Layer>()(eld::name_tag<alias::C>()).speak();
    eld::named_factory<alias::C, create_cat, Layer>()(eld::dname_t<Layer, alias::C>()).speak();

    auto bldr = eld::make_builder(eld::named_factory<alias::A>(&create_person),
                                  eld::named_factory<alias::B>([]() { return Dog(); }),
                                  eld::named_factory<alias::C, create_cat, Layer>());

    auto builder =   // TODO: make it work
        eld::make_builder(eld::named_factory<alias::A>(&create_person),
                          eld::named_factory<alias::B>([]() { return Dog(); }),
                          eld::named_factory<alias::C, create_cat, Layer>(),
                          eld::wrap_factory([]() { return NonTemplate(); }));

    builder(eld::build_tag<Person>()).speak();
    builder(eld::build_tag<Dog>()).speak();
    builder(eld::build_tag<Cat>()).speak();

    builder(eld::name_tag<alias::A>()).speak();
    builder(eld::name_tag<alias::B>()).speak();
    builder(eld::name_tag<alias::C>()).speak();
    builder(eld::dname_t<Layer, alias::C>()).speak();

    auto deducedLayer = Layer<Person, Dog, Cat>(Builder());
    //        eld::make_lasanga<Layer>(builder); // TODO: make it work
    deducedLayer.speak_all();

    //
    return 0;
}
