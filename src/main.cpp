#include <Windows.h>
#include <Shlwapi.h>

#include <cstdint>

#include "resource.h"
#include "defs.h"
#include "tools.h"
#include "neptune_mgmt.h"

namespace {
const char *clsName { "NdMainWnd" } ;
const char *exitRequestText { "Do you really want to exit?" };
const char *exitRequestTitle { "Exit application" };

bool confirmExit(HWND wnd) {
    return MessageBoxA(wnd, exitRequestText, exitRequestTitle, MB_YESNO | MB_ICONQUESTION) == IDYES;
}

HWND createLabeledIndicator(Ctx& ctx, const char *label, int y, uint16_t id) {
    CreateWindowA("STATIC", label, WS_CHILD|WS_VISIBLE|SS_SIMPLE, 10, y + 2, 50, 14, ctx.navPanel, (HMENU) -1, ctx.inst, NULL);
    
    return CreateWindowA("EDIT", "000.0", WS_CHILD|WS_VISIBLE|WS_BORDER|ES_READONLY, 60, y, 100, 20, ctx.navPanel, (HMENU) id, ctx.inst, NULL);
}

void onCreate(HWND wnd, LPARAM lp) {
    auto cs = reinterpret_cast<CREATESTRUCTA *>(lp);
    auto ctx = reinterpret_cast<Ctx *>(cs->lpCreateParams);

    ctx->mainWnd = wnd;

    SetWindowLongPtrA(wnd, GWLP_USERDATA, (LONG_PTR) ctx);

    createChartPanel(*ctx);
    createNavPanel(*ctx);

    ctx->latInd = createLabeledIndicator(*ctx, "Lat", 30, IDC_LAT);
    ctx->lonInd = createLabeledIndicator(*ctx, "Lon", 60, IDC_LON);
    ctx->curLatInd = createLabeledIndicator(*ctx, "Cur lat", 90, IDC_LAT);
    ctx->curLonInd = createLabeledIndicator(*ctx, "Cur lon", 120, IDC_LON);
    ctx->centerLatInd = createLabeledIndicator(*ctx, "Cnt lat", 150, IDC_LAT);
    ctx->centerLonInd = createLabeledIndicator(*ctx, "Cnt lon", 180, IDC_LON);
}

void onCommand (HWND wnd, uint16_t cmd) {
    switch (cmd) {
        case IDM_EXIT:
            if (confirmExit(wnd))
                DestroyWindow(wnd);

            break;
    }
}

void onTimer(HWND wnd) {
    auto ctx = getCtx(wnd);
}

void onSize(HWND wnd, uint16_t width, uint16_t height) {
    auto ctx = getCtx(wnd);

    MoveWindow(ctx->chartPanel, 0, 0, width - NAV_PANEL_WIDTH, height, true);
    MoveWindow(ctx->navPanel, width - NAV_PANEL_WIDTH, 0, NAV_PANEL_WIDTH, height, true);
}

LRESULT wndProc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_COMMAND:
            onCommand(wnd, LOWORD(wp));
            break;

        case WM_SIZE:
            onSize(wnd, LOWORD(lp), HIWORD(lp));
            break;

        case WM_CREATE:
            onCreate(wnd, lp);
            break;

        case WM_TIMER:
            onTimer(wnd);
            break;

        case WM_SYSCOMMAND:
            if (wp == SC_CLOSE) {
                if (!confirmExit(wnd))
                    return 0;
            }
            
        default:
            return DefWindowProcA(wnd, msg, wp, lp);
    }

    return 1L;
}

bool registerClass(HINSTANCE inst) {
    WNDCLASSA wndClass;

    memset(&wndClass, 0, sizeof(wndClass));

    wndClass.hbrBackground = GetSysColorBrush(COLOR_WINDOW);
    wndClass.hCursor = LoadCursorA(NULL, IDC_ARROW);
    wndClass.hIcon = LoadIconA(NULL, IDI_APPLICATION);
    wndClass.hInstance = inst;
    wndClass.lpfnWndProc = wndProc;
    wndClass.lpszClassName = clsName;
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpszMenuName = MAKEINTRESOURCEA(IDR_MAINMENU);

    return RegisterClassA(&wndClass);
}

}

int WinMain(
    HINSTANCE inst,
    HINSTANCE prevInst,
    char *cmdLine,
    int showCmd
) {
    if (!registerClass(inst))
        return 1;

    Ctx ctx;
    ctx.inst = inst;
    ctx.chartViewer = runChartViewer(ctx);

    HDC screenDC = GetDC(HWND_DESKTOP);
    int screenWidth = GetDeviceCaps(screenDC, HORZRES);
    int screenHeight = GetDeviceCaps(screenDC, VERTRES);
    ReleaseDC(HWND_DESKTOP, screenDC);

    auto mainWnd = CreateWindowA(clsName, "Neptune Demo", WS_OVERLAPPEDWINDOW|WS_VISIBLE, 0, 0, screenWidth, screenHeight, HWND_DESKTOP, NULL, inst, &ctx);

    ShowWindow(mainWnd, SW_SHOWMAXIMIZED);
    UpdateWindow(mainWnd);
    InvalidateRect(mainWnd, NULL, TRUE);

    MSG msg;

    while (GetMessageA(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    TerminateProcess(ctx.chartViewer, 0);

    return 0;
}