#include "grcv.h"
#include <iostream>

using namespace grcv;

#define TEST1(op_, b_, n_) \
{ \
    randbitsv<b_> a(0); \
    randbitsv<b_> b(0); \
    randbitsv<b_> c(0); \
    randgen gen; \
    gen.constraint( \
            a() op_ b() == c() \
        ); \
    for (int i=0; i<n_; i++) \
    { \
        gen.next(); \
        std::cout << a.to_ulong() << #op_ ; \
        std::cout << b.to_ulong() << "="; \
        std::cout << c.to_ulong() << std::endl; \
        assert((a.to_ulong() op_ b.to_ulong()) == c.to_ulong()); \
    } \
}

#define TEST2(op_, t_, n_) \
{ \
    randv<t_> a(0); \
    randv<t_> b(0); \
    randv<t_> c(0); \
    randgen gen; \
    gen.constraint( \
            (a() op_ b()) == c() \
        ); \
    for (int i=0; i<n_; i++) \
    { \
        gen.next(); \
        std::cout << a.value() << #op_; \
        std::cout << b.value() << "="; \
        std::cout << c.value() << std::endl; \
        assert((a.value() op_ b.value()) == c.value()); \
    } \
}

#define TEST3(op_, t_, n_) \
{ \
    randv<t_> a(0); \
    randv<t_> b(0); \
    randgen gen; \
    gen.constraint( \
            a() op_ b() \
        ); \
    for (int i=0; i<n_; i++) \
    { \
        gen.next(); \
        std::cout << a.value() << #op_; \
        std::cout << b.value() << std::endl; \
        assert(a.value() op_ b.value()); \
    } \
}

#define TEST4(op_, v_, t_, n_) \
{ \
    randv<t_> a(0); \
    randv<t_> b(0); \
    randgen gen; \
    gen.constraint( \
            (a() op_ v_) == b() \
        ); \
    for (int i=0; i<n_; i++) \
    { \
        gen.next(); \
        std::cout << a.value() << #op_ << v_ << "="; \
        std::cout << b.value() << std::endl; \
        assert((t_)(a.value() op_ v_) == b.value()); \
    } \
}

#define TEST5(op_, t_, n_) \
{ \
    randv<t_> a(0); \
    randv<t_> b(0); \
    randgen gen; \
    gen.constraint( \
            (op_ a()) == b() \
        ); \
    for (int i=0; i<n_; i++) \
    { \
        gen.next(); \
        std::cout << #op_ << a.value() << "="; \
        std::cout << b.value() << std::endl; \
        assert((t_)(op_ a.value()) == b.value()); \
    } \
}

#define TEST6(op_, t_, n_) \
{ \
    randv<t_> a(0); \
    randv<t_> b(0); \
    randgen gen; \
    gen.constraint( \
            (op_ (a() == b())) \
        ); \
    for (int i=0; i<n_; i++) \
    { \
        gen.next(); \
        std::cout << #op_ << "(" << a.value() << "==" << b.value() << ")" << std::endl; \
        assert(op_ (a.value() == b.value())); \
    } \
}

