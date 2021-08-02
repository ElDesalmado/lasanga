
// Your header with names

#include <iostream>

namespace signatures
{
    // not overridden
    template<typename...>
    struct bar;

    // default implementation
    template<typename T, typename...>
    struct foo
    {
        int operator()(const T &t, int i)
        {
            std::cout << "foo default tag: " << i << std::endl;
            return t.foo(i);
        }
    };

}   // namespace signatures

#include "lasanga/custom_override.h"

// generic
template<typename Impl>
class generic_interface
{
public:
    using implementation_type = Impl;

    template<typename... ArgsT>
    generic_interface(ArgsT &&...args)   //
      : impl_(std::forward<ArgsT>(args)...)
    {
    }

    constexpr decltype(auto) bar(double d, const char *str)
    {
        return eld::invoke<signatures::bar>(eld::default_call_tag,
                                            impl_,
                                            d,
                                            str,
                                            // default call
                                            [](auto &obj, double d, const char *str)
                                            { return obj.bar(d, str); });
    }

    constexpr decltype(auto) foo(int i) { return eld::invoke<signatures::foo>(impl_, i); }

private:
    implementation_type impl_;
};

/**
 * Example of a class which methods are not overridden.
 */
class impl_object
{
public:
    int foo(int i) const { return i; }

    void bar(double d, const char *str)
    {
        std::cout << "impl_object::bar called: " << str << ' ' << d << std::endl;
    }
};

/**
 * Example of a class with both methods being overridden by signature tags implementations.
 */
class impl_object_overridden
{
public:
    int foo(int) const
    {
        std::cout
            << "impl_object_overridden::foo is overridden by name tag and will never be called!"
            << std::endl;
        return 42;
    }

    void bar(double, const char *)
    {
        std::cout
            << "impl_object_overridden::bar is overridden by name tag and will never be called!"
            << std::endl;
    }
};

template<>
struct signatures::foo<impl_object_overridden>
{
    int operator()(const impl_object_overridden &, int)
    {
        std::cout << "impl_object_override::foo is overridden by the name tag" << std::endl;
        return 42;
    }
};

template<>
struct signatures::bar<impl_object_overridden>
{
    void operator()(impl_object_overridden &, double d, const char *str)
    {
        std::cout << "impl_object_override::bar is overridden by the name tag: " << str << ' ' << d
                  << std::endl;
    }
};

int main(int, char **)
{
    generic_interface<impl_object> implObject{};
    std::cout << implObject.foo(4) << std::endl;
    implObject.bar(8.15, "oceanic");

    generic_interface<impl_object_overridden> overriddenImpl{};
    std::cout << overriddenImpl.foo(4) << std::endl;
    overriddenImpl.bar(8.15, "oceanic");

    return 0;
}