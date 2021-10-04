#pragma once

namespace eld::generic
{
    /**
     * Customization point to resolve a fully specialized type for TGenericClassT class template.
     * Results either a complete type/fully specialized class template or a type tree.
     * @tparam BuilderT
     * @tparam TGenericClassT
     * @tparam ModifiersT
     */
    template<typename BuilderT, template<typename...> class TGenericClassT, typename... ModifiersT>
    struct resolve_generic_class;

}   // namespace eld::generic
