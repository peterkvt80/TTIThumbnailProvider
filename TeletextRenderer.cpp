#include "TeletextRenderer.h"
#include <sstream>
#include <algorithm>
#include <cstdio>

// External reference to DLL instance handle from DllMain.cpp
extern HINSTANCE g_hInst;

TeletextPage::TeletextPage()
{
    // Initialize all cells to defaults
    for (int row = 0; row < SCREEN_ROWS; row++)
    {
        for (int col = 0; col < SCREEN_COLS; col++)
        {
            m_cells[row][col] = TeletextCell();
        }
    }
    
    // Default to English national option
    m_nationalOption = 0;
    
    // Initialize national character maps
    InitializeNationalMaps();
}

void TeletextPage::InitializeNationalMaps()
{
    // Initialize all tables to empty first
    for (int i = 0; i < 13; i++)
    {
        m_nationalMaps[i].clear();
    }
    
    // 0 = English (National option 0)
    m_nationalMaps[0]['#'] = 0x00A3;  // £ pound sign
    m_nationalMaps[0]['['] = 0x2190;  // ← left arrow
    m_nationalMaps[0]['\\'] = 0x00BD; // ½ half
    m_nationalMaps[0][']'] = 0x2192;  // → right arrow
    m_nationalMaps[0]['^'] = 0x2191;  // ↑ up arrow
    m_nationalMaps[0]['_'] = 0x0023;  // # hash sign
    m_nationalMaps[0]['`'] = 0x2014;  // — em dash
    m_nationalMaps[0]['{'] = 0x00BC;  // ¼ quarter
    m_nationalMaps[0]['|'] = 0x2016;  // ‖ double pipe
    m_nationalMaps[0]['}'] = 0x00BE;  // ¾ three quarters
    m_nationalMaps[0]['~'] = 0x00F7;  // ÷ divide
    
    // 1 = German (National option 4 in PS encoding)
    m_nationalMaps[1]['#'] = 0x0023;  // # (not mapped in German, but wxTED shows £ -> #)
    m_nationalMaps[1]['$'] = 0x0024;  // $ dollar sign (not mapped)
    m_nationalMaps[1]['@'] = 0x00A7;  // § section sign
    m_nationalMaps[1]['['] = 0x00C4;  // Ä
    m_nationalMaps[1]['\\'] = 0x00D6; // Ö
    m_nationalMaps[1][']'] = 0x00DC;  // Ü
    m_nationalMaps[1]['^'] = 0x005E;  // ^ caret (not mapped)
    m_nationalMaps[1]['_'] = 0x005F;  // _ underscore (not mapped)
    m_nationalMaps[1]['`'] = 0x00B0;  // ° degree
    m_nationalMaps[1]['{'] = 0x00E4;  // ä
    m_nationalMaps[1]['|'] = 0x00F6;  // ö
    m_nationalMaps[1]['}'] = 0x00FC;  // ü
    m_nationalMaps[1]['~'] = 0x00DF;  // ß eszett
    
    // 2 = Swedish/Finnish (National option 2 in PS encoding)
    m_nationalMaps[2]['#'] = 0x0023;  // # (£ -> # in wxTED)
    m_nationalMaps[2]['$'] = 0x00A4;  // ¤ currency sign
    m_nationalMaps[2]['@'] = 0x00C9;  // É
    m_nationalMaps[2]['['] = 0x00C4;  // Ä
    m_nationalMaps[2]['\\'] = 0x00D6; // Ö
    m_nationalMaps[2][']'] = 0x00C5;  // Å
    m_nationalMaps[2]['^'] = 0x00DC;  // Ü
    m_nationalMaps[2]['_'] = 0x005F;  // _ underscore (not mapped)
    m_nationalMaps[2]['`'] = 0x00E9;  // é
    m_nationalMaps[2]['{'] = 0x00E4;  // ä
    m_nationalMaps[2]['|'] = 0x00F6;  // ö
    m_nationalMaps[2]['}'] = 0x00E5;  // å
    m_nationalMaps[2]['~'] = 0x00FC;  // ü
    
    // 3 = Italian (National option 6 in PS encoding)
    m_nationalMaps[3]['#'] = 0x00A3;  // £ pound
    m_nationalMaps[3]['$'] = 0x0024;  // $ (£ -> $ in wxTED)
    m_nationalMaps[3]['@'] = 0x00E9;  // é
    m_nationalMaps[3]['['] = 0x00B0;  // ° degree
    m_nationalMaps[3]['\\'] = 0x00E7; // ç
    m_nationalMaps[3][']'] = 0x2192;  // → right arrow
    m_nationalMaps[3]['^'] = 0x2191;  // ↑ up arrow
    m_nationalMaps[3]['_'] = 0x0023;  // #
    m_nationalMaps[3]['`'] = 0x00F9;  // ù
    m_nationalMaps[3]['{'] = 0x00E0;  // à
    m_nationalMaps[3]['|'] = 0x00F2;  // ò
    m_nationalMaps[3]['}'] = 0x00E8;  // è
    m_nationalMaps[3]['~'] = 0x00EC;  // ì
    
    // 4 = French (National option 1 in PS encoding)
    m_nationalMaps[4]['#'] = 0x00E9;  // é
    m_nationalMaps[4]['$'] = 0x00EF;  // ï
    m_nationalMaps[4]['@'] = 0x00E0;  // à
    m_nationalMaps[4]['['] = 0x00EB;  // ë
    m_nationalMaps[4]['\\'] = 0x00EA; // ê
    m_nationalMaps[4][']'] = 0x00F9;  // ù
    m_nationalMaps[4]['^'] = 0x00EE;  // î
    m_nationalMaps[4]['_'] = 0x0023;  // #
    m_nationalMaps[4]['`'] = 0x00E8;  // è
    m_nationalMaps[4]['{'] = 0x00E2;  // â
    m_nationalMaps[4]['|'] = 0x00F4;  // ô
    m_nationalMaps[4]['}'] = 0x00FB;  // û
    m_nationalMaps[4]['~'] = 0x00E7;  // ç
    
    // 5 = Portuguese/Spanish (National option 5 in PS encoding)
    m_nationalMaps[5]['#'] = 0x00E7;  // ç
    m_nationalMaps[5]['$'] = 0x0024;  // $ (£ -> $ in wxTED)
    m_nationalMaps[5]['@'] = 0x00A1;  // ¡
    m_nationalMaps[5]['['] = 0x00E1;  // á
    m_nationalMaps[5]['\\'] = 0x00E9; // é
    m_nationalMaps[5][']'] = 0x00ED;  // í
    m_nationalMaps[5]['^'] = 0x00F3;  // ó
    m_nationalMaps[5]['_'] = 0x00FA;  // ú
    m_nationalMaps[5]['`'] = 0x00BF;  // ¿
    m_nationalMaps[5]['{'] = 0x00FC;  // ü
    m_nationalMaps[5]['|'] = 0x00F1;  // ñ
    m_nationalMaps[5]['}'] = 0x00E8;  // è
    m_nationalMaps[5]['~'] = 0x00E0;  // à
    
    // 6 = Czech/Slovak (National option 3 in PS encoding)
    m_nationalMaps[6]['#'] = 0x0023;  // # (£ -> # in wxTED)
    m_nationalMaps[6]['$'] = 0x016F;  // ů
    m_nationalMaps[6]['@'] = 0x010D;  // č
    m_nationalMaps[6]['['] = 0x0165;  // ť
    m_nationalMaps[6]['\\'] = 0x017E; // ž
    m_nationalMaps[6][']'] = 0x00FD;  // ý
    m_nationalMaps[6]['^'] = 0x00ED;  // í
    m_nationalMaps[6]['_'] = 0x0159;  // ř
    m_nationalMaps[6]['`'] = 0x00E9;  // é
    m_nationalMaps[6]['{'] = 0x00E1;  // á
    m_nationalMaps[6]['|'] = 0x011B;  // ě
    m_nationalMaps[6]['}'] = 0x00FA;  // ú
    m_nationalMaps[6]['~'] = 0x0161;  // š
    
    // 7 = Romanian
    m_nationalMaps[7]['#'] = 0x0023;  // #
    m_nationalMaps[7]['$'] = 0x00A4;  // ¤
    m_nationalMaps[7]['@'] = 0x0162;  // Ţ
    m_nationalMaps[7]['['] = 0x00C2;  // Â
    m_nationalMaps[7]['\\'] = 0x015E; // Ş
    m_nationalMaps[7][']'] = 0x0102;  // Ă
    m_nationalMaps[7]['^'] = 0x00CE;  // Î
    m_nationalMaps[7]['_'] = 0x0131;  // ı
    m_nationalMaps[7]['`'] = 0x0163;  // ţ
    m_nationalMaps[7]['{'] = 0x00E2;  // â
    m_nationalMaps[7]['|'] = 0x015F;  // ş
    m_nationalMaps[7]['}'] = 0x0103;  // ă
    m_nationalMaps[7]['~'] = 0x00EE;  // î
    
    // 8 = Serbian/Croatian/Slovenian
    m_nationalMaps[8]['#'] = 0x0023;  // #
    m_nationalMaps[8]['$'] = 0x00CB;  // Ë
    m_nationalMaps[8]['@'] = 0x010C;  // Č
    m_nationalMaps[8]['['] = 0x0106;  // Ć
    m_nationalMaps[8]['\\'] = 0x017D; // Ž
    m_nationalMaps[8][']'] = 0x0110;  // Đ
    m_nationalMaps[8]['^'] = 0x0160;  // Š
    m_nationalMaps[8]['_'] = 0x00EB;  // ë
    m_nationalMaps[8]['`'] = 0x010D;  // č
    m_nationalMaps[8]['{'] = 0x0107;  // ć
    m_nationalMaps[8]['|'] = 0x017E;  // ž
    m_nationalMaps[8]['}'] = 0x0111;  // đ
    m_nationalMaps[8]['~'] = 0x0161;  // š
    
    // 9 = Estonian (use English as placeholder)
    m_nationalMaps[9] = m_nationalMaps[0];
    
    // 10 = Lettish/Lithuanian (use English as placeholder)
    m_nationalMaps[10] = m_nationalMaps[0];
    
    // 11 = Polish
    m_nationalMaps[11]['#'] = 0x0023;  // #
    m_nationalMaps[11]['$'] = 0x0144;  // ń
    m_nationalMaps[11]['@'] = 0x0105;  // ą
    m_nationalMaps[11]['['] = 0x01B5;  // Ƶ
    m_nationalMaps[11]['\\'] = 0x015A; // Ś
    m_nationalMaps[11][']'] = 0x0141;  // Ł
    m_nationalMaps[11]['^'] = 0x0107;  // ć
    m_nationalMaps[11]['_'] = 0x00F3;  // ó
    m_nationalMaps[11]['`'] = 0x0119;  // ę
    m_nationalMaps[11]['{'] = 0x017C;  // ż
    m_nationalMaps[11]['|'] = 0x015B;  // ś
    m_nationalMaps[11]['}'] = 0x0142;  // ł
    m_nationalMaps[11]['~'] = 0x017A;  // ź
    
    // 12 = Turkish
    m_nationalMaps[12]['#'] = 0x0167;  // ŧ
    m_nationalMaps[12]['$'] = 0x011F;  // ğ
    m_nationalMaps[12]['@'] = 0x0130;  // İ
    m_nationalMaps[12]['['] = 0x015E;  // Ş
    m_nationalMaps[12]['\\'] = 0x00D6; // Ö
    m_nationalMaps[12][']'] = 0x00C7;  // Ç
    m_nationalMaps[12]['^'] = 0x00DC;  // Ü
    m_nationalMaps[12]['_'] = 0x011E;  // Ğ
    m_nationalMaps[12]['`'] = 0x0131;  // ı
    m_nationalMaps[12]['{'] = 0x015F;  // ş
    m_nationalMaps[12]['|'] = 0x00F6;  // ö
    m_nationalMaps[12]['}'] = 0x00E7;  // ç
    m_nationalMaps[12]['~'] = 0x00FC;  // ü
}

