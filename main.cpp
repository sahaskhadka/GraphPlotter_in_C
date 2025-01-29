#include <graphics.h>
#include <cmath>
#include <stdio.h>
#include <string.h>
#include <conio.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define ORIGIN_X (SCREEN_WIDTH / 2)
#define ORIGIN_Y (SCREEN_HEIGHT / 2)
#define MAX_JUMP 100

struct FunctionParams {
    double a, b, c, d;
    char equation[100];
    int color;
} fp;

int func_type = 0;
double actual_scale = 50.0;

void drawAxes();
void drawGrid();
double calculateFunction(double x);
void getFunctionInput();
void plotFunction();
void drawTitle();
void updateScale(int delta);
void drawAxisTicks();

int main() {
    getFunctionInput();
    initwindow(SCREEN_WIDTH, SCREEN_HEIGHT, const_cast<char*>("Advanced Graph Plotter"), 0, 0, 1);
    while (true) {
        // Drawing to back buffer
        cleardevice();
        drawGrid();
        drawAxes();
        drawTitle();
        plotFunction();

        // Swap buffers
        swapbuffers();

        if (kbhit()) {
            char ch = getch();
            if (ch == 'q') break;
            else if (ch == '+') updateScale(1);
            else if (ch == '-') updateScale(-1);
        }
    }

    closegraph();
    return 0;
}


void drawAxes() {
    setcolor(LIGHTGRAY);
    line(0, ORIGIN_Y, SCREEN_WIDTH, ORIGIN_Y);  // X-axis
    line(ORIGIN_X, 0, ORIGIN_X, SCREEN_HEIGHT); // Y-axis
    drawAxisTicks();

    // X-axis arrow
    line(SCREEN_WIDTH - 10, ORIGIN_Y - 10, SCREEN_WIDTH, ORIGIN_Y);
    line(SCREEN_WIDTH - 10, ORIGIN_Y + 10, SCREEN_WIDTH, ORIGIN_Y);
    outtextxy(SCREEN_WIDTH - 20, ORIGIN_Y - 20, const_cast<char*>("X"));

    // Y-axis arrow
    line(ORIGIN_X - 10, 10, ORIGIN_X, 0);
    line(ORIGIN_X + 10, 10, ORIGIN_X, 0);
    outtextxy(ORIGIN_X + 10, 10, const_cast<char*>("Y"));
}



void drawGrid() {
    setcolor(DARKGRAY);
    const int MAJOR_STEP = 1;
    const int MINOR_DIVISIONS = 5;

    // Vertical grid lines
    double left = -ORIGIN_X / actual_scale;
    double right = (SCREEN_WIDTH - ORIGIN_X) / actual_scale;
    for (int x = floor(left); x <= ceil(right); x++) {
        int screenX = ORIGIN_X + x * actual_scale;
        if (x % MAJOR_STEP == 0) {
            for (int i = 1; i < MINOR_DIVISIONS; i++) {
                int minorX = screenX + (i * actual_scale) / MINOR_DIVISIONS;
                line(minorX, 0, minorX, SCREEN_HEIGHT);
            }
        }
    }

    // Horizontal grid lines
    double bottom = -ORIGIN_Y / actual_scale;
    double top = (SCREEN_HEIGHT - ORIGIN_Y) / actual_scale;
    for (int y = floor(bottom); y <= ceil(top); y++) {
        int screenY = ORIGIN_Y - y * actual_scale;
        if (y % MAJOR_STEP == 0) {
            for (int i = 1; i < MINOR_DIVISIONS; i++) {
                int minorY = screenY + (i * actual_scale) / MINOR_DIVISIONS;
                line(0, minorY, SCREEN_WIDTH, minorY);
            }
        }
    }
}

void drawAxisTicks() {
    setcolor(LIGHTGRAY);
    const int TICK_SIZE = 5;

    // X-axis ticks
    double left = -ORIGIN_X / actual_scale;
    double right = (SCREEN_WIDTH - ORIGIN_X) / actual_scale;
    for (int x = ceil(left); x <= floor(right); x++) {
        if (x == 0) continue;
        int screenX = ORIGIN_X + x * actual_scale;
        line(screenX, ORIGIN_Y - TICK_SIZE, screenX, ORIGIN_Y + TICK_SIZE);
        char label[10];
        sprintf(label, "%d", x);
        outtextxy(screenX - 10, ORIGIN_Y + 10, label);
    }

    // Y-axis ticks
    double bottom = -ORIGIN_Y / actual_scale;
    double top = (SCREEN_HEIGHT - ORIGIN_Y) / actual_scale;
    for (int y = ceil(bottom); y <= floor(top); y++) {
        if (y == 0) continue;
        int screenY = ORIGIN_Y - y * actual_scale;
        line(ORIGIN_X - TICK_SIZE, screenY, ORIGIN_X + TICK_SIZE, screenY);
        char label[10];
        sprintf(label, "%d", y);
        outtextxy(ORIGIN_X + 10, screenY - 10, label);
    }
}

