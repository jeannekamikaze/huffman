#pragma once

#include <memory>

namespace kxh
{

/// A node in a Huffman tree.
template <class T>
class node
{
public:

    node (T* e = nullptr, node<T>* l = nullptr, node<T>* r = nullptr)
        : elem_(e), left_(l), right_(r) {}

    /// Create the node's left child if it does not exist, then return it.
    node<T>* safe_left () {
        if (!left_.get())
            left_.reset(new node<T>());
        return left_.get();
    }

    /// Create the node's right child if it does not exist, then return it.
    node<T>* safe_right () {
        if (!right_.get())
            right_.reset(new node<T>());
        return right_.get();
    }

    /// Return the node's left child.
    /// Return null if the node is a leaf.
    const node<T>* left () const {
        return left_.get();
    }

    /// Return the node's right child.
    /// Return null if the node is a leaf.
    const node<T>* right () const {
        return right_.get();
    }

    /// Get the node's element.
    /// Pre: is_leaf()
    const T& elem () const {
        return *elem_;
    }

    /// Set the node's element.
    void set_elem (T* e) {
        elem_.reset(e);
    }

    /// Return true if the node is a leaf, false otherwise.
    bool is_leaf () const {
        return left() == nullptr && right() == nullptr;
    }

private:

    std::unique_ptr<T> elem_;
    std::unique_ptr<node<T>> left_;
    std::unique_ptr<node<T>> right_;
};

} // namespace kxh