wchar_t TeletextPage::ApplyNationalCharMap(uint8_t ch)
{
    // Check if this character has a national mapping
    auto it = m_nationalMaps[m_nationalOption].find(ch);
    if (it != m_nationalMaps[m_nationalOption].end())
    {
        return it->second;
    }
    
    // No mapping found, return character as-is
    return (wchar_t)ch;
}

bool TeletextPage::ParseTTI(const std::vector<uint8_t>& data)
{
    std::string content((char*)data.data(), data.size());
    std::istringstream stream(content);
    std::string line;
    
    bool foundFirstPage = false;
    
    while (std::getline(stream, line))
    {
        // Skip empty lines
        if (line.empty()) continue;
        
        // Check for PS (Page Status) command to extract language
        if (line.find("PS,") == 0)
        {
            // Parse PS command: PS,<hex_value>
            size_t comma = line.find(',');
            if (comma != std::string::npos)
            {
                std::string psValue = line.substr(comma + 1);
                // Convert hex string to integer
                unsigned int ps = 0;
                if (sscanf_s(psValue.c_str(), "%x", &ps) == 1)
                {
                    // Extract language bits (bits 7-9, mask 0x0380)
                    unsigned int langBits = ps & 0x0380;
                    
                    // Map to national option index
                    switch (langBits)
                    {
                        case 0x0000: m_nationalOption = 0; break; // English
                        case 0x0080: m_nationalOption = 4; break; // French
                        case 0x0100: m_nationalOption = 2; break; // Swedish/Finnish
                        case 0x0180: m_nationalOption = 6; break; // Czech/Slovak
                        case 0x0200: m_nationalOption = 1; break; // German
                        case 0x0280: m_nationalOption = 5; break; // Portuguese/Spanish
                        case 0x0300: m_nationalOption = 3; break; // Italian
                        default: m_nationalOption = 0; break;     // Default to English
                    }
                }
            }
        }
        
        // TTI format: lines starting with OL (Output Line) or FL (Fastext Line)
        // Format: OL,<row>,<data>
        if (line.find("OL,") == 0 || line.find("FL,") == 0)
        {
            foundFirstPage = true;
            
            // Extract row number
            size_t firstComma = line.find(',');
            if (firstComma == std::string::npos) continue;
            
            size_t secondComma = line.find(',', firstComma + 1);
            if (secondComma == std::string::npos) continue;
            
            std::string rowStr = line.substr(firstComma + 1, secondComma - firstComma - 1);
            int rowNumber = std::atoi(rowStr.c_str());
            
            // Only process displayable rows 0-24
            if (rowNumber >= 0 && rowNumber <= 24)
            {
                ParseLine(line, rowNumber);
            }
        }
        // Stop at next page
        else if (foundFirstPage && line.find("PN,") == 0)
        {
            break;
        }
    }
    
    // Fill any missing rows 1-23 with spaces (row 0 is skipped in rendering anyway)
    // This preserves blank space between rows with text
    for (int row = 1; row <= 23; row++)
    {
        // Check if this row is empty (default initialized)
        bool isEmpty = true;
        for (int col = 0; col < SCREEN_COLS; col++)
        {
            // Check if cell differs from default (space with white on black)
            if (m_cells[row][col].character != L' ' ||
                m_cells[row][col].foreground != WHITE ||
                m_cells[row][col].background != BLACK)
            {
                isEmpty = false;
                break;
            }
        }
        
        // If row is still default (was never parsed), ensure it has proper defaults
        // This handles missing rows in the TTI file
        if (isEmpty)
        {
            for (int col = 0; col < SCREEN_COLS; col++)
            {
                m_cells[row][col].character = L' ';
                m_cells[row][col].foreground = WHITE;
                m_cells[row][col].background = BLACK;
                m_cells[row][col].graphics = false;
                m_cells[row][col].separated = false;
                m_cells[row][col].doubleHeight = false;
                m_cells[row][col].held = false;
            }
        }
    }
    
    return foundFirstPage;
}

