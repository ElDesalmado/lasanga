
#include <lasanga/lasanga.h>

#include <atomic>
#include <cstdint>
#include <functional>

namespace eos::alias
{
    template<typename...>
    struct logger
    {
    };

    template<typename...>
    struct traits
    {
    };

    template<typename...>
    struct executor
    {
    };
}   // namespace eos::alias

namespace eos::generic::algorithm
{
    namespace alias
    {
        /**
         * Customization point to get current state interface.
         */
        template<typename...>
        struct state;

        /**
         * Customization point to get input interface.
         */
        template<typename...>
        struct input_interface;

        template<typename...>
        struct utility;

        /**
         * Customization point to get output interface.
         */
        template<typename...>
        struct output_interface;
    }   // namespace alias

    /**
     *
     * @tparam StateT Service State Interface. Provides algorithm Mode manipulation.
     * @tparam InputInterfaceT Input Interface. Provides access to current input.
     * @tparam UtilityT Utility Interface.
     * @tparam TraitsT
     */
    template<typename StateT,
        typename InputInterfaceT/*,
        typename UtilityT,
        typename TraitsT,
        typename LoggerT*/>
        class iteration
    {
    public:
        using state_type = StateT;
        using input_interface_type = InputInterfaceT;
        //        using utility_type = UtilityT;
        //        using traits = TraitsT;
        //        //        using logger_type = LoggerT;
        //
        //        using iteration_result_type = typename traits::iteration_result_type;
        //        using steering_type = typename traits::steering_type;

        template<typename BuilderT>
        constexpr explicit iteration(BuilderT &&builder)
          : state_(builder(eld::d_name_tag<alias::state, algorithm::iteration>())) /*,
             inputInterface_(
                 builder(eld::d_name_tag<alias::input_interface, algorithm::iteration>())),
             utility_(builder(eld::d_name_tag<alias::utility, algorithm::iteration>())),
             outputInterface_(
                 builder(eld::d_name_tag<alias::output_interface, algorithm::iteration>()))*/
        // TODO: add logger
        {
        }

    private:
        state_type state_;
        input_interface_type inputInterface_;
        //        utility_type utility_;
        //
        //        std::function<void(steering_type)> outputInterface_;
    };

}   // namespace eos::generic::algorithm

// TODO: deduce it from the class
template<>
struct eld::get_alias_list<eos::generic::algorithm::iteration>
{
    using type = eld::util::type_list<eld::wrapped_tt<eos::generic::algorithm::alias::state>,
    eld::wrapped_tt<eos::generic::algorithm::alias::input_interface>/*,
    eld::wrapped_tt<eos::generic::algorithm::alias::utility>,
    eld::wrapped_tt<eos::alias::traits>,
    eld::wrapped_tt<eos::alias::logger>*/>;
};

namespace eos::stubs
{
    using Percent = float;

    struct Steering
    {
        Percent x,   //
            y;
    };

    constexpr inline bool operator==(const Steering &lhv, const Steering &rhv)
    {
        return std::tie(lhv.x, lhv.y) == std::tie(rhv.x, rhv.y);
    }

    enum class IterationResult
    {
        success,
        warning,
        error
    };

    enum class ModeType : uint8_t
    {
        idle,
        manual,
        tracking,
        compensating
    };

    struct Traits
    {
        using mode_type = ModeType;
        using iteration_result_type = IterationResult;
        using steering_type = Steering;
    };

    struct Point
    {
    };

    struct Vehicle
    {
    };

    struct EOS
    {
    };

    struct Utility
    {
        Steering calc_focus_steering(const Point &, const Vehicle &, const EOS &) { return {}; }

        Point get_look_at(const Vehicle &, const EOS &) { return {}; }

        Point adjust_point(const Point &, const Steering &) { return {}; }
    };

    struct Input
    {
        Steering steering() const { return {}; }
        Point point() const { return {}; }
        Vehicle vehicle() const { return {}; }
        EOS eos() const { return {}; }
    };

    struct State
    {
        ModeType mode() const { return m_modeType; }
        void switch_mode(ModeType modeType) { m_modeType = modeType; }
        ModeType m_modeType = ModeType::manual;
    };

    template<typename InputT>
    struct InputInterface
    {
        InputT input() { return {}; }
    };

}   // namespace eos::stubs

/////////////////////////////////////////////////

// template<typename InputT, typename Callable>
// auto make_builder(Callable &&onIteration)
//{
//     namespace algorithm = eos::generic::algorithm;
//     namespace alias = eos::generic::algorithm::alias;
//
//     return eld::make_builder(
//         eld::d_named_factory<alias::state, algorithm::iteration>([]()
//         { return eos::stubs::State(); }),
// eld::d_named_factory<alias::input_interface, algorithm::iteration>(
//[]() { return eos::stubs::InputInterface<InputT>(); })/*,
// eld::d_named_factory<alias::utility, algorithm::iteration>(
//[]() { return eos::stubs::Utility(); }),
// eld::d_named_factory<alias::output_interface, algorithm::iteration>(
//[onIteration = std::forward<Callable>(onIteration)]() mutable { return onIteration; }),
// eld::named_factory<eos::alias::traits>([]() { return eos::stubs::Traits(); })*/);
// }

int main(int, char **)
{
    namespace algorithm = eos::generic::algorithm;
    namespace alias = eos::generic::algorithm::alias;



    std::atomic_bool called{ false };

    //    auto builder = make_builder<eos::stubs::Input>([&called](auto &&) { called = true; });
    //
    //    auto iteration = eld::make_lasanga<algorithm::iteration>(builder);
    return 0;
}
