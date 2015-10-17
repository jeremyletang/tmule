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

#ifndef IMM_LIST_H
#define IMM_LIST_H

#include <type_traits>
#include <functional>
#include <iostream>

namespace l {

class not_found: public std::exception {
public:
    std::string what_;
    not_found() = default;
    explicit not_found(const std::string& what_arg)
    : what_(what_arg) {}
    virtual const char* what() const throw()
    { if (what_ == "") {return "not_found"; } else { return what_.c_str(); }}
};

struct nil_t {};
struct empty_t {};

template <typename Head, typename Tail>
struct cons_ {
    using head_type = Head;
    using tail_type = Tail;
    cons_() = delete;
    constexpr explicit cons_(const Head& h, const Tail& t)
    : h(h), t(t) {}
    Head h;
    Tail t;
};

// is list helper

template <typename T>
struct is_imm_list : std::false_type {};
template <typename Head, typename Tail>
struct is_imm_list<cons_<Head, Tail>> : std::true_type {};

// not_nil list helper

template <typename Head, typename Tail>
struct is_not_nil_t {
    static const bool value =
        !std::is_same<nil_t, typename std::remove_cv<Head>::type>::value &&
        !std::is_same<nil_t, typename std::remove_cv<Tail>::type>::value;
};

template <typename Head, typename Tail>
struct is_not_empty_t {
    static const bool value =
        !std::is_same<empty_t, typename std::remove_cv<Head>::type>::value &&
        !std::is_same<empty_t, typename std::remove_cv<Tail>::type>::value;
};

// get the list size

template <std::size_t N, typename T>
struct length_ {};

template <std::size_t N, typename Head, typename Tail>
struct length_<N, cons_<Head, Tail>> {
    static constexpr std::size_t value = length_<N + 1, Tail>::value;
};

template <std::size_t N, typename Head>
struct length_<N, cons_<Head, nil_t>> {
    static constexpr std::size_t value = N + 1;
};

template <typename T>
struct length {
    static constexpr std::size_t value = length_<0, T>::value;
};

template <>
struct length<nil_t> {
    static constexpr std::size_t value = 0;
};

template <>
struct length<empty_t> {
    static constexpr std::size_t value = 0;
};

// is pos in list length
template <typename L, std::size_t N>
struct is_valid_list_pos {
    static constexpr bool value =
        length<L>::value > N;
};

// make a new list type from a type and a list

template <typename T, std::size_t N>
struct list_type_from_size {
    using type = cons_<T, typename list_type_from_size<T, N-1>::type>;
};

template <typename Head>
struct list_type_from_size<Head, 1> {
    using type = cons_<Head, nil_t>;
};

template <typename Head>
struct list_type_from_size<Head, 0> {
    using type = cons_<Head, nil_t>;
};

// nth

template <std::size_t N,
          typename L,
          std::enable_if_t<l::is_imm_list<L>::value>* = nullptr,
          std::enable_if_t<l::is_valid_list_pos<L, N>::value>* = nullptr,
          std::enable_if_t<N >= 1>* = nullptr>
constexpr auto nth(const L& l) noexcept -> typename L::head_type
{ return nth<N-1>(l.t); }

// end nth
template <std::size_t N,
          typename L,
          std::enable_if_t<l::is_imm_list<L>::value>* = nullptr,
          std::enable_if_t<N == 0>* = nullptr>
constexpr auto nth(const L& l) noexcept -> typename L::head_type
{ return l.h; }


// iter

template <typename L,
          typename Fn,
          std::enable_if_t<l::is_imm_list<L>::value>* = nullptr>
constexpr void iter(Fn f, const L& l) {
    f(l.h);
    iter(f, l.t);
}

template <typename Fn>
constexpr void iter(Fn f, const nil_t) {}

// iter2

template <typename L1,
          typename L2,
          typename Fn,
          std::enable_if_t<l::is_imm_list<L1>::value>* = nullptr,
          std::enable_if_t<l::is_imm_list<L2>::value>* = nullptr,
          std::enable_if_t<std::is_same<typename L1::head_type,
                                        typename L2::head_type>::value>* = nullptr>
constexpr void iter2(Fn f, const L1& l1, const L2& l2) {
    f(l1.h, l2.h);
    iter2(f, l1.t, l2.t);
}

template <typename Fn>
constexpr void iter2(Fn, nil_t, nil_t) {}


// iteri

template <std::size_t N,
          typename L,
          typename Fn,
          std::enable_if_t<l::is_imm_list<L>::value>* = nullptr>
constexpr void iteri_(Fn f, const L& l) {
    f(N, l.h);
    iteri_<N+1>(f, l.t);
}

template <std::size_t N, typename Fn>
constexpr void iteri_(Fn f, const nil_t) {}

template <typename L,
          typename Fn,
          std::enable_if_t<l::is_imm_list<L>::value>* = nullptr>
constexpr void iteri(Fn f, const L& l) {
    iteri_<0>(f, l);
}

// rev list

// implementation rev
template <typename From,
          typename To>
constexpr auto rev_(From f, To t) noexcept -> decltype(rev_(f.t, l::cons_<decltype(f.h), To>(f.h, t)))
{ return rev_(f.t, l::cons_<decltype(f.h), To>(f.h, t)); }

template <typename Head,
          typename To>
constexpr cons_<Head, To> rev_(const cons_<Head, nil_t>& f, To t) noexcept
{ return cons_<Head, To>(f.h, t); }

// rev list of more than one element
template <typename L,
          std::enable_if_t<l::is_imm_list<L>::value>* = nullptr,
          std::enable_if_t<l::is_not_nil_t<L, L>::value>* = nullptr>
constexpr L rev(const L& l) noexcept
{ return rev_(l.t, l::cons_<decltype(l.h), l::nil_t>(l.h, l::nil_t{})); }

// explictly one element
template <typename Head>
cons_<Head, nil_t> rev(const cons_<Head, nil_t>& l) noexcept
{ return l; }

// map
template <typename Fn,
          typename From,
          typename To>
constexpr auto map_(Fn f, From fl, To t) noexcept
    -> decltype(map_(f, fl.t, cons_<decltype(f(fl.h)), To>(f(fl.h), t)))
{ return map_(f, fl.t, l::cons_<decltype(f(fl.h)), To>(f(fl.h), t)); }

template <typename Fn,
          typename Head,
          typename To>
auto map_(Fn f, const cons_<Head, nil_t>& l, To t) noexcept -> cons_<decltype(f(l.h)), To>
{ return cons_<decltype(f(l.h)), To>(f(l.h), t); }

// map list of more than one element
template <typename Fn,
          typename L,
          std::enable_if_t<l::is_imm_list<L>::value>* = nullptr,
          std::enable_if_t<l::is_not_nil_t<L, L>::value>* = nullptr>
auto map(Fn f, const L& l) noexcept
    -> decltype(map_(f, l.t, cons_<decltype(f(l.h)), nil_t>(f(l.h), l::nil_t{})))
{ return map_(f, l.t, l::cons_<decltype(f(l.h)), l::nil_t>(f(l.h), l::nil_t{})); }

// explictly one element
template <typename Fn,
          typename Head>
auto map(Fn f, const cons_<Head, nil_t>& l) noexcept -> cons_<decltype(f(l.h)), nil_t>
{ return cons_<decltype(f(l.h)), nil_t>(f(l.h), nil_t{}); }




// map2
template <typename Fn,
          typename From1,
          typename From2,
          typename To>
constexpr auto map2_(Fn f, const From1& f1, const From2& f2, const To& t) noexcept
    -> decltype(map2_(f, f1.t, f2.t, cons_<decltype(f(f1.h, f2.h)), To>(f(f1.h, f2.h), t)))
{ return map2_(f, f1.t, f2.t, l::cons_<decltype(f(f1.h, f2.h)), To>(f(f1.h. f2.h), t)); }

template <typename Fn,
          typename Head1,
          typename Head2,
          typename To>
auto map2_(Fn f, const cons_<Head1, nil_t>& l1, const cons_<Head2, nil_t>& l2, const To& t) noexcept
    -> cons_<decltype(f(l1.h, l2.h)), To>
{ return cons_<decltype(f(l1.h, l2.h)), To>(f(l1.h, l2.h), t); }

// map list of more than one element
template <typename Fn,
          typename L1,
          typename L2,
          std::enable_if_t<l::is_imm_list<L1>::value>* = nullptr,
          std::enable_if_t<l::is_imm_list<L2>::value>* = nullptr,
          std::enable_if_t<l::is_not_nil_t<L1, L2>::value>* = nullptr,
          std::enable_if_t<l::length<L1>::value == l::length<L2>::value>* = nullptr>
auto map2(Fn f, const L1& l1, const L2& l2) noexcept
    -> decltype(map2_(f, l1.t, l2.t, cons_<decltype(f(l1.h, l2.h)), nil_t>(f(l1.h, l2.h), l::nil_t{})))
{ return map2_(f, l1.t, l2.t, cons_<decltype(f(l1.h, l2.h)), nil_t>(f(l1.h, l2.h), nil_t{})); }

// explictly one element
template <typename Fn,
          typename Head1,
          typename Head2>
auto map2(Fn f, const cons_<Head1, nil_t>& l1, const cons_<Head2, nil_t>& l2) noexcept
    -> cons_<decltype(f(l1.h, l2.h)), nil_t>
{ return cons_<decltype(f(l1.h, l2.h)), nil_t>(f(l1.h, l2.h), nil_t{}); }

// mapi
template <std::size_t N,
          typename Fn,
          typename From,
          typename To>
auto mapi_(Fn f, From fl, To t) noexcept
    // FIXME: not able to delcare this type.
    // -> decltype(mapi_<N+1>(f, fl.t, cons_<decltype(f(N, fl.h)), To>(f(N, fl.h), t)))
{ return mapi_<N+1>(f, fl.t, cons_<decltype(f(N, fl.h)), To>(f(N, fl.h), t)); }

template <std::size_t N,
          typename Fn,
          typename Head,
          typename To>
auto mapi_(Fn f, const cons_<Head, nil_t>& l, To t) noexcept -> cons_<decltype(f(N, l.h)), To>
{ return cons_<decltype(f(N, l.h)), To>(f(N, l.h), t); }

// mapi list of more than one element
template <typename Fn,
          typename L,
          std::enable_if_t<l::is_imm_list<L>::value>* = nullptr,
          std::enable_if_t<l::is_not_nil_t<L, L>::value>* = nullptr>
auto mapi(Fn f, const L& l) noexcept
    -> decltype(mapi_<1>(f, l.t, cons_<decltype(f(0, l.h)), nil_t>(f(0, l.h), l::nil_t{})))
{ return mapi_<1>(f, l.t, l::cons_<decltype(f(0, l.h)), l::nil_t>(f(0, l.h), l::nil_t{})); }

// explictly one element
template <typename Fn, typename Head>
auto mapi(Fn f, const cons_<Head, nil_t>& l) noexcept -> cons_<decltype(f(0, l.h)), nil_t>
{ return cons_<decltype(f(0, l.h)), nil_t>(f(0, l.h), nil_t{}); }

template <typename L1,
          typename L2,
          std::enable_if_t<length<L1>::value >= 1>* = nullptr>
auto append_(const L1& l1, const L2& l2) noexcept
    -> decltype(append_(l1.t, cons_<decltype(l1.h), L2>(l1.h, l2)))
{ return append_(l1.t, cons_<decltype(l1.h), L2>(l1.h, l2)); }

template <typename L1,
          typename L2,
          std::enable_if_t<length<L1>::value == 0>* = nullptr>
L2 append_(const L1& l1, const L2& l2) noexcept
{ return l2; }

template <typename L1,
          typename L2,
          std::enable_if_t<l::is_imm_list<L1>::value>* = nullptr,
          std::enable_if_t<l::is_imm_list<L2>::value>* = nullptr,
          std::enable_if_t<std::is_same<typename L1::head_type,
                                        typename L2::head_type>::value>* = nullptr>
auto append(const L1& l1, const L2& l2) noexcept -> decltype(append_(rev(l1), l2))
{ return append_(rev(l1), l2); }

template <typename L1,
          typename L2,
          std::enable_if_t<l::is_imm_list<L1>::value>* = nullptr,
          std::enable_if_t<l::is_imm_list<L2>::value>* = nullptr,
          std::enable_if_t<std::is_same<typename L1::head_type,
                                        typename L2::head_type>::value>* = nullptr>
auto rev_append(const L1& l1, const L2& l2) noexcept -> decltype(append_(rev(l1), l2))
{ return append_(l1, l2); }

template <typename L,
          std::enable_if_t<l::is_imm_list<L>::value>* = nullptr>
auto hd(const L& l) noexcept -> typename L::head_type
{ return l.h; }

template <typename L,
          std::enable_if_t<l::is_imm_list<L>::value>* = nullptr>
auto tl(const L& l) noexcept -> typename L::tail_type
{ return l.t; }

// for_all
template <typename L,
          typename Fn,
          std::enable_if_t<l::is_imm_list<L>::value>* = nullptr,
          std::enable_if_t<std::is_convertible<Fn,
             std::function<bool(typename L::head_type)>>::value>* = nullptr>
bool for_all(Fn f, const L& l) noexcept
{ return f(l.h) && for_all(f, l.t); }

// for_all end recursion
template <typename Fn>
bool for_all(Fn f, nil_t) noexcept
{ return true; }

// mem
template <typename A,
          typename L,
          std::enable_if_t<l::is_imm_list<L>::value>* = nullptr,
          std::enable_if_t<std::is_same<A, typename L::head_type>::value>* = nullptr>
bool mem(const A& a, const L& l) noexcept
{ return a == l.h || mem(a, l.t); }

template <typename A>
bool mem(const A&, nil_t) noexcept
{ return false; }

// for_all2
template <typename L1,
          typename L2,
          typename Fn,
          std::enable_if_t<l::is_imm_list<L1>::value>* = nullptr,
          std::enable_if_t<l::is_imm_list<L2>::value>* = nullptr,
          std::enable_if_t<l::length<L1>::value == l::length<L2>::value>* = nullptr,
          std::enable_if_t<std::is_convertible<Fn,
             std::function<bool(typename L1::head_type,
                                typename L2::head_type)>>::value>* = nullptr>
bool for_all2(Fn f, const L1& l1, const L2& l2) noexcept
{ return f(l1.h, l2.h) && for_all(f, l1.t, l2.t); }

// for_all2 end recursion
template <typename Fn>
bool for_all2(Fn f, nil_t, nil_t) noexcept
{ return true; }

// exists
template <typename L,
          typename Fn,
          std::enable_if_t<l::is_imm_list<L>::value>* = nullptr,
          std::enable_if_t<std::is_convertible<Fn,
              std::function<bool(typename L::head_type)>>::value>* = nullptr>
bool exists(Fn f, const L& l) noexcept
{ return f(l.h) || exists(f, l.t); }

// exists end recursion
template <typename Fn>
bool exists(Fn f, nil_t) noexcept
{ return false; }

// exists
template <typename L1,
          typename L2,
          typename Fn,
          std::enable_if_t<l::is_imm_list<L1>::value>* = nullptr,
          std::enable_if_t<l::is_imm_list<L2>::value>* = nullptr,
          std::enable_if_t<l::length<L1>::value == l::length<L2>::value>* = nullptr,
          std::enable_if_t<std::is_convertible<Fn,
              std::function<bool(typename L1::head_type,
                                 typename L2::head_type)>>::value>* = nullptr>
bool exists2(Fn f, const L1& l1, const L2& l2) noexcept {
    return f(l1.h, l2.h) || exists2(f, l1.t, l2.t);
}

// exists end recursion
template <typename Fn>
bool exists2(Fn f, nil_t, nil_t) noexcept
{ return false; }


// find
template <typename L,
          typename Fn,
          std::enable_if_t<l::is_imm_list<L>::value>* = nullptr,
          std::enable_if_t<!std::is_same<nil_t, typename L::tail_type>::value>* = nullptr,
          std::enable_if_t<std::is_convertible<Fn,
              std::function<bool(typename L::head_type)>>::value>* = nullptr>
auto find(Fn f, const L& l) noexcept -> typename L::head_type {
    if (f(l.h)) { return l.h; }
    else { return find(f, l.t); }
}


// find recursion end
template <typename L,
          typename Fn,
          std::enable_if_t<l::is_imm_list<L>::value>* = nullptr,
          std::enable_if_t<std::is_same<nil_t, typename L::tail_type>::value>* = nullptr>
auto find(Fn f, const L& l) -> typename L::head_type {
    if (not f(l.h)) { throw not_found{}; }
    return l.h;
}

template <typename A,
          typename B,
          typename Tail,
          std::enable_if_t<!std::is_same<nil_t, Tail>::value>* = nullptr>
auto assoc(const A& a, const cons_<std::tuple<A, B>, Tail>& l) -> B {
    if (std::get<0>(l.h) == a) { return std::get<1>(l.h); }
    return assoc(a, l.t);
}

// assoc recursion end
template <typename A,
          typename B>
auto assoc(const A& a, const cons_<std::tuple<A, B>, nil_t>& l) -> B {
    if (std::get<0>(l.h) != a) { throw not_found{}; }
    return std::get<1>(l.h);
}

template <typename A,
          typename B,
          typename Tail,
          std::enable_if_t<!std::is_same<nil_t, Tail>::value>* = nullptr>
bool mem_assoc(const A& a, const cons_<std::tuple<A, B>, Tail>& l) noexcept {
    if (std::get<0>(l.h) == a) { return true; }
    return mew_assoc(a, l.t);
}

// assoc recursion end
template <typename A,
          typename B>
bool mem_assoc(const A& a, const cons_<std::tuple<A, B>, nil_t>& l) noexcept {
    if (std::get<0>(l.h) != a) { return false; }
    return true;
}

} // f

