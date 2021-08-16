
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

namespace detail
{
    template<typename WrappedPredicateT, typename T>
    struct apply_predicate;

    template<template<typename...> class Predicate,
             typename T,
             typename... Modifiers,
             template<template<typename...> class, typename...>
             class PredicateWrapper>
    struct apply_predicate<PredicateWrapper<Predicate, Modifiers...>, T>
    {
        constexpr static auto value = Predicate<T, Modifiers...>::value;
    };

    template<typename T, typename... WrappedPredicates>
    struct apply_predicates
    {
        constexpr static auto value = std::conjunction_v<apply_predicate<WrappedPredicates, T>...>;
    };

    template<typename TupleT,
             typename ListWrappedPredicatesT,
             typename = decltype(std::make_index_sequence<std::tuple_size_v<TupleT>>())>
    struct map_tuple_index_sequence;

    template<template<typename...> class TupleT,
             typename... Types,
             template<typename...>
             class TypeListT,
             typename... WrappedPredicatesT,
             size_t... Indx>
    struct map_tuple_index_sequence<TupleT<Types...>,
                                    TypeListT<WrappedPredicatesT...>,
                                    std::index_sequence<Indx...>>
    {
    private:
        using tuple_input = TupleT<Types...>;

        using tuple_indices = decltype(std::tuple_cat(
            std::conditional_t<apply_predicates<Types, WrappedPredicatesT...>::value,
                               std::tuple<std::integral_constant<size_t, Indx>>,
                               std::tuple<>>()...));

        template<typename TupleI>
        struct sequence_from_tuple;

        template<template<typename...> class TupleIC, size_t... IC>
        struct sequence_from_tuple<TupleIC<std::integral_constant<size_t, IC>...>>
        {
            using type = std::index_sequence<IC...>;
        };

    public:
        using type = typename sequence_from_tuple<tuple_indices>::type;
    };

    template<typename TupleT,
             typename ListWrappedPredicatesT,
             typename = typename map_tuple_index_sequence<TupleT, ListWrappedPredicatesT>::type>
    struct map_tuple;

    template<template<typename...> class TupleT,
             typename... T,
             typename ListWrappedPredicatesT,
             size_t... Indx>
    struct map_tuple<TupleT<T...>, ListWrappedPredicatesT, std::index_sequence<Indx...>>
    {
        constexpr auto operator()(TupleT<T...> &tuple)
        {
            return TupleT<std::tuple_element_t<Indx, std::decay_t<decltype(tuple)>> &...>(
                std::get<Indx>(tuple)...);
        }

        constexpr auto operator()(const TupleT<T...> &tuple)
        {
            return TupleT<std::tuple_element_t<Indx, std::decay_t<decltype(tuple)>> &...>(
                std::get<Indx>(tuple)...);
        }
    };

    template<typename...>
    struct type_list;

}   // namespace detail

template<template<typename...> class, typename...>
struct wrapped_predicate;

template<typename... WrappedPredicates, typename TupleT>
constexpr auto map_tuple(TupleT &&tuple)
{
    return detail::map_tuple<std::decay_t<TupleT>, detail::type_list<WrappedPredicates...>>{}(
        tuple);
}

namespace traits
{
    template<typename DFactoryT>
    using is_unnamed = eld::detail::is_unnamed<typename DFactoryT::name_tag>;

    template<typename DFactoryT>
    using is_named = std::negation<is_unnamed<DFactoryT>>;

    template<typename DFactoryT>
    using name_tag = typename DFactoryT::name_tag;

    template<typename DFactoryT>
    using type = typename DFactoryT::type;

    template<typename DFactoryT>
    using depends_on_type = typename DFactoryT::depends_on_type;

    template<typename DFactoryT>
    using is_dependent = std::negation<eld::detail::is_unnamed<depends_on_type<DFactoryT>>>;

}   // namespace traits

template<typename... DesignatedFactories>
class builder
{
public:
    template<typename... DFactoriesT>
    constexpr explicit builder(DFactoriesT &&...designatedFactories)
      : designatedFactories_(std::forward<DFactoriesT>(designatedFactories)...)
    {
    }

