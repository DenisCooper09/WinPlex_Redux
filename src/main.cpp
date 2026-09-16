#define UNICODE
#define _UNICODE

#include <windows.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE)
            {
                PostQuitMessage(0);
                return 0;
            }
            break;
        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prev_instance, LPSTR cmd_line, int show)
{
    const wchar_t CLASS_NAME[] = L"WINPLEX_REDUX";

    WNDCLASSEX wc{};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.lpszClassName = CLASS_NAME;

    if (!RegisterClassEx(&wc))
    {
        MessageBox(nullptr, L"Window class registration failed.", CLASS_NAME, MB_ICONERROR | MB_OK);
        return -1;
    }

    RECT rect{0, 0, 1280, 720};
    DWORD style = WS_OVERLAPPEDWINDOW;
    AdjustWindowRect(&rect, style, FALSE);

    auto width = rect.right - rect.left;
    auto height = rect.bottom - rect.top;

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"WinPlex Redux",
        style,
        CW_USEDEFAULT, CW_USEDEFAULT,
        width, height,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (!hwnd)
    {
        MessageBox(nullptr, L"Failed to create window.", CLASS_NAME, MB_ICONERROR | MB_OK);
        return -1;
    }

    ShowWindow(hwnd, show);
    UpdateWindow(hwnd);

    MSG msg{};
    bool running = true;
    while (running)
    {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                running = false;
                break;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return 0;
}