bool TeletextPage::ParseEP1(const std::vector<uint8_t>& data)
{
    // EP1 format:
    // Header: 6 bytes
    //   - Byte 0: 0xFE (format identifier)
    //   - Byte 1: 0x01 (version)
    //   - Byte 2: Language/type byte (contains national option in bits 1-3)
    //   - Bytes 3-5: 0x00 0x00 0x00 (reserved)
    // Data: 24 rows × 40 characters = 960 bytes
    // Footer: 2 bytes (00 00)
    // Total: 968 bytes minimum
    
    if (data.size() < 968)
    {
        return false;
    }
    
    // Verify header signature (bytes 0-1)
    if (data[0] != 0xFE || data[1] != 0x01)
    {
        return false;
    }
    
    // Extract language information from byte 2
    // Bits 1-3 encode the national character set (similar to PS command bits 7-9)
    uint8_t languageByte = data[2];
    uint8_t nationalBits = (languageByte >> 1) & 0x07;
    
    // Map EP1 language bits to national option index
    // Based on ETS 300 706 teletext standard
    switch (nationalBits)
    {
        case 0: m_nationalOption = 0; break; // English
        case 1: m_nationalOption = 4; break; // French
        case 2: m_nationalOption = 2; break; // Swedish/Finnish
        case 3: m_nationalOption = 6; break; // Czech/Slovak
        case 4: m_nationalOption = 1; break; // German
        case 5: m_nationalOption = 5; break; // Portuguese/Spanish
        case 6: m_nationalOption = 3; break; // Italian
        case 7: m_nationalOption = 0; break; // Reserved (default to English)
        default: m_nationalOption = 0; break;
    }
    
    // Parse 24 rows of data (rows 0-23 in the file, which map to rows 1-24 in our array)
    // Row 0 in EP1 corresponds to row 1 in teletext (we skip the header row in rendering)
    size_t offset = 6; // Skip header
    
    for (int row = 0; row < 24 && offset + 40 <= data.size(); row++)
    {
        // EP1 rows 0-23 map to teletext rows 1-24
        int teletextRow = row + 1;
        
        // Parse character data - NO escape sequence processing
        TeletextColor currentFg = WHITE;
        TeletextColor currentBg = BLACK;
        bool graphicsMode = false;
        bool separated = false;
        bool doubleHeight = false;
        
        int colIndex = 0;
        for (int i = 0; i < 40 && colIndex < SCREEN_COLS; i++)
        {
            uint8_t ch = data[offset + i] & 0x7F; // Strip parity bit
            
            // Replace ESC (0x1B) with space (0x20)
            if (ch == 0x1B)
            {
                ch = 0x20;
            }
            
            // Control codes
            if (ch < 0x20)
            {
                switch (ch)
                {
                    case 0x00: case 0x01: case 0x02: case 0x03: case 0x04:
                    case 0x05: case 0x06: case 0x07:
                        currentFg = (TeletextColor)ch;
                        graphicsMode = false;
                        break;
                        
                    case 0x10: case 0x11: case 0x12: case 0x13: case 0x14:
                    case 0x15: case 0x16: case 0x17:
                        currentFg = (TeletextColor)(ch - 0x10);
                        graphicsMode = true;
                        break;
                        
                    case 0x19: // Contiguous graphics
                        separated = false;
                        break;
                        
                    case 0x1A: // Separated graphics
                        separated = true;
                        break;
                        
                    case 0x1C: // Black background
                        currentBg = BLACK;
                        break;
                        
                    case 0x1D: // New background
                        currentBg = currentFg;
                        break;
                        
                    case 0x0D: // Double height
                        doubleHeight = true;
                        break;
                        
                    case 0x0C: // Normal height
                        doubleHeight = false;
                        break;
                }
                
                m_cells[teletextRow][colIndex].character = L' ';
                m_cells[teletextRow][colIndex].foreground = currentFg;
                m_cells[teletextRow][colIndex].background = currentBg;
                m_cells[teletextRow][colIndex].doubleHeight = doubleHeight;
                colIndex++;
            }
            else
            {
                m_cells[teletextRow][colIndex].foreground = currentFg;
                m_cells[teletextRow][colIndex].background = currentBg;
                m_cells[teletextRow][colIndex].graphics = graphicsMode;
                m_cells[teletextRow][colIndex].separated = separated;
                m_cells[teletextRow][colIndex].doubleHeight = doubleHeight;
                
                if (graphicsMode && ch >= 0x20 && ch <= 0x7F)
                {
                    // Graphics mode - use teletext graphics characters
                    m_cells[teletextRow][colIndex].character = GetGraphicsChar(ch, separated);
                }
                else
                {
                    // Alphanumeric mode - apply national character mapping
                    m_cells[teletextRow][colIndex].character = ApplyNationalCharMap(ch);
                }
                colIndex++;
            }
        }
        
        // Fill any remaining columns with spaces
        while (colIndex < SCREEN_COLS)
        {
            m_cells[teletextRow][colIndex].character = L' ';
            m_cells[teletextRow][colIndex].foreground = WHITE;
            m_cells[teletextRow][colIndex].background = BLACK;
            m_cells[teletextRow][colIndex].graphics = false;
            m_cells[teletextRow][colIndex].separated = false;
            m_cells[teletextRow][colIndex].doubleHeight = false;
            colIndex++;
        }
        
        offset += 40; // Move to next row
    }
    
    // Row 0 (header row) is not stored in EP1 - initialize it to spaces
    for (int col = 0; col < SCREEN_COLS; col++)
    {
        m_cells[0][col].character = L' ';
        m_cells[0][col].foreground = WHITE;
        m_cells[0][col].background = BLACK;
        m_cells[0][col].graphics = false;
        m_cells[0][col].separated = false;
        m_cells[0][col].doubleHeight = false;
        m_cells[0][col].held = false;
    }
    
    return true;
}

