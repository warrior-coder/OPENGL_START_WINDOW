#include <windows.h>
#include <gl/gl.h>

#pragma comment (lib, "opengl32.lib")


LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);


// точка входа для графического приложения Windows
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASS wc;                        // WNDCLASS (window class) – класс окна
    HWND hwnd;                          // HWND (handle to the window) – дескриптор окна (дескриптор – определенное число, которое ОС использует для идентификации объектов)
    MSG msg;                            // MSG (message) – содержит информацию о сообщениях из очереди сообщений потока
    LPCWSTR className = L"className";   // LPCWSTR (long pointer to const wide string) – константная строка расширенных символов кодировки Unicode
    HDC hdc;                            // HDC (handle to the document context) – дескриптор контекста устройства (специальный набор значений, которые приложение использует для рисования в клиентской области окна)
    HGLRC hrc;                          // HGLRC (handle to the GL rendering context) – дескриптор контекста отрисовки OpenGL


    // регестрируем класс окна
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

    // создаем главное окно
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

    // показываем окно
    ShowWindow(hwnd, SW_SHOWNORMAL);

    // подключаем OpenGL к окну
    EnableOpenGL(hwnd, &hdc, &hrc);


    // основной цикл программы
    GLfloat angle = 0.0F;

    while (TRUE)
    {
        // проверка сообщения
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            // отправка или обработка сообщения
            TranslateMessage(&msg); // переводит сообщения виртуальных клавиш в символьные сообщения
            DispatchMessage(&msg); // отправляет сообщение оконной процедуре WindowProc()

            if (msg.message == WM_QUIT)
            {
                break;
            }
        }
        else
        {
            // отрисовка OpenGL
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


    // отключаем OpenGL
    DisableOpenGL(hwnd, hdc, hrc);

    // уничтожаем окно
    DestroyWindow(hwnd);


    return 0;
}

// оконная процедура WindowProc() обрабатывает сообщения окна
LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_CLOSE)
    {
        // в случае закрытия окна отправляем сообщение WM_QUIT в очередь сообщений
        PostQuitMessage(0);
    }
    else if (message == WM_PAINT)
    {
        PAINTSTRUCT ps;

        // подготавливает указанное окно к отрисовке и заполняет структуру PAINTSTRUCT информацией об отрисовке
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc = ps.rcPaint; // координаты прямоугольника для заполнения

        // создает логическую кисть указанного сплошного цвета
        HBRUSH hb = CreateSolidBrush(
            RGB(0, 0, 255)
        );

        // заполняет всю область указанного прямоугольника сплошным цветом
        FillRect(hdc, &rc, hb);

        // сигнализирует о завершении отрисовки в указанном окне
        EndPaint(hwnd, &ps);

        // удаляем кисть
        DeleteObject(hb);
    }
    else if (message == WM_KEYDOWN)
    {
        // в случае нажатия клавиши Escape отправляем сообщение WM_QUIT в очередь сообщений
        if (wParam == VK_ESCAPE) PostQuitMessage(0);
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}

void EnableOpenGL(HWND hwnd, HDC* hdc, HGLRC* hrc)
{
    PIXELFORMATDESCRIPTOR pfd; // pixel format descriptor – дескриптор формата пикселей
    int pixelFormat;

    // извлекает контекст устройства указанного окна
    *hdc = GetDC(hwnd);

    // установливаем формат пикселей для контекста устройства
    ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR)); // заполняет указанный буфер нулями
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    // сопоставляет формат пикселей контекста устройства с заданным форматом пикселей
    pixelFormat = ChoosePixelFormat(*hdc, &pfd);

    // устанавливает формат пикселей для указанного контекста устройства
    SetPixelFormat(*hdc, pixelFormat, &pfd);

    // создает новый контекст отрисовки OpenGL, подходящий для рисования на устройстве, на которое ссылается hdc 
    *hrc = wglCreateContext(*hdc);

    // делает указанный hrc текущим контекстом отрисовки вызывающего потока
    wglMakeCurrent(*hdc, *hrc);
}

void DisableOpenGL(HWND hwnd, HDC hdc, HGLRC hrc)
{
    // делаем текущий HGLRC неактуальным и освобождаем контекст устройства, который использовался контекстом рендеринга
    wglMakeCurrent(NULL, NULL);

    // удаляет указанный контекст отрисовки OpenGL
    wglDeleteContext(hrc);

    // освобождает контекст устройства DC заданного окна
    ReleaseDC(hwnd, hdc);
}