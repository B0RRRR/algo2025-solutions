#include <iostream>

int64_t Sum(int64_t a, int64_t b) {
    int64_t result = a + b;
    if (result > std::numeric_limits<int>::max()) {
        return std::numeric_limits<int>::max();
    }
    if (result < std::numeric_limits<int>::min()) {
        return std::numeric_limits<int>::min();
    }
    return result;
}
