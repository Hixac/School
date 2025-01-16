#include "./avl.h"

// Функция для вычисления максимума из двух чисел
int avl_max(int a, int b) {
    return (a > b) ? a : b;
}

// Функция для получения высоты узла
int avl_height(Avl_Node *node) {
    if (node == NULL)
        return 0;
    return node->height;
}

// Функция для создания нового узла
Avl_Node* avl_new_node(int data) {
    Avl_Node *node = (Avl_Node *)malloc(sizeof(Avl_Node));
    node->data = data;
    node->left = NULL;
    node->right = NULL;
    node->height = 1; // Начальная высота нового узла
    return node;
}

// Правый поворот вокруг узла y
Avl_Node* avl_right_rotate(Avl_Node *y) {
    Avl_Node *x = y->left;
    Avl_Node *T2 = x->right;

    // Выполняем поворот
    x->right = y;
    y->left = T2;

    // Обновляем высоты
    y->height = avl_max(avl_height(y->left), avl_height(y->right)) + 1;
    x->height = avl_max(avl_height(x->left), avl_height(x->right)) + 1;

    return x; // Новый корень поддерева
}

// Левый поворот вокруг узла x
Avl_Node* avl_left_rotate(Avl_Node *x) {
    Avl_Node *y = x->right;
    Avl_Node *T2 = y->left;

    // Выполняем поворот
    y->left = x;
    x->right = T2;

    // Обновляем высоты
    x->height = avl_max(avl_height(x->left), avl_height(x->right)) + 1;
    y->height = avl_max(avl_height(y->left), avl_height(y->right)) + 1;

    return y; // Новый корень поддерева
}

// Получение баланса узла
int avl_get_balance(Avl_Node *node) {
    if (node == NULL)
        return 0;
    return avl_height(node->left) - avl_height(node->right);
}

// Вставка узла в AVL дерево
Avl_Node *avl_insert(Avl_Node *node, int data) {
    // Обычная вставка как в бинарное дерево поиска
    if (node == NULL)
        return avl_new_node(data);

    if (data < node->data)
        node->left = avl_insert(node->left, data);
    else if (data > node->data)
        node->right = avl_insert(node->right, data);
    else
        return node; // Дубликаты не допускаются

    // Обновляем высоту текущего узла
    node->height = 1 + avl_max(avl_height(node->left), avl_height(node->right));

    // Получаем баланс узла
    int balance = avl_get_balance(node);

    // Балансировка дерева
    if (balance > 1 && data < node->left->data) // Лево-левый случай
        return avl_right_rotate(node);

    if (balance < -1 && data > node->right->data) // Право-правый случай
        return avl_left_rotate(node);

    if (balance > 1 && data > node->left->data) { // Лево-правый случай
        node->left = avl_left_rotate(node->left);
        return avl_right_rotate(node);
    }

    if (balance < -1 && data < node->right->data) { // Право-левый случай
        node->right = avl_right_rotate(node->right);
        return avl_left_rotate(node);
    }

    return node; // Возвращаем неизмененный узел
}

// Поиск узла с минимальным значением в дереве
Avl_Node *min_value_node(Avl_Node *node) {
    Avl_Node *current = node;
    while (current->left != NULL)
        current = current->left;
    return current;
}

// Удаление узла из AVL дерева
Avl_Node *avl_delete_node(Avl_Node *root, int data) {
    if (root == NULL)
        return root;

    // Рекурсивное удаление
    if (data < root->data)
        root->left = avl_delete_node(root->left, data);
    else if (data > root->data)
        root->right = avl_delete_node(root->right, data);
    else {
        // Узел с одним или без потомков
        if ((root->left == NULL) || (root->right == NULL)) {
            Avl_Node *temp = root->left ? root->left : root->right;

            if (temp == NULL) { // Нет потомков
                temp = root;
                root = NULL;
            } else // Один потомок
                *root = *temp;
            free(temp);
        } else {
            // Узел с двумя потомками
            Avl_Node *temp = min_value_node(root->right);
            root->data = temp->data;
            root->right = avl_delete_node(root->right, temp->data);
        }
    }

    if (root == NULL)
        return root;

    // Обновляем высоту текущего узла
    root->height = 1 + avl_max(avl_height(root->left), avl_height(root->right));

    // Получаем баланс узла
    int balance = avl_get_balance(root);

    // Балансировка дерева
    if (balance > 1 && avl_get_balance(root->left) >= 0) // Лево-левый случай
        return avl_right_rotate(root);

    if (balance > 1 && avl_get_balance(root->left) < 0) { // Лево-правый случай
        root->left = avl_left_rotate(root->left);
        return avl_right_rotate(root);
    }

    if (balance < -1 && avl_get_balance(root->right) <= 0) // Право-правый случай
        return avl_left_rotate(root);

    if (balance < -1 && avl_get_balance(root->right) > 0) { // Право-левый случай
        root->right = avl_right_rotate(root->right);
        return avl_left_rotate(root);
    }

    return root;
}

// Поиск узла в дереве
Avl_Node *avl_search(Avl_Node *root, int data) {
    if (root == NULL || root->data == data)
        return root;

    if (root->data < data)
        return avl_search(root->right, data);

    return avl_search(root->left, data);
}

void avl_print_tree(Avl_Node *root, int ind) {
    if (root != NULL) {
        char* indent_str = (char*)malloc(sizeof(char) * ind + 1);
        for (unsigned int i = 0; i < ind; ++i) {
            indent_str[i] = ' ';
        }
        indent_str[ind] = '\0';
        
        avl_print_tree(root->left, ind + 4);
        printf("%s%d\n", indent_str, root->data);
        avl_print_tree(root->right, ind + 4);

        free(indent_str);
    }
}
