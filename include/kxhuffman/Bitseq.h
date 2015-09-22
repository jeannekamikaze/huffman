#pragma once

#include <vector>
#include <cstdint>
#include <limits>

namespace kxh
{

using Block = std::uint64_t;
const Block leftmost = (Block) std::numeric_limits<std::int64_t>::min();
const int bpp = sizeof(Block)*8; // bits per block

class Bitseq
{
public:

    class const_iterator
    {
    public:

        bool operator!= (const const_iterator& that) const {
            return index != that.index;
        }

        const_iterator& operator++ () {
            index++;
            return *this;
        }

        bool operator* () const {
            return bitseq[index];
        }

    private:

        friend class Bitseq;

        const_iterator (const Bitseq& bitseq, std::size_t index)
            : bitseq(bitseq), index(index) {}

        const Bitseq& bitseq;
        std::size_t index;
    };

public:

    Bitseq& operator= (const Bitseq& that)
    {
        bits  = that.bits;
        count = that.count;
        return *this;
    }

    /// Push a bit.
    void push_back (bool x) {
        if (bits.size() == 0 || count == bpp) // ran out of bits for current block
        {
            bits.push_back(0);
            count = 0;
        }
        bits.back() |= (x ? (leftmost >> count) : 0);
        count++;
    }

    /// Pop the last inserted bit.
    void pop_back () {
        if (bits.empty()) return;
        if (count == 1)
        {
            bits.pop_back();
            count = sizeof(Block); // the block before the current one was full
        }
        else
        {
            count--;
            bits.back() = bits.back() & ~(leftmost >> count); // set it to 0
        }
    }

    /// Return the ith bit.
    bool operator[] (std::size_t index) const {
        return ( bits[index/bpp] & (leftmost >> (index % bpp)) ) != 0;
    }

    /// Return the number of bits in the sequence.
    std::size_t size () const {
        if (bits.empty()) return 0;
        else return (bits.size()-1)*bpp + count;
    }

    /// Reserve space for the given number of bits.
    void reserve (std::size_t size) {
        bits.reserve(size/bpp + 1); // just add 1, it's easier...
    }

    /// Return an iterator to the beginning of the sequence.
    const_iterator begin () const {
        return const_iterator(*this, 0);
    }

    /// Return an iterator to one past the end of the sequence.
    const_iterator end () const {
        return const_iterator(*this, size());
    }

private:

    std::vector<Block> bits;
    std::size_t count = 0; // the number of relevant bits in the current block
};

} // namespace kxh
