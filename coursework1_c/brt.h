#ifndef BRTREE_H
#define BRTREE_H

#include <stdio.h>
#include <stdlib.h>

// Перечисление для цвета узла
typedef enum { BRT_RED, BRT_BLACK } BRT_COLOR;

// Структура узла красно-черного дерева
typedef struct Brt_Node {
    int data;                // Значение узла
    BRT_COLOR color;        // Цвет узла
    struct Brt_Node *left;  // Левый потомок
    struct Brt_Node *right; // Правый потомок
    struct Brt_Node *parent; // Родительский узел
} Brt_Node;

Brt_Node* brt_create_node(int data);

void brt_insert(Brt_Node** root, int data);
void brt_delete_by_data(Brt_Node** root, int data);
Brt_Node* brt_search(Brt_Node* root, int data);

void brt_print_tree(Brt_Node *root, int ind);

#endif // BRTREE_H
