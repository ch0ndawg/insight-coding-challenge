//
//  Treap.h
//  hr_array_query
//
//  Created by Chris on 1/16/16.
//  Copyright © 2016 Chris. All rights reserved.
//

#ifndef Treap_h
#define Treap_h
#include <cmath>
#include <vector>
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <memory>
#include <utility>

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
    Priority priority;
    std::unique_ptr<TreapNode<Key,Value,Priority>> left;
    std::unique_ptr<TreapNode<Key,Value,Priority>> right;

    // extra size field for maintaining order statistics
    int size;

    TreapNode(const Key& k, const Value& v, Priority p, TreapNode<Key,Value,Priority>* l=nullptr, TreapNode<Key,Value,Priority>*r = nullptr)
    : key(k), val(v), left(l), right(r), size(1), priority(p)
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
    // get rid of unwieldy typename
    using node = TreapNode<Key, Value, Priority>;

    std::unique_ptr<node> root;
    //void postorder_cleanup(node* curr);
    void in_order(std::ostream &os, const std::unique_ptr<node>&) const;

    // order statistics
    node* getNode(int index) const; // basic iterator function; we may extend this but for now, privatize
    node* predecessor(const Key& k) const; // again, we may extend this for better functionality, but for now, privatize
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

    // allow default construction of the priority
    Treap<Key, Value, Priority> &insert(const Key& k, const Value & data, Priority p=Priority());
    Treap<Key, Value, Priority> &remove(const Key& k);
    Treap<Key, Value, Priority> &merge(Treap<Key, Value, Priority> &rhs);
    Treap<Key, Value, Priority> split(const Key &, bool inclusive=true);
    Treap<Key, Value, Priority> split(std::unique_ptr<node> &curr);



    void in_order(std::ostream &os) const { in_order(os,root); }
    int size() const { return sz(root); }

    // subscripting
    const Value &operator[](const Key &k) const;
    std::pair<Key, Value> orderStatistic(int index) const
    { node *res = getNode(index); return std::make_pair(res->key,res->val); } // array indexing

    Value &operator[](const Key &k) { return const_cast<Value&>(static_cast<const Treap<Key, Value, Priority>&>(*this)[k]); }

    // int getkey(node* curr) const ; // unnecessary for explicit treaps; however may be useful for computing order statistic
    ~Treap();

    bool empty() { return !root;}
};

template <class Key, class Value, class Priority>
inline std::ostream &operator <<(std::ostream&os, const Treap<Key, Value, Priority>& it)
{
    it.in_order(os);
    return os;
}

//using namespace std;

// IMPLEMENTATIONS

template<class Key, class Value, class Priority>
Treap<Key, Value, Priority>::Treap(std::unique_ptr<node>&& other)
: root(std::move(other)) // move construct; null out the destination. incredibly useful for splitting
{
    // possibly do rebalancing (although it seems safe to assume that a child node is already balanced
}

template<class Key, class Value, class Priority>
Treap<Key, Value, Priority>::~Treap() = default; // default destructor destroys elements in order of declaration

// default move construction transfers ownership of the smart pointer to the destination

template<class Key, class Value, class Priority>
Treap<Key, Value, Priority>::Treap(Treap<Key, Value, Priority>&& other) = default;
/*: root(other.root)
{
    other.root = nullptr;
} */


// default move assignment swaps ownership with the expiring temporary
template<class Key, class Value, class Priority>
Treap<Key, Value, Priority>& Treap<Key, Value, Priority>::operator=(Treap<Key, Value, Priority> &&other) = default;
/*{
    // swap; then the old tree that was *this will be deallocated by the destructor
    node *temp = root;
    root = other.root;
    other.root=temp;

    return *this;
} */

/*void Treap::postorder_cleanup(node* curr) // this is taken care of by smart pointers
{
    if (!curr) return; // null: do nothing
    postorder_cleanup(curr->left); // clean up left subtree
    postorder_cleanup(curr->right); // clean up right subtree
    delete curr; // clean up this node
}*/

template<class Key, class Value, class Priority>
Treap<Key, Value, Priority>& Treap<Key, Value, Priority>::insert(const Key& k, const Value &data, Priority p)
{
    // a one-element treap
    Treap<Key, Value, Priority> temp(std::unique_ptr<node>(new node(k, data,p)));

    // split the tree at index
    auto right_tree = split(k);
    // merge with the newly created node
    merge(temp);
    // merge with the right tree
    merge(right_tree);

    // (merge and split take care of all size management)
    return *this;
}

