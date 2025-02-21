#include "raylib.h"
#include "C:/raylib/raylib/src/raymath.h"

#include <iostream>
#include <fstream>
#include <stack>
#include <cstring>
#include <filesystem>
#include <cstdlib> // For exit()

int SCREEN_WIDTH = 640;
int SCREEN_HEIGHT = 320;

unsigned char *memory = new unsigned char[4096]; // 4KB of memory
unsigned char *screen = new unsigned char[2048]; // 64x32 monochrome display

unsigned char registers[16]; // 16 8-bit registers
unsigned short I;            // 16-bit register used as a memory address register
unsigned short pc = 0x200;   // 16-bit program counter, starting at 0x200 because the first 512 bytes are reserved
unsigned char delayTimer;
unsigned char soundTimer;

int keymap[16] = {
    KEY_X,     // 0x0
    KEY_ONE,   // 0x1
    KEY_TWO,   // 0x2
    KEY_THREE, // 0x3
    KEY_Q,     // 0x4
    KEY_W,     // 0x5
    KEY_E,     // 0x6
    KEY_A,     // 0x7
    KEY_S,     // 0x8
    KEY_D,     // 0x9
    KEY_Z,     // 0xA
    KEY_C,     // 0xB
    KEY_FOUR,  // 0xC
    KEY_R,     // 0xD
    KEY_F,     // 0xE
    KEY_V      // 0xF
};

std::stack<unsigned short> stack;

void InitialiseFontset()
{
    const unsigned char startIndex = 0x50;
    memory[startIndex + 0] = 0xF0;
    memory[startIndex + 1] = 0x90;
    memory[startIndex + 2] = 0x90;
    memory[startIndex + 3] = 0x90;
    memory[startIndex + 4] = 0xF0; // 0
    memory[startIndex + 5] = 0x20;
    memory[startIndex + 6] = 0x60;
    memory[startIndex + 7] = 0x20;
    memory[startIndex + 8] = 0x20;
    memory[startIndex + 9] = 0x70; // 1
    memory[startIndex + 10] = 0xF0;
    memory[startIndex + 11] = 0x10;
    memory[startIndex + 12] = 0xF0;
    memory[startIndex + 13] = 0x80;
    memory[startIndex + 14] = 0xF0; // 2
    memory[startIndex + 15] = 0xF0;
    memory[startIndex + 16] = 0x10;
    memory[startIndex + 17] = 0xF0;
    memory[startIndex + 18] = 0x10;
    memory[startIndex + 19] = 0xF0; // 3
    memory[startIndex + 20] = 0x90;
    memory[startIndex + 21] = 0x90;
    memory[startIndex + 22] = 0xF0;
    memory[startIndex + 23] = 0x10;
    memory[startIndex + 24] = 0x10; // 4
    memory[startIndex + 25] = 0xF0;
    memory[startIndex + 26] = 0x80;
    memory[startIndex + 27] = 0xF0;
    memory[startIndex + 28] = 0x10;
    memory[startIndex + 29] = 0xF0; // 5
    memory[startIndex + 30] = 0xF0;
    memory[startIndex + 31] = 0x80;
    memory[startIndex + 32] = 0xF0;
    memory[startIndex + 33] = 0x90;
    memory[startIndex + 34] = 0xF0; // 6
    memory[startIndex + 35] = 0xF0;
    memory[startIndex + 36] = 0x10;
    memory[startIndex + 37] = 0x20;
    memory[startIndex + 38] = 0x40;
    memory[startIndex + 39] = 0x40; // 7
    memory[startIndex + 40] = 0xF0;
    memory[startIndex + 41] = 0x90;
    memory[startIndex + 42] = 0xF0;
    memory[startIndex + 43] = 0x90;
    memory[startIndex + 44] = 0xF0; // 8
    memory[startIndex + 45] = 0xF0;
    memory[startIndex + 46] = 0x90;
    memory[startIndex + 47] = 0xF0;
    memory[startIndex + 48] = 0x10;
    memory[startIndex + 49] = 0xF0; // 9
    memory[startIndex + 50] = 0xF0;
    memory[startIndex + 51] = 0x90;
    memory[startIndex + 52] = 0xF0;
    memory[startIndex + 53] = 0x90;
    memory[startIndex + 54] = 0x90; // A
    memory[startIndex + 55] = 0xE0;
    memory[startIndex + 56] = 0x90;
    memory[startIndex + 57] = 0xE0;
    memory[startIndex + 58] = 0x90;
    memory[startIndex + 59] = 0xE0; // B
    memory[startIndex + 60] = 0xF0;
    memory[startIndex + 61] = 0x80;
    memory[startIndex + 62] = 0x80;
    memory[startIndex + 63] = 0x80;
    memory[startIndex + 64] = 0xF0; // C
    memory[startIndex + 65] = 0xE0;
    memory[startIndex + 66] = 0x90;
    memory[startIndex + 67] = 0x90;
    memory[startIndex + 68] = 0x90;
    memory[startIndex + 69] = 0xE0; // D
    memory[startIndex + 70] = 0xF0;
    memory[startIndex + 71] = 0x80;
    memory[startIndex + 72] = 0xF0;
    memory[startIndex + 73] = 0x80;
    memory[startIndex + 74] = 0xF0; // E
    memory[startIndex + 75] = 0xF0;
    memory[startIndex + 76] = 0x80;
    memory[startIndex + 77] = 0xF0;
    memory[startIndex + 78] = 0x80;
    memory[startIndex + 79] = 0x80; // F
}

