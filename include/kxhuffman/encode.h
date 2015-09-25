#pragma once

#include "HuffmanTree.h"
#include "common.h"

#include <vector>
#include <string>

namespace kxh
{

/// Perform a memory copy and advance the destination pointer.
void write (U8*& dst, const void* src, std::size_t num_bytes)
{
    memcpy(dst, src, num_bytes);
    dst += num_bytes;
}

/// Serialise the number.
BinaryBlob serialise_num (std::size_t val)
{
    if (val <= 255)
    {
        BinaryBlob buf(1+1,0);
        buf[0] = num_byte;
        buf[1] = (U8) val;
        return buf;
    }
    else if (val <= 65535)
    {
        U16 val16 = (U16) val;
        BinaryBlob buf(1+2,0);
        buf[0] = num_word;
        memcpy(&buf[1], &val16, sizeof(U16));
        return buf;
    }
    else if (val <= 4294967295)
    {
        U32 val32 = (U32) val;
        BinaryBlob buf(1+4,0);
        buf[0] = num_dword;
        memcpy(&buf[1], &val32, sizeof(U32));
        return buf;
    }
    else
    {
        U64 val64 = (U64) val;
        BinaryBlob buf(1+8,0);
        buf[0] = num_qword;
        memcpy(&buf[1], &val64, sizeof(U64));
        return buf;
    }
}

/// Encode the sequence using the given Huffman table.
template <class T, class iter_t>
Bitseq encode_seq (iter_t begin, const iter_t& end, const Table<T>& table)
{
    DEBUG_PRINT("Code sequence encoding:\n");
    Bitseq seq;
    for (; begin != end; ++begin)
    {
        auto it = table.find(*begin);
        DEBUG_ASSERT(it != table.end());
        seq.push_back(it->second);
#ifdef ALGORITHM_OUTPUT
        const Bitseq& c = it->second;
        DEBUG_PRINT("%c -> ", *begin); // assuming char sequence
        for (size_t i = 0; i < c.size(); ++i)
            DEBUG_PRINT("%d", c[i]);
        DEBUG_PRINT("\n");
#endif
    }
    return seq;
}

/// Rotate the byte 1 bit to the right.
inline U8 rotate_right (U8 c)
{
    return (c >> 1) | ((c & 1) << 7);
}

/// Serialise the bit sequence.
/// If write_num = false, then the number of bits in the bit sequence is not
/// included in the blob.
BinaryBlob serialise_bitseq (const Bitseq& bitseq, bool write_num = true)
{
    const std::size_t n = bitseq.size();

    std::string M_bytes = write_num ? serialise_num(n/8) : std::string();
    U8 M_bits  = write_num ? n%8 : 0;

    std::size_t buf_size
            = (n/8)  // number of whole bytes
            + (n%8 == 0 ? 0 : 1); // +1 if the last byte is not whole

    if (write_num)
    {
        buf_size += M_bytes.size();
        buf_size += 1; // M_bits
    }

    std::string data(buf_size,0);
    U8* ptr = (U8*) data.c_str();

    // write the number of bits in the bit sequence
    if (write_num)
    {
        write(ptr, &M_bytes[0], M_bytes.size());
        write(ptr, &M_bits, 1);
    }

    // write the bit sequence
    // start with 10000000
    // then rotate the mask to the right to write the next bit in the sequence
    U8 mask = 0x80;
    for (size_t i = 0; i < n; ++i)
    {
        ptr[i >> 3] |= (mask * (U8) bitseq[i]);
        mask = rotate_right(mask);
    }

    return data;
}

/// Convert the Huffman table into alphabet, encoding bits and length arrays.
template <class T>
void make_arrays (const Table<T>& table,
                  std::vector<T>& alphabet,
                  std::vector<U8>& lengths,
                  Bitseq& alphabits)
{
    for (const auto& keyval : table)
    {
        std::size_t num_bits = keyval.second.size();
        alphabet.push_back(keyval.first);
        lengths.push_back((U8)num_bits);
        for (std::size_t i = 0; i < num_bits; ++i)
            alphabits.push_back(keyval.second[i]);
    }
#ifdef ALGORITHM_OUTPUT
    printf("Alphabet encoding:\n");
    for (const auto& keyval : table)
    {
        printf("%c -> ", keyval.first); // assuming char sequence
        for (bool b : keyval.second)
            printf("%d", b);
        printf("\n");
    }
#endif
}

/// Serialise the alphabet, alphabet bit sequence, and length arrays.
template <class T>
BinaryBlob serialise_arrays (const std::vector<T>& alphabet,
                             const std::vector<U8>& lengths,
                             const Bitseq& alphabits)
{
    // do not include the number of bits in the serialised alphabits.
    // the lengths vector is enough to decode the alphabits.
    BinaryBlob serial_alphabits = serialise_bitseq(alphabits, false);

    std::string N = serialise_num(alphabet.size());

    size_t buf_size
            = N.size() // number of alphabet elements
            + sizeof(T) * alphabet.size() // alphabet elements
            + lengths.size() // alphabet bit sequence lengths
            + serial_alphabits.size(); // alphabet bit sequences

    std::string buf(buf_size,0);
    U8* ptr = (U8*) buf.c_str();

    write(ptr, &N[0], N.size());
    write(ptr, &alphabet[0], sizeof(T) * alphabet.size());
    write(ptr, &lengths[0], lengths.size());
    write(ptr, &serial_alphabits[0], serial_alphabits.size());

#ifdef ALGORITHM_OUTPUT
    printf("N: ");
    for (std::size_t i = 0; i < N.size(); ++i)
        printf("%02x ", N[i]);
    printf("\n");

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

    return buf;
}

/// Serialise the Huffman table and bit sequence.
template <class T>
BinaryBlob serialise (const Table<T>& table, const Bitseq& code)
{
    Bitseq alphabits;
    std::vector<T> alphabet;
    std::vector<U8> lengths;
    make_arrays(table, alphabet, lengths, alphabits);

    BinaryBlob serial_tree = serialise_arrays(alphabet, lengths, alphabits);
    BinaryBlob serial_code = serialise_bitseq(code);

    std::size_t s = serial_tree.size() + serial_code.size();
    std::string buf(s, 0);
    U8* ptr = (U8*) buf.c_str();

    write(ptr, &serial_tree[0], serial_tree.size());
    write(ptr, &serial_code[0], serial_code.size());

#ifdef ALGORITHM_OUTPUT
    printf("M: %u\n", code.size());

    printf("code bits: ");
    for (bool b : code)
        printf("%d", b);
    printf("\n");
#endif

    return buf;
}

template <class T, class iter_t>
BinaryBlob encode (iter_t begin, const iter_t& end)
{
    HuffmanTree<T> t(begin, end);
    Table<T> table = t.make_table();
    Bitseq code = encode_seq(begin, end, table);
    return serialise<T>(table, code);
}

} // namespace kxh
