#include <iostream>
#include "ndarray.hpp"

using namespace std;

int main() {
  toy::ndarray<int> A(2, 3, 4);

  cout << "dim = " << A.dim() << '\n';
  cout << "shape = " << A.shape() << '\n';
  cout << "strides = " << A.strides() << '\n';

  cout << '\n';
  
  cout << "&A(0, 0, 0): " << &A(0, 0, 0) <<'\n';
  cout << "&A(0, 0, 1): " << &A(0, 0, 1) <<'\n';
  cout << "&A(0, 1, 0): " << &A(0, 1, 0) <<'\n';
  cout << "&A(1, 0, 0): " << &A(1, 0, 0) <<'\n';

  cout << '\n';

  // A(0, 0, 0) = 123;

  // cout << "A[0][0][0]: " << A[0][0][0] <<'\n';
  // cout << "A[0][0][1]: " << A[0][0][1] <<'\n';
  // cout << "A[0][1][0]: " << A[0][1][0] <<'\n';
  // cout << "A[1][0][0]: " << A[1][0][0] <<'\n';

  // cout << '\n';

  // A[0][1][0] = A[0][0][0] * 3;

  // cout << "A(0, 1, 0): " << A(0, 1, 0) <<'\n';

  cout << '\n';
  cout << '\n';

  int mem[24];

  cout << "base = " << mem << '\n';
  cout << '\n';

  toy::ndarray<int, 2, 3, 4> C(mem);

  cout << "dim = " << C.dim() << '\n';
  cout << "shape = " << C.shape() << '\n';
  cout << "strides = " << C.strides() << '\n';

  cout << '\n';

  cout << "&C[0][0][0]: " << &C[0][0][0] <<'\n';
  cout << "&C[0][0][1]: " << &C[0][0][1] <<'\n';
  cout << "&C[0][1][0]: " << &C[0][1][0] <<'\n';
  cout << "&C[1][0][0]: " << &C[1][0][0] <<'\n';
}
