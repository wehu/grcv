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
#include <boost/type_traits.hpp>
#include <vector>
#include <bitset>
#include <bdd.h>

#include "grcv_base.h"
#include "grcv_trait.h"

namespace grcv
{

    class context_;
    class randgen;

    // base class of rand variable
    class randv_base : public grcv_base
    {
        friend class context_;
    public:
        randv_base(int size, bool is_signed, randgen * parent=NULL);
        virtual ~randv_base() {}
        int size() const { return _size; }
        bool is_signed() const { return _is_signed; }
        bool bvars_alloced() const { return _bvars_alloced; }
        const std::vector<bdd> & bvars() const { return _bvars; }
        const std::vector<int> & bvar_ids() const { return _bvar_ids; }
        virtual void assign(std::vector<bool> const & v) = 0;
    private:
        void alloc_bvars(const std::vector<bdd> & bvars);
    private:
        int              _size;
        bool             _is_signed;
        bool             _bvars_alloced;
        std::vector<bdd> _bvars;
        std::vector<int> _bvar_ids;
    };

    template <typename T>
    class randv_ : public randv_base
    {
        BOOST_STATIC_ASSERT_MSG(bitsize_trait<T>::value > 0, "the bit size of type must > 0");
    public:
        typedef randv_<T> this_type;
        typedef boost::proto::literal<randv_base *> node_type;

        randv_(T v, randgen * p=NULL, bool s=signed_trait<T>::value) :
            randv_base(bitsize_trait<T>::value, s, p),
            _value(v),
            _node(this) {}
        virtual ~randv_() {}
        const node_type & operator () () const { return _node; }
        virtual T value() const { return _value; }
    protected:
        T         _value;
        node_type _node;
    };

    template <typename T>
    class randv : public randv_<T> 
    {
        BOOST_STATIC_ASSERT_MSG(boost::is_float<T>::value     == 0, "float is not supported for randv");
        BOOST_STATIC_ASSERT_MSG(boost::is_pointer<T>::value   == 0, "pointer is not supported for randv");
        BOOST_STATIC_ASSERT_MSG(boost::is_reference<T>::value == 0, "reference is not supported for randv");
    public:
        randv(T v, randgen * p=NULL, bool s=signed_trait<T>::value) : randv_<T>(v, p, s) {}
        virtual ~randv() {}
        virtual void assign(std::vector<bool> const & v)
        {
            for (int i=0; i < v.size(); i++)
            {
                _bits[i] = v[i];
            }
            randv_<T>::_value = _bits.to_ulong();
        }
        const std::bitset<bitsize_trait<T>::value> & bits() const { return _bits; }
        unsigned long to_ulong() const { return _bits.to_ulong(); }
    private:
        std::bitset<bitsize_trait<T>::value> _bits;
    };

    template <int i, bool s=false>
    class randbitsv : public randv<std::bitset<i> >
    {
    public:
        randbitsv(unsigned long long v, randgen * p=NULL) : randv<std::bitset<i> >(v, p, s) {}
    };

    

}


