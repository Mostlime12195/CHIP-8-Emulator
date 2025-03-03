#include "chip8.h"
#include "raylib.h"
#include <iostream>

constexpr int WINDOW_WIDTH = 640;
constexpr int WINDOW_HEIGHT = 320;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <ROM file>" << std::endl;
        return 1;
    }

    Chip8 emulator;

    bool success = emulator.LoadROM(argv[1]); // Load the ROM file

    if (success == false)
    {
        std::cerr << "Failed to open ROM file: " << argv[1] << std::endl;
        return 1;
    }

    emulator.InitialiseFontset();

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "CHIP-8");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        emulator.UpdateTimers();
        // Execute multiple opcodes per frame as needed
        // 10 opcodes per frame is standard. Increase for faster
        // loops, decrease for slower loops.
        for (int i = 0; i < 10; i++)
        {
            emulator.ExecuteNextOpcode();
        }

        BeginDrawing();
        ClearBackground(BLACK);

        const auto &screen = emulator.GetScreen();
        for (int y = 0; y < SCREEN_HEIGHT_PIXELS; y++)
        {
            for (int x = 0; x < SCREEN_WIDTH_PIXELS; x++)
            {
                int idx = y * SCREEN_WIDTH_PIXELS + x;
                Color color = (screen[idx] == 1) ? WHITE : BLACK;
                DrawRectangle(x * DISPLAY_SCALE, y * DISPLAY_SCALE, DISPLAY_SCALE, DISPLAY_SCALE, color);
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}