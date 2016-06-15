#pragma once

#include "common.h"

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

    // iterate over the bits of the bit sequence
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

    Bitseq ()
        : count(0)
    {
        blocks.push_back(0);
    }

    Bitseq& operator= (const Bitseq& that) {
        blocks = that.blocks;
        count = that.count;
        return *this;
    }

    /// Push a bit.
    void push_bit (bool x) {
        if (count == bpp) // ran out of bits for current block
        {
            blocks.push_back(x ? leftmost : 0);
            count = 1;
        }
        else
        {
            blocks.back() |= (x ? (leftmost >> count) : 0);
            count++;
        }
        DEBUG_ASSERT(count > 0 && count <= bpp);
    }

    /// Push a byte.
    void push_byte (std::uint8_t byte, std::size_t num_bits = 8) {
        // byte must be placed in upper byte of block
        Block block = ((Block) byte) << (bpp-8);
        if (count + num_bits <= bpp) // fits in current block
        {
            blocks.back() |= (block >> count);
            count += num_bits;
            DEBUG_ASSERT(count > 0 && count <= bpp);
        }
        else
        {
            DEBUG_ASSERT(count > 0); // otherwise all bits fit
            std::size_t fit = bpp-count;
            if (fit == 0)
            {
                blocks.push_back(block);
                count = num_bits;
            }
            else
            {
                blocks.back() |= (block >> count);
                blocks.push_back(block << fit);
                count = num_bits - fit;
            }
            DEBUG_ASSERT(count > 0 && count <= bpp);
        }
    }

    /// Push a bit sequence.
    void push_seq (const Bitseq& seq) {
        if (seq.count == 0) return; // empty bitseq, nothing to copy
        if (count == 0) // uninitialised bitseq, just copy
        {
            *this = seq;
            return;
        }
        bool last_fits = count + seq.count <= bpp;
        bool fits = (seq.blocks.size() == 1) && last_fits;
        if (fits) // block in seq fits current block
        {
            blocks.back() |= (seq.blocks.back() >> count);
            count += seq.count;
            DEBUG_ASSERT(count > 0 && count <= bpp);
        }
        else // blocks in seq must be split
        {
            std::size_t fit = bpp-count;
            DEBUG_ASSERT(fit < bpp); // since count > 0
            for (std::size_t i = 0; i < seq.blocks.size(); ++i)
            {
                Block block = seq.blocks[i];
                if (fit > 0)
                    blocks.back() |= (block >> count);
                if (i != seq.blocks.size()-1 || !last_fits)
                    blocks.push_back(block << fit);
            }
            if (last_fits)
                count += seq.count;
            else
                count = (seq.count - fit);
            DEBUG_ASSERT(count > 0 && count <= bpp);
        }
    }

    /// Pop the last inserted bit.
    void pop () {
        // if count = 0, then the bitseq is empty, in which case
        // pop_back() is undefined
        DEBUG_ASSERT(count > 0);
        count--;
        if (count == 0) // block no longer has relevant bits
        {
            blocks.pop_back();
            count = bpp;
        }
        else blocks.back() = blocks.back() & ~(leftmost >> count); // set it to 0
        DEBUG_ASSERT(count > 0 && count <= bpp);
    }

    /// Return the ith bit.
    bool operator[] (std::size_t index) const {
        return ( blocks[index/bpp] & (leftmost >> (index % bpp)) ) != 0;
    }

    /// Return the number of bits in the sequence.
    std::size_t size () const {
        return (blocks.size()-1)*bpp + count;
    }

    /// Reserve space for the given number of bits.
    void reserve (std::size_t size) {
        blocks.reserve(size/bpp + 1); // just add 1, it's easier...
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

    // the blocks in the bitseq.
    // there is always at least 1 block, even in an empty bitseq.
    std::vector<Block> blocks;

    // the number of relevant bits in the current block, in [0,bpp].
    // if count = 0, the bitseq is empty.
    // non-empty bitseqs have count in [1,bpp].
    std::size_t count;
};

} // namespace kxh
