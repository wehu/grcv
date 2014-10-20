## A random constraint library based on proto and buddy

This is created for generating random values which meet constraints.
The library is based on boost::proto to create EDSL for constaint expression
and use buddy as solver.

#### Requirements:
1. boost
2. buddy

setenv BOOST_HOME AND BUDDY_HOME for them

#### Example:

	#include "grcv.h"
	#include <iostream>
	
	using namespace grcv;
	
	int main() {
	    randv<int> a(0);
	    randv<int> b(0);
	    randv<int> c(0);
	    randgen gen;
	    gen.constraint(
	            a() + b () == c()
	            && a() < 8 && a() >= 0
	            && b() < 10 && b() >= 0
	        );
	    gen.next();
	    std::cout << a.value() << " + ";
	    std::cout << b.value() << " = ";
	    std::cout << c.value() << std::endl;
	    assert((a.value() + b.value()) == c.value());
	}


