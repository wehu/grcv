/*
* Copyright 2014 huwei04@hotmail.com. All Rights Reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/

/*

This is created for generating random values which meet constraints.
The library is based on boost::proto to create EDSL for constaint expression
and use buddy as solver.

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

*/

#pragma once

#include "grcv_randv.h"
#include "grcv_const.h"
#include "grcv_randgen.h"
#include "grcv_range.h"

#define GRCV_SIGNED true
#define GRCV_UNSIGNED false


