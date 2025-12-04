// FinalExam.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "FinalExam.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_FINALEXAM, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FINALEXAM));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FINALEXAM));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_FINALEXAM);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//



BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//

RECT g_field = { 10, 10, 600, 600 };
// left=10, top=10, right=600, bottom=600
// PPT RECT 규칙 그대로 적용 (좌상단 → 우하단)

//플레이어
RECT g_player;
const int PLAYER_WIDTH = 60;
const int PLAYER_HEIGHT = 20;
const int PLAYER_SPEED = 20;

//장애물 크기
const int OBSTACLE_COUNT = 5;
RECT g_obstacles[OBSTACLE_COUNT];
const int OB_WIDTH = 40;
const int OB_HEIGHT = 20;
const int OB_SPEED = 5;

UINT_PTR ID_TIMER_GAME = 1;


// 필드 내부에서만 X 좌표 랜덤 생성
// PPT 기준: RECT 안에서 좌표 계산
int RandXInField()
{
    int minX = g_field.left;
    int maxX = g_field.right - OB_WIDTH;
    return minX + (rand() % (maxX - minX + 1));
}


// 게임 전체 초기화 — 플레이어/장애물 필드 안에 배치
void ResetGame(HWND hWnd)
{
    // 플레이어를 필드 하단 중앙에 배치
    int px = (g_field.left + g_field.right) / 2 - PLAYER_WIDTH / 2;
    int py = g_field.bottom - PLAYER_HEIGHT - 10;

    g_player.left = px;
    g_player.top = py;
    g_player.right = px + PLAYER_WIDTH;
    g_player.bottom = py + PLAYER_HEIGHT;

    // 장애물 초기화 — 필드 상단 밖에서 시작
    for (int i = 0; i < OBSTACLE_COUNT; i++)
    {
        int x = RandXInField();
        int y = g_field.top - (i * 120);

        g_obstacles[i].left = x;
        g_obstacles[i].top = y;
        g_obstacles[i].right = x + OB_WIDTH;
        g_obstacles[i].bottom = y + OB_HEIGHT;
    }

    InvalidateRect(hWnd, NULL, TRUE);
}

// 플레이어 목숨(라이프). 초기값 3으로 설정
int g_life = 3;


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
       
    case WM_CREATE:
    {
        srand((unsigned)GetTickCount());
        ResetGame(hWnd);
        SetTimer(hWnd, ID_TIMER_GAME, 30, NULL);  // 30ms마다 업데이트 (PPT Timer 활용)
    }
    break;


    // 키보드 입력 처리
    case WM_KEYDOWN:
    {
        // 왼쪽 이동
        if (wParam == VK_LEFT)
        {
            g_player.left -= PLAYER_SPEED;
            g_player.right -= PLAYER_SPEED;

            // 필드 왼쪽 경계 제한
            if (g_player.left < g_field.left)
            {
                int diff = g_field.left - g_player.left;
                g_player.left += diff;
                g_player.right += diff;
            }

            InvalidateRect(hWnd, NULL, TRUE);
        }

        // 오른쪽 이동
        else if (wParam == VK_RIGHT)
        {
            g_player.left += PLAYER_SPEED;
            g_player.right += PLAYER_SPEED;

            // 필드 오른쪽 경계 제한
            if (g_player.right > g_field.right)
            {
                int diff = g_player.right - g_field.right;
                g_player.left -= diff;
                g_player.right -= diff;
            }

            InvalidateRect(hWnd, NULL, TRUE);
        }
    }
    break;


    // 타이머 — 장애물 이동 + 충돌 체크
    case WM_TIMER:
    {
        if (wParam == ID_TIMER_GAME)
        {
            bool hit = false; // 충돌 여부 플래그

            for (int i = 0; i < OBSTACLE_COUNT; i++)
            {
                g_obstacles[i].top += OB_SPEED;
                g_obstacles[i].bottom += OB_SPEED;

                if (g_obstacles[i].top > g_field.bottom)
                {
                    int x = RandXInField();
                    int y = g_field.top - OB_HEIGHT;

                    g_obstacles[i].left = x;
                    g_obstacles[i].top = y;
                    g_obstacles[i].right = x + OB_WIDTH;
                    g_obstacles[i].bottom = y + OB_HEIGHT;
                }

                // ★ 충돌은 한 번만 처리하기 위해 hit == false 일 때만 검사
                if (!hit)
                {
                    RECT overlap;
                    if (IntersectRect(&overlap, &g_player, &g_obstacles[i]))
                    {
                        hit = true; // 이번 타이머에서는 다시 충돌 체크 금지
                    }
                }
            }

            // ★ for문 끝난 후 충돌을 한 번만 처리
            if (hit)
            {
                g_life--;

                if (g_life > 0)
                {
                    MessageBox(hWnd, L"충돌! 목숨이 1 줄었습니다!", L"Warning", MB_OK);
                }
                else
                {
                    MessageBox(hWnd, L"게임 오버!", L"Game Over", MB_OK);
                    g_life = 3; // 목숨 초기화
                }

                ResetGame(hWnd);
            }

            InvalidateRect(hWnd, NULL, TRUE);
        }
    }
    break;


    // 화면 그리기
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        RECT rc;
        GetClientRect(hWnd, &rc);
        FillRect(hdc, &rc, (HBRUSH)(COLOR_WINDOW + 1));

        // 필드 구역 테두리 표시
        Rectangle(hdc,
            g_field.left, g_field.top,
            g_field.right, g_field.bottom);

        // 플레이어 그리기
        Rectangle(hdc,
            g_player.left, g_player.top,
            g_player.right, g_player.bottom);

        // 장애물 그리기
        for (int i = 0; i < OBSTACLE_COUNT; i++)
        {
            Rectangle(hdc,
                g_obstacles[i].left, g_obstacles[i].top,
                g_obstacles[i].right, g_obstacles[i].bottom);
        }
        // 목숨 표시용 문자열 생성
        WCHAR buf[32];
        wsprintf(buf, L"LIFE : %d", g_life);

        // 화면 왼쪽 위에 텍스트 출력
        TextOut(hdc, 620, 20, buf, lstrlenW(buf));


        EndPaint(hWnd, &ps);
    }
    break;


    case WM_DESTROY:
        KillTimer(hWnd, ID_TIMER_GAME);
        PostQuitMessage(0);
        break;


    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

