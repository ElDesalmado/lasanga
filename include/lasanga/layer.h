#pragma once

#include "lasanga/traits.h"

namespace eld
{
    namespace detail
    {
        /**
         * Build logic
         *
         * if is_buildable
         *      if constructor accepts Builder as the first argument
         *          - build from (BuilderT, Args...)
         *       else
         *          - build from (Args...)
         * else
         *      if constructor accepts BuilderT
         *          - build from (Builder)
         *      else
         *          - default construct
         */

        // TODO: add version that adds Builder to the front of the tuple of arguments if T is
        //  constructable in that way
        template<typename T, typename Builder>
        constexpr decltype(auto) build(Builder &&builder, std::true_type /*is_buildable*/)
        {
            return builder()(tag::build<T>());
        }

        template<typename, typename Builder>
        constexpr decltype(auto) build(Builder &&builder, std::false_type /*is_buildable*/)
        {
            return builder;
        }

        template<typename T, typename Builder>
        constexpr decltype(auto) build(Builder &&builder)
        {
            return build<T>(builder, traits::is_buildable<T, Builder>());
        }

        template<typename T, size_t i>
        class component
        {
            constexpr static bool defaultConstructable = std::is_default_constructible_v<T>;

        public:
            using type = typename traits::get_type<T>::type;

            // would it resolve to move constructor?
            template<typename... ArgsT,
                     typename std::enable_if_t<sizeof...(ArgsT) || defaultConstructable, int> * =
                         nullptr>
            explicit component(ArgsT &&...args) : t_(std::forward<ArgsT>(args)...)
            {
            }

            template<typename TupleArgs, size_t... indx>
            component(TupleArgs &&tuple, std::index_sequence<indx...>)
              : component(std::forward<std::decay_t<std::tuple_element_t<indx, TupleArgs>>>(
                    std::get<indx>(tuple))...)
            {
            }

            template<typename Tuple,
                     typename std::enable_if_t<traits::is_tuple<Tuple>::value, int> * = nullptr>
            component(Tuple &&tuple)
              : component(std::forward<Tuple>(tuple),
                          std::make_index_sequence<std::tuple_size_v<Tuple>>())
            {
            }

            const type &get() const { return t_; }

            type &get() { return t_; }

            template<typename Tag,
                     typename = std::enable_if_t<
                         std::is_same_v<type, typename traits::get_type<Tag>::type>>>
            const type &get() const
            {
                return t_;
            };

            template<typename Tag,
                     typename = std::enable_if_t<
                         std::is_same_v<type, typename traits::get_type<Tag>::type>>>
            type &get()
            {
                return t_;
            };

        private:
            type t_;
        };

        template<typename Tuple, typename = std::make_index_sequence<std::tuple_size_v<Tuple>>>
        class layer;

        template<typename... T, size_t... indx>
        class layer<std::tuple<T...>, std::index_sequence<indx...>> : component<T, indx>...
        {
        public:
            layer(const layer &) = delete;
            layer &operator=(const layer &) = delete;

            template<typename BuilderT>
            explicit layer(BuilderT &&builder)
              : component<T, indx>(build<typename component<T, indx>::type>(builder))...
            {
            }
        };
    }   // namespace detail

    template<typename... T>
    using layer = detail::layer<std::tuple<T...>>;


}   // namespace eld
