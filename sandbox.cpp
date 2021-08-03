#include "lasanga/builder.h"
#include "lasanga/custom_override.h"
#include "lasanga/lasanga.h"

#include <iostream>

/**
 * Given a builder-aware NonTemplate class with an N <= M nested members and a builder composed of
 * M factories that are able to construct N nested members, the following is a valid expression.
 *      auto builder = make_builder(wrap_factory<M>()...);
 *      auto instance = make_lasanga<NonTemplate>(builder);
 * Where `builder` is an object of generic template builder class specialized from M factories,
 * `instance` is an object of user-defined NonTemplate class that uses `builder` to construct its
 * nested members
 *
 * Given a builder-aware Template class with P template parameters (which may be template classes
 * with J template parameters) and N <= M(P(J)) nested members and a builder composed of M(P(J))
 * factories that are able to construct N nested members the following is a valid expression.
 *      auto builder = make_builder(wrap_factory<P>()...
 *                                  wrap_factory<J>()...);
 *      auto instance = make_lasanga<Template>(builder);
 * Where...
 * `instance` is an object of deduced and fully specialized template class Template.
 *
 * In order to fully specialize template<A, B, C...>, where A, B and C may also be a
 * template<A1, B1, C1...>
 *
 * Given a `template<template<typename...> class GenericClass, typename... Modifiers>`, function
 * make_lasanga<GenericClass, Modifiers...> must:
 *  - deduce the number of template parameters for `GenericClass` with a given set of `Modifiers`
 *      - use a template_params_count<GenericClass, Modifiers...>
 *          or
 *      - count the number recursively, (?) using any_t as a template argument and checking
 *
 * // build by name
 * template <typename>
 * struct name_t{};
 *
 * // build by template name. Template NameTag can be used to overload a get<NameTag> function
 * template <template <typename...> class>
 * struct name_tt{};
 *
 * // TODO: (?) generic template tag to distinguish between template and template template
 *
 * // Dependent Name Tag. build by name within a template GenericClass using a set of Modifiers
 * template <template <typename...> class GenericClass, typename NameTag, typename ... Modifiers>
 * struct dname_t{};
 *
 * // build by type. May be used only if Type is already specialized
 * template <typename>
 * struct build_t{};
 *
 * template <template <typename...> class>
 * struct build_tt{};
 *
 * struct NameA;
 * struct NameB;
 * struct NameC;
 *
 * template <typename A, typename B, typename C>
 * class Layer
 * {
 * public:
 *   template <typename BuilderT>
 *   Layer(BuilderT &&builder)
 *      : a_(builder(name_t<NameA>()), // A must be deduced from Builder in make_layer, only by
 * NameA tag
 *        b_(builder(dname_t<Layer, NameB>()), // B must be deduced from Builder in make_layer.
 * Builder will use Layer and NameB tag to deduce B if specified, otherwise will use just NameB tag
 *        c_(builder(name_t<NameC>()), // C must be deduced from Builder in make_layer, only by
 * NameC tag
 *        nt_(builder(build_t<NonTemplate>()) // does not need to be deduced, Builder must contain a
 * designated factory
 *   {}
 * private:
 *      A a_;
 *      B b_;
 *      C c_;
 *      NonTemplate nt_;
 * };
 *
 * template <template<typename, typename, typename> class LayerT, typename BuilderT>
 * constexpr auto make_layer(BuilderT &&builder)
 * {
 *      // builder
 *      // Can this be automated?
 *      using spec_a =
 *          typename traits::builder<std::decay_t<BuilderT>>::template name_type<NameA>::type;
 *      using spec_b =
 *          typename traits::builder<std::decay_t<BuilderT>>::template name_type<NameB>::type;
 *      using spec_c =
 *          typename traits::builder<std::decay_t<BuilderT>>::template name_type<NameC>::type;
 *
 *      return Layer<spec_a, spec_b, spec_c>(builder);
 * }
 *
 * When making a builder, user
 */


template<template<typename...> class>
struct tag_tt
{
};

/*
 * Is one?
 * Is Infinite (max available)
 */

template<template<typename> class GenericClass>
constexpr size_t template_params_count(tag_tt<GenericClass>)
{
    return 1;
}

template<template<typename, typename> class GenericClass>
constexpr size_t template_params_count(tag_tt<GenericClass>)
{
    return 2;
}

template<typename>
class SingleParam
{
};

template<typename, typename>
class DoubleParams
{
};

static_assert(template_params_count(tag_tt<SingleParam>()) == 1);
static_assert(template_params_count(tag_tt<DoubleParams>()) == 2);

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
