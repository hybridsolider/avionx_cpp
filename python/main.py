import sys
from pygeomag import GeoMag
import open_meteo_API
def list_to_string(li:list):
    to_return = ""
    counter = 0
    for i in li:
        to_return += f"{counter}={i}"
        counter += 1
    return to_return

def dict_to_string(di:dict):
    to_return = ""
    for key, value in di.items():
        to_return += f"{key}={value} "
    return to_return

def get_wind(lat, lon, pressure, time): # time = "2025-10-01T20:00"
    return dict_to_string(open_meteo_API.sounding(lat,lon,pressure,time)) #pressure / 100 -> Pa -> hPa
    

def get_declination(lat, lon, alt_ft=0.0, year=2025.0): 
    alt_m = 0.3048 * float(alt_ft)
    alt_km = alt_m / 1000.0
    geo = GeoMag()  # uses WMM2025 by default
    result = geo.calculate(
        glat=float(lat), 
        glon=float(lon), 
        alt=alt_km, 
        time=float(year)
    )
    return result.d
    
def handle_request():
    operation = sys.argv[1]
    arg_list = sys.argv[2:]

    # Try to safely convert each argument to int or float
    converted_args = []
    for arg in arg_list:
        try:
            if '.' in arg:
                converted_args.append(float(arg))
            else:
                converted_args.append(int(arg))
        except ValueError:
            # leave it as string if it can't be converted
            converted_args.append(arg)

    # Call the function dynamically
    result = globals()[operation](*converted_args)

    # Print the result cleanly
    sys.stdout.write(str(result))
    
if __name__ == '__main__':
    handle_request()