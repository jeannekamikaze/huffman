#pragma once

#include <string>

namespace kxh
{

using BinaryBlob = std::string;

/// Encode the sequence using Huffman encoding.
template <class T, class iter_t>
BinaryBlob encode (iter_t begin, const iter_t& end);

/// Decode the binary blob using Huffman encoding.
template <class T, class cont_t>
void decode (const BinaryBlob&, cont_t& cont);

} // namespace kxh

#include "encode.h"
#include "decode.h"
