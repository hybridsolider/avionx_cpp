#include "flight_math.hpp"
#include <iostream>

int main()
{
    int CRS = 90;
    int TAS = 120;
    Wind wind;
    wind.speed = 15;
    wind.angle = 180;
    int variation = 10;
    int WCA = Course_calculation::calculate_wind_correction_angle(wind, CRS, TAS);
    std::cout << "WCA is equal to: " << WCA << '\n';

    return 0;
}