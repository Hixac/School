#ifndef AVL_HEADER
#define AVL_HEADER

#include <stdio.h>
#include <stdlib.h>

// Структура узла AVL дерева
typedef struct Avl_Node {
    int data;               // Данные узла
    struct Avl_Node *left;      // Левый потомок
    struct Avl_Node *right;     // Правый потомок
    int height;             // Высота узла
} Avl_Node;

int avl_max(int a, int b);
int avl_height(Avl_Node *node);

Avl_Node *avl_new_node(int data);
Avl_Node *avl_right_rotate(Avl_Node *y);
Avl_Node *avl_left_rotate(Avl_Node *x);

int avl_get_balance(Avl_Node *node);
Avl_Node *avl_insert(Avl_Node *node, int data);
Avl_Node *avl_min_value_node(Avl_Node *node);
Avl_Node *avl_delete_node(Avl_Node *root, int data);
Avl_Node *avl_search(Avl_Node *root, int data);

void avl_print_tree(Avl_Node *root, int ind);

#endif
