#include <Windows.h>
#include <Shlwapi.h>

#include "defs.h"
#include "tools.h"

namespace {

const char *stopEventName { "guiReaderStopEvent" };
const char *closeEventName { "guiReaderCloseEvent" };
const char *chartViewerName { "ChartViewer.exe" };

const char *latPrefix { "lat=\"" };
const char *lonPrefix { "lon=\"" };
const char *anglePrefix { "angle=\"" };
const char *pitchPrefix { "pitch=\"" };
const char *scalePrefix { "scale=\"" };
const char *bathyPeriodPrefix { "bathyperiod=\"" };
const char *xCursorPrefix { "xcursor=\"" };
const char *yCursorPrefix { "ycursor=\"" };
const char *frameSyncNoPrefix { "synNumberCadr=\"" };

BOOL CALLBACK enumProc(HWND wnd, LPARAM param) {
    if (GetParent(wnd) == HWND_DESKTOP) {
        *((HWND *) param) = wnd;
        return false;
    }

    return true;
}

}

HANDLE runChartViewer(Ctx& ctx) {
    char cvPath[MAX_PATH];
    GetModuleFileNameA(nullptr, cvPath, sizeof(cvPath));
    PathRemoveFileSpecA(cvPath);
    PathAppendA(cvPath, chartViewerName);

    STARTUPINFOA siInfo;
    PROCESS_INFORMATION piInfo;
    memset(&siInfo, 0, sizeof(siInfo));
    memset(&piInfo, 0, sizeof(piInfo));
    siInfo.cb = sizeof(siInfo);
    siInfo.wShowWindow = SW_SHOWMAXIMIZED;
    siInfo.dwFlags = STARTF_PREVENTPINNING | STARTF_FORCEOFFFEEDBACK | STARTF_UNTRUSTEDSOURCE | STARTF_USESHOWWINDOW;

    if (!CreateProcessA(cvPath, "2", nullptr, nullptr, false, 0, nullptr, nullptr, &siInfo, &piInfo))
        return nullptr;

    Sleep(1000);

    ctx.chartViewerWnd = nullptr;

    do {
        EnumThreadWindows(piInfo.dwThreadId, enumProc, (LPARAM) &ctx.chartViewerWnd);
        Sleep (100);
    } while (!ctx.chartViewerWnd);

    #if 0
    RECT cvRect;
    GetWindowRect(ctx.chartViewerWnd, &cvRect);
    MoveWindow(ctx.chartViewerWnd, 3000, 0, cvRect.right, cvRect.bottom, TRUE);
    #endif
    CloseHandle(piInfo.hThread);

    return piInfo.hProcess;
}

void dispatchMouseMsgToChartViewer(HWND wnd, UINT msg, WPARAM wp, LPARAM lp) {
    auto ctx = getCtx(wnd);

    SendMessage(ctx->chartViewerWnd, msg, wp, lp);
}

void queryPositionFilePathName(char *buffer, size_t size) {
    GetModuleFileNameA(nullptr, buffer, size);
    PathRemoveFileSpecA(buffer);
    PathAppendA(buffer, "service\\position.cmu");
}

bool extractDoubleValue(const std::string& content, const char *prefix, double& value, size_t& startPos) {
    startPos = content.find(prefix, startPos);

    if (startPos == std::string::npos)
        return false;

    startPos += strlen(prefix);
    value = atof(content.c_str() + startPos);

    return true;
}

bool extractIntValue(const std::string& content, const char *prefix, int& value, size_t& startPos) {
    startPos = content.find(prefix, startPos);

    if (startPos == std::string::npos)
        return false;

    startPos += strlen(prefix);
    value = atoi(content.c_str() + startPos);

    return true;
}

bool readPositionData(PositionData& data) {
    char filePath[MAX_PATH];

    queryPositionFilePathName(filePath, sizeof(filePath));

    if (PathFileExistsA(filePath)) {
        HANDLE file = CreateFileA(filePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, 0, nullptr);

        if (file == INVALID_HANDLE_VALUE)
            return false;

        auto size = GetFileSize(file, nullptr);

        std::string content;
        content.resize(size);

        unsigned long bytesRead;

        bool result = ReadFile(file, (void *) content.data(), size, &bytesRead, nullptr);

        CloseHandle(file);

        if (!result || bytesRead != size)
            return false;

        size_t pos = 0;

        return (
            extractDoubleValue(content, latPrefix, data.lat, pos) &&
            extractDoubleValue(content, lonPrefix, data.lon, pos) &&
            extractDoubleValue(content, anglePrefix, data.angle, pos) &&
            extractDoubleValue(content, pitchPrefix, data.pitch, pos) &&
            extractDoubleValue(content, scalePrefix, data.scale, pos) &&
            extractDoubleValue(content, bathyPeriodPrefix, data.bathyPeriod, pos) &&
            extractIntValue(content, xCursorPrefix, data.xCursor, pos) &&
            extractIntValue(content, yCursorPrefix, data.yCursor, pos) &&
            extractIntValue(content, frameSyncNoPrefix, data.syncFrameNo, pos)
        );
    }

    return false;
}

bool writePositionData(PositionData& data) {
    return true;
}

