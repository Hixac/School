#include "./brt.h"

// Создание нового узла
Brt_Node* brt_create_node(int data) {
    Brt_Node* node = (Brt_Node*)malloc(sizeof(Brt_Node));
    node->data = data;
    node->color = BRT_RED; // Новый узел всегда красный
    node->left = node->right = node->parent = NULL;
    return node;
}

// Возвращает указатель на дядю (брата родителя)
Brt_Node* brt_uncle(Brt_Node* node) {
    if (node->parent == NULL || node->parent->parent == NULL)
        return NULL; // Если нет родителя или дедушки, дядя отсутствует

    if (node->parent == node->parent->parent->left)
        return node->parent->parent->right; // Дядя справа
    else
        return node->parent->parent->left;  // Дядя слева
}

// Проверяет, является ли узел левым потомком своего родителя
int brt_is_on_left(Brt_Node* node) {
    return node == node->parent->left;
}

// Возвращает указатель на брата (сиблинга)
Brt_Node* brt_sibling(Brt_Node* node) {
    if (node->parent == NULL)
        return NULL; // Если нет родителя, брата нет

    if (brt_is_on_left(node))
        return node->parent->right; // Брат справа
    else
        return node->parent->left;  // Брат слева
}

// Перемещает узел вниз и заменяет его новым родителем
void brt_move_down(Brt_Node* node, Brt_Node* new_parent) {
    if (node->parent != NULL) {
        if (brt_is_on_left(node))
            node->parent->left = new_parent;
        else
            node->parent->right = new_parent;
    }
    new_parent->parent = node->parent;
    node->parent = new_parent;
}

// Проверяет, есть ли у узла красный потомок
int brt_has_red_child(Brt_Node* node) {
    return (node->left != NULL && node->left->color == BRT_RED) ||
           (node->right != NULL && node->right->color == BRT_RED);
}

// Левый поворот
void brt_left_rotate(Brt_Node** root, Brt_Node* x) {
    Brt_Node* new_parent = x->right;

    if (x == *root)
        *root = new_parent;

    brt_move_down(x, new_parent);

    x->right = new_parent->left;
    if (new_parent->left != NULL)
        new_parent->left->parent = x;

    new_parent->left = x;
}

// Правый поворот
void brt_right_rotate(Brt_Node** root, Brt_Node* x) {
    Brt_Node* new_parent = x->left;

    if (x == *root)
        *root = new_parent;

    brt_move_down(x, new_parent);

    x->left = new_parent->right;
    if (new_parent->right != NULL)
        new_parent->right->parent = x;

    new_parent->right = x;
}

// Обмен цветами между двумя узлами
void brt_swap_colors(Brt_Node* x1, Brt_Node* x2) {
    BRT_COLOR temp = x1->color;
    x1->color = x2->color;
    x2->color = temp;
}

// Обмен значениями между двумя узлами
void brt_swap_dataues(Brt_Node* u, Brt_Node* v) {
    int temp = u->data;
    u->data = v->data;
    v->data = temp;
}

// Восстановление свойств красно-черного дерева после вставки
void brt_fix_red(Brt_Node** root, Brt_Node* x) {
    if (x == *root) {
        x->color = BRT_BLACK;
        return;
    }

    Brt_Node* parent = x->parent;
    Brt_Node* grandparent = parent->parent;
    Brt_Node* uncle_node = brt_uncle(x);

    if (parent->color != BRT_BLACK) {
        if (uncle_node != NULL && uncle_node->color == BRT_RED) {
            parent->color = BRT_BLACK;
            uncle_node->color = BRT_BLACK;
            grandparent->color = BRT_RED;
            brt_fix_red(root, grandparent);
        } else {
            if (brt_is_on_left(parent)) {
                if (brt_is_on_left(x)) {
                    brt_swap_colors(parent, grandparent);
                } else {
                    brt_left_rotate(root, parent);
                    brt_swap_colors(x, grandparent);
                }
                brt_right_rotate(root, grandparent);
            } else {
                if (brt_is_on_left(x)) {
                    brt_right_rotate(root, parent);
                    brt_swap_colors(x, grandparent);
                } else {
                    brt_swap_colors(parent, grandparent);
                }
                brt_left_rotate(root, grandparent);
            }
        }
    }
}

