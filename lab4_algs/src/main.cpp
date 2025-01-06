#include <binary_tree.hpp>

int main() {
    My::balanced_binary_tree<int> tree;

    std::cin >> tree;
    
    try {
        double average = tree.find_average_shortest_path();
        std::cout << "Среднее арифметическое элементов самой короткой ветви: " << average << std::endl;

        std::cout << "Прямой обход: ";
        tree.pre_order_traversal(std::cout);
        std::cout << "\n";

        std::cout << "Обратный обход: ";
        tree.in_order_traversal(std::cout);
        std::cout << "\n";

        std::cout << "Концевой обход: ";
        tree.post_order_traversal(std::cout);
        std::cout << "\n";

        std::string filename = "tree.txt";
        tree.save_to_file(filename);
        std::cout << "Дерево сохранено в файл: " << filename << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }

    return 0;
}
