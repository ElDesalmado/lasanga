#pragma once

#include "lasanga/get_alias_list.h"
#include "lasanga/impl/resolve_generic_class.h"
#include "lasanga/utility/resolve_factory.h"

#include "lasanga/utility.h"

#include <tuple>
#include <utility>

/**
 * This is a default implementation for the generic::builder.
 */

namespace eld::impl
{
    template<typename... DesignatedFactoriesT>
    class builder
    {
    public:
        using factories_list = util::type_list<DesignatedFactoriesT...>;

        template<typename... XDesignatedFactoriesT>
        constexpr explicit builder(XDesignatedFactoriesT &&...factories)
          : factories_(std::forward<XDesignatedFactoriesT>(factories)...)
        {
        }

        template<typename AliasTagT,
                 template<typename...>
                 class TDependsOnT,
                 typename... Modifiers,
                 typename... ArgsT>
        decltype(auto) operator()(d_alias_t<AliasTagT, TDependsOnT, Modifiers...>, ArgsT &&...args)
        {
            using resolved_factory =
                typename util::resolve_factory<AliasTagT,
                                               wrapped_tt<TDependsOnT>,
                                               factories_list,
                                               util::type_list<Modifiers...>>::type;

            return construct<resolved_factory>(*this)(std::forward<ArgsT>(args)...);
        }

        template<typename AliasTagT, typename... ArgsT>
        decltype(auto) operator()(eld::alias_t<AliasTagT>, ArgsT &&...args)
        {
            // TODO: use updated version of resolve_factory
            return factory_by_alias<AliasTagT>()(factories_)(std::forward<ArgsT>(args)...);
        }

    private:

        template <typename AliasTagT>
        struct factory_by_alias;

        template<typename FactoryT, typename = typename std::decay_t<FactoryT>::type>
        struct construct;

    private:
        std::tuple<DesignatedFactoriesT...> factories_;
    };

}   // namespace eld::impl

#include "lasanga/impl/builder.ipp"