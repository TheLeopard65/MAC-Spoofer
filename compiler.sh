#!/bin/bash

# Function to check if the script is run with root privileges
check_privileges() {
    if [[ $EUID -ne 0 ]]; then
        echo "[-] This script must be run with root privileges."
        exit 1
    fi
}

# Function to compile for Linux
compile_linux() {
    echo "[+] Compiling the Mac Spoofer for Linux..."
    g++ -o mac_spoofer_linux linux.cpp
    echo "[+] Compilation for Linux completed."
}

# Function to compile for macOS
compile_macos() {
    echo "[+] Compiling the Mac Spoofer for macOS..."
    g++ -o mac_spoofer_macos macOS.cpp
    echo "[+] Compilation for macOS completed."
}

# Function to compile for Windows
compile_windows() {
    echo "[+] Compiling the Mac Spoofer for Windows..."
    echo "[!] NOTE: Run this command in Developer Command Prompt for Visual Studio."
    cl /EHsc /Fe:mac_spoofer_windows.exe windows.cpp wbemuuid.lib
    echo "[+] Compilation for Windows completed."
}

# Main script execution
case "$(uname)" in
    Linux)
        check_privileges
        compile_linux
        ;;
    Darwin)
        check_privileges
        compile_macos
        ;;
    CYGWIN*|MINGW32*|MSYS*|MINGW*)
        compile_windows
        ;;
    *)
        echo "[-] Unsupported operating system."
        exit 1
        ;;
esac
