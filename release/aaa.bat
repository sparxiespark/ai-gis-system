@echo off
chcp 65001 >nul

set OSGEO4W_ROOT=D:\cpp\OSGeo4W

echo ========================================
echo  Starting Spatial Analysis Engine
echo ========================================

:: CRITICAL: Add gdal-dev/bin FIRST (before calling o4w_env.bat)
set PATH=%OSGEO4W_ROOT%\apps\gdal-dev\bin;%PATH%
set PATH=%OSGEO4W_ROOT%\apps\Qt5\bin;%PATH%
set PATH=%OSGEO4W_ROOT%\apps\qgis-ltr\bin;%PATH%

:: Now call o4w_env.bat
call "%OSGEO4W_ROOT%\bin\o4w_env.bat"

:: Double-check PATH priority
set PATH=%OSGEO4W_ROOT%\apps\gdal-dev\bin;%PATH%

echo.
echo Current PATH includes:
echo - %OSGEO4W_ROOT%\apps\gdal-dev\bin
echo - %OSGEO4W_ROOT%\apps\Qt5\bin
echo - %OSGEO4W_ROOT%\apps\qgis-ltr\bin
echo.

:: Verify GDAL DLL exists
if exist "%OSGEO4W_ROOT%\apps\gdal-dev\bin\gdal-dev313.dll" (
    echo [OK] Found gdal-dev313.dll
) else (
    echo [ERROR] gdal-dev313.dll not found!
    pause
    exit /b 1
)

echo.
echo Launching application...
echo ========================================
echo.

SpatialAnalysisEngine.exe

if errorlevel 1 (
    echo.
    echo [ERROR] Application crashed or exited with error!
    echo.
    pause
)