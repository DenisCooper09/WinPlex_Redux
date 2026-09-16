#define UNICODE
#define _UNICODE

#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>

static const wchar_t WINPLEX_CLASS_NAME[] = L"WINPLEX_REDUX";

static ID3D11Device *          g_Device{nullptr};
static ID3D11DeviceContext *   g_Context{nullptr};
static IDXGISwapChain *        g_SwapChain{nullptr};
static ID3D11RenderTargetView *g_RenderTarget{nullptr};

bool InitD3D11(HWND hwnd, LONG width, LONG height)
{
    DXGI_SWAP_CHAIN_DESC scd{};
    scd.BufferCount                        = 1;
    scd.BufferDesc.Width                   = width;
    scd.BufferDesc.Height                  = height;
    scd.BufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferDesc.RefreshRate.Numerator   = 60;
    scd.BufferDesc.RefreshRate.Denominator = 1;
    scd.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow                       = hwnd;
    scd.SampleDesc.Count                   = 1;
    scd.SampleDesc.Quality                 = 0;
    scd.Windowed                           = TRUE;
    scd.SwapEffect                         = DXGI_SWAP_EFFECT_DISCARD;

    UINT create_flags{0};
#ifdef WINPLEX_DEBUG
    create_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL feature_level{};

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        create_flags,
        nullptr, 0,
        D3D11_SDK_VERSION,
        &scd,
        &g_SwapChain,
        &g_Device,
        &feature_level,
        &g_Context
    );

    if (FAILED(hr))
    {
        MessageBox(nullptr, L"D3D11CreateDeviceAndSwapChain failed.", WINPLEX_CLASS_NAME, MB_ICONERROR | MB_OK);
        return false;
    }

    ID3D11Texture2D *back_buffer{nullptr};
    g_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&back_buffer));
    g_Device->CreateRenderTargetView(back_buffer, nullptr, &g_RenderTarget);
    back_buffer->Release();

    g_Context->OMSetRenderTargets(1, &g_RenderTarget, nullptr);

    D3D11_VIEWPORT viewport{};
    viewport.Width    = static_cast<float>(width);
    viewport.Height   = static_cast<float>(height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    g_Context->RSSetViewports(1, &viewport);

    return true;
}

void CleanupD3D11()
{
    if (g_RenderTarget) g_RenderTarget->Release();
    if (g_SwapChain) g_SwapChain->Release();
    if (g_Context) g_Context->Release();
    if (g_Device) g_Device->Release();
}

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
    WNDCLASSEX wc{};
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wc.lpszClassName = WINPLEX_CLASS_NAME;

    if (!RegisterClassEx(&wc))
    {
        MessageBox(nullptr, L"Window class registration failed.", WINPLEX_CLASS_NAME, MB_ICONERROR | MB_OK);
        return -1;
    }

    RECT  rect{0, 0, 1280, 720};
    DWORD style = WS_OVERLAPPEDWINDOW;
    AdjustWindowRect(&rect, style, FALSE);

    auto width  = rect.right - rect.left;
    auto height = rect.bottom - rect.top;

    HWND hwnd = CreateWindowEx(
        0,
        WINPLEX_CLASS_NAME,
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
        MessageBox(nullptr, L"Failed to create window.", WINPLEX_CLASS_NAME, MB_ICONERROR | MB_OK);
        return -1;
    }

    ShowWindow(hwnd, show);
    UpdateWindow(hwnd);

    if (!InitD3D11(hwnd, width, height))
        return -1;

    MSG  msg{};
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

        constexpr float clear_color[4] = {0.1f, 0.1f, 0.2f, 1.0f};
        g_Context->ClearRenderTargetView(g_RenderTarget, clear_color);
        g_SwapChain->Present(1, 0);
    }

    CleanupD3D11();
    return 0;
}
