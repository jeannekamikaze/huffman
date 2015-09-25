#define BOOST_TEST_MODULE ptrace test
#include <boost/test/unit_test.hpp>

#include <kxhuffman/Bitseq.h>

using namespace kxh;

void equal (const Bitseq& bitseq, const std::vector<int>& values)
{
    BOOST_REQUIRE_EQUAL(bitseq.size(), values.size());
    for (std::size_t i = 0; i < values.size(); ++i)
        BOOST_REQUIRE_EQUAL((int) bitseq[i], values[i++]);
}

void equal (const Bitseq& a, const Bitseq& b)
{
    BOOST_REQUIRE_EQUAL(a.size(), b.size());

    for (std::size_t i = 0; i < a.size(); ++i)
        BOOST_REQUIRE_EQUAL(a[i], b[i]);

    std::size_t i = 0;
    for (bool x : a)
    {
        BOOST_REQUIRE_EQUAL(b[i], x);
        i++;
    }

    i = 0;
    for (bool x : b)
    {
        BOOST_REQUIRE_EQUAL(a[i], x);
        i++;
    }
}

void contains (const Bitseq& a, const Bitseq& b, std::size_t offset)
{
    BOOST_REQUIRE(a.size() >= b.size());

    for (std::size_t i = 0; i < b.size(); ++i)
        BOOST_REQUIRE_EQUAL(a[offset+i], b[i]);

    std::size_t i = 0;
    for (bool x : b)
    {
        BOOST_REQUIRE_EQUAL(a[offset+i], x);
        i++;
    }
}

Bitseq create (std::size_t N, int start_val = 1)
{
    int val = start_val;
    Bitseq a;
    for (int i = 0; i < N; ++i)
    {
        a.push_back(val != 0);
        val = 1 - val;
    }
    return a;
}

Bitseq from_block (Block b, std::size_t N)
{
    Bitseq seq;
    for (std::size_t i = 0; i < N; ++i)
        seq.push_back((b & (leftmost >> i)) != 0);
    return seq;
}

BOOST_AUTO_TEST_CASE(bitseq_empty)
{
    Bitseq bitseq;
    BOOST_REQUIRE_EQUAL(bitseq.size(), 0);
    bitseq = Bitseq();
    BOOST_REQUIRE_EQUAL(bitseq.size(), 0);
}

BOOST_AUTO_TEST_CASE(bitseq_empty_push_bits_fits)
{
    Bitseq bitseq;
    std::vector<int> values = {0, 0, 1, 0, 1, 1, 1, 0};
    for (int x : values)
        bitseq.push_back(x != 0);
    equal(bitseq, values);
}

BOOST_AUTO_TEST_CASE(bitseq_empty_push_bits_not_fits)
{
    int val = 1;
    std::vector<int> values;
    for (int i = 0; i < 131; ++i)
    {
        values.push_back(val);
        val = 1 - val;
    }

    Bitseq bitseq;
    for (int x : values)
        bitseq.push_back(x != 0);

    equal(bitseq, values);
}

BOOST_AUTO_TEST_CASE(bitseq_empty_push_bitseq_fits)
{
    int N = 47;
    Bitseq a = create(N);

    Bitseq b;
    b.push_back(a);

    equal(a, b);
    BOOST_REQUIRE_EQUAL(a.size(), N);
}

BOOST_AUTO_TEST_CASE(bitseq_empty_push_bitseq_no_fit)
{
    int N = 2347;
    Bitseq a = create(N);

    Bitseq b;
    b.push_back(a);

    equal(a, b);
    BOOST_REQUIRE_EQUAL(a.size(), N);
}

BOOST_AUTO_TEST_CASE(bitseq_non_empty_push_bitseq_no_fit)
{
    int N = 2347;
    Bitseq a = create(N);

    int M = 1717;
    Bitseq b = create(M, !a[a.size()-1]);

    Bitseq c = a;
    c.push_back(b);

    Bitseq d = create(N+M);

    contains(c, a, 0);
    contains(c, b, N);
    BOOST_REQUIRE_EQUAL(c.size(), M+N);
    equal(c, d);
}

BOOST_AUTO_TEST_CASE(bitseq_full_push_bitseq_no_fit)
{
    int N = 17*bpp;
    Bitseq a = create(N);

    int M = 2347;
    Bitseq b = create(M, !a[a.size()-1]);

    Bitseq c = a;
    c.push_back(b);

    Bitseq d = create(N+M);

    contains(c, a, 0);
    contains(c, b, N);
    BOOST_REQUIRE_EQUAL(c.size(), M+N);
    equal(c, d);
}

BOOST_AUTO_TEST_CASE(bitseq_push_bitseq_partial_fit)
{
    int N = 3*bpp + 5;
    Bitseq a = create(N);

    int M = bpp + 17;
    Bitseq b = create(M, !a[a.size()-1]);

    Bitseq c = a;
    c.push_back(b);

    Bitseq d = create(N+M);

    contains(c, a, 0);
    contains(c, b, N);
    BOOST_REQUIRE_EQUAL(c.size(), M+N);
    equal(c, d);
}

BOOST_AUTO_TEST_CASE(bitseq_full_push_bitseq_partial_fit)
{
    int N = 3*bpp;
    Bitseq a = create(N);

    int M = bpp + 17;
    Bitseq b = create(M, !a[a.size()-1]);

    Bitseq c = a;
    c.push_back(b);

    Bitseq d = create(N+M);

    contains(c, a, 0);
    contains(c, b, N);
    BOOST_REQUIRE_EQUAL(c.size(), M+N);
    equal(c, d);
}

BOOST_AUTO_TEST_CASE(bitseq_full_push_bitseq_full)
{
    int N = 3*bpp;
    Bitseq a = create(N);

    int M = 5*bpp;
    Bitseq b = create(M, !a[a.size()-1]);

    Bitseq c = a;
    c.push_back(b);

    Bitseq d = create(N+M);

    contains(c, a, 0);
    contains(c, b, N);
    BOOST_REQUIRE_EQUAL(c.size(), M+N);
    equal(c, d);
}

BOOST_AUTO_TEST_CASE(bitseq_full_push_single_block)
{
    Bitseq a = from_block(0x683A06CFBB983505, 64);
    Bitseq b = from_block(0xD400000000000000, 6);

    Bitseq c = a;
    c.push_back(b);

    contains(c, a, 0);
    contains(c, b, 64);
}

BOOST_AUTO_TEST_CASE(bitseq_push_pop)
{
    int N = 2355;
    Bitseq a = create(N);
    Bitseq b = a;

    b.push_back(1);
    BOOST_REQUIRE_EQUAL(b.size(), N+1);
    b.pop_back();
    equal(a, b);

    b.push_back(0);
    BOOST_REQUIRE_EQUAL(b.size(), N+1);
    b.pop_back();
    equal(a, b);
}

BOOST_AUTO_TEST_CASE(bitseq_full_push_pop)
{
    {
        Bitseq a = create(bpp);
        Bitseq b = a;
        BOOST_REQUIRE_EQUAL(b.size(), bpp);
        b.push_back(0);
        BOOST_REQUIRE_EQUAL(b.size(), bpp+1);
        b.pop_back();
        equal(a, b);
    }
    {
        Bitseq a = create(3*bpp);
        Bitseq b = a;
        BOOST_REQUIRE_EQUAL(b.size(), 3*bpp);
        b.push_back(1);
        BOOST_REQUIRE_EQUAL(b.size(), 3*bpp+1);
        b.pop_back();
        equal(a, b);
    }
}
