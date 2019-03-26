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

// compare (node a < node b) according to key
bool rbCompare(Node* a, Node* b, Key key)
{
    switch(key)
    {
    case NUMBER:
        return a->food->number < b->food->number;
    case NAME:
        return strcasecmp(a->food->name, b->food->name) < 0;
    case MANUFACTURER:
        return strcasecmp(a->food->manufacturer, b->food->manufacturer) < 0;
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
    case SERVING_UNITS:
        return strcasecmp(a->food->servingUnits, b->food->servingUnits) < 0;
    case HOUSEHOLD_SERVING_SIZE:
        return a->food->householdServingSize < b->food->householdServingSize;
    case HOUSEHOLD_SERVING_UNITS:
        return strcasecmp(a->food->householdServingUnits, b->food->householdServingUnits) < 0;
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


long getLongKey(Node* node, Key key)
{
    if (key == NUMBER)
        return node->food->number;
    return -1;
}

char* getStringKey(Node* node, Key key)
{
    switch (key)
    {
    case NAME:
        return node->food->name;
    case MANUFACTURER:
        return node->food->manufacturer;
    case SERVING_UNITS:
        return node->food->servingUnits;
    case HOUSEHOLD_SERVING_UNITS:
        return node->food->householdServingUnits;
    default:
        return NULL;
    }
}

double getDoubleKey(Node* node, Key key)
{
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
    {
        node = node->right;
        while (node->left != NIL)
            node = node->left;
        return node;
    }

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

Node** rbSearchLongRecursive(Node* node, long query, Key key, int results)
{
    if (node == NIL) // query not found
        return NULL;
    if (getLongKey(node, key) == query)
        return rbGetResults(node, results);
    Node** recursiveResult;
    if (query < getLongKey(node, key))
        recursiveResult = rbSearchLongRecursive(node->left, query, key, results);
    else
        recursiveResult = rbSearchLongRecursive(node->right, query, key, results);
    // if query not found, return second best
    if (recursiveResult == NULL)
        return rbGetResults(node, results);
    return recursiveResult;
}

Node** rbSearchLong(Tree* tree, long query, int results)
{
    // if tree is not keyed with a long, return NULL
    if (getLongKey(tree->root, tree->key) == -1)
        return NULL;
    long minDiff = INT_MAX;
    Node* minDiffNode = NIL;
    return rbSearchLongRecursive(tree->root, query, tree->key, results, &minDiff, &minDiffNode);
}

Node** rbSearchStringRecursive(Node* node, char* query, Key key, int results)
{
    if (node == NIL) // query not found
        return NULL;
    if (strcasecmp(getStringKey(node, key), query) == 0)
        return rbGetResults(node, results);
    Node** recursiveResult;
    if (strcasecmp(query, getStringKey(node, key)) < 0)
        recursiveResult = rbSearchStringRecursive(node->left, query, key, results);
    else
        recursiveResult = rbSearchStringRecursive(node->right, query, key, results);
    // if query not found, return second best
    if (recursiveResult == NULL)
        return rbGetResults(node, results);
    return recursiveResult;
}

Node** rbSearchString(Tree* tree, char* query, int results)
{
    // if tree is not keyed with a string, return NULL
    if (getStringKey(tree->root, tree->key) == NULL)
        return NULL;
    return rbSearchStringRecursive(tree->root, query, tree->key, results);
}

Node** rbSearchDoubleRecursive(Node* node, double query, Key key, int results)
{
    if (node == NIL) // query not found
        return NULL;
    if (getLongKey(node, key) == query)
        return rbGetResults(node, results);
    Node** recursiveResult;
    if (query < getLongKey(node, key))
        recursiveResult = rbSearchDoubleRecursive(node->left, query, key, results);
    else
        recursiveResult = rbSearchDoubleRecursive(node->right, query, key, results);
    // if query not found, return second best
    if (recursiveResult == NULL)
        return rbGetResults(node, results);
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
