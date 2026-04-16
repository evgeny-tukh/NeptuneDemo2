#pragma once

#include <Windows.h>

#define NAV_PANEL_WIDTH 200
#define ID_UPDATE_TIMER 100

struct PositionData {
    double lat;
    double lon;
    double angle;
    double pitch;
    double scale;
    double bathyPeriod;
    int xCursor;
    int yCursor;
    int syncFrameNo;
};

struct NavData {
    double cog = 0.0;
    double sog = 0.0;
    double hdg = 0.0;
    double stw = 0.0;
    double lat = 0.0;
    double lon = 0.0;
    time_t utc = 0;
};

struct Ctx {
    HINSTANCE inst;
    HWND mainWnd;
    HWND chartPanel;
    HWND navPanel;
    HWND latInd;
    HWND lonInd;
    HWND curLatInd;
    HWND curLonInd;
    HWND centerLatInd;
    HWND centerLonInd;

    HWND chartViewerWnd;

    HANDLE chartViewer;

    UINT updateTimer;

    double lat = 60.0;
    double lon = 29.0;
    double centerLat = 60.5;
    double centerLon = 29.5;
    double curLat = 60.25;
    double curLon = 29.25;
};

void createChartPanel(Ctx& ctx);
void createNavPanel(Ctx& ctx);

