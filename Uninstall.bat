@echo off
REM Uninstallation script for TTI Thumbnail Provider
REM Run as Administrator

echo Uninstalling TTI Thumbnail Provider...
echo.

REM Unregister the DLL
regsvr32 /u /s TTIThumbnailProvider.dll

if %ERRORLEVEL% EQU 0 (
    echo Successfully unregistered TTI Thumbnail Provider
    echo.
    echo Clearing thumbnail cache...
    
    REM Clear thumbnail cache
    del /f /s /q "%LocalAppData%\Microsoft\Windows\Explorer\thumbcache_*.db" 2>nul
    
    REM Restart Explorer
    echo Restarting Windows Explorer...
    taskkill /f /im explorer.exe
    start explorer.exe
    
    echo.
    echo Uninstallation complete!
) else (
    echo ERROR: Failed to unregister DLL
    echo Please make sure you are running as Administrator
)

echo.
pause
