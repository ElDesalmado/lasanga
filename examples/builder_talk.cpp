
#include "generic_talk.h"

#include <lasanga/lasanga.h>

// here we create some speakers
struct Person
{
    void speak() { std::cout << "Person" << std::endl; }
};

struct Dog
{
    void speak() { std::cout << "Dog" << std::endl; }
};

struct Cat
{
    void speak() { std::cout << "Cat" << std::endl; }
};

// factories

struct create_cat
{
    Cat operator()() { return {}; }
};

Person create_person() { return {}; }

int main(int, char **)
{
    // create and customize a builder. The order of arguments does not matter
    auto builder = eld::make_builder(eld::named_factory<alias::C, create_cat, Crowd>(),
                                     eld::wrap_factory([]() { return NonTemplate(); }),
                                     eld::named_factory<alias::A>(&create_person),
                                     eld::named_factory<alias::B>([]() { return Dog(); }));

    auto crowdA = eld::make_lasanga<Crowd>(builder);
    crowdA.speak_all();
    /* Output:
     * Person
     * Dog
     * Cat
     * NonTemplate
     */
    std::cout << std::endl << std::endl;

    // We can, however, customize our builder and map aliases to different classes
    auto customizedBuilder =
        eld::make_builder(eld::named_factory<alias::C, create_cat, Crowd>(),
                          eld::wrap_factory([]() { return NonTemplate(); }),
                          eld::named_factory<alias::B>(&create_person),
                          eld::named_factory<alias::A>([]() { return Dog(); }));

    auto crowdB = eld::make_lasanga<Crowd>(customizedBuilder);
    crowdB.speak_all();
    /* Output:
     * Dog
     * Person
     * Cat
     * NonTemplate
     */

    return 0;
}
