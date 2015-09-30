// This software is in the public domain. Where that dedication is not
// recognized, you are granted a perpetual, irrevocable license to copy
// and modify this file as you see fit.

#define UNICODE
#define _UNICODE

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#include <windows.h>
#include <commdlg.h>
#include <shellapi.h>
#include <shlobj.h>

#include <cstdlib>
#include <cstdio>

#include <string>

#include "lp.h"

#define LP_WNDCLASSNAME L"Lazy Window Class"

HMODULE module = NULL;
ATOM classAtom = NULL;
HWND window = NULL;
HWND parentWindow = NULL;
DWORD dwStyle = 0;

bool windowCloseRequested = false;

bool windowResized = false;

int windowWidth = 0;
int windowHeight = 0;

int mouseX = 0;
int mouseY = 0;

bool buttonState[3] = { false };
bool lastButtonState[3] = { false };
bool buttonDoubleClicked[3] = { false };

bool keyState[256] = { false };
bool lastKeyState[256] = { false };
bool keyDownRepeat[256] = { false };
std::string keyChars;

int wheelDelta = 0;

std::string fileOpenRequested;

const int vk_to_lpk[256] = {
-1,
-1, // VK_LBUTTON
-1, // VK_RBUTTON
-1, // VK_CANCEL
-1, // VK_MBUTTON
-1,
-1,
-1,
LP_KEY_BACKSPACE, // VK_BACK
LP_KEY_TAB, // VK_TAB
-1,
-1,
-1, // VK_CLEAR
LP_KEY_ENTER, // VK_RETURN
-1,
-1,
LP_KEY_SHIFT, // VK_SHIFT
LP_KEY_CONTROL, // VK_CONTROL
LP_KEY_ALT, // VK_MENU
-1, // VK_PAUSE
-1, // VK_CAPITAL
-1,
-1,
-1,
-1,
-1,
-1,
LP_KEY_ESCAPE, // VK_ESCAPE
-1,
-1,
-1,
-1,
LP_KEY_SPACE, // VK_SPACE
-1, // VK_PRIOR
-1, // VK_NEXT
LP_KEY_END, // VK_END
LP_KEY_HOME, // VK_HOME
LP_KEY_LEFT, // VK_LEFT
LP_KEY_UP, // VK_UP
LP_KEY_RIGHT, // VK_RIGHT
LP_KEY_DOWN, // VK_DOWN
-1, // VK_SELECT
-1, // VK_PRINT
-1, // VK_EXECUTE
-1, // VK_SNAPSHOT
LP_KEY_INSERT, // VK_INSERT
LP_KEY_DELETE, // VK_DELETE
-1, // VK_HELP
LP_KEY_0, // 0
LP_KEY_1, // 1
LP_KEY_2, // 2
LP_KEY_3, // 3
LP_KEY_4, // 4
LP_KEY_5, // 5
LP_KEY_6, // 6
LP_KEY_7, // 7
LP_KEY_8, // 8
LP_KEY_9, // 9
-1,
-1,
-1,
-1,
-1,
-1,
-1,
LP_KEY_A, // A
LP_KEY_B, // B
LP_KEY_C, // C
LP_KEY_D, // D
LP_KEY_E, // E
LP_KEY_F, // F
LP_KEY_G, // G
LP_KEY_H, // H
LP_KEY_I, // I
LP_KEY_J, // J
LP_KEY_K, // K
LP_KEY_L, // L
LP_KEY_M, // M
LP_KEY_N, // N
LP_KEY_O, // O
LP_KEY_P, // P
LP_KEY_Q, // Q
LP_KEY_R, // R
LP_KEY_S, // S
LP_KEY_T, // T
LP_KEY_U, // U
LP_KEY_V, // V
LP_KEY_W, // W
LP_KEY_X, // X
LP_KEY_Y, // Y
LP_KEY_Z, // Z
-1,
-1,
-1,
-1,
-1,
LP_KEY_0, // VK_NUMPAD0
LP_KEY_1, // VK_NUMPAD1
LP_KEY_2, // VK_NUMPAD2
LP_KEY_3, // VK_NUMPAD3
LP_KEY_4, // VK_NUMPAD4
LP_KEY_5, // VK_NUMPAD5
LP_KEY_6, // VK_NUMPAD6
LP_KEY_7, // VK_NUMPAD7
LP_KEY_8, // VK_NUMPAD8
LP_KEY_9, // VK_NUMPAD9
-1,
LP_KEY_PLUS, // VK_ADD
-1, // VK_SEPARATOR
LP_KEY_MINUS, // VK_SUBTRACT
-1, // VK_DECIMAL
-1, // VK_DIVIDE
LP_KEY_F1, // VK_F1
LP_KEY_F2, // VK_F2
LP_KEY_F3, // VK_F3
LP_KEY_F4, // VK_F4
LP_KEY_F5, // VK_F5
LP_KEY_F6, // VK_F6
LP_KEY_F7, // VK_F7
LP_KEY_F8, // VK_F8
LP_KEY_F9, // VK_F9
LP_KEY_F10, // VK_F10
LP_KEY_F11, // VK_F11
LP_KEY_F12, // VK_F12
-1, // VK_F13
-1, // VK_F14
-1, // VK_F15
-1, // VK_F16
-1, // VK_F17
-1, // VK_F18
-1, // VK_F19
-1, // VK_F20
-1, // VK_F21
-1, // VK_F22
-1, // VK_F23
-1, // VK_F24
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1, // VK_NUMLOCK
-1, // VK_SCROLL
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1, // VK_LSHIFT
-1, // VK_RSHIFT
-1, // VK_LCONTROL
-1, // VK_RCONTROL
-1, // VK_LMENU
-1, // VK_RMENU
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
LP_KEY_PLUS, // VK_OEM_PLUS
-1,
LP_KEY_MINUS, // VK_OEM_MINUS
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
LP_KEY_BRACKETLEFT, // VK_OEM_4
-1,
LP_KEY_BRACKETRIGHT, // VK_OEM_6
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1,
-1, // VK_PLAY
-1, // VK_ZOOM
-1,
-1,
-1,
-1,
};

