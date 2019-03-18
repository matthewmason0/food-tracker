//
// Created by matthew on 3/13/19.
//

#include <stdio.h>
#include <stdlib.h>
#include "entries.h"
#include "rbTree.h"

void printEntryNumber(Entry* node)
{
    printf("Number: %ld, Name: %s\n", node->number, node->name);
}

int main()
{
    Tree* rbTree = rbNewTree();

    Entry* test = newEntry(12345678, "Sweet Tea", "me", 100, 0, 0, 2, 20, "fl oz", 1, "glass");
    rbInsert(rbTree, test);

    printEntryNumber(rbTree->root);

    rbDeleteTree(rbTree);

//    Entry* test2 = malloc(sizeof(Entry));
//    test2->number = 2;
//    rbInsert(rbTree, test2);
//
//    printEntryNumber(rbTree->root->left);
//
//    rbDelete(rbTree, test);
//
//    printEntryNumber(rbTree->root);
//
//    rbDelete(rbTree, test2);

    return 0;
}