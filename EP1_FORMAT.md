# EP1 File Format Specification

## Overview

EP1 is a binary teletext page format used by Edit.tf and wxTED editors. It stores a single teletext page in raw binary format with minimal overhead.

## File Structure

An EP1 file has a fixed structure:

```
Offset  Size    Description
------  ------  -----------
0x000   6       Header (FE 01 09 00 00 00)
0x006   960     Page data (24 rows × 40 characters)
0x3C6   2       Footer (00 00)
------  ------  -----------
Total:  968 bytes
```

### Header (6 bytes)

Header that identifies the file as EP1 format and encodes metadata:
- Byte 0: `0xFE` - Format identifier
- Byte 1: `0x01` - Version
- Byte 2: **Language/Type byte** - Encodes national character set
  - Bits 1-3: National option (0-7)
  - Other bits: Format type
  - Common value: `0x09` (binary: 0000**100**1 = English, bits 1-3 = 100 = 4, but maps to option 0)
- Bytes 3-5: `0x00 0x00 0x00` - Reserved

**Language Encoding in Byte 2:**

The national character set is encoded in bits 1-3 of byte 2:

| Bits 1-3 | Value | Language | National Option |
|----------|-------|----------|-----------------|
| 000 | 0 | English | 0 |
| 001 | 1 | French | 4 |
| 010 | 2 | Swedish/Finnish | 2 |
| 011 | 3 | Czech/Slovak | 6 |
| 100 | 4 | German | 1 |
| 101 | 5 | Portuguese/Spanish | 5 |
| 110 | 6 | Italian | 3 |
| 111 | 7 | Reserved | 0 (defaults to English) |

**Examples:**
- `0x09` (binary: 00001001) → bits 1-3 = 100 (4) → German mapping
- `0x01` (binary: 00000001) → bits 1-3 = 000 (0) → English
- `0x05` (binary: 00000101) → bits 1-3 = 010 (2) → Swedish/Finnish
- `0x0B` (binary: 00001011) → bits 1-3 = 101 (5) → Portuguese/Spanish

To extract: `nationalBits = (byte2 >> 1) & 0x07`

### Page Data (960 bytes)

24 rows of 40 characters each, stored sequentially:
- Row 0: Bytes 6-45 (40 bytes)
- Row 1: Bytes 46-85 (40 bytes)
- ...
- Row 23: Bytes 926-965 (40 bytes)

**Important:** EP1 row 0 corresponds to teletext row 1 (the header row 0 is not stored).

### Footer (2 bytes)

Two null bytes: `0x00 0x00`

## Character Encoding

EP1 files use **raw teletext encoding** without escape sequences:

### 7-bit Encoding
- All characters use 7-bit values (bit 7 is parity and should be stripped)
- Character values are masked with `0x7F`

### ESC Character Handling

**Important:** Unlike TTI format, EP1 files do NOT use escape sequences for control codes.

- Control codes (0x00-0x1F) are stored directly as raw bytes
- If ESC (0x1B) appears in the data, it should be **replaced with a space (0x20)**
- No escape sequence processing is performed

**Example:**
```
Byte: 0x1B
Action: Replace with 0x20 (space)
Result: Displays as a space character
```

## Control Codes

Same as TTI format:

### Alphanumeric Colors (0x00-0x07)
- 0x00: Black (extended)
- 0x01: Red
- 0x02: Green
- 0x03: Yellow
- 0x04: Blue
- 0x05: Magenta
- 0x06: Cyan
- 0x07: White

### Graphics Colors (0x10-0x17)
- 0x10: Black graphics (extended)
- 0x11: Red graphics
- 0x12: Green graphics
- 0x13: Yellow graphics
- 0x14: Blue graphics
- 0x15: Magenta graphics
- 0x16: Cyan graphics
- 0x17: White graphics

### Other Control Codes
- 0x0C: Normal height
- 0x0D: Double height
- 0x19: Contiguous graphics
- 0x1A: Separated graphics
- 0x1C: Black background
- 0x1D: New background

## Differences from TTI Format

| Feature | TTI Format | EP1 Format |
|---------|-----------|------------|
| File type | Text | Binary |
| Size | Variable | Fixed (968 bytes) |
| Header row | Stored as OL,0 | Not stored |
| Rows stored | Variable (0-24) | Fixed (1-24, 960 bytes) |
| Control codes | ESC + (code + 0x40) | Raw bytes (0x00-0x1F) |
| ESC handling | Escape sequence | Replaced with space (0x20) |
| Line markers | Yes (OL,row,data) | No |
| Human readable | Yes | No |

## Reading EP1 Files

### Algorithm