void TeletextPage::ParseLine(const std::string& line, int rowIndex)
{
    if (rowIndex < 0 || rowIndex >= SCREEN_ROWS) return;
    
    // Find the data after the second comma
    size_t firstComma = line.find(',');
    if (firstComma == std::string::npos) return;
    
    size_t secondComma = line.find(',', firstComma + 1);
    if (secondComma == std::string::npos) return;
    
    std::string data = line.substr(secondComma + 1);
    
    // Row 0 is the header - first 8 characters must be spaces
    if (rowIndex == 0)
    {
        // Ensure first 8 positions are spaces
        for (int i = 0; i < 8 && i < SCREEN_COLS; i++)
        {
            m_cells[rowIndex][i].character = L' ';
            m_cells[rowIndex][i].foreground = WHITE;
            m_cells[rowIndex][i].background = BLACK;
        }
    }
    
    // Parse character data
    // Control codes are stored as ESC (0x1B) followed by (code + 0x40)
    TeletextColor currentFg = WHITE;
    TeletextColor currentBg = BLACK;
    bool graphicsMode = false;
    bool separated = false;
    bool doubleHeight = false;
    int heldChar = 0x20;
    
    int colIndex = (rowIndex == 0) ? 8 : 0; // Start at column 8 for header row
    
    for (size_t i = 0; i < data.length() && colIndex < SCREEN_COLS; i++)
    {
        uint8_t ch = (uint8_t)data[i] & 0x7F; // Strip parity bit (bit 7)
        
        // Check for ESC (0x1B) followed by encoded control code
        if (ch == 0x1B && i + 1 < data.length())
        {
            // Read next character and decode: subtract 0x40 to get control code
            i++;
            ch = ((uint8_t)data[i] & 0x7F) - 0x40;
        }
        
        // Control codes
        if (ch < 0x20)
        {
            switch (ch)
            {
                case 0x00: case 0x01: case 0x02: case 0x03: case 0x04:
                case 0x05: case 0x06: case 0x07:
                    // Alphanumeric color: 0x00=Black, 0x01=Red, 0x02=Green, 0x03=Yellow, 0x04=Blue, 0x05=Magenta, 0x06=Cyan, 0x07=White
                    currentFg = (TeletextColor)ch;  // Direct mapping: code value = color enum value
                    graphicsMode = false;
                    break;
                    
                case 0x10: case 0x11: case 0x12: case 0x13: case 0x14:
                case 0x15: case 0x16: case 0x17:
                    // Graphics color: 0x10=Black, 0x11=Red, 0x12=Green, 0x13=Yellow, 0x14=Blue, 0x15=Magenta, 0x16=Cyan, 0x17=White
                    currentFg = (TeletextColor)(ch - 0x10);  // 0x10 -> 0x00 (BLACK), 0x11 -> 0x01 (RED), etc.
                    graphicsMode = true;
                    break;
                    
                case 0x19: // Contiguous graphics
                    separated = false;
                    break;
                    
                case 0x1A: // Separated graphics
                    separated = true;
                    break;
                    
                case 0x1C: // Black background
                    currentBg = BLACK;
                    break;
                    
                case 0x1D: // New background
                    currentBg = currentFg;
                    break;
                    
                case 0x0D: // Double height
                    doubleHeight = true;
                    break;
                    
                case 0x0C: // Normal height
                    doubleHeight = false;
                    break;
            }
            
            m_cells[rowIndex][colIndex].character = L' ';
            m_cells[rowIndex][colIndex].foreground = currentFg;
            m_cells[rowIndex][colIndex].background = currentBg;
            m_cells[rowIndex][colIndex].doubleHeight = doubleHeight;
            colIndex++;
        }
        else
        {
            m_cells[rowIndex][colIndex].foreground = currentFg;
            m_cells[rowIndex][colIndex].background = currentBg;
            m_cells[rowIndex][colIndex].graphics = graphicsMode;
            m_cells[rowIndex][colIndex].separated = separated;
            m_cells[rowIndex][colIndex].doubleHeight = doubleHeight;
            
            if (graphicsMode && ch >= 0x20 && ch <= 0x7F)
            {
                // Graphics mode - use teletext graphics characters
                m_cells[rowIndex][colIndex].character = GetGraphicsChar(ch, separated);
            }
            else
            {
                // Alphanumeric mode - apply national character mapping
                m_cells[rowIndex][colIndex].character = ApplyNationalCharMap(ch);
            }
            colIndex++;
        }
    }
}

