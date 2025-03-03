#pragma once
#include "raylib.h"
#include <vector>
#include <array>
#include <stack>
#include <cstdint>
#include <string>
#include <fstream>
#include <random>
#include <iostream>

constexpr int MEMORY_SIZE = 4096;
constexpr int SCREEN_WIDTH_PIXELS = 64;
constexpr int SCREEN_HEIGHT_PIXELS = 32;
constexpr int DISPLAY_SCALE = 10;

class Chip8
{
public:
    Chip8();
    ~Chip8();

    // Loads a ROM file into memory.
    // Returns true if the ROM was loaded successfully.
    bool LoadROM(const char *romPath);

    // Initializes the built-in fontset into memory.
    void InitialiseFontset();

    // Executes the next opcode in memory.
    void ExecuteNextOpcode();

    // Updates the delay and sound timers.
    void UpdateTimers();

    // Returns a const reference to the screen buffer for drawing.
    const std::array<uint8_t, SCREEN_WIDTH_PIXELS * SCREEN_HEIGHT_PIXELS> &GetScreen() const { return m_screen; }

private:
    // 4KB of memory
    std::array<uint8_t, MEMORY_SIZE> m_memory{};

    // Monochrome display buffer. Each element represents a pixel.
    std::array<uint8_t, SCREEN_WIDTH_PIXELS * SCREEN_HEIGHT_PIXELS> m_screen{};

    // CPU registers (V0 to VF)
    std::array<uint8_t, 16> m_registers{};

    // Memory index register.
    uint16_t m_I = 0;

    // Program counter, starts at 0x200.
    uint16_t m_pc = 0x200;

    // Delay timer.
    uint8_t m_delayTimer = 0;

    // Sound timer.
    uint8_t m_soundTimer = 0;

    // Stack for storing return addresses.
    std::stack<uint16_t> m_stack;

    // Key mapping for CHIP-8 input.
    std::array<uint8_t, 16> m_keymap{
        KEY_X, KEY_ONE, KEY_TWO, KEY_THREE,
        KEY_Q, KEY_W, KEY_E, KEY_A,
        KEY_S, KEY_D, KEY_Z, KEY_C,
        KEY_FOUR, KEY_R, KEY_F, KEY_V};

    // --- Opcode decoding helpers ---
    // Dispatches opcode decoding to the correct helper function.
    void DecodeOpcode(uint16_t opcode);
    // Opcode groups
    void Opcode0(uint16_t opcode);
    void Opcode1(uint16_t opcode);
    void Opcode2(uint16_t opcode);
    void Opcode3(uint16_t opcode);
    void Opcode4(uint16_t opcode);
    void Opcode5(uint16_t opcode);
    void Opcode6(uint16_t opcode);
    void Opcode7(uint16_t opcode);
    void Opcode8(uint16_t opcode);
    void Opcode9(uint16_t opcode);
    void OpcodeA(uint16_t opcode);
    void OpcodeB(uint16_t opcode);
    void OpcodeC(uint16_t opcode);
    void OpcodeD(uint16_t opcode);
    void OpcodeE(uint16_t opcode);
    void OpcodeF(uint16_t opcode);
};

// Constructor: Initialize all state.
Chip8::Chip8()
{
    m_memory.fill(0);
    m_screen.fill(0);
    m_registers.fill(0);
    m_pc = 0x200; // Program counter starts at 0x200.
    m_delayTimer = 0;
    m_soundTimer = 0;
}

// Destructor: No dynamic memory to free.
Chip8::~Chip8()
{
}

// Loads the ROM from file into memory starting at address 0x200.
bool Chip8::LoadROM(const char *romPath)
{
    std::ifstream file(romPath, std::ios::binary | std::ios::ate | std::ios::in);
    if (!file.is_open())
    {
        std::cerr << "Failed to open ROM file: " << romPath << std::endl;
        return false;
    }
    std::streampos size = file.tellg();
    if (size <= 0)
    {
        std::cerr << "ROM file is empty or unreadable." << std::endl;
        return false;
    }
    std::vector<char> buffer(static_cast<size_t>(size));
    file.seekg(0, std::ios::beg);
    file.read(buffer.data(), size);
    file.close();

    // Load ROM into memory starting at address 512 (0x200).
    for (size_t i = 0; i < buffer.size(); i++)
    {
        m_memory[i + 512] = static_cast<uint8_t>(buffer[i]);
    }
    return true;
}

