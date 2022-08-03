//
// Created by Gunter on 26/03/2021.
//
# define M_PI           3.14159265358979323846  /* pi */
#include "MessageHandler.h"

LRESULT CALLBACK MessageHandler::handle(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    PAINTSTRUCT ps;
    HDC hdc;
    RECT clientRect;
    HBITMAP hbmp;

    switch (msg) {
        case WM_ENTERSIZEMOVE:
            moving = true;
            break;
        case WM_EXITSIZEMOVE:
            moving = false;
            GetClientRect(hwnd, &clientRect);
            InvalidateRect(hwnd, nullptr, FALSE);
            break;
        case WM_SIZE:
            if (wParam == SIZE_MAXIMIZED) {
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            break;
        case WM_HELP:
            help = !help;
            InvalidateRect(hwnd, nullptr, FALSE);
            break;
        case WM_PAINT:
            if (!moving) {
                hdc = BeginPaint(hwnd, &ps);
                GetClientRect(hwnd, &clientRect);

                width = clientRect.right;
                height = clientRect.bottom;

                unsigned __int64 now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

                for (int yPixel = 0; yPixel < height; ++yPixel) {
                    for (int xPixel = 0; xPixel < width; ++xPixel) {
                        double x, y;
                        screenToPoint(x, y, xPixel, yPixel);
                        const double cx = x, cy = y;
                        int counter = 0;
                        while (x * x + y * y <= 4 && ++counter != threshold) {
                            mandelbrot(x, y, cx, cy);
                        }
                        COLORREF cRef;
                        if (counter == threshold) {
                            cRef = 0;
                        } else {
                            cRef = filter(counter);
                        }
                        SetPixel(hdc, xPixel, yPixel, cRef);
                    }
                }

                if (help) {
                    unsigned __int64 after = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                    unsigned __int64 diff = after - now;

                    std::stringstream stream;
                    stream << std::fixed << std::setprecision(2) << scale;
                    std::string zoom = stream.str();

                    const std::string menu[] = {
                            "Mousewheel: Zoom",
                            "Middle-Mouse: Move To",
                            "Threshold: " + std::to_string(threshold),
                            "Zoom: " + zoom,
                            "Load Time: " + std::to_string(diff) + "ms",
                            "X POS: " + std::to_string(posX),
                            "Y POS: " + std::to_string(posY),
                    };
                    std::string text;
                    for (const std::string &menuItem : menu) {
                        text += menuItem + "\n";
                    }
//                    SetBkMode(hdc, TRANSPARENT);
                    SetBkColor(hdc, 0x696969);
                    DrawTextA(hdc, text.c_str(), -1, &clientRect, DT_LEFT);
                }

                EndPaint(hwnd, &ps);
            }
            break;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_LBUTTONDOWN:
            lButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            break;
        case WM_LBUTTONUP:
            lButtonUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            break;
        case WM_MBUTTONDOWN:
            moveTo(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            InvalidateRect(hwnd, nullptr, FALSE);
            break;
        case WM_MOUSEWHEEL:
            scroll(GET_WHEEL_DELTA_WPARAM(wParam), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
            InvalidateRect(hwnd, nullptr, FALSE);
            break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}
//static int count = 0;
COLORREF MessageHandler::filter(const int counter, int type) const {
    const double c = 256. * counter / threshold;
//    std::cout << c << std::endl;
//    if (++count == 5000) {
//        exit(0);
//    }
    switch (type) {
        case 0:
            return RGB(c, 0, 0);
        case 1:
            return RGB(0, c, 0);
        case 2:
            return RGB(0, 0, c);
        case 3: {
            const float saturation = 100;
            const float value = counter < threshold ? 100 : 0;
            return HSVtoRGB(c, saturation, value);
        }
        default:
            return RGB(c, c, c);
    }
}

void MessageHandler::lButtonDown(int xPixel, int yPixel) {
//    std::cout << "Left DOWN" << std::endl;
//    double x, y;
//    screenToPoint(x, y, xPixel, yPixel);
//    std::cout << "x: " << x << ", y: " << y << std::endl;
}

void MessageHandler::lButtonUp(int x, int y) {
//    std::cout << "Left UP" << std::endl;
//    std::cout << "x: " << x << ", y: " << y << std::endl;
}

void MessageHandler::scroll(short zDelta, int xPixel, int yPixel) {
    bool zoomIn = zDelta > 0;
    double xMax, yMax;
    screenToPoint(xMax, yMax, width / 2, height / 2);
    if (zoomIn) {
        scale *= 2;
    } else {
        scale /= 2;
    }
    threshold = scale * M_PI * 100;
    posX = xMax - (ratioFX + ratioTX) / 2 / scale;
}

void MessageHandler::moveTo(int xPixel, int yPixel) {
    double x, y;
    screenToPoint(x, y, xPixel, yPixel);
    posX = x - (ratioFX + ratioTX) / 2 / scale;
    posY = -y;
}

void MessageHandler::mandelbrot(double &x, double &y, const double cx, const double cy) {
    const double xTmp = x * x - y * y + cx;
    y = 2 * x * y + cy;
    x = xTmp;
}

COLORREF MessageHandler::HSVtoRGB(float H, float S, float V) const {
    if (H > 360 || H < 0 || S > 100 || S < 0 || V > 100 || V < 0) {
        std::cout << "The given HSV values are not in valid range" << std::endl;
        return 0;
    }
    float s = S / 100;
    float v = V / 100;
    float C = s * v;
    float X = C * (1 - abs(std::fmod(H / 60.0, 2) - 1));
    float m = v - C;
    float r, g, b;
    if (H >= 0 && H < 60) {
        r = C, g = X, b = 0;
    } else if (H >= 60 && H < 120) {
        r = X, g = C, b = 0;
    } else if (H >= 120 && H < 180) {
        r = 0, g = C, b = X;
    } else if (H >= 180 && H < 240) {
        r = 0, g = X, b = C;
    } else if (H >= 240 && H < 300) {
        r = X, g = 0, b = C;
    } else {
        r = C, g = 0, b = X;
    }
    return RGB((r + m) * 255, (g + m) * 255, (b + m) * 255);
}
