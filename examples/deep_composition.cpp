
/**
 * This example shows how lasanga can facilitate abstraction and customization of objects within
 * deep composition, i.e. a type that is composed of several layers of nested types.
 *
 */

// Layer.h

#include <lasanga/alias_list.h>
#include <lasanga/tags.h>

#include <iostream>

// TODO: add namespace

namespace alias
{
    struct A;
    struct B;
    struct C;
}   // namespace alias

// forward declaration to be used for get_alias_list specialization
template<typename AT, typename BT, typename CT>
class Layer;

/**
 * If using aliases to specialize a class template, user must provide a get_alias_list
 * specialization with a list of aliases, corresponding to the order of class template template
 * arguments.
 */
template<>
struct eld::get_alias_list<Layer>
{
    using type = eld::alias_list<alias::A, alias::B, alias::C>;
};

template<typename AT, typename BT, typename CT>
class Layer
{
public:
    /**
     * Abstracting construction of aggregated objects through a builder allows to customize objects
     * construction without changing a single line of the classes constructor code. Modifying
     * arguments for each object generally happens outside the scope of this header.
     *
     * Using aliases in builder tags is another level of abstraction. One of the purposes of lasagna
     * is to provide the means of automatic template argument deduction using a builder object.
     * Manually specializing class templates such as this can be cumbersome. It is even more
     * convoluted, when a template argument is a class template itself that needs specialization.
     *
     * In this example Layer is agnostic of the type traits of its aggregated components when
     * invoking the builder.
     * @tparam BuilderT
     * @param builder
     */
    template<typename BuilderT>
    constexpr explicit Layer(BuilderT &&builder)
      : a_(builder(
            eld::d_alias_tag<alias::A, ::Layer>())),   // :: is required due to ambiguous call error
        b_(builder(eld::d_alias_tag<alias::B, ::Layer>())),
        c_(builder(eld::d_alias_tag<alias::C, ::Layer>()))
    {
    }

    void print()
    {
        std::cout << "Layer\n";
        a_.print();
        b_.print();
        c_.print();
    }

private:
    AT a_;
    BT b_;
    CT c_;
};

#include <lasanga/lasanga.h>

template<const char C>
struct dummy_t
{
    void print() { std::cout << "dummy_" << C << '\n'; }
};

// example for custom builder implementation
struct dummy_build_impl
{
    template<typename... Modifiers, typename... ArgsT>
    decltype(auto) construct(eld::d_alias_t<alias::A, Layer, Modifiers...>, ArgsT &&...)
    {
        return dummy_t<'A'>();
    }

    template<typename... Modifiers, typename... ArgsT>
    decltype(auto) construct(eld::d_alias_t<alias::B, Layer, Modifiers...>, ArgsT &&...)
    {
        return dummy_t<'B'>();
    }

    template<typename... Modifiers, typename... ArgsT>
    decltype(auto) construct(eld::d_alias_t<alias::C, Layer, Modifiers...>, ArgsT &&...)
    {
        return dummy_t<'C'>();
    }
};

using dummy_builder = eld::generic::builder<dummy_build_impl>;

template<template<typename...> class TGenericClassT, typename... ModifiersT>
struct eld::generic::resolve_generic_class<dummy_builder, TGenericClassT, ModifiersT...>
{
    using type = void;
};

// Layer-only specialization
template<typename... ModifiersT>
struct eld::generic::resolve_generic_class<dummy_builder, Layer, ModifiersT...>
{
    using type = Layer<dummy_t<'A'>, dummy_t<'B'>, dummy_t<'C'>>;
};

void example_custom_builder()
{
    auto builder = eld::make_builder<dummy_build_impl>();
    auto layer = eld::make_lasanga<Layer>(builder);
    layer.print();
}

void example_builder()
{
    auto builder = eld::make_builder<eld::util::builder_impl>(
        eld::d_named_factory<alias::A, Layer>([]() { return dummy_t<'A'>(); }),
        eld::d_named_factory<alias::B, Layer>([]() { return dummy_t<'B'>(); }),
        eld::d_named_factory<alias::C, Layer>([]() { return dummy_t<'C'>(); })
    );

    auto layer = eld::make_lasanga<Layer>(builder);
    layer.print();
}

int main()
{
    example_custom_builder();
    example_builder();

    return 0;
}
