# Teletext2.ttf Font Information

## Overview

The `teletext2.ttf` font is required for proper rendering of teletext graphics characters. It contains 2×3 block graphics glyphs in the Unicode private use area.

## Font Specifications

### Character Mappings

The font must contain glyphs for teletext block graphics in the following Unicode ranges:

#### Contiguous Graphics (solid blocks)
- **U+E680 to U+E69F**: Patterns 0x00 to 0x1F (32 characters)
- **U+E6C0 to U+E6DF**: Patterns 0x20 to 0x3F (32 characters)

#### Separated Graphics (blocks with spacing)
- **U+E6A0 to U+E6BF**: Patterns 0x00 to 0x1F (32 characters)
- **U+E6E0 to U+E6FF**: Patterns 0x20 to 0x3F (32 characters)

Total: 128 graphics glyphs

### Block Pattern Encoding

Each glyph represents a 2×3 grid of pixels. The lower 6 bits of the character code determine which pixels are filled:

```
Bit positions in 2×3 grid:

    ┌───┬───┐
    │ 0 │ 1 │  ← Top row
    ├───┼───┤
    │ 2 │ 3 │  ← Middle row
    ├───┼───┤
    │ 4 │ 5 │  ← Bottom row
    └───┴───┘

Pattern bits: 5 4 3 2 1 0
```

### Examples

| Pattern | Binary | Hex | Contiguous | Separated | Description |
|---------|--------|-----|------------|-----------|-------------|
| 0x00 | 000000 | 0x00 | U+E680 | U+E6A0 | Empty |
| 0x01 | 000001 | 0x01 | U+E681 | U+E6A1 | Top-right only |
| 0x0F | 001111 | 0x0F | U+E68F | U+E6AF | Right column |
| 0x15 | 010101 | 0x15 | U+E695 | U+E6B5 | Right column, alternating |
| 0x3F | 111111 | 0x3F | U+E6DF | U+E6FF | Full block |

## Contiguous vs. Separated Graphics

### Contiguous Graphics
- Blocks are solid and touch each other
- Used when teletext control code 0x19 (Contiguous Graphics) is active (default)
- No gaps between pixels

### Separated Graphics
- Blocks have small gaps between them
- Used when teletext control code 0x1A (Separated Graphics) is active
- Gaps help distinguish individual mosaic blocks

## Font Requirements

The font should be:
- **Monospaced**: All glyphs must have the same width
- **Fixed cell size**: Glyphs should fill the character cell consistently
- **Clear blocks**: Graphics should be solid and clearly visible
- **Proper spacing**: For separated graphics, gaps should be approximately 1/5 of the block size

## Obtaining the Font

### Option 1: Use an Existing Teletext Font

Several teletext fonts are available that include these graphics characters:
- Search for "teletext font" or "Galax font"
- Look for fonts designed for teletext emulation
- Verify the font includes glyphs in the U+E680-U+E6FF range

### Option 2: Create Your Own Font

You can create a custom font using font editors like:

#### Using FontForge (Free, Open Source)

1. **Install FontForge**: Download from https://fontforge.org/

2. **Create New Font**:
   - File → New
   - Set encoding to Unicode (ISO 10646-1)

3. **Add Glyphs**:
   - Navigate to private use area: U+E680
   - For each pattern (0x00 to 0x3F):
     - Create contiguous glyph at U+E680 + pattern (for patterns 0x00-0x1F)
     - Create contiguous glyph at U+E6C0 + (pattern - 0x20) (for patterns 0x20-0x3F)
     - Create separated glyph at U+E6A0 + pattern (for patterns 0x00-0x1F)
     - Create separated glyph at U+E6E0 + (pattern - 0x20) (for patterns 0x20-0x3F)

4. **Draw Each Glyph**:
   - Set character width to match other characters (typically 600-1000 units)
   - Draw rectangles based on the bit pattern
   - For 2×3 grid, divide cell into 2 columns and 3 rows
   - Fill rectangles where corresponding bits are set

5. **Contiguous Example** (pattern 0x15 = binary 010101):
   ```
   [ ][ ]     Bit 0=0, Bit 1=1 → right filled
   [ ][ ]     Bit 2=0, Bit 3=1 → right filled  
   [ ][ ]     Bit 4=0, Bit 5=1 → right filled
   ```

