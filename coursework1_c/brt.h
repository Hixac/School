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

// Создание нового узла
Brt_Node* brt_create_node(int data);

// Вставка значения в дерево
void brt_insert(Brt_Node** root, int data);

// Удаление значения из дерева
void brt_delete_by_data(Brt_Node** root, int data);

// Поиск узла по значению
Brt_Node* brt_search(Brt_Node* root, int data);

// Обход дерева в порядке in-order
void brt_print_tree(Brt_Node *root, int ind);

// Обход дерева в порядке level-order
void brt_level_order(Brt_Node* root);

#endif // BRTREE_H
