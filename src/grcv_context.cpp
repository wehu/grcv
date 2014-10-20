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

#include "grcv_context.h"
#include "grcv_randgen.h"

namespace grcv
{

    // context class for rand variable collection and bdd allocation

    void context_::add_randv(randv_base * rv)
    {
        if (_randvs.find(rv) == _randvs.end())
        {
            logger::assert_(!_frozen, "attempt to add more random variable after allocate all random variables");
            _randvs.insert(std::make_pair(rv, true));
        }
    }
    std::vector<randv_base *> context_::randvs()
    {
        std::vector<randv_base *> r;
        for(std::map<randv_base *, bool>::iterator it = _randvs.begin(); it != _randvs.end(); it++)
        {
            r.push_back(it->first);
        }
        return r;
    }
    void context_::alloc_randvs()
    {
        int max_size = 0;
        int tot_size = 0;
        std::vector<std::vector<bdd> > res;
        std::vector<randv_base *> vs;
        _frozen = true;
        for(std::map<randv_base *, bool>::iterator it = _randvs.begin(); it != _randvs.end(); it++)
        {
            randv_base * v = it->first;
            if (!v->bvars_alloced()) vs.push_back(v);
        }
        int vs_size = vs.size();
        for(int i = 0; i < vs_size; i++)
        {
            if (max_size < vs[i]->size())
                max_size = vs[i]->size();
            tot_size += vs[i]->size();
            res.push_back(std::vector<bdd>());
        }
        int vn = bdd_varnum();
        bdd_extvarnum(tot_size);
        for(int i = 0; i < max_size; i++)
        {
            for(int j = 0; j < vs_size; j++)
            {
                if(i < vs[j]->size())
                {
                    res[j].push_back(bdd_ithvar(vn));
                    vn ++;
                }
            }
        }
        for(int i = 0; i < vs_size; i++)
        {
            vs[i]->alloc_bvars(res[i]);
        }
    }


    void context::plus_constraint(bdd c) const
    {
        _rg->_res &= c;
    }

}


