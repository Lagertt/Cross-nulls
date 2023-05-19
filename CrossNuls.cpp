#pragma comment (lib, "Msimg32.lib")
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <windowsx.h>
#include <iostream>
#include <fstream>
#include <wingdi.h>

using namespace std;

#define KEY_SHIFTED     0x8000
#define KEY_C           0x43
#define KEY_SHIFT       0x10
#define KEY_ESC         0x1B
#define KEY_ENTER       0x0D
#define KEY_CTRL        0x11
#define KEY_Q           0x51

#define KEY_1           0x61
#define KEY_2           0x62
#define KEY_3           0x63
#define KEY_4           0x64
#define KEY_5           0x65
#define KEY_6           0x66
#define KEY_7           0x67

TCHAR szName[] = TEXT("Global\MyFileMappingObject");

const TCHAR szWinClass[] = _T("Win32SampleApp");
const TCHAR szWinName[] = _T("Win32SampleWindow");
HWND hwnd;               /* This is the handle for our window */
HBRUSH hBrush, newBrush;           /* Current brush */
int n = 3; // ������ ����� �� ���������
int CLred, CLgreen, CLblue, CBred, CBgreen, CBblue; // ����� ����� (CL) � ���� (CB)
bool config = false;

UINT msgnum;
INT* map{}; //������ � ��������� � �������� (����� ������)
int user = 1; //����� ������������
int step = 1; //��� � ����

HANDLE hDraw;	//����� ���������� 
HPEN hPen, Pen;

HANDLE mutex; 

HANDLE potok;


bool issuspended = false;	//� ��� ���������
bool draw_running;	//���� ���������

bool resume_thread = true;

/* Runs Notepad */
void RunNotepad(void)
{
    STARTUPINFO sInfo;
    PROCESS_INFORMATION pInfo;

    ZeroMemory(&sInfo, sizeof(STARTUPINFO));

    CreateProcess(_T("C:\\Windows\\Notepad.exe"),
        NULL, NULL, NULL, FALSE, 0, NULL, NULL, &sInfo, &pInfo);
}

//�������� ��������� ����
int check_field()
{
    int winner = 0;

    //�������� �� ��������    
    for (int i = 0; i < n; i++)
    {
        int first = 0;
        bool flag = true;
        for (int j = 0; j < n; j++)
        {
            if (j == 0)
            {
                first = map[i * n + j]; //���������� ������ �������
            }
            else
            {
                if (map[i * n + j] != first || map[i * n + j] == 0) //���� ���������� �������� �� ������� (��� ������), �� ����
                    flag = false;
            }
        }
        if (flag == true)
        {
            if (first == 1) //���� ���� ������� �� �����, �� ������� ����� ����� 2
                winner = 2;
            else
                winner = 1;
            break;
        }
    }

    //�������� �� �������
    for (int j = 0; j < n; j++)
    {
        int first = 0;
        bool flag = true;
        for (int i = 0; i < n; i++)
        {
            if (i == 0)
            {
                first = map[i * n + j]; //���������� ������ �������
            }
            else
            {
                if (map[i * n + j] != first || map[i * n + j] == 0) //���� ���������� �������� �� ������� (��� ������), �� ����
                    flag = false;
            }
        }
        if (flag == true)
        {
            if (first == 1) //���� ���� ������� �� �����, �� ������� ����� ����� 2
                winner = 2;
            else
                winner = 1;
            break;
        }
    }

    //�������� ������� ���������
    int first = map[0];
    bool flag = true;
    for (int j = 0, i = 0; i < n; j++, i++)
    {
        if (map[i * n + j] != first || map[i * n + j] == 0) //���� ���������� �������� �� ������� (��� ������), �� ����
            flag = false;
    }
    if (flag == true)
    {
        if (first == 1) //���� ���� ������� �� �����, �� ������� ����� ����� 2
            winner = 2;
        else
            winner = 1;
    }

    //�������� �������� ���������
    first = map[n - 1];
    flag = true;
    for (int j = 0, i = n - 1; j < n; j++, i--)
    {
        if (map[i * n + j] != first || map[i * n + j] == 0) //���� ���������� �������� �� ������� (��� ������), �� ����
            flag = false;
    }
    if (flag == true)
    {
        if (first == 1) //���� ���� ������� �� �����, �� ������� ����� ����� 2
            winner = 2;
        else
            winner = 1;
    }


    //���������� ������� ������ ����� ��� ��������
    if (winner)
    {
        if (winner == 2 && user % 2 == 0)
            return 1;
        else if (winner == 1 && user % 2 == 1)
            return 1;
        else
            return 2;
    }

    //������� ������� ������
    int nf = 0;
    for (int i = 0; i < n * n; i++)
    {
        if (map[i] != 0)
            nf++;
    }
    if (nf == n * n)	//���� ��� ������
        return 3;	//�� �����


    return 0;
}

