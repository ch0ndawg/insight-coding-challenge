//
//  implicit_treap.cpp
//  hr_array_query
//
//  Created by Chris on 1/16/16.
//  Copyright © 2016 Chris. All rights reserved.
//
#include <cmath>
#include <vector>
#include <memory>
#include <iostream>

#include "implicit_treap.h"
using namespace std;


implicit_treap::implicit_treap(std::unique_ptr<node>&& other)
: root(std::move(other)) // move construct; null out the destination. incredibly useful for splitting
{
    // possibly do rebalancing (although it seems safe to assume that a child node is already balanced
}

implicit_treap::~implicit_treap() = default; // default destructor destroys elements in order of declaration

// default move construction transfers ownership of the smart pointer to the destination
implicit_treap::implicit_treap(implicit_treap&& other) = default;
/*: root(other.root)
{
    other.root = nullptr;
} */

implicit_treap::implicit_treap(std::istream& is, int N)
: implicit_treap() // delegating constructor
{
    for (int i=0; i<N;i++) {
        int x;
        is >> x;
        // insert in order
        insert(x);
    }
}
// default move assignment swaps ownership with the expiring temporary
implicit_treap& implicit_treap::operator=(implicit_treap &&other) = default;
/*{
    // swap; then the old tree that was *this will be deallocated by the destructor
    node *temp = root;
    root = other.root;
    other.root=temp;
    
    return *this;
} */

/*void implicit_treap::postorder_cleanup(node* curr)
{
    if (!curr) return; // null: do nothing
    postorder_cleanup(curr->left); // clean up left subtree
    postorder_cleanup(curr->right); // clean up right subtree
    delete curr; // clean up this node
}*/

// Insert, using randomly generated priorities.


implicit_treap& implicit_treap::insert(int data, int index)
{
    // a one-element treap
    implicit_treap temp(unique_ptr<node>(new node(data)));
    if (index==-1) {
        merge(temp);
    } else {
        // split the tree at index
        auto right_tree = split(index);
        // merge with the newly created node
        merge(temp);
        // merge with the right tree
        merge(right_tree);
    }
    // (merge and split take care of all size management)
    return *this;
}

implicit_treap& implicit_treap::remove(int index)
{
    // get the right tree
    auto right_tree = split(index);
    // toss the actual element by splitting. The unused return value will call the destructor
    split(index-1);
    // merge with the remainder
    merge(right_tree);
    return *this;
}

implicit_treap& implicit_treap::merge(implicit_treap &rhs)
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
            implicit_treap temp(std::move(rhs.root->left));
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
            implicit_treap temp(std::move(root->right));
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

implicit_treap implicit_treap::split(int index, int add)
{
    implicit_treap res;
    if (!empty()) {
        // for implicit key calculation
        // second param. takes care of right children addition
        int curr_pos = add + sz(root->left);
        if (curr_pos <= index) {
            // cut off the right subtree
            implicit_treap temp(std::move(root->right));
            
            // recursively split the right subtree,
            // store *its* right split in res and temp becomes *its* left split
            res = temp.split(index,curr_pos+1);
            
            // make the temp's left split as the right subree part of this left split
            root->right = std::move(temp.root);
            // update size
            root->size =sz(root->left)+1+sz(root->right);
        } else {
            // get the left subtree
            implicit_treap temp(std::move(root->left));
            
            // split the left subtree, store *its* right split in res and temp becomes *its* left split
            res = temp.split(index, add);
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

int implicit_treap::getkey(node* theNode) const
{ /*
   node * curr = root.get();
   while (true) {
   while (curr)
   } */
    // we have to do inorder traversal (linear time)
    // if we store the parents, this can be accomplished in logarithmic time
    return 0;
}
// Array indexing operation: the implicit key
// Basic strategy: count the predecessors using stored size information

node *implicit_treap::get_node(int index) const
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

// checked access
const int & implicit_treap::at(int index) const
{
    if (index < 0 || index >= size()) throw out_of_range("Tree out of bounds");
    return (*this)[index];
}



void implicit_treap::in_order(std::ostream &os, const unique_ptr<node>& curr) const
{
    if (!curr) {
        return;
    }
    in_order(os,curr->left);
    os << curr->val << ' ';
    in_order(os,curr->right);
}

