#include <cmath>
#ifndef FLIGHT_MATH
#define FLIGHT_MATH

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

class Convertion
{
public:

};

class Course_calculation
{
private:
    static double deg_to_rad(double num)
    {

        return num * M_PI / 180.0;

        
    }
    static double rad_to_deg(double num)
    {
        return num * 180.0 / M_PI;
    }
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
};

#endif