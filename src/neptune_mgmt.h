#pragma once

#include <Windows.h>

#include "defs.h"

HANDLE runChartViewer(Ctx& ctx);
void dispatchMouseMsgToChartViewer(HWND wnd, UINT msg, WPARAM wp, LPARAM lp);
bool readPositionData(PositionData& data);
bool writePositionData(PositionData& data);
