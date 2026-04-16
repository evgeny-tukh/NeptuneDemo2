#include <Windows.h>

#include "defs.h"
#include "resource.h"
#include "defs.h"
#include "tools.h"

namespace {
const char *clsName { "npDemoNavPanel" } ;

void updateData(Ctx *ctx) {
    SetWindowTextA(ctx->latInd, formatLat(ctx->lat).c_str());
    SetWindowTextA(ctx->lonInd, formatLat(ctx->lon).c_str());
    SetWindowTextA(ctx->centerLatInd, formatLat(ctx->centerLat).c_str());
    SetWindowTextA(ctx->centerLonInd, formatLat(ctx->centerLon).c_str());
    SetWindowTextA(ctx->curLatInd, formatLat(ctx->curLat).c_str());
    SetWindowTextA(ctx->curLonInd, formatLat(ctx->curLon).c_str());
}

void onCreate(HWND wnd, LPARAM lp) {
    auto cs = reinterpret_cast<CREATESTRUCTA *>(lp);
    auto ctx = reinterpret_cast<Ctx *>(cs->lpCreateParams);

    SetWindowLongPtrA(wnd, GWLP_USERDATA, (LONG_PTR) ctx);
}

LRESULT wndProc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_TIMER:
            updateData(getCtx(wnd));
            break;
        case WM_CREATE:
            onCreate(wnd, lp);
            break;

        default:
            return DefWindowProcA(wnd, msg, wp, lp);
    }

    return 1L;
}

bool registerClass(Ctx& ctx) {
    WNDCLASSA wndClass;

    memset(&wndClass, 0, sizeof(wndClass));

    wndClass.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
    wndClass.hCursor = LoadCursorA(NULL, IDC_ARROW);
    wndClass.hIcon = LoadIconA(NULL, IDI_APPLICATION);
    wndClass.hInstance = ctx.inst;
    wndClass.lpfnWndProc = wndProc;
    wndClass.lpszClassName = clsName;
    wndClass.style = CS_HREDRAW | CS_VREDRAW;

    return RegisterClassA(&wndClass);
}

}

void createNavPanel(Ctx& ctx) {
    RECT parentClient;

    GetClientRect(ctx.mainWnd, &parentClient);
    registerClass(ctx);

    ctx.navPanel = CreateWindowA(
        clsName,
        NULL,
        WS_CHILD|WS_VISIBLE,
        parentClient.right - NAV_PANEL_WIDTH,
        0,
        NAV_PANEL_WIDTH,
        parentClient.bottom,
        ctx.mainWnd,
        (HMENU) ID_NAVPANEL,
        ctx.inst,
        &ctx
    );

    ctx.updateTimer = SetTimer(ctx.navPanel, ID_UPDATE_TIMER, 500, NULL);
}
