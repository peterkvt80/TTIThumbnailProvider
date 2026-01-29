@echo off
REM Installation script for TTI Thumbnail Provider
REM Run as Administrator

echo Installing TTI Thumbnail Provider...
echo.

REM Register the DLL
regsvr32 /s TTIThumbnailProvider.dll

if %ERRORLEVEL% EQU 0 (
    echo Successfully registered TTI Thumbnail Provider
    echo.
    echo Clearing thumbnail cache...
    
    REM Clear thumbnail cache to force regeneration
    del /f /s /q "%LocalAppData%\Microsoft\Windows\Explorer\thumbcache_*.db" 2>nul
    
    REM Restart Explorer to apply changes
    echo Restarting Windows Explorer...
    taskkill /f /im explorer.exe
    start explorer.exe
    
    echo.
    echo Installation complete!
    echo Thumbnails should now appear for .tti files.
) else (
    echo ERROR: Failed to register DLL
    echo Please make sure you are running as Administrator
)

echo.
pause
