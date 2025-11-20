#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void rLeft(AVLNode<Key, Value>* node);
    void rRight(AVLNode<Key,Value>* node);
    void insertBalance(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* node);
    void removeBalance(AVLNode<Key, Value>* node, int diff);

};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // empty tree
    if(this->root_ == NULL) {
        this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, NULL);
        return;
    }

    //find insertion location, like in bst
    AVLNode<Key, Value>* curr = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key,Value>* parent = NULL;

    while (curr != NULL) {
        parent = curr;

        //if key is found update value and return
        if (new_item.first == curr->getKey()) {
            curr->setValue(new_item.second);
            return;
        } else if (new_item.first < curr->getKey()) {
            curr = curr->getLeft();
        } else {
            curr = curr->getRight();
        }
    }

    //create new node
    AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, parent);

    if (new_item.first < parent->getKey()) {
        parent->setLeft(newNode);
    } else {
        parent->setRight(newNode);    
    }

    //update balance and fix accordingly
    if (parent->getBalance() == -1 || parent->getBalance() == 1) {
        parent->setBalance(0);
    } else {
        if (newNode == parent->getLeft()) {
            parent->updateBalance(1);
        } else {
            parent->updateBalance(-1);
        }

    insertBalance(parent, newNode);

    }

}

template<class Key, class Value>
void AVLTree<Key, Value>::insertBalance(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* node) 
{// empty tree
    if (parent == NULL || parent->getParent() == NULL) {
        return;
    }
    //parent of parent - grandparent
    AVLNode<Key, Value>* gParent = parent->getParent();

    // parent is left child of gParent
    if (parent == gParent->getLeft()) {
        gParent->updateBalance(1);

        if (gParent->getBalance() == 0) {
            return;
        } else if (gParent->getBalance() == 1) {
            insertBalance(gParent, parent);
        } else { //need to rotate
            if (node == parent->getLeft()) {
                //rotate right
                rRight(gParent);
                parent->setBalance(0);
                gParent->setBalance(0);
            } else {
                // left then right rotate
                rLeft(parent);
                rRight(gParent);

                if (node->getBalance() == 1) {
                    parent->setBalance(0);
                    gParent->setBalance(-1);
                } else if (node->getBalance() == 0) {
                    parent->setBalance(0);
                    gParent->setBalance(0);
                } else {
                    parent->setBalance(1);
                    gParent->setBalance(0);
                }
                node->setBalance(0);
            }
        }
    } else { // parent is right child of gParent
        gParent->updateBalance(-1);

        if (gParent->getBalance() == 0) {
            return;
        } else if (gParent->getBalance() == -1) {
            insertBalance(gParent, parent);
        } else { //need to rotate
            if (node == parent->getRight()) {
                //rotate left
                rLeft(gParent);
                parent->setBalance(0);
                gParent->setBalance(0);
            } else {
                // right then left rotate
                rRight(parent);
                rLeft(gParent);

                if (node->getBalance() == -1) {
                    parent->setBalance(0);
                    gParent->setBalance(1);
                } else if (node->getBalance() == 0) {
                    parent->setBalance(0);
                    gParent->setBalance(0);
                } else {
                    parent->setBalance(-1);
                    gParent->setBalance(0);
                }
                node->setBalance(0);
            }
        }
    }

}





/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
    
    AVLNode<Key, Value>*node = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));

    // empty tree
    if (node == NULL) {
        return;
    }

    // if 2 children
    if (node->getLeft() != NULL && node->getRight() != NULL) {
        AVLNode<Key, Value>* pre = static_cast<AVLNode<Key, Value>*>(BinarySearchTree<Key, Value>::predecessor(node));
        nodeSwap(node, pre);
    }

    // node has 1 or 0 children
    AVLNode<Key, Value>* parent = node->getParent();
    int diff = 0;

    if (parent != NULL) {
        if (node == parent->getLeft()) {
            diff = -1; // remove from left 
        } else {
            diff = 1; //remove from right
        }
    }

    // move child up
    AVLNode<Key, Value>* child = node->getLeft();
    if (child == NULL) {
        child = node->getRight();
    }

    if (parent == NULL) {
        this->root_ = child;
    } else if (node == parent->getLeft()) {
        parent->setLeft(child);
    } else {
        parent->setRight(child);
    }

    if (child != NULL) {
        child->setParent(parent);
    }
    // delete the node
    delete node;
    // fix the balance
    removeBalance(parent, diff);
}