wchar_t TeletextPage::GetGraphicsChar(uint8_t code, bool separated)
{
    // Graphics characters mapping:
    // 0x20-0x3F → patterns 0x00-0x1F (using bits 0-4)
    // 0x40-0x5F → blast through (show as alphanumeric characters, not graphics)
    // 0x60-0x7F → patterns 0x20-0x3F (using bits 0-5 with bit 5 set)
    
    // Check for blast-through range
    if (code >= 0x40 && code <= 0x5F)
    {
        // Blast through - return the character as-is to display alphanumerically
        return (wchar_t)code;
    }
    
    // Determine pattern based on range
    uint8_t pattern;
    if (code >= 0x20 && code <= 0x3F)
    {
        // Lower range: pattern is bits 0-4 (0x00-0x1F)
        pattern = code & 0x1F;
    }
    else // code >= 0x60 && code <= 0x7F
    {
        // Upper range: pattern is bits 0-5 (0x20-0x3F)
        pattern = code & 0x3F;
    }
    
    // Map to Unicode private use area glyphs in teletext2.ttf
    // Contiguous: U+E680-U+E69F (patterns 0x00-0x1F), U+E6C0-U+E6DF (patterns 0x20-0x3F)
    // Separated:  U+E6A0-U+E6BF (patterns 0x00-0x1F), U+E6E0-U+E6FF (patterns 0x20-0x3F)
    
    wchar_t baseChar;
    
    if (separated)
    {
        // Separated graphics
        if (pattern < 0x20)
        {
            baseChar = 0xE6A0 + pattern;  // U+E6A0 to U+E6BF
        }
        else
        {
            baseChar = 0xE6E0 + (pattern - 0x20);  // U+E6E0 to U+E6FF
        }
    }
    else
    {
        // Contiguous graphics
        if (pattern < 0x20)
        {
            baseChar = 0xE680 + pattern;  // U+E680 to U+E69F
        }
        else
        {
            baseChar = 0xE6C0 + (pattern - 0x20);  // U+E6C0 to U+E6DF
        }
    }
    
    return baseChar;
}