//����� ��������� � �����������
void move_test(HWND hwnd)
{
    switch (check_field())	//�������� ����
    {
    case 1:
        MessageBox(hwnd, L"�� ��������!", L"�����������!", MB_OK);	//������� ���������
        PostMessage(hwnd, WM_CLOSE, NULL, NULL);	//������� ������ ���� ��������� � �������� ����� ��������� ��������� ��������� � ������������� ���� ��������
        break;
    case 2:
        MessageBox(hwnd, L"�� ���������", L"��������!", MB_OK);	//������� ���������
        PostMessage(hwnd, WM_CLOSE, NULL, NULL);	//������� ������ ���� ��������� � �������� ����� ��������� ��������� ��������� � ������������� ���� ��������
        break;
    case 3:
        MessageBox(hwnd, L"�����!", L"�����������!", MB_OK);	//������� ���������
        PostMessage(hwnd, WM_CLOSE, NULL, NULL);	//������� ������ ���� ��������� � �������� ����� ��������� ��������� ��������� � ������������� ���� ��������
        break;
    default:
        break;
    }
}

//��������� �������� � �������
void DrawShapes(HWND hwnd)
{
    HDC hdc = GetDC(hwnd);

    Pen = CreatePen(0, 2, RGB(0, 0, 0));
    SelectObject(hdc, Pen);
    RECT rect;
    GetClientRect(hwnd, &rect);
    int len = (rect.right - rect.left) / n;
    int high = (rect.bottom - rect.top) / n;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
        {
            if (map[i * n + j] == 1)
                if (len <= high)
                    Arc(hdc, len * i, high * j + (high - len) / 2, len * (i + 1), high * (j + 1) - (high - len) / 2, high * j + (high - len) / 2, len * (i + 1), high * j + (high - len) / 2, len * (i + 1));
                else
                    Arc(hdc, len * i + (len - high) / 2, high * j, len * (i + 1) - (len - high) / 2, high * (j + 1), len * i + (len - high) / 2, high * j, len * i + (len - high) / 2, high * j);
            if (map[i * n + j] == 2)
            {
                MoveToEx(hdc, len* i, high* j, NULL);
                LineTo(hdc, len* (i + 1), high* (j + 1));
                MoveToEx(hdc, len* i, high* (j + 1), NULL);
                LineTo(hdc, len* (i + 1), high* j);
            }
        }

    ReleaseDC(hwnd, hdc);

    
}

