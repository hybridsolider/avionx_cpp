import requests

def sounding(lat, lon, pressure_Pa, time_UTC):
    """
    Pobiera temperaturę i wiatr z modelu GFS Open-Meteo i interpoluje pionowo i w czasie.
    Dodatkowo stosuje redukcję wiatru, żeby był bardziej realistyczny.
    Args:
        lat (float): szerokość geograficzna
        lon (float): długość geograficzna
        pressure_Pa (float): poziom ciśnienia (np. 754)
        time_UTC (str): czas w formacie "YYYY-MM-DDTHH:MM" (UTC)
    Returns:
        dict: {'pressure_Pa', 'temperature_C', 'wind_speed_kt', 'wind_direction_deg'}
    """
    available_levels = [1000, 925, 850, 700, 500, 400, 300, 250, 200]

    # Znajdujemy dolny i górny poziom ciśnienia
    lower_candidates = [p for p in available_levels if p <= pressure_Pa]
    lower = max(lower_candidates) if lower_candidates else min(available_levels)
    upper_candidates = [p for p in available_levels if p >= pressure_Pa]
    upper = min(upper_candidates) if upper_candidates else max(available_levels)

    # Budujemy listę zmiennych do pobrania
    vars_ = []
    for lvl in [lower, upper]:
        vars_.extend([
            f"temperature_{lvl}hPa",
            f"wind_speed_{lvl}hPa",
            f"wind_direction_{lvl}hPa"
        ])

    url = (
        f"https://api.open-meteo.com/v1/gfs?"
        f"latitude={lat}&longitude={lon}"
        f"&hourly={','.join(vars_)}&timezone=UTC"
    )

    r = requests.get(url)
    r.raise_for_status()
    data = r.json()

    # Ucinamy czas do dokładności minutowej
    times = [t[:16] for t in data["hourly"]["time"]]
    target_time_str = time_UTC[:16]

    # Szukamy dwóch najbliższych godzin
    times_before = [t for t in times if t <= target_time_str]
    times_after = [t for t in times if t >= target_time_str]

    if times_before:
        t0 = max(times_before)
    else:
        t0 = times[0]   # fallback: pierwszy dostępny czas

    if times_after:
        t1 = min(times_after)
    else:
        t1 = times[-1]  # fallback: ostatni dostępny czas

    i0, i1 = times.index(t0), times.index(t1)

    # Interpolacja czasowa (w minutach)
    def to_minutes(tstr):
        h, m = map(int, tstr[-5:].split(":"))
        return h * 60 + m

    minutes0 = to_minutes(t0)
    minutes1 = to_minutes(t1)
    minutest = to_minutes(target_time_str)

    if minutes1 == minutes0:
        t_frac = 0
    else:
        t_frac = (minutest - minutes0) / (minutes1 - minutes0)

    def interp(v0, v1, f):
        return v0 + (v1 - v0) * f

    result = {}
    for param in ["temperature", "wind_speed", "wind_direction"]:
        v_lower = interp(
            data["hourly"][f"{param}_{lower}hPa"][i0],
            data["hourly"][f"{param}_{lower}hPa"][i1],
            t_frac
        )
        v_upper = interp(
            data["hourly"][f"{param}_{upper}hPa"][i0],
            data["hourly"][f"{param}_{upper}hPa"][i1],
            t_frac
        )

        if lower == upper:
            v = v_lower
        else:
            # interpolacja pionowa (liniowa po ciśnieniu)
            frac = (pressure_Pa - upper) / (lower - upper)
            v = interp(v_upper, v_lower, frac)

        # Redukcja wiatru
        if param == "wind_speed":
            if pressure_Pa > 950:      # bardzo nisko
                v *= 0.35
            elif pressure_Pa > 850:    # niska troposfera
                v *= 0.7
            v = v * 0.539957#1.94384  # m/s -> kt
        if param == "wind_direction": # without % 360 it sometimes shits with shitty negative numbers
            v = v % 360
        result[param] = v

    result["pressure_Pa"] = pressure_Pa
    result["temperature_C"] = result.pop("temperature")
    result["wind_speed_kt"] = result.pop("wind_speed")
    # print(url)
    return result


# =========================
# Przykład użycia
if __name__ == "__main__":
    lat, lon = 50.478, 19.085  # EPKT
    pressure = 496  # ~18000ft / 7C
    time = "2025-11-17T20:00"

    res = sounding(lat, lon, pressure, time)
    print(res)