void (*repaintCallback)() = 0;

std::wstring utf8_to_wide(const std::string& str)
{    
  // size is the number of output characters including nul
  const int size = MultiByteToWideChar(CP_UTF8,0,str.c_str(),-1,NULL,0); 
  std::wstring wide_str(size,0);
  MultiByteToWideChar(CP_UTF8,0,str.c_str(),-1,&wide_str[0],size);
  wide_str.resize(size-1);
  return wide_str;
}

std::string wide_to_utf8(const std::wstring& wide_str)
{
  // size is the number of output characters including nul
  const int size = WideCharToMultiByte(CP_UTF8,0,wide_str.c_str(),-1,NULL,0,NULL,NULL);
  std::string str(size,0);
  WideCharToMultiByte(CP_UTF8,0,wide_str.c_str(),-1,&str[0],size,NULL,NULL);
  str.resize(size-1);
  return str;
}

HMODULE GetCurrentModule()
{
  MEMORY_BASIC_INFORMATION mbi;
  static int dummy;
  VirtualQuery(&dummy,&mbi,sizeof(mbi));
 
  return (HMODULE)mbi.AllocationBase;
}

void lpSetRepaintCallback(void (*callback)())
{
  repaintCallback = callback;
}

static LRESULT CALLBACK windowProc(HWND wnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
  if (msg==WM_CLOSE)
  {
    windowCloseRequested = true;
    return 0;
  }
  if (msg==WM_TIMER)
  {
    KillTimer(wnd,wParam);
    return 0;
  }
  if (msg==WM_PAINT)
  {   
    if (repaintCallback!=0) { repaintCallback(); } 
  }
  else if (msg==WM_SIZE)
  {
    windowWidth  = LOWORD(lParam);
    windowHeight = HIWORD(lParam);
    windowResized = true;
  }
  else if (msg==WM_MOUSEMOVE)
  {
    //mouseX = (int)((short)LOWORD(lParam));
    //mouseY = (int)((short)HIWORD(lParam));
    return 0;  
  }
  else if (msg==WM_LBUTTONDOWN || msg==WM_LBUTTONDBLCLK)
  {
    buttonState[LP_BUTTON_LEFT] = true;
    if(msg==WM_LBUTTONDBLCLK) { buttonDoubleClicked[LP_BUTTON_LEFT] = true; }

    SetCapture(wnd);    
    return 0;
  }
  else if (msg==WM_RBUTTONDOWN || msg==WM_RBUTTONDBLCLK)
  {
    buttonState[LP_BUTTON_RIGHT] = true;
    if(msg==WM_RBUTTONDBLCLK) { buttonDoubleClicked[LP_BUTTON_RIGHT] = true; }

    SetCapture(wnd);
    return 0;
  }
  else if (msg==WM_MBUTTONDOWN || msg==WM_MBUTTONDBLCLK)
  {
    buttonState[LP_BUTTON_MIDDLE] = true;
    if(msg==WM_MBUTTONDBLCLK) { buttonDoubleClicked[LP_BUTTON_MIDDLE] = true; }

    SetCapture(wnd);
    return 0;
  }
  else if (msg==WM_LBUTTONUP)
  {
    buttonState[LP_BUTTON_LEFT] = false;

    ReleaseCapture();
    return 0;
  } 
  else if (msg==WM_RBUTTONUP)
  {
    buttonState[LP_BUTTON_RIGHT] = false;

    ReleaseCapture();
    return 0;
  } 
  else if (msg==WM_MBUTTONUP)
  {
    buttonState[LP_BUTTON_MIDDLE] = false;

    ReleaseCapture();
    return 0;
  } 
  else if (msg==WM_KEYDOWN || msg==WM_SYSKEYDOWN)
  {
    int key = wParam < 256 ? vk_to_lpk[wParam] : -1;
    if (key!=-1)
    {
      keyState[key] = true; 
      keyDownRepeat[key] = true;
    }
  }
  else if (msg==WM_KEYUP || msg==WM_SYSKEYUP)
  {
    int key = wParam < 256 ? vk_to_lpk[wParam] : -1;
    if (key!=-1) { keyState[key] = false; }
  }
  else if (msg==WM_CHAR)
  {
    if (wParam!=0x08 &&  // backspace 
        wParam!=0x0A &&  // linefeed 
        wParam!=0x1B &&  // escape 
        wParam!=0x09 &&  // tab // XXX: send four spaces ? 
        wParam!=0x0D)    // carriage return
    { 
      // ignore non-ascii chars XXX: unicode!
      //if (wParam>=32 && wParam<=127) { keyChar = wParam; }
      keyChars = wide_to_utf8(std::wstring(1,wParam));
    }
  }
  else if (msg==WM_MOUSEWHEEL)
  {
    wheelDelta = (((int)wParam) >> 16);
    return 0;
  }
  else if (msg==WM_SYSCOMMAND)
  {
    if (((wParam & 0xfff0) == SC_KEYMENU) && (lParam>>16)<=0) return 0;
  }
  else if (msg==WM_KILLFOCUS)
  {
    for(int key=0;key<256;key++) { keyState[key] = false; }
  }
  else if (msg==WM_DROPFILES)
  {
    // XXX: what to do if there are multiple files in the HDROP structure

    HDROP hDrop = (HDROP)wParam;
    
    const int wide_fileLength = DragQueryFileW(hDrop,0,NULL,0)+1;
    std::wstring wide_file(wide_fileLength,0);
    DragQueryFileW(hDrop,0,&wide_file[0],wide_fileLength);
    wide_file.resize(wide_fileLength-1);
    
    fileOpenRequested = wide_to_utf8(wide_file);

    SetForegroundWindow(window);

    return 0;
  }

  return DefWindowProc(wnd,msg,wParam,lParam);
}