// Loads the built-in CHIP-8 fontset into memory starting at 0x50.
void Chip8::InitialiseFontset()
{
    const unsigned char startIndex = 0x50;
    m_memory[startIndex + 0] = 0xF0;
    m_memory[startIndex + 1] = 0x90;
    m_memory[startIndex + 2] = 0x90;
    m_memory[startIndex + 3] = 0x90;
    m_memory[startIndex + 4] = 0xF0; // 0

    m_memory[startIndex + 5] = 0x20;
    m_memory[startIndex + 6] = 0x60;
    m_memory[startIndex + 7] = 0x20;
    m_memory[startIndex + 8] = 0x20;
    m_memory[startIndex + 9] = 0x70; // 1

    m_memory[startIndex + 10] = 0xF0;
    m_memory[startIndex + 11] = 0x10;
    m_memory[startIndex + 12] = 0xF0;
    m_memory[startIndex + 13] = 0x80;
    m_memory[startIndex + 14] = 0xF0; // 2

    m_memory[startIndex + 15] = 0xF0;
    m_memory[startIndex + 16] = 0x10;
    m_memory[startIndex + 17] = 0xF0;
    m_memory[startIndex + 18] = 0x10;
    m_memory[startIndex + 19] = 0xF0; // 3

    m_memory[startIndex + 20] = 0x90;
    m_memory[startIndex + 21] = 0x90;
    m_memory[startIndex + 22] = 0xF0;
    m_memory[startIndex + 23] = 0x10;
    m_memory[startIndex + 24] = 0x10; // 4

    m_memory[startIndex + 25] = 0xF0;
    m_memory[startIndex + 26] = 0x80;
    m_memory[startIndex + 27] = 0xF0;
    m_memory[startIndex + 28] = 0x10;
    m_memory[startIndex + 29] = 0xF0; // 5

    m_memory[startIndex + 30] = 0xF0;
    m_memory[startIndex + 31] = 0x80;
    m_memory[startIndex + 32] = 0xF0;
    m_memory[startIndex + 33] = 0x90;
    m_memory[startIndex + 34] = 0xF0; // 6

    m_memory[startIndex + 35] = 0xF0;
    m_memory[startIndex + 36] = 0x10;
    m_memory[startIndex + 37] = 0x20;
    m_memory[startIndex + 38] = 0x40;
    m_memory[startIndex + 39] = 0x40; // 7

    m_memory[startIndex + 40] = 0xF0;
    m_memory[startIndex + 41] = 0x90;
    m_memory[startIndex + 42] = 0xF0;
    m_memory[startIndex + 43] = 0x90;
    m_memory[startIndex + 44] = 0xF0; // 8

    m_memory[startIndex + 45] = 0xF0;
    m_memory[startIndex + 46] = 0x90;
    m_memory[startIndex + 47] = 0xF0;
    m_memory[startIndex + 48] = 0x10;
    m_memory[startIndex + 49] = 0xF0; // 9

    m_memory[startIndex + 50] = 0xF0;
    m_memory[startIndex + 51] = 0x90;
    m_memory[startIndex + 52] = 0xF0;
    m_memory[startIndex + 53] = 0x90;
    m_memory[startIndex + 54] = 0x90; // A

    m_memory[startIndex + 55] = 0xE0;
    m_memory[startIndex + 56] = 0x90;
    m_memory[startIndex + 57] = 0xE0;
    m_memory[startIndex + 58] = 0x90;
    m_memory[startIndex + 59] = 0xE0; // B

    m_memory[startIndex + 60] = 0xF0;
    m_memory[startIndex + 61] = 0x80;
    m_memory[startIndex + 62] = 0x80;
    m_memory[startIndex + 63] = 0x80;
    m_memory[startIndex + 64] = 0xF0; // C

    m_memory[startIndex + 65] = 0xE0;
    m_memory[startIndex + 66] = 0x90;
    m_memory[startIndex + 67] = 0x90;
    m_memory[startIndex + 68] = 0x90;
    m_memory[startIndex + 69] = 0xE0; // D

    m_memory[startIndex + 70] = 0xF0;
    m_memory[startIndex + 71] = 0x80;
    m_memory[startIndex + 72] = 0xF0;
    m_memory[startIndex + 73] = 0x80;
    m_memory[startIndex + 74] = 0xF0; // E

    m_memory[startIndex + 75] = 0xF0;
    m_memory[startIndex + 76] = 0x80;
    m_memory[startIndex + 77] = 0xF0;
    m_memory[startIndex + 78] = 0x80;
    m_memory[startIndex + 79] = 0x80; // F
}

