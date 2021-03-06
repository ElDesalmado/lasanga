#pragma once

/**
 * This is an example of a user's header for a generic class that can be fully customized using a
 * builder.
 */

#include <lasanga/tags.h>

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
      : a_(builder(eld::alias_tag<alias::A>())),
        b_(builder(eld::alias_tag<alias::B>())),
        c_(builder(eld::d_alias_tag<alias::C, Crowd>())), // this is a dependent name.
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

#include <lasanga/get_alias_list.h>

// user has to provide a specialization for a list of aliases in the same exact order they are used
// as template parameters
template<>
struct eld::get_alias_list<Crowd>
{
    using type = eld::alias_list<alias::A, wrapped_tt<alias::B>, alias::C>;
};