template<class Key, class Value>
void AVLTree<Key, Value>::removeBalance(AVLNode<Key, Value>* node, int diff)
{// empty tree
    if (node == NULL) {
        return;
    }
    //set parent node
    AVLNode<Key, Value>* parent = node->getParent();

    int nDiff = 0;
    if (parent != NULL) {
        if (node == parent->getLeft()) {
            nDiff = -1;
        } else {
            nDiff = 1;
        }
    }

    if (diff == -1) {
        // remove from left subtree
        if (node->getBalance() + diff == -2) {
            AVLNode<Key, Value>* biggerC = node->getRight();
            if (biggerC->getBalance() == -1) {
                // rotate left since right right
                rLeft(node);
                node->setBalance(0);
                biggerC->setBalance(0);
                removeBalance(parent, nDiff);
            } else if (biggerC->getBalance() == 0) {
                rLeft(node);
                node->setBalance(-1);
                biggerC->setBalance(1);
            } else {
                // rotate right and then left
                AVLNode<Key, Value>* gChild = biggerC->getLeft();
                rRight(biggerC);
                rLeft(node);

                if (gChild->getBalance() == 1) {
                    node->setBalance(0);
                    biggerC->setBalance(-1);
                } else if (gChild->getBalance() == 0) {
                    node->setBalance(0);
                    biggerC->setBalance(0);
                } else {
                    node->setBalance(1);
                    biggerC->setBalance(0);
                }
                gChild->setBalance(0);
                removeBalance(parent, nDiff);
            }
        } else if (node->getBalance() + diff == -1) {
            node->setBalance(-1);
        } else {
            node->setBalance(0);
            removeBalance(parent, nDiff);
        }
    
    } else {
        // remove from right subtree
        if (node->getBalance() + diff == 2) {
            AVLNode<Key, Value>* biggerC = node->getLeft();
            if (biggerC->getBalance() == 1) {
                // rotate right since left left
                rRight(node);
                node->setBalance(0);
                biggerC->setBalance(0);
                removeBalance(parent, nDiff);

            } else if (biggerC->getBalance() == 0) {
                rRight(node);
                node->setBalance(1);
                biggerC->setBalance(-1);

            } else {
                // rotate left and then right
                AVLNode<Key, Value>* gChild = biggerC->getRight();
                rLeft(biggerC);
                rRight(node);

                if (gChild->getBalance() == -1) {
                    node->setBalance(0);
                    biggerC->setBalance(1);
                } else if (gChild->getBalance() == 0) {
                    node->setBalance(0);
                    biggerC->setBalance(0);
                } else {
                    node->setBalance(-1);
                    biggerC->setBalance(0);
                }
                gChild->setBalance(0);
                removeBalance(parent, nDiff);
            }
        } else if (node->getBalance() + diff == 1) {
            node->setBalance(1);
        } else {
            node->setBalance(0);
            removeBalance(parent, nDiff);
        }
    }

}


template<class Key, class Value>
void AVLTree<Key, Value>::rLeft(AVLNode<Key, Value>* node) 
{
    AVLNode<Key, Value>* childR = node->getRight();

    //move childR left subtree to node right
    node->setRight(childR->getLeft());
    if (childR->getLeft() != NULL) {
        childR->getLeft()->setParent(node);
    }

    //link childR to node parent
    childR->setParent(node->getParent());
    //check if the node was the root
    if (node->getParent() == NULL) {
        this->root_ = childR;
    } else if (node == node->getParent()->getLeft()) {
        node->getParent()->setLeft(childR);
    } else {
        node->getParent()->setRight(childR);
    }

    //put the node as childR left childR
    childR->setLeft(node);
    node->setParent(childR);

}

template<class Key, class Value>
void AVLTree<Key, Value>::rRight(AVLNode<Key, Value>* node) 
{
    AVLNode<Key, Value>* childL = node->getLeft();

    //move childL right subtree to node left
    node->setLeft(childL->getRight());
    if (childL->getRight() != NULL) {
        childL->getRight()->setParent(node);
    }

    //link childL to node parent
    childL->setParent(node->getParent());
    //check if the node was the root
    if (node->getParent() == NULL) {
        this->root_ = childL;
    } else if (node == node->getParent()->getLeft()) {
        node->getParent()->setLeft(childL);
    } else {
        node->getParent()->setRight(childL);
    }

    //put the node as childR left childL
    childL->setRight(node);
    node->setParent(childL);

}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
