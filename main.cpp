// The MIT License (MIT)
//
// Copyright (c) 2015 Jeremy Letang
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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
    std::cout << std::boolalpha << l::mem(2, a) << std::endl;
    std::cout << std::boolalpha << l::mem(10, a) << std::endl;
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
    l::iter2([](auto e1, auto e2){ std::cout << "e1:" << e1 << ", e2:" << e2 << std::endl;}, a, a);
    auto to_map = cons(1, cons(0, nil));
    l::cons_<float, l::cons_<float, l::nil_t>> m =
        l::map([](auto e){ return static_cast<float>(e * 10); }, to_map);
    l::cons_<float, l::cons_<float, l::nil_t>> mi =
        l::mapi([](std::size_t i, auto e){ return static_cast<float>(e * 10 + i); }, to_map);
}
