#include "lasanga/builder.h"
#include "lasanga/custom_override.h"
#include "lasanga/lasanga.h"

#include <iostream>

/**
 * auto builder = make_builder(...
 *      algo<generic_class<ImplA>>,
 *      // algo<generic_class<ImplB>>
 *      );
 * auto service = make_service<Impl>(builder);
 *
 * generic_class<Spec> servece{...}; // bad!
 * auto service = make_service<Spec>(...);
 *
 *
 * template <typename Algo, typename Users, typename Subs>
 * service
 * {
 * Algo algo;
 * Users users;
 * Subs subs;
 * }
 */

using namespace eld;

template<typename Impl>
class generic_class
{
public:
    using implementation_type = Impl;

    template<typename... ArgsT>
    generic_class(ArgsT &&...args)   //
      : impl_(std::forward<ArgsT>(args)...)
    {
    }

private:
    implementation_type impl_;
};

class non_template
{
public:
    non_template() = default;
};

class impl
{
public:
    impl(int, double) {}
};

class BuilderNonTuple
{
public:
    template<template<typename...> class Generic, typename...>
    struct default_specialization;

    non_template operator()(build_t<non_template>) { return {}; }

    impl operator()(build_tt<generic_class>) { return { 4, 8.15 }; }

    // TODO: use tuples
};

template<>
struct BuilderNonTuple::default_specialization<generic_class>
{
    using type = impl;
};

template<typename Impl>
class Layer
{
public:
    template<typename BuilderT>
    Layer(BuilderT &&builder)
      : nonTemplate_(builder(build_t<non_template>())),
        genericClass_(builder(build_tt<generic_class>()))
    {
    }

private:
    non_template nonTemplate_;
    generic_class<Impl> genericClass_;
};

/**
 * Resursive auto deduction is possible with descriptor tags.
 *
 * In order for a class to utilize a template builder patter it must define NameTags for each of its
 * members:
 *
 * namespace names
 * {
 *      struct pepe;
 *      struct kaka;
 *      struct fofo;
 * }
 *
 * // TODO: deduce Pepe, Kaka, and Fofo using make_object<Buildable>.
 * template <typename Pepe,
 *           typename Kaka,
 *           typename Fofo>
 * class Buildable
 * {
 * public:
 *
 * private:
 *      Pepe p_;
 *      Kaka k_;
 *      Fofo f_;
 * };
 *
 * Pepe, Kaka and Fofo itself may be templates...
 *
 * Need a type map to get indexes for specializations of types corresponding to tags pepe, kaka and
 * fofo
 *
 *
 */

template<typename BuilderT>
constexpr auto make_layer(BuilderT &&builder)
{
    // can this be automated?
    using generic_default = typename traits::builder<
        std::decay_t<BuilderT>>::template default_specialization<generic_class>;

    /**
     * In order to automate this, one may need to get a list of template template classes within
     */

    return Layer<generic_default>(builder);
}

/**
 * Layer <typename DescriptorT,
 *        typename TypeA,
 *        typename TypeB> - composition that will give a list of types and names to be used
 *
 * if a builder has associated list of specializations build_tt<GenericClass, Name_n>, one could use
 * a special function to specialize GenericClass given a Map of Name_n ->
 *
 */

impl func_create_impl() { return { 4, 8.15 }; }

struct create_impl
{
    impl operator()() { return { 4, 8.15 }; }
};

struct name_impl;

struct person
{
    void speak() { std::cout << "person" << std::endl; }
};

struct dog
{
    void speak() { std::cout << "dog" << std::endl; }
};

struct cat
{
    void speak() { std::cout << "cat" << std::endl; }
};

person create_person() { return {}; }

struct create_cat
{
    cat operator()() { return {}; }
};

int main(int, char **)
{
    auto builder = make_builder(wrap_factory(&create_person),
                                wrap_factory<create_cat>(),
                                wrap_factory([]() { return dog(); }));

    builder(build_t<person>()).speak();
    builder(build_t<cat>()).speak();
    builder(build_t<dog>()).speak();

    auto layerDeduced = make_layer(BuilderNonTuple());

    create_impl createImpl{};

    auto designatedFactoryImpl = detail::designated_factory<create_impl, impl>();
    impl i = designatedFactoryImpl(build_t<impl>());
    auto designatedFactoryGeneric = detail::designated_factory<create_impl, generic_class<impl>>();
    impl i2 = designatedFactoryGeneric(build_tt<generic_class>());

    auto designatedFactoryNamedImpl =
        detail::designated_factory<create_impl, descriptor_t<name_impl, impl>>();
    impl iNamed = designatedFactoryNamedImpl(build_t<name_impl>());

    impl fromTemplateArgument = wrap_factory<create_impl>()(build_t<impl>());
    impl fromFunctionPointer = wrap_factory(&func_create_impl)(build_t<impl>());
    impl fromCallableClass = wrap_factory(create_impl())(build_t<impl>());
    impl fromLambda = wrap_factory([]() { return impl(4, 8.15); })(build_t<impl>());

    auto implLambda = []() { return impl(4, 8.15); };
    static_assert(std::is_same_v<impl, decltype(implLambda())>);
    impl fromLambda2 = wrap_factory(implLambda)(build_t<impl>());   //

    impl fromGenericTemplate =
        wrap_factory<generic_class, create_impl>()(build_tt<generic_class>());

    return 0;
}
