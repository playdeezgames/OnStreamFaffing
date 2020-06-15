#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static HINSTANCE hInst = NULL;
static const WCHAR* szWindowClass = L"PlumbingWindowClass";

static ATOM MyRegisterClass(HINSTANCE hInstance, WNDPROC WndProc)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = NULL;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = GetSysColorBrush(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = NULL;

    return RegisterClassExW(&wcex);
}

static BOOL InitInstance(HINSTANCE hInstance, int nCmdShow, LPCWSTR title, int width, int height)
{
    hInst = hInstance;

    RECT rcWindow;
    SetRect(&rcWindow, 0, 0, width, height);
    AdjustWindowRectEx(&rcWindow, WS_POPUPWINDOW | WS_CAPTION, FALSE, 0);

    HWND hWnd = CreateWindowW(szWindowClass, title, WS_POPUPWINDOW | WS_CAPTION, CW_USEDEFAULT, CW_USEDEFAULT, rcWindow.right - rcWindow.left, rcWindow.bottom - rcWindow.top, NULL, NULL, hInstance, NULL);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

int Plumbing_run(HINSTANCE hInstance, WNDPROC WndProc, int nCmdShow, LPCWSTR title, int width, int height)
{
    MyRegisterClass(hInstance, WndProc);

    if (!InitInstance(hInstance, nCmdShow, title, width, height))
    {
        return FALSE;
    }

    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;

}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
extern LPCWSTR gameTitle;
extern int gameClientWidth;
extern int gameClientHeight;

int APIENTRY WinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPTSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    return Plumbing_run(hInstance, WndProc, nCmdShow, gameTitle, gameClientWidth, gameClientHeight);
}
