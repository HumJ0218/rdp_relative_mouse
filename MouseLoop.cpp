#include <windows.h>
#include <iostream>
#include <string>
#include "ScreenMetric.h"

ScreenMetric screenMetric{};
int boundaryLoopThreshold = 5;

LRESULT CALLBACK hookMouseLoop(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode < 0)
    {
        return CallNextHookEx(NULL, nCode, wParam, lParam);
    }

    if (wParam != WM_MOUSEMOVE)
    {
        return CallNextHookEx(NULL, nCode, wParam, lParam);
    }

    MSLLHOOKSTRUCT* pMouseStruct = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
    int x = pMouseStruct->pt.x;
    int y = pMouseStruct->pt.y;

    bool cross = false;
    int crossedX = x;
    int crossedY = y;

    if (x < boundaryLoopThreshold)
    {
        printf("left, ");

        cross = true;
        crossedX = screenMetric.xDeviceSize() - boundaryLoopThreshold * 2;
    }
    else if (x > screenMetric.xDeviceSize() - boundaryLoopThreshold)
    {
        printf("right, ");

        cross = true;
        crossedX = boundaryLoopThreshold * 2;
    }

    if (y < boundaryLoopThreshold)
    {
        printf("top, ");

        cross = true;
        crossedY = screenMetric.yDeviceSize() - boundaryLoopThreshold * 2;
    }
    else if (y > screenMetric.yDeviceSize() - boundaryLoopThreshold)
    {
        printf("bottom, ");

        cross = true;
        crossedY = boundaryLoopThreshold * 2;
    }

    if (cross)
    {
        SetCursorPos(screenMetric.xDeviceToSystem(crossedX), screenMetric.yDeviceToSystem(crossedY));
        // block the original mouse move, otherwise the cursor will be moved back
        printf("%d,%d -> %d,%d\n", x, y, crossedX, crossedY);
        return 1;
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int main(int argc, char* argv[])
{

    HHOOK hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, hookMouseLoop, NULL, 0);

    if (hMouseHook == NULL)
    {
        printf("Failed to install mouse hook\n");
        return 1;
    }
    else {
        printf("ready\n");
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hMouseHook);

    return 0;
}
