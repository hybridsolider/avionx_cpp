#include <cmath>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include "csv_handler.hpp"
#ifndef FLIGHT_MATH
#define FLIGHT_MATH



//========================================
//
//            CONST VALUES
//
//========================================
// R = 6371.0 km 
#define R 3440.0 // NM

#define UNIVERSAL_GAS_CONSTANT 8.3144598 
#define MOLAR_MASS_OF_AIR 0.0289644 
#define GRAVITY 9.80665  
#define TEMP_LAPSE_RATE 0.0065 

struct Wind
{
    int angle;
    int speed;
};

struct Time
{
    int hour;
    int minutes;
    int seconds;
};

struct Data
{
    int year;
    int month;
    int day;
    Time time;
};

struct Leaf
{
    double latitude; // or y-axis
    double longitude; // or x-axis
    std::string ident;
    Leaf* leaf_neg;
    Leaf* leaf_pos;
};

//========================================
//
//          PRECISION ROUNDING
//
//========================================
template <typename K, typename T> 
T rounded(K value, int precision) // how many nums after comma
{
    T prec = std::pow(10, (float)precision);
    return std::round(prec * static_cast<T>(value)) / prec;
}

//========================================
//
//            CONVERTIONS
//
//========================================
double deg_to_rad(double num)
{

    return num * M_PI / 180.0;

        
}
double rad_to_deg(double num)
{
    return num * 180.0 / M_PI;
}
double celcius_to_kelvin(double temperature)
{
    return temperature + 273.15;
}
double kelvin_to_celcius(double temperature)
{
    return temperature - 273.15;
}

double pounds_to_kilos(double pounds)
{
    return rounded<double,double>(pounds * 0.45359237, 3);
}
double kilos_to_pounds(double kilos)
{
    return rounded<double,double>(kilos / 0.45359237, 3);
}

//========================================
//
//            CALCUlATIONS
//
//========================================


double median(std::vector<double> list)
{
    std::sort(list.begin(), list.end());
    int len = list.size();

    // Check if the number of elements is odd
    if (len % 2 != 0)
        return (double)list[len / 2];

    // If the number of elements is even, return the average
    // of the two middle elements
    return (double)(list[(len - 1) / 2] + list[len / 2]) / 2.0;
}

class Course_and_distance_calculations
{

public:
    static int calculate_true_heading(int HDG, double variation, int precision)
    {
        return rounded<double,int>((double)HDG + variation, precision);
    }


    static int calculate_wind_correction_angle(Wind& wind, int CRS, int TAS) 
    {
        // WCA = arcsin((Wspeed * sin(Wangle - CRS)) / TAS)
        double rad_CRS = deg_to_rad(CRS);
        double rad_wind_angle = deg_to_rad(wind.angle);
        double angle_delta = abs(rad_wind_angle - rad_CRS);

        double sine = std::sin(angle_delta);
        double asine = std::asin((wind.speed * sine) / TAS);
        
        return std::round(rad_to_deg(asine));
    }

    static int calculate_course(double lat1, double lon1, double lat2, double lon2) 
    {
        // θ=atan2(sinΔλ⋅cosφ2​,cosφ1​⋅sinφ2​−sinφ1​⋅cosφ2​⋅cosΔλ)
        double lambda1 = deg_to_rad(lon1);
        double lambda2 = deg_to_rad(lon2);

        double phi1 = deg_to_rad(lat1);
        double phi2 = deg_to_rad(lat2);

        double cos_phi1 = std::cos(phi1);
        double cos_phi2 = std::cos(phi2);

        double sin_phi1 = std::sin(phi1);
        double sin_phi2 = std::sin(phi2);

        double delta_lambda = lambda2 - lambda1;
        double sin_delta_lambda = std::sin(delta_lambda);
        double cos_delta_lambda = std::cos(delta_lambda);

        double theta = std::atan2(sin_delta_lambda * cos_phi2, cos_phi1 * sin_phi2 - sin_phi1 * cos_phi2 * cos_delta_lambda);
        double deg_theta = rad_to_deg(theta);
        if (deg_theta < 0) deg_theta += 360; // crs -10 => 350

        return static_cast<int>(std::round(deg_theta));
    }

