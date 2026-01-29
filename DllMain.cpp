#include <windows.h>
#include <shlwapi.h>
#include <strsafe.h>
#include "TTIThumbnailProvider.h"

// Global DLL reference count
long g_cDllRef = 0;
HINSTANCE g_hInst = NULL;

// Class factory implementation
class ClassFactory : public IClassFactory
{
public:
    ClassFactory() : m_cRef(1) { }

    // IUnknown
    IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv)
    {
        static const QITAB qit[] = 
        {
            QITABENT(ClassFactory, IClassFactory),
            { 0 },
        };
        return QISearch(this, qit, riid, ppv);
    }

    IFACEMETHODIMP_(ULONG) AddRef()
    {
        return InterlockedIncrement(&m_cRef);
    }

    IFACEMETHODIMP_(ULONG) Release()
    {
        ULONG cRef = InterlockedDecrement(&m_cRef);
        if (0 == cRef)
        {
            delete this;
        }
        return cRef;
    }

    // IClassFactory
    IFACEMETHODIMP CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppv)
    {
        if (pUnkOuter != NULL)
        {
            return CLASS_E_NOAGGREGATION;
        }

        TTIThumbnailProvider *pProvider = new (std::nothrow) TTIThumbnailProvider();
        if (!pProvider)
        {
            return E_OUTOFMEMORY;
        }

        HRESULT hr = pProvider->QueryInterface(riid, ppv);
        pProvider->Release();
        return hr;
    }

    IFACEMETHODIMP LockServer(BOOL fLock)
    {
        if (fLock)
        {
            InterlockedIncrement(&g_cDllRef);
        }
        else
        {
            InterlockedDecrement(&g_cDllRef);
        }
        return S_OK;
    }

private:
    long m_cRef;
};

// DLL Entry Point
BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    switch (dwReason)
    {
        case DLL_PROCESS_ATTACH:
            g_hInst = hModule;
            DisableThreadLibraryCalls(hModule);
            break;
    }
    return TRUE;
}

// DLL Export Functions
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void **ppv)
{
    if (rclsid != CLSID_TTIThumbnailProvider)
    {
        return CLASS_E_CLASSNOTAVAILABLE;
    }

    ClassFactory *pFactory = new (std::nothrow) ClassFactory();
    if (!pFactory)
    {
        return E_OUTOFMEMORY;
    }

    HRESULT hr = pFactory->QueryInterface(riid, ppv);
    pFactory->Release();
    return hr;
}

STDAPI DllCanUnloadNow()
{
    return (g_cDllRef > 0) ? S_FALSE : S_OK;
}

// Helper function to convert GUID to string
HRESULT CreateGUIDString(REFGUID guid, LPWSTR pszBuf, UINT cchBuf)
{
    int result = swprintf_s(pszBuf, cchBuf,
        L"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
        guid.Data1, guid.Data2, guid.Data3,
        guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
        guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
    
    return (result > 0) ? S_OK : E_FAIL;
}

// Registration helper
HRESULT RegisterInprocServer(PCWSTR pszModule, REFCLSID clsid, PCWSTR pszFriendlyName, PCWSTR pszThreadModel)
{
    HRESULT hr;
    WCHAR szCLSID[MAX_PATH];
    WCHAR szSubkey[MAX_PATH];

    hr = CreateGUIDString(clsid, szCLSID, ARRAYSIZE(szCLSID));
    if (SUCCEEDED(hr))
    {
        if (swprintf_s(szSubkey, ARRAYSIZE(szSubkey),
            L"CLSID\\%s", szCLSID) > 0)
        {
            hr = S_OK;
        }
        else
        {
            hr = E_FAIL;
        }
        
        if (SUCCEEDED(hr))
        {
            // Create CLSID key
            HKEY hKey = NULL;
            hr = HRESULT_FROM_WIN32(RegCreateKeyExW(HKEY_CLASSES_ROOT, szSubkey,
                0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL));
            if (SUCCEEDED(hr))
            {
                RegSetValueExW(hKey, NULL, 0, REG_SZ, (LPBYTE)pszFriendlyName,
                    (DWORD)((wcslen(pszFriendlyName) + 1) * sizeof(WCHAR)));

                // Create InprocServer32 key
                HKEY hSubkey = NULL;
                hr = HRESULT_FROM_WIN32(RegCreateKeyExW(hKey, L"InprocServer32",
                    0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hSubkey, NULL));
                if (SUCCEEDED(hr))
                {
                    RegSetValueExW(hSubkey, NULL, 0, REG_SZ, (LPBYTE)pszModule,
                        (DWORD)((wcslen(pszModule) + 1) * sizeof(WCHAR)));
                    RegSetValueExW(hSubkey, L"ThreadingModel", 0, REG_SZ,
                        (LPBYTE)pszThreadModel, (DWORD)((wcslen(pszThreadModel) + 1) * sizeof(WCHAR)));
                    RegCloseKey(hSubkey);
                }
                RegCloseKey(hKey);
            }
        }
    }
    return hr;
}

HRESULT UnregisterInprocServer(REFCLSID clsid)
{
    WCHAR szCLSID[MAX_PATH];
    WCHAR szSubkey[MAX_PATH];

    HRESULT hr = CreateGUIDString(clsid, szCLSID, ARRAYSIZE(szCLSID));
    if (SUCCEEDED(hr))
    {
        if (swprintf_s(szSubkey, ARRAYSIZE(szSubkey), L"CLSID\\%s", szCLSID) > 0)
        {
            hr = S_OK;
        }
        else
        {
            hr = E_FAIL;
        }
        
        if (SUCCEEDED(hr))
        {
            hr = HRESULT_FROM_WIN32(RegDeleteTreeW(HKEY_CLASSES_ROOT, szSubkey));
        }
    }
    return hr;
}

STDAPI DllRegisterServer()
{
    WCHAR szModule[MAX_PATH];
    if (GetModuleFileNameW(g_hInst, szModule, ARRAYSIZE(szModule)) == 0)
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    // Register COM server
    HRESULT hr = RegisterInprocServer(szModule, CLSID_TTIThumbnailProvider,
        L"TTI Thumbnail Provider", L"Apartment");

    if (SUCCEEDED(hr))
    {
        // Register thumbnail handler for .tti extension
        WCHAR szCLSID[MAX_PATH];
        CreateGUIDString(CLSID_TTIThumbnailProvider, szCLSID, ARRAYSIZE(szCLSID));

        HKEY hKey = NULL;
        hr = HRESULT_FROM_WIN32(RegCreateKeyExW(HKEY_CLASSES_ROOT,
            L".tti\\shellex\\{e357fccd-a995-4576-b01f-234630154e96}",
            0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL));
        
        if (SUCCEEDED(hr))
        {
            RegSetValueExW(hKey, NULL, 0, REG_SZ, (LPBYTE)szCLSID,
                (DWORD)((wcslen(szCLSID) + 1) * sizeof(WCHAR)));
            RegCloseKey(hKey);
        }
    }

    return hr;
}

STDAPI DllUnregisterServer()
{
    HRESULT hr = UnregisterInprocServer(CLSID_TTIThumbnailProvider);

    if (SUCCEEDED(hr))
    {
        RegDeleteTreeW(HKEY_CLASSES_ROOT, L".tti\\shellex");
    }

    return hr;
}
