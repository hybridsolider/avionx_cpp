#include "csv.h"
#include <string>
#include <map>

#ifndef CSV_HANDLER
#define CSV_HANDLER
struct Waypoint
{
    std::string ident;
    std::string name;
    std::string type;
    std::string frequency_khz;
    std::string country;
    float longitude;
    float latitude;
    float magnetic_variation;
};

struct frequency
{
    std::string ident;
    std::string type;
    std::string description;
    int frequency_mhz;
};


struct Airport
{
    std::string ident; // code. (EPWA)
    std::string icao; // icao code (EPWA)
    std::string iata; // iata code (WAW)
    std::string type; // how large the airport is (large_airport)
    std::string name; // name of an airport (Warsaw Chopin Airport)
    std::string country;
    std::string municipality; 
    float latitude;
    float longitude;
    int elevation;
    std::map<std::string, frequency> frequencies; // key = type 
};




#endif

