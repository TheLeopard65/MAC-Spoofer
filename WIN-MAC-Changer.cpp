#include <windows.h>
#include <wbemidl.h>
#include <comdef.h>
#include <string>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <memory>
#include <vector>

using namespace std;

// Macro for checking OS type
#if defined(_WIN32) || defined(_WIN64)
#define isWindows() true
#else
#define isWindows() false
#endif

// Struct for storing output to getIWbemClassObjectField()
struct VariantType {
    enum Type {
        VT_BSTR,
        VT_I4,
        VT_DISPATCH,
        VT_I2,
        VT_NULL,
        VT_BOOL
    } type;
    union {
        LONG lVal;
        IDispatch* pdispVal;
        BSTR bstrVal;
        SHORT iVal;
        VARIANT_BOOL boolVal;
    } value;
};

// Function prototypes
const BYTE* generateMACAddress();
char hexify(int);
shared_ptr<VariantType> getIWbemClassObjectField(IWbemClassObject*, const wstring&);
void resetNIC(IWbemServices*, const wstring&);
void printVariantType(const shared_ptr<VariantType>&);
string convertBSTRToString(BSTR);
bool validAddress(const string&);
void setupWmiApi(IWbemLocator**, IWbemServices**);
wstring getDeviceId(IWbemLocator*, IWbemServices*, const string&);
void printUsage(const char*);
bool parseArguments(int, char*[], string&, string&);

// Helper function to print colored messages
void printWithColor(const string& message, int colorCode) {
    // 0 - Reset, 1 - Red, 2 - Green, 3 - Yellow
    string colorCodes[] = {"\033[0m", "\033[31m", "\033[32m", "\033[33m"};
    cout << colorCodes[colorCode] << message << colorCodes[0] << endl;
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

// Function to parse command-line arguments
bool parseArguments(int argc, char* argv[], string& action, string& value) {
    if (argc != 3) {
        return false;
    }

    string option(argv[1]);
    if (option == "-r" || option == "-s") {
        action = option;
        value = argv[2];
        return true;
    }
    return false;
}

// Function to get the device ID
wstring getDeviceId(IWbemLocator* pLoc, IWbemServices* pSvc, const string& name) {
    wstring wName(name.begin(), name.end());
    wstring query = L"SELECT * FROM Win32_NetworkAdapter WHERE Name = \"" + wName + L"\"";
    _bstr_t langArg(L"WQL");
    _bstr_t queryArg(query.c_str());

    IEnumWbemClassObject* pEnum = nullptr;
    HRESULT hr = pSvc->ExecQuery(langArg, queryArg, WBEM_FLAG_FORWARD_ONLY, nullptr, &pEnum);

    if (FAILED(hr)) {
        printWithColor("[-] Unable to retrieve network adapters. Error code = " + to_string(hr), 1);
        pLoc->Release();
        pSvc->Release();
        exit(1);
    }

    IWbemClassObject* obj = nullptr;
    ULONG numElm = 0;
    hr = pEnum->Next(WBEM_INFINITE, 1, &obj, &numElm);

    if (SUCCEEDED(hr) && obj != nullptr) {
        auto pDeviceID = getIWbemClassObjectField(obj, L"DeviceID");
        wstring deviceId(pDeviceID->value.bstrVal, SysStringLen(pDeviceID->value.bstrVal));
        obj->Release();
        pEnum->Release();
        return deviceId;
    } else {
        printWithColor("[-] No adapter was found with name \"" + name + "\".", 1);
        pLoc->Release();
        pSvc->Release();
        exit(1);
    }
}

// Sets up WMI API
void setupWmiApi(IWbemLocator** ppLoc, IWbemServices** ppSvc) {
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr)) {
        printWithColor("[-] Failed to initialize COM library. Error code = " + to_string(hr), 1);
        exit(1);
    }

    hr = CoInitializeSecurity(nullptr, -1, nullptr, nullptr, RPC_C_AUTHN_LEVEL_CONNECT, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE, 0);
    if (FAILED(hr)) {
        printWithColor(" [-] Failed to initialize security. Error code = " + to_string(hr), 1);
        CoUninitialize();
        exit(1);
    }

    hr = CoCreateInstance(CLSID_WbemLocator, nullptr, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)ppLoc);
    if (FAILED(hr)) {
        printWithColor("[-] Failed to create IWbemLocator object. Error code = " + to_string(hr), 1);
        CoUninitialize();
        exit(1);
    }

    hr = (*ppLoc)->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), nullptr, nullptr, nullptr, 0, nullptr, nullptr, ppSvc);
    if (FAILED(hr)) {
        printWithColor("[-] Unable to connect to ROOT\\CIMV2. Error code = " + to_string(hr), 1);
        (*ppLoc)->Release();
        CoUninitialize();
        exit(1);
    }

    hr = CoSetProxyBlanket(*ppSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, nullptr, EOAC_NONE);
    if (FAILED(hr)) {
        printWithColor("[-] Could not set proxy blanket. Error code = " + to_string(hr), 1);
        (*ppSvc)->Release();
        (*ppLoc)->Release();
        CoUninitialize();
        exit(1);
    }
}

