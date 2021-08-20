#pragma once

namespace eld::util
{
    template<typename FromT, template<typename...> class TToT>
    struct convert_type_list;

    template<template<typename...> class TFromT,
             typename... TypesT,
             template<typename...>
             class TToT>
    struct convert_type_list<TFromT<TypesT...>, TToT>
    {
        using type = TToT<TypesT...>;
    };

    template<typename FromT, template<typename...> class TToT>
    using convert_type_list_t = typename convert_type_list<FromT, TToT>::type;

}   // namespace eld::util