    /**
     * Building:
     * One tag type (build, name, d_name) specialization allows construction of only one type of
     * object.
     * - using build_t: will build an object of typename
     * - using name_t and name_tt: will build an object that is registered with the given alias
     * - using d_name_t and d_name_tt: will try to build an object using a dependent name. If no
     * dependent name is registered, will fall back to name_t or name_tt
     *
     * Type tree for specialization:
     * Given a root template class, builder will use root's name_list to recursively create a type
     * tree that can be used to fully specialize the given template class.
     */

    /**
     * Build using build_t tag.
     * \details First will try to find a unique unnamed designated factory.
     * Then will try to find a unique named designated factory.
     * If other than one factories are suitable, will throw a compile time error.
     * @tparam T
     * @return
     */
    template<typename T>
    decltype(auto) operator()(eld::build_t<T> buildTag)
    {
        auto mappedTuple =
            map_tuple<wrapped_predicate<traits::is_unnamed>, wrapped_predicate<same_type, T>>(
                designatedFactories_);

        return construct(buildTag,
                         mappedTuple);
    }

    template<typename NameTag>
    decltype(auto) operator()(eld::name_t<NameTag>)
    {
        auto mappedTuple =
            map_tuple<wrapped_predicate<traits::is_named>,
                      wrapped_predicate<same_name_tag, NameTag>>(designatedFactories_);
        return construct(mappedTuple);
    }

    /**
     * Build using template NameTag
     * @tparam NameTagT
     * @return
     */
    template<template<typename...> class NameTagT>
    decltype(auto) operator()(eld::name_tt<NameTagT>)
    {
        return (*this)(eld::name_t<eld::type_tt<NameTagT>>());
    }

    /**
     * Build using dependent name tag. Will first try to find a designated factory with requested
     * DependsOnT. Will fall back to name_t if not found.
     * @tparam DependsOnT
     * @tparam NameTag
     * @tparam ...
     * @param dNameTag
     * @return
     */
    template<typename DependsOnT, typename NameTag, typename...>
    decltype(auto) operator()(eld::d_name_t<DependsOnT, NameTag> dNameTag)
    {
        auto mappedTuple =
            map_tuple<wrapped_predicate<traits::is_dependent>,
                      wrapped_predicate<same_depends_on, DependsOnT>>(designatedFactories_);

        return construct(dNameTag,
                         mappedTuple);
    }

    template<typename DependentName, template<typename...> class TNameTagT, typename... Modifiers>
    decltype(auto) operator()(eld::d_name_tt<DependentName, TNameTagT, Modifiers...>)
    {
        return (*this)(eld::d_name_t<DependentName, eld::type_tt<TNameTagT>, Modifiers...>());
    }

    template<template<typename...> class TDependsOnT, typename NameTag, typename... Modifiers>
    decltype(auto) operator()(eld::dt_name_t<TDependsOnT, NameTag, Modifiers...>)
    {
        return (*this)(eld::d_name_t<eld::type_tt<TDependsOnT>, NameTag, Modifiers...>());
    }

    template<template<typename...> class TDependsOnT,
             template<typename...>
             class TNameTagT,
             typename... Modifiers>
    decltype(auto) operator()(eld::dt_name_tt<TDependsOnT, TNameTagT, Modifiers...>)
    {
        return (*this)(
            eld::d_name_t<eld::type_tt<TDependsOnT>, eld::type_tt<TNameTagT>, Modifiers...>());
    }

private:
    template<typename FactoryT, typename NameTag>
    using same_name_tag = std::is_same<traits::name_tag<FactoryT>, NameTag>;

    template<typename FactoryT, typename Type>
    using same_type = std::is_same<traits::type<FactoryT>, Type>;

    template<typename FactoryT, typename DependsOnT>
    using same_depends_on = std::is_same<traits::depends_on_type<FactoryT>, DependsOnT>;

    template<typename T, typename... Factories>
    decltype(auto) construct(eld::build_t<T>,
                             std::tuple<Factories &...> tupleFactories)
    {
        return construct(tupleFactories);
    }

