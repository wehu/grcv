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

#include <boost/preprocessor/repetition.hpp>
#include <boost/type_traits.hpp>
#include <bitset>

namespace grcv
{

    // bit size trait for types
    template <typename T>
    struct bitsize_trait
    {
        enum bitsize
        {
            value = sizeof(T) * 8
        };
    };

    // signed traist for types
    template <typename T>
    struct signed_trait
    {
       enum is_signed
       {
           value = boost::is_signed<T>::value
       };
    };

    template <> struct bitsize_trait<bool>
    {
        enum bitsize
        {
            value = 1
        };
    };

    #define GRCV_TRAIT_BITSET(z, n, unused) \
    template <> \
    struct bitsize_trait<std::bitset<n+1> > \
    { \
        enum bitsize \
        { \
            value = n+1 \
        }; \
    }; \
    template <> \
    struct signed_trait <std::bitset<n+1> > \
    { \
       enum is_signed \
       { \
           value = false \
       }; \
    };

    BOOST_PP_REPEAT(256, GRCV_TRAIT_BITSET, ~)

}


