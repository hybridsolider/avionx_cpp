
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
    std::string Frequency_khz;
    std::string Frequency_khz_DME;
    std::string country;
    double longitude;
    double latitude;
    double magnetic_variation;
    int elevation;
};

struct Frequency
{
    std::string ident;
    std::string type;
    std::string description;
    int Frequency_mhz;
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
    double latitude;
    double longitude;
    int elevation;
    std::map<std::string, Frequency> frequencies; // key = type 
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
    static double safe_stod(const std::string& s, double default_value = 0.0f) {
    if (s.empty()) return default_value;
    try {
        return std::stod(s);
    } catch (...) {
        return default_value;
    }
}
public:
    static std::map<std::string,Frequency> load_airport_frequencies(std::string filename) 
    {
        std::map<std::string, Frequency> frequencies;
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filename << "\n";
            return frequencies;
        }

        std::string airport_ident;
        std::map<std::string, Frequency> freq; // ident / Frequency
        std::string line;
        std::getline(file, line);

        while (std::getline(file, line)) {
            auto row = parse_csv_line(line);
            


        }
    }
    static std::map<std::string, Airport> load_airports(std::string filename)
    {
        std::map<std::string,Airport> airports;
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

            Airport a;
            a.ident = row[1];
            a.type = row[2];
            a.name = row[3];
            a.latitude = stod(row[4]);
            a.longitude = stod(row[5]);
            if (!row[6].empty()) 
            {
                a.elevation = std::stoi(row[6]);
            } else {a.elevation = 0;}
            a.country = row[8];
            a.municipality = row[10];
            a.icao = row[12];
            a.iata = row[13];

            airports[a.ident] = a;
            
        }

        return airports;
    }
    static std::map<std::string, Waypoint> load_waypoints(std::string filename)
    {
        std::map<std::string,Waypoint> waypoints;
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filename << "\n";
            return waypoints;
        }      
        std::string line;
        std::getline(file, line);

        while (std::getline(file, line)) {
            auto row = parse_csv_line(line);
            Waypoint w;

            w.ident = row[2];
            w.name = row[3];
            w.type = row[4];
            w.Frequency_khz = row[5];
            w.latitude = safe_stod(row[6]);
            w.longitude = safe_stod(row[7]);
            if (!row[8].empty()) 
            {
                w.elevation = std::stoi(row[8]);
            } else {w.elevation = 0;}
            w.country = row[9];
            w.Frequency_khz_DME = row[10];
            w.magnetic_variation = safe_stod(row[16]);

            waypoints[w.ident] = w;
            
        }
        return waypoints;
    }
    static std::map<std::string, Waypoint> load_waypointsz(std::string filename)
    {
        std::map<std::string,Waypoint> waypoints;
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filename << "\n";
            return waypoints;
        }      
        std::string line;
        std::getline(file, line);

        while (std::getline(file, line)) {
            auto row = parse_csv_line(line);
            Waypoint w;

            w.ident = row[2];
            w.name = row[3];
            w.type = row[4];
            w.Frequency_khz = row[5];
            w.latitude = safe_stod(row[6]);
            w.longitude = safe_stod(row[7]);
            if (!row[8].empty()) 
            {
                w.elevation = std::stoi(row[8]);
            } else {w.elevation = 0;}
            w.country = row[9];
            w.Frequency_khz_DME = row[10];
            w.magnetic_variation = safe_stod(row[16]);

            waypoints[w.ident] = w;
            
        }
        return waypoints;
    }
    
};



#endif



// fixes: https://github.com/FayyazAK/Global-Aviation-Waypoints/tree/main