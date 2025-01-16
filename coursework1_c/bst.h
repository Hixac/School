#ifndef BST_HEADER
#define BST_HEADER

#include <stdio.h>
#include <stdlib.h>

// Структура узла дерева
typedef struct Bst_Node {
    int data;               // Данные узла
    struct Bst_Node *left;      // Левый потомок
    struct Bst_Node *right;     // Правый потомок
} Bst_Node;

Bst_Node *bst_new_node(int data);
Bst_Node *bst_min_value_node(Bst_Node *node);

Bst_Node *bst_delete_node(Bst_Node *root, int data);
Bst_Node *bst_insert(Bst_Node *root, int data);
Bst_Node *bst_search(Bst_Node *root, int data);

void bst_print_tree(Bst_Node *root, int ind);

#endif