// ---------------------------------------------------------------------
// Opcode Execution: Break out the decoding over several functions.
// This first function fetches the opcode and dispatches to DecodeOpcode().
void Chip8::ExecuteNextOpcode()
{
    uint16_t opcode = (m_memory[m_pc] << 8) | m_memory[m_pc + 1];
    DecodeOpcode(opcode);
}

// Dispatches opcode decoding to the appropriate helper based on the high nibble.
void Chip8::DecodeOpcode(uint16_t opcode)
{
    switch (opcode & 0xF000)
    {
    case 0x0000:
        Opcode0(opcode);
        break;
    case 0x1000:
        Opcode1(opcode);
        break;
    case 0x2000:
        Opcode2(opcode);
        break;
    case 0x3000:
        Opcode3(opcode);
        break;
    case 0x4000:
        Opcode4(opcode);
        break;
    case 0x5000:
        Opcode5(opcode);
        break;
    case 0x6000:
        Opcode6(opcode);
        break;
    case 0x7000:
        Opcode7(opcode);
        break;
    case 0x8000:
        Opcode8(opcode);
        break;
    case 0x9000:
        Opcode9(opcode);
        break;
    case 0xA000:
        OpcodeA(opcode);
        break;
    case 0xB000:
        OpcodeB(opcode);
        break;
    case 0xC000:
        OpcodeC(opcode);
        break;
    case 0xD000:
        OpcodeD(opcode);
        break;
    case 0xE000:
        OpcodeE(opcode);
        break;
    case 0xF000:
        OpcodeF(opcode);
        break;
    default:
        // Unrecognized opcode; increment program counter to maintain execution.
        m_pc += 2;
        break;
    }
}

// ---------------------------------------------------------------------
// Opcode Group 0x0 (system instructions, clear/return)
void Chip8::Opcode0(uint16_t opcode)
{
    switch (opcode)
    {
    case 0x00E0: // Clear the screen.
        m_screen.fill(0);
        m_pc += 2;
        break;
    case 0x00EE: // Return from subroutine.
        if (!m_stack.empty())
        {
            m_pc = m_stack.top();
            m_stack.pop();
        }
        else
        {
            m_pc += 2; // Underflow error handling could be added.
        }
        break;
    default:
        // For undocumented 0x0NNN system calls, simply increment.
        m_pc += 2;
        break;
    }
}

// ---------------------------------------------------------------------
// 0x1NNN: Jump to address NNN.
void Chip8::Opcode1(uint16_t opcode)
{
    m_pc = opcode & 0x0FFF;
}

// ---------------------------------------------------------------------
// 0x2NNN: Call subroutine at address NNN.
void Chip8::Opcode2(uint16_t opcode)
{
    m_stack.push(m_pc + 2);
    m_pc = opcode & 0x0FFF;
}

// ---------------------------------------------------------------------
// 0x3XNN: Skip next instruction if Vx equals NN.
void Chip8::Opcode3(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t nn = opcode & 0x00FF;
    m_pc += (m_registers[x] == nn) ? 4 : 2;
}

