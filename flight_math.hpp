#include <cmath>
#ifndef FLIGHT_MATH
#define FLIGHT_MATH

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

template <typename K, typename T> 
T rounded(K value, int precision) // how many nums after comma
{
    T prec = std::pow(10, (float)precision);
    return std::round(prec * static_cast<T>(value)) / prec;
}


double deg_to_rad(double num)
    {

        return num * M_PI / 180.0;

        
    }
double rad_to_deg(double num)
    {
        return num * 180.0 / M_PI;
    }

class Course_and_distance_calculation
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
};

double calculate_pressure(double press_at_MSL, double temp_at_MSL, int altitude)
{
    // P = Pmsl(1 - (L*h)/Tmsl) ** ((g*M)/(L*R))
    double temp_K = 273.15 + temp_at_MSL;
    double altitude_m = 0.3048 * altitude;
    double power = (GRAVITY * MOLAR_MASS_OF_AIR) / (UNIVERSAL_GAS_CONSTANT * TEMP_LAPSE_RATE);
    double a = 1 - (TEMP_LAPSE_RATE * altitude_m)/temp_K;
    double P = std::pow(press_at_MSL * a, power);
    return rounded<double, double>(P,2);

}

class Speed_calculation
{
public:
    static int calculate_true_airspeed()
    {

    }
};



#endif