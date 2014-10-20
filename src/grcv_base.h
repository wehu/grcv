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

#include <bdd.h>

#define GRCV_INIT_NODES 1000000
#define GRCV_INIT_CACHE_NODES 100000

namespace grcv
{

    // auto initialize bdd environment

    extern void grcv_bdd_gbchandler(int pre, bddGbcStat *s);

    class grcv_base
    {
    private:
        class grcv_auto
        {
        public:
            grcv_auto()
            {
                bdd_init(GRCV_INIT_NODES, GRCV_INIT_CACHE_NODES);
                bdd_disable_reorder();
                bdd_gbc_hook(grcv_bdd_gbchandler);
            }
            ~grcv_auto()
            {
                bdd_done();
            }
        };
    public:
        grcv_base()
        {
            static grcv_auto _auto;
        }
        virtual ~grcv_base(){}
    };

}