void ExecuteNextOpcode()
{
    unsigned short opcode = memory[pc] << 8 | memory[pc + 1];
    int vx, vy;
    unsigned short x, y, height, pixel;
    bool keyPress;

    // Decode and execute opcode
    switch (opcode & 0xF000)
    {
    case 0x0000:
        switch (opcode)
        {
        case 0x00E0:
            // Clear the screen
            for (int i = 0; i < 2048; i++)
            {
                screen[i] = 0;
            }

            break;
        case 0x00EE:
            // Return from subroutine
            if (stack.empty())
            {
                std::cerr << "Stack underflow!" << std::endl;
                exit(1);
            }
            pc = stack.top();
            stack.pop();
            return; // Skip the automatic PC increment
        default:
            break;
        }
        break;
    case 0x1000:
        // Jump to address nnn
        pc = opcode & 0x0FFF;
        return; // Skip the default pc increment
    case 0x2000:
        stack.push(pc + 2); // Push next instruction address
        pc = opcode & 0x0FFF;
        return;
    case 0x3000:
        // Skip next instruction if Vx == kk
        if (registers[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
        {
            pc += 2;
        }
        break;
    case 0x4000:
        if (registers[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
        {
            pc += 2;
        }
        break;
    case 0x5000:
        if (registers[(opcode & 0x0F00) >> 8] == registers[(opcode & 0x00F0) >> 4])
        {
            pc += 2;
        }
        break;
    case 0x6000:
        registers[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
        break;
    case 0x7000:
        registers[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
        break;
    case 0x8000:
        vx = (opcode & 0x0F00) >> 8;
        vy = (opcode & 0x00F0) >> 4;
        switch (opcode & 0x000F)
        {
        case 0x0000:
            registers[vx] = registers[vy];
            break;
        case 0x0001:
            registers[vx] |= registers[vy];
            break;
        case 0x0002:
            registers[vx] &= registers[vy];
            break;
        case 0x0003:
            registers[vx] ^= registers[vy];
            break;
        case 0x0004:
            registers[0xF] = (registers[vx] + registers[vy] > 255) ? 1 : 0;
            registers[vx] += registers[vy];
            break;
        case 0x0005:
            registers[0xF] = (registers[vx] > registers[vy]) ? 1 : 0;
            registers[vx] -= registers[vy];
            break;
        case 0x0006:
            registers[0xF] = (registers[vx] & 0x1) ? 1 : 0;
            registers[vx] >>= 1;
            break;
        case 0x0007:
            registers[0xF] = (registers[vy] > registers[vx]) ? 1 : 0;
            registers[vx] = registers[vy] - registers[vx];
            break;
        case 0x000E:
            registers[0xF] = (registers[vx] & 0x80) ? 1 : 0;
            registers[vx] <<= 1;
            break;
        default:
            break;
        }
        break;
    case 0x9000:
        if (registers[(opcode & 0x0F00) >> 8] != registers[(opcode & 0x00F0) >> 4])
        {
            pc += 2;
        }
        break;
    case 0xA000:
        I = opcode & 0x0FFF;
        break;
    case 0xB000:
        pc = (opcode & 0x0FFF) + registers[0];
        return;
    case 0xC000:
        registers[(opcode & 0x0F00) >> 8] = (rand() % 255) & (opcode & 0x00FF);
        break;
    case 0xD000:
        x = registers[(opcode & 0x0F00) >> 8];
        y = registers[(opcode & 0x00F0) >> 4];
        height = opcode & 0x000F;
        for (int yline = 0; yline < height; yline++)
        {
            pixel = memory[I + yline];
            for (int xline = 0; xline < 8; xline++)
            {
                if ((pixel & (0x80 >> xline)) != 0)
                {
                    if (screen[(x + xline + ((y + yline) * 64))] == 1)
                    {
                        registers[0xF] = 1;
                    }
                    screen[x + xline + ((y + yline) * 64)] ^= 1;
                }
            }
        }
        break;
    case 0xE000:
        switch (opcode & 0x00FF)
        {
        case 0x009E:
            if (IsKeyDown(keymap[registers[(opcode & 0x0F00) >> 8]]))
            {
                pc += 2;
            }
            break;
        case 0x00A1:
            if (!IsKeyDown(keymap[registers[(opcode & 0x0F00) >> 8]]))
            {
                pc += 2;
            }
            break;
        default:
            break;
        }
        break;
    case 0xF000:
        switch (opcode & 0x00FF)
        {
        case 0x0007:
            registers[(opcode & 0x0F00) >> 8] = delayTimer;
            break;
        case 0x000A:
            keyPress = false;
            for (int i = 0; i < 16; i++)
            {
                if (IsKeyDown(keymap[i]))
                {
                    registers[(opcode & 0x0F00) >> 8] = i;
                    keyPress = true;
                }
            }
            if (!keyPress)
            {
                return;
            }
            break;
        case 0x0015:
            delayTimer = registers[(opcode & 0x0F00) >> 8];
            break;
        case 0x0018:
            soundTimer = registers[(opcode & 0x0F00) >> 8];
            break;
        case 0x001E:
            I += registers[(opcode & 0x0F00) >> 8];
            break;
        case 0x0029:
            I = registers[(opcode & 0x0F00) >> 8] * 0x5;
            break;
        case 0x0033:
            memory[I] = registers[(opcode & 0x0F00) >> 8] / 100;
            memory[I + 1] = (registers[(opcode & 0x0F00) >> 8] / 10) % 10;
            memory[I + 2] = registers[(opcode & 0x0F00) >> 8] % 10;
            break;
        case 0x0055:
            for (int i = 0; i <= ((opcode & 0x0F00) >> 8); i++)
            {
                memory[I + i] = registers[i];
            }
            break;
        case 0x0065:
            for (int i = 0; i <= ((opcode & 0x0F00) >> 8); i++)
            {
                registers[i] = memory[I + i];
            }
            break;
        default:
            break;
        }
    }

    // Move to the next opcode (most instructions are 2 bytes long)
    pc += 2;
}

int main(int argc, char *argv[])
{

    memset(memory, 0, 4096);
    memset(screen, 0, 2048);

    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <ROM file>" << std::endl;
        return 1;
    }

    ChangeDirectory(GetApplicationDirectory());

    std::ifstream file(argv[1], std::ios::binary | std::ios::ate | std::ios::in);
    if (file.is_open())
    {
        std::streampos size = file.tellg();
        char *buffer = new char[size];
        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();

        for (int i = 0; i < size; i++)
        {
            memory[i + 512] = buffer[i];
        }

        delete[] buffer;
    }
    else
    {
        std::cerr << "Failed to open file: " << argv[1] << std::endl;
        return 1;
    }

    InitialiseFontset();

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "CHIP-8");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {

        if (delayTimer > 0)
        {
            delayTimer--;
        }
        if (soundTimer > 0)
        {
            soundTimer--;
        }

        for (int i = 0; i < 10; i++)
        {
            ExecuteNextOpcode();
        }

        // Draw
        BeginDrawing();

        ClearBackground(BLACK);

        for (int i = 0; i < 2048; i++)
        {
            int x = (i % 64) * 10;
            int y = (i / 64) * 10;
            if (screen[i] == 1)
            {
                DrawRectangle(x, y, 10, 10, WHITE);
            }
        }

        EndDrawing();
    }

    delete[] memory;
    delete[] screen;

    CloseWindow();
    return 0;
}