static ATOM registerWindowClass()
{
  WNDCLASS wc;
  ZeroMemory(&wc,sizeof(wc));

  wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
  wc.lpfnWndProc   = (WNDPROC)windowProc;
  wc.hInstance     = module;
  wc.hCursor       = LoadCursor(NULL,IDC_ARROW);
  wc.lpszClassName = LP_WNDCLASSNAME;
  wc.hIcon         = LoadIconW(module,L"IDI_MAIN_ICON");
  return RegisterClassW(&wc);
}

void lpInit(void* _parentWindow,int width,int height)
{
  module = GetCurrentModule();
  
  parentWindow = (HWND)_parentWindow;

  fileOpenRequested.clear();

  classAtom = registerWindowClass();

  dwStyle = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE;
  dwStyle |= WS_OVERLAPPED | WS_CAPTION  | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX;
  
  DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;

  RECT rect = {0,0,windowWidth,windowHeight};
  AdjustWindowRect(&rect,dwStyle,FALSE);

  window = CreateWindowExW(dwExStyle,            // Extended style
                           LP_WNDCLASSNAME,      // Class name
                           L"",                  // Window title
                           dwStyle,              // Defined window style
                           CW_USEDEFAULT,        // Window position
                           CW_USEDEFAULT,        // Window position
                           rect.right-rect.left, // Decorated window width
                           rect.bottom-rect.top, // Decorated window height
                           parentWindow,    
                           NULL,                 // No menu
                           module,               // XXX !
                           NULL);                // Nothing to WM_CREATE

  DragAcceptFiles(window,TRUE);
  
  BringWindowToTop(window);
  SetForegroundWindow(window);
}

