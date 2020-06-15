#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include <stdlib.h>

#define CELL_COLUMNS 40
#define CELL_ROWS 30
#define CELL_WIDTH 16
#define CELL_HEIGHT 16
#define SCREEN_WIDTH ((CELL_COLUMNS)*(CELL_WIDTH))
#define SCREEN_HEIGHT ((CELL_ROWS)*(CELL_HEIGHT))

int gameClientWidth = SCREEN_WIDTH;
int gameClientHeight = SCREEN_HEIGHT;
int soundEnabled = 1;
LPCWSTR gameTitle = L"Zig-Zag";
LPCWSTR turnSound = L"turn.wav";
LPCWSTR deathSound = L"death.wav";
LPCWSTR startMessage = L"Press <SPACE> to Start!";
LPCWSTR mutedMessage = L"MUTED";
LPCWSTR muteMessage = L"[M] to toggle sound";
LPCWSTR scoreFormat = L"Score: %d";
LPCWSTR highScoreFormat = L"Hi Score: %d";

#define TIMER_INTERVAL 50
#define TAIL_LENGTH 6

typedef enum {
    GAME_OVER,
    IN_PLAY
} GAMESTATE;

int blocks[CELL_ROWS] = { 0 };
int tail[TAIL_LENGTH] = { 0 };
int direction = 0;
int score = 0;
int highScore = 0;
int runLength = 0;
HBRUSH blackBrush = NULL;
HBRUSH whiteBrush = NULL;
HBRUSH blueBrush = NULL;
HBRUSH redBrush = NULL;
HBRUSH yellowBrush = NULL;
UINT_PTR timer = 0;
GAMESTATE gameState = GAME_OVER;

void RequestSound(LPCWSTR sound)
{
    if (soundEnabled)
    {
        PlaySoundW(sound, NULL, SND_ASYNC | SND_FILENAME);
    }
}

void RedrawScreen(HDC hdc)
{
    RECT rc = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };
    FillRect(hdc, &rc, blackBrush);

    for (int row = 0; row < TAIL_LENGTH; ++row)
    {
        SetRect(&rc, CELL_WIDTH * tail[row], CELL_HEIGHT * row, CELL_WIDTH * tail[row] + CELL_WIDTH, CELL_HEIGHT * row + CELL_HEIGHT);
        FillRect(hdc, &rc, (row < TAIL_LENGTH - 1) ? (yellowBrush) : (redBrush));
    }

    for (int row = 0; row < CELL_ROWS; ++row)
    {
        SetRect(&rc, CELL_WIDTH * blocks[row], CELL_HEIGHT * row, CELL_WIDTH * blocks[row] + CELL_WIDTH, CELL_HEIGHT * row + CELL_HEIGHT);
        FillRect(hdc, &rc, whiteBrush);
    }

    SetRect(&rc, 0, 0, CELL_WIDTH, SCREEN_HEIGHT);
    FillRect(hdc, &rc, blueBrush);
    SetRect(&rc, SCREEN_WIDTH - CELL_WIDTH, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    FillRect(hdc, &rc, blueBrush);

    highScore = (score > highScore) ? (score) : highScore;

    SetTextColor(hdc, (score==highScore) ? (RGB(0, 255, 255)) : (RGB(0, 255, 0)));
    SetBkColor(hdc, RGB(0, 0, 0));

    SetRect(&rc, CELL_WIDTH, 0, SCREEN_WIDTH/2, CELL_HEIGHT);
    WCHAR buffer[50];
    wsprintfW(buffer, scoreFormat, score);
    DrawTextW(hdc, buffer, -1, &rc, DT_LEFT | DT_VCENTER);

    SetRect(&rc, SCREEN_WIDTH / 2, 0, SCREEN_WIDTH - CELL_WIDTH, CELL_HEIGHT);
    wsprintfW(buffer, highScoreFormat, highScore);
    DrawTextW(hdc, buffer, -1, &rc, DT_RIGHT | DT_VCENTER);

    if (gameState == GAME_OVER)
    {
        SetTextColor(hdc, RGB(128, 0, 128));
        SetRect(&rc, 0, SCREEN_HEIGHT-CELL_HEIGHT, SCREEN_WIDTH , SCREEN_HEIGHT);
        DrawTextW(hdc, startMessage, -1, &rc, DT_CENTER | DT_VCENTER);
        SetRect(&rc, 0, SCREEN_HEIGHT - CELL_HEIGHT * 2, SCREEN_WIDTH, SCREEN_HEIGHT - CELL_HEIGHT);
        DrawTextW(hdc, muteMessage, -1, &rc, DT_CENTER | DT_VCENTER);
        if (!soundEnabled)
        {
            SetRect(&rc, 0, SCREEN_HEIGHT - CELL_HEIGHT * 3, SCREEN_WIDTH, SCREEN_HEIGHT - CELL_HEIGHT * 2);
            DrawTextW(hdc, mutedMessage, -1, &rc, DT_CENTER | DT_VCENTER);
        }
    }

}

