#include "flight_math.hpp"
#include "csv_handler.hpp"
#include "handle_python.hpp"
#include <iostream>
#include <map>

int main()
{
    int CRS = 90;
    int IAS = 100;
    
    Wind wind;
    wind.speed = 15;
    wind.angle = 180;
    int variation = 10;
    int altitude = 10000;
    int press_at_MSL = 101325;
    int temp_at_MSL = 15;

    int TAS = Speed_calculations::calculate_true_airspeed(IAS,temp_at_MSL,altitude,0);
    int GS = Speed_calculations::calculate_ground_speed(TAS,CRS,wind);
    std::cout << "IAS: " << IAS << "\n";
    std::cout << "TAS: " << TAS << "\n";
    std::cout << "GS: " << GS << "\n";
    std::cout << "Wind shift: " << GS - TAS << "\n";


    int WCA = Course_and_distance_calculations::calculate_wind_correction_angle(wind, CRS, TAS);
    std::cout << "WCA is equal to: " << WCA << '\n';


    std::map<std::string,Airport> airports = Load_data::load_airports("data/map/airports.csv");

    Airport EPWA = airports["EPWA"];
    Airport EPLL = airports["EPLL"];

    std::cout << "Course: " << Course_and_distance_calculations::calculate_course(EPWA.latitude, EPWA.longitude, EPLL.latitude, EPLL.longitude) << "\n";
    std::cout << "Distance [nm]: " << Course_and_distance_calculations::calculate_distance(EPWA.latitude, EPWA.longitude, EPLL.latitude, EPLL.longitude) << "\n";

    std::cout << "Pressure at 10000ft: " << calculate_pressure(101325, 15, 10000) << "\n";
    std::cout << "Temp at 10000ft: " << calculate_temperature(15, 10000, 0) << "\n";

    double ETE = Time_calculations::calculate_ETE(Course_and_distance_calculations::calculate_distance(EPWA.latitude, EPWA.longitude, EPLL.latitude, EPLL.longitude),GS);
    Time time = Time_calculations::format_time(ETE);
    std::cout << "ETE: " << Time_calculations::display_time(time) << "\n";

    std::cout << "Declination of EPWA: " << get_declination(EPWA.latitude, EPWA.longitude);
    
    return 0;
}