void updateScale(int delta) {
    double factor = (delta > 0) ? 1.1 : 0.9;
    actual_scale *= factor;
   
    // Limit zoom range
    if (actual_scale < 10) actual_scale = 10;
    if (actual_scale > 200) actual_scale = 200;
}

double calculateFunction(double x) {
    switch (func_type) {
        case 1: return fp.a * x + fp.b;
        case 2: return fp.a * x * x + fp.b * x + fp.c;
        case 3: return fp.a * x * x * x + fp.b * x * x + fp.c * x + fp.d;
        case 4: return fp.a * sin(fp.b * x);
        case 5: return fp.a * exp(fp.b * x);
        case 6: return fp.a * log(fp.b * x);
        default: return 0;
    }
}

void getFunctionInput() {
    printf("Available functions:\n");
    printf("1. Linear (ax + b)\n");
    printf("2. Quadratic (ax2 + bx + c)\n");
    printf("3. Cubic (ax3 + bx2 + cx + d)\n");
    printf("4. Sinusoidal (asin(bx))\n");
    printf("5. Exponential (ae^(bx))\n");
    printf("6. Logarithmic (alog(bx))\n\n");
    printf("Select function (1-6): ");
    scanf("%d", &func_type);

    switch (func_type) {
        case 1:
            printf("Enter slope (a) and intercept (b): ");
            scanf("%lf %lf", &fp.a, &fp.b);
            sprintf(fp.equation, "y = %.2fx + %.2f", fp.a, fp.b);
            break;

        case 2:
            printf("Enter a, b, c: ");
            scanf("%lf %lf %lf", &fp.a, &fp.b, &fp.c);
            sprintf(fp.equation, "y = %.2fx² + %.2fx + %.2f", fp.a, fp.b, fp.c);
            break;

        case 3:
            printf("Enter a, b, c, d: ");
            scanf("%lf %lf %lf %lf", &fp.a, &fp.b, &fp.c, &fp.d);
            sprintf(fp.equation, "y = %.2fx³ + %.2fx² + %.2fx + %.2f",
                    fp.a, fp.b, fp.c, fp.d);
            break;

        case 4:
            printf("Enter amplitude (a) and frequency (b): ");
            scanf("%lf %lf", &fp.a, &fp.b);
            sprintf(fp.equation, "y = %.2f·sin(%.2fx)", fp.a, fp.b);
            break;

        case 5:
            printf("Enter coefficient (a) and exponent factor (b): ");
            scanf("%lf %lf", &fp.a, &fp.b);
            sprintf(fp.equation, "y = %.2f·e^{%.2fx}", fp.a, fp.b);
            break;

        case 6:
            printf("Enter coefficient (a) and base multiplier (b): ");
            scanf("%lf %lf", &fp.a, &fp.b);
            sprintf(fp.equation, "y = %.2f·log(%.2fx)", fp.a, fp.b);
            break;

        default:
            printf("Invalid choice! Using linear function.\n");
            func_type = 1;
            fp.a = 1; fp.b = 0;
            strcpy(fp.equation, const_cast<char*>("y = x"));
    }

    fp.color = COLOR(0, 255, 0);
}


void plotFunction() {
    int prev_x = 0;
    double prev_y_world = calculateFunction((0 - ORIGIN_X) / actual_scale);
    int prev_y = ORIGIN_Y - static_cast<int>(prev_y_world * actual_scale);

    for (int x_pixel = 0; x_pixel < SCREEN_WIDTH; x_pixel++) {
        double x_world = (x_pixel - ORIGIN_X) / actual_scale;
        double y_world = calculateFunction(x_world);

        if (func_type == 6 && x_world <= 0) {
            prev_x = x_pixel;
            continue;
        }

        int y_pixel = ORIGIN_Y - static_cast<int>(y_world * actual_scale);
        y_pixel = (y_pixel < 0) ? 0 : (y_pixel >= SCREEN_HEIGHT) ? SCREEN_HEIGHT - 1 : y_pixel;

        if (abs(y_pixel - prev_y) < MAX_JUMP && x_pixel > 0) {
            setcolor(fp.color);
            line(prev_x, prev_y, x_pixel, y_pixel);
        }

        prev_x = x_pixel;
        prev_y = y_pixel;
    }
}

void drawTitle() {
    setcolor(WHITE);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    outtextxy(10, 10, fp.equation);
}
