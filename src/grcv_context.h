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
#include <map>
#include <bitset>
#include <bdd.h>

#include "grcv_randv.h"
#include "grcv_const.h"
#include "grcv_logger.h"

#define GRCV_LIMIT_MAX_BITS 15
#define GRCV_LIMIT_MAX_BITS_STR "15"

namespace grcv
{

    // context class for rand variable collection and bdd allocation

    class randgen;
    class randv_base;

    class context_ :
        public boost::proto::callable_context<context_, boost::proto::null_context>
    {
        friend class randgen;
        friend class randv_base;

    public:
        typedef void result_type;
        result_type operator () (boost::proto::tag::terminal op, randv_base * rv)
        {
            add_randv(rv);
        }
    private:
        void add_randv(randv_base * rv);
        std::vector<randv_base *> randvs();
        void alloc_randvs();
        context_() : _frozen(false) {}
    private:
        bool                         _frozen;
        std::map<randv_base *, bool> _randvs;
    };

    // context class for bdd tree
    class context :
        public boost::proto::callable_context<context const>
    {
    public:
        context(randgen * rg) : _rg(rg) {}
        typedef struct
        {
            bool s;
            std::vector<bdd> v;
        } result_type;
        result_type operator () (boost::proto::tag::terminal op, randv_base * rv) const
        {
            result_type res = {rv->is_signed(), rv->bvars()};
            return res;
        }
        template <typename T>
        result_type operator () (boost::proto::tag::terminal op, const constval<T> & c) const
        {
            result_type res = {c.is_signed(), c.bcons()};
            return res;
        }
        template <int i, bool s>
        result_type operator () (boost::proto::tag::terminal op, const constbits<i, s> & c) const
        {
            result_type res = {c.is_signed(), c.bcons()};
            return res;
        }
        template <typename T>
        result_type operator () (boost::proto::tag::terminal op, const T & c) const
        {
            constval<T> c_(c);
            result_type res = {c_.is_signed(), c_.bcons()};
            return res;
        }
        template <typename L, typename R>
        result_type operator () (boost::proto::tag::plus op, const L & l, const R & r) const
        {
            const result_type & l_ = boost::proto::eval(l, *this);
            const result_type & r_ = boost::proto::eval(r, *this);
            logger::assert_(l_.v.size() == r_.v.size(), "the bits of the operands for operator + dismatch");
            logger::assert_(l_.s == r_.s, "the signed of the operands for operator + dismatch");
            int n = l_.v.size();
            result_type res;
            res.s = l_.s;
            bdd c = bddfalse;
            bdd s = bddfalse;
            if (res.s)
            {
                s = l_.v[n-1] ^ r_.v[n-1];
            }
            for (int i = 0; i < n; i++)
            {
                res.v.push_back(l_.v[i] ^ r_.v[i] ^ c);
                c = (l_.v[i] & r_.v[i]) | (l_.v[i] & c) | (r_.v[i] & c);
            }
            s = s ^ c;
            if (res.s) plus_constraint(!(s ^ res.v[n-1]));
            else plus_constraint(!s);
            return res;
        }
        template <typename L, typename R>
        result_type operator () (boost::proto::tag::minus op, const L & l, const R & r) const
        {
            const result_type & l_ = boost::proto::eval(l, *this);
            const result_type & r_ = boost::proto::eval(r, *this);
            logger::assert_(l_.v.size() == r_.v.size(), "the bits of the operands for operator - dismatch");
            logger::assert_(l_.s == r_.s, "the signed of the operands for operator - dismatch");
            int n = l_.v.size();
            result_type res;
            res.s = l_.s;
            bdd c = bddtrue;
            bdd s = bddtrue;
            if (res.s)
            {
                s = l_.v[n-1] ^ (!r_.v[n-1]);
            }
            for (int i = 0; i < n; i++)
            {
                res.v.push_back(l_.v[i] ^ (!r_.v[i]) ^ c);
                c = (l_.v[i] & (!r_.v[i])) | (l_.v[i] & c) | ((!r_.v[i]) & c);
            }
            s = s ^ c;
            if (res.s) plus_constraint(!(s ^ res.v[n-1]));
            else plus_constraint(!s);
            return res;
        }
        template <typename R>
        result_type operator () (boost::proto::tag::unary_plus op, const R & r) const
        {
            return boost::proto::eval(r, *this);
        }
        template <typename R>
        result_type operator () (boost::proto::tag::negate op, const R & r) const
        {
            const result_type & r_ = boost::proto::eval(r, *this);
            logger::assert_(r_.s, "the operand for operator - must signed");
            int n = r_.v.size();
            result_type res;
            res.s = r_.s;
            bdd c;
            c = bddtrue;
            for (int i = 0; i < n; i++)
            {
                res.v.push_back((!r_.v[i]) ^ c);
                c = (!r_.v[i]) & c;
            };
            return res;
        }
        template <typename L, typename R>
        result_type operator () (boost::proto::tag::multiplies op, const L & l, const R & r) const
        {
            const result_type & l_ = boost::proto::eval(l, *this);
            const result_type & r_ = boost::proto::eval(r, *this);
            logger::assert_(l_.v.size() == r_.v.size(), "the bits of the operands for operator * dismatch");
            logger::assert_(!l_.s && l_.s == r_.s, "the operands for operator * must not signed");
            int n = l_.v.size();
            logger::assert_(n <= GRCV_LIMIT_MAX_BITS, "the bits of the operands for operator * is too big: > " GRCV_LIMIT_MAX_BITS_STR);
            result_type res;
            res.s = l_.s;
            bdd a[n];
            bdd b[n];
            bdd c;
            bdd s;
            for (int i = 0; i < n; i++)
            {
                a[i] = bddfalse;
            }
            for (int i = 0; i < n; i++)
            {
                for (int j = 0; j < n; j++)
                {
                    if (j < i)
                    {
                        b[j] = bddfalse;
                        plus_constraint(!(l_.v[n-1-j] & r_.v[i]));
                    }
                    else b[j] = l_.v[j-i] & r_.v[i];
                }
                c = bddfalse;
                s = bddfalse;
                for (int j = 0; j < n; j++)
                {
                    bdd tmp = (a[j] ^ b[j] ^ c);
                    c = (a[j] & b[j]) | (a[j] & c) | (b[j] & c);
                    a[j] = tmp;
                }
                s = s ^ c;
                plus_constraint(!s);
            }
            for (int i = 0; i < n; i++)
            {
                res.v.push_back(a[i]);
            }
            return res;
        }
        template <typename L, typename R>
        result_type operator () (boost::proto::tag::divides op, const L & l, const R & r) const
        {
            const result_type & l_ = boost::proto::eval(l, *this);
            const result_type & r_ = boost::proto::eval(r, *this);
            logger::assert_(l_.v.size() == r_.v.size(), "the bits of the operands for operator / dismatch");
            logger::assert_(!l_.s && l_.s == r_.s, "the operands for operator / must not signed");
            int n = l_.v.size();
            logger::assert_(n <= GRCV_LIMIT_MAX_BITS, "the bits of the operands for operator / is too big: > " GRCV_LIMIT_MAX_BITS_STR);
            bdd r__ = bddfalse;
            for(int i = 0; i < n; i++)
            {
                r__ |= r_.v[i];
            }
            plus_constraint(r__);
            result_type res;
            res.s = l_.s;
            bdd a[n];
            bdd b[n];
            bdd c;
            bdd d[n];
            for (int i = 0; i < n; i++)
            {
                a[i] = bddfalse;
            }
            for (int i = 0; i < n; i++)
            {
                for (int j = n - 1; j > 0; j--)
                {
                    a[j] = a[j - 1];
                }
                a[0] = l_.v[n-i-1];
                c = bddtrue;
                for (int j = 0; j < n; j++)
                {
                    b[j] = (a[j] ^ (!r_.v[j]) ^ c);
                    c = (a[j] & (!r_.v[j])) | (a[j] & c) | ((!r_.v[j]) & c);
                }
                for (int j = 0; j < n; j++)
                {
                    a[j] = (c & b[j]) | ((!c) & a[j]);
                }
                d[n-i-1] = c;
            }
            for (int i = 0; i < n; i++)
            {
                res.v.push_back(d[i]);
            }
            return res;
        }
        template <typename L, typename R>
        result_type operator () (boost::proto::tag::modulus op, const L & l, const R & r) const
        {
            const result_type & l_ = boost::proto::eval(l, *this);
            const result_type & r_ = boost::proto::eval(r, *this);
            logger::assert_(l_.v.size() == r_.v.size(), "the bits of the operands for operator \% dismatch");
            logger::assert_(!l_.s && l_.s == r_.s, "the operands for operator \% must not signed");
            int n = l_.v.size();
            logger::assert_(n <= GRCV_LIMIT_MAX_BITS, "the bits of the operands for operator \% is too big: > " GRCV_LIMIT_MAX_BITS_STR);
            bdd r__ = bddfalse;
            for(int i = 0; i < n; i++)
            {
                r__ |= r_.v[i];
            }
            plus_constraint(r__);
            result_type res;
            res.s = l_.s;
            bdd a[n];
            bdd b[n];
            bdd c;
            for (int i = 0; i < n; i++)
            {
                a[i] = bddfalse;
            }
            for (int i = 0; i < n; i++)
            {
                for (int j = n - 1; j > 0; j--)
                {
                    a[j] = a[j - 1];
                }
                a[0] = l_.v[n-i-1];
                c = bddtrue;
                for (int j = 0; j < n; j++)
                {
                    b[j] = (a[j] ^ (!r_.v[j]) ^ c);
                    c = (a[j] & (!r_.v[j])) | (a[j] & c) | ((!r_.v[j]) & c);
                }
                for (int j = 0; j < n; j++)
                {
                    a[j] = (c & b[j]) | ((!c) & a[j]);
                }
            }
            for (int i = 0; i < n; i++)
            {
                res.v.push_back(a[i]);
            }
            return res;
        }
        template <typename L, typename R>
        result_type operator () (boost::proto::tag::less op, const L & l, const R & r) const
        {
            const result_type & l_ = boost::proto::eval(l, *this);
            const result_type & r_ = boost::proto::eval(r, *this);
            logger::assert_(l_.v.size() == r_.v.size(), "the bits of the operands for operator < dismatch");
            logger::assert_(l_.s == r_.s, "the signed of the operands for operator < dismatch");
            int n = l_.v.size();
            result_type res;
            res.s = false;
            bdd c;
            c = bddtrue;
            bdd s = bddtrue;
            if (l_.s)
            {
                s = l_.v[n-1] ^ (!r_.v[n-1]);
            }
            for (int i = 0; i < n; i++)
            {
                c = (l_.v[i] & (!r_.v[i])) | (l_.v[i] & c) | ((!r_.v[i]) & c);
            }
            s = s ^ c;
            res.v.push_back(s);
            return res;
        }
        template <typename L, typename R>
        result_type operator () (boost::proto::tag::greater op, const L & l, const R & r) const
        {
            const result_type & l_ = boost::proto::eval(l, *this);
            const result_type & r_ = boost::proto::eval(r, *this);
            logger::assert_(l_.v.size() == r_.v.size(), "the bits of the operands for operator > dismatch");
            logger::assert_(l_.s == r_.s, "the signed of the operands for operator > dismatch");
            return boost::proto::eval((!(l < r)) && (l != r), *this);
        }
        template <typename L, typename R>
        result_type operator () (boost::proto::tag::greater_equal op, const L & l, const R & r) const
        {
            const result_type & l_ = boost::proto::eval(l, *this);
            const result_type & r_ = boost::proto::eval(r, *this);
            logger::assert_(l_.v.size() == r_.v.size(), "the bits of the operands for operator >= dismatch");
            logger::assert_(l_.s == r_.s, "the signed of the operands for operator >= dismatch");
            return boost::proto::eval(!(l < r), *this);
        }
        template <typename L, typename R>
        result_type operator () (boost::proto::tag::less_equal op, const L & l, const R & r) const
        {
            const result_type & l_ = boost::proto::eval(l, *this);
            const result_type & r_ = boost::proto::eval(r, *this);
            logger::assert_(l_.v.size() == r_.v.size(), "the bits of the operands for operator <= dismatch");
            logger::assert_(l_.s == r_.s, "the signed of the operands for operator <= dismatch");
            return boost::proto::eval(!(l > r), *this);
        }
        template <typename L, typename R>
        result_type operator () (boost::proto::tag::equal_to op, const L & l, const R & r) const
        {
            const result_type & l_ = boost::proto::eval(l, *this);
            const result_type & r_ = boost::proto::eval(r, *this);
            result_type res;
            res.s = false;
            bdd t = bddfalse;
            if (l_.s == r_.s) 
            {
                for (int i = 0; i < l_.v.size(); i++)
                {
                    t |= l_.v[i] ^ r_.v[i];
                }
                res.v.push_back(!t);
            } else res.v.push_back(bddfalse);
            return res;
        }
        template <typename L, typename R>
        result_type operator () (boost::proto::tag::not_equal_to op, const L & l, const R & r) const
        {
            const result_type & l_ = boost::proto::eval(l, *this);
            const result_type & r_ = boost::proto::eval(r, *this);
            logger::assert_(l_.v.size() == r_.v.size(), "the bits of the operands for operator != dismatch");
            return boost::proto::eval(!(l==r), *this);
        }
        template <typename L, typename R>
        result_type operator () (boost::proto::tag::logical_and op, const L & l, const R & r) const
        {
            const result_type & l_ = boost::proto::eval(l, *this);
            const result_type & r_ = boost::proto::eval(r, *this);
            result_type res;
            res.s = false;
            bdd tl = bddfalse;
            for (int i = 0; i < l_.v.size(); i++)
            {
                tl |= l_.v[i];
            }
            bdd tr = bddfalse;
            for (int i = 0; i < r_.v.size(); i++)
            {
                tr |= r_.v[i];
            }
            res.v.push_back(tl & tr);
            return res;
        }
        template <typename L, typename R>
        result_type operator () (boost::proto::tag::logical_or op, const L & l, const R & r) const
        {
            return boost::proto::eval(!(!l && !r), *this);
        }
        template <typename R>
        result_type operator () (boost::proto::tag::logical_not op, const R & r) const
        {
            const result_type & r_ = boost::proto::eval(r, *this);
            result_type res;
            res.s = false;
            bdd tr = bddfalse;
            for (int i = 0; i < r_.v.size(); i++)
            {
                tr |= r_.v[i];
            }
            res.v.push_back(!tr);
            return res;
        }
        template <typename L, typename R>
        result_type operator () (boost::proto::tag::bitwise_and op, const L & l, const R & r) const
        {
            const result_type & l_ = boost::proto::eval(l, *this);
            const result_type & r_ = boost::proto::eval(r, *this);
            logger::assert_(l_.v.size() == r_.v.size(), "the bits of the operands for operator & dismatch");
            result_type res;
            res.s = l_.s;
            for (int i = 0; i < l_.v.size(); i++)
            {
                res.v.push_back(l_.v[i] & r_.v[i]);
            }
            return res;
        }
        template <typename L, typename R>
        result_type operator () (boost::proto::tag::bitwise_or op, const L & l, const R & r) const
        {
            const result_type & l_ = boost::proto::eval(l, *this);
            const result_type & r_ = boost::proto::eval(r, *this);
            logger::assert_(l_.v.size() == r_.v.size(), "the bits of the operands for operator | dismatch");
            return boost::proto::eval(~((~l) & (~r)), *this);
        }
        template <typename L, typename R>
        result_type operator () (boost::proto::tag::bitwise_xor op, const L & l, const R & r) const
        {
            const result_type & l_ = boost::proto::eval(l, *this);
            const result_type & r_ = boost::proto::eval(r, *this);
            logger::assert_(l_.v.size() == r_.v.size(), "the bits of the operands for operator ^ dismatch");
            return boost::proto::eval((l & (~r)) | ((~l) & r), *this);
        }
        template <typename R>
        result_type operator () (boost::proto::tag::complement op, const R & r) const
        {
            const result_type & r_ = boost::proto::eval(r, *this);
            result_type res;
            res.s = r_.s;
            for (int i = 0; i < r_.v.size(); i++)
            {
                res.v.push_back(!r_.v[i]);
            }
            return res;
        }
        template <typename L, typename R>
        result_type operator () (boost::proto::tag::shift_left op, const L & l, const R & r) const
        {
            const result_type & l_ = boost::proto::eval(l, *this);
            BOOST_STATIC_ASSERT_MSG(boost::is_integral<typename boost::proto::result_of::value<R>::type>::value,
                "the right operand for operator << must be a integeral");
            int r_ = boost::proto::value(r);
            logger::assert_(r_ >= 0, "the bits of the right operand for operator << must >= 0");
            result_type res;
            res.s = l_.s;
            for (int i = 0; i < r_; i++)
            {
                res.v.push_back(bddfalse);
            }
            for (int i = 0; i < l_.v.size() - r_; i++)
            {
                res.v.push_back(l_.v[i]);
            }
            return res;
        }
        template <typename L, typename R>
        result_type operator () (boost::proto::tag::shift_right op, const L & l, const R & r) const
        {
            const result_type & l_ = boost::proto::eval(l, *this);
            BOOST_STATIC_ASSERT_MSG(boost::is_integral<typename boost::proto::result_of::value<R>::type>::value,
                "the right operand for operator >> must be a integeral");
            int r_ = boost::proto::value(r);
            logger::assert_(r_ >= 0, "the right operand for operator >> must >= 0");
            result_type res;
            res.s = l_.s;
            bdd s = bddfalse;
            if (l_.s) s = l_.v[l_.v.size()-1];
            for (int i = r_; i < l_.v.size(); i++)
            {
                res.v.push_back(l_.v[i]);
            }
            for (int i = 0; i < r_; i++)
            {
                res.v.push_back(s);
            }
            return res;
        }
        struct at_or_range
        {
            typedef std::vector<int> result_type;
            at_or_range (const int &a)
            {
                v.push_back(a);
            }
            at_or_range (const at_or_range &a, const at_or_range &b)
            {
                for(int i=0; i < a.v.size(); i++)
                {
                    v.push_back(a.v[i]);
                }
                for(int i=0; i < b.v.size(); i++)
                {
                    v.push_back(b.v[i]);
                }
            }
            result_type v;
        };
        struct at_or_range_grammar :
            boost::proto::or_
            <
                boost::proto::when<
                    boost::proto::terminal<int>
                   ,at_or_range(
                        boost::proto::_value
                    )
                >,
                boost::proto::when<
                    boost::proto::comma<at_or_range_grammar
                                       ,boost::proto::terminal<int> >
                   ,at_or_range(
                        at_or_range_grammar(boost::proto::_left)
                       ,at_or_range_grammar(boost::proto::_right)
                    ) 
                >
            >
        {};
        template <typename L, typename R>
        result_type operator () (boost::proto::tag::subscript op, const L & l, const R & r) const
        {
            BOOST_MPL_ASSERT((boost::proto::matches<R, at_or_range_grammar>));
            const result_type & l_ = boost::proto::eval(l, *this);
            at_or_range_grammar grammar;
            at_or_range r_ = grammar(r);
            result_type res;
            res.s = false;
            logger::assert_(r_.v[0] >= 0, "the lower bound for operator [] must >= 0");
            int v = r_.v[0];
            int pv = r_.v[0];
            for(int i = 0; i < r_.v.size(); i++)
            {
                pv = v;
                v = r_.v[i];
                if (v != -1) logger::assert_(v>=pv, "the higher bound for operator [] must >= the lower bound");
                if (v >= 0)
                {
                    logger::assert_(v<l_.v.size(), "the higher bound for operator [] must < the bit size of the left operand");
                    res.v.push_back(l_.v[v]);
                }
                else if (v == -1)
                {
                    int nv = -1;
                    while (i < r_.v.size() - 1 && r_.v[i+1] == -1) i++;
                    if (i < r_.v.size() - 1) nv = r_.v[i+1];
                    logger::assert_(nv == -1 || nv<l_.v.size(), "the higher bound for operator [] must < the bit size of the left operand");
                    for(v=pv+1; v < l_.v.size() && (nv == -1 || v < nv); v++)
                    {
                        res.v.push_back(l_.v[v]);
                    }
                }
            }
            return res;
        }
    private:
        void plus_constraint(bdd c) const;
    private:
        randgen * _rg;

    };

}


