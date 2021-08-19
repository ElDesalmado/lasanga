#include "lasanga/builder.h"
#include "lasanga/custom_override.h"
#include "lasanga/layer.h"

#include <iostream>

/**
 * Given a builder-aware NonTemplate class with an N <= M nested members and a builder composed of
 * M factories that are able to construct N nested members, the following is a valid expression.
 *      auto builder = make_builder(wrap_factory_old<M>()...);
 *      auto instance = make_lasanga<NonTemplate>(builder);
 * Where `builder` is an object of generic template builder class specialized from M factories,
 * `instance` is an object of user-defined NonTemplate class that uses `builder` to construct its
 * nested members
 *
 * Given a builder-aware Template class with P template parameters (which may be template classes
 * with J template parameters) and N <= M(P(J)) nested members and a builder composed of M(P(J))
 * factories that are able to construct N nested members the following is a valid expression.
 *      auto builder = make_builder(wrap_factory_old<P>()...
 *                                  wrap_factory_old<J>()...);
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


int main(int, char **)
{

    return 0;
}
