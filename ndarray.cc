#include <iostream>

constexpr int prod() {
  return 1;
}

template <typename... Args>
constexpr int prod(int arg, Args... args) {
  return arg * prod(args...);
}


int __partial_prod(int * buf) {
  buf[0] = 1;
  return 1;
}

template <typename... Args>
int __partial_prod(int * buf, int arg, Args... args) {
  buf[0] = arg * __partial_prod(buf + 1, args...);
  return buf[0];
}

int * partial_prod() {
  return new int[1]{1};
}

template <typename... Args>
int * partial_prod(Args... args) {
  int * buf = new int[sizeof...(args) + 1];
  __partial_prod(buf, args...);
  return buf;
}


template <typename T>
class slice {
  T * _mem;
  int _size;

public:
  slice(T * mem, int size) : _mem(mem), _size(size) {
  }

  T & operator[] (int i) {
    return _mem[i];
  }

  T const & operator[] (int i) const {
    return _mem[i];
  }

  bool empty() const {
    return _size == 0;
  }

  int size() const {
    return _size;
  }

  friend std::ostream & operator << (std::ostream & os, slice<T> const & s) {
    if (s.empty()) {
      os << "[]";
    } else {
      os << '[' << s[0];
      for (int i = 1, n = s.size(); i < n; ++i) {
        os << ' ' << s[i];
      }
      os << ']';
    }
    return os;
  }
};

template <typename T, int... Sizes>
class subscriptor;

template <typename T, int Size, int ElSize, int... Sizes>
class subscriptor<T, Size, ElSize, Sizes...> {
  T * _base;
  int _offset;

public:
  constexpr subscriptor(T * base, int offset) : _base(base), _offset(offset) {
  }

  constexpr auto operator[] (int i) {
    return subscriptor<T, ElSize, Sizes...>(_base, _offset + i * ElSize);
  }
};

template <typename T, int Size>
class subscriptor<T, Size, 1> {
  T * _base;
  int _offset;

public:
  constexpr subscriptor(T * base, int offset) : _base(base), _offset(offset) {
  }

  constexpr T & operator[] (int i) {
    return _base[_offset + i];
  }

  constexpr int offset(int i) const {
    return _offset + i;
  }
};

template <typename T>
struct variadic_values {
  template <T... ts>
  struct type {
  };
};

template <int... ints>
using variadic_ints = variadic_values<int>::type<ints...>;

template <typename...>
struct variadic_uncurrying;

template <
  template <int...> typename T,
  template <typename, int...> typename R,
  typename Type,
  int... ints>
struct variadic_uncurrying<T<ints...>, R<Type>> {
  using type = R<Type, ints...>;
};

template <typename...>
struct variadic_reverse;

template <
  template <int...> typename T,
  template <int...> typename R,
  int... rs>
struct variadic_reverse<T<>, R<rs...>> {
  using type = R<rs...>;
};

template <
  template <int...> typename T,
  template <int...> typename R,
  int t,
  int... ts,
  int... rs>
struct variadic_reverse<T<t, ts...>, R<rs...>> : variadic_reverse<T<ts...>, R<t, rs...>> {
};

template <
  template <int...> typename T,
  int... ts>
struct variadic_reverse<T<ts...>> : variadic_reverse<T<ts...>, T<>>  {
};


template <typename...>
struct partial_product;

template <
  template <int...> typename T,
  int... ts>
struct partial_product<T<ts...>> : partial_product<T<>, typename variadic_reverse<T<ts...>>::type, T<1>> {
};

template <
  template <int...> typename T,
  int t,
  int... ts,
  int r,
  int... rs>
struct partial_product<T<>, T<t, ts...>, T<r, rs...>> : partial_product<T<>, T<ts...>, T<t * r, r, rs...>> {
};

template <
  template <int...> typename T,
  int... rs>
struct partial_product<T<>, T<>, T<rs...>> {
  using type = T<rs...>;
};

template <int... Shape>
using shapes_to_sizes = typename partial_product<variadic_ints<Shape...>>::type;

template <typename T, int... Shape>
using subscriptor_helper = typename variadic_uncurrying<shapes_to_sizes<Shape...>, subscriptor<T>>::type;

template <typename T, int... Shape>
class ndarray : public subscriptor_helper<T, Shape...> {
  using super = subscriptor_helper<T, Shape...>;

public:
  ndarray(T * base) : super(base, 0) {
  }
};

