//
// Created by Gunter on 26/03/2021.
//

#ifndef MANDELBROT_MESSAGEHANDLER_H
#define MANDELBROT_MESSAGEHANDLER_H

#include <windowsx.h>
#include <windows.h>
#include <iostream>
#include <cmath>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>

class MessageHandler {
private:
    int width = 1;
    int height = 1;
    int threshold = 100;
    bool moving = false;
    bool help = true;
    double scale = 1;
    double posX = 0;
    double posY = 0;
    double ratioFX = -2.5;
    double ratioTX = 1;
    double ratioFY = -1;
    double ratioTY = 1;

public:
    LRESULT CALLBACK handle(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    void lButtonDown(int x, int y);

    void mandelbrot(double &x, double &y, double cx, double cy);

    void screenToPoint(double &x, double &y, const int xPixel, const int yPixel) const {
        double xTotal = std::abs(ratioFX) + std::abs(ratioTX);
        double yTotal = std::abs(ratioFY) + std::abs(ratioTY);
        x = double(xPixel) / width * xTotal / scale + ratioFX / scale + posX;
        y = double(yPixel) / height * yTotal / scale + ratioFY / scale - posY;
    }

    COLORREF filter(int counter, int type = 3) const;

    COLORREF HSVtoRGB(float H, float S, float V) const;

    void lButtonUp(int x, int y);

    void scroll(short zDelta, int x, int y);

    void moveTo(int xPixel, int yPixel);
};


#endif //MANDELBROT_MESSAGEHANDLER_H
