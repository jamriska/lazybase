// This software is in the public domain. Where that dedication is not
// recognized, you are granted a perpetual, irrevocable license to copy
// and modify this file as you see fit.

#ifndef LAZY_PLATFORM_H_
#define LAZY_PLATFORM_H_

#include <cstdio>
#include <string>

#define LP_BUTTON_LEFT    0
#define LP_BUTTON_RIGHT   1
#define LP_BUTTON_MIDDLE  2

enum LpKey
{
  LP_KEY_ESCAPE = 0,
  LP_KEY_TAB,
  LP_KEY_BACKSPACE,
  LP_KEY_ENTER,
  LP_KEY_INSERT,
  LP_KEY_DELETE,
  LP_KEY_PAUSE,
  LP_KEY_PRINT,
  LP_KEY_SYSREQ,
  LP_KEY_CLEAR,
  LP_KEY_HOME,
  LP_KEY_END,
  LP_KEY_LEFT,
  LP_KEY_UP,
  LP_KEY_RIGHT,
  LP_KEY_DOWN,
  LP_KEY_PAGEUP,
  LP_KEY_PAGEDOWN,
  LP_KEY_SHIFT,
  LP_KEY_CONTROL,
  LP_KEY_META,
  LP_KEY_ALT,
  LP_KEY_ALTGR,
  LP_KEY_CAPSLOCK,
  LP_KEY_NUMLOCK,
  LP_KEY_SCROLLLOCK,
  LP_KEY_F1,
  LP_KEY_F2,
  LP_KEY_F3,
  LP_KEY_F4,
  LP_KEY_F5,
  LP_KEY_F6,
  LP_KEY_F7,
  LP_KEY_F8,
  LP_KEY_F9,
  LP_KEY_F10,
  LP_KEY_F11,
  LP_KEY_F12,
  LP_KEY_SPACE,
  LP_KEY_EXCLAM,
  LP_KEY_QUOTEDBL,
  LP_KEY_NUMBERSIGN,
  LP_KEY_DOLLAR,
  LP_KEY_PERCENT,
  LP_KEY_AMPERSAND,
  LP_KEY_APOSTROPHE,
  LP_KEY_PARENLEFT,
  LP_KEY_PARENRIGHT,
  LP_KEY_ASTERISK,
  LP_KEY_PLUS,
  LP_KEY_COMMA,
  LP_KEY_MINUS,
  LP_KEY_PERIOD,
  LP_KEY_SLASH,
  LP_KEY_0,
  LP_KEY_1,
  LP_KEY_2,
  LP_KEY_3,
  LP_KEY_4,
  LP_KEY_5,
  LP_KEY_6,
  LP_KEY_7,
  LP_KEY_8,
  LP_KEY_9,
  LP_KEY_COLON,
  LP_KEY_SEMICOLON,
  LP_KEY_LESS,
  LP_KEY_EQUAL,
  LP_KEY_GREATER,
  LP_KEY_QUESTION,
  LP_KEY_AT,
  LP_KEY_A,
  LP_KEY_B,
  LP_KEY_C,
  LP_KEY_D,
  LP_KEY_E,
  LP_KEY_F,
  LP_KEY_G,
  LP_KEY_H,
  LP_KEY_I,
  LP_KEY_J,
  LP_KEY_K,
  LP_KEY_L,
  LP_KEY_M,
  LP_KEY_N,
  LP_KEY_O,
  LP_KEY_P,
  LP_KEY_Q,
  LP_KEY_R,
  LP_KEY_S,
  LP_KEY_T,
  LP_KEY_U,
  LP_KEY_V,
  LP_KEY_W,
  LP_KEY_X,
  LP_KEY_Y,
  LP_KEY_Z,
  LP_KEY_BRACKETLEFT,
  LP_KEY_BACKSLASH,
  LP_KEY_BRACKETRIGHT,
  LP_KEY_ASCIICIRCUM,
  LP_KEY_UNDERSCORE,
  LP_KEY_QUOTELEFT,
  LP_KEY_BRACELEFT,
  LP_KEY_BAR,
  LP_KEY_BRACERIGHT,
  LP_KEY_ASCIITILD,
  LP_KEY_KP_0,
  LP_KEY_KP_1,
  LP_KEY_KP_2,
  LP_KEY_KP_3,
  LP_KEY_KP_4,
  LP_KEY_KP_5,
  LP_KEY_KP_6,
  LP_KEY_KP_7,
  LP_KEY_KP_8,
  LP_KEY_KP_9,
  LP_KEY_KP_ADD,
  LP_KEY_KP_SUBTRACT
};

void lpInit(void* parentWindow,int width,int height);
void lpUpdate();
void lpCleanUp();

void lpBlit(int width,int height,unsigned char* data);

void lpWaitForEvent();
bool lpEventsPending();

void lpBeginModal();
void lpEndModal();

bool lpWindowCloseRequested();
bool lpWindowResized();

int lpWindowWidth();
int lpWindowHeight();

void lpSetWindowSize(int width,int height,int anchor);
void lpSetWindowTitle(const std::string& text);

bool lpKeyWentDown(int key);
bool lpKeyWentDownRepeat(int key);
bool lpKeyIsDown(int key);
bool lpKeyWentUp(int key);
std::string lpKeyChars();

int lpMouseX();
int lpMouseY();

bool lpMouseButtonWentDown(int button);
bool lpMouseButtonIsDown(int button);
bool lpMouseButtonWentUp(int button);
bool lpMouseButtonDoubleClicked(int button);

int lpMouseWheelDelta();

typedef void* LPCursor;

void lpSetCursor(LPCursor cursor);
LPCursor lpMakeCursor(const void* data,int width,int height,int hotspotX,int hotspotY);
LPCursor lpArrowCursor();
LPCursor lpHandCursor();

int lpGetTime();

// XXX this should work from any thread
void lpWakeUp();

// XXX this works from main thread ONLY !!!!
void lpWakeUpAfter(int time);

std::string lpFileOpenDialog(const std::string& initialDir);
std::string lpFileSaveDialog();
std::string lpDirSelectDialog();

void lpMessageBox(const std::string& text);

void lpSetRepaintCallback(void (*callback)());

std::string lpClipBoardText();

std::string lpFileOpenRequested();

void lpOpenURL(const std::string& url);

bool lpFileExists(const std::string& path);
bool lpDirExists(const std::string& path);
void lpMakeDir(const std::string& path);
std::string lpAppDataDir(const std::string& appName);

std::string lpMakeTempFile();
bool lpDeleteFile(const std::string& fileName);

FILE* lp_fopen(const std::string& filename,const std::string& mode);

#endif
