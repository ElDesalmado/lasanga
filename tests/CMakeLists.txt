link_libraries(eld::lasanga GTest::gtest)

macro(eld_add_test src_name)
    add_executable(test.${src_name} ${src_name}.cpp)
    add_test(NAME test.${src_name}
            COMMAND test.${src_name})
endmacro()

eld_add_test(resolve_name_list)
eld_add_test(apply_predicates)