template <typename T>
class subscriptor<T> {
protected:
  int _dim;
  int * _sizes;
  T * _mem;

public:
  subscriptor(int dim, int * sizes, T * mem) : _dim(dim), _sizes(sizes), _mem(mem) {
  }

  subscriptor operator[] (int i) {
    return subscriptor(_dim - 1, _sizes + 1, _mem + i * _sizes[1]);
  }

  operator T & () {
    return *_mem;
  }

  subscriptor & operator= (int x) {
    (T &) *this = x;
  }

  template <typename ...Args>
  T & operator() (Args... args) {
    return subscript(_sizes, _mem, args...);
  }

private:
  static T & subscript(int * sizes, T * mem) {
    return *mem;
  }

  template <typename ...Args>
  static T & subscript(int * sizes, T * mem, int i, Args... args) {
    return subscript(sizes + 1, mem + i * sizes[1], args...);
  }
};

template <typename T>
class ndarray<T> : public subscriptor<T> {
  int * _shape;

public:
  template <typename... Args>
  ndarray(Args... args) : 
      subscriptor<T>(sizeof...(args), partial_prod(args...), new T[prod(args...)]),
      _shape(new int[sizeof...(args)] {args...}) {
    std::cout << "ndarray tot_size " << prod(args...) << ", base " << this->_mem << '\n';
  }

  int dim() const {
    return this->_dim;
  }

  slice<int> const shape() const {
    return slice<int>(_shape, this->_dim);
  }

  slice<int> const sizes() const {
    return slice<int>(this->_sizes, this->_dim + 1);
  }
};

template <typename T, T value>
class constexpr_test {
  friend std::ostream & operator << (std::ostream & os, constexpr_test const & s) {
    os << value;
    return os;
  }
};

#include <typeinfo>

using namespace std;

int main() {
  // ndarray<int> A(2, 3, 4);

  // cout << "dim = " << A.dim() << '\n';
  // cout << "shape = " << A.shape() << '\n';
  // cout << "sizes = " << A.sizes() << '\n';

  // cout << '\n';
  
  // cout << "&A(0, 0, 0): " << &A(0, 0, 0) <<'\n';
  // cout << "&A(0, 0, 1): " << &A(0, 0, 1) <<'\n';
  // cout << "&A(0, 1, 0): " << &A(0, 1, 0) <<'\n';
  // cout << "&A(1, 0, 0): " << &A(1, 0, 0) <<'\n';

  // cout << '\n';

  // A(0, 0, 0) = 123;

  // cout << "A[0][0][0]: " << A[0][0][0] <<'\n';
  // cout << "A[0][0][1]: " << A[0][0][1] <<'\n';
  // cout << "A[0][1][0]: " << A[0][1][0] <<'\n';
  // cout << "A[1][0][0]: " << A[1][0][0] <<'\n';

  // cout << '\n';

  // A[0][1][0] = A[0][0][0] * 3;

  // cout << "A(0, 1, 0): " << A(0, 1, 0) <<'\n';

  int mem[24];
  // subscriptor<int, 24, 12, 4, 1> B(mem, 0);

  cout << "base = " << mem << '\n';
  cout << '\n';

  // cout << "&B[0][0][0]: " << &B[0][0][0] <<'\n';
  // cout << "&B[0][0][1]: " << &B[0][0][1] <<'\n';
  // cout << "&B[0][1][0]: " << &B[0][1][0] <<'\n';
  // cout << "&B[1][0][0]: " << &B[1][0][0] <<'\n';

  // cout << constexpr_test<int, subscriptor<int, 24, 12, 4, 1>(mem, 0)[0][0].offset(1)>() << '\n';
  // cout << constexpr_test<int, subscriptor<int, 24, 12, 4, 1>(mem, 0)[0][1].offset(0)>() << '\n';
  // cout << constexpr_test<int, subscriptor<int, 24, 12, 4, 1>(mem, 0)[1][0].offset(0)>() << '\n';

  ndarray<int, 2, 3, 4> C(mem);

  cout << "&C[0][0][0]: " << &C[0][0][0] <<'\n';
  cout << "&C[0][0][1]: " << &C[0][0][1] <<'\n';
  cout << "&C[0][1][0]: " << &C[0][1][0] <<'\n';
  cout << "&C[1][0][0]: " << &C[1][0][0] <<'\n';
}
