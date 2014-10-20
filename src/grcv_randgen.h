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

#pragma once

#include <vector>
#include <map>
#include <bdd.h>

#include "grcv_logger.h"
#include "grcv_base.h"
#include "grcv_context.h"

namespace grcv
{

    enum gcrv_rand_mode
    {
        RAND,
        CUSTOM
    };

    class randv_base;
    class context_;
    class context;

    // random generator class
    class randgen : public grcv_base
    {
        friend class randv_base;
        friend class context;

    public:
        void seed(unsigned int s);
        void mode(gcrv_rand_mode m = RAND, unsigned int (*alg)(unsigned int) = NULL);
        randgen();
        virtual ~randgen() {}
        template <typename E> void constraint (E const & expr)
        {
            // collect and allocate rand variables
            boost::proto::eval(expr, __ctxt);
            __ctxt.alloc_randvs();
            // build bdd tree
            const context::result_type & res = boost::proto::eval(expr, _ctxt);
            logger::assert_(res.v.size() == 1, "the expression of constraint must be a bool expression");
            _res &= res.v[0];
            _changed = true;
        }

    private:
        int rand_();
        bool randsat_rec(bdd r);
        bool randsat();
        unsigned int calc_weight_rec(bdd r);
        void calc_weight();

    public:
        void next();

    private:
        bdd                 _res;
        bool                _changed;
        unsigned int        _seed;
        gcrv_rand_mode      _mode;
        unsigned int (*_rand_alg)(unsigned int);
        context             _ctxt;
        context_            __ctxt;
        std::map<int, bool> _sat;
        std::map<int, unsigned int>  _weight;
    };

}