static constexpr l::nil_t nil{};
static constexpr l::empty_t empty{};

// construct from a element and a list
template<typename Head,
         typename Tail,
         std::enable_if_t<l::is_not_nil_t<Head, Tail>::value>* = nullptr,
         std::enable_if_t<l::is_not_empty_t<Head, Tail>::value>* = nullptr,
         std::enable_if_t<std::is_same<Head, typename Tail::head_type>::value>* = nullptr>
constexpr l::cons_<Head, Tail> cons(const Head& h, const Tail& t)
{ return std::move(l::cons_<Head, Tail>(h, t)); }

// contruct from an element + nil
template<typename Head>
constexpr l::cons_<Head, l::nil_t> cons(const Head& h, l::nil_t n)
{ return std::move(l::cons_<Head, l::nil_t>(h, n)); }

// constructor from an empty list
template<typename Head>
constexpr l::cons_<Head, l::nil_t> cons(const Head& h, l::empty_t e)
{ return std::move(l::cons_<Head, l::nil_t>(h, nil)); }

// constructor from an element
template <typename Head>
constexpr l::cons_<Head, l::nil_t> cons(const Head& h)
{ return std::move(l::cons_<Head, l::nil_t>(h, nil)); }

template <typename Head,
          typename Tail>
std::ostream& operator<<(std::ostream& os, const l::cons_<Head, Tail>& l) {
    os << "[";
    std::size_t i = l::length<l::cons_<Head, Tail>>::value;
    iter([&](Head e) {
        i -= 1;
        os << e;
        if (i != 0) { os << ", "; };
    }, l);
    os << "]";
    return os;
}

#endif // IMM_LIST_H