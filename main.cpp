#include <list.h>
#include <iostream>

template <typename L>
auto add_element(L l) {
    std::cout << "list size before add_one: " << l::length<decltype(l)>::value << std::endl;
    auto _ = cons(42, l);
    std::cout << "list size after add_element: " << l::length<decltype(_)>::value << std::endl;
}

int main() {
    auto a = cons(4, cons(3, cons(2, cons(1, cons(0, nil)))));
    auto b = l::rev(a);
    auto c = l::append(a, cons(10, cons(11, cons(12, cons(13, cons(14))))));
    auto d = l::rev_append(a, cons(10, cons(11, cons(12, cons(13, cons(14))))));
    auto e = cons(std::make_tuple('a', 1), cons(std::make_tuple('b', 42), cons(std::make_tuple('c', 84))));
    int i = l::assoc('c', e);

    std::cout << a << std::endl;
    std::cout << b << std::endl;
    std::cout << c << std::endl;
    std::cout << d << std::endl;
    std::cout << std::boolalpha << l::exists([](auto i) {return i == 3;}, a) << std::endl;
    std::cout << std::boolalpha << l::exists([](auto i) {return i == 42;}, a) << std::endl;
    std::cout << std::boolalpha << l::for_all([](auto i) {return i < 5;}, a) << std::endl;
    std::cout << std::boolalpha << l::for_all([](auto i) {return i > 5;}, a) << std::endl;
    std::cout << "assoc 'c' in e: " << i << std::endl;
    std::cout << "find 3 - 1 in a: " << l::find([](auto i) {return i == (3-1); }, a) << std::endl;
    std::cout << "a len: " << l::length<decltype(a)>::value << std::endl;
    std::cout << "c len: " << l::length<decltype(a)>::value << std::endl;
    std::cout << "cons(42) len: " << l::length<decltype(cons(42))>::value << std::endl;
    std::cout << "hd a: " << l::hd(a) << std::endl;
    std::cout << "tl a: " << l::tl(a) << std::endl;
    std::cout << "nth(0) a: " << l::nth<0>(a) << std::endl;
    std::cout << "nth(3) a: " << l::nth<3>(a) << std::endl;
    std::cout << "nth(4) a: " << l::nth<4>(a) << std::endl;
    add_element(nil);
    add_element(empty);
    add_element(cons(20, cons(10)));
    l::iteri([](std::size_t i, auto e){ std::cout << "i: " << i << " -> " << e << std::endl;}, a);
    // auto m = l::map([](auto e) -> float { return static_cast<float>(e * 10); }, a);
}