template<class Key, class Value, class Priority>
Treap<Key, Value, Priority>& Treap<Key, Value, Priority>::remove(const Key& k)
{
    // get the right tree
    auto right_tree = split(k);
 
    // toss the actual element by splitting. The unused return value will call the destructor
    // the only time we use non-inclusive splitting
    split(k, /* inclusive */ false);
    // merge with the remainder
    merge(right_tree);
    
    return *this;
}

template<class Key, class Value, class Priority>
Treap<Key, Value, Priority>& Treap<Key, Value, Priority>::merge(Treap<Key, Value, Priority> &rhs)
{
    // if current tree is empty, move rhs to it.
    if (empty()) {
        *this = std::move(rhs);
    }
    if (!rhs.empty()) {
        // must ensure priority is compatible
        // right tree has higher priority
        if (root->priority < rhs.root->priority) {
            // cut off the left branch of rhs tree:
            Treap<Key, Value, Priority> temp(std::move(rhs.root->left));
            // merge with cut left branch; assume all its sizes are correct
            merge(temp);
            // reattach merged tree to left
            rhs.root->left = std::move(root);
            // update size
            rhs.root->size = sz(rhs.root->left) + 1 + sz(rhs.root->right);
            // call this the tree
            *this = std::move(rhs);
        }
        // right tree has lower priority
        else {
            // cut off right branch of this tree
            Treap<Key, Value, Priority> temp(std::move(root->right));
            // merge the cut branch with rhs; assume its sizes are correct
            temp.merge(rhs);
            // reattach merged tree to the right
            root->right = std::move(temp.root);
            // update size
            root->size = sz(root->left)+1+sz(root->right);
        }
    }
    return *this;
}

template<class Key, class Value, class Priority>
Treap<Key, Value, Priority> Treap<Key, Value, Priority>::split(const Key& index, bool inclusive)
{
    Treap<Key, Value, Priority> res;
    if (!empty()) {
        bool rootIsLess = inclusive? (root->key <= index) : (root->key < index);
        if (rootIsLess) {
            // cut off the right subtree
            Treap<Key, Value, Priority> temp(std::move(root->right));

            // recursively split the right subtree,
            // store *its* right split in res and temp becomes *its* left split
            res = temp.split(index,inclusive);

            // make the temp's left split as the right subree part of this left split
            root->right = std::move(temp.root);
            // update size
            root->size =sz(root->left)+1+sz(root->right);
        } else {
            // get (and cut off) the left subtree
            Treap<Key, Value, Priority> temp(std::move(root->left));

            // split the left subtree, store *its* right split in res and temp becomes *its* left split
            res = temp.split(index,inclusive);
            // make this result the left subtree
            root->left = std::move(res.root);

            // update size
            root->size =sz(root->left)+1+sz(root->right);


            // swap out. Want this tree to be the left result,

            // so move the root to the return value (right result)
            res = std::move(*this);

            // set this tree to temp
            *this = std::move(temp);
        }
    }
    return res;
}

//template<class Key, class Value, class Priority>
//int Treap<Key,Value,Priority>::getkey(node* theNode) const
//{ /*
//   node * curr = root.get();
//   while (true) {
//   while (curr)
//   } */
//    // we have to do inorder traversal (linear time)
//    // if we store the parents, this can be accomplished in logarithmic time
//    return 0;
//}
// Array indexing operation: the implicit key
// Basic strategy: count the predecessors using stored size information

template <class Key, class Value, class Priority>
typename Treap<Key, Value, Priority>::node *Treap<Key, Value, Priority>::getNode(int index) const
{
    // use raw pointers: here, we are using pointers in their *iterator* sense.
    node *curr = root.get();
    int count = 0; // current count so far

    while (true) {
        int leftCount = sz(curr->left); // get size of the left node
        // add the size of the left subtree to the current count
        // if this exceeds the index, adjust and move left until it doesn't exceed
        while (leftCount + count > index) {
            curr = curr->left.get();
            leftCount = sz(curr->left);
        }
        count += leftCount; // add this count on
        // if moving left overshoots the mark, then there must be some right children to consider
        if (count < index) {
            count++;
            curr = curr->right.get(); // move right, and loop again (to find leftmost)
        }
        else break; // otherwise, we've landed spot on
    }
    // return the current value
    return curr;
}

template <class Key, class Value, class Priority>
typename Treap<Key, Value, Priority>::node *Treap<Key, Value, Priority>::predecessor(const Key& k) const
{
    return nullptr;
}


template <class Key, class Value, class Priority>
void Treap<Key, Value, Priority>::in_order(std::ostream &os, const std::unique_ptr<node>& curr) const
{
    if (!curr) {
        return;
    }
    in_order(os,curr->left);
    os << curr->val << ' ';
    in_order(os,curr->right);
}


#endif /* Treap_h */
