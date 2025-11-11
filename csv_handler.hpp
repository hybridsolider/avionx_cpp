
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
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

class Load_data
{
private:
    static std::vector<std::string> parse_csv_line(const std::string& line)
    {
        std::vector<std::string> result;
        std::string cell;
        bool in_quotes = false;

        for (size_t i = 0; i < line.size(); ++i)
        {
            char c = line[i];

            if (c == '"')
            {
                // Handle escaped quote ("")
                if (in_quotes && i + 1 < line.size() && line[i + 1] == '"') {
                    cell += '"';
                    ++i;
                } else {
                    in_quotes = !in_quotes;
                }
            }
            else if (c == ',' && !in_quotes)
            {
                result.push_back(cell);
                cell.clear();
            }
            else
            {
                cell += c;
            }
        }

        result.push_back(cell);
        return result;
    }
public:
    static std::vector<Airport> load_airports(std::string filename)
    {
        std::vector<Airport> airports;
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filename << "\n";
            return airports;
        }

        std::string line;
        std::getline(file, line);

        while (std::getline(file, line)) {
            auto row = parse_csv_line(line);
            // Convert row to Airport
            if (row.size() >= 6) { // check enough columns
                Airport a;
                a.ident = row[1];
                a.type = row[2];
                a.name = row[3];
                a.latitude = stof(row[4]);
                a.longitude = stof(row[5]);
                if (!row[6].empty()) 
                {
                    a.elevation = std::stoi(row[6]);
                } else {a.elevation = 0;}
                a.country = row[8];
                a.municipality = row[10];
                a.icao = row[12];
                a.iata = row[13];

                airports.push_back(a);
            }
        }

        return airports;
    }
    
};



#endif

