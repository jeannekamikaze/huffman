#pragma once

#include "HuffmanTree.h"
#include "common.h"

#include <vector>
#include <string>

namespace kxh
{

/// Perform a memory copy and advance the source pointer.
void read (U8* dst, const U8*& src, std::size_t num_bytes)
{
    memcpy(dst, src, num_bytes);
    src += num_bytes;
}

/// Deserialise the number.
/// Advance the pointer to the element past the serialised number.
std::size_t deserialise_num (const U8*& ptr)
{
    num_type nt = (num_type) *ptr++;
    switch (nt)
    {
    case num_byte:
    {
        return *ptr++;
    }
    case num_word:
    {
        U16 val16;
        read((U8*)&val16, ptr, sizeof(U16));
        return val16;
    }
    case num_dword:
    {
        U32 val32;
        read((U8*)&val32, ptr, sizeof(U32));
        return val32;
    }
    case num_qword:
    {
        U64 val64;
        read((U8*)&val64, ptr, sizeof(U64));
        return val64;
    }
    default: break;
    }
    throw std::runtime_error("invalid number type");
}

/// Deserialise the byte.
/// Transforms 01001... to the values 0, 1, 0, 0, 1, ...
/// c: the input byte.
/// n: the number of relevant bits in 'c'.
/// cont: container where result should be placed.
template <class cont_t>
void deserialise_byte (U8 c, std::size_t n, cont_t& seq)
{
    U8 mask = 0x80;
    for (std::size_t i = 0; i < n; ++i)
    {
        bool b = (c & mask) != 0;
        seq.push_back(b);
        mask = mask >> 1;
    }
}

/// Deserialise the blob into a bit sequence.
/// If 'size' is 0, the number of bits in the bit sequence is decoded from the blob.
/// Advance the pointer past the serialised sequence.
/// Return the number of bits in the resulting bit sequence.
std::size_t deserialise_bitseq (const U8*& ptr,
                                Bitseq& seq,
                                std::size_t size = 0)
{
    // read the number of bits in the bit sequence if necessary
    std::size_t M;
    if (size == 0)
    {
        std::size_t M_bytes = deserialise_num(ptr);
        U8 M_bits = *ptr++;
        M = M_bytes*8 + M_bits;
    }
    else M = size;

    std::size_t whole_bytes = M/8;
    std::size_t partial_byte_bits = M%8;

    for (std::size_t i = 0; i < whole_bytes; ++i)
        seq.push_byte(*ptr++);

    if (partial_byte_bits > 0)
        seq.push_byte(*ptr++, partial_byte_bits);

    return M;
}

/// Convert the alphabet, encoding bits and length arrays into a Huffman table.
template <class T>
Table<T> make_table (const std::vector<T>& alphabet,
                     const std::vector<U8>& lengths,
                     const Bitseq& alphabits)
{
    Table<T> table;
    std::size_t o = 0;
    for (std::size_t i = 0; i < alphabet.size(); ++i)
    {
        // lengths[i] = length of this bitseq
        Bitseq bits;
        for (U8 j = 0; j < lengths[i]; ++j)
            bits.push_back(alphabits[o+j]);
        table[alphabet[i]] = bits;
        o += lengths[i];
    }
    return table;
}

/// Deserialise the blob into alphabet, alphabet bit sequence, and length arrays.
/// Advance the pointer to the element past the data.
template <class T>
void deserialise_arrays (const U8*& ptr,
                         std::vector<T>& alphabet,
                         std::vector<U8>& lengths,
                         Bitseq& alphabits)
{
    // read the number of elements in the alphabet
    std::size_t N = deserialise_num(ptr);

    // read the alphabet
    alphabet.resize(N);
    const T* tptr = (const T*) ptr;
    for (std::size_t i = 0; i < N; ++i)
        alphabet[i] = *tptr++;
    ptr = (U8*) tptr;

    // read the alphabit sequence lengths
    lengths.resize(N);
    read(&lengths[0], ptr, N);

    // compute the number of bits in the alphabits sequence
    std::size_t M = 0;
    for (U8 L : lengths)
        M += L;

    // read the alphabits sequence
    deserialise_bitseq(ptr, alphabits, M);

#ifdef ALGORITHM_OUTPUT
    printf("N: %u\n", N);

    printf("Alphabet: ");
    for (const T& x : alphabet)
        printf("%c ", x);
    printf("\n");

    printf("Lengths : ");
    for (U8 l : lengths)
        printf("%d ", l);
    printf("\n");

    printf("Alphabits: ");
    for (bool b : alphabits)
        printf("%d", b);
    printf("\n");
#endif
}

/// Deserialise the blob into a Huffman table and a bit sequence.
template <class T>
void deserialise (const U8*& ptr, Table<T>& table, Bitseq& code)
{
    std::vector<T> alphabet;
    std::vector<U8> lengths;
    Bitseq alphabits;
    deserialise_arrays(ptr, alphabet, lengths, alphabits);
    deserialise_bitseq(ptr, code);
    table = make_table(alphabet, lengths, alphabits);
#ifdef ALGORITHM_OUTPUT
    printf("Alphabet encoding:\n");
    for (const auto& keyval : table)
    {
        printf("%c -> ", keyval.first); // assuming char sequence
        for (bool b : keyval.second)
            printf("%d", b);
        printf("\n");
    }

    printf("M: %u\n", code.size());

    printf("code bits: ");
    for (bool b : code)
        printf("%d", b);
    printf("\n");
#endif
}

/// Decode the sequence using the given Huffman table.
template <class T, class iter_t, class cont_t>
void decode_seq (const iter_t& begin, const iter_t& end,
                 const Table<T>& table, cont_t& cont)
{
    HuffmanTree<T> tree(table);
    tree.decode(begin, end, cont);
}

template <class T, class cont_t>
void decode (const BinaryBlob& blob, cont_t& cont)
{
    Table<T> table;
    Bitseq code;
    const U8* ptr = (const U8*) blob.c_str();
    deserialise(ptr, table, code);
    decode_seq(code.begin(), code.end(), table, cont);
}

} // namespace kxh
