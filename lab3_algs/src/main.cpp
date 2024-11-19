#include <iostream>
#include <unordered_set>
#include <double_linked_list.hpp>

int main() {
	My::DoubleLinkedList<int> L1;
	My::DoubleLinkedList<int> L2;

    // Ввод первого списка L1
    std::cout << "Введите элементы списка L1 (введите -1 для завершения): ";
    int num;
    while (std::cin >> num && num != -1) {
        L1.push_back(num);
    }

    // Ввод второго списка L2
    std::cout << "Введите элементы списка L2 (введите -1 для завершения): ";
    while (std::cin >> num && num != -1) {
        L2.push_back(num);
    }

    // Формируем множество чисел из L1 для быстрого поиска
    std::unordered_set<int> setL1;
    for (int i = 0; i < L1.size(); ++i) {
        auto val = L1[i];
        if (val.has_value()) {
            setL1.insert(val.value());
        }
    }

    // Добавляем элементы из L2 в конец L1, если их нет в L1
    for (int i = 0; i < L2.size(); ++i) {
        auto val = L2[i];
        if (val.has_value() && setL1.find(val.value()) == setL1.end()) {
            L1.push_back(val.value());
            setL1.insert(val.value()); // Обновляем множество
        }
    }

    // Вывод списка L1 на экран
    std::cout << "Список L1 после добавления элементов: ";
    std::cout << L1.log_nodes() << std::endl;

    return 0;
}
