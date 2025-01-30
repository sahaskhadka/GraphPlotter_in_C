#include <graphics.h>
#include <cmath>
#include <stdio.h>
#include <string.h>
#include <conio.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define MAX_JUMP 100

struct FunctionParams {
    double a, b, c, d;
    char equation[100];
    int color;
} fp;

int func_type = 0;
double actual_scale = 50.0;
int origin_x = SCREEN_WIDTH / 2;
int origin_y = SCREEN_HEIGHT / 2;

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
    initwindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Advanced Graph Plotter");
    
    bool is_dragging = false;
    int prev_mx = 0, prev_my = 0;
    
    while (true) {
        // Handle mouse input
        if (ismouseclick(WM_LBUTTONDOWN)) {
            prev_mx = mousex();
            prev_my = mousey();
            is_dragging = true;
            clearmouseclick(WM_LBUTTONDOWN);
        }
        if (ismouseclick(WM_LBUTTONUP)) {
            is_dragging = false;
            clearmouseclick(WM_LBUTTONUP);
        }
        if (is_dragging) {
            int current_mx = mousex();
            int current_my = mousey();
            int delta_x = current_mx - prev_mx;
            int delta_y = current_my - prev_my;
            origin_x += delta_x;
            origin_y += delta_y;
            prev_mx = current_mx;
            prev_my = current_my;
        }

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
    line(0, origin_y, SCREEN_WIDTH, origin_y);  // X-axis
    line(origin_x, 0, origin_x, SCREEN_HEIGHT); // Y-axis
    drawAxisTicks();

    // X-axis arrow
    line(SCREEN_WIDTH - 10, origin_y - 10, SCREEN_WIDTH, origin_y);
    line(SCREEN_WIDTH - 10, origin_y + 10, SCREEN_WIDTH, origin_y);
    outtextxy(SCREEN_WIDTH - 20, origin_y - 20, "X");

    // Y-axis arrow
    line(origin_x - 10, 10, origin_x, 0);
    line(origin_x + 10, 10, origin_x, 0);
    outtextxy(origin_x + 10, 10, "Y");
}

void drawGrid() {
    setcolor(DARKGRAY);
    const int MAJOR_STEP = 1;
    const int MINOR_DIVISIONS = 5;

    // Vertical grid lines
    double left = (0.0 - origin_x) / actual_scale;
    double right = (SCREEN_WIDTH - origin_x) / actual_scale;
    for (int x = ceil(left); x <= floor(right); x++) {
        int screenX = origin_x + x * actual_scale;
        if (x % MAJOR_STEP == 0) {
            for (int i = 1; i < MINOR_DIVISIONS; i++) {
                int minorX = screenX + (i * actual_scale) / MINOR_DIVISIONS;
                line(minorX, 0, minorX, SCREEN_HEIGHT);
            }
        }
    }

    // Horizontal grid lines
    double bottom = (origin_y - SCREEN_HEIGHT) / actual_scale;
    double top = origin_y / actual_scale;
    for (int y = ceil(bottom); y <= floor(top); y++) {
        int screenY = origin_y - y * actual_scale;
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
    double left = (0.0 - origin_x) / actual_scale;
    double right = (SCREEN_WIDTH - origin_x) / actual_scale;
    for (int x = ceil(left); x <= floor(right); x++) {
        if (x == 0) continue;
        int screenX = origin_x + x * actual_scale;
        line(screenX, origin_y - TICK_SIZE, screenX, origin_y + TICK_SIZE);
        char label[10];
        sprintf(label, "%d", x);
        outtextxy(screenX - 10, origin_y + 10, label);
    }

    // Y-axis ticks
    double bottom = (origin_y - SCREEN_HEIGHT) / actual_scale;
    double top = origin_y / actual_scale;
    for (int y = ceil(bottom); y <= floor(top); y++) {
        if (y == 0) continue;
        int screenY = origin_y - y * actual_scale;
        line(origin_x - TICK_SIZE, screenY, origin_x + TICK_SIZE, screenY);
        char label[10];
        sprintf(label, "%d", y);
        outtextxy(origin_x + 10, screenY - 10, label);
    }
}

void updateScale(int delta) {
    double factor = (delta > 0) ? 1.1 : 0.9;
    actual_scale *= factor;
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

void plotFunction() {
    int prev_x = 0;
    double prev_y_world = calculateFunction((0 - origin_x) / actual_scale);
    int prev_y = origin_y - static_cast<int>(prev_y_world * actual_scale);

    for (int x_pixel = 0; x_pixel < SCREEN_WIDTH; x_pixel++) {
        double x_world = (x_pixel - origin_x) / actual_scale;
        double y_world = calculateFunction(x_world);

        if (func_type == 6 && x_world <= 0) {
            prev_x = x_pixel;
            continue;
        }

        int y_pixel = origin_y - static_cast<int>(y_world * actual_scale);
        y_pixel = (y_pixel < 0) ? 0 : (y_pixel >= SCREEN_HEIGHT) ? SCREEN_HEIGHT - 1 : y_pixel;

        if (abs(y_pixel - prev_y) < MAX_JUMP && x_pixel > 0) {
            setcolor(fp.color);
            line(prev_x, prev_y, x_pixel, y_pixel);
        }

        prev_x = x_pixel;
        prev_y = y_pixel;
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
            strcpy(fp.equation, "y = x");
    }

    fp.color = COLOR(0, 255, 0);
}

void drawTitle() {
    setcolor(WHITE);
    settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
    outtextxy(10, 10, fp.equation);
}
