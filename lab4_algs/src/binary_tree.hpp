#include <iostream>
#include <vector>
#include <limits>
#include <queue>
#include <fstream>
#include <stdexcept>
#include <numeric>

namespace My {

    // Шаблон структуры узла бинарного дерева
    // Позволяет использовать любой тип данных для значений узлов
    template<typename T>
    struct tree_node {
        T value;
        tree_node* left;
        tree_node* right;

        tree_node(T val) : value(val), left(nullptr), right(nullptr) {}
    };

    // Шаблон класса бинарного дерева поиска
    // Реализует базовые операции для работы с деревом
    template<typename T>
    class binary_tree {
    public:
        binary_tree() : m_root(nullptr) {}

        virtual ~binary_tree() { clear(m_root); }

        // Ввод данных
        std::istream& input(std::istream& in) {
            std::cout << "Введите последовательность целых чисел (завершите ввод символом 'q'):\n";
            int value;
            while (std::cin >> value) {
                add(value);
            }

            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            return in;
        }

        // Вывод данных
        std::ostream& output(std::ostream& out) {
            print_tree(m_root, out);
            return out;
        }

        // Перезагрузка вывода
        friend std::ostream& operator<<(std::ostream& out, binary_tree& tree) {
            return tree.output(out);
        }

        // Перезагрузка ввода
        friend std::istream& operator>>(std::istream& in, binary_tree& tree) {
            return tree.input(in);
        }
        
        // Добавление элемента в дерево
        virtual void add(T value) {
            m_root = add_recursive(m_root, value);
        }

        // Нахождение средней длины элементов самой короткой ветви
        double find_average_shortest_path() {
            if (!m_root) {
                throw std::runtime_error("Дерево пустое.");
            }

            std::vector<T> min_path = find_shortest_path(m_root);
            if (min_path.empty()) {
                throw std::runtime_error("Не удалось найти ветвь.");
            }

            T sum = std::accumulate(min_path.begin(), min_path.end(), T(0));
            return static_cast<double>(sum) / min_path.size();
        }

        // Прямой обход дерева (корень → левое → правое)
        void pre_order_traversal(std::ostream& out) {
            pre_order(m_root, out);
        }

        // Обратный обход дерева (левое → корень → правое)
        void in_order_traversal(std::ostream& out) {
            in_order(m_root, out);
        }

        // Концевой обход дерева (левое → правое → корень)
        void post_order_traversal(std::ostream& out) {
            post_order(m_root, out);
        }

        // Сохранение дерева в файл
        void save_to_file(const std::string& filename) {
            std::ofstream file(filename);
            if (!file.is_open()) {
                throw std::runtime_error("Не удалось открыть файл для записи.");
            }
            file << *this;
            file.close();
        }

        // Загрузка дерева из файла
        void load_from_file(const std::string& filename) {
            std::ifstream file(filename);
            if (!file.is_open()) {
                throw std::runtime_error("Не удалось открыть файл для чтения.");
            }
            clear(m_root);
            file >> *this;
            file.close();
        }

    protected:
        tree_node<T>* m_root;

        // Рекурсивное добавление элемента в дерево
        virtual tree_node<T>* add_recursive(tree_node<T>* node, T value) {
            if (!node) {
                return new tree_node<T>(value);
            }
            if (value < node->value) {
                node->left = add_recursive(node->left, value);
            } else {
                node->right = add_recursive(node->right, value);
            }
            return node;
        }

        void print_tree(tree_node<T>* node, std::ostream& out, int indent = 0) const 
        {
            if (node) {
                print_tree(node->right, out, indent + 4); // Выводим правое поддерево 
                out << std::string(indent, ' ') << node->value << "\n"; // Выводим значение текущего узла с текущим отступом
                print_tree(node->left, out, indent + 4); // Выводим левое поддерево 
            }
        }
        
        // Очистка дерева
        void clear(tree_node<T>* node) {
            if (node) {
                clear(node->left);
                clear(node->right);
                delete node;
            }
        }

        // Прямой обход дерева
        void pre_order(tree_node<T>* node, std::ostream& out) {
            if (node) {
                out << node->value << " ";
                pre_order(node->left, out);
                pre_order(node->right, out);
            }
        }

        // Обратный обход дерева
        void in_order(tree_node<T>* node, std::ostream& out) {
            if (node) {
                in_order(node->left, out);
                out << node->value << " ";
                in_order(node->right, out);
            }
        }

        // Концевой обход дерева
        void post_order(tree_node<T>* node, std::ostream& out) {
            if (node) {
                post_order(node->left, out);
                post_order(node->right, out);
                out << node->value << " ";
            }
        }

        // Поиск самой короткой ветви
        std::vector<T> find_shortest_path(tree_node<T>* node) {
            if (!node) {
                return {};
            }

            std::queue<std::pair<tree_node<T>*, std::vector<T>>> q;
            q.push({node, {node->value}});
            std::vector<T> shortest_path;
            int min_depth = std::numeric_limits<int>::max();

            while (!q.empty()) {
                auto [current, path] = q.front();
                q.pop();

                    // Если листовой узел
                if (!current->left && !current->right) {
                    if (path.size() < min_depth || (path.size() == min_depth && path > shortest_path)) {
                        min_depth = path.size();
                        shortest_path = path;
                    }
                }

                if (current->left) {
                    std::vector<T> left_path = path;
                    left_path.push_back(current->left->value);
                    q.push({current->left, left_path});
                }

                if (current->right) {
                    std::vector<T> right_path = path;
                    right_path.push_back(current->right->value);
                    q.push({current->right, right_path});
                }
            }

            return shortest_path;
        }
    };

    // Производный класс: Сбалансированное дерево
    // Добавляет балансировку при необходимости
    template<typename T>
    class balanced_binary_tree : public binary_tree<T> {
    public:
        void add(T value) override {
            this->m_root = add_and_balance(this->m_root, value);
        }

    private:
        tree_node<T>* add_and_balance(tree_node<T>* node, T value) {
            node = this->add_recursive(node, value);
            return balance(node);
        }

        int height(tree_node<T>* node) {
            if (!node) return 0;
            return 1 + std::max(height(node->left), height(node->right));
        }

        int balance_factor(tree_node<T>* node) {
            if (!node) return 0;
            return height(node->left) - height(node->right);
        }

        tree_node<T>* balance(tree_node<T>* node) {
            if (!node) return nullptr;

            int bf = balance_factor(node);

            if (bf > 1) {
                if (balance_factor(node->left) < 0) {
                    node->left = rotate_left(node->left);
                }
                return rotate_right(node);
            }

            if (bf < -1) {
                if (balance_factor(node->right) > 0) {
                    node->right = rotate_right(node->right);
                }
                return rotate_left(node);
            }

            return node;
        }

        tree_node<T>* rotate_left(tree_node<T>* node) {
            tree_node<T>* new_root = node->right;
            node->right = new_root->left;
            new_root->left = node;
            return new_root;
        }

        tree_node<T>* rotate_right(tree_node<T>* node) {
            tree_node<T>* new_root = node->left;
            node->left = new_root->right;
            new_root->right = node;
            return new_root;
        }
    };

    
}