    static double calculate_distance(double lat1, double lon1, double lat2, double lon2)
    {
        // delta_phi   = radians(lat2 - lat1)
        // delta_lambda= radians(lon2 - lon1)
        // phi1        = radians(lat1)
        // phi2        = radians(lat2)
        // a           = sin(delta_phi/2)^2 + cos(phi1) * cos(phi2) * sin(delta_lambda/2)^2
        // c           = 2 * atan2(sqrt(a), sqrt(1-a))
        // distance    = R * c

        double lambda1 = deg_to_rad(lon1);
        double lambda2 = deg_to_rad(lon2);
        double delta_lambda = lambda2 - lambda1;

        double phi1 = deg_to_rad(lat1);
        double phi2 = deg_to_rad(lat2);
        double delta_phi = phi2 - phi1;

        double a = std::pow(std::sin(delta_phi/2),2) + std::cos(phi1) * std::cos(phi2) * std::pow(std::sin(delta_lambda/2),2);
        
        double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1-a));

        

        return R * c;
    }

    static double calculate_short_distance(double lat1, double lon1, double lat2, double lon2) // works fine with smaller numbers
    {
        return std::sqrt(std::pow(lat1 - lat2, 2) + std::pow(lon1 - lon2,2));
    }

    template <typename T, typename distance_func>
    static T nearest_waypoint(std::vector<T>& waypoints, double latitude, double longitude, distance_func distance_function)
    {
        T first_val = waypoints[0];
        double min_distance = distance_function(first_val.latitude, first_val.longitude, latitude, longitude);
        for (size_t i = 0; i < waypoints.size(); i++)
        {
            double distance = distance_function(waypoints[i].latitude, waypoints[i].longitude, latitude, longitude);
            if(distance < min_distance)
            {
                min_distance = distance;
                first_val = waypoints[i];
            }
        }
        return first_val;

    }

    // template <typename T>
    // static std::vector<std::string> nearest_waypoints(std::map<std::string, T>& waypoints, double latitude, double longitude, double max_distance)
    // {  
    //     std::vector<std::string> out;
    //     for (auto const& [key, val] : waypoints)
    //     {
    //         if(distance_function(val.latitude, val.longitude, latitude, longitude) <= max_distance)
    //         {
    //             out.push_back(val.ident);
    //         }
    //     }
    //     return out;
    // } should be vector instead of map
};

double calculate_pressure(double press_at_MSL, double temp_at_MSL, int altitude)
{
    // P = Pmsl(1 - (L*h)/Tmsl) ** ((g*M)/(L*R))
    double temp_K = 273.15 + temp_at_MSL;
    double altitude_m = 0.3048 * altitude;
    double power = (GRAVITY * MOLAR_MASS_OF_AIR) / (UNIVERSAL_GAS_CONSTANT * TEMP_LAPSE_RATE);
    double a = 1 - (TEMP_LAPSE_RATE * altitude_m)/temp_K;
    double P = press_at_MSL * std::pow(a, power);
    return rounded<double, double>(P,2);

}

double calculate_temperature(int temperature, int altitude, int elevation)
{
    return temperature - 2 * ((altitude - elevation) / 1000.0); // .0 makes it double instead of int
}

class Speed_calculations
{
public:
    static int calculate_true_airspeed(int IAS, int temp, int altitude, int elevation)
    {
        double temp_K = celcius_to_kelvin(static_cast<double>(temp));
        double temp_K_ISA = celcius_to_kelvin(calculate_temperature(temp,altitude,elevation));
        return static_cast<int>(IAS * std::sqrt(temp_K/temp_K_ISA) + 0.5); // + 0.5 because static_cast always rounds it down (1.9 -> 1) so by adding 0.5 (1.4 + 0.5 -> 1.9 -> 1) but (1.6 + 0.5 -> 2.1 -> 2)
    }
    static int calculate_ground_speed(int TAS, int CRS, Wind& wind)
    {
        int delta_angle =  (wind.angle - CRS + 360) % 360; // if wind = 0 and CRS = 90, difference is equal to -90. if we add 360 we got 270. 270 % 360 = 270. No negative course.
        int ground_speed = static_cast<int>(TAS + wind.speed * std::cos(deg_to_rad(delta_angle)) + 0.5);
        return ground_speed;
    }
};

class Time_calculations
{
public:

