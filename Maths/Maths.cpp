#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

#include "./Maths.hpp"

using namespace std;

int mod(const int& a, const int& b)
{
    int r = a % b;
    return r < 0 ? r + b : r;
}

// because its big-endian, thus, it needs reverse
int reverseInt (int i)
{
    unsigned char c1, c2, c3, c4;

    c1 = i & 255;
    c2 = (i >> 8) & 255;
    c3 = (i >> 16) & 255;
    c4 = (i >> 24) & 255;

    return ((int)c1 << 24) + ((int)c2 << 16) + ((int)c3 << 8) + c4;
}
