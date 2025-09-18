#include <omp.h>

#include <print>
#include <iostream>

int main() {

    int i = 0;
    std::print("How much threads need to be allocated for program? (>0) ");
    std::cin >> i;
    std::println();

    omp_set_num_threads(i);

#pragma omp parallel for
    for (size_t i = 0; i < 10; ++i) {
        std::println("Println's Hello, World from thread {} of {}", omp_get_thread_num(), omp_get_num_threads());
    }

#pragma omp parallel for
    for (size_t i = 0; i < 10; ++i) {
        std::cout << "Cout's Hello, World from thread " << omp_get_thread_num() << " of " << omp_get_num_threads() << '\n';
    }

    return 0;
}
