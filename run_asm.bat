@echo off
echo Assembling and linking %1...

REM Assemble the .asm file
nasm -f win64 %1 -o main.obj

REM Link without C library
link /subsystem:console /entry:_start main.obj /out:program.exe

REM Run the program if linking was successful
if %errorlevel% equ 0 (
    echo Running program...
    .\program.exe
) else (
    echo Linking failed.
)
