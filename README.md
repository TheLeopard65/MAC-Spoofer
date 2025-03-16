# MAC Spoofer

![License](https://img.shields.io/badge/license-MIT-brightgreen)

## Overview

The **MAC Spoofer** is a cross-platform utility designed to change the MAC address of your network interfaces on Linux, macOS, and Windows systems. This tool is particularly useful for privacy-conscious users who want to obscure their device's identity on a network or for testing purposes in network security.

## Features

- **Cross-Platform Support**: Works on Linux, macOS, and Windows.
- **Easy to Use**: Simple command-line interface for setting and resetting MAC addresses.
- **Root Privileges Check**: Ensures the script is run with the necessary permissions.
- **Error Handling**: Provides clear error messages for common issues.

## Table of Contents

- [Installation](#installation)
- [Usage](#usage)
- [Supported Platforms](#supported-platforms)
- [Requirements](#requirements)
- [License](#license)
- [Contributing](#contributing)
- [Contact](#contact)

## Installation

1. **Clone the repository**:
   ```bash
   git clone https://github.com/TheLeopard65/MAC-Spoofer.git
   cd MAC-Spoofer
   ```

2. **Compile the source code**:
   Run the `compiler.sh` script to compile the appropriate version for your operating system:
   ```bash
   sudo bash compiler.sh
   ```

## Usage

The MAC Spoofer can be used with the following commands:

### Set a New MAC Address

```bash
./mac_spoofer_<os> -s <interface_name> <mac_address>
```

### Reset the Network Interface

```bash
./mac_spoofer_<os> -r <interface_name>
```

### Example

To set a new MAC address on a Linux system:
```bash
sudo ./mac_spoofer_linux -s eth0 00:11:22:33:44:55
```

To reset the network interface:
```bash
sudo ./mac_spoofer_linux -r eth0
```

### Command-Line Options

- `-s <interface_name> <mac_address>`: Set a new MAC address for the specified interface.
- `-r <interface_name>`: Reset the specified network interface.

## Supported Platforms

- **Linux**: Compiled with `g++`.
- **macOS**: Compiled with `g++`.
- **Windows**: Compiled with Visual Studio's Developer Command Prompt.

## Requirements

Before compiling, ensure you have the following installed:

- **Linux**:
  - `build-essential`
  - `g++`
  - `net-tools`
  - `iproute2`

- **macOS**:
  - Xcode Command Line Tools

- **Windows**:
  - Visual Studio with C++ development tools

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! If you have suggestions for improvements or new features, please open an issue or submit a pull request.

1. Fork the repository.
2. Create a new branch (`git checkout -b feature-branch`).
3. Make your changes and commit them (`git commit -m 'Add new feature'`).
4. Push to the branch (`git push origin feature-branch`).
5. Open a pull request.

---

Thank you for using MAC Spoofer! Happy spoofing!
