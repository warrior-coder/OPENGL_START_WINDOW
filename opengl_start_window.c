#include <windows.h>
#include <gl/gl.h>

#pragma comment (lib, "opengl32.lib")


LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);


// ����� ����� ��� ������������ ���������� Windows
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASS wc;                        // WNDCLASS (window class) � ����� ����
    HWND hwnd;                          // HWND (handle to the window) � ���������� ���� (���������� � ������������ �����, ������� �� ���������� ��� ������������� ��������)
    MSG msg;                            // MSG (message) � �������� ���������� � ���������� �� ������� ��������� ������
    LPCWSTR className = L"className";   // LPCWSTR (long pointer to const wide string) � ����������� ������ ����������� �������� ��������� Unicode
    HDC hdc;                            // HDC (handle to the document context) � ���������� ��������� ���������� (����������� ����� ��������, ������� ���������� ���������� ��� ��������� � ���������� ������� ����)
    HGLRC hrc;                          // HGLRC (handle to the GL rendering context) � ���������� ��������� ��������� OpenGL


    // ������������ ����� ����
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = NULL;
    wc.hCursor = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = className;

    RegisterClass(&wc);

    // ������� ������� ����
    hwnd = CreateWindow(
        className,
        L"OpenGL Start Window",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        400,
        400,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hwnd) return 1;

    // ���������� ����
    ShowWindow(hwnd, SW_SHOWNORMAL);

    // ���������� OpenGL � ����
    EnableOpenGL(hwnd, &hdc, &hrc);


    // �������� ���� ���������
    GLfloat angle = 0.0F;

    while (TRUE)
    {
        // �������� ���������
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            // �������� ��� ��������� ���������
            TranslateMessage(&msg); // ��������� ��������� ����������� ������ � ���������� ���������
            DispatchMessage(&msg); // ���������� ��������� ������� ��������� WindowProc()

            if (msg.message == WM_QUIT)
            {
                break;
            }
        }
        else
        {
            // ��������� OpenGL
            glClearColor(0, 0, 0, 0);
            glClear(GL_COLOR_BUFFER_BIT);

            glPushMatrix();

            glRotatef(angle, 0, 0, 1);

            glBegin(GL_TRIANGLES);
            glColor3f(1, 0, 0); glVertex2f(0, 1);
            glColor3f(0, 1, 0); glVertex2f(0.87, -0.5);
            glColor3f(0, 0, 1); glVertex2f(-0.87, -0.5);
            glEnd();

            glPopMatrix();

            SwapBuffers(hdc);

            angle += 1.0F;
            Sleep(5);
        }
    }


    // ��������� OpenGL
    DisableOpenGL(hwnd, hdc, hrc);

    // ���������� ����
    DestroyWindow(hwnd);


    return 0;
}

// ������� ��������� WindowProc() ������������ ��������� ����
LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_CLOSE)
    {
        // � ������ �������� ���� ���������� ��������� WM_QUIT � ������� ���������
        PostQuitMessage(0);
    }
    else if (message == WM_PAINT)
    {
        PAINTSTRUCT ps;

        // �������������� ��������� ���� � ��������� � ��������� ��������� PAINTSTRUCT ����������� �� ���������
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc = ps.rcPaint; // ���������� �������������� ��� ����������

        // ������� ���������� ����� ���������� ��������� �����
        HBRUSH hb = CreateSolidBrush(
            RGB(0, 0, 255)
        );

        // ��������� ��� ������� ���������� �������������� �������� ������
        FillRect(hdc, &rc, hb);

        // ������������� � ���������� ��������� � ��������� ����
        EndPaint(hwnd, &ps);

        // ������� �����
        DeleteObject(hb);
    }
    else if (message == WM_KEYDOWN)
    {
        // � ������ ������� ������� Escape ���������� ��������� WM_QUIT � ������� ���������
        if (wParam == VK_ESCAPE) PostQuitMessage(0);
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

void EnableOpenGL(HWND hwnd, HDC* hdc, HGLRC* hrc)
{
    PIXELFORMATDESCRIPTOR pfd; // pixel format descriptor � ���������� ������� ��������
    int pixelFormat;

    // ��������� �������� ���������� ���������� ����
    *hdc = GetDC(hwnd);

    // ������������� ������ �������� ��� ��������� ����������
    ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR)); // ��������� ��������� ����� ������
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    // ������������ ������ �������� ��������� ���������� � �������� �������� ��������
    pixelFormat = ChoosePixelFormat(*hdc, &pfd);

    // ������������� ������ �������� ��� ���������� ��������� ����������
    SetPixelFormat(*hdc, pixelFormat, &pfd);

    // ������� ����� �������� ��������� OpenGL, ���������� ��� ��������� �� ����������, �� ������� ��������� hdc 
    *hrc = wglCreateContext(*hdc);

    // ������ ��������� hrc ������� ���������� ��������� ����������� ������
    wglMakeCurrent(*hdc, *hrc);
}

void DisableOpenGL(HWND hwnd, HDC hdc, HGLRC hrc)
{
    // ������ ������� HGLRC ������������ � ����������� �������� ����������, ������� ������������� ���������� ����������
    wglMakeCurrent(NULL, NULL);

    // ������� ��������� �������� ��������� OpenGL
    wglDeleteContext(hrc);

    // ����������� �������� ���������� DC ��������� ����
    ReleaseDC(hwnd, hdc);
}