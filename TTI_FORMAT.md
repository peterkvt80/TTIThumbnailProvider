# TTI File Format Specification

## Overview

TTI (Teletext Intermediate) is a text-based format for storing teletext pages. This document describes the encoding used in TTI files and how the renderer processes them.

## File Structure

A typical TTI file consists of:

```
DE,<description>       # Description line
PN,<page_number>       # Page number (e.g., 10001)
SC,<subcode>          # Subcode
PS,<page_status>      # Page status
OL,<row>,<data>       # Output lines (rows 0-24)
FL,<row>,<data>       # Fastext lines (optional)
```

### Header Row (Row 0)

Row 0 is special - it contains the page header:
- The first 8 characters **must always be spaces**
- These 8 spaces are reserved and should not contain displayable content
- Page header content starts from column 8 onwards

Example:
```
OL,0,        GTest Teletext Page
     ^^^^^^^^ - Must be 8 spaces
              ^ - Control codes can appear from column 8
               ^^^^^^^^^^^^^^^^^^^ - Header content
```

## 7-Bit Encoding

TTI files use 7-bit encoding:
- All characters use only bits 0-6
- Bit 7 (parity bit) is always set to 0
- The renderer strips bit 7 when reading: `ch = ch & 0x7F`

## Control Character Encoding

Teletext control characters (codes 0x00-0x1F) cannot be directly stored in text files. The TTI format uses an escape sequence encoding:

### Encoding Rule

If a character code is below 0x20 (32 decimal):
1. Write ESC character (0x1B, 27 decimal)
2. Write the character code + 0x40 (64 decimal)

This converts codes 0x01-0x1F to a two-byte sequence: ESC followed by 0x41-0x5F

**Important:** Only lines beginning with `OL` have their control codes encoded this way.

### Decoding Rule

When reading a TTI file's OL lines:
1. Read each character and strip the parity bit (AND with 0x7F)
2. If character is 0x1B (ESC), read the next character
3. Subtract 0x40 from the next character to get the control code
4. Process the control code

### Example Encodings

