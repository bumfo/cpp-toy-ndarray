#ifndef VARIADIC_HPP
#define VARIADIC_HPP

#include <iostream>


namespace toy {
namespace variadic {


constexpr int prod() {
  return 1;
}

template <typename... Args>
constexpr int prod(int arg, Args... args) {
  return arg * prod(args...);
}


int __suffix_prod(int * buf) {
  buf[0] = 1;
  return 1;
}

template <typename... Args>
int __suffix_prod(int * buf, int arg, Args... args) {
  buf[0] = arg * __suffix_prod(buf + 1, args...);
  return buf[0];
}

int * suffix_prod() {
  return new int[1]{1};
}

template <typename... Args>
int * suffix_prod(Args... args) {
  int * buf = new int[sizeof...(args) + 1];
  __suffix_prod(buf, args...);
  return buf;
}


template <typename T>
struct values {
  template <T... ts>
  struct type {
  };

  template <bool>
  static void print(std::ostream & os) {
  }

  template <bool, T t, T... ts>
  static void print(std::ostream & os) {
    os << ' ' << t;
    print<true, ts...>(os);
  }

  template <bool>
  static void print(std::ostream & os, bool) {
  }

  template <bool, T t, T... ts>
  static void print(std::ostream & os, bool) {
    os << t;
    print<true, ts...>(os);
  }

  template <T... ts>
  friend std::ostream & operator << (std::ostream & os, type<ts...> const & s) {
    os.put('[');
    print<true, ts...>(os, true);
    os.put(']');
    return os;
  }
};

template <int... ts>
using ints = values<int>::type<ts...>;


template <typename...>
struct reverse;

template <
  template <int...> typename T,
  int... rs>
struct reverse<T<>, T<rs...>> {
  using type = T<rs...>;
};

template <
  template <int...> typename T,
  int t,
  int... ts,
  int... rs>
struct reverse<T<t, ts...>, T<rs...>> : reverse<T<ts...>, T<t, rs...>> {
};

template <
  template <int...> typename T,
  int... ts>
struct reverse<T<ts...>> : reverse<T<ts...>, T<>>  {
};


template <typename...>
struct suffix_product_t;

template <
  template <int...> typename T,
  int... ts>
struct suffix_product_t<T<ts...>> : suffix_product_t<T<>, typename reverse<T<ts...>>::type, T<1>> {
};

template <
  template <int...> typename T,
  int t,
  int... ts,
  int r,
  int... rs>
struct suffix_product_t<T<>, T<t, ts...>, T<r, rs...>> : suffix_product_t<T<>, T<ts...>, T<t * r, r, rs...>> {
};

template <
  template <int...> typename T,
  int... rs>
struct suffix_product_t<T<>, T<>, T<rs...>> {
  using type = T<rs...>;
};

template <int... ts>
using suffix_product = typename suffix_product_t<ints<ts...>>::type;


} // variadic
} // toy


#endif // VARIADIC_HPP
