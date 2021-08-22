
#include "lasanga/utility/type_tree.h"

#include <iostream>
#include <string>

std::string constructed;

template<typename... T>
struct GenericRoot : T...
{
};

template<const char L, size_t Index = 0>
struct Letter
{
    Letter() { constructed += L + std::to_string(Index) + ' '; }
};

int main()
{
    using namespace eld::util;

    GenericRoot<Letter<'A'>, Letter<'B'>, Letter<'C'>, Letter<'D'>>();
    std::cout << constructed << std::endl;
    constructed.clear();

    specialize_tree<
        type_node<GenericRoot, Letter<'A'>, Letter<'B'>, Letter<'C'>, Letter<'D'>>>::type();
    std::cout << constructed << std::endl;
    constructed.clear();

    using nested_type = specialize_tree<
        type_node<GenericRoot,
                  type_node<GenericRoot,
                            Letter<'A', 1>,
                            type_node<GenericRoot, type_node<GenericRoot, Letter<'B', 1>>>>,
                  Letter<'A'>,
                  Letter<'B'>>>::type;


    [[maybe_unused]] nested_type nt{};
    std::cout << constructed << std::endl;
    constructed.clear();

    return 0;
}
