
#include "lasanga/utility/apply_predicates.h"
#include "lasanga/utility/map_type_list.h"

#include <gtest/gtest.h>

/**
 *
 */

namespace stub
{
    namespace alias
    {
        template<typename...>
        struct state;

        template<typename...>
        struct input_interface;

        template<typename...>
        struct utility;

        template<typename...>
        struct output_interface;
    }   // namespace alias

    struct state
    {
    };

    struct input_interface
    {
    };

    struct utility
    {
    };

    struct output_interface
    {
    };

    struct dummy
    {
    };

    struct barbie
    {
    };

}   // namespace stub

TEST(resolve_name_list, resolve_name)
{

    FAIL();
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}