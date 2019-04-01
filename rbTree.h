//
// Created by matthew on 3/17/19.
//

#ifndef FOOD_TRACKER_RBTREE_H
#define FOOD_TRACKER_RBTREE_H

#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>
#include "food.h"

Node* NIL;

Tree* rbNewTree(Key key)
{
    Tree* tree = malloc(sizeof(Tree));
    tree->root = NIL;
    tree->key = key;
    return tree;
}

void rbDeleteTreeRecursive(Node* node, bool deleteFoods)
{
    if (node != NIL)
    {
        rbDeleteTreeRecursive(node->left, deleteFoods);
        if (deleteFoods)
            deleteFood(node->food);
        deleteNode(node);
        rbDeleteTreeRecursive(node->right, deleteFoods);
    }
}

void rbDeleteTree(Tree* tree, bool deleteFoods)
{
    rbDeleteTreeRecursive(tree->root, deleteFoods);
    free(tree);
}

// page 313
void rbLeftRotate(Tree* tree, Node* node)
{
    Node* y = node->right;
    node->right = y->left;
    if (y->left != NIL)
        y->left->parent = node;
    y->parent = node->parent;
    if (node->parent == NIL)
        tree->root = y;
    else if (node == node->parent->left)
        node->parent->left = y;
    else
        node->parent->right = y;
    y->left = node;
    node->parent = y;
}

// page 313
void rbRightRotate(Tree* tree, Node* node)
{
    Node* y = node->left;
    node->left = y->right;
    if (y->right != NIL)
        y->right->parent = node;
    y->parent = node->parent;
    if (node->parent == NIL)
        tree->root = y;
    else if (node == node->parent->right)
        node->parent->right = y;
    else
        node->parent->left = y;
    y->right = node;
    node->parent = y;
}

// page 316
void rbInsertFixup(Tree* tree, Node* node)
{
    while (node->parent->color == RED)
    {
        // if parent is a left child
        if (node->parent == node->parent->parent->left)
        {
            Node* y = node->parent->parent->right;
            if (y->color == RED)
            {
                node->parent->color = BLACK;
                y->color = BLACK;
                node->parent->parent->color = RED;
                node = node->parent->parent;
            }
            else if (node == node->parent->right)
            {
                node = node->parent;
                rbLeftRotate(tree, node);
            }
            else
            {
                node->parent->color = BLACK;
                node->parent->parent->color = RED;
                rbRightRotate(tree, node->parent->parent);
            }
        }
        else // parent is a right child
        {
            Node* y = node->parent->parent->left;
            if (y->color == RED)
            {
                node->parent->color = BLACK;
                y->color = BLACK;
                node->parent->parent->color = RED;
                node = node->parent->parent;
            }
            else if (node == node->parent->left)
            {
                node = node->parent;
                rbRightRotate(tree, node);
            }
            else
            {
                node->parent->color = BLACK;
                node->parent->parent->color = RED;
                rbLeftRotate(tree, node->parent->parent);
            }
        }
    }
    tree->root->color = BLACK;
}

char* getStringKey(Node* node, Key key)
{
    if (node == NIL)
        return NULL;
    char* stringKey;
    switch (key)
    {
    case NUMBER:
        stringKey = node->food->number; break;
    case NAME:
        stringKey = node->food->name; break;
    case UPC:
        stringKey = node->food->upc; break;
    case MANUFACTURER:
        stringKey = node->food->manufacturer; break;
    case SERVING_UNITS:
        stringKey = node->food->servingUnits; break;
    case HOUSEHOLD_SERVING_UNITS:
        stringKey = node->food->householdServingUnits; break;
    default:
        return NULL;
    }
    // skip article at start of key
    if (strncasecmp(stringKey, "a ", 2) == 0)
        return stringKey + 2;
    else if (strncasecmp(stringKey, "an ", 3) == 0)
        return stringKey + 3;
    else if (strncasecmp(stringKey, "the ", 4) == 0)
        return stringKey + 4;
    return stringKey;
}

