#pragma once

#include "HuffmanNode.h"
#include "Bitseq.h"

#include <unordered_map>
#include <queue>
#include <vector>

namespace kxh
{

/// Maps values to their binary representation.
template <typename T>
using Table = std::unordered_map<T,Bitseq>;

/// Maps values to their frequency in the input data.
template <typename T>
using FrequencyMap = std::unordered_map<T,int>;

/// Construct a Huffman tree from a sequence.
template <class T, class iter_t>
node<T>* from_sequence (iter_t begin, const iter_t& end);

template <class T>
class HuffmanTree
{
public:

    /// Construct a Huffman tree from a sequence.
    template <class iter_t>
    HuffmanTree (iter_t begin, const iter_t& end)
        : root(from_sequence<T>(begin, end)) {}

    /// Construct a Huffman tree from a table.
    HuffmanTree (const Table<T>& table);

    /// Serialise the Huffman tree into a table.
    Table<T> make_table () const;

    /// Decode the bit sequence.
    template <class bits_iter_t, class data_cont_t>
    void decode (bits_iter_t begin, const bits_iter_t& end, data_cont_t& data);

private:

    std::unique_ptr<node<T>> root;
};

/// Compute the sequence's frequency map.
template <class T, class iter_t>
FrequencyMap<T> compute_frequencies (iter_t begin, const iter_t& end)
{
    FrequencyMap<T> freqs;
    for (; begin != end; ++begin) freqs[*begin]++;
    return freqs;
}

template <typename T>
using qelem = std::pair<node<T>*,int>;

/// Compare two nodes using their frequencies.
template <typename T>
struct node_cmp
{
    bool operator() (const qelem<T>& n1, const qelem<T>& n2) const {
        return n1.second > n2.second;
    }
};

/// Construct a Huffman tree from a sequence.
template <class T, class iter_t>
node<T>* from_sequence (iter_t begin, const iter_t& end)
{
    FrequencyMap<T> freqs = compute_frequencies<T>(begin, end);

    using node_queue = std::priority_queue<qelem<T>, std::vector<qelem<T>>, node_cmp<T>>;
    node_queue q;

    // Create a leaf for every symbol and put it in the queue.
    for (const auto& keyval : freqs)
    {
        node<T>* n = new node<T>(new T(keyval.first));
        qelem<T> p = std::make_pair(n, keyval.second);
        q.push(p);
    }

    // Construct the tree.
    while (q.size() > 1)
    {
        qelem<T> p1 = q.top();
        q.pop();
        qelem<T> p2 = q.top();
        q.pop();

        node<T>* n = new node<T>(nullptr, p1.first, p2.first);
        qelem<T> p = std::make_pair(n, p1.second + p2.second);
        q.push(p);
    }

    return q.top().first;
}

/// Construct the path as described by 'path' rooted at the node
/// and insert the given element.
template <class T>
void make_path (node<T>* n, const T& elem, const Bitseq& path)
{
    for (size_t i = 0; i < path.size(); ++i)
    {
        if (path[i] == 0) n = n->safe_left();
        else              n = n->safe_right();
    }
    n->set_elem(new T(elem));
}

/// Construct a Huffman tree from a table.
template <class T>
HuffmanTree<T>::HuffmanTree (const Table<T>& table)
{
    root.reset(new node<T>);
    for (const auto& keyval : table)
        make_path(root.get(), keyval.first, keyval.second);
}

/// Build a Huffman table.
/// 'table' is the output.
/// 'code' is the binary sequence / path / node code corresponding to 'n'.
/// 'n' is the current node to be serialised into the table.
template <class T>
void build_table (Table<T>& table, Bitseq& code, const node<T>* n)
{
    if (n->is_leaf())
    {
        table[n->elem()] = code;
    }
    else // internal node
    {
        if (n->left())
        {
            code.push_bit(0); // left turn
            build_table(table, code, n->left());
            code.pop(); // remove the 0 we just pushed
        }
        if (n->right())
        {
            code.push_bit(1); // right turn
            build_table(table, code, n->right());
            code.pop(); // remove the 1 we just pushed
        }
    }
}

/// Serialise the Huffman tree into a table.
template <class T>
Table<T> HuffmanTree<T>::make_table () const
{
    Table<T> table;
    Bitseq code;
    build_table<T>(table, code, root.get());
    return table;
}

/// Decode the bit sequence.
template <class T> template <class bits_iter_t, class data_cont_t>
void HuffmanTree<T>::decode (bits_iter_t begin, const bits_iter_t& end,
                             data_cont_t& data)
{
    const node<T>* n = root.get();
    for (; begin != end; ++begin)
    {
        if (*begin) n = n->right();
        else        n = n->left();

        if (n->is_leaf())
        {
            data.push_back(n->elem());
            n = root.get();
        }
    }
}

} // namespace kxh
