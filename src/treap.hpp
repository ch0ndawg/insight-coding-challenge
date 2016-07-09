//
//  Treap.h
//  hr_array_query
//
//  Created by Chris on 1/16/16.
//  Copyright © 2016 Chris. All rights reserved.
//

#ifndef Treap_h
#define Treap_h
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <memory>

// design decision: unique_ptrs help with memory management.
// however they are subject to some restrictions: nodes may only have one owner
// (by definition of unique_ptr), and either belong to one and only one tree,
// or one and only one parent node.
// In particular, we cannot use tree objects to refer to subtrees
// Actually, a node itself is the true recursive data structure; tree objects merely provide interface.
template <class Key, class Value, class Priority>
struct TreapNode {
    Key key;
    Value val;
    const Priority &priority;
    std::unique_ptr<node<Key,Value,Priority>> left;
    std::unique_ptr<node<Key,Value,Priority>> right;

    // extra size field for maintaining order statistics
    int size;

    TreapNode(const Key& k, const Value& v, const Priority &p, node* l=nullptr, node*r = nullptr)
    : Key(k), val(v), left(l), right(r), size(1), priority(p)
    {}
};

// allow size computation with null nodes without extra conditionals cluttering up the code
template <class Key, class Value, class Priority>
inline int sz(const std::unique_ptr<TreapNode<Key,Value,Priority>> &t)
{
    return t?t->size:0;
}

template<class Key, class Value, class Priority>
class Treap {
    // unwieldy
    using node = TreapNode<Key, Value, Priority>;

    std::unique_ptr<node> root;
    //void postorder_cleanup(node* curr);
    void in_order(std::ostream &os, const std::unique_ptr<node>&) const;
    node* get_node(int index) const; // basic iterator function; we may extend this but for now, privatize
public:
    Treap() : root(nullptr) {}

    // move construct from node: this is will be used to prune a tree at some child node
    // implementation in terms of unique_ptr disallows tree objects to refer to a
    // proper subtree of another tree: a tree object always contains a root whose pointer
    // is not owned by anyone else, such as the child node of another tree.
    // (to enable such semantics, we would use shared_ptr's)
    Treap(std::unique_ptr<node>&&);

    Treap(std::istream& is, int N);

    // deep copy ops
    Treap(const Treap<Key, Value, Priority>&);
    Treap<Key, Value, Priority>& operator=(const Treap<Key, Value, Priority>&);

    // move constructor: steal the pointer
    Treap(Treap<Key, Value, Priority>&& other);
    Treap<Key, Value, Priority>& operator=(Treap<Key, Value, Priority>&& other);


    Treap<Key, Value, Priority> &insert(const Key& k, const Value & data);
    Treap<Key, Value, Priority> &remove(const Key& k);
    Treap<Key, Value, Priority> &merge(Treap &rhs);
  //  Treap<Key, Value, Priority> split(int index, int add=0);
    Treap<Key, Value, Priority> split(std::unique_ptr<node> &curr);



    void in_order(std::ostream &os) const { in_order(os,root); }
    int size() const { return sz(root); }
    const int &operator[](int index) const {return get_node(index)->val; } // array indexing
    int &operator[](int index) { return const_cast<int&>(static_cast<const Treap<Key, Value, Priority>&>(*this)[index]); }
    const int &at(int index) const; // array indexing, checked
    int &at(int index) { return const_cast<int&>(static_cast<const Treap<Key, Value, Priority>&>(*this).at(index)); }

    int getkey(node* curr) const ;
    ~Treap();

    bool empty() { return !root;}
};

inline std::ostream &operator <<(std::ostream&os, const Treap& it)
{
    it.in_order(os);
    return os;
}
#endif /* Treap_h */
