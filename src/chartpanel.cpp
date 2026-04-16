#include <Windows.h>

#include <cstdint>

#include "defs.h"
#include "tools.h"
#include "neptune_mgmt.h"
#include "resource.h"

namespace {
const char *clsName { "npDemoChartWnd" } ;

void onCreate(HWND wnd, LPARAM lp) {
    auto cs = reinterpret_cast<CREATESTRUCTA *>(lp);
    auto ctx = reinterpret_cast<Ctx *>(cs->lpCreateParams);

    SetWindowLongPtrA(wnd, GWLP_USERDATA, (LONG_PTR) ctx);
}

void onSize(HWND wnd, uint16_t width, uint16_t height) {
    #if 0
    auto ctx = getCtx(wnd);
    RECT wndRect;
    RECT clientRect;
    GetWindowRect(wnd, &wndRect);
    GetClientRect(wnd, &clientRect);

    POINT topLeft;
    topLeft.x = wndRect.left;
    topLeft.y = wndRect.top;
    ClientToScreen(wnd, &topLeft);
    MoveWindow(ctx->chartViewerWnd, topLeft.x, topLeft.y, width, height, TRUE);
    #endif
}

void onPaint(HWND wnd) {
    auto ctx = getCtx(wnd);

    PAINTSTRUCT ps;
    HDC paintDC = BeginPaint(wnd, &ps);
    HDC viewerDC = GetDC(ctx->chartViewerWnd);
    unsigned long err = GetLastError();
    RECT client;
    RECT cvClient;

    GetClientRect(wnd, &client);
    GetClientRect(ctx->chartViewerWnd, &cvClient);
    int centerX = cvClient.right >> 1;
    int centerY = cvClient.bottom >> 1;
    BitBlt(paintDC, 0, 0, client.right, client.bottom, viewerDC, centerX - (client.right >> 1), centerY - (client.bottom >> 1), SRCCOPY);

    ReleaseDC(ctx->chartViewerWnd, viewerDC);
    EndPaint(wnd, &ps);
}

void followChartViewer(HWND wnd) {
    PositionData posData;
    auto ctx = getCtx(wnd);

    readPositionData(posData);

    ctx->centerLat = rad2deg(mercator2Lat(posData.lat));
    ctx->centerLon = rad2deg(posData.lon);
}

LRESULT wndProc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_SIZE:
            onSize(wnd, LOWORD(lp), HIWORD(lp));
            break;

        case WM_CREATE:
            onCreate(wnd, lp);
            break;

        case WM_PAINT:
            onPaint(wnd);
            break;

        default:
            if (msg >= WM_MOUSEFIRST && msg <= WM_MOUSELAST) {
                dispatchMouseMsgToChartViewer(wnd, msg, wp, lp);
                followChartViewer(wnd);
                InvalidateRect(wnd, NULL, TRUE);
                break;
            }

            return DefWindowProcA(wnd, msg, wp, lp);
    }

    return 1L;
}


bool registerClass(Ctx& ctx) {
    WNDCLASSA wndClass;

    memset(&wndClass, 0, sizeof(wndClass));

    wndClass.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
    wndClass.hCursor = LoadCursorA(NULL, IDC_ARROW);
    wndClass.hIcon = LoadIconA(NULL, IDI_APPLICATION);
    wndClass.hInstance = ctx.inst;
    wndClass.lpfnWndProc = wndProc;
    wndClass.lpszClassName = clsName;
    wndClass.style = CS_HREDRAW | CS_VREDRAW;

    return RegisterClassA(&wndClass);
}
}

void createChartPanel(Ctx& ctx) {
    RECT parentClient;

    GetClientRect(ctx.mainWnd, &parentClient);
    registerClass(ctx);

    ctx.chartPanel = CreateWindowA(
        clsName,
        NULL,
        WS_CHILD|WS_VISIBLE,
        0,
        0,
        parentClient.right - NAV_PANEL_WIDTH,
        parentClient.bottom,
        ctx.mainWnd,
        (HMENU) ID_CHARTWINDOW,
        ctx.inst,
        &ctx
    );
}
