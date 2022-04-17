#include <Windows.h>
#include <gl/GL.h>


LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM); // оконная процедура обработки сообщения окна
void EnableOpenGL(HWND, HDC*, HGLRC*); // подключение OpenGL к контектсу окна
void DisableOpenGL(HWND, HDC, HGLRC); // отключение OpenGL


// точка входа для графического приложения Windows
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASS wc;                        // WNDCLASS (window class) – класс окна
    HWND hwnd;                          // HWND (handle to the window) – дескриптор окна (дескриптор – определенное число, которое ОС использует для идентификации объектов)
    MSG msg;                            // MSG (message) – содержит информацию о сообщениях из очереди сообщений потока
    LPCWSTR className = L"className";   // LPCWSTR (long pointer to const wide string) – константная строка расширенных символов кодировки Unicode
    HDC hdc;                            // HDC (handle to the document context) – дескриптор контекста устройства (специальный набор значений, которые приложение использует для рисования в клиентской области окна)
    HGLRC hrc;                          // HGLRC (handle to the GL rendering context) – дескриптор контекста отрисовки OpenGL

    // регестрация класса окна
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

    // создание главного окна
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

    if (!hwnd)
    {
        return 1;
    }

    ShowWindow(hwnd, SW_SHOWNORMAL); // показываем окно

    // подключение OpenGL к окну
    EnableOpenGL(hwnd, &hdc, &hrc);

    // основной цикл программы
    GLfloat rotateAngle = 0.0f;

    while (TRUE)
    {
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
            // отрисовка изображения
            glClearColor(0.5f, 0.5f, 0.5f, 0);
            glClear(GL_COLOR_BUFFER_BIT);

            glPushMatrix(); // помещение матрицы в стек
            glTranslatef(0, 0, 0); // смешение системы координат
            glRotatef(rotateAngle, 0, 0, 1); // вращение системы координат на rotateAngle вокруг Oz
            rotateAngle += 2.0f;


            glBegin(GL_TRIANGLES);
                glColor3f(1, 0, 0); glVertex2f(  0.0f,  1.0f);
                glColor3f(0, 1, 0); glVertex2f( 0.87f, -0.5f);
                glColor3f(0, 0, 1); glVertex2f(-0.87f, -0.5f);
            glEnd();

            glPopMatrix(); // загрузка матрицы из стека (возвращение ее прежнего состояния)

            // обмен переднего и заднего буферов
            SwapBuffers(hdc);

            Sleep(5);
        }
    }

    // отключение OpenGL
    DisableOpenGL(hwnd, hdc, hrc);

    // уничтожение окна
    DestroyWindow(hwnd);

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_CLOSE)
    {
        // в случае закрытия окна отправляем сообщение WM_QUIT в очередь сообщений
        PostQuitMessage(0);
    }
    else if (message == WM_PAINT)
    {
        // сообщение WM_PAINT отправляется, когда система делает запрос на закрашивание окна приложения
        PAINTSTRUCT ps; // структура с информацией об отрисовке окна
        HBRUSH hb = CreateSolidBrush(RGB(0, 0, 255)); // создание сплошного логическую кисть указанного цвета
        
        HDC hdc = BeginPaint(hwnd, &ps); // подготавливка указанного окна к отрисовке и заполнение структуры PAINTSTRUCT информацией об отрисовке
            RECT rc = ps.rcPaint; // координаты прямоугольника для заполнения
            FillRect(hdc, &rc, hb); // заполнение прямоугольника цветом кисти
        EndPaint(hwnd, &ps); // завершение отрисовки в указанном окне

        DeleteObject(hb); // удаление кисти
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
    PIXELFORMATDESCRIPTOR pfd; // PIXELFORMATDESCRIPTOR (pixel format descriptor) – дескриптор формата пикселей
    int pixelFormat;

    *hdc = GetDC(hwnd); // извлекает контекст устройства указанного окна

    // установливка формата пикселей для контекста устройства
    ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR)); // заполняет указанный буфер нулями

    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    pixelFormat = ChoosePixelFormat(*hdc, &pfd); // сопоставляет формат пикселей контекста устройства с заданным форматом пикселей

    SetPixelFormat(*hdc, pixelFormat, &pfd); // устанавливает формат пикселей для указанного контекста устройства
    *hrc = wglCreateContext(*hdc); // создает новый контекст отрисовки OpenGL, подходящий для рисования на устройстве, на которое ссылается hdc 
    wglMakeCurrent(*hdc, *hrc); // делает указанный hrc текущим контекстом отрисовки вызывающего потока
}

void DisableOpenGL(HWND hwnd, HDC hdc, HGLRC hrc)
{
    wglMakeCurrent(NULL, NULL); // делаем текущий HGLRC неактуальным и освобождаем контекст устройства, который использовался контекстом рендеринга
    wglDeleteContext(hrc); // удаляет указанный контекст отрисовки OpenGL
    ReleaseDC(hwnd, hdc); // освобождает контекст устройства DC заданного окна
}