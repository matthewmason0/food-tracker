//
// Created by matthew on 3/17/19.
//

#ifndef FOOD_TRACKER_RBTREE_H
#define FOOD_TRACKER_RBTREE_H

#include <stdlib.h>
#include <stdbool.h>
#include "entries.h"

Node* NIL;

Tree* rbNewTree()
{
    NIL = malloc(sizeof(Node));
    NIL->color = BLACK;
    Tree* tree = malloc(sizeof(Tree));
    tree->root = NIL;
    return tree;
}

void rbDeleteTreeRecursive(Node* node)
{
    if (node != NIL)
    {
        rbDeleteTreeRecursive(node->left);
        deleteFood(node->food);
        free(node);
        rbDeleteTreeRecursive(node->right);
    }
}

void rbDeleteTree(Tree* tree)
{
    rbDeleteTreeRecursive(tree->root);
    free(NIL);
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
        y->left->parent = node;
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
bool rbCompare(Node *a, Node *b, Key key)
{
    switch(key)
    {
    case NUMBER:
        return a->food->number < b->food->number;
    case NAME:
        return strcmp(a->food->name, b->food->name) < 0;
    case MANUFACTURER:
        return strcmp(a->food->manufacturer, b->food->manufacturer) < 0;
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
        return strcmp(a->food->servingUnits, b->food->servingUnits) < 0;
    case HOUSEHOLD_SERVING_SIZE:
        return a->food->householdServingSize < b->food->householdServingSize;
    case HOUSEHOLD_SERVING_UNITS:
        return strcmp(a->food->householdServingUnits, b->food->householdServingUnits) < 0;
    }
}

// compare (

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
                rbLeftRotate(tree, node->parent);
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
                rbRightRotate(tree, w);
                w = node->parent->left;
            }
            else
            {
                w->color = node->parent->color;
                node->parent->color = BLACK;
                w->left->color = BLACK;
                rbLeftRotate(tree, node->parent);
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

Node* rbSearchLongRecursive(Node* node, long query, Key key)
{
    if (getLongKey(node, key) == query)
        return node;
    if (node == NIL)
        return NULL;
    if (getLongKey(node, key) < query)
        return rbSearchLongRecursive(node->left, query, key);
    return rbSearchLongRecursive(node->right, query, key);
}

Node* rbSearchLong(Tree* tree, long query)
{
    // if tree is not keyed with a long, return NULL
    if (getLongKey(tree->root, tree->key) == -1)
        return NULL;
    return rbSearchLongRecursive(tree->root, query, tree->key);
}

Node* rbSearchStringRecursive(Node* node, char* query, Key key)
{
    if (strcmp(getStringKey(node, key), query) == 0)
        return node;
    if (node == NIL)
        return NULL;
    if (strcmp(query, getStringKey(node, key)) < 0)
        return rbSearchStringRecursive(node->left, query, key);
    return rbSearchStringRecursive(node->right, query, key);
}

Node* rbSearchString(Tree* tree, char* query)
{
    // if tree is not keyed with a string, return NULL
    if (getStringKey(tree->root, tree->key) == NULL)
        return NULL;
    return rbSearchStringRecursive(tree->root, query, tree->key);
}

Node* rbSearchDoubleRecursive(Node* node, double query, Key key)
{
    if (getDoubleKey(node, key) == query)
        return node;
    if (node == NIL)
        return NULL;
    if (getDoubleKey(node, key) < query)
        return rbSearchDoubleRecursive(node->left, query, key);
    return rbSearchDoubleRecursive(node->right, query, key);
}

Node* rbSearchDouble(Tree* tree, double query)
{
    // if tree is not keyed with a long, return NULL
    if (getDoubleKey(tree->root, tree->key) == -1)
        return NULL;
    return rbSearchDoubleRecursive(tree->root, query, tree->key);
}

#endif //FOOD_TRACKER_RBTREE_H
