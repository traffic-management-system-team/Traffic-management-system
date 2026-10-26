// Basic Win32 GUI for the Traffic Management System (GUI foundation issue).
// Build: see BUILD_TESTING.md section "GUI foundation".
#include <windows.h>
#include <cstdio>
#include <cstring>
#include "gui_model.h"

namespace {

constexpr int ID_START = 1, ID_PAUSE = 2, ID_RESET = 3;
constexpr UINT_PTR ID_TIMER = 1;
constexpr UINT TICK_MS = 500;

SimModel g_model;
HWND g_pauseBtn = nullptr;

COLORREF signalColor(SimModel::Signal s) {
    switch (s) {
        case SimModel::Signal::Green:  return RGB(40, 180, 60);
        case SimModel::Signal::Yellow: return RGB(240, 200, 30);
        default:                       return RGB(210, 40, 40);
    }
}

void fillRect(HDC dc, int l, int t, int r, int b, COLORREF c) {
    HBRUSH br = CreateSolidBrush(c);
    RECT rc{l, t, r, b};
    FillRect(dc, &rc, br);
    DeleteObject(br);
}

void drawLight(HDC dc, int cx, int cy, COLORREF c) {
    HBRUSH br = CreateSolidBrush(c);
    HGDIOBJ old = SelectObject(dc, br);
    Ellipse(dc, cx - 10, cy - 10, cx + 10, cy + 10);
    SelectObject(dc, old);
    DeleteObject(br);
}

void drawScene(HDC dc) {
    fillRect(dc, 0, 0, 900, 600, RGB(235, 235, 235));
    fillRect(dc, 0, 260, 600, 340, RGB(70, 70, 70));   // horizontal road
    fillRect(dc, 260, 0, 340, 600, RGB(70, 70, 70));   // vertical road

    // Vehicles (queue state), max 8 drawn per lane; the count is printed as text.
    const COLORREF car = RGB(30, 90, 200);
    for (int r = 0; r < SimModel::kRoads; ++r) {
        const int n = static_cast<int>(g_model.queueLength(r));
        const int shown = n > 8 ? 8 : n;
        for (int i = 0; i < shown; ++i) {
            const int d = 10 + i * 22;
            switch (r) {
                case 0: fillRect(dc, 265, 260 - d - 18, 295, 260 - d, car); break;
                case 1: fillRect(dc, 340 + d, 265, 340 + d + 18, 295, car); break;
                case 2: fillRect(dc, 305, 340 + d, 335, 340 + d + 18, car); break;
                default: fillRect(dc, 260 - d - 18, 305, 260 - d, 335, car); break;
            }
        }
    }

    // Signals
    drawLight(dc, 245, 245, signalColor(g_model.signal(0)));
    drawLight(dc, 355, 245, signalColor(g_model.signal(1)));
    drawLight(dc, 355, 355, signalColor(g_model.signal(2)));
    drawLight(dc, 245, 355, signalColor(g_model.signal(3)));

    SetBkMode(dc, TRANSPARENT);
    const char* names[SimModel::kRoads] = {"North", "East", "South", "West"};
    const int lx[SimModel::kRoads] = {190, 400, 400, 190};
    const int ly[SimModel::kRoads] = {20, 230, 580, 370};
    char buf[96];
    for (int r = 0; r < SimModel::kRoads; ++r) {
        std::snprintf(buf, sizeof buf, "%s queue: %u", names[r],
                      static_cast<unsigned>(g_model.queueLength(r)));
        TextOutA(dc, lx[r], ly[r], buf, static_cast<int>(std::strlen(buf)));
    }

    // Right-hand panel
    const char* st = g_model.state() == SimModel::State::Running ? "Running"
                   : g_model.state() == SimModel::State::Paused  ? "Paused" : "Stopped";
    std::snprintf(buf, sizeof buf, "State: %s   Tick: %lu", st, g_model.tickCount());
    TextOutA(dc, 620, 150, buf, static_cast<int>(std::strlen(buf)));

    fillRect(dc, 620, 200, 880, 330, RGB(250, 250, 250));
    TextOutA(dc, 630, 210, "Statistics (placeholder)", 24);
    TextOutA(dc, 630, 235, "Tracked by Statistics Manager issue", 35);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE:
        CreateWindowA("BUTTON", "Start", WS_CHILD | WS_VISIBLE, 620, 20, 120, 30, hwnd,
                      reinterpret_cast<HMENU>(static_cast<INT_PTR>(ID_START)), nullptr, nullptr);
        g_pauseBtn = CreateWindowA("BUTTON", "Pause", WS_CHILD | WS_VISIBLE, 620, 60, 120, 30, hwnd,
                      reinterpret_cast<HMENU>(static_cast<INT_PTR>(ID_PAUSE)), nullptr, nullptr);
        CreateWindowA("BUTTON", "Reset", WS_CHILD | WS_VISIBLE, 620, 100, 120, 30, hwnd,
                      reinterpret_cast<HMENU>(static_cast<INT_PTR>(ID_RESET)), nullptr, nullptr);
        SetTimer(hwnd, ID_TIMER, TICK_MS, nullptr);
        return 0;
    case WM_COMMAND:
        switch (LOWORD(wp)) {
            case ID_START: g_model.start(); break;
            case ID_PAUSE: g_model.togglePause(); break;
            case ID_RESET: g_model.reset(); break;
            default: return DefWindowProcA(hwnd, msg, wp, lp);
        }
        SetWindowTextA(g_pauseBtn, g_model.state() == SimModel::State::Paused ? "Resume" : "Pause");
        InvalidateRect(hwnd, nullptr, FALSE);   // refresh after state change
        return 0;
    case WM_TIMER:
        if (g_model.tick()) InvalidateRect(hwnd, nullptr, FALSE);
        return 0;
    case WM_ERASEBKGND:
        return 1;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(hwnd, &ps);
        HDC mem = CreateCompatibleDC(dc);
        HBITMAP bmp = CreateCompatibleBitmap(dc, 900, 600);
        HGDIOBJ old = SelectObject(mem, bmp);
        drawScene(mem);
        BitBlt(dc, 0, 0, 900, 600, mem, 0, 0, SRCCOPY);   // double-buffered
        SelectObject(mem, old);
        DeleteObject(bmp);
        DeleteDC(mem);
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_DESTROY:
        KillTimer(hwnd, ID_TIMER);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcA(hwnd, msg, wp, lp);
}

} // namespace

int WINAPI WinMain(HINSTANCE inst, HINSTANCE, LPSTR, int show) {
    WNDCLASSA wc{};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = inst;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = "TrafficGuiWindow";
    RegisterClassA(&wc);

    RECT rc{0, 0, 900, 600};
    AdjustWindowRect(&rc, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE);
    HWND hwnd = CreateWindowA("TrafficGuiWindow", "Traffic Management System",
                              WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                              CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top,
                              nullptr, nullptr, inst, nullptr);
    if (!hwnd) return 1;
    ShowWindow(hwnd, show);

    MSG m;
    while (GetMessageA(&m, nullptr, 0, 0) > 0) {
        TranslateMessage(&m);
        DispatchMessageA(&m);
    }
    return static_cast<int>(m.wParam);
}