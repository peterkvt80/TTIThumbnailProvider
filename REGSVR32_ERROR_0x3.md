# Troubleshooting: RegSvr32 Exit Code 0x3

## Error Message

```
TTIThumbnailProvider
Unable to register the DLL/OCX: RegSvr32 failed with exit code 0x3
```

## What Does Exit Code 0x3 Mean?

Exit code 0x3 typically indicates one of these issues:
1. **Missing dependencies** - Required DLLs not found
2. **Path issues** - DLL path contains problematic characters or is too long
3. **Permission issues** - Insufficient privileges to register
4. **Runtime library missing** - Visual C++ Runtime not installed

## Most Common Cause: Missing Visual C++ Runtime

The thumbnail provider DLL requires the **Microsoft Visual C++ Redistributable** to be installed.

### Solution 1: Include VC++ Redistributable in Installer

Update your Inno Setup script to include the Visual C++ Runtime:

```ini
[Files]
; ... existing files ...

; Include VC++ Redistributable (adjust path to your version)
Source: "vc_redist.x64.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall

[Run]
; Install VC++ Redistributable before registering DLL
Filename: "{tmp}\vc_redist.x64.exe"; Parameters: "/quiet /norestart"; StatusMsg: "Installing Visual C++ Runtime..."; Flags: waituntilterminated

; ... existing run entries ...
```

**Where to get VC++ Redistributable:**
- Visual Studio 2019: https://aka.ms/vs/16/release/vc_redist.x64.exe
- Visual Studio 2022: https://aka.ms/vs/17/release/vc_redist.x64.exe

### Solution 2: Better Error Handling in Inno Setup

Improve the installation script to handle registration failures gracefully:

```ini
[Files]
; Use 'regserver' flag but add error checking
Source: "E:\dev\TTIIcon\TTIThumbnailProvider.dll"; DestDir: "{app}"; Flags: ignoreversion restartreplace; Components: thumbnails
; Remove 'regserver' flag and do manual registration with error handling

[Code]
function InitializeSetup(): Boolean;
var
  ResultCode: Integer;
  VCRuntimeInstalled: Boolean;
begin
  Result := True;
  
  // Check if VC++ Runtime is installed
  VCRuntimeInstalled := RegKeyExists(HKLM, 'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64');
  
  if not VCRuntimeInstalled then
  begin
    if MsgBox('The Microsoft Visual C++ Runtime is required but not installed. Install it now?', 
              mbConfirmation, MB_YESNO) = IDYES then
    begin
      // Installation will proceed and install runtime
      Result := True;
    end
    else
    begin
      MsgBox('Installation cannot continue without the Visual C++ Runtime.', mbError, MB_OK);
      Result := False;
    end;
  end;
end;

procedure CurStepChanged(CurStep: TSetupStep);
var
  ResultCode: Integer;
  DllPath: String;
begin
  if CurStep = ssPostInstall then
  begin
    // Register thumbnail provider DLL manually with error checking
    DllPath := ExpandConstant('{app}\TTIThumbnailProvider.dll');
    
    if FileExists(DllPath) then
    begin
      if not Exec('regsvr32.exe', '/s "' + DllPath + '"', '', SW_HIDE, ewWaitUntilTerminated, ResultCode) then
      begin
        MsgBox('Failed to execute regsvr32.exe', mbError, MB_OK);
      end
      else if ResultCode <> 0 then
      begin
        MsgBox('Failed to register thumbnail provider (Error code: ' + IntToStr(ResultCode) + ').' + #13#10 + 
               'Thumbnails may not work. Try installing the Visual C++ Redistributable.', 
               mbError, MB_OK);
      end;
    end;
  end;
end;
```

## Diagnostic Steps

### Step 1: Check Dependencies

Use **Dependency Walker** (depends.exe) to check what DLLs are missing:

1. Download Dependency Walker from https://www.dependencywalker.com/
2. Open `TTIThumbnailProvider.dll` in Dependency Walker
3. Look for any DLLs marked in red (missing)

**Common missing dependencies:**
- `MSVCP140.dll` - Visual C++ Runtime
- `VCRUNTIME140.dll` - Visual C++ Runtime
- `api-ms-win-*.dll` - Windows API sets (usually not an issue on Windows 10+)

### Step 2: Manual Registration Test

Try registering manually to see exact error:

```batch
cd "C:\Program Files\wxTED"
regsvr32 TTIThumbnailProvider.dll
```

If this fails, you'll see a more detailed error message.

### Step 3: Check Event Viewer

1. Open Event Viewer (eventvwr.msc)
2. Navigate to Windows Logs → Application
3. Look for errors around the time of installation
4. Check for "SideBySide" errors (indicate runtime issues)

### Step 4: Verify DLL Architecture

Ensure the DLL architecture matches the OS:

```batch
dumpbin /headers TTIThumbnailProvider.dll | findstr machine
```

Should show:
- `8664 machine (x64)` for 64-bit
- `14C machine (x86)` for 32-bit

**Important:** 64-bit Windows can run both, but you should prefer 64-bit DLL on 64-bit OS.

## Solutions by Root Cause

### If Missing MSVCP140.dll / VCRUNTIME140.dll

**Install Visual C++ Redistributable:**
- Download from: https://aka.ms/vs/17/release/vc_redist.x64.exe
- Run installer
- Retry wxTED installation

### If DLL Path Too Long

