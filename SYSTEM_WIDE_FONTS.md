# Font Installation - System-Wide Approach

## Overview

This document describes how to install teletext fonts system-wide so they're available to both wxTED and the thumbnail provider, as well as any other applications.

## Inno Setup Configuration

Your current Inno Setup script (lines 56-57) installs fonts to the Windows Fonts folder:

```ini
Source: "E:\dev\TTIIcon\teletext2.ttf"; DestDir: "{fonts}"; FontInstall: "teletext2"; Flags: onlyifdoesntexist uninsneveruninstall; Components: thumbnails
Source: "E:\dev\TTIIcon\teletext4.ttf"; DestDir: "{fonts}"; FontInstall: "teletext4"; Flags: onlyifdoesntexist uninsneveruninstall; Components: thumbnails
```

**This is now correct!** With the updated thumbnail provider code, this will work properly.

## How It Works

### Updated Thumbnail Provider Behavior

The thumbnail provider now checks **two locations** for fonts:

1. **First check: DLL directory**
   - Path: `C:\Program Files\wxTED\`
   - Checked first for faster loading
   
2. **Second check: Windows Fonts folder**
   - Path: `C:\Windows\Fonts\`
   - Checked if fonts not found in DLL directory

### Font Loading Process

```
RenderToBitmap() called
    ↓
Try DLL directory: C:\Program Files\wxTED\teletext2.ttf
    ↓
Not found? Try Windows Fonts: C:\Windows\Fonts\teletext2.ttf
    ↓
Found? Load with AddFontResourceExW(..., FR_PRIVATE, 0)
    ↓
Render thumbnail
    ↓
Unload with RemoveFontResourceExW()
```

## Benefits of System-Wide Installation

✅ **Shared resource**: One copy of fonts used by all applications
✅ **wxTED compatibility**: wxTED editor can use the same fonts
✅ **Standard location**: Fonts in expected Windows location
✅ **Easy management**: Can view fonts in Windows Fonts folder
✅ **Proper uninstall**: Inno Setup handles font uninstallation

## Installation Locations Comparison

| Aspect | DLL Directory | Windows Fonts |
|--------|---------------|---------------|
| Path | `C:\Program Files\wxTED\` | `C:\Windows\Fonts\` |
| Available to wxTED | No (unless added to path) | Yes |
| Available to other apps | No | Yes |
| Requires admin | Yes (for Program Files) | Yes |
| Shows in Font Manager | No | Yes |
| Inno Setup flag | `DestDir: "{app}"` | `DestDir: "{fonts}"` |

## Your Current Setup (CORRECT)

Your Inno Setup script is already configured correctly for system-wide installation:

```ini
[Files]
; wxTED gets the font too
Source: "...\teletext2.ttf"; DestDir: "{fonts}"; FontInstall: "teletext2"; Flags: onlyifdoesntexist uninsneveruninstall; Components: wxTED

; Thumbnail provider uses system fonts
Source: "...\teletext2.ttf"; DestDir: "{fonts}"; FontInstall: "teletext2"; Flags: onlyifdoesntexist uninsneveruninstall; Components: thumbnails
Source: "...\teletext4.ttf"; DestDir: "{fonts}"; FontInstall: "teletext4"; Flags: onlyifdoesntexist uninsneveruninstall; Components: thumbnails
```

**Note:** Line 51 and 56 both install teletext2.ttf - this is redundant but harmless due to the `onlyifdoesntexist` flag.

## Optional Optimization

You could remove the duplicate teletext2.ttf installation:

```ini
[Files]
; Existing wxTED files
Source: "E:\dev\wxTED-GitHub\wxted\bin\Release\wxTED.exe"; DestDir: "{app}"; Flags: ignoreversion; Components: wxTED
; ... other files ...

; Install fonts once, available to both wxTED and thumbnails
Source: "E:\dev\wxTED-GitHub\wxted\bin\Release\teletext2.ttf"; DestDir: "{fonts}"; FontInstall: "teletext2"; Flags: onlyifdoesntexist uninsneveruninstall; Components: wxTED thumbnails

; Thumbnail Provider DLL
Source: "E:\dev\TTIIcon\TTIThumbnailProvider.dll"; DestDir: "{app}"; Flags: ignoreversion regserver restartreplace; Components: thumbnails

; Double height font only needed by thumbnails
Source: "E:\dev\TTIIcon\teletext4.ttf"; DestDir: "{fonts}"; FontInstall: "teletext4"; Flags: onlyifdoesntexist uninsneveruninstall; Components: thumbnails
```

Note the `Components: wxTED thumbnails` - this installs the font if EITHER component is selected.

## Verification After Installation

Users can verify fonts are installed:

### Method 1: Font Settings
1. Open Windows Settings
2. Go to Personalization → Fonts
3. Look for "Teletext2" and "Teletext4"

### Method 2: Fonts Folder
1. Open File Explorer
2. Navigate to `C:\Windows\Fonts\`
3. Look for `teletext2.ttf` and `teletext4.ttf`

### Method 3: Check Thumbnails
1. Open folder with .tti or .ep1 files
2. View as Large Icons
3. Thumbnails should show proper graphics (not Courier New)

## Troubleshooting

### Thumbnails Still Use Courier New

**Check 1: Are fonts installed?**
```
dir C:\Windows\Fonts\teletext*.ttf
```
Should show both files.

**Check 2: Clear thumbnail cache**
```batch
del /f /s /q %LocalAppData%\Microsoft\Windows\Explorer\thumbcache_*.db
taskkill /f /im explorer.exe
start explorer.exe
```

**Check 3: Verify registration**
```
reg query "HKCR\.tti\ShellEx\{E357FCCD-A995-4576-B01F-234630154E96}"
```

### Font Installation Failed

If Inno Setup fails to install fonts:

1. **Manual installation:**
   - Navigate to font files
   - Right-click → Install for all users
   
2. **Check permissions:**
   - Ensure installer ran as administrator
   - Check Windows Fonts folder is writable

3. **Check font file validity:**
   - Open .ttf files in Windows Font Viewer
   - Verify glyphs are present

## Uninstallation

The `uninsneveruninstall` flag means fonts will **NOT** be removed during uninstall. This is intentional because:

- Other applications might be using the fonts
- Users might want to keep the fonts
- Fonts are small and don't take much space

If users want to manually remove fonts:
1. Open `C:\Windows\Fonts\`
2. Select `teletext2.ttf` and `teletext4.ttf`
3. Press Delete

## Summary

✅ **Your current Inno Setup script is correct**
✅ **Updated thumbnail provider now checks Windows Fonts folder**
✅ **Fonts will be available system-wide**
✅ **Both wxTED and thumbnail provider will work**
✅ **No changes to your Inno Setup script needed**

The issue was in the thumbnail provider code, not your installation script. The code has been updated to check the Windows Fonts folder as a fallback location.
