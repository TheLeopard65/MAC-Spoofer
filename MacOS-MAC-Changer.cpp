#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <regex>

using namespace std;

// Function prototypes
string executeCommand(const string&);
bool validMACAddress(const string&);
void printUsage(const char*);
void resetNetworkInterface(const string&);
void setMACAddress(const string&, const string&);

// Helper function to execute shell commands
string executeCommand(const string& command) {
    array<char, 128> buffer;
    string result;
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    
    if (!pipe) {
        throw runtime_error("popen() failed!");
    }

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    return result;
}

// Function to check if MAC address format is valid
bool validMACAddress(const string& mac) {
    regex mac_regex("^([0-9A-Fa-f]{2}:){5}[0-9A-Fa-f]{2}$");
    return regex_match(mac, mac_regex);
}

// Function to print usage instructions
void printUsage(const char* programName) {
    cout << "[-] USAGE SYNTAX: " << programName << " -r <interface_name> | -s <mac_address>" << endl;
    cout << "[~] EXPLANATION:" << endl;
    cout << "[!]  -r <interface_name>  Reset the network interface specified by <interface_name>" << endl;
    cout << "[!]  -s <mac_address>     Set a new MAC address specified by <mac_address>" << endl;
    cout << "[!]  <interface_name>      The name of the network interface to reset" << endl;
    cout << "[!]  <mac_address>         The new MAC address to set in format XX:XX:XX:XX:XX:XX" << endl;
}

// Function to reset the network interface
void resetNetworkInterface(const string& interfaceName) {
    string command = "sudo ifconfig " + interfaceName + " down && sudo ifconfig " + interfaceName + " up";
    string output = executeCommand(command);
    cout << "[+] Network interface " << interfaceName << " has been reset." << endl;
    cout << output << endl;
}

// Function to set the MAC address
void setMACAddress(const string& interfaceName, const string& macAddress) {
    if (!validMACAddress(macAddress)) {
        cout << "[-] Invalid MAC address format." << endl;
        return;
    }

    string command = "sudo ifconfig " + interfaceName + " ether " + macAddress;
    string output = executeCommand(command);
    cout << "[+] MAC address for " << interfaceName << " set to " << macAddress << "." << endl;
    cout << output << endl;
}

// Main function to execute the operations
int main(int argc, char* argv[]) {
    if (argc != 3) {
        printUsage(argv[0]);
        return 1;
    }

    string action(argv[1]);
    string value(argv[2]);

    if (action == "-r") {
        resetNetworkInterface(value);
    } else if (action == "-s") {
        size_t pos = value.find(':');
        if (pos == string::npos || pos == 0 || pos == value.length() - 1) {
            cout << "[-] Invalid MAC address format." << endl;
            printUsage(argv[0]);
            return 1;
        }

        string interfaceName = value.substr(0, pos);
        string macAddress = value.substr(pos + 1);
        setMACAddress(interfaceName, macAddress);
    } else {
        printUsage(argv[0]);
        return 1;
    }

    return 0;
}