// Validates MAC address
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

// Converts BSTR to std::string
string convertBSTRToString(BSTR s) {
    wstring ws(s, SysStringLen(s));
    return string(ws.begin(), ws.end());
}

// Gets specified field of input IWbemClassObject accordingly
shared_ptr<VariantType> getIWbemClassObjectField(IWbemClassObject* obj, const wstring& field) {
    VARIANT vRet;
    VariantInit(&vRet);
    HRESULT hr = obj->Get(field.c_str(), 0, &vRet, nullptr, nullptr);
    
    if (FAILED(hr)) {
        printWithColor("[-] Unable to get IWbemClassObject's field. Error code = " + to_string(hr), 1);
        exit(1);
    }

    auto rtn = make_shared<VariantType>();
    if (vRet.vt == VT_BSTR) {
        rtn->value.bstrVal = SysAllocString(vRet.bstrVal);
        rtn->type = VariantType::VT_BSTR;
    } else if (vRet.vt == VT_I4) {
        rtn->value.lVal = vRet.lVal;
        rtn->type = VariantType::VT_I4;
    } else if (vRet.vt == VT_DISPATCH) {
        rtn->value.pdispVal = vRet.pdispVal;
        rtn->type = VariantType::VT_DISPATCH;
    } else if (vRet.vt == VT_I2) {
        rtn->value.iVal = vRet.iVal;
        rtn->type = VariantType::VT_I2;
    } else if (vRet.vt == VT_NULL) {
        rtn->type = VariantType::VT_NULL;
    } else if (vRet.vt == VT_BOOL) {
        rtn->value.boolVal = vRet.boolVal;
        rtn->type = VariantType::VT_BOOL;
    } else {
        printWithColor("[-] Unexpected variant type. Error code = " + to_string(vRet.vt), 1);
        VariantClear(&vRet);
        exit(1);
    }
    VariantClear(&vRet);
    return rtn;
}

// Resets NIC (Network Interface Card)
void resetNIC(IWbemServices* pSvc, const wstring& deviceId) {
    wstring query = L"SELECT * FROM Win32_NetworkAdapterConfiguration WHERE SettingID = \"" + deviceId + L"\"";
    _bstr_t langArg(L"WQL");
    _bstr_t queryArg(query.c_str());

    IEnumWbemClassObject* pEnum = nullptr;
    HRESULT hr = pSvc->ExecQuery(langArg, queryArg, WBEM_FLAG_FORWARD_ONLY, nullptr, &pEnum);

    if (FAILED(hr)) {
        printWithColor("[-] Unable to query network adapter configurations. Error code = " + to_string(hr), 1);
        exit(1);
    }

    IWbemClassObject* obj = nullptr;
    ULONG numElm = 0;
    hr = pEnum->Next(WBEM_INFINITE, 1, &obj, &numElm);

    if (SUCCEEDED(hr) && obj != nullptr) {
        auto pEnable = getIWbemClassObjectField(obj, L"Enable");
        if (pEnable->type == VariantType::VT_I4 && pEnable->value.lVal == 0) {
            printWithColor("The adapter is already enabled. Nothing to do.", 2);
        } else {
            printWithColor("[!] Enabling the adapter...", 3);
        }

        auto pMethod = getIWbemClassObjectField(obj, L"Enable");
        if (pMethod->type == VariantType::VT_DISPATCH) {
            DISPPARAMS dp = { nullptr, nullptr, 0, 0 };
            VARIANT vResult;
            VariantInit(&vResult);
            hr = pMethod->value.pdispVal->Invoke(0, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &dp, &vResult, nullptr, nullptr);
            if (FAILED(hr)) {
                printWithColor("[-] Unable to invoke the method. Error code = " + to_string(hr), 1);
                exit(1);
            }
            printWithColor("[+] Network adapter has been successfully enabled.", 2);
            VariantClear(&vResult);
        } else {
            printWithColor("[-] Unexpected variant type for method call.", 1);
            exit(1);
        }
        obj->Release();
    } else {
        printWithColor("[-] No network adapter found with SettingID \"" + convertBSTRToString(deviceId.c_str()) + "\".", 1);
    }
    pEnum->Release();
}

// Main function to execute the operations
int main(int argc, char* argv[]) {
    string action, value;

    // Parse command-line arguments
    if (!parseArguments(argc, argv, action, value)) {
        printUsage(argv[0]);
        return 1;
    }

    if (action == "-r") {
        IWbemLocator* pLoc = nullptr;
        IWbemServices* pSvc = nullptr;
        setupWmiApi(&pLoc, &pSvc);
        wstring deviceId = getDeviceId(pLoc, pSvc, value);
        resetNIC(pSvc, deviceId);
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();

        printWithColor("[+] Operation completed successfully.", 2);
    } else if (action == "-s") {
        if (!validAddress(value)) {
            printWithColor("[-] Invalid MAC address format.", 1);
            printUsage(argv[0]);
            return 1;
        }
        printWithColor("[+] Setting MAC address not implemented for Windows in this example.", 3);
    }

    return 0;
}