| Control Code | Name | Hex Sequence | Bytes | Text Display |
|--------------|------|--------------|-------|--------------|
| 0x00 | Black Alpha | 1B 40 | ESC @ | May show as [@ or ^[@ |
| 0x01 | Red Alpha | 1B 41 | ESC A | May show as [A or ^[A |
| 0x02 | Green Alpha | 1B 42 | ESC B | May show as [B or ^[B |
| 0x03 | Yellow Alpha | 1B 43 | ESC C | May show as [C or ^[C |
| 0x04 | Blue Alpha | 1B 44 | ESC D | May show as [D or ^[D |
| 0x05 | Magenta Alpha | 1B 45 | ESC E | May show as [E or ^[E |
| 0x06 | Cyan Alpha | 1B 46 | ESC F | May show as [F or ^[F |
| 0x07 | White Alpha | 1B 47 | ESC G | May show as [G or ^[G |
| 0x10 | Black Graphics | 1B 50 | ESC P | May show as [P or ^[P |
| 0x11 | Red Graphics | 1B 51 | ESC Q | May show as [Q or ^[Q |
| 0x12 | Green Graphics | 1B 52 | ESC R | May show as [R or ^[R |
| 0x13 | Yellow Graphics | 1B 53 | ESC S | May show as [S or ^[S |
| 0x14 | Blue Graphics | 1B 54 | ESC T | May show as [T or ^[T |
| 0x15 | Magenta Graphics | 1B 55 | ESC U | May show as [U or ^[U |
| 0x16 | Cyan Graphics | 1B 56 | ESC V | May show as [V or ^[V |
| 0x17 | White Graphics | 1B 57 | ESC W | May show as [W or ^[W |
| 0x19 | Contiguous Graphics | 1B 59 | ESC Y | May show as [Y or ^[Y |
| 0x1A | Separated Graphics | 1B 5A | ESC Z | May show as [Z or ^[Z |
| 0x1C | Black Background | 1B 5C | ESC \ | May show as [\ or ^[\ |
| 0x1D | New Background | 1B 5D | ESC ] | May show as [] or ^[] |
| 0x0C | Normal Height | 1B 4C | ESC L | May show as [L or ^[L |
| 0x0D | Double Height | 1B 4D | ESC M | May show as [M or ^[M |

**Note:** Black (0x00/0x10) is not part of the original teletext standard but is supported by most modern devices. How the ESC character appears in a text editor depends on the editor. Some show it as `[`, others as `^[`, and some don't display it at all. The actual byte in the file is always 0x1B.

## Practical Example

Let's examine line 2 from the sample file:

**Hex bytes:**
```
1B 41 52 45 44 20 20 20 1B 42 47 52 45 45 4E 20 20 1B 43 59 45 4C 4C 4F 57 20 1B 46 43 59 41 4E
```

**Decoding:**
- `1B 41` = ESC + A → Control code 0x01 (Red alphanumeric)
- `52 45 44 20 20 20` = "RED   " (displayed in red)
- `1B 42` = ESC + B → Control code 0x02 (Green alphanumeric)
- `47 52 45 45 4E 20 20` = "GREEN  " (displayed in green)
- `1B 43` = ESC + C → Control code 0x03 (Yellow alphanumeric)
- `59 45 4C 4C 4F 57 20` = "YELLOW " (displayed in yellow)
- `1B 46` = ESC + F → Control code 0x06 (Cyan alphanumeric)
- `43 59 41 4E` = "CYAN" (displayed in cyan)

**In text editor (ESC may display as `[` or `^[`):**
```
OL,2,^[ARED   ^[BGREEN  ^[CYELLOW ^[FCYAN
```
or
```
OL,2,[ARED   [BGREEN  [CYELLOW [FCYAN
```

**Important:** What you see in the text editor depends on how it displays ESC (0x1B). The actual file contains the raw 0x1B byte.

## Complete Sample Line Breakdown

### Graphics Line Example
```
OL,5,[A````````````````````````````````````````
```

**Hex bytes:** `1B 41 60 60 60 60...`

Breaking this down:
- `OL,5,` - Output Line command for row 5
- `1B 41` - ESC + A (appears as `[A` in some editors) → Control code 0x01 (Red Alphanumeric)
- `60 60 60...` - Backtick characters (0x60) repeated 40 times

When Red Alphanumeric is active, the backticks display as regular text. If this were Red Graphics mode (`1B 51`), the backticks would appear as block graphics.

### Header Row Example
```
OL,0,[GTest TTeletext Page                   
```

**Hex bytes:** `1B 47 54 65 73 74...`

Breaking this down:
- `OL,0,` - Output Line command for row 0 (header)
- No explicit leading spaces in the data, but parser enforces 8 spaces in positions 0-7
- `1B 47` - ESC + G (appears as `[G` in some editors) → Control code 0x07 (White Alphanumeric)
- `54 65 73 74 20 54 54 65 6C 65 74 65 78 74 20 50 61 67 65` - "Test TTeletext Page"

### Graphics Mode Example
```
OL,24,[Q``````````````````````````````````````
```

**Hex bytes:** `1B 51 60 60 60 60...`

Breaking this down:
- `1B 51` - ESC + Q (appears as `[Q` in some editors) → Control code 0x11 (Red Graphics)
- `60 60 60...` - In graphics mode, these render as solid block characters

## Implementation in C++

```cpp
void ParseLine(const std::string& line, int rowIndex)
{
    // ... extract data from line ...
    
    int colIndex = (rowIndex == 0) ? 8 : 0; // Skip first 8 chars on row 0
    
    for (size_t i = 0; i < data.length() && colIndex < SCREEN_COLS; i++)
    {
        // Strip parity bit
        uint8_t ch = (uint8_t)data[i] & 0x7F;
        
        // Check for ESC (0x1B) followed by encoded control code
        if (ch == 0x1B && i + 1 < data.length())
        {
            // Read next character and decode: subtract 0x40
            i++;
            ch = ((uint8_t)data[i] & 0x7F) - 0x40;
        }
        
        // Process ch as control code or displayable character
        // ...
    }
}
```

## Control Code Effects

### Alphanumeric Color Codes (0x00-0x07)

When encountered, these codes:
1. Set the current foreground color (0x00=Black, 0x01=Red, 0x02=Green, 0x03=Yellow, 0x04=Blue, 0x05=Magenta, 0x06=Cyan, 0x07=White)
2. Switch to alphanumeric (text) mode
3. Display a space character

**Color Mapping:**
- 0x00 → Black (extended, not in original standard)
- 0x01 → Red
- 0x02 → Green
- 0x03 → Yellow
- 0x04 → Blue
- 0x05 → Magenta
- 0x06 → Cyan
- 0x07 → White

### Graphics Color Codes (0x10-0x17)

When encountered, these codes:
1. Set the current foreground color (same color mapping as alphanumeric: 0x10=Black through 0x17=White)
2. Switch to graphics mode
3. Display a space character

**Color Mapping:**
- 0x10 → Black (extended, not in original standard)
- 0x11 → Red
- 0x12 → Green
- 0x13 → Yellow
- 0x14 → Blue
- 0x15 → Magenta
- 0x16 → Cyan
- 0x17 → White

### Background Codes

- **0x1C (Black Background)**: Sets background to black
- **0x1D (New Background)**: Sets background to current foreground color

### Graphics Mode Codes

- **0x19 (Contiguous Graphics)**: Graphics characters displayed as solid blocks
- **0x1A (Separated Graphics)**: Graphics characters displayed with separation

### Height Control Codes

- **0x0C (Normal Height)**: Resets characters to normal height (default)
- **0x0D (Double Height)**: Makes subsequent characters double height
  - Each character is rendered at 2× size using the teletext4.ttf font
  - The top half sits in the current row, bottom half extends into the row below
  - Characters are not split - a single glyph spans two rows

## Character Interpretation

After control codes are processed, characters are interpreted based on the current mode:

### Alphanumeric Mode
- Characters 0x20-0x7D are displayed as their ASCII equivalents
- **Special mappings:**
  - 0x7E (`~` tilde) → U+00F7 (division sign ÷)
  - 0x7F → U+E65F (special teletext character in teletext2.ttf)

### Graphics Mode
- Characters 0x20-0x7F are interpreted as 2×3 block graphics
- Each character represents a 2×3 grid of pixels
- The 6 bits (bits 0-5) determine which pixels are lit

## Graphics Character Encoding

Graphics characters use the lower 6 bits to encode a 2×3 pixel grid:

```
Bit:  5 4 3 2 1 0
Pos:  ┌─┬─┐
      │0│1│  (top row)
      ├─┼─┤
      │2│3│  (middle row)
      ├─┼─┤
      │4│5│  (bottom row)
      └─┴─┘
```

For example:
- 0x20 (all bits 0) = empty
- 0x3F (all bits 1) = full block
- 0x21 (bit 0 set) = top-right pixel only

## Notes for wxTED Users

If your TTI files are generated by wxTED or compatible editors:
- They should already use the ESC+0x40 encoding
- Control codes are stored as 0x1B followed by (code+0x40)
- The 7-bit format should be maintained
- Header row (row 0) should be handled properly by the parser

## Validation

To verify a TTI file is properly encoded:

1. **Check for raw control codes**: No bytes 0x01-0x1F should appear directly in OL lines (0x1B for ESC is OK)
2. **Validate escape sequences**: Every 0x1B should be followed by a character 0x41-0x5F
3. **Verify line format**: Lines should start with OL, FL, PN, DE, SC, PS, etc.
4. **Check line length**: Data after OL,<row>, should result in 40 characters or less (when decoded)
5. **Header row rule**: OL,0 parser enforces 8 leading space positions

## Creating TTI Files Programmatically

When writing a TTI file:

```cpp
void WriteControlCode(std::ofstream& file, uint8_t code)
{
    if (code < 0x20)
    {
        // Encode as ESC + (code + 0x40)
        file.put(0x1B);           // ESC character
        file.put(code + 0x40);    // A-_ for codes 0x01-0x1F
    }
    else
    {
        // Regular character, ensure 7-bit
        file.put(code & 0x7F);
    }
}

void WriteHeaderRow(std::ofstream& file, const std::string& headerText)
{
    file << "OL,0,";
    
    // Row 0 content - parser will enforce 8 leading spaces
    // You can include them explicitly or let the parser handle it
    
    // Write header content with control codes as needed
    file << headerText << "\r\n";
}
```

**Important:** The ESC character (0x1B) must be written as an actual byte value, not as a visible character.

## Common Mistakes

1. **Forgetting to strip parity bit**: Always AND with 0x7F when reading
2. **Not using ESC character**: Control codes must be preceded by 0x1B (ESC)
3. **Wrong offset**: Using +0x30 instead of +0x40 for encoding
4. **Forgetting header handling**: Row 0 needs special parser handling for 8 leading spaces
5. **Treating all lines the same**: Only OL lines use control code encoding
6. **Text editor confusion**: ESC (0x1B) may display as `[`, `^[`, or not at all - but it's always 0x1B in the file

## Reference Implementation

The TTI Thumbnail Provider correctly implements:
- Parity bit stripping (& 0x7F)
- ESC sequence decoding (0x1B followed by (char - 0x40))
- Header row special handling (8 space positions enforced on row 0)
- All standard teletext control codes
- Proper color and mode switching

See `TeletextRenderer.cpp` function `ParseLine()` for the complete implementation.
