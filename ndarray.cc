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
  T * _mem;

public:
  constexpr subscriptor(T * mem) : _mem(mem) {
  }

  constexpr auto operator[] (int i) {
    return subscriptor<T, ElSize, Sizes...>(_mem + i * ElSize);
  }
};

template <typename T, int Size>
class subscriptor<T, Size, 1> {
  T * _mem;

public:
  constexpr subscriptor(T * mem) : _mem(mem) {
  }

  constexpr T & operator[] (int i) {
    return _mem[i];
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
class ndarray : public subscriptor<T> {
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
  subscriptor<int, 24, 12, 4, 1> B(mem);

  cout << "base = " << mem << '\n';
  cout << '\n';

  cout << "&B[0][0][0]: " << &B[0][0][0] <<'\n';
  cout << "&B[0][0][1]: " << &B[0][0][1] <<'\n';
  cout << "&B[0][1][0]: " << &B[0][1][0] <<'\n';
  cout << "&B[1][0][0]: " << &B[1][0][0] <<'\n';
}
