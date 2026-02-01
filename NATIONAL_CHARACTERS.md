# National Character Options

## Overview

Teletext supports 13 different national character sets to display language-specific characters. The renderer maps specific character codes (0x23, 0x24, 0x40, 0x5B-0x5F, 0x60, 0x7B-0x7F) to different Unicode characters depending on the selected national option.

## National Options

| Index | Language | Status |
|-------|----------|--------|
| 0 | English | **Implemented** |
| 1 | German | Placeholder (uses English) |
| 2 | Swedish/Finnish | Placeholder (uses English) |
| 3 | Italian | Placeholder (uses English) |
| 4 | French | Placeholder (uses English) |
| 5 | Portuguese/Spanish | Placeholder (uses English) |
| 6 | Czech/Slovak | Placeholder (uses English) |
| 7 | Romanian | Placeholder (uses English) |
| 8 | Serbian/Croatian/Slovenian | Placeholder (uses English) |
| 9 | Estonian | Placeholder (uses English) |
| 10 | Lettish/Lithuanian | Placeholder (uses English) |
| 11 | Polish | Placeholder (uses English) |
| 12 | Turkish | Placeholder (uses English) |

## Character Mapping Structure

National character mappings replace specific ASCII character codes with language-appropriate characters:

```
TTI Character Code → Unicode Character
```

### Mapped Character Positions

The following character codes can be remapped (all others pass through unchanged):

- `0x23` (#)
- `0x24` ($)
- `0x40` (@)
- `0x5B` ([)
- `0x5C` (\)
- `0x5D` (])
- `0x5E` (^)
- `0x5F` (_)
- `0x60` (`)
- `0x7B` ({)
- `0x7C` (|)
- `0x7D` (})
- `0x7E` (~)
- `0x7F` (DEL)

## English Mapping (National Option 0)

Currently implemented:

| TTI Code | Character | Unicode | Description |
|----------|-----------|---------|-------------|
| 0x23 | # | U+00A3 | £ (pound sign) |
| 0x24 | $ | U+0024 | $ (dollar sign) |
| 0x40 | @ | U+0040 | @ (at sign) |
| 0x5B | [ | U+2190 | ← (left arrow) |
| 0x5C | \ | U+00BD | ½ (one half) |
| 0x5D | ] | U+2192 | → (right arrow) |
| 0x5E | ^ | U+2191 | ↑ (up arrow) |
| 0x5F | _ | U+0023 | # (number sign) |
| 0x60 | ` | U+2014 | — (em dash) |
| 0x7B | { | U+00BC | ¼ (one quarter) |
| 0x7C | \| | U+2016 | ‖ (double vertical line) |
| 0x7D | } | U+00BE | ¾ (three quarters) |
| 0x7E | ~ | U+00F7 | ÷ (division sign) |
| 0x7F | DEL | U+E65F | █ (block - teletext glyph) |

## Implementation

### Data Structure

```cpp
// In TeletextPage class
int m_nationalOption;  // 0-12
std::map<uint8_t, wchar_t> m_nationalMaps[13];
```

### Initialization

```cpp
void TeletextPage::InitializeNationalMaps()
{
    // Create English mapping
    std::map<uint8_t, wchar_t> englishMap;
    englishMap[0x23] = 0x00A3;  // £
    englishMap[0x24] = 0x0024;  // $
    // ... etc
    
    // Copy to all 13 tables (placeholder)
    for (int i = 0; i < 13; i++)
    {
        m_nationalMaps[i] = englishMap;
    }
}
```

### Usage

```cpp
wchar_t TeletextPage::ApplyNationalCharMap(uint8_t ch)
{
    auto it = m_nationalMaps[m_nationalOption].find(ch);
    if (it != m_nationalMaps[m_nationalOption].end())
    {
        return it->second;
    }
    return (wchar_t)ch;  // No mapping, return as-is
}
```

### Applied During Parsing

```cpp
// In ParseLine and ParseEP1
else
{
    // Alphanumeric mode - apply national character mapping
    m_cells[rowIndex][colIndex].character = ApplyNationalCharMap(ch);
}
```

## Adding New Language Mappings

To add a specific language mapping (e.g., German = index 1):

1. Find the national character mapping specification for that language
2. Update `InitializeNationalMaps()`:

```cpp
void TeletextPage::InitializeNationalMaps()
{
    // ... English mapping code ...
    
    // German mapping (index 1)
    m_nationalMaps[1][0x23] = 0x0023;  // #
    m_nationalMaps[1][0x24] = 0x0024;  // $
    m_nationalMaps[1][0x40] = 0x00A7;  // § (section sign)
    m_nationalMaps[1][0x5B] = 0x00C4;  // Ä
    m_nationalMaps[1][0x5C] = 0x00D6;  // Ö
    m_nationalMaps[1][0x5D] = 0x00DC;  // Ü
    m_nationalMaps[1][0x5E] = 0x005E;  // ^
    m_nationalMaps[1][0x5F] = 0x005F;  // _
    m_nationalMaps[1][0x60] = 0x00B0;  // ° (degree)
    m_nationalMaps[1][0x7B] = 0x00E4;  // ä
    m_nationalMaps[1][0x7C] = 0x00F6;  // ö
    m_nationalMaps[1][0x7D] = 0x00FC;  // ü
    m_nationalMaps[1][0x7E] = 0x00DF;  // ß
    m_nationalMaps[1][0x7F] = 0xE65F;  // █ (block)
    
    // Repeat for other languages...
}
```

## Future Enhancements

### Dynamic Selection

Currently, the national option is hardcoded to 0 (English). Future enhancements could include:

1. **Parse from TTI file**: Look for national option command in TTI metadata
2. **User preference**: Allow user to select national option in settings
3. **Auto-detection**: Detect from page content or language hints

### Control Code

Teletext uses control code **0x08-0x0F** for selecting national character set during transmission. This could be implemented as:

```cpp
case 0x08: case 0x09: case 0x0A: case 0x0B:
case 0x0C: case 0x0D: case 0x0E: case 0x0F:
    m_nationalOption = ch - 0x08;  // Sets national option 0-7
    break;
```

## Testing

To test different national options:

1. Create test TTI files with characters in the mapped range (0x23-0x7F)
2. Temporarily change `m_nationalOption = 0;` to different values
3. Verify correct Unicode characters are displayed
4. Once all mappings are added, implement proper national option selection

## References

- Teletext specification: ETS 300 706
- National option character tables: Annex of ETS 300 706
- Unicode character references: unicode.org

## Example Test Content

TTI file content to test national characters:

```
OL,1,G#$@[\]^_`{|}~
```

**English (option 0) should display:**
```
£$@←½→↑#—¼‖¾÷█
```

**German (option 1) would display:**
```
#$§ÄÖÜ^_°äöüß█
```

(Once German mapping is implemented)
