# MAC Changer

This repository provides tools to change or reset MAC addresses on different operating systems: Windows, Linux, and macOS. The tools are implemented in C++ and each supports a set of functionalities tailored to their respective platforms.

## Files

### 1. `WIN-MAC-Changer.cpp`

This file contains a Windows-specific MAC address changer implemented using WMI (Windows Management Instrumentation). It allows you to:

- Reset a network interface.
- (The functionality to set a MAC address is not implemented in this example.)

**Usage:**

```bash
WIN-MAC-Changer.exe -r <interface_name>
```

- `-r <interface_name>`: Reset the network interface specified by `<interface_name>`

**Example:**

```bash
WIN-MAC-Changer.exe -r "Ethernet"
```

### 2. `LIN-MAC-Changer.cpp`

This file contains a Linux-specific MAC address changer that utilizes system commands. It provides functionality to:

- Reset a network interface.
- Set a new MAC address for a network interface.

**Usage:**

```bash
LIN-MAC-Changer -r <interface_name>
LIN-MAC-Changer -s <interface_name> <mac_address>
```

- `-r <interface_name>`: Reset the network interface specified by `<interface_name>`
- `-s <interface_name> <mac_address>`: Set a new MAC address for `<interface_name>`

**Example:**

```bash
LIN-MAC-Changer -r eth0
LIN-MAC-Changer -s eth0 00:11:22:33:44:55
```

### 3. `MacOS-MAC-Changer.cpp`

This file is for macOS and provides tools to:

- Reset a network interface.
- Set a new MAC address.

**Usage:**

```bash
MacOS-MAC-Changer -r <interface_name>
MacOS-MAC-Changer -s <mac_address>
```

- `-r <interface_name>`: Reset the network interface specified by `<interface_name>`
- `-s <mac_address>`: Set a new MAC address

**Example:**

```bash
MacOS-MAC-Changer -r en0
MacOS-MAC-Changer -s 00:11:22:33:44:55
```

## Building

To build these tools, you need to have a C++ compiler and the respective platform's development tools installed.

### For Windows:

1. Open the Developer Command Prompt for Visual Studio.
2. Navigate to the directory containing `WIN-MAC-Changer.cpp`.
3. Compile using:

   ```bash
   cl WIN-MAC-Changer.cpp /link /out:WIN-MAC-Changer.exe
   ```

### For Linux:

1. Open a terminal.
2. Navigate to the directory containing `LIN-MAC-Changer.cpp`.
3. Compile using:

   ```bash
   g++ LIN-MAC-Changer.cpp -o LIN-MAC-Changer
   ```

### For macOS:

1. Open a terminal.
2. Navigate to the directory containing `MacOS-MAC-Changer.cpp`.
3. Compile using:

   ```bash
   g++ MacOS-MAC-Changer.cpp -o MacOS-MAC-Changer
   ```

## Notes

- **Permissions**: Changing MAC addresses generally requires administrative privileges. Ensure you run the tools with sufficient permissions.
- **Error Handling**: Error handling is basic in these examples. For production use, consider enhancing error checking and handling.

## Contributing

Feel free to fork the repository and submit pull requests with improvements or additional features. If you encounter any issues, please open an issue in the repository.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
