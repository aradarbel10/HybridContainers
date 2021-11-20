# HybridContainers
Create mutable C++ containers at compile-time, then use them during run-time!

## Motivation
The C++ is getting more and more of its features `constexpr`ed as time goes on. Sadly, one huge thing is missing from it -- constexpr containers.

With the introduction of C++20, `std::vector` and `std::string` are now usable in "constexpr-context" thanks to transient allocations (caveant: [language support](##Language-Support)). This means your vectors and strings can allocate memory inside constexpr functions, but that memory ***cannot*** leak, and must be deallocated by the end of the function. Moreover, containers like `std::set` and `std::map` are currently not usable at all in constexpr-context. Hybrid comes to solve these problems.

## How To Use
Using Hybrid is very simple, the hope is that you'll barely need to modify your existing code.

Let's take a look at this simple `squares` function
```cpp
// here's the algorithm we want to compute during compile time
// notice it uses a plain ol' std::vector
constexpr std::vector<int> squares(int n) {
    std::vector<int> result;
    for (int i = 1; i * i <= n; i++) {
        result.push_back(i * i);
    }
    return result;
}
```

This function can be used normally during run-time:
```cpp
auto dynamic_result = squares(70);
// == { 1, 4, 9, 16, 25, 36, 49, 64 }
```
Or during compile-time, with the `hybrid_compute` macro:
```cpp
constexpr auto static_result = hybrid_compute(squares, 70);
static_assert(static_result == hybrid::array{ 1, 4, 9, 16, 25, 36, 49, 64 });
```

But don't forget that these containers are *hybrid*:
```cpp
std::vector<int> back_to_runtime = hybrid::cast(static_result);
back_to_runtime.push_back(42);

for (int i = 0; i < back_to_runtime.size(); i++) {
	std::cout << (i == 0 ? "" : ", ") << back_to_runtime[i];
}
```

And the output is as expected,
```
1, 4, 9, 16, 25, 36, 49, 64, 42
```
Awesome!


## Under The Hood
Hybrid gets around the language's limitation using one simple macro:
```cpp
#define hybrid_compute(func, ...) \
    hybrid::compute_val < []() consteval { return hybrid::compute_size(&func, __VA_ARGS__); } \
    () > (&func, __VA_ARGS__)
```
This macro takes a function and its arguments, and calls it twice. The first call (`compute_size`) just returns the size of the resulting container. This is crucial, because this size is then given to the second call (`compute_val`) that actually returns the result of the function call.

Without knowing the size of the result in advance, we can't know the size of the result in advance. The size can't be computed inside the function call itself because in C++ function parameters are *never constant expressions, thus cannot be used as template parameters*. That's why we have to first know the size outside of the function call.

## Installation
Hybrid is a header-only library. To use, copy the `include/` folder into your project directory, and you're ready to go!

The repo also includes a VS solution that runs an example program `main.cpp`. I intentionally chose the MSVC compiler for this (see [language support](##Language-Support)).

## Language Support
As of right now, Hybrid relies completely on `std::vector` as the underlying container for everything else. In C++20 ([P1004R2](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p1004r2.pdf)) `std::vector` got its constexpr constructors, destructor, and everything else it needs to be used in constexpr context.

This library improvement is currently only provided in MSVC and libstdc++12, but not in clang's libc++! If you're going to use Hybrid, you'll need to either use one of those library implementations, or wait until I get my own vector implementation into Hybrid (might never happen).

Other than all that, Hybrid makes light use of other C++20 features, so make sure your compiler supports those as well.