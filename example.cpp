#include <iostream>
#include <type_traits>
#include "multicast.hpp"

using std::cout;

void f(int n) { cout << "f(" << n << ")\n"; }

struct g {
    void operator ()(int n) { cout << "g::()(" << n << ")\n"; }
};

void h(double n) { cout << "h(" << n << ")\n"; }

int main() {
    static_assert(
        std::is_same<
            util::multifunction<void(int)>::argument_type,
            int
        >::value,
        "util::multifunction member types are wrong");

    util::multifunction<void(int)> event;

    cout << "Adding f, g, h\n";
    auto f_tok = event += f;
    auto g_tok = event += g();
    auto h_tok = event += h;

    event(1);

    cout << "\nRemoving g\n";
    event -= g_tok;
    event(2);

    cout << "\nRemoving h\n";
    event -= h_tok;
    event(3);

    cout << "\nRemoving g (again!)\n";
    event -= g_tok;
    event(4);

    cout << "\nRemoving f\n";
    event -= f_tok;
    event(5);

    cout << "\nOnce more, with lambdas\n";
    event += [](int n) { cout << "[](" << n << ")\n"; };
    event += [=](int n) { cout << "[=](" << n << ")\n"; };
    event += [&](int n) { cout << "[&](" << n << ")\n"; };
    int n = 6;
    event(n);
}