6. **Separated Example** (same pattern with gaps):
   ```
   [ ] [ ]    Small gaps between blocks
   [ ] [ ]    
   [ ] [ ]
   ```

7. **Set Font Properties**:
   - Element → Font Info
   - Set Family Name: "Teletext2"
   - Set Weight: Regular
   - Set fixed pitch flag

8. **Export Font**:
   - File → Generate Fonts
   - Select TrueType
   - Save as `teletext2.ttf`

### Option 3: Convert from Existing Bitmap Font

If you have bitmap teletext graphics, you can convert them to a TrueType font:

1. Create bitmap images for each glyph (64 patterns × 2 styles = 128 glyphs)
2. Use a bitmap-to-font converter like:
   - FontForge (can import bitmap images)
   - BitFontMaker2 (online tool)
   - FontStruct (online tool)

## Testing the Font

After creating or obtaining the font:

1. **Install the font temporarily**:
   ```
   Right-click teletext2.ttf → Install for current user
   ```

2. **Test in a text editor**:
   - Open Notepad or similar
   - Change font to "Teletext2"
   - Type or paste Unicode characters U+E680, U+E681, etc.
   - Verify graphics blocks appear correctly

3. **Test with the thumbnail provider**:
   - Place `teletext2.ttf` next to `TTIThumbnailProvider.dll`
   - Generate thumbnails for TTI files
   - Check that graphics characters render properly

## Font Metrics

Recommended metrics for best results:

- **Units per EM**: 1000 or 2048
- **Glyph width**: Same as regular characters (typically 600 for monospace)
- **Block size**: Should fill most of the character cell
- **Separated gap**: Approximately 20% of block dimension
- **Ascent**: 800 (for 1000 units per EM)
- **Descent**: 200 (for 1000 units per EM)

## Python Script to Generate Pattern Mappings

Here's a helper script to generate the bit patterns:

```python
def get_pattern_unicode(pattern, separated=False):
    """Convert pattern (0x00-0x3F) to Unicode codepoint"""
    if separated:
        if pattern < 0x20:
            return 0xE6A0 + pattern
        else:
            return 0xE6E0 + (pattern - 0x20)
    else:
        if pattern < 0x20:
            return 0xE680 + pattern
        else:
            return 0xE6C0 + (pattern - 0x20)

def pattern_to_grid(pattern):
    """Convert pattern byte to 2x3 grid description"""
    grid = [
        [(pattern >> 0) & 1, (pattern >> 1) & 1],  # Top row
        [(pattern >> 2) & 1, (pattern >> 3) & 1],  # Middle row
        [(pattern >> 4) & 1, (pattern >> 5) & 1],  # Bottom row
    ]
    return grid

# Generate all patterns
for pattern in range(0x40):
    grid = pattern_to_grid(pattern)
    cont = get_pattern_unicode(pattern, False)
    sep = get_pattern_unicode(pattern, True)
    print(f"Pattern 0x{pattern:02X}: Contiguous U+{cont:04X}, Separated U+{sep:04X}")
    for row in grid:
        print(f"  {'█' if row[0] else ' '}{'█' if row[1] else ' '}")
```

## License Considerations

If you create a custom font:
- Specify the license (e.g., OFL, MIT, Public Domain)
- Include a LICENSE file with the font
- Ensure the license is compatible with your project

If using an existing font:
- Check and comply with the font's license
- Some licenses may require attribution
- Verify commercial use is permitted if applicable

## Troubleshooting

### Graphics Don't Display
- Verify font file is named exactly `teletext2.ttf`
- Check font is in same directory as DLL
- Test font installation manually
- Check Windows Event Viewer for font loading errors

### Wrong Graphics Appear
- Verify Unicode mappings match specification
- Check bit order in pattern encoding
- Test individual glyphs in font editor

### Spacing Issues
- Adjust glyph widths to be uniform
- Verify monospace flag is set
- Check font metrics match template

## Additional Resources

- Teletext specification: ITU-R BT.653
- Unicode private use areas: U+E000 to U+F8FF
- FontForge documentation: https://fontforge.org/docs/
- Teletext graphics explained: Wikipedia "Teletext" article
