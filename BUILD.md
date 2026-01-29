# Build Instructions for TTI Thumbnail Provider

## Prerequisites

### Required Software

1. **Visual Studio 2019 or later** (Recommended)
   - Download from: https://visualstudio.microsoft.com/
   - Required components:
     - Desktop development with C++
     - Windows 10 SDK (10.0.19041.0 or later)
     - MSVC v142 or later compiler

2. **Alternative: CMake** (Optional)
   - Version 3.15 or later
   - Download from: https://cmake.org/

## Building with Visual Studio

### Step 1: Open the Project

1. Launch Visual Studio
2. Open the solution:
   - File → Open → Project/Solution
   - Navigate to `TTIThumbnailProvider.vcxproj`
   - Click Open

### Step 2: Select Configuration

Choose your build configuration from the toolbar:
- **Debug**: For development and testing (includes debug symbols)
- **Release**: For production use (optimized)

Choose your platform:
- **Win32**: For 32-bit Windows (compatible with both 32-bit and 64-bit Windows)
- **x64**: For 64-bit Windows only (better performance on 64-bit systems)

### Step 3: Build the Project

1. Build → Build Solution (or press F7)
2. Wait for compilation to complete
3. Check the Output window for any errors

### Step 4: Locate the Output

The compiled DLL will be located at:
- Debug builds: `Debug\TTIThumbnailProvider.dll` or `x64\Debug\TTIThumbnailProvider.dll`
- Release builds: `Release\TTIThumbnailProvider.dll` or `x64\Release\TTIThumbnailProvider.dll`

## Building with CMake

### Step 1: Generate Build Files

Open Command Prompt or PowerShell in the project directory:

```cmd
# Create build directory
mkdir build
cd build

# Generate Visual Studio project files
cmake .. -G "Visual Studio 16 2019" -A x64

# Or for 32-bit
cmake .. -G "Visual Studio 16 2019" -A Win32
```

### Step 2: Build

```cmd
# Build using CMake
cmake --build . --config Release

# Or for Debug
cmake --build . --config Debug
```

### Step 3: Install (Optional)

```cmd
cmake --install . --prefix "C:\Program Files\TTIThumbnailProvider"
```

## Building from Command Line (MSBuild)

### Prerequisites
- Visual Studio Build Tools installed
- Developer Command Prompt for VS

### Commands

```cmd
# Open Developer Command Prompt for Visual Studio

# Navigate to project directory
cd path\to\TTIThumbnailProvider

# Build Release version (64-bit)
msbuild TTIThumbnailProvider.vcxproj /p:Configuration=Release /p:Platform=x64

# Build Debug version (32-bit)
msbuild TTIThumbnailProvider.vcxproj /p:Configuration=Debug /p:Platform=Win32
```

## Common Build Issues and Solutions

### Issue: "Windows SDK not found"

**Solution:**
1. Open Visual Studio Installer
2. Modify your Visual Studio installation
3. Under "Individual components", search for "Windows 10 SDK"
4. Install at least one SDK version (10.0.19041.0 or later recommended)

### Issue: "Cannot open include file: 'thumbcache.h'"

**Solution:**
- Ensure Windows SDK is properly installed
- Check that the SDK include path is configured:
  - Project Properties → C/C++ → General → Additional Include Directories
  - Should include: `$(WindowsSdkDir)Include\$(WindowsSDKVersion)um`

### Issue: "Unresolved external symbol"

**Solution:**
- Verify all source files are included in the project
- Check that required libraries are linked:
  - Project Properties → Linker → Input → Additional Dependencies
  - Should include: `shlwapi.lib;gdi32.lib`

### Issue: "LNK1561: entry point must be defined"

**Solution:**
- Ensure `TTIThumbnailProvider.def` is included in the project
- Check Project Properties → Linker → Input → Module Definition File
- Should be set to: `TTIThumbnailProvider.def`

### Issue: Platform toolset not found

**Solution:**
1. Right-click project → Properties
2. Configuration Properties → General
3. Change "Platform Toolset" to your installed version
4. Apply and rebuild

## Customization Before Building

### Change GUID

Before building, you should generate your own unique GUID:

1. In Visual Studio: Tools → Create GUID
2. Select "Registry Format"
3. Click "New GUID"
4. Copy the GUID
5. Open `TTIThumbnailProvider.h`
6. Replace the GUID in `CLSID_TTIThumbnailProvider`

