/*
 * HEF (Huffman Encoded File) File Format
 *
 * Given:
 *
 * A = {x0, x1, ..., xN} - Alphabet
 * S = {s0, s1, ..., sN} - Alphabet values' bit sequences
 * B = b0b1...bM         - Data bit sequence
 *
 * We produce the following file:
 *
 * ; Huffman tree
 * [N: num]
 * [x0, x1, ..., xN]
 * [L0, L1, ..., LN]
 * [s0s1...sN]
 * ; Encoded data
 * [M_bytes: num] // number of whole bytes, M/8
 * {M_bits: U8]   // number of remaining bits, M%8
 * [b0b1...bM]
 *
 * where:
 *
 * - 'num' is an U8, U16, U32 or U64 depending on the value being encoded:
 *     if N <= 255,   num is an U8
 *     if N <= 65535, num is an U16
 *     etc.
 *   and pre-prended with an U8 indicating its size:
 *     0 = num is an U8
 *     1 = num is an U16
 *     2 = num is an U32
 *     3 = num is an U64
 *   examples:
 *         0 becomes 00 00
 *         1 becomes 00 01
 *       255 becomes 00 FF
 *       256 becomes 01 01 00
 *     65535 becomes 01 FF FF
 *     65536 becomes 02 00 01 00 00
 *
 * - L0, L1, ..., LN are the lengths of the bit sequences s0, s1, ..., sN
 */

#pragma once

#include <cstdint>

using U8  = std::uint8_t;
using U16 = std::uint16_t;
using U32 = std::uint32_t;
using U64 = std::uint64_t;

enum num_type
{
    num_byte  = 0,
    num_word  = 1,
    num_dword = 2,
    num_qword = 3
};

#ifdef ALGORITHM_OUTPUT
#include <cstdio>
#define DEBUG_PRINT printf
#else
#define DEBUG_PRINT(...)
#endif

#ifdef KX_DEBUG
#include <cassert>
#define DEBUG_ASSERT assert
#else
#define DEBUG_ASSERT(x)
#endif
