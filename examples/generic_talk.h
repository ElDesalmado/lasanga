#pragma once

/**
 * This is an example of a user's header for a generic class that can be fully customized using a
 * builder.
 */

#include "lasanga/builder.h"

#include <iostream>

// this is a non-template class for a sake of example
struct NonTemplate
{
    void speak() { std::cout << "NonTemplate" << std::endl; }
};

// Deduction works by looking up types from specified aliases
namespace alias
{
    struct A;

    template<typename...>
    struct B;

    struct C;
}   // namespace alias

template<typename A, typename B, typename C>
class Crowd
{
public:
    template<typename BuilderT>
    Crowd(BuilderT &&builder)
      : a_(builder(eld::name_tag<alias::A>())),
        b_(builder(eld::name_tag<alias::B>())),
        c_(builder(eld::dt_name_t<Crowd, alias::C>())), // this is a dependent name.
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

// user has to provide a specialization for a list of aliases in the same exact order they are used
// as template parameters
template<>
struct eld::get_name_list<Crowd>
{
    using type = type_list<alias::A, type_tt<alias::B>, alias::C>;
};