    template<typename T>
    decltype(auto) construct(eld::build_t<T>,
                             std::tuple<>)
    {
        auto mappedTuple =
            map_tuple<wrapped_predicate<traits::is_named>, wrapped_predicate<same_type, T>>(
                designatedFactories_);
        return construct(mappedTuple);
    }

    template<typename DependsOnT, typename NameTag, typename... FactoriesT>
    decltype(auto) construct(eld::d_name_t<DependsOnT, NameTag>,
                             std::tuple<FactoriesT &...> tupleFactories)
    {
        return construct(tupleFactories);
    }

    template<typename DependsOnT, typename NameTag>
    decltype(auto) construct(eld::d_name_t<DependsOnT, NameTag>,
                             std::tuple<>)
    {
        return (*this)(eld::name_t<NameTag>());
    }

    template<typename... FoundFactories>
    decltype(auto) construct(std::tuple<FoundFactories &...> factories)
    {
        constexpr size_t tupleSize = std::tuple_size_v<decltype(factories)>;
        static_assert(tupleSize != 0, "No designated factories have been found.");
        static_assert(
            tupleSize <= 1,
            "Can't resolve designated factory. Several suitable factories were registered");

        return std::get<0>(factories)();
    }

private:
    std::tuple<DesignatedFactories...> designatedFactories_;
};

template<typename... DesignatedFactories>
constexpr auto make_builder(DesignatedFactories &&...factories)
{
    // TODO: combine named with unnamed if type is the same?
    return builder<std::decay_t<DesignatedFactories>...>(
        std::forward<DesignatedFactories>(factories)...);
}

int main(int, char **)
{
    namespace detail = eld::detail;

    struct dummy
    {
    };

    using tuple_input = std::tuple<double, int, dummy, size_t>;

    using output_sequence = typename ::detail::map_tuple_index_sequence<
        tuple_input,
        ::detail::type_list<wrapped_predicate<std::is_integral>>>::type;
    static_assert(std::is_same_v<output_sequence, std::index_sequence<1, 3>>);

    using find_int = typename ::detail::map_tuple_index_sequence<
        tuple_input,
        ::detail::type_list<wrapped_predicate<std::is_same, int>>>::type;
    static_assert(std::is_same_v<find_int, std::index_sequence<1>>);

    tuple_input tupleInput{};
    auto mappedTuple = map_tuple<wrapped_predicate<std::is_integral>>(tupleInput);
    static_assert(std::is_same_v<decltype(mappedTuple), std::tuple<int &, size_t &>>);

    auto mappedTuple2 = map_tuple<wrapped_predicate<std::is_integral>>(tuple_input());
    static_assert(std::is_same_v<decltype(mappedTuple2), std::tuple<int &, size_t &>>);

    auto personUnnamedFactory = eld::wrap_factory(&create_person);
    static_assert(::traits::is_unnamed<decltype(personUnnamedFactory)>::value);

    auto newBuilder = ::make_builder(eld::wrap_factory(&create_person),
                                     eld::named_factory<alias::A>(&create_person),
                                     eld::named_factory<alias::B>([]() { return Dog(); }));

    newBuilder(eld::build_tag<Person>()).speak();
    newBuilder(eld::name_tag<alias::A>()).speak();
    newBuilder(eld::build_tag<Dog>()).speak();
    newBuilder(eld::name_tag<alias::B>()).speak();

    eld::wrap_factory (&create_person)(eld::build_tag<Person>()).speak();
    eld::wrap_factory([]() { return Dog(); })(eld::build_tag<Dog>()).speak();
    eld::wrap_factory<create_cat>()(eld::build_tag<Cat>()).speak();

    // named
    eld::named_factory<alias::A> (&create_person)(eld::name_tag<alias::A>()).speak();
    eld::named_factory<alias::B>([]() { return Dog(); })(eld::name_tag<alias::B>()).speak();
    eld::named_factory<alias::C, create_cat, Layer>()(eld::name_tag<alias::C>()).speak();
    eld::named_factory<alias::C, create_cat, Layer>()(eld::dt_name_t<Layer, alias::C>()).speak();

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
    builder(eld::dt_name_t<Layer, alias::C>()).speak();

    auto deducedLayer = eld::make_lasanga<Layer>(builder);
    deducedLayer.speak_all();

    return 0;
}