void lpBeginModal()
{
  if (parentWindow!=NULL) { EnableWindow(parentWindow,FALSE); }
}

void lpEndModal()
{ 
  if (parentWindow!=NULL) { EnableWindow(parentWindow,TRUE); }
}

bool lpEventsPending()
{
  MSG message;
    
  if (PeekMessageW(&message,NULL,0,0,PM_NOREMOVE))
  {
    return true;
  }
 
  return false;
}

void lpWaitForEvent()
{
  WaitMessage();
}

void lpUpdate()
{
  fileOpenRequested.clear();

  windowCloseRequested = false;
  windowResized = false;

  for(int button=0;button<3;button++)
  {
    lastButtonState[button] = buttonState[button]; 
    buttonDoubleClicked[button] = false;
  }
  
  for(int key=0;key<256;key++) { lastKeyState[key] = keyState[key]; } 
  for(int key=0;key<256;key++) { keyDownRepeat[key] = false; }
  keyChars.clear();

  wheelDelta = 0;

  MSG message;
    
  while(PeekMessageW(&message,NULL,0,0,PM_REMOVE))
  {
    TranslateMessage(&message);
    DispatchMessageW(&message);
    
    UINT msg = message.message;

    if (msg == WM_LBUTTONDOWN   ||
        msg == WM_RBUTTONDOWN   ||
        msg == WM_MBUTTONDOWN   ||
        msg == WM_LBUTTONDBLCLK ||
        msg == WM_RBUTTONDBLCLK ||
        msg == WM_MBUTTONDBLCLK ||
        msg == WM_LBUTTONUP     ||
        msg == WM_RBUTTONUP     ||
        msg == WM_MBUTTONUP     ||
        msg == WM_KEYDOWN       || 
        msg == WM_KEYUP         ||
        msg == WM_SYSKEYUP      ||
        msg == WM_SYSKEYDOWN    ||
        msg == WM_CHAR)
    {
      break;
    }    
  }

  {
    CURSORINFO cursorInfo;
    cursorInfo.cbSize = sizeof(CURSORINFO);

    GetCursorInfo(&cursorInfo);
    ScreenToClient(window,&cursorInfo.ptScreenPos);

    mouseX = cursorInfo.ptScreenPos.x;
    mouseY = cursorInfo.ptScreenPos.y;  
  }
}

void lpCleanUp()
{
  if (window!=NULL)
  {
    DestroyWindow(window);
    window = NULL;
  }

  if(classAtom!=0)
  {
    UnregisterClassW(LP_WNDCLASSNAME,module);
    classAtom = 0;
  }   

  fileOpenRequested.clear();
}

bool lpWindowCloseRequested()
{
  return windowCloseRequested;
}

bool lpWindowResized()
{
  return windowResized;
}

int lpWindowWidth()
{
  return windowWidth;
}

int lpWindowHeight()
{
  return windowHeight;
}