// ---------------------------------------------------------------------
// 0x4XNN: Skip next instruction if Vx does not equal NN.
void Chip8::Opcode4(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t nn = opcode & 0x00FF;
    m_pc += (m_registers[x] != nn) ? 4 : 2;
}

// ---------------------------------------------------------------------
// 0x5XY0: Skip next instruction if Vx equals Vy.
void Chip8::Opcode5(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    m_pc += (m_registers[x] == m_registers[y]) ? 4 : 2;
}

// ---------------------------------------------------------------------
// 0x6XNN: Set Vx to NN.
void Chip8::Opcode6(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    m_registers[x] = opcode & 0x00FF;
    m_pc += 2;
}

// ---------------------------------------------------------------------
// 0x7XNN: Add NN to Vx (carry flag is not changed).
void Chip8::Opcode7(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    m_registers[x] += opcode & 0x00FF;
    m_pc += 2;
}

// ---------------------------------------------------------------------
// 0x8XY_: Arithmetic and logical operations.
void Chip8::Opcode8(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    uint8_t subcode = opcode & 0x000F;
    switch (subcode)
    {
    case 0x0: // Set Vx to Vy.
        m_registers[x] = m_registers[y];
        break;
    case 0x1: // Set Vx to Vx OR Vy.
        m_registers[x] |= m_registers[y];
        break;
    case 0x2: // Set Vx to Vx AND Vy.
        m_registers[x] &= m_registers[y];
        break;
    case 0x3: // Set Vx to Vx XOR Vy.
        m_registers[x] ^= m_registers[y];
        break;
    case 0x4: // Add Vy to Vx; set VF to 1 on carry.
    {
        uint16_t sum = m_registers[x] + m_registers[y];
        m_registers[0xF] = (sum > 0xFF) ? 1 : 0;
        m_registers[x] = static_cast<uint8_t>(sum);
        break;
    }
    case 0x5: // Vx -= Vy; set VF to 0 when borrow occurs.
        m_registers[0xF] = (m_registers[x] > m_registers[y]) ? 1 : 0;
        m_registers[x] -= m_registers[y];
        break;
    case 0x6: // Shift Vx right by one. VF is set to the least significant bit prior.
        m_registers[0xF] = m_registers[x] & 0x1;
        m_registers[x] >>= 1;
        break;
    case 0x7: // Set Vx to Vy minus Vx; set VF to 0 when borrow occurs.
        m_registers[0xF] = (m_registers[y] > m_registers[x]) ? 1 : 0;
        m_registers[x] = m_registers[y] - m_registers[x];
        break;
    case 0xE: // Shift Vx left by one. VF is set to the most significant bit before shifting.
        m_registers[0xF] = (m_registers[x] & 0x80) >> 7;
        m_registers[x] <<= 1;
        break;
    default:
        break;
    }
    m_pc += 2;
}

// ---------------------------------------------------------------------
// 0x9XY0: Skip next instruction if Vx does not equal Vy.
void Chip8::Opcode9(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    m_pc += (m_registers[x] != m_registers[y]) ? 4 : 2;
}

// ---------------------------------------------------------------------
// 0xANNN: Set I to address NNN.
void Chip8::OpcodeA(uint16_t opcode)
{
    m_I = opcode & 0x0FFF;
    m_pc += 2;
}

// ---------------------------------------------------------------------
// 0xBNNN: Jump to address NNN plus V0.
void Chip8::OpcodeB(uint16_t opcode)
{
    m_pc = (opcode & 0x0FFF) + m_registers[0];
}

// ---------------------------------------------------------------------
// 0xCXNN: Set Vx to (random byte AND NN).
void Chip8::OpcodeC(uint16_t opcode)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution dis(0, 256);

    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t nn = opcode & 0x00FF;
    // Using the Mersenne tittie twister algorithm to generate random numbers.
    m_registers[x] = (dis(gen)) & nn;
    m_pc += 2;
}