//����� ���������� 
DWORD WINAPI drawer(HWND hwnd)
{
    HDC hdc;
    RECT rect;
    TRIVERTEX vertex[2]; //������ ��������� ��� �������� ���������� � ����� � � ��������� �����  

    vertex[0].Red = 0 * 256;
    vertex[0].Green = 0 * 256;
    vertex[0].Blue = 255 * 256;

    vertex[1].Red = 255 * 256;
    vertex[1].Green = 0 * 256;
    vertex[1].Blue = 0 * 256;



    GRADIENT_RECT gRect; //��������� GRADIENT_RECT ���������� ������ ���� ������ � ������� pVertex � ������� GradientFill . 
    gRect.UpperLeft = 0; //����� �������
    gRect.LowerRight = 1; //������ ������

    bool change0 = false, change1 = false; //����� ����� ����������� ��������� ������ (���� ��� 255, �� �������� ���������, ����� �����������)
    int speed = 2000;    

    while (resume_thread)
    {
        GetClientRect(hwnd, &rect);
        hdc = GetDC(hwnd);
        
        vertex[0].x = 0; //�� ������ � ���������, �.�. rect ����� �������� ������
        vertex[0].y = 0;
        vertex[1].x = rect.right;
        vertex[1].y = rect.bottom;

        //������ ������ �������� ������ �� ����� ������ ������        
        DWORD result = WaitForSingleObject(hDraw, 100); //��� � ������ ����� ��������� ����� (��� ��������� �����)
        if (result != WAIT_OBJECT_0) 
        {
            if (change1) 
                vertex[1].Green = vertex[1].Green - 1 * speed; //���� �������� �����������, �� �������� �������
            else 
                vertex[1].Green = vertex[1].Green + 1 * speed; //���� �� ��������, �� ���������� 1
            if (vertex[1].Green > 255 * 256 - speed - 1) change1 = true; //��������� ����� �� ������ ����������� ����������
            else if (vertex[1].Green < speed + 1) change1 = false;
   

            if (change0) 
                vertex[0].Green = vertex[0].Green - 1 * speed;
            else 
                vertex[0].Green = vertex[0].Green + 1 * speed;            
            if (vertex[0].Green > 255 * 256 - speed - 1) change0 = true;
            else if (vertex[0].Green < speed + 1) change0 = false;

        }        
        GradientFill( //��������� ������������� � ����������� ��������� (�� gdi)
            hdc, 
            vertex,             //��������� �� ������ �������� TRIVERTEX, ������ �� ������� ���������� �������.
            2,                     //���������� ������ � pVertex 
            &gRect,                //������ �������� � ������ ��������������
            1,                     //���������� ��������� ��������������� � gRect
            GRADIENT_FILL_RECT_V); //����� ����������� �������  (������ ����)




        //������ �����
        Pen = CreatePen(0, 2, RGB(CLred % 256, CLgreen % 256, CLblue % 256));
        SelectObject(hdc, Pen);
        int len = (rect.right - rect.left) / n;
        int high = (rect.bottom - rect.top) / n;
        for (int i = 1; i < n; i++)
        {
            MoveToEx(hdc, len * i, rect.top, NULL);
            LineTo(hdc, len * i, rect.bottom);
            MoveToEx(hdc, rect.left, high * i, NULL);
            LineTo(hdc, rect.right, high * i);
        }
        ReleaseDC(hwnd, hdc);



        //������ ������� ��������� �����
        DrawShapes(hwnd);

    }
    return 0;
}

//����� ��� �������� ����
DWORD WINAPI potok1(HWND hwnd)
{
    int a = 0;
    while (true) a*=a; 
    return 0;
}

