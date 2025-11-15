#include "flight_math.hpp"
#include "csv_handler.hpp"
#include <iostream>
#include <map>

int main()
{
    int CRS = 90;
    int TAS = 120;
    Wind wind;
    wind.speed = 15;
    wind.angle = 180;
    int variation = 10;
    int WCA = Course_and_distance_calculation::calculate_wind_correction_angle(wind, CRS, TAS);
    std::cout << "WCA is equal to: " << WCA << '\n';


    std::map<std::string,Airport> airports = Load_data::load_airports("data/map/airports.csv");

    Airport EPWA = airports["EPWA"];
    Airport EPLL = airports["EPLL"];

    std::cout << "Course: " << Course_and_distance_calculation::calculate_course(EPWA.latitude, EPWA.longitude, EPLL.latitude, EPLL.longitude) << "\n";
    std::cout << "Distance [nm]: " << Course_and_distance_calculation::calculate_distance(EPWA.latitude, EPWA.longitude, EPLL.latitude, EPLL.longitude) << "\n";

    return 0;
}