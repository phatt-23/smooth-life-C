#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#define WIDTH  230
#define HEIGHT 100
char level[] = " `.-=:rceoaS$#";
#define level_count ((float)sizeof(level)/sizeof(*level) - 1)
float grid     [HEIGHT][WIDTH] = {0};
float grid_diff[HEIGHT][WIDTH] = {0};

const float ra = 11; // r_a - the radius of the outer circle
const float ri = 7; // r_i - the raius of the inner circle
float area_ri = ri*ri; // the power of the inner circle's radius
float area_ra = ra*ra; // the power of the outer circle's radius
float M = 0; // the area of the inner circle
float N = 0; // the area of the outer circle

// magic values from the SmoothLife paper
const float alpha = 0.028; 
const float b1 = 0.278;
const float b2 = 0.365;
const float d1 = 0.267;
const float d2 = 0.445;
const float dt = 0.1; // difference step

void compute_discrete_areas() {
    for(int dy = -(ra - 1); dy < ra; ++dy) {
        for(int dx = -(ra - 1); dx < ra; ++dx) {
            int dist_2 = dx*dx + dy*dy;
            if(dist_2 <= area_ri) M += 1;
            else if(dist_2 <= area_ra) N += 1;
        }
    }
}

float rand_float() {
    return (float)rand()/(float)RAND_MAX;
}

void random_grid() {
    for(size_t i = 2*HEIGHT/7; i < 5*HEIGHT/7; ++i) {
        for(size_t j = 2*WIDTH/7; j < 5*WIDTH/7; ++j) {
            grid[i][j] = rand_float();
        }
    }
}

void display_grid(float t_grid[HEIGHT][WIDTH]) {
    for(size_t i = 0; i < HEIGHT; ++i) {
        for(size_t j = 0; j < WIDTH; ++j) {
            char c = level[(int)(t_grid[i][j]*(level_count - 1))];
            fputc(c, stdout);
            fputc(c, stdout);
        }
        fputc('\n', stdout);
    }
}

int emod(int a, int b) {
    return (a % b + b) % b;
}

float sigma_1(float x, float a) {
    return 1.0f / (1.0f + expf(-(x - a)*4.0f / alpha));
}

float sigma_2(float x, float a, float b) {
    return sigma_1(x, a) * (1.0f - sigma_1(x, b));
}

float sigma_m(float x, float y, float m) {
    return x * (1 - sigma_1(m, 0.5f)) + y * sigma_1(m, 0.5f);
}

float fn_s(float n, float m) {
    return sigma_2(n, sigma_m(b1, d1, m), sigma_m(b2, d2, m));
}

void compute_grid_diff() {
    for(int cy = 0; cy < HEIGHT; ++cy) {
        for(int cx = 0; cx < WIDTH; ++cx) {
            float m = 0;
            float n = 0;
            for(int dy = -(ra - 1); dy < ra; ++dy) {
                for(int dx = -(ra - 1); dx < ra; ++dx) {
                    int y = emod(cy + dy, HEIGHT); 
                    int x = emod(cx + dx, WIDTH);
                    int dist_2 = dx*dx + dy*dy;
                    if(dist_2 <= area_ri) 
                        m += grid[y][x];
                    else if(dist_2 <= area_ra) 
                        n += grid[y][x];
                }
            }
            m /= M;
            n /= N;
            float q = fn_s(n, m);
            grid_diff[cy][cx] = 2*q - 1;
        }
    }
}

void clamp(float* x, float l, float h) {
    if(*x < l) *x = l;
    if(*x > h) *x = h;
}

int main() {
    srand(time(0));
    compute_discrete_areas();
    random_grid();
    while(1) {
        printf("\e[1;1H\e[2J");
        compute_grid_diff();
        for(size_t y = 0; y < HEIGHT; ++y) {
            for(size_t x = 0; x < WIDTH; ++x) {
                grid[y][x] += dt*grid_diff[y][x];
                clamp(&grid[y][x], 0, 1);
            }
        }
        display_grid(grid);
    }

    return 0;
}
