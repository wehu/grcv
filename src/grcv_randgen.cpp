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

#include "grcv_randgen.h"

namespace grcv
{

    void randgen::seed(unsigned int s)
    {
        _seed = s;
    }
    void randgen::mode(gcrv_rand_mode m, unsigned int (*alg)(unsigned int))
    {
        _mode = m;
        if (_mode == CUSTOM)
        {
            logger::assert_(alg != NULL, "should give a algorithm when randgen's mode == CUSTOM");
            _rand_alg = alg;
        }
    }
    randgen::randgen() : _res(bddtrue), _changed(true), _seed(0),
                _mode(RAND), _rand_alg(NULL), _ctxt(this)
    {
        mode();
    }
    int randgen::rand_()
    {
        if (_mode == RAND)
        {
            return rand_r(&_seed);
        }
        else
        {
            _seed = _rand_alg(_seed);
            return _seed;
        }
    }

    bool randgen::randsat_rec(bdd r)
    {
           if (r == bddtrue) { return true; }
           if (r == bddfalse) { return false; }
           if (bdd_low(r) != bddfalse && bdd_high(r) == bddfalse)
           {
              _sat.insert(std::make_pair(bdd_var(r), false));
              return randsat_rec(bdd_low(r));
           }
           else if (bdd_low(r) == bddfalse && bdd_high(r) != bddfalse)
           {
              _sat.insert(std::make_pair(bdd_var(r), true));
              return randsat_rec(bdd_high(r));
           }
           else if (bdd_low(r) != bddfalse && bdd_high(r) != bddfalse)
           {
            unsigned int rw = 1;
            unsigned int hw = 1;
            unsigned int s = 0;
            if (bdd_low(r) != bddtrue)
                rw += _weight[bdd_var(bdd_low(r))];
            if (bdd_high(r) != bddtrue)
                hw += _weight[bdd_var(bdd_high(r))];
            unsigned int w = rw + hw;
            if (w == 0)
            {
                rw = 1;
                w = 2;
            }
            s = rand_() % w;
            if (s < rw)
            {
                _sat.insert(std::make_pair(bdd_var(r), false));
                return randsat_rec(bdd_low(r));
            }
              else
            {
                _sat.insert(std::make_pair(bdd_var(r), true));
                return randsat_rec(bdd_high(r));
            }
           }
           else return false;
    }

    bool randgen::randsat()
    {
        _sat.clear();
        return randsat_rec(_res);
    }

    unsigned int randgen::calc_weight_rec(bdd r)
    {
        if (r == bddtrue) return 1;
        if (r == bddfalse) return 0;
        unsigned int res = 0;
        if (_weight.find(bdd_var(r)) == _weight.end())
            res = calc_weight_rec(bdd_low(r)) + calc_weight_rec(bdd_high(r));
        else return _weight[bdd_var(r)];
        _weight.insert(std::make_pair(bdd_var(r), res));
        return res;
    }

    void randgen::calc_weight()
    {
        if (_changed)
        {
            _changed = false;
            _weight.clear();
            calc_weight_rec(_res);
        }
    }

    void randgen::next()
    {
        calc_weight();
        logger::assert_(randsat(), "no satisfied result found");
        const std::vector<randv_base *> & randvs = __ctxt.randvs();
        for (std::vector<randv_base *>::const_iterator it = randvs.begin(); it != randvs.end(); it++)
        {
            const std::vector<int> & ids = (*it)->bvar_ids();
            std::vector<bool> r;
            for (std::vector<int>::const_iterator it1 = ids.begin(); it1 != ids.end(); it1++)
            {
                if (_sat.find(*it1) == _sat.end())
                {
                    r.push_back(rand_()%2);
                }
                else r.push_back(_sat[*it1]);
            }
            (*it)->assign(r);
        }
    }

}