```cpp
bool ParseEP1(const std::vector<uint8_t>& data)
{
    // 1. Verify file size
    if (data.size() < 968) return false;
    
    // 2. Check header signature
    if (data[0] != 0xFE || data[1] != 0x01)
        return false;
    
    // 3. Extract language from byte 2
    uint8_t languageByte = data[2];
    uint8_t nationalBits = (languageByte >> 1) & 0x07;
    
    // Map to national option (0=English, 1=French, 2=Swedish, etc.)
    int nationalOption = MapLanguageBits(nationalBits);
    
    // 4. Parse 24 rows
    size_t offset = 6; // Skip header
    
    for (int row = 0; row < 24; row++)
    {
        // Parse 40 characters
        for (int col = 0; col < 40; col++)
        {
            uint8_t ch = data[offset++] & 0x7F; // Strip parity
            
            // Replace ESC with space
            if (ch == 0x1B)
            {
                ch = 0x20; // Space
            }
            
            // Process character (control code or displayable)
            // Apply national character mapping if in alphanumeric mode
            ProcessCharacter(row, col, ch, nationalOption);
        }
    }
    
    return true;
}
```

## Creating EP1 Files

### Algorithm

```cpp
void WriteEP1(const TeletextPage& page, const std::string& filename)
{
    std::ofstream file(filename, std::ios::binary);
    
    // Write header
    file.put(0xFE);
    file.put(0x01);
    file.put(0x09);
    file.put(0x00);
    file.put(0x00);
    file.put(0x00);
    
    // Write 24 rows (rows 1-24 of teletext page)
    for (int row = 1; row <= 24; row++)
    {
        for (int col = 0; col < 40; col++)
        {
            uint8_t ch = page.GetCharacter(row, col);
            
            // Write character directly (control codes as raw bytes)
            file.put(ch & 0x7F);  // 7-bit character
        }
    }
    
    // Write footer
    file.put(0x00);
    file.put(0x00);
}
```

**Note:** Control codes (0x00-0x1F) are written directly as raw bytes, not as escape sequences.

## Format Validation

To verify an EP1 file:

1. **Check file size**: Must be exactly 968 bytes
2. **Verify header**: 
   - Byte 0 must be `0xFE` (format identifier)
   - Byte 1 must be `0x01` (version)
   - Byte 2 contains language/type information (variable)
3. **Extract language**: Use bits 1-3 of byte 2 to determine national character set
4. **Check footer**: Bytes 966-967 should be `00 00` (optional check)
5. **Validate characters**: 
   - All characters should be 7-bit (check bit 7)
   - Control codes (0x00-0x1F) should be raw bytes
   - ESC (0x1B) should be treated as a space (0x20)

## Common Issues

### File Size
- EP1 files must be exactly 968 bytes
- Missing rows or truncated files will fail to parse
- Extra data after footer is typically ignored

### Character Encoding
- Remember to strip bit 7 (parity bit) when reading
- Control codes are raw bytes (0x00-0x1F), not escaped
- ESC (0x1B) should be replaced with space (0x20)
- No escape sequence processing needed

### Row Mapping
- EP1 row 0 = Teletext row 1
- EP1 row 23 = Teletext row 24
- Teletext row 0 (header) is not stored in EP1

## Compatibility

EP1 format is compatible with:
- **Edit.tf** - Original teletext editor (uses byte 2 for language/type)
- **wxTED** - Cross-platform teletext editor (reads and writes byte 2 language encoding)
- **Flair32** - Teletext test file generator
- **TTI Thumbnail Provider** - Windows thumbnail renderer (extracts language from byte 2)

**Note on Language Encoding:**
- The language encoding in byte 2 (bits 1-3) follows the ETS 300 706 teletext standard
- This is the same encoding used in the PS (Page Status) command in TTI files
- Different editors may use different default values for byte 2, but the language bits remain consistent
- The most common value is 0x09 (English), but files may have other values depending on the language

## Example File

Hex dump of a minimal EP1 file (first 100 bytes):

```
Offset  00 01 02 03 04 05 06 07  08 09 0A 0B 0C 0D 0E 0F  ASCII
------  -----------------------  -----------------------  ----------------
000000  FE 01 09 00 00 00 07 54  65 73 74 20 50 61 67 65  .......Test Page
000010  20 20 20 20 20 20 20 20  20 20 20 20 20 20 20 20                  
000020  20 20 20 20 20 20 20 20  20 20 20 20 20 01 52 45                .RE
000030  44 20 20 20 02 47 52 45  45 4E 20 20 03 59 45 4C  D   .GREEN  .YEL
000040  4C 4F 57 20 06 43 59 41  4E 20 20 20 20 20 20 20  LOW .CYAN       
000050  20 20 20 20 20 20 20 20  20 20 20 20 20 20 20 20                  
000060  20 20 20 20                                        
```

This shows:
- Header: `FE 01 09 00 00 00`
- Row 0 data starting at offset 6
- Control codes as raw bytes: `07` (White), `01` (Red), `02` (Green), `03` (Yellow), `06` (Cyan)
- Text content like "Test Page", "RED", "GREEN", etc.
- **Note:** Control codes are stored as raw bytes (e.g., `01` for Red), NOT as escape sequences

## Advantages of EP1

- **Compact**: Fixed 968-byte size
- **Fast**: No parsing of line markers
- **Simple**: Direct binary format
- **Complete**: Always stores all 24 rows

## Disadvantages of EP1

- **Not human-readable**: Binary format
- **No metadata**: Can't store page numbers, descriptions
- **Fixed size**: Always 968 bytes even if mostly empty
- **No header row**: Row 0 not stored
