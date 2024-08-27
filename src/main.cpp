#include "stl_include.hpp"


int main()
{
    std::vector<int> a;
    std::span b(a);
    b.begin();
    return 0;
}