// Поиск узла с минимальным значением в поддереве
Brt_Node* brt_successor(Brt_Node* x) {
    Brt_Node* temp = x;
    while (temp->left != NULL)
        temp = temp->left;
    return temp;
}

// Поиск узла, который заменит удаляемый узел
Brt_Node* brt_replace(Brt_Node* x) {
    if (x->left != NULL && x->right != NULL)
        return brt_successor(x->right);

    if (x->left == NULL && x->right == NULL)
        return NULL;

    if (x->left != NULL)
        return x->left;
    else
        return x->right;
}

// Удаление узла
void brt_delete_node(Brt_Node** root, Brt_Node* v) {
    Brt_Node* u = brt_replace(v);
    int uvBlack = ((u == NULL || u->color == BRT_BLACK) && (v->color == BRT_BLACK));
    Brt_Node* parent = v->parent;

    if (u == NULL) {
        if (v == *root) {
            *root = NULL;
        } else {
            if (uvBlack) {
                // Исправление двойного черного
            } else {
                if (brt_sibling(v) != NULL)
                    brt_sibling(v)->color = BRT_RED;
            }

            if (brt_is_on_left(v)) {
                parent->left = NULL;
            } else {
                parent->right = NULL;
            }
        }
        free(v);
        return;
    }

    if (v->left == NULL || v->right == NULL) {
        if (v == *root) {
            v->data = u->data;
            v->left = v->right = NULL;
            free(u);
        } else {
            if (brt_is_on_left(v)) {
                parent->left = u;
            } else {
                parent->right = u;
            }
            free(v);
            u->parent = parent;
            if (uvBlack) {
                // Исправление двойного черного
            } else {
                u->color = BRT_BLACK;
            }
        }
        return;
    }

    brt_swap_dataues(u, v);
    brt_delete_node(root, u);
}

Brt_Node* brt_search(Brt_Node* root, int data) {
    if (root == NULL || root->data == data)
        return root;

    if (root->data < data)
        return brt_search(root->right, data);

    return brt_search(root->left, data);
}

// Поиск узла по значению
Brt_Node* brt_search_last(Brt_Node* root, int data) {
    Brt_Node* temp = root;
    while (temp != NULL) {
        if (data < temp->data) {
            if (temp->left == NULL)
                break;
            else
                temp = temp->left;
        } else if (data == temp->data) {
            break;
        } else {
            if (temp->right == NULL)
                break;
            else
                temp = temp->right;
        }
    }
    return temp;
}


// Вставка значения в дерево
void brt_insert(Brt_Node** root, int data) {
    Brt_Node* new_node = brt_create_node(data);
    if (*root == NULL) {
        new_node->color = BRT_BLACK;
        *root = new_node;
    } else {
        Brt_Node* temp = brt_search_last(*root, data);

        if (temp->data == data) {
            return; // Значение уже существует
        }

        new_node->parent = temp;

        if (data < temp->data)
            temp->left = new_node;
        else
            temp->right = new_node;

        brt_fix_red(root, new_node);
    }
}

// Удаление значения из дерева
void brt_delete_by_data(Brt_Node** root, int data) {
    if (*root == NULL)
        return;

    Brt_Node* v = brt_search_last(*root, data);

    if (v->data != data) {
        return;
    }

    brt_delete_node(root, v);
}

// Обход дерева в порядке in-order
void brt_inorder(Brt_Node* root) {
    if (root == NULL)
        return;
    brt_inorder(root->left);
    printf("%d ", root->data);
    brt_inorder(root->right);
}

void brt_print_tree(Brt_Node *root, int ind) {
    if (root != NULL) {
        char* indent_str = (char*)malloc(sizeof(char) * ind + 1);
        for (unsigned int i = 0; i < ind; ++i) {
            indent_str[i] = ' ';
        }
        indent_str[ind] = '\0';
        
        brt_print_tree(root->left, ind + 4);
        printf("%s%d\n", indent_str, root->data);
        brt_print_tree(root->right, ind + 4);

        free(indent_str);
    }
}
