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

#include <boost/proto/proto.hpp>
#include <boost/static_assert.hpp>
#include <vector>
#include <bitset>
#include <bdd.h>

#include "grcv_trait.h"

namespace grcv
{

    // constant class
    template <typename T>
    class constval : public grcv_base
    {
        BOOST_STATIC_ASSERT_MSG(bitsize_trait<T>::value > 0, "the bit size of type must > 0");
    public:
        constval(T v, bool s=signed_trait<T>::value) :
            _value(v),
            _size(bitsize_trait<T>::value),
            _bits(v),
            _is_signed(s)
        {
            for (int i = 0; i < _bits.size(); i ++)
            {
                if (_bits[i]) _bcons.push_back(bddtrue);
                else _bcons.push_back(bddfalse);
            }
        }
        virtual ~constval() {}
        int size() const { return _size; }
        bool is_signed() const { return _is_signed; }
        T value() const { return _value; } 
        const std::vector<bdd> & bcons() const { return _bcons; }
        const std::bitset<bitsize_trait<T>::value> & bits() const { return _bits; }
        unsigned long to_ulong() const { return _bits.to_ulong(); }
    private:
        T                _value;
        int              _size;
        bool             _is_signed;
        std::bitset<bitsize_trait<T>::value> _bits;
        std::vector<bdd> _bcons;
    };

    template <int i, bool s=false>
    class constbits : public constval<std::bitset<i> >
    {
    public:
        constbits(unsigned long long v) : constval<std::bitset<i> >(v, s) {}
    };

    // helper function for constant
    template <typename T>
    typename boost::proto::literal<constval<T> > con(T v)
    {
        return boost::proto::literal<constval<T> >(constval<T>(v));
    }

    template <int i, bool s>
    typename boost::proto::literal<constbits<i, s> > conbits(unsigned long long v)
    {
        return boost::proto::literal<constbits<i, s> >(constbits<i, s>(v));
    }

}