/*  This function is called by the Windows function DispatchMessage()  */
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == msgnum)	//���� ������ ����������������� ���������
    {
        move_test(hwnd);	//��������� ���, �� ����������� �� ����
        step++;
    }

    switch (message)                  /* handle the messages */
    {

    case WM_DESTROY:
    {
        RECT rect;
        ofstream outfile("config.txt");
        GetWindowRect(hwnd, &rect);

        //���������� � ���������������� ����
        outfile << n << "\n";//n
        outfile << rect.right - rect.left << " " << rect.bottom - rect.top << "\n";// ��������� � ������� ����
        outfile << CBred << " " << CBgreen << " " << CBblue << "\n"; //���� ����
        outfile << abs(CLred) % 256 << " " << abs(CLgreen) % 256 << " " << abs(CLblue) % 256 << "\n";//���� �����
        outfile.close();

        resume_thread = false; //�������� ��������� ������, ����� �� ����������

        PostQuitMessage(0);       /* send a WM_QUIT to the message queue */
        return 0;
    }

    case WM_SIZE:
        InvalidateRect(hwnd, NULL, TRUE);
        return 0;

    case WM_KEYUP:
        switch (wParam)
        {
        case KEY_ESC://ESC
            PostQuitMessage(0);
            return 0;

        case KEY_C://Shift + C 
            if (GetKeyState(KEY_SHIFT) & 0x8000)
                RunNotepad();
            return 0;

        case KEY_Q:  //Ctrl + Q 
            if (GetKeyState(KEY_CTRL) & 0x8000)
                PostQuitMessage(0);
            return 0;
        

        case VK_SPACE://�� ������� ����������������/������������ �����

            if (!issuspended)
                SuspendThread(hDraw); //���� ����� �� �������������, �� ������������� 
            else
                ResumeThread(hDraw); //����� ���������
            issuspended = !issuspended; //�������� ��������
            return 0;

        case KEY_1: //��� ������� ����� ������������� ��������������� ��������� ������ ���������
            SetThreadPriority(hDraw, THREAD_PRIORITY_IDLE);
            return 0;
        case KEY_2:
            SetThreadPriority(hDraw, THREAD_PRIORITY_LOWEST);
            return 0;
        case KEY_3:
            SetThreadPriority(hDraw, THREAD_PRIORITY_BELOW_NORMAL);
            return 0;
        case KEY_4:
            SetThreadPriority(hDraw, THREAD_PRIORITY_NORMAL);
            return 0;
        case KEY_5:
            SetThreadPriority(hDraw, THREAD_PRIORITY_ABOVE_NORMAL);
            return 0;
        case KEY_6:
            SetThreadPriority(hDraw, THREAD_PRIORITY_HIGHEST);
            return 0;
        case KEY_7:
            SetThreadPriority(hDraw, THREAD_PRIORITY_TIME_CRITICAL);
            return 0;

        }
        return 0;

    case WM_LBUTTONUP://����� ������ 
    {
        RECT rect;
        GetClientRect(hwnd, &rect);
        int len = (rect.right - rect.left) / n;
        int high = (rect.bottom - rect.top) / n;
        int x = GET_X_LPARAM(lParam) / len;
        int y = GET_Y_LPARAM(lParam) / high;
        

        if (map[x * n + y] == 0) //���� � ������ �����
        {          
            WaitForSingleObject(mutex, 0);
            
            if (step % 2 == user % 2)                  
            {           
                if (user % 2 == 1)
                    map[x * n + y] = 2; // ���� ��� �������� ������ �������
                else
                    map[x * n + y] = 1; // �����

                PostMessage(HWND_BROADCAST, msgnum, NULL, NULL);	//����������������� ��������� �� ���������� ���������� ���� �����������                
            }
            else   
            {
                MessageBox(hwnd, L"������ ��� ������� ������", L"������", MB_OK);                
            }     
            ReleaseMutex(mutex);                        
        }

        return 0;
    }


    case WM_MOUSEWHEEL://����c� ����
    {

        //������, �� �� ��� ������� �����������
        if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
        {
            CLred += 1;
            CLgreen += 5;
            CLblue += 5;
        }
        else
        {
            CLred -= 5;
            CLgreen -= 2;
            CLblue -= 2;
        }

        return 0;
    }

    case WM_CREATE:	//����������� ����� ����� ��������
    {
        DWORD tmp;
        hDraw = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)drawer, hwnd, NULL, &tmp); //������� ����� ���������

        //potok = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)potok1, hwnd, NULL, &tmp); //������� ����� ��� �������� ����������������� ��������� �����������
        //SetThreadPriority(potok, THREAD_PRIORITY_NORMAL);

        return 0;
    }

    }

    /* for messages that we don't deal with */
    return DefWindowProc(hwnd, message, wParam, lParam);
}

