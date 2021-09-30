#pragma once

#include "lasanga/utility/filter_type_list.h"
#include "lasanga/utility/traits.h"

#include <type_traits>

namespace eld::util
{
    namespace detail
    {
        template <typename AT, typename BT>
        struct same_wrapped_tt : std::is_same<AT, BT> {};

        template <template <typename...> class TAT,
                 template <typename...> class TBT,
                 template <template <typename...> class> class TWrappedATT,
                 template <template <typename...> class> class TWrappedBTT>
        struct same_wrapped_tt<TWrappedATT<TAT>, TWrappedBTT<TBT>> : traits::is_same_tt<TAT, TBT>{};
    }   // namespace detail

    /**
     * Resolve a designated factory by Alias, TGenericClassT and a set of modifiers.
     * @tparam AliasT
     * @tparam TGenericClassT
     * @tparam ListFactoriesT
     * @tparam ListModifiersT
     */
    template<typename AliasT,
             typename DependsOnT,
             typename ListFactoriesT,
             typename ListModifiersT>
    class resolve_factory
    {
        template<typename FactoryT>
        using same_alias_t = detail::same_wrapped_tt<typename FactoryT::alias_tag, AliasT>;

        template<typename FactoryT>
        using same_depends_on_t =
            detail::same_wrapped_tt<typename FactoryT::depends_on_type, DependsOnT>;

        // TODO: implement
        using filtered_type_list = typename filter_type_list_conjunction<ListFactoriesT,
                                                                         same_alias_t,
                                                                         same_depends_on_t>::type;

        static_assert(traits::type_list_size<filtered_type_list>::value <= 1,
                      "Multiple designated factories resolved from Alias and GenericClass");
        static_assert(traits::type_list_size<filtered_type_list>::value > 0,
                      "No designated factories resolved from Alias and GenericClass");

    public:
        using type = typename traits::element_type<0, filtered_type_list>::type;
    };

}   // namespace eld::util
