@echo off
title Generate EchoSystem Documentation
echo ========================================
echo   Generating EchoSystem Documentation
echo ========================================
echo.

REM Проверка наличия Doxygen
where doxygen >nul 2>nul
if %errorlevel% neq 0 (
    echo ERROR: Doxygen not found!
    echo Please install Doxygen from https://www.doxygen.nl/
    pause
    exit /b 1
)

REM Проверка наличия Doxyfile_common
if not exist "Doxyfile_common" (
    echo ERROR: Doxyfile_common not found!
    echo Please create Doxyfile_common first.
    pause
    exit /b 1
)

REM Удаление старой документации (опционально)
echo Cleaning old documentation...
if exist docs rmdir /s /q docs

REM Генерация документации
echo Generating documentation...
doxygen Doxyfile_common

REM Проверка результата
if %errorlevel% equ 0 (
    echo.
    echo ========================================
    echo   Documentation generated successfully!
    echo   Open: docs/html/index.html
    echo ========================================
) else (
    echo.
    echo ERROR: Documentation generation failed!
    echo Check Doxyfile_common for errors.
)

echo.
pause