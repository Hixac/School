#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>

// Узел бинарного дерева
struct tree_node {
    std::string key;     // Ключ идентификатора
    tree_node* left;     // Левый потомок
    tree_node* right;    // Правый потомок

    // Конструктор узла
    tree_node(const std::string& k) : key(k), left(nullptr), right(nullptr) {}
};

// Хеш-таблица с разрешением коллизий через бинарное дерево
class hash_table {
private:
    std::vector<tree_node*> m_table; // Вектор указателей на корни бинарных деревьев
    size_t m_ids_counter;            // Счётчик идентификаторов
    size_t m_table_size;             // Размер хеш-таблицы
    size_t m_total_collisions;       // Общее количество коллизий
    size_t m_total_comparisons;      // Общее количество сравнений при поиске

    // Хеш-функция (метод деления)
    size_t hash_function(const std::string& identifier) const {
        size_t hash = 0;
        for (char ch : identifier) {
            hash = (hash * 31 + ch) % m_table_size; // Простая хеш-функция с коэффициентом 31
        }
        return hash % m_table_size;
    }

    // Вставка элемента в бинарное дерево
    void insert_to_tree(tree_node*& root, const std::string& identifier) {
        if (!root) {
            root = new tree_node(identifier); // Создание нового узла, если дерево пустое
        } else if (identifier < root->key) {
            insert_to_tree(root->left, identifier); // Вставка в левое поддерево
        } else if (identifier > root->key) {
            insert_to_tree(root->right, identifier); // Вставка в правое поддерево
        }
        // Идентификаторы равны — ничего не делаем (идентификаторы уникальны)
    }

    // Подсчёт узлов в дереве для коллизий
    size_t count_tree_nodes(tree_node* root) const {
        if (!root) return 0;
        return 1 + count_tree_nodes(root->left) + count_tree_nodes(root->right);
    }

    // Поиск элемента в бинарном дереве с подсчётом сравнений
    bool search_in_tree(tree_node* root, const std::string& identifier, size_t& comparisons) const {
        if (!root) return false;                        // Элемент не найден
        comparisons++;                                  // Сравнение текущего узла
        if (identifier == root->key) return true;       // Элемент найден
        if (identifier < root->key) {
            return search_in_tree(root->left, identifier, comparisons); // Поиск в левом поддереве
        }
        return search_in_tree(root->right, identifier, comparisons);    // Поиск в правом поддереве
    }

    // Удаление бинарного дерева (рекурсивно)
    void delete_tree(tree_node* root) {
        if (!root) return;
        delete_tree(root->left);  // Удаление левого поддерева
        delete_tree(root->right); // Удаление правого поддерева
        delete root;              // Удаление текущего узла
    }
    
public:
    hash_table(size_t size) 
        : m_table(size, nullptr),
          m_table_size(size),
          m_ids_counter(0),
          m_total_collisions(0),
          m_total_comparisons(0) {}

    ~hash_table() {
        for (auto& root : m_table) {
            delete_tree(root); // Удаляем каждое дерево в хеш-таблице
        }
    }

    // Вставка идентификатора в хеш-таблицу
    void insert(const std::string& identifier) {
        size_t index = hash_function(identifier); // Получаем индекс через хеш-функцию
        if (m_table[index]) {
            m_total_collisions++; // Коллизия, если ячейка уже содержит дерево
        }
        insert_to_tree(m_table[index], identifier); // Вставляем в бинарное дерево по индексу
    }

    // Поиск идентификатора в хеш-таблице
    bool search(const std::string& identifier) {
        size_t index = hash_function(identifier); // Получаем индекс через хеш-функцию
        size_t comparisons = 0; // Счётчик сравнений
        bool found = search_in_tree(m_table[index], identifier, comparisons);
        m_total_comparisons += comparisons;
        return found;
    }

    // Вывод статистики
    void print_statistics() const {
        double avg_collisions = static_cast<double>(m_total_collisions) / m_ids_counter;

        std::cout << "\nСтатистика:\n";
        std::cout << "Общее число коллизий: " << m_total_collisions << "\n";
        std::cout << "Среднее число коллизий: " << avg_collisions << "\n";
        std::cout << "Число сравнений при поиске: " << m_total_comparisons << "\n";
    }

    // Рекурсивный вывод содержимого дерева
    void print_tree(tree_node* root, int level) const {
        if (!root) return;
        print_tree(root->right, level + 1);
        std::cout << std::setw(level + 4) << "" << root->key << "\n";
        print_tree(root->left, level + 1);
    }
    
    // Вывод таблицы
    void print_table() const {
        std::cout << "Хеш-таблица:\n";
        for (size_t i = 0; i < m_table_size; ++i) {
            std::cout << "Индекс " << i << ":\n";
            print_tree(m_table[i], 0);
            std::cout << "\n";
        }
    }

    // Чтение идентификаторов из файла
    void read_identifiers(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Ошибка: не удалось открыть файл " << filename << std::endl;
        }
        std::string identifier;
        while (file >> identifier) {
            if (identifier.length() <= 32) {
                insert(identifier); // Добавляем идентификатор в список
            } else {
                std::cerr << "Пропущен идентификатор (слишком длинный): " << identifier << std::endl;
            }
        }
    }
};