// compare (node a < node b) according to key
bool rbCompare(Node* a, Node* b, Key key)
{
    switch(key)
    {
    case CALORIES:
        return a->food->calories < b->food->calories;
    case CARBOHYDRATES:
        return a->food->carbohydrates < b->food->carbohydrates;
    case FAT:
        return a->food->fat < b->food->fat;
    case PROTEIN:
        return a->food->protein < b->food->protein;
    case SERVING_SIZE:
        return a->food->servingSize < b->food->servingSize;
    case HOUSEHOLD_SERVING_SIZE:
        return a->food->householdServingSize < b->food->householdServingSize;
    case NUMBER:
    case NAME:
    case UPC:
    case MANUFACTURER:
    case SERVING_UNITS:
    case HOUSEHOLD_SERVING_UNITS:
        return strcasecmp(getStringKey(a, key), getStringKey(b, key)) < 0;
    }
}

// page 315
void rbInsert(Tree* tree, Node* node)
{
    Node* y = NIL;
    Node* x = tree->root;

    while (x != NIL)
    {
        y = x;
        if (rbCompare(node, x, tree->key))
            x = x->left;
        else
            x = x->right;
    }
    node->parent = y;
    if (y == NIL)
        tree->root = node;
    else if (rbCompare(node, y, tree->key))
        y->left = node;
    else
        y->right = node;
    node->left = NIL;
    node->right = NIL;
    node->color = RED;
    rbInsertFixup(tree, node);
}

// page 323
void rbTransplant(Tree* tree, Node* oldNode, Node* newNode)
{
    if (oldNode->parent == NIL)
        tree->root = newNode;
    else if (oldNode == oldNode->parent->left)
        oldNode->parent->left = newNode;
    else
        oldNode->parent->right = newNode;
    newNode->parent = oldNode->parent;
}

Node* rbMinimum(Node* node)
{
    while (node->left != NIL)
        node = node->left;
    return node;
}

// page 326
void rbDeleteFixup(Tree* tree, Node* node)
{
    while (node != tree->root && node->color == BLACK)
    {
        // if node is a left child
        if (node == node->parent->left)
        {
            Node* w = node->parent->right;
            if (w->color == RED)
            {
                w->color = BLACK;
                node->parent->color = RED;
                rbLeftRotate(tree, node->parent);
                w = node->parent->right;
            }
            if (w->left->color == BLACK && w->right->color == BLACK)
            {
                w->color = RED;
                node = node->parent;
            }
            else if (w->right->color == BLACK)
            {
                w->left->color = BLACK;
                w->color = RED;
                rbRightRotate(tree, w);
                w = node->parent->right;
            }
            else
            {
                w->color = node->parent->color;
                node->parent->color = BLACK;
                w->right->color = BLACK;
                rbLeftRotate(tree, node->parent);
                node = tree->root;
            }
        }
        else // node is a right child
        {
            Node* w = node->parent->left;
            if (w->color == RED)
            {
                w->color = BLACK;
                node->parent->color = RED;
                rbRightRotate(tree, node->parent);
                w = node->parent->left;
            }
            if (w->left->color == BLACK && w->right->color == BLACK)
            {
                w->color = RED;
                node = node->parent;
            }
            else if (w->left->color == BLACK)
            {
                w->right->color = BLACK;
                w->color = RED;
                rbLeftRotate(tree, w);
                w = node->parent->left;
            }
            else
            {
                w->color = node->parent->color;
                node->parent->color = BLACK;
                w->left->color = BLACK;
                rbRightRotate(tree, node->parent);
                node = tree->root;
            }
        }
    }
    node->color = BLACK;
}

// page 324
void rbDelete(Tree* tree, Node* node)
{
    Node* y = node;
    Color oldColor = y->color;
    Node* x;
    if (node->left == NIL)
    {
        x = node->right;
        rbTransplant(tree, node, node->right);
    }
    else if (node->right == NIL)
    {
        x = node->left;
        rbTransplant(tree, node, node->left);
    }
    else
    {
        y = rbMinimum(node->right);
        oldColor = y->color;
        x = y->right;
        if (y->parent == node)
            x->parent = y;
        else
        {
            rbTransplant(tree, y, y->right);
            y->right = node->right;
            y->right->parent = y;
        }
        rbTransplant(tree, node, y);
        y->left = node->left;
        y->left->parent = y;
        y->color = node->color;
    }
    if (oldColor == BLACK)
        rbDeleteFixup(tree, x);
    deleteFood(node->food);
    free(node);
}

