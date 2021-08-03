#include "lasanga/builder.h"
#include "lasanga/custom_override.h"
#include "lasanga/lasanga.h"

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

template<typename BuilderT>
struct builder_traits
{
    using builder_type = BuilderT;

    // returns a type (or a list of types (?))
    template<template<typename...> class GenericClass>
    using default_specialization = typename BuilderT::template default_specialization<GenericClass>;
};

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
 *
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
 *
 */

int main(int, char **)
{
    auto layerDeduced = make_layer(BuilderNonTuple());

    return 0;
}
