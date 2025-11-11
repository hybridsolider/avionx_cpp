#include "csv_handler.hpp"

int main()
{
    std::cout << "LOADING WAYPOINTS... (1/2)\n";
    std::vector<Waypoint> waypoints = Load_data::load_waypoints("data/map/navaids.csv");
    std::cout << "LOADING AIRPORTS... (2/2)\n";
    std::vector<Airport> airports = Load_data::load_airports("data/map/airports.csv");
    std::cout << "loaded succesfully!\n";

    return 0;
}