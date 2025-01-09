#include "hash_table.hpp"

int main() {
    std::string filename = "identifiers.txt"; // Имя файла с идентификаторами
    size_t table_size = 10;             // Размер хеш-таблицы

    // Создание хеш-таблицы
    hash_table table(table_size);
    table.read_identifiers(filename);
    
    // Поиск идентификаторов
    std::string query;
    std::cout << "Введите идентификатор для поиска (или 'exit' для выхода): ";
    while (std::cin >> query && query != "exit") {
        if (table.search(query)) {
            std::cout << "Идентификатор найден!" << std::endl;
        } else {
            std::cout << "Идентификатор не найден." << std::endl;
        }
        std::cout << "Введите идентификатор для поиска (или 'exit' для выхода): ";
    }

    // Вывод статистики
    table.print_statistics();

    table.print_table();

    return 0;
}
