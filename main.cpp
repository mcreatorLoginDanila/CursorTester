#include <windows.h>
#include <vector>

HBRUSH hBrushBg;
HBRUSH hBrushZone;
HBRUSH hBrushHover;
HBRUSH hBrushBorder;

struct Zone {
    RECT rect;
    HCURSOR cursor;
    LPCSTR cursorName;
};

std::vector<Zone> zones;
int hoveredZoneIndex = -1;

void InitZones(int clientW, int clientH) {
    zones.clear();

    LPCSTR cursorIds[] = {
        IDC_ARROW,
        IDC_IBEAM,
        IDC_HAND,
        IDC_WAIT,
        IDC_APPSTARTING,
        IDC_CROSS,
        IDC_SIZEALL,
        IDC_NO,
        IDC_HELP,
        IDC_SIZENS,
        IDC_SIZEWE,
        IDC_SIZENWSE,
        IDC_SIZENESW
    };

    int cols = 4;
    int count = sizeof(cursorIds) / sizeof(cursorIds[0]);
    int rows = (count + cols - 1) / cols;

    int padding = 20;
    int blockW = (clientW - (cols + 1) * padding) / cols;
    int blockH = (clientH - (rows + 1) * padding) / rows;

    for (int i = 0; i < count; ++i) {
        int r = i / cols;
        int c = i % cols;

        Zone z;
        z.rect.left = padding + c * (blockW + padding);
        z.rect.top = padding + r * (blockH + padding);
        z.rect.right = z.rect.left + blockW;
        z.rect.bottom = z.rect.top + blockH;
        
        z.cursor = LoadCursor(NULL, cursorIds[i]);
        zones.push_back(z);
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE:
        hBrushBg = CreateSolidBrush(RGB(5, 5, 25));
        hBrushZone = CreateSolidBrush(RGB(20, 20, 60));
        hBrushHover = CreateSolidBrush(RGB(50, 50, 120));
        hBrushBorder = CreateSolidBrush(RGB(80, 80, 140));
        
        RECT rc;
        GetClientRect(hwnd, &rc);
        InitZones(rc.right, rc.bottom);
        return 0;

    case WM_SIZE:
        {
            int w = LOWORD(lParam);
            int h = HIWORD(lParam);
            InitZones(w, h);
            InvalidateRect(hwnd, NULL, TRUE);
        }
        return 0;

    case WM_MOUSEMOVE:
        {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            int newHover = -1;

            for (size_t i = 0; i < zones.size(); ++i) {
                if (x >= zones[i].rect.left && x <= zones[i].rect.right &&
                    y >= zones[i].rect.top && y <= zones[i].rect.bottom) {
                    newHover = (int)i;
                    break;
                }
            }

            if (newHover != hoveredZoneIndex) {
                hoveredZoneIndex = newHover;
                
                if (hoveredZoneIndex != -1) {
                    SetCursor(zones[hoveredZoneIndex].cursor);
                } else {
                    SetCursor(LoadCursor(NULL, IDC_ARROW));
                }

                InvalidateRect(hwnd, NULL, FALSE);
            }
        }
        return 0;

    case WM_SETCURSOR:
        if (LOWORD(lParam) == HTCLIENT) {
            if (hoveredZoneIndex != -1) {
                SetCursor(zones[hoveredZoneIndex].cursor);
            } else {
                SetCursor(LoadCursor(NULL, IDC_ARROW));
            }
            return TRUE;
        }
        break;

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            RECT clientRect;
            GetClientRect(hwnd, &clientRect);
            FillRect(hdc, &clientRect, hBrushBg);

            for (int i = 0; i < zones.size(); ++i) {
                HBRUSH currentBrush = (i == hoveredZoneIndex) ? hBrushHover : hBrushZone;
                
                FillRect(hdc, &zones[i].rect, currentBrush);
                
                FrameRect(hdc, &zones[i].rect, hBrushBorder);
            }

            EndPaint(hwnd, &ps);
        }
        return 0;

    case WM_ERASEBKGND:
        return 1;

    case WM_DESTROY:
        DeleteObject(hBrushBg);
        DeleteObject(hBrushZone);
        DeleteObject(hBrushHover);
        DeleteObject(hBrushBorder);
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char CLASS_NAME[] = "CursorTesterClass";

    WNDCLASS wc = { };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClass(&wc);

    int w = 800;
    int h = 600;
    int x = (GetSystemMetrics(SM_CXSCREEN) - w) / 2;
    int y = (GetSystemMetrics(SM_CYSCREEN) - h) / 2;

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "Cursor Tester",
        WS_OVERLAPPEDWINDOW,
        x, y, w, h,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) return 0;

    ShowWindow(hwnd, nCmdShow);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}