**Move installation to shorter path:**
```ini
; In Inno Setup
DefaultDirName={pf}\wxTED    ; Good (short)
; Instead of:
DefaultDirName={pf}\My Long Application Name\Version 1.0\wxTED    ; Bad (long)
```

### If Permission Issues

**Run installer as Administrator:**
1. Right-click setup.exe
2. Select "Run as administrator"
3. Accept UAC prompt

### If 32-bit/64-bit Mismatch

**Build correct architecture:**
- For 64-bit Windows: Build x64 version
- For 32-bit Windows: Build x86 version
- Or provide both in installer and detect at runtime

## Improved Inno Setup Script

Here's a complete improved version:

```ini
#define MyAppName "wxTED"
#define MyAppVersion "1.65"

[Setup]
AppId={{BF8D5847-6816-45D9-AB36-99F50155DCE4}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
DefaultDirName={pf}\{#MyAppName}
PrivilegesRequired=admin
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64
ChangesAssociations=yes

[Files]
Source: "E:\dev\wxTED-GitHub\wxted\bin\Release\wxTED.exe"; DestDir: "{app}"; Flags: ignoreversion; Components: wxTED
Source: "E:\dev\wxTED-GitHub\wxted\bin\Release\teletext2.ttf"; DestDir: "{fonts}"; FontInstall: "teletext2"; Flags: onlyifdoesntexist uninsneveruninstall; Components: wxTED

; Thumbnail Provider
Source: "E:\dev\TTIIcon\x64\Release\TTIThumbnailProvider.dll"; DestDir: "{app}"; Flags: ignoreversion restartreplace; Components: thumbnails; Check: Is64BitInstallMode
Source: "E:\dev\TTIIcon\teletext2.ttf"; DestDir: "{fonts}"; FontInstall: "teletext2"; Flags: onlyifdoesntexist uninsneveruninstall; Components: thumbnails
Source: "E:\dev\TTIIcon\teletext4.ttf"; DestDir: "{fonts}"; FontInstall: "teletext4"; Flags: onlyifdoesntexist uninsneveruninstall; Components: thumbnails

; VC++ Redistributable
Source: "vc_redist.x64.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall; Check: VCRedistNeedsInstall

[Run]
; Install VC++ Runtime if needed
Filename: "{tmp}\vc_redist.x64.exe"; Parameters: "/quiet /norestart"; StatusMsg: "Installing Visual C++ Runtime..."; Flags: waituntilterminated; Check: VCRedistNeedsInstall

[Code]
function VCRedistNeedsInstall: Boolean;
begin
  // Check if VC++ 2015-2022 Redistributable is installed
  Result := not RegKeyExists(HKLM, 'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64');
end;

procedure CurStepChanged(CurStep: TSetupStep);
var
  ResultCode: Integer;
  DllPath: String;
begin
  if CurStep = ssPostInstall then
  begin
    // Register thumbnail provider
    DllPath := ExpandConstant('{app}\TTIThumbnailProvider.dll');
    
    if FileExists(DllPath) then
    begin
      if Exec('regsvr32.exe', '/s "' + DllPath + '"', '', SW_HIDE, ewWaitUntilTerminated, ResultCode) then
      begin
        if ResultCode <> 0 then
        begin
          Log('RegSvr32 failed with code: ' + IntToStr(ResultCode));
          MsgBox('Thumbnail provider registration failed. Thumbnails may not work.' + #13#10 + 
                 'Error code: ' + IntToStr(ResultCode), mbError, MB_OK);
        end;
      end;
      
      // Clear thumbnail cache
      DelTree(ExpandConstant('{localappdata}\Microsoft\Windows\Explorer\thumbcache_*.db'), False, True, True);
      
      // Restart Explorer
      Exec('taskkill', '/f /im explorer.exe', '', SW_HIDE, ewNoWait, ResultCode);
      Exec('explorer.exe', '', '', SW_SHOWNORMAL, ewNoWait, ResultCode);
    end;
  end;
end;

procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
var
  ResultCode: Integer;
  DllPath: String;
begin
  if CurUninstallStep = usUninstall then
  begin
    // Unregister thumbnail provider
    DllPath := ExpandConstant('{app}\TTIThumbnailProvider.dll');
    
    if FileExists(DllPath) then
    begin
      Exec('regsvr32.exe', '/u /s "' + DllPath + '"', '', SW_HIDE, ewWaitUntilTerminated, ResultCode);
    end;
  end;
end;
```

## Quick Fix for Users

If users encounter this error during installation:

1. **Download and install VC++ Redistributable:**
   - Go to: https://aka.ms/vs/17/release/vc_redist.x64.exe
   - Run the installer
   - Reboot if prompted

2. **Retry wxTED installation:**
   - Run wxTED installer again
   - Should complete successfully

3. **If still fails, manual registration:**
   ```batch
   cd "C:\Program Files\wxTED"
   regsvr32 TTIThumbnailProvider.dll
   ```

## Prevention: Include Runtime in Installer

**Best practice:** Always include VC++ Runtime in your installer to avoid this issue entirely.

Add to your Inno Setup:
1. Download `vc_redist.x64.exe` 
2. Place in your installer source directory
3. Add to `[Files]` section
4. Add to `[Run]` section with `/quiet` parameter
5. Add check to only install if needed

This ensures users have all dependencies before DLL registration.