Example:
```cpp
// Replace this:
static const GUID CLSID_TTIThumbnailProvider = 
{ 0xF8A7B9C2, 0x1234, 0x5678, { 0x9A, 0xBC, 0xDE, 0xF0, 0x12, 0x34, 0x56, 0x78 } };

// With your own GUID from guidgen.exe
```

### Adjust Compiler Settings

For optimal performance:

1. Project Properties → C/C++ → Optimization
   - Optimization: Maximum Optimization (Favor Speed) `/O2`
   - Inline Function Expansion: Any Suitable `/Ob2`

2. Project Properties → C/C++ → Code Generation
   - Runtime Library: Multi-threaded DLL (`/MD` for Release)

## Testing the Build

### Step 1: Verify DLL

After building, verify the DLL:

```cmd
# Check DLL dependencies
dumpbin /dependents TTIThumbnailProvider.dll

# Check exported functions
dumpbin /exports TTIThumbnailProvider.dll
```

Expected exports:
- DllCanUnloadNow
- DllGetClassObject
- DllRegisterServer
- DllUnregisterServer

### Step 2: Test Registration

Test if the DLL can be registered:

```cmd
# Register (run as Administrator)
regsvr32 TTIThumbnailProvider.dll

# Should display: "DllRegisterServer in TTIThumbnailProvider.dll succeeded"

# Unregister
regsvr32 /u TTIThumbnailProvider.dll
```

### Step 3: Test with Sample File

1. Register the DLL
2. Create or copy a .tti file to a test folder
3. Open the folder in Windows Explorer
4. Switch to Large Icons or Extra Large Icons view
5. Verify that thumbnails appear

## Clean Build

To perform a clean build:

### Visual Studio
1. Build → Clean Solution
2. Build → Rebuild Solution

### Command Line
```cmd
msbuild TTIThumbnailProvider.vcxproj /t:Clean
msbuild TTIThumbnailProvider.vcxproj /t:Rebuild /p:Configuration=Release
```

## Creating a Release Package

After successfully building:

1. Create a folder for distribution:
   ```
   TTIThumbnailProvider-Release/
   ├── TTIThumbnailProvider.dll
   ├── Install.bat
   ├── Uninstall.bat
   ├── README.md
   └── sample.tti
   ```

2. Copy files:
   - The compiled DLL from `Release\` folder
   - Installation scripts
   - Documentation
   - Sample TTI file

3. Test the package:
   - Run Install.bat as Administrator
   - Verify thumbnails work
   - Run Uninstall.bat to test cleanup

## 64-bit vs 32-bit Considerations

### Which Version to Build?

- **32-bit (Win32)**: Works on both 32-bit and 64-bit Windows
- **64-bit (x64)**: Only works on 64-bit Windows, potentially better performance

### Recommendation

Build both versions and provide users with installation instructions:
- On 64-bit Windows: Use the 64-bit version
- On 32-bit Windows: Use the 32-bit version

## Debugging

### Attaching to Explorer for Debugging

1. Build Debug configuration
2. Debug → Attach to Process
3. Find `explorer.exe`
4. Attach
5. Navigate to a folder with .tti files
6. Breakpoints in your code should now hit

### Debug Output

Add debug output to your code:

```cpp
OutputDebugString(L"TTI: Parsing started\n");
```

View output in:
- Visual Studio: Debug → Windows → Output
- DebugView utility from Sysinternals

## Performance Optimization

For production builds:

1. Enable Link-Time Code Generation (LTCG):
   - Project Properties → C/C++ → Optimization → Whole Program Optimization: Yes
   - Project Properties → Linker → Optimization → Link Time Code Generation: Use Link Time Code Generation

2. Optimize for size or speed:
   - For smaller DLL: `/O1` or `/Os`
   - For faster execution: `/O2` or `/Ot`

## Troubleshooting Build Errors

### "Cannot find include file"

Check include paths:
```
Project Properties → C/C++ → General → Additional Include Directories
```

### "Cannot find library file"

Check library paths:
```
Project Properties → Linker → General → Additional Library Directories
```

### Unicode issues

Ensure UNICODE is defined:
```
Project Properties → C/C++ → Preprocessor → Preprocessor Definitions
```
Should include: `UNICODE;_UNICODE`

## Additional Resources

- [Windows Shell Extensions Documentation](https://docs.microsoft.com/en-us/windows/win32/shell/shell-exts)
- [IThumbnailProvider Interface](https://docs.microsoft.com/en-us/windows/win32/api/thumbcache/nn-thumbcache-ithumbnailprovider)
- [COM Programming in Windows](https://docs.microsoft.com/en-us/windows/win32/com/component-object-model--com--portal)
