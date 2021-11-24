
//#define _WIN32_WINNT_WIN10                  0x0A00
#include <Windows.h>
/*
//#include <shellscalingapi.h>

typedef WINUSERAPI DPI_AWARENESS_CONTEXT WINAPI FNGetWindowDpiAwarenessContext(HWND);
typedef FNGetWindowDpiAwarenessContext * pFNGetWindowDpiAwarenessContext;


typedef WINUSERAPI HRESULT WINAPI FNSetProcessDpiAwareness(PROCESS_DPI_AWARENESS value);
typedef FNSetProcessDpiAwareness * pFNSetProcessDpiAwareness;


typedef WINUSERAPI DPI_AWARENESS_CONTEXT WINAPI FNSetThreadDpiAwarenessContext(
   DPI_AWARENESS_CONTEXT dpiContext
   );

typedef FNSetThreadDpiAwarenessContext * pFNSetThreadDpiAwarenessContext;

typedef WINUSERAPI  BOOL WINAPI FNAreDpiAwarenessContextsEqual(
   DPI_AWARENESS_CONTEXT dpiContextA,
   DPI_AWARENESS_CONTEXT dpiContextB
   );

typedef FNAreDpiAwarenessContextsEqual * pFNAreDpiAwarenessContextsEqual;

typedef WINUSERAPI DPI_AWARENESS_CONTEXT WINAPI FNGetWindowDpiAwarenessContext(HWND);
typedef FNGetWindowDpiAwarenessContext * pFNGetWindowDpiAwarenessContext;
*/
typedef WINUSERAPI UINT WINAPI FNGetDpiForWindow(HWND);
typedef FNGetDpiForWindow * pFNGetDpiForWindow;
pFNGetDpiForWindow pfnGetDpiForWindow = NULL;
/*
bool dpi_awareness_is_per_monitor_v2_window(HWND hwnd)
{

   HMODULE hmodule = GetModuleHandle("user32.dll");

   if(hmodule == NULL)
   {

      return false;

   }


   pFNGetWindowDpiAwarenessContext pfn = (pFNGetWindowDpiAwarenessContext)
      GetProcAddress(hmodule,"GetWindowDpiAwarenessContext");

   if(pfn == NULL)
   {

      return false;

   }
   pFNAreDpiAwarenessContextsEqual pfn2 = (pFNAreDpiAwarenessContextsEqual)
      GetProcAddress(hmodule,"AreDpiAwarenessContextsEqual");

   if(pfn == NULL)
   {

      return false;

   }

   DPI_AWARENESS_CONTEXT context = pfn(hwnd);

   return pfn2(context, DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);



}


bool dpi_awareness_is_per_monitor_window(HWND hwnd)
{

   HMODULE hmodule = GetModuleHandle("user32.dll");

   if(hmodule == NULL)
   {

      return false;

   }
   pFNGetWindowDpiAwarenessContext pfn = (pFNGetWindowDpiAwarenessContext)
      GetProcAddress(hmodule,"GetWindowDpiAwarenessContext");

   if(pfn == NULL)
   {

      return false;

   }
   pFNAreDpiAwarenessContextsEqual pfn2 = (pFNAreDpiAwarenessContextsEqual)
      GetProcAddress(hmodule,"AreDpiAwarenessContextsEqual");

   if(pfn == NULL)
   {

      return false;

   }

   DPI_AWARENESS_CONTEXT context = pfn(hwnd);

   return pfn2(context,DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);



}

bool process_set_dpi_awareness_per_monitor_v2()
{

   HMODULE hmodule = GetModuleHandle("Shcore.dll");

   if(hmodule == NULL)
   {

      return false;

   }
   pfnGetDpiForWindow =(pFNGetDpiForWindow)

      GetProcAddress(GetModuleHandle("user32.dll"),"GetDpiForWindow");

   pFNSetProcessDpiAwareness pfn = (pFNSetProcessDpiAwareness)
      GetProcAddress(hmodule,"SetProcessDpiAwareness");

   if(pfn == NULL)
   {

      return false;

   }
   HRESULT hr = pfn(PROCESS_PER_MONITOR_DPI_AWARE);

   return SUCCEEDED(hr);
   

}

bool thread_set_dpi_awareness_per_monitor_v2()
{

   HMODULE hmodule = GetModuleHandle("user32.dll");

   if(hmodule == NULL)
   {

      return false;

   }
   pFNSetThreadDpiAwarenessContext pfn = (pFNSetThreadDpiAwarenessContext)
      GetProcAddress(hmodule,"SetThreadDpiAwarenessContext");

   if(pfn == NULL)
   {

      return false;

   }

   pFNAreDpiAwarenessContextsEqual pfn2 = (pFNAreDpiAwarenessContextsEqual)
      GetProcAddress(hmodule,"AreDpiAwarenessContextsEqual");

   if(pfn == NULL)
   {

      return false;

   }

   DPI_AWARENESS_CONTEXT context = pfn(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
   return pfn2(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2, context);
}*/


UINT window_dpi(HWND hwnd)
{
   if(pfnGetDpiForWindow == NULL)
   {
      auto dc = GetDC(nullptr);
      auto x = GetDeviceCaps(dc,LOGPIXELSX);
      ReleaseDC(nullptr,dc);
      return x;
   }
   return pfnGetDpiForWindow(hwnd);
}