COLORREF TeletextPage::GetColorRef(TeletextColor color)
{
    switch (color)
    {
        case BLACK:   return RGB(0, 0, 0);
        case RED:     return RGB(255, 0, 0);
        case GREEN:   return RGB(0, 255, 0);
        case YELLOW:  return RGB(255, 255, 0);
        case BLUE:    return RGB(0, 0, 255);
        case MAGENTA: return RGB(255, 0, 255);
        case CYAN:    return RGB(0, 255, 255);
        case WHITE:   return RGB(255, 255, 255);
        default:      return RGB(255, 255, 255);
    }
}

HBITMAP TeletextPage::RenderToBitmap(UINT width, UINT height)
{
    // Create bitmap
    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, width, height);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);
    
    // Exclude row 0 from rendering - only render rows 1-24
    const int RENDER_ROWS = SCREEN_ROWS - 1;  // 24 rows instead of 25
    
    // Calculate cell dimensions based on 24 rows
    int cellWidth = width / SCREEN_COLS;
    int cellHeight = height / RENDER_ROWS;
    
    // Try to load teletext fonts from multiple locations
    wchar_t fontPath[MAX_PATH];
    wchar_t dllPath[MAX_PATH];
    bool font2Added = false;
    bool font4Added = false;
    
    // External reference to DLL instance handle from DllMain.cpp
    extern HINSTANCE g_hInst;
    
    // Try to load teletext2.ttf
    // First try: DLL directory
    if (g_hInst && GetModuleFileNameW(g_hInst, dllPath, MAX_PATH))
    {
        wcscpy_s(fontPath, MAX_PATH, dllPath);
        wchar_t* lastSlash = wcsrchr(fontPath, L'\\');
        if (lastSlash)
        {
            *(lastSlash + 1) = L'\0';
            wcscat_s(fontPath, MAX_PATH, L"teletext2.ttf");
            
            if (AddFontResourceExW(fontPath, FR_PRIVATE, 0) > 0)
            {
                font2Added = true;
            }
        }
    }
    
    // Second try: Windows Fonts folder
    if (!font2Added)
    {
        wchar_t winDir[MAX_PATH];
        if (GetWindowsDirectoryW(winDir, MAX_PATH))
        {
            wcscpy_s(fontPath, MAX_PATH, winDir);
            wcscat_s(fontPath, MAX_PATH, L"\\Fonts\\teletext2.ttf");
            
            if (AddFontResourceExW(fontPath, FR_PRIVATE, 0) > 0)
            {
                font2Added = true;
            }
        }
    }
    
    // Try to load teletext4.ttf
    // First try: DLL directory
    if (g_hInst && GetModuleFileNameW(g_hInst, dllPath, MAX_PATH))
    {
        wcscpy_s(fontPath, MAX_PATH, dllPath);
        wchar_t* lastSlash = wcsrchr(fontPath, L'\\');
        if (lastSlash)
        {
            *(lastSlash + 1) = L'\0';
            wcscat_s(fontPath, MAX_PATH, L"teletext4.ttf");
            
            if (AddFontResourceExW(fontPath, FR_PRIVATE, 0) > 0)
            {
                font4Added = true;
            }
        }
    }
    
    // Second try: Windows Fonts folder
    if (!font4Added)
    {
        wchar_t winDir[MAX_PATH];
        if (GetWindowsDirectoryW(winDir, MAX_PATH))
        {
            wcscpy_s(fontPath, MAX_PATH, winDir);
            wcscat_s(fontPath, MAX_PATH, L"\\Fonts\\teletext4.ttf");
            
            if (AddFontResourceExW(fontPath, FR_PRIVATE, 0) > 0)
            {
                font4Added = true;
            }
        }
    }
    
    SetBkMode(hdcMem, OPAQUE);
    
    // First pass: determine which rows have double height
    bool rowHasDoubleHeight[SCREEN_ROWS] = {false};
    for (int row = 0; row < SCREEN_ROWS; row++)
    {
        for (int col = 0; col < SCREEN_COLS; col++)
        {
            if (m_cells[row][col].doubleHeight)
            {
                rowHasDoubleHeight[row] = true;
                break;
            }
        }
    }
    
    // Render each cell, starting from row 1 (skip row 0), only render up to row 23
    for (int row = 1; row <= 23; row++)
    {
        // Calculate the display row (0-22 for rows 1-23)
        int displayRow = row - 1;
        
        // Check if the previous row has double height - if so, skip this row entirely
        bool previousRowHasDoubleHeight = false;
        if (row > 1 && rowHasDoubleHeight[row - 1])
        {
            // Rows 23 and 24 don't extend down
            if (row - 1 != 23 && row - 1 != 24)
            {
                previousRowHasDoubleHeight = true;
            }
        }
        
        // Skip this row if previous row had double height (it's already been rendered)
        if (previousRowHasDoubleHeight)
        {
            continue;
        }
        
        // Normal rendering for this row
        for (int col = 0; col < SCREEN_COLS; col++)
        {
            // Don't allow double height on rows 23, 24 (they don't extend down)
            bool allowDoubleHeight = (row != 23 && row != 24);
            DrawCell(hdcMem, row, col, cellWidth, cellHeight, displayRow, font2Added, font4Added, allowDoubleHeight);
        }
    }
    
    // Remove the temporary fonts if we added them
    if (font2Added || font4Added)
    {
        if (g_hInst && GetModuleFileNameW(g_hInst, dllPath, MAX_PATH))
        {
            if (font2Added)
            {
                // Try DLL directory first
                wcscpy_s(fontPath, MAX_PATH, dllPath);
                wchar_t* lastSlash = wcsrchr(fontPath, L'\\');
                if (lastSlash)
                {
                    *(lastSlash + 1) = L'\0';
                    wcscat_s(fontPath, MAX_PATH, L"teletext2.ttf");
                    RemoveFontResourceExW(fontPath, FR_PRIVATE, 0);
                }
                
                // Also try Windows Fonts folder
                wchar_t winDir[MAX_PATH];
                if (GetWindowsDirectoryW(winDir, MAX_PATH))
                {
                    wcscpy_s(fontPath, MAX_PATH, winDir);
                    wcscat_s(fontPath, MAX_PATH, L"\\Fonts\\teletext2.ttf");
                    RemoveFontResourceExW(fontPath, FR_PRIVATE, 0);
                }
            }
            
            if (font4Added)
            {
                // Try DLL directory first
                wcscpy_s(fontPath, MAX_PATH, dllPath);
                wchar_t* lastSlash = wcsrchr(fontPath, L'\\');
                if (lastSlash)
                {
                    *(lastSlash + 1) = L'\0';
                    wcscat_s(fontPath, MAX_PATH, L"teletext4.ttf");
                    RemoveFontResourceExW(fontPath, FR_PRIVATE, 0);
                }
                
                // Also try Windows Fonts folder
                wchar_t winDir[MAX_PATH];
                if (GetWindowsDirectoryW(winDir, MAX_PATH))
                {
                    wcscpy_s(fontPath, MAX_PATH, winDir);
                    wcscat_s(fontPath, MAX_PATH, L"\\Fonts\\teletext4.ttf");
                    RemoveFontResourceExW(fontPath, FR_PRIVATE, 0);
                }
            }
        }
    }
    
    SelectObject(hdcMem, hOldBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);
    
    return hBitmap;
}