void lpSetWindowSize(int width,int height,int anchor)
{
  RECT rect = {0,0,width,height};
  AdjustWindowRect(&rect,dwStyle,FALSE);

  const int newWidth  = rect.right-rect.left;
  const int newHeight = rect.bottom-rect.top;

  RECT windowRect;
  GetWindowRect(window,&windowRect);
  const int oldWidth  = windowRect.right-windowRect.left;
  const int oldHeight = windowRect.bottom-windowRect.top;

  if (anchor==1 && newHeight>oldHeight && (windowRect.top-(newHeight-oldHeight))<0) { anchor = 0; }
  if (anchor==2 && newHeight>oldHeight && (windowRect.top-(newHeight-oldHeight)/2)<0) { anchor = 0; }
  
  if (anchor==0) { SetWindowPos(window,HWND_TOP,windowRect.left,windowRect.top,newWidth,newHeight,SWP_SHOWWINDOW); }
  if (anchor==1) { SetWindowPos(window,HWND_TOP,windowRect.left,windowRect.top-(newHeight-oldHeight),newWidth,newHeight,SWP_SHOWWINDOW); }
  if (anchor==2) { SetWindowPos(window,HWND_TOP,windowRect.left,windowRect.top-(newHeight-oldHeight)/2,newWidth,newHeight,SWP_SHOWWINDOW); }
}

void lpSetWindowTitle(const std::string& text)
{
  SetWindowTextW(window,utf8_to_wide(text).c_str());
}

bool lpKeyWentDown(int key)
{
  return (keyState[key]==true && lastKeyState[key]==false);
}

bool lpKeyWentDownRepeat(int key)
{
  return keyDownRepeat[key];
}

bool lpKeyIsDown(int key)
{
  return keyState[key];
}

bool lpKeyWentUp(int key)
{
  return (keyState[key]==false && lastKeyState[key]==true);
}

std::string lpKeyChars()
{
  return keyChars;
}

int lpMouseX()
{
  return mouseX;
}

int lpMouseY()
{
  return mouseY;
}

bool lpMouseButtonWentDown(int button)
{
  return (buttonState[button]==true && lastButtonState[button]==false);
}

bool lpMouseButtonIsDown(int button)
{
  return buttonState[button];
}

bool lpMouseButtonWentUp(int button)
{
  return (buttonState[button]==false && lastButtonState[button]==true);
}

bool lpMouseButtonDoubleClicked(int button)
{
  return buttonDoubleClicked[button];
}

int lpMouseWheelDelta()
{
  return wheelDelta;
}

void lpBlit(int width,int height,unsigned char* data)
{
  BITMAPINFOHEADER bitmapInfo;  
  
  ZeroMemory(&bitmapInfo,sizeof(BITMAPINFOHEADER));  
  
  bitmapInfo.biSize = sizeof(BITMAPINFOHEADER);     
  bitmapInfo.biWidth = width;
  bitmapInfo.biHeight = -height;
  bitmapInfo.biPlanes = 1;
  bitmapInfo.biBitCount = 32;
  bitmapInfo.biCompression = BI_RGB;  
  bitmapInfo.biSizeImage = (bitmapInfo.biWidth*bitmapInfo.biHeight*bitmapInfo.biBitCount)/8;

  HDC dc = GetDC(window);
  
  SetDIBitsToDevice(dc,0,0,width,height,0,0,0,height,data,(LPBITMAPINFO)&bitmapInfo,DIB_RGB_COLORS);

  ReleaseDC(window,dc);            
}

void lpSetCursor(LPCursor cursor)
{
  SetCursor((HCURSOR)cursor);
  SetClassLongPtr(window,GCLP_HCURSOR,(LONG)cursor);
}