// ---------------------------------------------------------------------
// 0xDXYN: Draw sprite at (Vx, Vy) with height N. Set VF to collision.
void Chip8::OpcodeD(uint16_t opcode)
{
    uint8_t x = m_registers[(opcode & 0x0F00) >> 8];
    uint8_t y = m_registers[(opcode & 0x00F0) >> 4];
    uint8_t height = opcode & 0x000F;
    m_registers[0xF] = 0;

    // Loop through each row.
    for (int row = 0; row < height; row++)
    {
        uint8_t spriteByte = m_memory[m_I + row];
        // Loop through each bit in the row.
        for (int col = 0; col < 8; col++)
        {
            // Check if the current bit is set.
            if ((spriteByte & (0x80 >> col)) != 0)
            {
                // Calculate index in 1D screen array.
                int idx = ((y + row) % SCREEN_HEIGHT_PIXELS) * SCREEN_WIDTH_PIXELS + ((x + col) % SCREEN_WIDTH_PIXELS);
                // Check if a pixel collision occurred.
                if (m_screen[idx] == 1)
                {
                    m_registers[0xF] = 1;
                }
                m_screen[idx] ^= 1;
            }
        }
    }
    m_pc += 2;
}

// ---------------------------------------------------------------------
// 0xEX__ instructions, input related.
void Chip8::OpcodeE(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t key = m_registers[x];
    switch (opcode & 0x00FF)
    {
    case 0x9E: // Skip if key in Vx is pressed.
        m_pc += IsKeyDown(m_keymap[key]) ? 4 : 2;
        break;
    case 0xA1: // Skip if key in Vx is not pressed.
        m_pc += (!IsKeyDown(m_keymap[key])) ? 4 : 2;
        break;
    default:
        m_pc += 2;
        break;
    }
}

// ---------------------------------------------------------------------
// 0xFX__ instructions, miscellaneous functions.
void Chip8::OpcodeF(uint16_t opcode)
{
    uint8_t x = (opcode & 0x0F00) >> 8;
    switch (opcode & 0x00FF)
    {
    case 0x07: // Set Vx to delay timer value.
        m_registers[x] = m_delayTimer;
        m_pc += 2;
        break;
    case 0x0A: // Wait for a key press, and store it in Vx.
    {
        bool keyPressed = false;
        for (int i = 0; i < m_keymap.size(); i++)
        {
            if (IsKeyDown(m_keymap[i]))
            {
                m_registers[x] = i;
                keyPressed = true;
                break;
            }
        }
        if (!keyPressed)
            return; // Skip updating the pc to repeat this opcode.
        m_pc += 2;
        break;
    }
    case 0x15: // Set delay timer to Vx.
        m_delayTimer = m_registers[x];
        m_pc += 2;
        break;
    case 0x18: // Set sound timer to Vx.
        m_soundTimer = m_registers[x];
        m_pc += 2;
        break;
    case 0x1E: // Add Vx to I.
        m_I += m_registers[x];
        m_pc += 2;
        break;
    case 0x29:                             // Set I to the location of sprite for digit Vx.
        m_I = 0x50 + (m_registers[x] * 5); // Each digit sprite is 5 bytes.
        m_pc += 2;
        break;
    case 0x33: // Store BCD representation of Vx in memory locations I, I+1, and I+2.
    {
        uint8_t value = m_registers[x];
        m_memory[m_I] = value / 100;
        m_memory[m_I + 1] = (value / 10) % 10;
        m_memory[m_I + 2] = value % 10;
        m_pc += 2;
        break;
    }
    case 0x55: // Store registers V0 to Vx in memory starting at address I.
        for (int i = 0; i <= x; i++)
        {
            m_memory[m_I + i] = m_registers[i];
        }
        m_pc += 2;
        break;
    case 0x65: // Read registers V0 to Vx from memory starting at address I.
        for (int i = 0; i <= x; i++)
        {
            m_registers[i] = m_memory[m_I + i];
        }
        m_pc += 2;
        break;
    default:
        m_pc += 2;
        break;
    }
}

// ---------------------------------------------------------------------
// Updates the delay and sound timers.
void Chip8::UpdateTimers()
{
    if (m_delayTimer > 0)
        m_delayTimer--;
    if (m_soundTimer > 0)
        m_soundTimer--;
}