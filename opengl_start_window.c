#include <windows.h>
#include <gl/gl.h>

#pragma comment (lib, "opengl32.lib")


LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);


// точка входа для графического приложения Windows
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;      // WNDCLASSEX (window class) – класс окна
    HWND hwnd;          // HWND (handle to the window) – дескриптор окна (программа ссылается на окно с помощью дескрипторов, это просто число, которое ОС использует для идентификации объекта)
    HDC hdc;            // HDC (handle to the document context) – дескриптор контекста устройства (специальный набор значений, которые приложение использует для рисования в клиентской области окна)
    HGLRC hrc;          // HGLRC (handle to the GL rendering context) – дескриптор контекста отрисовки OpenGL
    MSG msg;            // MSG (message) – содержит информацию о сообщениях из очереди сообщений потока


    // регестрируем класс окна
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = NULL;
    wc.hCursor = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = L"сlassName";
    wc.hIconSm = NULL;

    RegisterClassEx(&wc);


    // создаем главное окно
    hwnd = CreateWindowEx(
        0,
        L"сlassName",
        L"Hello OpenGL",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        256,
        256,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hwnd) return 1;


    // показываем окно
    ShowWindow(hwnd, nCmdShow);

    // подключаем OpenGL к окну
    EnableOpenGL(hwnd, &hdc, &hrc);


    // основной цикл программы
    float angle = 0.0f;

    while (TRUE)
    {
        // проверка сообщения
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            // отправка или обработка сообщения
            TranslateMessage(&msg); // переводит сообщения виртуальных клавиш в символьные сообщения
            DispatchMessage(&msg); // отправляет сообщение оконной процедуре WindowProc()

            if (msg.message == WM_QUIT) break;
        }
        else
        {
            // отрисовка OpenGL

            glClearColor(0, 0, 0, 0);
            glClear(GL_COLOR_BUFFER_BIT);

            glPushMatrix();
            
            glRotatef(angle, 0, 0, 1);

            glBegin(GL_TRIANGLES);
                glColor3f(1, 0, 0); glVertex2f(    0,    1);
                glColor3f(0, 1, 0); glVertex2f( 0.87, -0.5);
                glColor3f(0, 0, 1); glVertex2f(-0.87, -0.5);
            glEnd();

            glPopMatrix();

            SwapBuffers(hdc);

            angle += 1.0f;
            Sleep(4);
        }
    }


    // отключаем OpenGL
    DisableOpenGL(hwnd, hdc, hrc);

    // уничтожаем окно
    DestroyWindow(hwnd);


    return 0;
}

// оконная процедура WindowProc() обрабатывает сообщения окна
LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_CLOSE) PostQuitMessage(0);
    else if (message == WM_KEYDOWN)
    {
        if (wParam == VK_ESCAPE) PostQuitMessage(0);
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

void EnableOpenGL(HWND hwnd, HDC* hdc, HGLRC* hrc)
{
    PIXELFORMATDESCRIPTOR pfd; // pixel format descriptor – дескриптор формата пикселей
    int fmt;

    // получаем контекст устройства
    *hdc = GetDC(hwnd); // извлекает дескриптор контекста устройства указанного окна

    // установливаем формат пикселей для контекста устройства
    ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR)); // заполняет указанный буфер нулями
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    fmt = ChoosePixelFormat(*hdc, &pfd); // сопоставляет формат пикселей контекста устройства с заданным форматом пикселей
    SetPixelFormat(*hdc, fmt, &pfd); // устанавливает формат пикселей указанного контекста устройства

    // создаем и включаем контекст отрисовки
    *hrc = wglCreateContext(*hdc); // создает новый контекст отрисовки OpenGL, подходящий для рисования на устройстве, на которое ссылается hdc 
    wglMakeCurrent(*hdc, *hrc); // делает указанный hrc текущим контекстом отрисовки вызывающего потока
}

void DisableOpenGL(HWND hwnd, HDC hdc, HGLRC hrc)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hrc);
    ReleaseDC(hwnd, hdc);
}