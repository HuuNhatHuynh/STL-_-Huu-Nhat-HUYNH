#include "stl_rotate.hpp"
#include <iostream>

using namespace std;

int main()
{
    Mesh LOWER_0, UPPER_0, LOWER_20, UPPER_20, LOWER_30, UPPER_30, LOWER_45, UPPER_45;

    parse_file("LOWER 0.stl", LOWER_0);
    parse_file("UPPER 0.stl", UPPER_0);

    float theta20 = -atan(1)*4/9;
    float theta30 = -atan(1)*2/3;
    float theta45 = -atan(1);

    LOWER_20 = stl_rotate(LOWER_0, theta20);
    UPPER_20 = stl_rotate(UPPER_0, theta20);

    LOWER_30 = stl_rotate(LOWER_0, theta30);
    UPPER_30 = stl_rotate(UPPER_0, theta30);

    LOWER_45 = stl_rotate(LOWER_0, theta45);
    UPPER_45 = stl_rotate(UPPER_0, theta45);

    write_file("LOWER 20.stl", LOWER_20);
    write_file("UPPER 20.stl", UPPER_20);

    write_file("LOWER 30.stl", LOWER_30);
    write_file("UPPER 30.stl", UPPER_30);

    write_file("LOWER 45.stl", LOWER_45);
    write_file("UPPER 45.stl", UPPER_45);
}

