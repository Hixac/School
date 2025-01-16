#include "./bst.h"

// Создание нового узла
Bst_Node *bst_new_node(int data) {
    Bst_Node *node = (Bst_Node *)malloc(sizeof(Bst_Node));
    node->data = data;
    node->left = NULL;
    node->right = NULL;
    return node;
}

// Вставка узла в дерево
Bst_Node *bst_insert(Bst_Node *root, int data) {
    if (root == NULL)
        return bst_new_node(data);

    if (data < root->data)
        root->left = bst_insert(root->left, data);
    else if (data > root->data)
        root->right = bst_insert(root->right, data);

    return root;
}

// Поиск узла с минимальным значением
Bst_Node *bst_min_value_node(Bst_Node *node) {
    Bst_Node *current = node;
    while (current->left != NULL)
        current = current->left;
    return current;
}

// Удаление узла из дерева
Bst_Node *bst_delete_node(Bst_Node *root, int data) {
    if (root == NULL)
        return root;

    // Рекурсивное удаление
    if (data < root->data)
        root->left = bst_delete_node(root->left, data);
    else if (data > root->data)
        root->right = bst_delete_node(root->right, data);
    else {
        // Узел с одним или без потомков
        if (root->left == NULL) {
            Bst_Node *temp = root->right;
            free(root);
            return temp;
        } else if (root->right == NULL) {
            Bst_Node *temp = root->left;
            free(root);
            return temp;
        }

        // Узел с двумя потомками
        Bst_Node *temp = bst_min_value_node(root->right);
        root->data = temp->data;
        root->right = bst_delete_node(root->right, temp->data);
    }
    return root;
}

// Поиск узла в дереве
Bst_Node *bst_search(Bst_Node *root, int data) {
    if (root == NULL || root->data == data)
        return root;

    if (root->data < data)
        return bst_search(root->right, data);

    return bst_search(root->left, data);
}

void bst_print_tree(Bst_Node *root, int ind) {
    if (root != NULL) {
        char* indent_str = (char*)malloc(sizeof(char) * ind + 1);
        for (unsigned int i = 0; i < ind; ++i) {
            indent_str[i] = ' ';
        }
        indent_str[ind] = '\0';
        
        bst_print_tree(root->left, ind + 4);
        printf("%s%d\n", indent_str, root->data);
        bst_print_tree(root->right, ind + 4);

        free(indent_str);
    }
}
