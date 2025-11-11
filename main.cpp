#include "csv_handler.hpp"

int main()
{

    std::vector<Airport> airports = Load_data::load_airports("data/map/airports.csv");
    for (Airport& x : airports)
    {
        std::cout << x.ident << "\n";

    }
    return 0;
}