    static Time format_time(double hour=0, double minutes=0, double seconds=0)
    {
        Time time;
        time.hour = static_cast<int>(hour);
        time.minutes =  static_cast<int>(minutes);
        time.seconds =  static_cast<int>(seconds);

        if (hour - time.hour != 0) // ex. 2.1234h: 2.1234h - 2.0h = 0.1234h
        {
            minutes += 60 * (hour - time.hour); // 2.1234h - 2.0h = 0.1234h | 0.1234h * 60 = 7.404m
            time.minutes = static_cast<int>(minutes); // 7.404m => (int) 7m
        }
        if (minutes - time.minutes != 0) // 7.404m - 7m = 0.404m
        {
            seconds += 60 * (minutes - time.minutes); // 7.404m - 7m = 0.404 m| 0.404m * 60 = 24.24s
            time.seconds = static_cast<int>(seconds + 0.5); // 24s
        }
        return time;
    }
    static std::string display_time(Time& time, char splitter=':')
    {
        std::string form_time;
        if (time.hour < 10) form_time += "0";
        form_time += std::to_string(time.hour);
        form_time += splitter;
        if (time.minutes < 10) form_time += "0";
        form_time += std::to_string(time.minutes);
        form_time += splitter;
        if (time.seconds < 10) form_time += "0";
        form_time += std::to_string(time.seconds);

        return form_time;
    }
    static double calculate_ETE(double distance, int ground_speed)
    {
        return distance / static_cast<double>(ground_speed);
    }
};

class Fuel_calculations
{
public:
    static double fuel_needed(double fuel_burning_rate, double time)
    {
        return fuel_burning_rate * time;
    }
    static double fuel_weight(double density, double volume)
    {
        return pounds_to_kilos(density * volume);
    } 
};


class KN_tree
{
public:
    template <typename T>
    static void to_leafs(const std::map<std::string, T>& waypoints, std::vector<Leaf>& leafs)
    {
        // makes waypoint list lighter and makes it vector
        Leaf leaf;
        leafs.reserve(waypoints.size());
        for (auto const& [key, val] : waypoints)
        {
            leafs.push_back
            (
                Leaf
                {
                    val.latitude,
                    val.longitude,
                    val.ident,
                    nullptr,
                    nullptr
                }
            );
        }

    }

    static void map_coordinates(std::vector<Leaf>& waypoints, std::vector<Leaf>& out, Leaf& center)
    {
        // maps Leafs. changes latitude to y and longitude to x
        out.clear();
        out.reserve(waypoints.size());

        double center_lat_rad = deg_to_rad(center.latitude);
        double center_lon_rad = deg_to_rad(center.longitude);
        double cos_center_lat = std::cos(center_lat_rad);
        for (size_t i = 0; i < waypoints.size(); i++)
        {
            double lat_rad = deg_to_rad(waypoints[i].latitude);
            double lon_rad = deg_to_rad(waypoints[i].longitude);
            out.emplace_back(
                Leaf
                {
                    R * (lat_rad - center_lat_rad),
                    R * (lon_rad - center_lon_rad) * cos_center_lat,
                    waypoints[i].ident,
                    nullptr,
                    nullptr
                }
            );
        }
    }

    static void change_center(std::vector<Leaf>& waypoints, std::vector<Leaf>& out, Leaf& center)
    {
        // allows for changing center of map (0,0) coordinates
        out.clear();
        out.reserve(waypoints.size());
        for (size_t i = 0; i < waypoints.size(); i++)
        {
            out.emplace_back
            (
                Leaf
                {
                    waypoints[i].latitude - center.latitude,
                    waypoints[i].longitude - center.longitude,
                    waypoints[i].ident,
                    nullptr, 
                    nullptr
                }
            );
        }
    }
    

    static Leaf find_center(const std::vector<Leaf>& waypoints)
    {
        double x_sum = 0;
        double y_sum = 0;
        double z_sum = 0;

        for (auto& wp : waypoints)
        {
            double lat_rad = wp.latitude * M_PI / 180.0;
            double lon_rad = wp.longitude * M_PI / 180.0;

            x_sum += cos(lat_rad) * cos(lon_rad);
            y_sum += cos(lat_rad) * sin(lon_rad);
            z_sum += sin(lat_rad);
        }

        double count = static_cast<double>(waypoints.size());
        double x_avg = x_sum / count;
        double y_avg = y_sum / count;
        double z_avg = z_sum / count;

        double lon = atan2(y_avg, x_avg) * 180.0 / M_PI;
        double hyp = sqrt(x_avg * x_avg + y_avg * y_avg);
        double lat = atan2(z_avg, hyp) * 180.0 / M_PI;

        return Leaf{
            lat,
            lon,
            "geographic_center",
            nullptr,
            nullptr
        };
    }

};


#endif