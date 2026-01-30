#pragma once

#include <windows.h>
#include <vector>
#include <string>

// Teletext character cell dimensions
const int CHAR_WIDTH = 12;
const int CHAR_HEIGHT = 20;
const int SCREEN_COLS = 40;
const int SCREEN_ROWS = 25;

// Teletext colors (standard palette)
enum TeletextColor
{
    BLACK = 0,
    RED = 1,
    GREEN = 2,
    YELLOW = 3,
    BLUE = 4,
    MAGENTA = 5,
    CYAN = 6,
    WHITE = 7
};

// Character cell with attributes
struct TeletextCell
{
    wchar_t character;
    TeletextColor foreground;
    TeletextColor background;
    bool doubleHeight;
    bool graphics;
    bool separated;
    bool held;
    
    TeletextCell() : 
        character(L' '), 
        foreground(WHITE), 
        background(BLACK),
        doubleHeight(false),
        graphics(false),
        separated(false),
        held(false)
    {}
};

class TeletextPage
{
public:
    TeletextPage();
    
    // Parse TTI format data
    bool ParseTTI(const std::vector<uint8_t>& data);
    
    // Parse EP1 format data
    bool ParseEP1(const std::vector<uint8_t>& data);
    
    // Render page to bitmap
    HBITMAP RenderToBitmap(UINT width, UINT height);
    
private:
    TeletextCell m_cells[SCREEN_ROWS][SCREEN_COLS];
    
    // Helper methods
    void ParseLine(const std::string& line, int rowIndex);
    COLORREF GetColorRef(TeletextColor color);
    void DrawCell(HDC hdc, int row, int col, int cellWidth, int cellHeight, int displayRow, bool hasFont2, bool hasFont4, bool allowDoubleHeight);
    wchar_t GetGraphicsChar(uint8_t code, bool separated);
};
