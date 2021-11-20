#include "hybrid/hybrid.hpp"
#include <iostream>
#include <vector>

// here's the algorithm we want to compute during compile time
constexpr std::vector<int> squares(int n) {
	// it uses a plain ol' vector, so you don't need to change anything in your code
	std::vector<int> result;
	for (int i = 1; i * i <= n; i++) {
		result.push_back(i * i);
	}
	return result;
}

void test_array() {
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
	std::cout << "\n\n";
}

// another algorithm, this time it uses `set`
constexpr hybrid::set<int> factors(int num) {
	// in this one we can't use `std::set` simply because it's not a literal type yet
	// hopefully one of the next standards will enable that
	hybrid::set<int> result;

	for (int i = 1; i <= num / 2; i++) {
		if (num % i == 0)
			result.insert(i);
	}

	result.insert(num);
	return result;
}

void test_set() {
	// let's look at a demonstration similar to before, just to see that everything works
	auto dynamic_result = factors(50);

	constexpr auto static_result = hybrid_compute(factors, 50);
	static_assert(static_result == hybrid::set{1, 2, 5, 10, 25, 50});

	hybrid::set<int> back_to_runtime = hybrid::cast(static_result);
	back_to_runtime.insert(5);

	for (auto iter = back_to_runtime.begin(); iter != back_to_runtime.end(); iter++) {
		std::cout << (iter == back_to_runtime.begin() ? "" : ", ") << *iter;
	}
	std::cout << "\n\n";
}


// lastly, you can also use `map`
constexpr hybrid::map<std::string_view, int> zip_dict(const hybrid::array<std::string_view>& keys, const hybrid::array<int>& vals) {
	if (keys.size() != vals.size())
		throw std::invalid_argument{"sizes must match!"};

	hybrid::map<std::string_view, int> result;

	auto key_iter = keys.begin();
	auto val_iter = vals.begin();
	for (; key_iter != keys.end(); key_iter++, val_iter++) {
		result.insert(*key_iter, *val_iter);
	}

	return result;
}

void test_map() {
	auto dynamic_result = zip_dict({"one", "two", "three", "seven"}, {1, 2, 3, 7});

	constexpr auto static_result = hybrid_compute(zip_dict, hybrid::array<std::string_view>{"one", "two", "three", "seven"}, hybrid::array<int>{1, 2, 3, 7});
	static_assert(static_result == hybrid::map<std::string_view, int>{{"one", 1}, {"two", 2}, {"three", 3}, {"seven", 7}});

	hybrid::map<std::string_view, int> back_to_runtime = hybrid::cast(static_result);
	back_to_runtime.insert("six", 6);

	for (auto iter = back_to_runtime.begin(); iter != back_to_runtime.end(); iter++) {
		std::cout << (iter == back_to_runtime.begin() ? "[" : ", [") << iter->first << "; " << iter->second << "]";
	}
	std::cout << "\n\n";
}


int main() {

	test_array();
	test_set();
	test_map();


	return 0;
}