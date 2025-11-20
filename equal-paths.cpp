#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)
#include <iostream>
#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
// create a function to check for each node's height
// return -1 if any of the paths differ
// C1: if node is null return -1
// C2: leaf node return 0
// C3: recursively call on the node

int heightCheck(Node * node) {
  if (node == NULL) {
    return -1;
  }

  if (node->left == NULL && node->right == NULL) {
    return 0;
  }

  int lHeight = heightCheck(node->left);
  int rHeight = heightCheck(node->right);

  // if either side of the tree has been found to not been balanced, immedietly return -2
  if (lHeight == -2 || rHeight == -2) {
    return -2;
  }

  // if one side of the tree is null use the other side + 1 
  if (lHeight == -1) {
    return rHeight + 1;
  }

  if (rHeight == -1) {
    return lHeight + 1;
  }

  // check if l and r side don't have equal heights return -2
  if (rHeight != lHeight) {
    return -2;
  }

  // if both sides have equal lengths add 1 to lHeight so that the parent node is correct
  return lHeight + 1;

}



bool equalPaths(Node * root)
{
  if (root == NULL) {
    return true;
  }

  int height = heightCheck(root);
  if (height == -2) {
    return false;
  } else {
    return true;
  }
  
}