void TeletextPage::DrawCell(HDC hdc, int row, int col, int cellWidth, int cellHeight, int displayRow, bool hasFont2, bool hasFont4, bool allowDoubleHeight)
{
    TeletextCell& cell = m_cells[row][col];
    
    RECT rect;
    rect.left = col * cellWidth;
    rect.top = displayRow * cellHeight;
    rect.right = rect.left + cellWidth;
    rect.bottom = rect.top + cellHeight;
    
    // Fill background for this cell
    // If double height, extend background to cover next row too
    if (cell.doubleHeight && allowDoubleHeight)
    {
        // Extend background to cover both rows
        RECT bgRect = rect;
        bgRect.bottom = rect.top + (cellHeight * 2);
        HBRUSH hBrush = CreateSolidBrush(GetColorRef(cell.background));
        FillRect(hdc, &bgRect, hBrush);
        DeleteObject(hBrush);
    }
    else
    {
        // Normal height background
        HBRUSH hBrush = CreateSolidBrush(GetColorRef(cell.background));
        FillRect(hdc, &rect, hBrush);
        DeleteObject(hBrush);
    }
    
    // Draw character if not space
    if (cell.character != L' ')
    {
        HFONT hFont = NULL;
        int fontSize = cellHeight;
        const wchar_t* fontName = L"Courier New";
        
        RECT textRect = rect;
        
        // Determine font and size based on double height (only if allowed for this row)
        if (cell.doubleHeight && allowDoubleHeight)
        {
            fontSize = cellHeight * 2;  // Double the font size
            fontName = hasFont4 ? L"Teletext4" : L"Courier New";
            
            // Extend the rectangle to span two rows vertically
            textRect.bottom = textRect.top + (cellHeight * 2);
        }
        else
        {
            // Normal height: use teletext2 if available, Courier New as fallback
            fontName = hasFont2 ? L"Teletext2" : L"Courier New";
        }
        
        // Create font
        hFont = CreateFontW(
            fontSize, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            NONANTIALIASED_QUALITY, FIXED_PITCH | FF_MODERN, fontName);
        
        HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
        
        SetTextColor(hdc, GetColorRef(cell.foreground));
        SetBkColor(hdc, GetColorRef(cell.background));
        
        wchar_t str[2] = { cell.character, 0 };
        DrawTextW(hdc, str, 1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);
        
        SelectObject(hdc, hOldFont);
        DeleteObject(hFont);
    }
}
