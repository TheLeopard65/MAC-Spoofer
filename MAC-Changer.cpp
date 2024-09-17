#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <regex>

using namespace std;

const string GREEN = "\033[32m";   // Green
const string RED = "\033[31m";     // Red
const string YELLOW = "\033[33m";  // Yellow
const string RESET = "\033[0m";    // Reset to default color

void showUsage() {
    cout << YELLOW << "USAGE SYNTAX: MAC-Changer -i <INTERFACE> -m <MAC_ADDRESS>" << RESET << endl;
}

string execCommand(const string& command) {
    string result;
    char buffer[128];
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        cerr << RED << "[!] popen() failed!" << RESET << endl;
        return result;
    }
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    pclose(pipe);
    return result;
}

void macChangerLinuxMac(const string& interface, const string& newMAC) {
    cout << YELLOW << "[+] Changing MAC address for " << interface << " to " << newMAC << RESET << endl;

    execCommand("sudo ifconfig " + interface + " down");
    execCommand("sudo ifconfig " + interface + " hw ether " + newMAC);
    execCommand("sudo ifconfig " + interface + " up");
}

void macChangerWindows(const string& interface, const string& newMAC) {
    cout << YELLOW << "[+] Changing MAC address for " << interface << " to " << newMAC << RESET << endl;

    string command = "wmic path win32_networkadapter where (NetConnectionID=\"" + interface + "\") call "
                     "Enable";
    execCommand(command);
    command = "wmic path win32_networkadapter where (NetConnectionID=\"" + interface + "\") call "
              "Disable";
    execCommand(command);
    command = "netsh interface set interface \"" + interface + "\" newname=\"" + newMAC + "\"";
    execCommand(command);
}

string getCurrMAC(const string& interface) {
    string result = execCommand("ifconfig " + interface);
    regex macRegex(R"(\b([0-9a-fA-F]{2}[:-]){5}[0-9a-fA-F]{2}\b)");
    smatch macMatch;
    if (regex_search(result, macMatch, macRegex)) {
        return macMatch.str();
    } else {
        cerr << RED << "[!] MAC address cannot be read" << RESET << endl;
        return "";
    }
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        showUsage();
        return 1;
    }

    string interface;
    string mac;

    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "-i" && i + 1 < argc) {
            interface = argv[++i];
        } else if (arg == "-m" && i + 1 < argc) {
            mac = argv[++i];
        } else {
            showUsage();
            return 1;
        }
    }

    if (interface.empty() || mac.empty()) {
        showUsage();
        return 1;
    }

    #if defined(_WIN32) || defined(_WIN64)
    macChangerWindows(interface, mac);
    string currMAC = getCurrMAC(interface);

    if (currMAC == mac) {
        cout << RED << "[!] MAC has not changed: " << currMAC << RESET << endl;
    } else {
        cout << GREEN << "[+] New MAC-Address is: " << currMAC << RESET << endl;
    }
    #elif defined(__unix__) || defined(__APPLE__)
    macChangerLinuxMac(interface, mac);
    string currMAC = getCurrMAC(interface);

    if (currMAC == mac) {
        cout << RED << "[!] MAC has not changed: " << currMAC << RESET << endl;
    } else {
        cout << GREEN << "[+] New MAC-Address is: " << currMAC << RESET << endl;
    }
    #else
    cerr << RED << "[!] Unsupported Operating System" << RESET << endl;
    return 1;
    #endif

    return 0;
}