double getDoubleKey(Node* node, Key key)
{
    if (node == NIL)
        return -1;
    switch (key)
    {
    case CALORIES:
        return node->food->calories;
    case CARBOHYDRATES:
        return node->food->carbohydrates;
    case FAT:
        return node->food->fat;
    case PROTEIN:
        return node->food->protein;
    case SERVING_SIZE:
        return node->food->servingSize;
    case HOUSEHOLD_SERVING_SIZE:
        return node->food->householdServingSize;
    default:
        return -1;
    }
}

Node* rbGetSuccessor(Node *node)
{
    // if the node has a right subtree, find its leftmost node
    if (node->right != NIL)
        return rbMinimum(node->right);

    // else, look up the tree for a left child and return its parent
    Node* parent = node->parent;
    while (parent != NIL && node == parent->right)
    {
        node = parent;
        parent = parent->parent;
    }
    return parent;
}

Node** rbGetResults(Node* node, int results)
{
    Node** nodes = malloc(sizeof(Node*) * results);
    for (int i = 0; i < results; i++)
    {
        if (node != NIL)
        {
            nodes[i] = node;
            node = rbGetSuccessor(node);
        }
        else
            nodes[i] = NIL;
    }
    return nodes;
}

Node** rbSearchStringRecursive(Node* node, char* query, Key key, int results)
{
    if (node == NIL) // query not found
        return NULL;
    if (strcasecmp(query, getStringKey(node, key)) == 0)
        return rbGetResults(node, results);
    Node** recursiveResult;
    if (strcasecmp(query, getStringKey(node, key)) < 0)
        recursiveResult = rbSearchStringRecursive(node->left, query, key, results);
    else
        recursiveResult = rbSearchStringRecursive(node->right, query, key, results);
    // if query not found, return second best
    if (recursiveResult == NULL)
    {
        // favor nodes greater than query
        // e.g. this prevents the first result from being last item starting with "a" for a search starting with "b"
        if (strcasecmp(query, getStringKey(node, key)) < 0)
            return rbGetResults(node, results);
        return NULL; // go back one more node
    }
    return recursiveResult;
}

Node** rbSearchString(Tree* tree, char* query, int results)
{
    // if tree is not keyed with a string, return NULL
    if (getStringKey(tree->root, tree->key) == NULL)
        return NULL;
    // skip article at start of query
    if (strncasecmp(query, "a ", 2) == 0)
        query += 2;
    else if (strncasecmp(query, "an ", 3) == 0)
        query += 3;
    else if (strncasecmp(query, "the ", 4) == 0)
        query += 4;
    return rbSearchStringRecursive(tree->root, query, tree->key, results);
}

Node** rbSearchDoubleRecursive(Node* node, double query, Key key, int results)
{
    if (node == NIL) // query not found
        return NULL;
    if (query == getDoubleKey(node, key))
        return rbGetResults(node, results);
    Node** recursiveResult;
    if (query < getDoubleKey(node, key))
        recursiveResult = rbSearchDoubleRecursive(node->left, query, key, results);
    else
        recursiveResult = rbSearchDoubleRecursive(node->right, query, key, results);
    // if query not found, return second best
    if (recursiveResult == NULL)
    {
        // favor nodes greater than query; e.g. see rbSearchStringRecursive
        if (query < getDoubleKey(node, key))
            return rbGetResults(node, results);
        return NULL; // go back one more node
    }
    return recursiveResult;
}

Node** rbSearchDouble(Tree* tree, double query, int results)
{
    // if tree is not keyed with a long, return NULL
    if (getDoubleKey(tree->root, tree->key) == -1)
        return NULL;
    return rbSearchDoubleRecursive(tree->root, query, tree->key, results);
}

#endif //FOOD_TRACKER_RBTREE_H
