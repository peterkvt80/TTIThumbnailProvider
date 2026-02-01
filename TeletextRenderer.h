#pragma once

#include <windows.h>
#include <vector>
#include <string>
#include <map>

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
    
    // National character option (0-12)
    // 0=English, 1=German, 2=Swedish/Finnish, 3=Italian, 4=French,
    // 5=Portuguese/Spanish, 6=Czech/Slovak, 7=Romanian, 8=Serbian/Croatian/Slovenian,
    // 9=Estonian, 10=Lettish/Lithuanian, 11=Polish, 12=Turkish
    int m_nationalOption;
    
    // National character mapping tables (character code -> Unicode)
    // All tables initialized with English mapping, can be updated for other languages
    std::map<uint8_t, wchar_t> m_nationalMaps[13];
    
    // Helper methods
    void InitializeNationalMaps();
    void ParseLine(const std::string& line, int rowIndex);
    COLORREF GetColorRef(TeletextColor color);
    void DrawCell(HDC hdc, int row, int col, int cellWidth, int cellHeight, int displayRow, bool hasFont2, bool hasFont4, bool allowDoubleHeight);
    wchar_t GetGraphicsChar(uint8_t code, bool separated);
    wchar_t ApplyNationalCharMap(uint8_t ch);
};
