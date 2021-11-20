#include "../include/hybrid.hpp"
#include <iostream>
#include <vector>

// here's the algorithm we want to compute during compile time
constexpr std::vector<int> squares(int n) {
    // it uses a plain ol' vector, so you don't need to change anything in your code
    std::vector<int> result;
    // we want to find all the perfect square less than
    for (int i = 1; i * i <= n; i++) {
        result.push_back(i * i);
    }
    return result;
}

// main
int main() {
    // we can use the `squares` function dynamically if we want
    auto dynamic_result = squares(70);

    // or statically!
    constexpr auto static_result = hybrid_compute(squares, 70);
    static_assert(static_result == hybrid::array{1, 4, 9, 16, 25, 36, 49, 64});

    // and it can turn back to being dynamic. cool!
    std::vector<int> back_to_runtime = hybrid::cast(static_result);
    back_to_runtime.push_back(42);

    for (int i = 0; i < back_to_runtime.size(); i++) {
        std::cout << (i == 0 ? "" : ", ") << back_to_runtime[i];
    }

    return 0;
}