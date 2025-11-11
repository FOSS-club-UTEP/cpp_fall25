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

// --- Fixed helpers ---

struct LeakBuffer {
    int n;
    int* data;

    LeakBuffer(int n)
        : n(n), data(new int[n]) {
        // alloc on construction
    }

    ~LeakBuffer() {
        delete[] data;     // release internal buffer
    }

    void touch() {
        if (n > 0) data[0] = 7;
    }
};

static std::vector<int*> g_foodLeaks;

void buggyWrite(int* arr, int len) {
    if (len > 0) {
        arr[len - 1] = 123;   // in-bounds write
    }
}

int main() {
    InitWindow(W * SIZE, H * SIZE, "Snake + ASan Lab");
    SetTargetFPS(60);

    // AUDIO: initialize audio device
    InitAudioDevice();

    // AUDIO: load and start UTEP fight song (looped music stream)
    Music fightSong = LoadMusicStream("utep_fight_song.mp3");
    PlayMusicStream(fightSong);

    std::deque<Cell> snake;
    snake.push_front({W / 2, H / 2});

    Cell dir{1, 0};
    Cell food{rx(rng), ry(rng)};
    float stepTime = 0.12f;
    float acc = 0.0f;
    bool alive = true;

    LeakBuffer leaky(16);
    leaky.touch();

    while (!WindowShouldClose()) {
        // AUDIO: update music each frame
        UpdateMusicStream(fightSong);

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

            if (alive && head.x == food.x && head.y == food.y) {
                // ate food -> grow
                food = {rx(rng), ry(rng)};

                int* chunk = new int[256];
                chunk[0] = (int)snake.size();
                g_foodLeaks.push_back(chunk);

                if (snake.size() % 5 == 0) {
                    int* small = new int[4];
                    buggyWrite(small, 4);
                    delete[] small;
                }
            } else {
                snake.pop_back();
            }
        }

        BeginDrawing();
        ClearBackground(Color{18, 18, 18, 255});

        // food
        DrawRectangle(food.x * SIZE, food.y * SIZE, SIZE, SIZE, WHITE);

        // snake
        for (size_t i = 0; i < snake.size(); ++i) {
            auto c = snake[i];
            DrawRectangle(c.x * SIZE, c.y * SIZE, SIZE, SIZE,
                          i == 0 ? RED : MAROON);
        }

        if (!alive) {
            DrawText("Game Over - Press ESC", 20, 20, 20, RAYWHITE);
        }

        EndDrawing();
    }

    // free chunks
    for (int* p : g_foodLeaks) {
        delete[] p;
    }
    g_foodLeaks.clear();

    // AUDIO: cleanup music + audio device
    UnloadMusicStream(fightSong);
    CloseAudioDevice();

    CloseWindow();
    return 0;
}
