#include <iostream>

int Sum(int a, int b) {
    int result = a + b;
    if (result > std::numeric_limits<int>::max())
        return std::numeric_limits<int>::max();
    if (result < std::numeric_limits<int>::min())
        return std::numeric_limits<int>::min();
    return result;
}
