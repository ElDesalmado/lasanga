
// Your header with names

#include <iostream>

namespace names
{
    // not overridden
    template<typename...>
    struct person;

    // default implementation
    template<typename T, typename...>
    struct animal
    {
        constexpr decltype(auto) operator()(T &obj) { return obj.animal(); }
    };
}   // namespace names

#include "lasanga/custom_override.h"

// generic
template<typename ImplT>
class generic_composition
{
public:
    using implementation_type = ImplT;

    template<typename... ArgsT>
    generic_composition(ArgsT &&...args)   //
      : impl_(std::forward<ArgsT>(args)...)
    {
    }

    constexpr decltype(auto) person()
    {
        return eld::get<names::person>(eld::default_call_tag,
                                       impl_,
                                       [](auto &&obj) { return obj.person(); });
    }

    constexpr decltype(auto) animal() { return eld::get<names::animal>(impl_); }

private:
    implementation_type impl_;
};

class Animal
{
public:
    void speak() const { std::cout << "I am an Animal" << std::endl; }
};

class Person
{
public:
    void speak() const { std::cout << "I am an Person" << std::endl; }
};

class CompositionObject
{
public:
    Animal &animal() { return animal_; }
    Person &person() { return person_; }

private:
    Animal animal_;
    Person person_;
};

// using tuple as a composition
using CompositionTuple = std::tuple<Person, Animal>;

// need to override name tag, since it has default implementation and a top priority
template<>
struct names::animal<CompositionTuple>
{
    const Animal &operator()(const CompositionTuple &tuple) { return std::get<Animal>(tuple); }

    Animal &operator()(CompositionTuple &tuple) { return std::get<Animal>(tuple); }
};

template<>
struct eld::map_key_type<names::animal<CompositionTuple>> : eld::value_type<Animal>
{
};

template<>
struct eld::map_key_type<names::person<CompositionTuple>> : eld::value_type<Person>
{
};

int main(int, char **)
{
    generic_composition<CompositionObject> objectComposition{};

    objectComposition.person().speak();
    objectComposition.animal().speak();

    static_assert(eld::traits::is_tuple<CompositionTuple>());

    generic_composition<CompositionTuple> tupleComposition{};
    tupleComposition.person().speak();
    tupleComposition.animal().speak();

    //
    return 0;
}
