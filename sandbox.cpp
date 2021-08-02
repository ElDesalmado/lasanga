#include "lasanga/custom_override.h"
#include "lasanga/lasanga.h"

struct get_bar;

struct foo
{
    int get_bar() { return 42; }
};

int main(int, char **)
{
    foo f;
//    eld::detail::get_tuple_element<get_bar>(f, std::false_type());

    return 0;
}