void UpdateScreen(HWND hWnd)
{
    if (gameState == IN_PLAY)
    {
        for (int row = 0; row < CELL_ROWS - 1; ++row)
        {
            blocks[row] = blocks[row + 1];
        }
        blocks[CELL_ROWS - 1] = 1 + (rand() % (CELL_COLUMNS - 2));
        for (int row = 0; row < TAIL_LENGTH - 1; ++row)
        {
            tail[row] = tail[row + 1];
        }
        tail[TAIL_LENGTH - 1] += direction;
        if (tail[TAIL_LENGTH - 1] == 0 || tail[TAIL_LENGTH - 1] == CELL_COLUMNS - 1 || tail[TAIL_LENGTH - 1] == blocks[TAIL_LENGTH - 1])
        {
            RequestSound(deathSound);
            gameState = GAME_OVER;
        }
        runLength++;
    }
    InvalidateRect(hWnd, NULL, FALSE);
}

void CreateBrushes()
{
    blackBrush = CreateSolidBrush(RGB(0, 0, 0));
    whiteBrush = CreateSolidBrush(RGB(255, 255, 255));
    blueBrush = CreateSolidBrush(RGB(0, 0, 255));
    redBrush = CreateSolidBrush(RGB(255, 0, 0));
    yellowBrush = CreateSolidBrush(RGB(255, 255, 0));
}

void ResetGame()
{
    for (int row = 0; row < TAIL_LENGTH; ++row)
    {
        tail[row] = CELL_COLUMNS / 2;
    }

    for (int row = 0; row < CELL_ROWS; ++row)
    {
        blocks[row] = 0;
    }

    score = 0;
    runLength = 0;
}

void InitScreen(HWND hWnd)
{
    CreateBrushes();

    timer = SetTimer(hWnd, 1, TIMER_INTERVAL, NULL);
    ResetGame();
}

void CleanUpScreen(HWND hWnd)
{
    if (blackBrush)
    {
        DeleteObject(blackBrush);
        blackBrush = NULL;
    }
    if (whiteBrush)
    {
        DeleteObject(whiteBrush);
        whiteBrush = NULL;
    }
    if (blueBrush)
    {
        DeleteObject(blueBrush);
        blueBrush = NULL;
    }
    if (redBrush)
    {
        DeleteObject(redBrush);
        redBrush = NULL;
    }
    if (yellowBrush)
    {
        DeleteObject(yellowBrush);
        yellowBrush = NULL;
    }
    if (timer)
    {
        KillTimer(hWnd, timer);
        timer = 0;
    }
}

void StartGame()
{
    for (int row = 0; row < CELL_ROWS; ++row)
    {
        blocks[row] = 0;
    }
    for (int row = 0; row < TAIL_LENGTH; ++row)
    {
        tail[row] = CELL_COLUMNS / 2;
    }
    direction = 1;
    score = 0;
    runLength = 0;
    gameState = IN_PLAY;
}

void ScoreRun()
{
    WCHAR buffer[20];
    wsprintfW(buffer, L"%d", runLength);
    OutputDebugStringW(buffer);
    score += (runLength * (runLength + 1) / 2);
    runLength = 0;
}

void HandleKey(WPARAM wParam)
{
    if (gameState == IN_PLAY)
    {
        if (wParam == VK_LEFT && direction!=-1)
        {
            RequestSound(turnSound);
            direction = -1;
            ScoreRun();
        }
        else if (wParam == VK_RIGHT && direction!=1)
        {
            RequestSound(turnSound);
            direction = 1;
            ScoreRun();
        }
    }
    else
    {
        if (wParam == VK_SPACE)
        {
            StartGame();
        }
        else if (wParam == 'M')
        {
            soundEnabled = !soundEnabled;
        }
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        InitScreen(hWnd);
        break;
    case WM_TIMER:
        UpdateScreen(hWnd);
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            RedrawScreen(hdc);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        CleanUpScreen(hWnd);
        PostQuitMessage(0);
        break;
    case WM_KEYDOWN:
        HandleKey(wParam);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