//////////////////////////////////////////////////
// ① 화면 그리기(GDI)
//   모든 그리기는 OS에게 요청하는 방식 (PPT 0909)
//////////////////////////////////////////////////

// • WM_PAINT 때만 화면 출력이 보장된다.
// • BeginPaint / EndPaint : 화면 갱신 시 HDC 획득/반납
// • GetDC / ReleaseDC : WM_PAINT 외부에서 HDC 사용
// • 점/선/면 그리기 API
//      MoveToEx(hdc, x, y, NULL)      → 선 시작점 설정
//      LineTo(hdc, x, y)              → 선 그리기
//      Rectangle(hdc, L, T, R, B)     → 사각형
//      Ellipse(hdc, L, T, R, B)       → 타원
// • HDC 속성 변경
//      SelectObject(hdc, pen/brush)
//      DeleteObject(obj)
//      → 새로운 펜/브러시 적용 후 원래 객체 복구 필요


//////////////////////////////////////////////////
// ② 입력 처리(마우스 / 키보드)
//   모든 입력은 메시지(WM_~)로 전달된다. (PPT 0916)
//////////////////////////////////////////////////

// • 마우스 메시지:
//      WM_LBUTTONDOWN, WM_LBUTTONUP, WM_MOUSEMOVE
// • 좌표 획득(lParam):
//      x = LOWORD(lParam)
//      y = HIWORD(lParam)
// • 키보드 메시지:
//      WM_KEYDOWN, WM_KEYUP
//      wParam = 눌린 키(VK_LEFT, VK_RIGHT 등)
// • 문자열 출력:
//      wsprintf(buf, L"%d", n)  → 문자열 생성
//      lstrlenW(buf)           → 문자열 길이
//      TextOut(hdc, x, y, buf, len)
// • 입력은 모두 콜백이므로 '전역 변수'로 상태 유지 필요


//////////////////////////////////////////////////
// ③ RECT(구역 관리) & 충돌 처리
//   좌표 기반 게임/그림판 기본 자료형 (PPT 0923, 0930)
//////////////////////////////////////////////////

// • RECT 구조체: left, top, right, bottom
//      반드시 left<right, top<bottom 유지해야 정상 동작
// • 이동할 때는 양쪽 값을 함께 이동해야 함
//      rect.left += dx; rect.right += dx;
//      rect.top  += dy; rect.bottom += dy;
// • 충돌(겹침) 판정 API:
//      IntersectRect(&dst, &r1, &r2)
//      → TRUE : 겹침 있음 (dst에 겹친 영역 반환)
//      → FALSE: 겹침 없음


//////////////////////////////////////////////////
// ④ Timer(주기적 실행)
//   게임 루프를 구현할 때 핵심 (PPT 0930)
//////////////////////////////////////////////////

// • SetTimer(hWnd, ID, ms, NULL)
//      → ms 간격으로 WM_TIMER 메시지를 발생
// • KillTimer(hWnd, ID)
//      → 타이머 제거
// • WM_TIMER:
//      wParam = 타이머 ID
// • 주기적 이동, 애니메이션, 게임 로직 업데이트에 사용


//////////////////////////////////////////////////
// ⑤ Thread(스레드)
//   새로운 실행 흐름 생성 (PPT 1028)
//////////////////////////////////////////////////

// • 스레드 함수 원형 (고정):
//      DWORD WINAPI ThreadFunc(LPVOID param);
// • 스레드 생성:
//      CreateThread(NULL, 0, ThreadFunc, param, 0, NULL);
// • 제어:
//      SuspendThread(hThread)
//      ResumeThread(hThread)
// • 목적:
//      동시에 움직이는 객체, 별도 작업 처리


//////////////////////////////////////////////////
// ⑥ Process(프로세스 생성/제어)
//   CreateProcess 를 통한 자식 프로세스 실행 (PPT 1118)
//////////////////////////////////////////////////

// • 주요 구조체:
//      STARTUPINFO si;
//      PROCESS_INFORMATION pi;
// • 프로세스 생성:
//      CreateProcess(path, ..., &si, &pi);
// • 프로세스 종료:
//      TerminateProcess(pi.hProcess, exitCode);
// • 핸들 해제:
//      CloseHandle(pi.hProcess);
//      CloseHandle(pi.hThread);
// • 윈도우는 1 Process = 기본적으로 1 Thread 실행


//////////////////////////////////////////////////
// ⑦ IPC(프로세스 간 통신) & 메시징
//   Win32 메시지 전달 방식 (PPT 1125)
//////////////////////////////////////////////////