LPCursor lpMakeCursor(const void *data,int width,int height,int hotspotX,int hotspotY)
{
  unsigned int *bmp = (unsigned int*)data;

  HDC hDC        = GetDC(NULL);
  HDC hMainDC    = CreateCompatibleDC(hDC);
  HDC hAndMaskDC = CreateCompatibleDC(hDC); 
  HDC hXorMaskDC = CreateCompatibleDC(hDC); 

  HBITMAP hAndMaskBitmap = CreateCompatibleBitmap(hDC,width,height);
  HBITMAP hXorMaskBitmap = CreateCompatibleBitmap(hDC,width,height);

  HBITMAP hOldAndMaskBitmap = (HBITMAP)SelectObject(hAndMaskDC,hAndMaskBitmap);
  HBITMAP hOldXorMaskBitmap = (HBITMAP)SelectObject(hXorMaskDC,hXorMaskBitmap);

  for (int y=0;y<height;y++)
  for (int x=0;x<width;x++)
  {
    unsigned int color = bmp[x+y*width];
    unsigned char a = (color & 0xff000000) >> 24;
      
    if (a == 0)
    {
      SetPixel(hAndMaskDC,x,y,RGB(255,255,255));
      SetPixel(hXorMaskDC,x,y,RGB(0,0,0));
    }
    else
    {
      unsigned char r = (color & 0x00ff0000) >> 16;
      unsigned char g = (color & 0x0000ff00) >> 8;
      unsigned char b = (color & 0x00ff00ff);

      SetPixel(hAndMaskDC,x,y,RGB(0,0,0));
      SetPixel(hXorMaskDC,x,y,RGB(r,g,b));
    }
  }

  SelectObject(hAndMaskDC,hOldAndMaskBitmap);
  SelectObject(hXorMaskDC,hOldXorMaskBitmap);
  
  DeleteDC(hAndMaskDC);
  DeleteDC(hXorMaskDC);

  ReleaseDC(NULL,hDC);

  ICONINFO iconinfo = {0};
  iconinfo.fIcon    = FALSE;
  iconinfo.xHotspot = (DWORD)hotspotX;
  iconinfo.yHotspot = (DWORD)hotspotY;
  iconinfo.hbmMask  = hAndMaskBitmap;
  iconinfo.hbmColor = hXorMaskBitmap;

  HCURSOR hCursor = CreateIconIndirect(&iconinfo);
  
  return hCursor;
}

LPCursor lpArrowCursor()
{
  return LoadCursor(NULL,IDC_ARROW);
}

LPCursor lpHandCursor()
{
  return LoadCursor(NULL,IDC_HAND);
}

int lpGetTime()
{
  //XXX overflow (rollback) !!!
  return GetTickCount();
}

// XXX this should work from any thread
void lpWakeUp()
{
  PostMessageW(window,(WM_USER+0),0,0);
}

// XXX this works from main thread ONLY !!!!
void lpWakeUpAfter(int time)
{
  SetTimer(NULL,NULL,time,NULL);
}

std::string lpFileOpenDialog(const std::string& initialDir)
{
  OPENFILENAME ofn;
  
  const int fileBufferMaxChars = 4096;
  const size_t fileBufferNumBytes = (fileBufferMaxChars+1)*sizeof(wchar_t);
  wchar_t* fileBuffer = (wchar_t*)malloc(fileBufferNumBytes);
  memset(fileBuffer,0,fileBufferNumBytes);
  
  const std::wstring wide_initialDir = utf8_to_wide(initialDir);

  ZeroMemory(&ofn,sizeof(ofn));
  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = window;
  ofn.lpstrFile = fileBuffer;
  ofn.nMaxFile = fileBufferMaxChars;
  ofn.lpstrFilter = L"BJF Files (*.bjf)\0*.bjf\0\0";
  ofn.nFilterIndex = 0;
  ofn.lpstrFileTitle = NULL;
  ofn.nMaxFileTitle = 0;
  ofn.lpstrInitialDir = wide_initialDir.empty() ? NULL : wide_initialDir.c_str();
  ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
 
  std::string fileName; 
  
  if (GetOpenFileNameW(&ofn)==TRUE)
  {
    fileName = wide_to_utf8(ofn.lpstrFile);
  }

  free(fileBuffer);
    
  return fileName;
}

std::string lpFileSaveDialog()
{
  OPENFILENAME ofn;
  
  const int fileBufferMaxChars = 4096;
  const size_t fileBufferNumBytes = (fileBufferMaxChars+1)*sizeof(wchar_t);
  wchar_t* fileBuffer = (wchar_t*)malloc(fileBufferNumBytes);
  memset(fileBuffer,0,fileBufferNumBytes);
  
  ZeroMemory(&ofn,sizeof(ofn));
  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = window;
  ofn.lpstrFile = fileBuffer;
  ofn.nMaxFile = fileBufferMaxChars;
  ofn.lpstrFilter = L"BJF Files (*.bjf)\0*.bjf\0\0";
  ofn.nFilterIndex = 0;
  ofn.lpstrFileTitle = NULL;
  ofn.nMaxFileTitle = 0;
  ofn.lpstrInitialDir = NULL;
  ofn.Flags = OFN_PATHMUSTEXIST;// | OFN_FILEMUSTEXIST;

  std::string fileName; 

  if (GetSaveFileNameW(&ofn)==TRUE)
  {
    fileName = wide_to_utf8(ofn.lpstrFile);
  }

  free(fileBuffer);
  
  return fileName.empty() ? fileName : (fileName+std::string(".bjf"));
}

