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
    std::map<std::string,Waypoint> waypoints = Load_data::load_waypoints("data/map/navaids.csv");
    std::map<std::string,Waypoint> fixes = Load_data::load_fixes("data/map/fixes.csv");
    waypoints.insert(fixes.begin(), fixes.end());

    Airport EPWA = airports["EPWA"];
    Airport EPLL = airports["EPLL"];

    std::cout << "Course: " << Course_and_distance_calculations::calculate_course(EPWA.latitude, EPWA.longitude, EPLL.latitude, EPLL.longitude) << "\n";
    std::cout << "Distance [nm]: " << Course_and_distance_calculations::calculate_distance(EPWA.latitude, EPWA.longitude, EPLL.latitude, EPLL.longitude) << "\n";

    std::cout << "Pressure at 10000ft: " << calculate_pressure(101325, 15, 10000) << "\n";
    std::cout << "Temp at 10000ft: " << calculate_temperature(15, 10000, 0) << "\n";

    double ETE = Time_calculations::calculate_ETE(Course_and_distance_calculations::calculate_distance(EPWA.latitude, EPWA.longitude, EPLL.latitude, EPLL.longitude),GS);
    Time time = Time_calculations::format_time(ETE);
    std::cout << "ETE: " << Time_calculations::display_time(time) << "\n";

    std::cout << "Declination of EPWA: " << get_declination(EPWA.latitude, EPWA.longitude) << "\n";

    std::cout << "KDTREE:\n";
    std::vector<Leaf> leafs;
    KN_tree::to_leafs(waypoints, leafs);
    for (Leaf l : leafs)
    {
        std::cout << "\nident: " << l.ident << "\nlatitude: " << l.latitude << "\nlongitude: " << l.longitude;
    }
    std::cout << "\nTransformed wp to leafes (1/\n";
    Leaf center = KN_tree::find_center(leafs);
    Leaf near_center = Course_and_distance_calculations::nearest_waypoint<Leaf>
    (
        leafs,
        center.latitude,
        center.longitude, 
        Course_and_distance_calculations::calculate_short_distance
    );
    std::cout << "center is:\nlat: " << center.latitude << "\nlon: " << center.longitude;
    std::cout << "near center wpt is:\nlat: " << near_center.latitude << "\nlon: " << near_center.longitude << "\nident: " << near_center.ident;
    std::cout << "\nFound center and nearest waypoint from it! (2/\n";
    std::vector<Leaf> leafs_as_math;
    KN_tree::map_coordinates(leafs,leafs_as_math,near_center);
    for (Leaf l : leafs_as_math)
    {
        std::cout << "\nident: " << l.ident << "\nlatitude: " << l.latitude << "\nlongitude: " << l.longitude;
    }

    std::cout << "\nTransformed math coordinates from geographical (3/\n";    


    
    return 0;
}