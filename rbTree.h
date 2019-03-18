//
// Created by matthew on 3/17/19.
//

#ifndef FOOD_TRACKER_RBTREE_H
#define FOOD_TRACKER_RBTREE_H

#include <stdlib.h>
#include "entries.h"

Entry* NIL;

Tree* rbNewTree()
{
    NIL = malloc(sizeof(Entry));
    NIL->color = BLACK;
    Tree* tree = malloc(sizeof(Tree));
    tree->root = NIL;
    return tree;
}

void rbDeleteTreeRecursive(Entry* node)
{
    if (node != NIL)
    {
        rbDeleteTreeRecursive(node->left);
        deleteEntry(node);
        rbDeleteTreeRecursive(node->right);
    }
}

void rbDeleteTree(Tree* tree)
{
    rbDeleteTreeRecursive(tree->root);
    deleteEntry(NIL);
}

// page 313
void rbLeftRotate(Tree* tree, Entry* node)
{
    Entry* y = node->right;
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
void rbRightRotate(Tree* tree, Entry* node)
{
    Entry* y = node->left;
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
void rbInsertFixup(Tree* tree, Entry* node)
{
    while (node->parent->color == RED)
    {
        // if parent is a left child
        if (node->parent == node->parent->parent->left)
        {
            Entry* y = node->parent->parent->right;
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
                rbRightRotate(tree, node);
            }
        }
        else // parent is a right child
        {
            Entry* y = node->parent->parent->left;
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
                rbLeftRotate(tree, node);
            }
        }
    }
    tree->root->color = BLACK;
}

// page 315
void rbInsert(Tree* tree, Entry* node)
{
    Entry* y = NIL;
    Entry* x = tree->root;

    while (x != NIL)
    {
        y = x;
        if (node->number < x->number)
            x = x->left;
        else
            x = x->right;
    }
    node->parent = y;
    if (y == NIL)
        tree->root = node;
    else if (node->number < y->number)
        y->left = node;
    else
        y->right = node;
    node->left = NIL;
    node->right = NIL;
    node->color = RED;
    rbInsertFixup(tree, node);
}

// page 323
void rbTransplant(Tree* tree, Entry* oldNode, Entry* newNode)
{
    if (oldNode->parent == NIL)
        tree->root = newNode;
    else if (oldNode == oldNode->parent->left)
        oldNode->parent->left = newNode;
    else
        oldNode->parent->right = newNode;
    newNode->parent = oldNode->parent;
}

Entry* rbMinimum(Entry* node)
{

}

// page 326
void rbDeleteFixup(Tree* tree, Entry* node)
{
    while (node != tree->root && node->color == BLACK)
    {
        // if node is a left child
        if (node == node->parent->left)
        {
            Entry* w = node->parent->right;
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
            Entry* w = node->parent->left;
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
void rbDelete(Tree* tree, Entry* node)
{
    Entry* y = node;
    Color oldColor = y->color;
    Entry* x;
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
    deleteEntry(node);
}

#endif //FOOD_TRACKER_RBTREE_H