#define TEST7(i_, j_, t_, n_) \
{ \
    randv<t_> a(0); \
    randbitsv<j_ - i_ + 1> b(0); \
    randgen gen; \
    gen.constraint( \
            (a()[_##i_, __, _##j_] == b()) \
        ); \
    for (int i=0; i<n_; i++) \
    { \
        gen.next(); \
        std::cout << a.value() << "[" << i_ << "," << j_ << "]==" << b.to_ulong() << std::endl; \
        assert(((a.value() & ((1 << (j_+1)) - 1)) >> i_) == b.to_ulong()); \
    } \
}

#define N 100

int main () {

    TEST1(*, 10, N)
    TEST1(/, 10, N)
    TEST1(%, 11, N)

    TEST2(+, short, N)
    TEST2(+, unsigned short, N)
    TEST2(+, int, N)
    TEST2(+, unsigned int, N)
    TEST2(+, long, N)
    TEST2(+, unsigned long, N)
    TEST2(+, long long, N)
    TEST2(+, unsigned long long, N)

    TEST2(-, short, N)
    TEST2(-, unsigned short, N)
    TEST2(-, int, N)
    TEST2(-, unsigned int, N)
    TEST2(-, long, N)
    TEST2(-, unsigned long, N)
    TEST2(-, long long, N)
    TEST2(-, unsigned long long, N)

    TEST2(|, short, N)
    TEST2(|, unsigned short, N)
    TEST2(|, int, N)
    TEST2(|, unsigned int, N)
    TEST2(|, long, N)
    TEST2(|, unsigned long, N)
    TEST2(|, long long, N)
    TEST2(|, unsigned long long, N)

    TEST2(&, short, N)
    TEST2(&, unsigned short, N)
    TEST2(&, int, N)
    TEST2(&, unsigned int, N)
    TEST2(&, long, N)
    TEST2(&, unsigned long, N)
    TEST2(&, long long, N)
    TEST2(&, unsigned long long, N)

    TEST2(^, short, N)
    TEST2(^, unsigned short, N)
    TEST2(^, int, N)
    TEST2(^, unsigned int, N)
    TEST2(^, long, N)
    TEST2(^, unsigned long, N)
    TEST2(^, long long, N)
    TEST2(^, unsigned long long, N)

    TEST3(&&, bool, N)
    TEST3(&&, short, N)
    TEST3(&&, unsigned short, N)
    TEST3(&&, int, N)
    TEST3(&&, unsigned int, N)
    TEST3(&&, long, N)
    TEST3(&&, unsigned long, N)
    TEST3(&&, long long, N)
    TEST3(&&, unsigned long long, N)

    TEST3(||, bool, N)
    TEST3(||, short, N)
    TEST3(||, unsigned short, N)
    TEST3(||, int, N)
    TEST3(||, unsigned int, N)
    TEST3(||, long, N)
    TEST3(||, unsigned long, N)
    TEST3(||, long long, N)
    TEST3(||, unsigned long long, N)

    TEST3(>, short, N)
    TEST3(>, unsigned short, N)
    TEST3(>, int, N)
    TEST3(>, unsigned int, N)
    TEST3(>, long, N)
    TEST3(>, unsigned long, N)
    TEST3(>, long long, N)
    TEST3(>, unsigned long long, N)

    TEST3(>=, short, N)
    TEST3(>=, unsigned short, N)
    TEST3(>=, int, N)
    TEST3(>=, unsigned int, N)
    TEST3(>=, long, N)
    TEST3(>=, unsigned long, N)
    TEST3(>=, long long, N)
    TEST3(>=, unsigned long long, N)

    TEST3(==, short, N)
    TEST3(==, unsigned short, N)
    TEST3(==, int, N)
    TEST3(==, unsigned int, N)
    TEST3(==, long, N)
    TEST3(==, unsigned long, N)
    TEST3(==, long long, N)
    TEST3(==, unsigned long long, N)

    TEST3(!=, short, N)
    TEST3(!=, unsigned short, N)
    TEST3(!=, int, N)
    TEST3(!=, unsigned int, N)
    TEST3(!=, long, N)
    TEST3(!=, unsigned long, N)
    TEST3(!=, long long, N)
    TEST3(!=, unsigned long long, N)

    TEST3(<=, short, N)
    TEST3(<=, unsigned short, N)
    TEST3(<=, int, N)
    TEST3(<=, unsigned int, N)
    TEST3(<=, long, N)
    TEST3(<=, unsigned long, N)
    TEST3(<=, long long, N)
    TEST3(<=, unsigned long long, N)

    TEST3(<, short, N)
    TEST3(<, unsigned short, N)
    TEST3(<, int, N)
    TEST3(<, unsigned int, N)
    TEST3(<, long, N)
    TEST3(<, unsigned long, N)
    TEST3(<, long long, N)
    TEST3(<, unsigned long long, N)

    for(int ii=0; ii < 4; ii++)
    {
    TEST4(<<, ii, short, N)
    TEST4(<<, ii, unsigned short, N)
    TEST4(<<, ii, int, N)
    TEST4(<<, ii, unsigned int, N)
    TEST4(<<, ii, long, N)
    TEST4(<<, ii, unsigned long, N)
    TEST4(<<, ii, long long, N)
    TEST4(<<, ii, unsigned long long, N)

    TEST4(>>, ii, short, N)
    TEST4(>>, ii, unsigned short, N)
    TEST4(>>, ii, int, N)
    TEST4(>>, ii, unsigned int, N)
    TEST4(>>, ii, long, N)
    TEST4(>>, ii, unsigned long, N)
    TEST4(>>, ii, long long, N)
    TEST4(>>, ii, unsigned long long, N)

    }

    TEST5(~, short, N)
    TEST5(~, unsigned short, N)
    TEST5(~, int, N)
    TEST5(~, unsigned int, N)
    TEST5(~, long, N)
    TEST5(~, unsigned long, N)
    TEST5(~, long long, N)
    TEST5(~, unsigned long long, N)

    TEST6(!, short, N)
    TEST6(!, unsigned short, N)
    TEST6(!, int, N)
    TEST6(!, unsigned int, N)
    TEST6(!, long, N)
    TEST6(!, unsigned long, N)
    TEST6(!, long long, N)
    TEST6(!, unsigned long long, N)

    TEST7(2, 3, short, N)
    TEST7(2, 3, unsigned short, N)
    TEST7(2, 3, int, N)
    TEST7(2, 3, unsigned int, N)
    TEST7(2, 3, long, N)
    TEST7(2, 3, unsigned long, N)
    TEST7(2, 3, long long, N)
    TEST7(2, 3, unsigned long long, N)

    TEST7(0, 8, short, N)
    TEST7(0, 8, unsigned short, N)
    TEST7(0, 8, int, N)
    TEST7(0, 8, unsigned int, N)
    TEST7(0, 8, long, N)
    TEST7(0, 8, unsigned long, N)
    TEST7(0, 8, long long, N)
    TEST7(0, 8, unsigned long long, N)

}
