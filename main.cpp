#include "include/hybrid/array.hpp"
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

/*

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


// even nested arrays
constexpr hybrid::array<hybrid::array<int>> multiplication_table(hybrid::array<int> xs, hybrid::array<int> ys) {
	hybrid::array<hybrid::array<int>> result;
	for (int x : xs) {
		hybrid::array<int> row;
		row.reserve(ys.size());
		for (int y : ys) {
			row.push_back(x * y);
		}
		result.push_back(row);
	}
	return result;
}

void test_nested() {
	auto dynamic_result = multiplication_table({1, 2, 3, 4, 5, 6}, {2, 4, 5, 7, 8});

	for (const auto& row : dynamic_result) {
		for (int num : row) {
			std::cout << num << ", ";
		}
		std::cout << "\n";
	}
	std::cout << "\n\n";

	constexpr auto static_size = hybrid::compute_size(multiplication_table, std::vector<int>{1, 2, 3, 4, 5, 6}, std::vector<int>{2, 4, 5, 7, 8});
	static_assert(static_size == 6);
	std::cout << "size: " << static_size << "\n";

	constexpr auto sizes = hybrid::max_inner_size(multiplication_table(hybrid::array{1, 2, 3, 4, 5, 6}, hybrid::array{2, 4, 5, 7, 8}));
	std::cout << "max size: " << sizes << '\n';

	auto test = hybrid::cast(dynamic_result);

	constexpr auto inner_size = [ptr = 42]() consteval {
		auto result_once = multiplication_table({1, 2, 3, 4, 5, 6}, {2, 4, 5, 7, 8});

		auto dimensions = ([]<typename T>() consteval { return 6; }).operator()<int>();

		return 5;
	}();
	static_assert(inner_size == 5);
}

constexpr hybrid::set<std::pair<char, int>> count_characters(std::string_view str) {
	hybrid::set<std::pair<char, int>> result;


	for (int i = 0; i < str.size(); i++) {
		result.insert({str[i], 0});
	}

	//std::for_each(str.begin(), str.end(), [&](const char c) { result.insert({ c, 0 }); });

	return result;
}

void test_count_characters() {
	//auto dynamic_result = count_characters("to be or not to be");

	//constexpr auto counts = hybrid::compute_size(count_characters, hybrid::array<char>{'a', 'b', 'c', 'd'});
	//static_assert(counts == 4);

	constexpr auto counts = hybrid::compute_size(count_characters, std::string_view{"abcdefghijklmnopqrst uvwxyz"});
	std::cout << "plz size " << counts << '\n';
	//static_assert(counts[' '] == 5);

	//std::cout << "\n\ncounts:\n";
	//for (const auto& [c, i] : counts) {
	//	std::cout << "(" << c << ", " << i << ")\n";
	//}
}

*/

int main() {

	test_array();
	//test_set();
	//test_map();
	//test_nested();
	//test_count_characters();

	return 0;
}