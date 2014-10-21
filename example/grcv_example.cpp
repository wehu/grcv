#include "grcv.h"
#include <iostream>

using namespace grcv;

class randobj : public randgen
{
public:
   randv<int> a;
   randv<int> b; 
   randv<int> c;
   randobj() : a(0, this), b(0, this), c(0, this)
   {
      constraint(
         a() > 0
      );
      constraint(
         a() + b() == c()
      );
      constraint(
         b() < 0 && b()[_2, __, _5] == conbits<4, GRCV_UNSIGNED>(0)
         && a()[_2, __, _5] == b()[_3, __, _6]
      );
   }
};

int main ()
{
   randobj o;
   for (int i=0; i < 10; i++)
   {
      o.next();
      std::cout << o.a.value() << std::endl;
      std::cout << o.b.value() << std::endl;
      std::cout << o.c.value() << std::endl;
      std::cout << o.a.bits() << std::endl;
      std::cout << o.b.bits() << std::endl;
      std::cout << o.c.bits() << std::endl;
   }
}

