#include <Windows.h>
#include <float.h>

#include <string>

#include "defs.h"

std::string formatAngle(double angle) {
    char buffer[100];
    sprintf(buffer, "%05.1f", angle);

    return std::string(buffer);
}

std::string formatFloat(double value) {
    char buffer[100];
    sprintf(buffer, "%.1f", value);

    return std::string(buffer);
}

std::string formatCoord(double coord, int numOfDegDigits, const char *wsChars) {
    double absCoord = fabs(coord);

    if (absCoord > 180.0)
        return {};
        
    char buf[50];
    char fmt[50];
    bool negative = coord < 0.0;
    int degrees = (int) absCoord;
    double minutes = (absCoord - degrees) * 60.0;
    
    strcpy(fmt, "%02d %6.3f%c");

    fmt[2] = '0' + numOfDegDigits;

    sprintf(buf, fmt, degrees, minutes, wsChars[negative ? 1 : 0]);

    return buf;
}

std::string formatLat(double lat) {
    return formatCoord(lat, 2, "NS");
}

std::string formatLon(double lon) {
    return formatCoord(lon, 3, "EW");
}

Ctx *getCtx(HWND wnd) {
    return reinterpret_cast<Ctx *>(GetWindowLongPtrA(wnd, GWLP_USERDATA));
}

