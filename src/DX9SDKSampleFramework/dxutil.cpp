//-----------------------------------------------------------------------------
// File: DXUtil.cpp
//
// Desc: Shortcut macros and functions for using DX objects
//
// Copyright (c) Microsoft Corporation. All rights reserved
//-----------------------------------------------------------------------------
#include "dxstdafx.h"

#pragma warning(push)
#pragma warning(disable : 6031) // return value ignored
#pragma warning(disable : 26812) // unscooped enum

#ifdef UNICODE
    typedef HINSTANCE (WINAPI* LPShellExecute)(HWND hwnd, LPCWSTR lpOperation, LPCWSTR lpFile, LPCWSTR lpParameters, LPCWSTR lpDirectory, INT nShowCmd);
#else
    typedef HINSTANCE (WINAPI* LPShellExecute)(HWND hwnd, LPCSTR lpOperation, LPCSTR lpFile, LPCSTR lpParameters, LPCSTR lpDirectory, INT nShowCmd);
#endif


#ifndef UNDER_CE
//-----------------------------------------------------------------------------
// Name: DXUtil_GetDXSDKMediaPathCch()
// Desc: Returns the DirectX SDK media path
//       cchDest is the size in TCHARs of strDest.  Be careful not to 
//       pass in sizeof(strDest) on UNICODE builds.
//-----------------------------------------------------------------------------
HRESULT DXUtil_GetDXSDKMediaPathCch( TCHAR* strDest, int cchDest )
{
    if( strDest == nullptr || cchDest < 1 )
        return E_INVALIDARG;

    lstrcpy( strDest, TEXT("") );

    // Open the appropriate registry key
    HKEY  hKey;
    LONG lResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                                _T("Software\\Microsoft\\DirectX SDK"),
                                0, KEY_READ, &hKey );
    if( ERROR_SUCCESS != lResult )
        return E_FAIL;

    DWORD dwType;
    DWORD dwSize = cchDest * sizeof(TCHAR);
    lResult = RegQueryValueEx( hKey, _T("DX9J3SDK Samples Path"), nullptr,
                              &dwType, (BYTE*)strDest, &dwSize );
    strDest[cchDest-1] = 0; // RegQueryValueEx doesn't nullptr term if buffer too small
    RegCloseKey( hKey );

    if( ERROR_SUCCESS != lResult )
        return E_FAIL;

    const TCHAR* strMedia = _T("\\Media\\");
    if (lstrlen(strDest) + lstrlen(strMedia) < cchDest)
        //_tcscat( strDest, strMedia );
        strcat_s(strDest, MAX_PATH, strMedia);
    else
        return E_INVALIDARG;

    return S_OK;
}
#endif // !UNDER_CE



#ifndef UNDER_CE
//-----------------------------------------------------------------------------
// Name: DXUtil_FindMediaFileCch()
// Desc: Trys to find the location of a SDK media file
//       cchDest is the size in TCHARs of strDestPath.  Be careful not to 
//       pass in sizeof(strDest) on UNICODE builds.
//-----------------------------------------------------------------------------
HRESULT DXUtil_FindMediaFileCch( TCHAR* strDestPath, int cchDest, LPCTSTR strFilename )
{
    HRESULT hr;
    TCHAR* strLeafNameTmp = nullptr;
    TCHAR strLeafName[MAX_PATH];
    TCHAR strExePath[MAX_PATH];
    TCHAR strExeName[MAX_PATH];
    TCHAR strMediaDir[MAX_PATH];
    TCHAR strSearchPath[MAX_PATH];
    TCHAR* strLastSlash = nullptr;
    int cchPath;
    BOOL bFound = FALSE;

    if( nullptr==strFilename || nullptr==strDestPath || cchDest < 10 )
        return E_INVALIDARG;

    lstrcpy( strDestPath, TEXT("") );
    lstrcpy( strLeafName, TEXT("") );

    // Append current working directory to strFilename and extract the leaf filename 
    cchPath = GetFullPathName(strFilename, MAX_PATH, strSearchPath, &strLeafNameTmp);
    if ((cchPath == 0) || (MAX_PATH <= cchPath))
        return E_FAIL;
    if( strLeafNameTmp )
        lstrcpyn( strLeafName, strLeafNameTmp, MAX_PATH );

    // Search in .\ 
    if( GetFileAttributes( strSearchPath ) != 0xFFFFFFFF )
        bFound = true;

    if( !bFound )
    {
        // Search in ..\ 
        _snprintf_s( strSearchPath, MAX_PATH, TEXT("..\\%s"), strLeafName );
        strSearchPath[MAX_PATH-1] = 0;
        if( GetFileAttributes( strSearchPath ) != 0xFFFFFFFF )
            bFound = true;
    }

    if( !bFound )
    {
        // Search in ..\..\  
        _snprintf_s( strSearchPath, MAX_PATH, TEXT("..\\..\\%s"), strLeafName );
        strSearchPath[MAX_PATH-1] = 0;
        if( GetFileAttributes( strSearchPath ) != 0xFFFFFFFF )
            bFound = true;
    }
    
    if( !bFound )
    {
        // Get the exe name, and exe path
        GetModuleFileName( nullptr, strExePath, MAX_PATH );
        strExePath[MAX_PATH-1]=0;
        strLastSlash = _tcsrchr( strExePath, TEXT('\\') );
        if( strLastSlash )
        {
            lstrcpyn( strExeName, &strLastSlash[1], MAX_PATH );

            // Chop the exe name from the exe path
            *strLastSlash = 0;

            // Chop the .exe from the exe name
            strLastSlash = _tcsrchr( strExeName, TEXT('.') );
            if( strLastSlash )
                *strLastSlash = 0;
        }

        // Search in the executable directory
        _snprintf_s( strSearchPath, MAX_PATH, _T("%s\\%s"), strExePath, strLeafName );
        strSearchPath[MAX_PATH-1] = 0;
        if( GetFileAttributes( strSearchPath ) != 0xFFFFFFFF )
            bFound = TRUE;

        if( !bFound )
        {
            // Search in "%EXE_DIR%\.."
            _snprintf_s( strSearchPath, MAX_PATH, _T("%s\\..\\%s"), strExePath, strLeafName );
            strSearchPath[MAX_PATH-1] = 0;
            if( GetFileAttributes( strSearchPath ) != 0xFFFFFFFF )
                bFound = TRUE;
        }

        if( !bFound )
        {
            // Search in "%EXE_DIR%\..\.."
            _snprintf_s( strSearchPath, MAX_PATH, _T("%s\\..\\..\\%s"), strExePath, strLeafName );
            strSearchPath[MAX_PATH-1] = 0;
            if( GetFileAttributes( strSearchPath ) != 0xFFFFFFFF )
                bFound = TRUE;
        }

        if( !bFound )
        {
            // Search in "%EXE_DIR%\..\%EXE_NAME%\".  This matches the DirectX SDK layout
            _snprintf_s( strSearchPath, MAX_PATH, TEXT("%s\\..\\%s\\%s"), strExePath, strExeName, strLeafName );
            strSearchPath[MAX_PATH-1] = 0;
            if( GetFileAttributes( strSearchPath ) != 0xFFFFFFFF )
                bFound = TRUE;
        }
    }
    
    if( !bFound )
    {
        // Get the media dir 
        if( FAILED( hr = DXUtil_GetDXSDKMediaPathCch( strMediaDir, MAX_PATH ) ) )
            return hr;

        // Search in SDK's media dir 
        _snprintf_s( strSearchPath, MAX_PATH, TEXT("%s%s"), strMediaDir, strLeafName );
        strSearchPath[MAX_PATH-1] = 0;
        if( GetFileAttributes( strSearchPath ) != 0xFFFFFFFF )
            bFound = TRUE;
    }

    if( bFound )
    {
        // Found the file, so copy the search path to the output buffer
        // and return success
        lstrcpyn( strDestPath, strSearchPath, cchDest );
        return S_OK;
    }
    else
    {
        // On failure, return the file as the path but return an error code
        lstrcpyn( strDestPath, strFilename, cchDest );
        return HRESULT_FROM_WIN32( ERROR_FILE_NOT_FOUND );
    }
}
#endif // !UNDER_CE




