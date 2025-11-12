#include "raylib.h"
#include <deque>
#include <random>
#include <vector>

struct Cell { int x, y; };

static constexpr int W = 32;
static constexpr int H = 24;
static constexpr int SIZE = 24;

// RNG for food
static std::mt19937 rng{12345};
static std::uniform_int_distribution<int> rx(0, W - 1);
static std::uniform_int_distribution<int> ry(0, H - 1);

// --- Intentional bug helpers ---


struct LeakBuffer {
    int n;
    int* data;

    LeakBuffer(int n)
        : n(n), data(new int[n]) {
    }

    ~LeakBuffer() {

    }

    void touch() {
        // safe write if used correctly
        if (n > 0) data[0] = 7;
    }
};

// Vector to track each food we find per-food allocations 
static std::vector<int*> g_foodLeaks; 

// Useless Write  
void buggyWrite(int* arr, int len) {
    if (len > 0) {
        arr[len] = 123; 
    }
}

// Global leaked object
static LeakBuffer* g_leaky = nullptr;

int main() {
    InitWindow(W * SIZE, H * SIZE, "Snake + ASan Lab");
    SetTargetFPS(60);

    std::deque<Cell> snake;
    snake.push_front({W / 2, H / 2});

    Cell dir{1, 0};
    Cell food{rx(rng), ry(rng)};
    float stepTime = 0.12f;
    float acc = 0.0f;
    bool alive = true;

    
    g_leaky = new LeakBuffer(16);
    g_leaky->touch();

    while (!WindowShouldClose()) {
        // input
        if (IsKeyPressed(KEY_UP)    && dir.y == 0) dir = { 0,-1};
        if (IsKeyPressed(KEY_DOWN)  && dir.y == 0) dir = { 0, 1};
        if (IsKeyPressed(KEY_LEFT)  && dir.x == 0) dir = {-1, 0};
        if (IsKeyPressed(KEY_RIGHT) && dir.x == 0) dir = { 1, 0};

        acc += GetFrameTime();
        if (alive && acc >= stepTime) {
            acc = 0.0f;

            // move head
            Cell head = snake.front();
            head.x = (head.x + dir.x + W) % W;
            head.y = (head.y + dir.y + H) % H;

            // self-collision
            for (auto& c : snake) {
                if (c.x == head.x && c.y == head.y) {
                    alive = false;
                    break;
                }
            }

            snake.push_front(head);

            // We found food!
            if (alive && head.x == food.x && head.y == food.y) {
                // ate food -> grow
                food = {rx(rng), ry(rng)};

                
                int* chunk = new int[256];
                chunk[0] = (int)snake.size();
                g_foodLeaks.push_back(chunk); 

                // trigger something important obviously
                if (snake.size() % 5 == 0) {
                    int* small = new int[1];
                    buggyWrite(small, 1); // writes
                    delete [] small; 
                    
                }
            } else {
                snake.pop_back();
            }
        }

        BeginDrawing();
        ClearBackground(Color{18, 18, 18, 255});

        // draw food
        DrawRectangle(food.x * SIZE, food.y * SIZE, SIZE, SIZE, WHITE);

        // draw snake
        for (size_t i = 0; i < snake.size(); ++i) {
            auto c = snake[i];
            // Head red, body darker red
            DrawRectangle(c.x * SIZE, c.y * SIZE, SIZE, SIZE, i == 0 ? RED : MAROON);
        }


        if (!alive) {
            DrawText(TextFormat("Score: %d", (int) g_foodLeaks.size()), 20, 20, 20, RAYWHITE);
            DrawText("Game Over - Press ESC", 20, 50, 20, RAYWHITE);
        }


        EndDrawing();
    }

    // BUG: 

    CloseWindow();
    return 0;
}
