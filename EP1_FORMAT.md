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

Fixed byte sequence that identifies the file as EP1 format:
- Byte 0: `0xFE` - Format identifier
- Byte 1: `0x01` - Version
- Byte 2: `0x09` - Type
- Bytes 3-5: `0x00 0x00 0x00` - Reserved

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

EP1 files use **raw teletext encoding** with escape sequences:

### 7-bit Encoding
- All characters use 7-bit values (bit 7 is parity and should be stripped)
- Character values are masked with `0x7F`

### Escape Sequences

Control characters (0x00-0x1F) are encoded using escape sequences:

**Format:** `ESC (0x1B) + encoded_byte`

**Decoding:**
1. When reading byte `0x1B`, read the next byte
2. Mask the next byte with `0x3F` to get the control code
3. Process the control code

**Example:**
```
Byte sequence: 1B 41
Step 1: Detect ESC (0x1B)
Step 2: Read next byte (0x41)
Step 3: Mask with 0x3F: 0x41 & 0x3F = 0x01
Result: Control code 0x01 (Red alphanumeric)
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
| Control codes | ESC + (code + 0x40) | ESC + (byte & 0x3F) |
| Line markers | Yes (OL,row,data) | No |
| Human readable | Yes | No |

## Reading EP1 Files

### Algorithm

```cpp
bool ParseEP1(const std::vector<uint8_t>& data)
{
    // 1. Verify file size
    if (data.size() < 968) return false;
    
    // 2. Check header
    if (data[0] != 0xFE || data[1] != 0x01 || data[2] != 0x09)
        return false;
    
    // 3. Parse 24 rows
    size_t offset = 6; // Skip header
    
    for (int row = 0; row < 24; row++)
    {
        // Parse 40 characters
        for (int col = 0; col < 40; col++)
        {
            uint8_t ch = data[offset++] & 0x7F; // Strip parity
            
            // Check for escape sequence
            if (ch == 0x1B && col + 1 < 40)
            {
                col++;
                ch = data[offset++] & 0x3F; // Decode control code
            }
            
            // Process character (control code or displayable)
            ProcessCharacter(row, col, ch);
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
            
            // If control code, write as escape sequence
            if (ch < 0x20)
            {
                file.put(0x1B);           // ESC
                file.put(ch | 0x40);      // Encode: code OR 0x40
            }
            else
            {
                file.put(ch & 0x7F);      // Regular character (7-bit)
            }
        }
    }
    
    // Write footer
    file.put(0x00);
    file.put(0x00);
}
```

## Format Validation

To verify an EP1 file:

1. **Check file size**: Must be exactly 968 bytes
2. **Verify header**: Bytes 0-2 must be `FE 01 09`
3. **Check footer**: Bytes 966-967 should be `00 00` (optional check)
4. **Validate characters**: 
   - All characters should be 7-bit (check bit 7)
   - ESC (0x1B) should be followed by a valid byte

## Common Issues

### File Size
- EP1 files must be exactly 968 bytes
- Missing rows or truncated files will fail to parse
- Extra data after footer is typically ignored

### Character Encoding
- Remember to strip bit 7 (parity bit) when reading
- Escape sequences use `& 0x3F` not `- 0x40`
- Control codes below 0x20 must be escaped

### Row Mapping
- EP1 row 0 = Teletext row 1
- EP1 row 23 = Teletext row 24
- Teletext row 0 (header) is not stored in EP1

## Compatibility

EP1 format is compatible with:
- **Edit.tf** - Original teletext editor
- **wxTED** - Cross-platform teletext editor
- **TTI Thumbnail Provider** - Windows thumbnail renderer

## Example File

Hex dump of a minimal EP1 file (first 100 bytes):

```
Offset  00 01 02 03 04 05 06 07  08 09 0A 0B 0C 0D 0E 0F  ASCII
------  -----------------------  -----------------------  ----------------
000000  FE 01 09 00 00 00 1B 47  54 65 73 74 20 50 61 67  .......GTest Pag
000010  65 20 20 20 20 20 20 20  20 20 20 20 20 20 20 20  e               
000020  20 20 20 20 20 20 20 20  20 20 20 20 20 1B 41 52                .AR
000030  45 44 20 20 20 1B 42 47  52 45 45 4E 20 20 1B 43  ED   .BGREEN  .C
000040  59 45 4C 4C 4F 57 20 1B  46 43 59 41 4E 20 20 20  YELLOW .FCYAN   
000050  20 20 20 20 20 20 20 20  20 20 20 20 20 20 20 20                  
000060  20 20 20 20                                        
```

This shows:
- Header: `FE 01 09 00 00 00`
- Row 0 data starting at offset 6
- Control codes like `1B 47` (White), `1B 41` (Red)
- Text content like "Test Page", "RED", "GREEN", etc.

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