//-----------------------------------------------------------------------------
// Name: DXUtil_ReadStringRegKeyCch()
// Desc: Helper function to read a registry key string
//       cchDest is the size in TCHARs of strDest.  Be careful not to 
//       pass in sizeof(strDest) on UNICODE builds.
//-----------------------------------------------------------------------------
HRESULT DXUtil_ReadStringRegKeyCch( HKEY hKey, LPCTSTR strRegName, TCHAR* strDest, 
                                    DWORD cchDest, LPCTSTR strDefault )
{
    DWORD dwType;
    DWORD cbDest = cchDest * sizeof(TCHAR);

    if (strDest == nullptr)
        return S_FALSE;

    if( ERROR_SUCCESS != RegQueryValueEx( hKey, strRegName, 0, &dwType, 
                                          (BYTE*)strDest, &cbDest ) )
    {
        _tcsncpy_s( strDest, MAX_PATH, strDefault, cchDest );
        strDest[cchDest-1] = 0;
        return S_FALSE;
    }
    else
    {     
        if( dwType != REG_SZ )
        {
            _tcsncpy_s( strDest, MAX_PATH, strDefault, cchDest );
            strDest[cchDest-1] = 0;
            return S_FALSE;
        }   
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DXUtil_WriteStringRegKey()
// Desc: Helper function to write a registry key string
//-----------------------------------------------------------------------------
HRESULT DXUtil_WriteStringRegKey( HKEY hKey, LPCTSTR strRegName,
                                  LPCTSTR strValue )
{
    if( nullptr == strValue )
        return E_INVALIDARG;
        
    DWORD cbValue = ((DWORD)_tcslen(strValue)+1) * sizeof(TCHAR);

    if( ERROR_SUCCESS != RegSetValueEx( hKey, strRegName, 0, REG_SZ, 
                                        (BYTE*)strValue, cbValue ) )
        return E_FAIL;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DXUtil_ReadFloatRegKey()
// Desc: Helper function to read a registry key string
//-----------------------------------------------------------------------------
HRESULT DXUtil_ReadFloatRegKey( HKEY hKey, LPCTSTR strRegName, FLOAT* pfDest, FLOAT fDefault )
{
    if( nullptr == pfDest )
        return E_INVALIDARG;

    TCHAR sz[256];
    float fResult;

    TCHAR strDefault[256];
    _snprintf_s( strDefault, 256, TEXT("%f"), fDefault );
    strDefault[255] = 0;
   
    if( SUCCEEDED( DXUtil_ReadStringRegKeyCch( hKey, strRegName, sz, 256, strDefault ) ) )
    {
        int nResult = _stscanf_s( sz, TEXT("%f"), &fResult );
        if( nResult == 1 )
        {
            *pfDest = fResult;
            return S_OK;           
        }
    }

    *pfDest = fDefault;
    return S_FALSE;
}




//-----------------------------------------------------------------------------
// Name: DXUtil_WriteFloatRegKey()
// Desc: Helper function to write a registry key string
//-----------------------------------------------------------------------------
HRESULT DXUtil_WriteFloatRegKey( HKEY hKey, LPCTSTR strRegName, FLOAT fValue )
{
    TCHAR strValue[256];
    _snprintf_s( strValue, 256, TEXT("%f"), fValue );
    strValue[255] = 0;

    return DXUtil_WriteStringRegKey( hKey, strRegName, strValue );
}




//-----------------------------------------------------------------------------
// Name: DXUtil_ReadIntRegKey()
// Desc: Helper function to read a registry key int
//-----------------------------------------------------------------------------
HRESULT DXUtil_ReadIntRegKey( HKEY hKey, LPCTSTR strRegName, DWORD* pdwDest, 
                              DWORD dwDefault )
{
    DWORD dwType;
    DWORD dwLength = sizeof(DWORD);

    if( ERROR_SUCCESS != RegQueryValueEx( hKey, strRegName, 0, &dwType, 
                                          (BYTE*)pdwDest, &dwLength ) )
    {
        if (pdwDest != nullptr)
            *pdwDest = dwDefault;

        return S_FALSE;
    }
    else
    {
        if( dwType != REG_DWORD )
        {
            if (pdwDest != nullptr)
                *pdwDest = dwDefault;

            return S_FALSE;
        }
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DXUtil_WriteIntRegKey()
// Desc: Helper function to write a registry key int
//-----------------------------------------------------------------------------
HRESULT DXUtil_WriteIntRegKey( HKEY hKey, LPCTSTR strRegName, DWORD dwValue )
{
    if( ERROR_SUCCESS != RegSetValueEx( hKey, strRegName, 0, REG_DWORD, 
                                        (BYTE*)&dwValue, sizeof(DWORD) ) )
        return E_FAIL;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DXUtil_ReadBoolRegKey()
// Desc: Helper function to read a registry key BOOL
//-----------------------------------------------------------------------------
HRESULT DXUtil_ReadBoolRegKey( HKEY hKey, LPCTSTR strRegName, BOOL* pbDest, 
                              BOOL bDefault )
{
    DWORD dwType;
    DWORD dwLength = sizeof(BOOL);

    if( ERROR_SUCCESS != RegQueryValueEx( hKey, strRegName, 0, &dwType, 
                                          (BYTE*)pbDest, &dwLength ) )
    {
        if (pbDest != nullptr)
            *pbDest = bDefault;

        return S_FALSE;
    }
    else
    {
        if( dwType != REG_DWORD )
        {
            if (pbDest != nullptr)
                *pbDest = bDefault;

            return S_FALSE;
        }
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DXUtil_WriteBoolRegKey()
// Desc: Helper function to write a registry key BOOL
//-----------------------------------------------------------------------------
HRESULT DXUtil_WriteBoolRegKey( HKEY hKey, LPCTSTR strRegName, BOOL bValue )
{
    if( ERROR_SUCCESS != RegSetValueEx( hKey, strRegName, 0, REG_DWORD, 
                                        (BYTE*)&bValue, sizeof(BOOL) ) )
        return E_FAIL;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DXUtil_ReadGuidRegKey()
// Desc: Helper function to read a registry key guid
//-----------------------------------------------------------------------------
HRESULT DXUtil_ReadGuidRegKey( HKEY hKey, LPCTSTR strRegName, GUID* pGuidDest, 
                               GUID& guidDefault )
{
    DWORD dwType;
    DWORD dwLength = sizeof(GUID);

    if( ERROR_SUCCESS != RegQueryValueEx( hKey, strRegName, 0, &dwType, 
                                          (LPBYTE) pGuidDest, &dwLength ) )
    {
        *pGuidDest = guidDefault;
        return S_FALSE;
    }
    else
    {
        if( dwType != REG_BINARY )
        {
            if (pGuidDest != nullptr)
                *pGuidDest = guidDefault;

            return S_FALSE;
        }
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DXUtil_WriteGuidRegKey()
// Desc: Helper function to write a registry key guid
//-----------------------------------------------------------------------------
HRESULT DXUtil_WriteGuidRegKey( HKEY hKey, LPCTSTR strRegName, GUID guidValue )
{
    if( ERROR_SUCCESS != RegSetValueEx( hKey, strRegName, 0, REG_BINARY, 
                                        (BYTE*)&guidValue, sizeof(GUID) ) )
        return E_FAIL;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DXUtil_Timer()
// Desc: Performs timer opertations. Use the following commands:
//          TIMER_RESET           - to reset the timer
//          TIMER_START           - to start the timer
//          TIMER_STOP            - to stop (or pause) the timer
//          TIMER_ADVANCE         - to advance the timer by 0.1 seconds
//          TIMER_GETABSOLUTETIME - to get the absolute system time
//          TIMER_GETAPPTIME      - to get the current time
//          TIMER_GETELAPSEDTIME  - to get the time that elapsed between 
//                                  TIMER_GETELAPSEDTIME calls
//-----------------------------------------------------------------------------
FLOAT __stdcall DXUtil_Timer( TIMER_COMMAND command )
{
    static BOOL     m_bTimerInitialized = FALSE;
    static BOOL     m_bUsingQPF         = FALSE;
    static BOOL     m_bTimerStopped     = TRUE;
    static LONGLONG m_llQPFTicksPerSec  = 0;

    // Initialize the timer
    if( FALSE == m_bTimerInitialized )
    {
        m_bTimerInitialized = TRUE;

        // Use QueryPerformanceFrequency() to get frequency of timer.  If QPF is
        // not supported, we will timeGetTime() which returns milliseconds.
        LARGE_INTEGER qwTicksPerSec;
        m_bUsingQPF = QueryPerformanceFrequency( &qwTicksPerSec );
        if( m_bUsingQPF )
            m_llQPFTicksPerSec = qwTicksPerSec.QuadPart;
    }

    if( m_bUsingQPF )
    {
        static LONGLONG m_llStopTime        = 0;
        static LONGLONG m_llLastElapsedTime = 0;
        static LONGLONG m_llBaseTime        = 0;
        double fTime;
        double fElapsedTime;
        LARGE_INTEGER qwTime;
        
        // Get either the current time or the stop time, depending
        // on whether we're stopped and what command was sent
        if( m_llStopTime != 0 && command != TIMER_START && command != TIMER_GETABSOLUTETIME)
            qwTime.QuadPart = m_llStopTime;
        else
            QueryPerformanceCounter( &qwTime );

        // Return the elapsed time
        if( command == TIMER_GETELAPSEDTIME )
        {
            fElapsedTime = (double) ( qwTime.QuadPart - m_llLastElapsedTime ) / (double) m_llQPFTicksPerSec;
            m_llLastElapsedTime = qwTime.QuadPart;
            return (FLOAT) fElapsedTime;
        }
    
        // Return the current time
        if( command == TIMER_GETAPPTIME )
        {
            double fAppTime = (double) ( qwTime.QuadPart - m_llBaseTime ) / (double) m_llQPFTicksPerSec;
            return (FLOAT) fAppTime;
        }
    
        // Reset the timer
        if( command == TIMER_RESET )
        {
            m_llBaseTime        = qwTime.QuadPart;
            m_llLastElapsedTime = qwTime.QuadPart;
            m_llStopTime        = 0;
            m_bTimerStopped     = FALSE;
            return 0.0f;
        }
    
        // Start the timer
        if( command == TIMER_START )
        {
            if( m_bTimerStopped )
                m_llBaseTime += qwTime.QuadPart - m_llStopTime;
            m_llStopTime = 0;
            m_llLastElapsedTime = qwTime.QuadPart;
            m_bTimerStopped = FALSE;
            return 0.0f;
        }
    
        // Stop the timer
        if( command == TIMER_STOP )
        {
            if( !m_bTimerStopped )
            {
                m_llStopTime = qwTime.QuadPart;
                m_llLastElapsedTime = qwTime.QuadPart;
                m_bTimerStopped = TRUE;
            }
            return 0.0f;
        }
    
        // Advance the timer by 1/10th second
        if( command == TIMER_ADVANCE )
        {
            m_llStopTime += m_llQPFTicksPerSec/10;
            return 0.0f;
        }

        if( command == TIMER_GETABSOLUTETIME )
        {
            fTime = qwTime.QuadPart / (double) m_llQPFTicksPerSec;
            return (FLOAT) fTime;
        }

        return -1.0f; // Invalid command specified
    }
    else
    {
        // Get the time using timeGetTime()
        static double m_fLastElapsedTime  = 0.0;
        static double m_fBaseTime         = 0.0;
        static double m_fStopTime         = 0.0;
        double fTime;
        double fElapsedTime;
        
        // Get either the current time or the stop time, depending
        // on whether we're stopped and what command was sent
        if( m_fStopTime != 0.0 && command != TIMER_START && command != TIMER_GETABSOLUTETIME)
            fTime = m_fStopTime;
        else
            fTime = GETTIMESTAMP() * 0.001;
    
        // Return the elapsed time
        if( command == TIMER_GETELAPSEDTIME )
        {   
            fElapsedTime = (double) (fTime - m_fLastElapsedTime);
            m_fLastElapsedTime = fTime;
            return (FLOAT) fElapsedTime;
        }
    
        // Return the current time
        if( command == TIMER_GETAPPTIME )
        {
            return (FLOAT) (fTime - m_fBaseTime);
        }
    
        // Reset the timer
        if( command == TIMER_RESET )
        {
            m_fBaseTime         = fTime;
            m_fLastElapsedTime  = fTime;
            m_fStopTime         = 0;
            m_bTimerStopped     = FALSE;
            return 0.0f;
        }
    
        // Start the timer
        if( command == TIMER_START )
        {
            if( m_bTimerStopped )
                m_fBaseTime += fTime - m_fStopTime;
            m_fStopTime = 0.0f;
            m_fLastElapsedTime  = fTime;
            m_bTimerStopped = FALSE;
            return 0.0f;
        }
    
        // Stop the timer
        if( command == TIMER_STOP )
        {
            if( !m_bTimerStopped )
            {
                m_fStopTime = fTime;
                m_fLastElapsedTime  = fTime;
                m_bTimerStopped = TRUE;
            }
            return 0.0f;
        }
    
        // Advance the timer by 1/10th second
        if( command == TIMER_ADVANCE )
        {
            m_fStopTime += 0.1f;
            return 0.0f;
        }

        if( command == TIMER_GETABSOLUTETIME )
        {
            return (FLOAT) fTime;
        }

        return -1.0f; // Invalid command specified
    }
}




//-----------------------------------------------------------------------------
// Name: DXUtil_ConvertAnsiStringToWideCch()
// Desc: This is a UNICODE conversion utility to convert a CHAR string into a
//       WCHAR string. 
//       cchDestChar is the size in TCHARs of wstrDestination.  Be careful not to 
//       pass in sizeof(strDest) 
//-----------------------------------------------------------------------------
HRESULT DXUtil_ConvertAnsiStringToWideCch( WCHAR* wstrDestination, const CHAR* strSource, 
                                     int cchDestChar )
{
    if( wstrDestination==NULL || strSource==NULL || cchDestChar < 1 )
        return E_INVALIDARG;

    int nResult = MultiByteToWideChar( CP_ACP, 0, strSource, -1, 
                                       wstrDestination, cchDestChar );
    wstrDestination[cchDestChar-1] = 0;
    
    if( nResult == 0 )
        return E_FAIL;
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DXUtil_ConvertWideStringToAnsi()
// Desc: This is a UNICODE conversion utility to convert a WCHAR string into a
//       CHAR string. 
//       cchDestChar is the size in TCHARs of strDestination
//-----------------------------------------------------------------------------
HRESULT DXUtil_ConvertWideStringToAnsiCch( CHAR* strDestination, const WCHAR* wstrSource, 
                                     int cchDestChar )
{
    if( strDestination==NULL || wstrSource==NULL || cchDestChar < 1 )
        return E_INVALIDARG;

    int nResult = WideCharToMultiByte( CP_ACP, 0, wstrSource, -1, strDestination, 
                                       cchDestChar*sizeof(CHAR), nullptr, nullptr );
    strDestination[cchDestChar-1] = 0;
    
    if( nResult == 0 )
        return E_FAIL;
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DXUtil_ConvertGenericStringToAnsi()
// Desc: This is a UNICODE conversion utility to convert a TCHAR string into a
//       CHAR string. 
//       cchDestChar is the size in TCHARs of strDestination
//-----------------------------------------------------------------------------
HRESULT DXUtil_ConvertGenericStringToAnsiCch( CHAR* strDestination, const TCHAR* tstrSource, 
                                           int cchDestChar )
{
    if( strDestination==NULL || tstrSource==NULL || cchDestChar < 1 )
        return E_INVALIDARG;

#ifdef _UNICODE
    return DXUtil_ConvertWideStringToAnsiCch( strDestination, tstrSource, cchDestChar );
#else
    strncpy_s( strDestination, cchDestChar, tstrSource, cchDestChar );
    strDestination[cchDestChar-1] = '\0';
    return S_OK;
#endif   
}




//-----------------------------------------------------------------------------
// Name: DXUtil_ConvertGenericStringToWide()
// Desc: This is a UNICODE conversion utility to convert a TCHAR string into a
//       WCHAR string. 
//       cchDestChar is the size in TCHARs of wstrDestination.  Be careful not to 
//       pass in sizeof(strDest) 
//-----------------------------------------------------------------------------
HRESULT DXUtil_ConvertGenericStringToWideCch( WCHAR* wstrDestination, const TCHAR* tstrSource, 
                                           int cchDestChar )
{
    if( wstrDestination==NULL || tstrSource==NULL || cchDestChar < 1 )
        return E_INVALIDARG;

#ifdef _UNICODE
    wcsncpy( wstrDestination, tstrSource, cchDestChar );
    wstrDestination[cchDestChar-1] = L'\0';
    return S_OK;
#else
    return DXUtil_ConvertAnsiStringToWideCch( wstrDestination, tstrSource, cchDestChar );
#endif    
}




//-----------------------------------------------------------------------------
// Name: DXUtil_ConvertAnsiStringToGeneric()
// Desc: This is a UNICODE conversion utility to convert a CHAR string into a
//       TCHAR string. 
//       cchDestChar is the size in TCHARs of tstrDestination.  Be careful not to 
//       pass in sizeof(strDest) on UNICODE builds
//-----------------------------------------------------------------------------
HRESULT DXUtil_ConvertAnsiStringToGenericCch( TCHAR* tstrDestination, const CHAR* strSource, 
                                           int cchDestChar )
{
    if( tstrDestination==NULL || strSource==NULL || cchDestChar < 1 )
        return E_INVALIDARG;
        
#ifdef _UNICODE
    return DXUtil_ConvertAnsiStringToWideCch( tstrDestination, strSource, cchDestChar );
#else
    strncpy_s( tstrDestination, cchDestChar, strSource, cchDestChar );
    tstrDestination[cchDestChar-1] = '\0';
    return S_OK;
#endif    
}




//-----------------------------------------------------------------------------
// Name: DXUtil_ConvertAnsiStringToGeneric()
// Desc: This is a UNICODE conversion utility to convert a WCHAR string into a
//       TCHAR string. 
//       cchDestChar is the size in TCHARs of tstrDestination.  Be careful not to 
//       pass in sizeof(strDest) on UNICODE builds
//-----------------------------------------------------------------------------
HRESULT DXUtil_ConvertWideStringToGenericCch( TCHAR* tstrDestination, const WCHAR* wstrSource, 
                                           int cchDestChar )
{
    if( tstrDestination==NULL || wstrSource==NULL || cchDestChar < 1 )
        return E_INVALIDARG;

#ifdef _UNICODE
    wcsncpy( tstrDestination, wstrSource, cchDestChar );
    tstrDestination[cchDestChar-1] = L'\0';    
    return S_OK;
#else
    return DXUtil_ConvertWideStringToAnsiCch( tstrDestination, wstrSource, cchDestChar );
#endif
}




//-----------------------------------------------------------------------------
// Name: DXUtil_LaunchReadme()
// Desc: Finds and opens the readme for this sample
//-----------------------------------------------------------------------------
VOID DXUtil_LaunchReadme( HWND hWnd, LPCTSTR strLoc )
{
#ifdef UNDER_CE
    // This is not available on PocketPC
    MessageBox( hWnd, TEXT("For operating instructions, please open the ")
                      TEXT("readme.txt file included with the project."),
                TEXT("DirectX SDK Sample"), MB_ICONWARNING | MB_OK );

    return;
#endif

    const int NUM_FILENAMES = 2;
    LPCTSTR strFilenames[] = 
    {
        TEXT("readme.htm"),
        TEXT("readme.txt")
    };

    TCHAR strReadmePath[1024];
    TCHAR strExeName[MAX_PATH];
    TCHAR strExePath[MAX_PATH];
    TCHAR strSamplePath[MAX_PATH];
    TCHAR* strLastSlash = nullptr;

    for( int i=0; i < NUM_FILENAMES; i++ )
    {
        LPCTSTR strFilename = strFilenames[i];
        bool bSuccess = false;
        bool bFound = false;
        
        lstrcpy( strReadmePath, TEXT("") );
        lstrcpy( strExePath, TEXT("") );
        lstrcpy( strExeName, TEXT("") );
        lstrcpy( strSamplePath, TEXT("") );

        // If the user provided a location for the readme, check there first.
        if( strLoc )
        {
            HKEY  hKey;
            LONG lResult = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                                        _T("Software\\Microsoft\\DirectX SDK"),
                                        0, KEY_READ, &hKey );
            if( ERROR_SUCCESS == lResult )
            {
                DWORD dwType;
                DWORD dwSize = MAX_PATH * sizeof(TCHAR);
                lResult = RegQueryValueEx( hKey, _T("DX9J3SDK Samples Path"), nullptr,
                                        &dwType, (BYTE*)strSamplePath, &dwSize );
                strSamplePath[MAX_PATH-1] = 0; // RegQueryValueEx doesn't nullptr term if buffer too small
                
                if( ERROR_SUCCESS == lResult )
                {
                    _snprintf_s( strReadmePath, 1023, TEXT("%s\\C++\\%s\\%s"),
                                strSamplePath, strLoc, strFilename );
                    strReadmePath[1023] = 0;

                    if( GetFileAttributes( strReadmePath ) != 0xFFFFFFFF )
                        bFound = TRUE;
                }
            }

            RegCloseKey( hKey );
        }

        // Get the exe name, and exe path
        GetModuleFileName( nullptr, strExePath, MAX_PATH );
        strExePath[MAX_PATH-1]=0;

        strLastSlash = _tcsrchr( strExePath, TEXT('\\') );
        if( strLastSlash )
        {
            _tcsncpy_s( strExeName, &strLastSlash[1], MAX_PATH );
            strExeName[MAX_PATH-1]=0;

            // Chop the exe name from the exe path
            *strLastSlash = 0;

            // Chop the .exe from the exe name
            strLastSlash = _tcsrchr( strExeName, TEXT('.') );
            if( strLastSlash )
                *strLastSlash = 0;
        }

        if( !bFound )
        {
            // Search in "%EXE_DIR%\..\%EXE_NAME%\".  This matches the DirectX SDK layout
            strcpy_s( strReadmePath, strExePath );

            strLastSlash = _tcsrchr( strReadmePath, TEXT('\\') );
            if( strLastSlash )
                *strLastSlash = 0;
            lstrcat( strReadmePath, TEXT("\\") );
            lstrcat( strReadmePath, strExeName );
            lstrcat( strReadmePath, TEXT("\\") );
            lstrcat( strReadmePath, strFilename );
            if( GetFileAttributes( strReadmePath ) != 0xFFFFFFFF )
                bFound = TRUE;
        }

        if( !bFound )
        {
            // Search in "%EXE_DIR%\..\BumpMapping\%EXE_NAME%\".
            strcpy_s( strReadmePath, strExePath );

            strLastSlash = _tcsrchr( strReadmePath, TEXT('\\') );
            if( strLastSlash )
                *strLastSlash = 0;
            lstrcat( strReadmePath, TEXT("\\BumpMapping\\") );
            lstrcat( strReadmePath, strExeName );
            lstrcat( strReadmePath, TEXT("\\") );
            lstrcat( strReadmePath, strFilename );
            if( GetFileAttributes( strReadmePath ) != 0xFFFFFFFF )
                bFound = TRUE;
        }

        if( !bFound )
        {
            // Search in "%EXE_DIR%\..\EnvMapping\%EXE_NAME%\".
            strcpy_s( strReadmePath, strExePath );

            strLastSlash = _tcsrchr( strReadmePath, TEXT('\\') );
            if( strLastSlash )
                *strLastSlash = 0;
            lstrcat( strReadmePath, TEXT("\\EnvMapping\\") );
            lstrcat( strReadmePath, strExeName );
            lstrcat( strReadmePath, TEXT("\\") );
            lstrcat( strReadmePath, strFilename );
            if( GetFileAttributes( strReadmePath ) != 0xFFFFFFFF )
                bFound = TRUE;
        }

        if( !bFound )
        {
            // Search in "%EXE_DIR%\..\Meshes\%EXE_NAME%\".
            strcpy_s( strReadmePath, strExePath );

            strLastSlash = _tcsrchr( strReadmePath, TEXT('\\') );
            if( strLastSlash )
                *strLastSlash = 0;
            lstrcat( strReadmePath, TEXT("\\Meshes\\") );
            lstrcat( strReadmePath, strExeName );
            lstrcat( strReadmePath, TEXT("\\") );
            lstrcat( strReadmePath, strFilename );
            if( GetFileAttributes( strReadmePath ) != 0xFFFFFFFF )
                bFound = TRUE;
        }

        if( !bFound )
        {
            // Search in "%EXE_DIR%\..\StencilBuffer\%EXE_NAME%\".
            strcpy_s( strReadmePath, strExePath );

            strLastSlash = _tcsrchr( strReadmePath, TEXT('\\') );
            if( strLastSlash )
                *strLastSlash = 0;
            lstrcat( strReadmePath, TEXT("\\StencilBuffer\\") );
            lstrcat( strReadmePath, strExeName );
            lstrcat( strReadmePath, TEXT("\\") );
            lstrcat( strReadmePath, strFilename );
            if( GetFileAttributes( strReadmePath ) != 0xFFFFFFFF )
                bFound = TRUE;
        }

        if( !bFound )
        {
            // Search in "%EXE_DIR%\"
            strcpy_s( strReadmePath, strExePath );
            lstrcat( strReadmePath, TEXT("\\") );
            lstrcat( strReadmePath, strFilename );
            if( GetFileAttributes( strReadmePath ) != 0xFFFFFFFF )
                bFound = TRUE;
        }

        if( !bFound )
        {
            // Search in "%EXE_DIR%\.."
            strcpy_s( strReadmePath, strExePath );
            strLastSlash = _tcsrchr( strReadmePath, TEXT('\\') );
            if( strLastSlash )
                *strLastSlash = 0;
            lstrcat( strReadmePath, TEXT("\\") );
            lstrcat( strReadmePath, strFilename );
            if( GetFileAttributes( strReadmePath ) != 0xFFFFFFFF )
                bFound = TRUE;
        }

        if( !bFound )
        {
            // Search in "%EXE_DIR%\..\.."
            strcpy_s( strReadmePath, strExePath );
            strLastSlash = _tcsrchr( strReadmePath, TEXT('\\') );
            if( strLastSlash )
                *strLastSlash = 0;
            strLastSlash = _tcsrchr( strReadmePath, TEXT('\\') );
            if( strLastSlash )
                *strLastSlash = 0;
            lstrcat( strReadmePath, TEXT("\\") );
            lstrcat( strReadmePath, strFilename );
            if( GetFileAttributes( strReadmePath ) != 0xFFFFFFFF )
                bFound = TRUE;
        }

        if( bFound )
        {
            // GetProcAddress for ShellExecute, so we don't have to include shell32.lib 
            // in every project that uses dxutil.cpp
            LPShellExecute pShellExecute = nullptr;
            HINSTANCE hInstShell32 = LoadLibrary(TEXT("shell32.dll"));
            if (hInstShell32 != nullptr)
            {
    #ifdef UNICODE
                pShellExecute = (LPShellExecute)GetProcAddress(hInstShell32, _TWINCE("ShellExecuteW"));
    #else
                pShellExecute = (LPShellExecute)GetProcAddress(hInstShell32, _TWINCE("ShellExecuteA"));
    #endif
                if( pShellExecute != nullptr )
                {
                    if( pShellExecute( hWnd, TEXT("open"), strReadmePath, nullptr, nullptr, SW_SHOW ) > (HINSTANCE) 32 )
                        bSuccess = true;
                }

                FreeLibrary(hInstShell32);
            }
        }

        if( bSuccess )
            return;
    }

    // Tell the user that the readme couldn't be opened
    MessageBox( hWnd, TEXT("Could not find readme"), 
                TEXT("DirectX SDK Sample"), MB_ICONWARNING | MB_OK );
}





//-----------------------------------------------------------------------------
// Name: DXUtil_Trace()
// Desc: Outputs to the debug stream a formatted string with a variable-
//       argument list.
//-----------------------------------------------------------------------------
VOID DXUtil_Trace( LPCTSTR strMsg, ... )
{
#if defined(DEBUG) | defined(_DEBUG)
    TCHAR strBuffer[512] = { 0 };
    
    va_list args;
    va_start(args, strMsg);
    _vsntprintf_s( strBuffer, 512, strMsg, args );
    va_end(args);

    strBuffer[sizeof(strBuffer)-1] = '\0';
    OutputDebugString( strBuffer );
#else
    UNREFERENCED_PARAMETER(strMsg);
#endif
}




//-----------------------------------------------------------------------------
// Name: DXUtil_ConvertStringToGUID()
// Desc: Converts a string to a GUID
//-----------------------------------------------------------------------------
HRESULT DXUtil_ConvertStringToGUID( const TCHAR* strSrc, GUID* pGuidDest )
{
    UINT aiTmp[10];

    if( sscanf_s( strSrc, TEXT("{%8X-%4X-%4X-%2X%2X-%2X%2X%2X%2X%2X%2X}"),
                    &pGuidDest->Data1, 
                    &aiTmp[0], &aiTmp[1], 
                    &aiTmp[2], &aiTmp[3],
                    &aiTmp[4], &aiTmp[5],
                    &aiTmp[6], &aiTmp[7],
                    &aiTmp[8], &aiTmp[9] ) != 11 )
    {
        ZeroMemory( pGuidDest, sizeof(GUID) );
        return E_FAIL;
    }
    else
    {
        pGuidDest->Data2       = (USHORT) aiTmp[0];
        pGuidDest->Data3       = (USHORT) aiTmp[1];
        pGuidDest->Data4[0]    = (BYTE) aiTmp[2];
        pGuidDest->Data4[1]    = (BYTE) aiTmp[3];
        pGuidDest->Data4[2]    = (BYTE) aiTmp[4];
        pGuidDest->Data4[3]    = (BYTE) aiTmp[5];
        pGuidDest->Data4[4]    = (BYTE) aiTmp[6];
        pGuidDest->Data4[5]    = (BYTE) aiTmp[7];
        pGuidDest->Data4[6]    = (BYTE) aiTmp[8];
        pGuidDest->Data4[7]    = (BYTE) aiTmp[9];
        return S_OK;
    }
}




//-----------------------------------------------------------------------------
// Name: DXUtil_ConvertGUIDToStringCch()
// Desc: Converts a GUID to a string 
//       cchDestChar is the size in TCHARs of strDest.  Be careful not to 
//       pass in sizeof(strDest) on UNICODE builds
//-----------------------------------------------------------------------------
HRESULT DXUtil_ConvertGUIDToStringCch( const GUID* pGuidSrc, TCHAR* strDest, int cchDestChar )
{
    int nResult = _snprintf_s( strDest, 64, cchDestChar, TEXT("{%0.8X-%0.4X-%0.4X-%0.2X%0.2X-%0.2X%0.2X%0.2X%0.2X%0.2X%0.2X}"),
               pGuidSrc->Data1, pGuidSrc->Data2, pGuidSrc->Data3,
               pGuidSrc->Data4[0], pGuidSrc->Data4[1],
               pGuidSrc->Data4[2], pGuidSrc->Data4[3],
               pGuidSrc->Data4[4], pGuidSrc->Data4[5],
               pGuidSrc->Data4[6], pGuidSrc->Data4[7] );

    if( nResult < 0 )
        return E_FAIL;
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CArrayList constructor
// Desc: 
//-----------------------------------------------------------------------------
CArrayList::CArrayList( ArrayListType Type, UINT BytesPerEntry )
{
    if( Type == AL_REFERENCE )
        BytesPerEntry = sizeof(void*);
    m_ArrayListType = Type;
    m_pData = nullptr;
    m_BytesPerEntry = BytesPerEntry;
    m_NumEntries = 0;
    m_NumEntriesAllocated = 0;
}



//-----------------------------------------------------------------------------
// Name: CArrayList destructor
// Desc: 
//-----------------------------------------------------------------------------
CArrayList::~CArrayList( void )
{
    if( m_pData != nullptr )
        delete[] m_pData;
}




//-----------------------------------------------------------------------------
// Name: CArrayList::Add
// Desc: Adds pEntry to the list.
//-----------------------------------------------------------------------------
HRESULT CArrayList::Add( void* pEntry )
{
    if( m_BytesPerEntry == 0 )
        return E_FAIL;
    if( m_pData == nullptr || m_NumEntries + 1 > m_NumEntriesAllocated )
    {
        void* pDataNew;
        UINT NumEntriesAllocatedNew;
        if( m_NumEntriesAllocated == 0 )
            NumEntriesAllocatedNew = 16;
        else
            NumEntriesAllocatedNew = m_NumEntriesAllocated * 2;
        pDataNew = new BYTE[NumEntriesAllocatedNew * m_BytesPerEntry];
        if( pDataNew == nullptr )
            return E_OUTOFMEMORY;
        if( m_pData != nullptr )
        {
            CopyMemory( pDataNew, m_pData, m_NumEntries * m_BytesPerEntry );
            delete[] m_pData;
        }
        m_pData = pDataNew;
        m_NumEntriesAllocated = NumEntriesAllocatedNew;
    }

    if( m_ArrayListType == AL_VALUE )
        CopyMemory( (BYTE*)m_pData + (m_NumEntries * m_BytesPerEntry), pEntry, m_BytesPerEntry );
    else
        *(((void**)m_pData) + m_NumEntries) = pEntry;
    m_NumEntries++;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CArrayList::Remove
// Desc: Remove the item at Entry in the list, and collapse the array. 
//-----------------------------------------------------------------------------
void CArrayList::Remove( UINT Entry )
{
    // Decrement count
    m_NumEntries--;

    // Find the entry address
    BYTE* pData = (BYTE*)m_pData + (Entry * m_BytesPerEntry);

    // Collapse the array
    MoveMemory( pData, pData + m_BytesPerEntry, ( m_NumEntries - Entry ) * m_BytesPerEntry );
}




//-----------------------------------------------------------------------------
// Name: CArrayList::GetPtr
// Desc: Returns a pointer to the Entry'th entry in the list.
//-----------------------------------------------------------------------------
void* CArrayList::GetPtr( UINT Entry )
{
    if( m_ArrayListType == AL_VALUE )
        return (BYTE*)m_pData + (Entry * m_BytesPerEntry);
    else
        return *(((void**)m_pData) + Entry);
}




//-----------------------------------------------------------------------------
// Name: CArrayList::Contains
// Desc: Returns whether the list contains an entry identical to the 
//       specified entry data.
//-----------------------------------------------------------------------------
bool CArrayList::Contains( void* pEntryData )
{
    for( UINT iEntry = 0; iEntry < m_NumEntries; iEntry++ )
    {
        if( m_ArrayListType == AL_VALUE )
        {
            if( memcmp( GetPtr(iEntry), pEntryData, m_BytesPerEntry ) == 0 )
                return true;
        }
        else
        {
            if( GetPtr(iEntry) == pEntryData )
                return true;
        }
    }
    return false;
}




//-----------------------------------------------------------------------------
// Name: BYTE helper functions
// Desc: cchDestChar is the size in BYTEs of strDest.  Be careful not to 
//       pass use sizeof() if the strDest is a string pointer.  
//       eg.
//       TCHAR* sz = new TCHAR[100]; // sizeof(sz)  == 4
//       TCHAR sz2[100];             // sizeof(sz2) == 200
//-----------------------------------------------------------------------------
HRESULT DXUtil_ConvertAnsiStringToWideCb( WCHAR* wstrDestination, const CHAR* strSource, int cbDestChar )
{
    return DXUtil_ConvertAnsiStringToWideCch( wstrDestination, strSource, cbDestChar / sizeof(WCHAR) );
}

HRESULT DXUtil_ConvertWideStringToAnsiCb( CHAR* strDestination, const WCHAR* wstrSource, int cbDestChar )
{
    return DXUtil_ConvertWideStringToAnsiCch( strDestination, wstrSource, cbDestChar / sizeof(CHAR) );
}

HRESULT DXUtil_ConvertGenericStringToAnsiCb( CHAR* strDestination, const TCHAR* tstrSource, int cbDestChar )
{
    return DXUtil_ConvertGenericStringToAnsiCch( strDestination, tstrSource, cbDestChar / sizeof(CHAR) );
}

HRESULT DXUtil_ConvertGenericStringToWideCb( WCHAR* wstrDestination, const TCHAR* tstrSource, int cbDestChar )
{
    return DXUtil_ConvertGenericStringToWideCch( wstrDestination, tstrSource, cbDestChar / sizeof(WCHAR) );
}

HRESULT DXUtil_ConvertAnsiStringToGenericCb( TCHAR* tstrDestination, const CHAR* strSource, int cbDestChar )
{
    return DXUtil_ConvertAnsiStringToGenericCch( tstrDestination, strSource, cbDestChar / sizeof(TCHAR) );
}

HRESULT DXUtil_ConvertWideStringToGenericCb( TCHAR* tstrDestination, const WCHAR* wstrSource, int cbDestChar )
{
    return DXUtil_ConvertWideStringToGenericCch( tstrDestination, wstrSource, cbDestChar / sizeof(TCHAR) );
}

HRESULT DXUtil_ReadStringRegKeyCb( HKEY hKey, LPCTSTR strRegName, TCHAR* strDest, DWORD cbDest, LPCTSTR strDefault )
{
    return DXUtil_ReadStringRegKeyCch( hKey, strRegName, strDest, cbDest / sizeof(TCHAR), strDefault );
}

HRESULT DXUtil_ConvertGUIDToStringCb( const GUID* pGuidSrc, TCHAR* strDest, int cbDestChar )
{
    return DXUtil_ConvertGUIDToStringCch( pGuidSrc, strDest, cbDestChar / sizeof(TCHAR) );
}

#ifndef UNDER_CE
HRESULT DXUtil_GetDXSDKMediaPathCb( TCHAR* szDest, int cbDest )
{
    return DXUtil_GetDXSDKMediaPathCch( szDest, cbDest / sizeof(TCHAR) );
}

HRESULT DXUtil_FindMediaFileCb( TCHAR* szDestPath, int cbDest, LPCTSTR strFilename )
{
    return DXUtil_FindMediaFileCch( szDestPath, cbDest / sizeof(TCHAR), strFilename );
}
#endif // !UNDER_CE

#pragma warning(pop)
