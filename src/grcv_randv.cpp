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

#include "grcv_randv.h"
#include "grcv_randgen.h"
#include "grcv_context.h"
#include "grcv_logger.h"

namespace grcv
{

    void randv_base::alloc_bvars(const std::vector<bdd> & bvars)
    {
        logger::assert_(!_bvars_alloced, "attempt to allocate bdd vars to a randv in more than once");
        _bvars_alloced = true;
        for (int i = 0; i < _size; i++)
        {
            bdd v = bvars[i];
            _bvars.push_back(v);
            _bvar_ids.push_back(bdd_var(v));
        }
    }

    randv_base::randv_base(int size, bool is_signed, randgen * parent) :
        _size(size), _is_signed(is_signed), _bvars_alloced(false)
    {
        if (parent) parent->__ctxt.add_randv(this);
    }

}


