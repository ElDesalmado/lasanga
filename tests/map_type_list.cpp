
#include "lasanga/utility/map_type_list.h"

#include <gtest/gtest.h>

template<template<typename...> class, typename...>
struct wrapped_tt;

TEST(map_type_list, is_integral)
{
    FAIL();
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
