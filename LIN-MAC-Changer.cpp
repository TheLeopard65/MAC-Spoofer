#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <vector>

using namespace std;

// Helper function to execute system commands and capture output
string exec(const string& cmd) {
    array<char, 128> buffer;
    string result;
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) {
        cerr << "[-] Failed to open pipe." << endl;
        return "";
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

// Function to validate MAC address
bool validAddress(const string& mac) {
    if (mac.length() == 17) {
        for (size_t i = 0; i < mac.length(); ++i) {
            if (i % 3 == 2) {
                if (mac[i] != ':') {
                    return false;
                }
            } else if (!((mac[i] >= '0' && mac[i] <= '9') || (mac[i] >= 'A' && mac[i] <= 'F') || (mac[i] >= 'a' && mac[i] <= 'f'))) {
                return false;
            }
        }
        return true;
    }
    return false;
}

// Function to reset network interface
void resetNIC(const string& interfaceName) {
    string cmd = "ifconfig " + interfaceName + " down && ifconfig " + interfaceName + " up";
    string result = exec(cmd);
    if (result.empty()) {
        cout << "[+] Network interface " << interfaceName << " reset successfully." << endl;
    } else {
        cerr << "[-] Failed to reset network interface: " << result << endl;
    }
}

// Function to set MAC address
void setMACAddress(const string& interfaceName, const string& macAddress) {
    string cmd = "ip link set dev " + interfaceName + " address " + macAddress;
    string result = exec(cmd);
    if (result.empty()) {
        cout << "[+] MAC address of " << interfaceName << " set to " << macAddress << " successfully." << endl;
    } else {
        cerr << "[-] Failed to set MAC address: " << result << endl;
    }
}

// Function to print usage instructions
void printUsage(const char* programName) {
    cout << "[-] USAGE SYNTAX: " << programName << " -r <interface_name> | -s <interface_name> <mac_address>" << endl;
    cout << "[~] EXPLANATION:" << endl;
    cout << "[!]  -r <interface_name>  Reset the network interface specified by <interface_name>" << endl;
    cout << "[!]  -s <interface_name> <mac_address> Set a new MAC address for <interface_name> to <mac_address>" << endl;
    cout << "[!]  <interface_name>      The name of the network interface (e.g., eth0, wlan0)" << endl;
    cout << "[!]  <mac_address>         The new MAC address to set in format XX:XX:XX:XX:XX:XX" << endl;
}

// Function to parse command-line arguments
bool parseArguments(int argc, char* argv[], string& action, string& interfaceName, string& macAddress) {
    if (argc < 3) {
        return false;
    }

    string option(argv[1]);
    if (option == "-r") {
        action = option;
        interfaceName = argv[2];
        return true;
    } else if (option == "-s" && argc == 4) {
        action = option;
        interfaceName = argv[2];
        macAddress = argv[3];
        return true;
    }
    return false;
}

// Main function to execute operations
int main(int argc, char* argv[]) {
    string action, interfaceName, macAddress;

    // Parse command-line arguments
    if (!parseArguments(argc, argv, action, interfaceName, macAddress)) {
        printUsage(argv[0]);
        return 1;
    }

    if (action == "-r") {
        resetNIC(interfaceName);
    } else if (action == "-s") {
        if (!validAddress(macAddress)) {
            cerr << "[-] Invalid MAC address format." << endl;
            printUsage(argv[0]);
            return 1;
        }
        setMACAddress(interfaceName, macAddress);
    }

    return 0;
}
