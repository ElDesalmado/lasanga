
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
        c_(builder(eld::dt_name_t<Layer, alias::C>())),
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

// factories

struct create_cat
{
    Cat operator()() { return {}; }
};

Person create_person() { return {}; }

int main(int, char **)
{
    namespace detail = eld::detail;

    struct dummy
    {
    };

    using tuple_input = std::tuple<double, int, dummy, size_t>;

    using output_sequence = typename eld::detail::map_tuple_index_sequence<
        tuple_input,
        eld::type_list<eld::wrapped_predicate<std::is_integral>>>::type;
    static_assert(std::is_same_v<output_sequence, std::index_sequence<1, 3>>);

    using find_int = typename eld::detail::map_tuple_index_sequence<
        tuple_input,
        eld::type_list<eld::wrapped_predicate<std::is_same, int>>>::type;
    static_assert(std::is_same_v<find_int, std::index_sequence<1>>);

    tuple_input tupleInput{};
    auto mappedTuple = eld::map_tuple<eld::wrapped_predicate<std::is_integral>>(tupleInput);
    static_assert(std::is_same_v<decltype(mappedTuple), std::tuple<int &, size_t &>>);

    auto mappedTuple2 = eld::map_tuple<eld::wrapped_predicate<std::is_integral>>(tuple_input());
    static_assert(std::is_same_v<decltype(mappedTuple2), std::tuple<int &, size_t &>>);

    auto personUnnamedFactory = eld::wrap_factory(&create_person);
    static_assert(eld::traits::is_unnamed<decltype(personUnnamedFactory)>::value);

    auto newBuilder = eld::make_builder(eld::wrap_factory(&create_person),
                                        eld::named_factory<alias::A>(&create_person),
                                        eld::named_factory<alias::B>([]() { return Dog(); }),
                                        eld::named_factory<alias::C>([]() { return Cat(); }));

    newBuilder(eld::build_tag<Person>()).speak();
    newBuilder(eld::name_tag<alias::A>()).speak();
    newBuilder(eld::build_tag<Dog>()).speak();
    newBuilder(eld::name_tag<alias::B>()).speak();
    newBuilder(eld::d_name_t<alias::C, alias::A>()).speak();   // TODO: fix this. It must be a cat

    eld::wrap_factory (&create_person)().speak();
    eld::wrap_factory([]() { return Dog(); })().speak();
    eld::wrap_factory<create_cat>()().speak();

    // named
    eld::named_factory<alias::A> (&create_person)().speak();
    eld::named_factory<alias::B>([]() { return Dog(); })().speak();
    eld::d_named_factory<alias::A, alias::C>([]() { return Cat(); })().speak();
    eld::d_named_factory<alias::A, alias::C, create_cat>()().speak();
    eld::d_named_factory<alias::A, alias::C> (&create_person)().speak();

    eld::d_named_factory<alias::B, alias::C>([]() { return Dog(); })().speak();
    eld::d_named_factory<alias::B, alias::C, create_cat>()().speak();
    eld::d_named_factory<alias::B, alias::C> (&create_person)().speak();

    eld::d_named_factory<alias::B, alias::B>([]() { return Dog(); })().speak();
    eld::d_named_factory<alias::B, alias::B, create_cat>()().speak();
    eld::d_named_factory<alias::B, alias::B> (&create_person)().speak();

    eld::d_named_factory<alias::C, Layer, create_cat>()().speak();

    auto builder =   // TODO: make it work
        eld::make_builder(eld::named_factory<alias::A>(&create_person),
                          eld::named_factory<alias::B>([]() { return Dog(); }),
                          eld::d_named_factory<alias::C, Layer, create_cat>(),
                          eld::wrap_factory([]() { return NonTemplate(); }));

    builder(eld::build_tag<Person>()).speak();
    builder(eld::build_tag<Dog>()).speak();
    builder(eld::build_tag<Cat>()).speak();

    builder(eld::name_tag<alias::A>()).speak();
    builder(eld::name_tag<alias::B>()).speak();
    builder(eld::name_tag<alias::C>()).speak();
    builder(eld::dt_name_t<Layer, alias::C>()).speak();

    auto deducedLayer = eld::make_lasanga<Layer>(builder);
    deducedLayer.speak_all();

    return 0;
}