std::string lpDirSelectDialog()
{
  std::string directory;

  IFileDialog* pfd;
  if (SUCCEEDED(CoCreateInstance(CLSID_FileOpenDialog,NULL,CLSCTX_ALL,IID_IFileOpenDialog,(void**)(&pfd))))
  {
    DWORD dwOptions;
    if (SUCCEEDED(pfd->GetOptions(&dwOptions)))
    {
      pfd->SetOptions(dwOptions | FOS_PICKFOLDERS);
    }

    if (SUCCEEDED(pfd->Show(NULL)))
    {
      IShellItem *psi;
      PWSTR wide_path;
      if (SUCCEEDED(pfd->GetResult(&psi)))
      {
        if (SUCCEEDED(psi->GetDisplayName(SIGDN_FILESYSPATH,&wide_path)))
        {
          directory = wide_to_utf8(wide_path);         
          CoTaskMemFree(wide_path);
        }
        psi->Release();          
      }
    }
    pfd->Release();
  }

  return directory;    
}

void lpMessageBox(const std::string& text)
{
  MessageBoxW(window,utf8_to_wide(text).c_str(),L"",MB_OK | MB_SYSTEMMODAL);
}

std::string lpClipBoardText()
{
  std::string text;

  if (OpenClipboard(NULL)) // XXX: window instead of NULL?
  {
    HANDLE hClipboardData = GetClipboardData(CF_UNICODETEXT);
    if (hClipboardData)
    {
      wchar_t* wide_text = (wchar_t*)GlobalLock(hClipboardData);
      if (wide_text)
      {
        text = wide_to_utf8(wide_text);
        GlobalUnlock(hClipboardData);
      }
    }
    CloseClipboard();
  }

  return text;      
}

std::string lpFileOpenRequested()
{
  return fileOpenRequested;
}

void lpOpenURL(const std::string& url)
{
  ShellExecuteW(NULL,L"open",utf8_to_wide(url).c_str(),NULL,NULL,SW_SHOWNORMAL);
}

bool lpFileExists(const std::string& path)
{
  DWORD attributes = GetFileAttributesW(utf8_to_wide(path).c_str());

  if (attributes!=INVALID_FILE_ATTRIBUTES &&
      !(attributes&FILE_ATTRIBUTE_DIRECTORY)) { return true;  }
   
  return false;
}

bool lpDirExists(const std::string& path)
{
  DWORD attributes = GetFileAttributesW(utf8_to_wide(path).c_str());

  if (attributes==INVALID_FILE_ATTRIBUTES) { return false; }
  if (attributes&FILE_ATTRIBUTE_DIRECTORY) { return true;  }
   
  return false;
}

void lpMakeDir(const std::string& path)
{
  CreateDirectoryW(utf8_to_wide(path).c_str(),NULL);
}

std::string lpAppDataDir(const std::string& appName)
{
  wchar_t wide_path[MAX_PATH] = {0};
  SHGetFolderPathW(NULL,CSIDL_APPDATA,0,NULL,wide_path);
  std::string dir = wide_to_utf8(wide_path)+"/"+appName;  
  if (!lpDirExists(dir)) { lpMakeDir(dir); }
  return dir;
}

std::string lpMakeTempFile()
{
  wchar_t wide_path[MAX_PATH] = {0};
  const int size = GetTempPathW(MAX_PATH,wide_path);
  if (size==0 || size>MAX_PATH) { return std::string(); }

  wchar_t wide_fileName[MAX_PATH] = {0};
  if (GetTempFileNameW(wide_path,L"LZY",0,wide_fileName)==0) { return std::string(); }
  
  return wide_to_utf8(wide_fileName);
}

bool lpDeleteFile(const std::string& fileName)
{
  return (DeleteFileW(utf8_to_wide(fileName).c_str())==TRUE);
}

FILE* lp_fopen(const std::string& filename,const std::string& mode)
{
  return _wfopen(utf8_to_wide(filename).c_str(),utf8_to_wide(mode).c_str());
}
