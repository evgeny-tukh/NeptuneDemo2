#pragma once

#include <string>

std::string formatLat(double lat);
std::string formatLon(double lon);

Ctx *getCtx(HWND wnd);

inline const double PI { 3.1415926535897932384626433832795 };
inline const double PI_DIV_4 { PI * 0.25 };

inline double rad2deg(double val) { return val * 180.0 / PI; }
inline double lat2Mercator(double lat) { return log(tan((lat) * 0.5 + PI_DIV_4)); }
inline double mercator2Lat(double merc) { return 2.0 * (atan(exp((merc))) - PI_DIV_4); }

