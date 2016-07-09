//
//  implicit_treap.h
//  hr_array_query
//
//  Created by Chris on 1/16/16.
//  Copyright © 2016 Chris. All rights reserved.
//

#ifndef implicit_treap_h
#define implicit_treap_h
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
struct node {
    int val;
    std::unique_ptr<node> left;
    std::unique_ptr<node> right;
    int size;
    int priority;
    
    node(int v=0, node* l=nullptr, node*r = nullptr) : val(v), left(l), right(r), size(1), priority(std::rand())
    {}
};

// allow size computation with null nodes without extra conditionals cluttering up the code
inline int sz(const std::unique_ptr<node> &t)
{
    return t?t->size:0;
}

class implicit_treap {
    std::unique_ptr<node> root;
    //void postorder_cleanup(node* curr);
    void in_order(std::ostream &os, const std::unique_ptr<node>&) const;
    node* get_node(int index) const; // basic iterator function; we may extend this but for now, privatize
public:
    implicit_treap() : root(nullptr) {}
    
    // move construct from node: this is will be used to prune a tree at some child node
    // implementation in terms of unique_ptr disallows tree objects to refer to a
    // proper subtree of another tree: a tree object always contains a root whose pointer
    // is not owned by anyone else, such as the child node of another tree.
    // (to enable such semantics, we would use shared_ptr's)
    implicit_treap(std::unique_ptr<node>&&);
    
    implicit_treap(std::istream& is, int N);
    
    // deep copy ops
    implicit_treap(const implicit_treap&);
    implicit_treap& operator=(const implicit_treap&);
    
    // move constructor: steal the pointer
    implicit_treap(implicit_treap&& other);
    implicit_treap& operator=(implicit_treap&& other);
    
    
    implicit_treap &insert(int data, int index=-1);
    implicit_treap &remove(int index);
    implicit_treap &merge(implicit_treap &rhs);
    implicit_treap split(int index, int add=0);
    implicit_treap split(std::unique_ptr<node> &curr);
    
    
    
    void in_order(std::ostream &os) const { in_order(os,root); }
    int size() const { return sz(root); }
    const int &operator[](int index) const {return get_node(index)->val; } // array indexing
    int &operator[](int index) { return const_cast<int&>(static_cast<const implicit_treap&>(*this)[index]); }
    const int &at(int index) const; // array indexing, checked
    int &at(int index) { return const_cast<int&>(static_cast<const implicit_treap&>(*this).at(index)); }
    
    int getkey(node* curr) const ;
    ~implicit_treap();
    
    bool empty() { return !root;}
};

inline std::ostream &operator <<(std::ostream&os, const implicit_treap& it)
{
    it.in_order(os);
    return os;
}
#endif /* implicit_treap_h */