int main(int argc, char** argv)
{

    mutex = CreateMutex(NULL, FALSE, NULL);

    setlocale(LC_ALL, "Russian"); //������� ���� ����������

    int width = 320, height = 240, choice;
    CLred = 255; CLgreen = 0; CLblue = 0; //���� ����� �� ���������
    CBred = 0; CBblue = 255; CBgreen = 0; //���� ���� �� ���������


    HANDLE hMapFile;
    msgnum = RegisterWindowMessage(szName);
    hMapFile = OpenFileMapping(
        FILE_MAP_ALL_ACCESS,   // read/write access
        TRUE,                 // do inherit the name
        szName);       // name of mapping object


    if (hMapFile == NULL)
    {
        //first process

        ifstream outfile("config.txt");
        if (!outfile.fail()) //���� ���� ���������������� ����
            outfile >> n >> width >> height >> CBred >> CBgreen >> CBblue >> CLred >> CLgreen >> CLblue;
        outfile.close();

        if (argc == 2)
            n = atoi(argv[1]);

        hMapFile = CreateFileMapping(
            INVALID_HANDLE_VALUE,    // use paging file
            NULL,                    // default security
            PAGE_READWRITE,          // read/write access
            0,                       // maximum object size (high-order DWORD)
            sizeof(INT) * n * n,               // maximum object size (low-order DWORD)
            szName);


        map = (LPINT)MapViewOfFile(hMapFile,   // handle to map object
            FILE_MAP_ALL_ACCESS, // read/write permission
            0,
            0,
            sizeof(INT) * n * n);

                
    }
        
    else
    {
        //second process

        ifstream outfile("config.txt");
        outfile >> n >> width >> height >> CBred >> CBgreen >> CBblue >> CLred >> CLgreen >> CLblue;;
        outfile.close();

        map = (LPINT)MapViewOfFile(hMapFile, // handle to map object
            FILE_MAP_ALL_ACCESS,  // read/write permission
            0,
            0,
            sizeof(INT) * n * n);


        bool flag = false;
        for (int i = 0; i < n * n; i++) //���� � ����� ������ ��� ���� �������� ���������� ������, �������� �� 0
            if (map[i] > 0)
            {
                flag = !flag; 
                break;
            }
        if (flag) step++; //�� ��� �������� ������� �������� ������ �������� ����


        WaitForSingleObject(mutex, 0);
        user++;
        ReleaseMutex(mutex);
    }

    


    BOOL bMessageOk;
    MSG message;            /* Here message to the application are saved */
    WNDCLASS wincl = { 0 };         /* Data structure for the windowclass */

    /* Harcode show command num when use non-winapi entrypoint */
    int nCmdShow = SW_SHOW;
    /* Get handle */
    HINSTANCE hThisInstance = GetModuleHandle(NULL);

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szWinClass;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by Windows */

    /* Use custom brush to paint the background of the window */
    //hBrush = CreateSolidBrush(RGB(CBred, CBgreen, CBblue));
    //wincl.hbrBackground = hBrush;
    /* Register the window class, and if it fails quit the program */
    if (!RegisterClass(&wincl))
        return 0;
    /* The class is registered, let's create the program*/
    hwnd = CreateWindow(
        szWinClass,          /* Classname */
        szWinName,       /* Title Text */
        WS_OVERLAPPEDWINDOW, /* default window */
        CW_USEDEFAULT,       /* Windows decides the position */
        CW_USEDEFAULT,       /* where the window ends up on the screen */
        width,                 /* The programs width */
        height,                 /* and height in pixels */
        HWND_DESKTOP,        /* The window is a child-window to desktop */
        NULL,                /* No menu */
        hThisInstance,       /* Program Instance handler */
        NULL                 /* No Window Creation data */
    );
    /* Make the window visible on the screen */
    ShowWindow(hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while ((bMessageOk = GetMessage(&message, NULL, 0, 0)) != 0)
    {
        /* Yep, fuck logic: BOOL mb not only 1 or 0.
         * See msdn at https://msdn.microsoft.com/en-us/library/windows/desktop/ms644936(v=vs.85).aspx
         */
        if (bMessageOk == -1)
        {
            puts("Suddenly, GetMessage failed! You can call GetLastError() to see what happend");
            break;
        }
        /* Translate virtual-key message into character message */
        TranslateMessage(&message);
        /* Send message to WindowProcedure */
        DispatchMessage(&message);
    }



    /* Cleanup stuff */
    CloseHandle(mutex);
    
    WaitForSingleObject(hDraw, INFINITE);	//��������� ���������� ������ ���������
    CloseHandle(hDraw);

    DestroyWindow(hwnd);
    UnregisterClass(szWinClass, hThisInstance);
    DeleteObject(hBrush);
    DeleteObject(newBrush);

    UnmapViewOfFile(map);
    CloseHandle(hMapFile);
    return 0;
}



