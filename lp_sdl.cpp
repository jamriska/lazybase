#include <SDL/SDL.h>

#include <cstdio>
#include <cstring>

#include "lp.h"

bool windowCloseRequested = false;

bool windowResized = false;

int windowWidth = 0;
int windowHeight = 0;

int mouseX = 0;
int mouseY = 0;

bool buttonState[3] = { false };
bool lastButtonState[3] = { false };

bool keyState[256] = { false };
bool lastKeyState[256] = { false };

int wheelDelta = 0;

void lpInit(void* _parentWindow)
{
  windowWidth = 1024;
  windowHeight = 576;
  SDL_Init(SDL_INIT_VIDEO);
  SDL_SetVideoMode(windowWidth,windowHeight,32,SDL_SWSURFACE); 
  
  for(int button=0;button<3;button++) { lastButtonState[button] = false; }  
  for(int key=0;key<256;key++) { lastKeyState[key] = false; }


}

void lpBeginModal()
{

}

void lpEndModal()
{ 

}

bool lpEventsPending()
{
  return (SDL_PollEvent(0)==1);
}

void lpWaitForEvent()
{
 // SDL_WaitEvent(0);
}

void lpUpdate()
{
  windowCloseRequested = false;
  windowResized = false;

  for(int button=0;button<3;button++) { lastButtonState[button] = buttonState[button]; }
  
  for(int key=0;key<256;key++) { lastKeyState[key] = keyState[key]; }

  wheelDelta = 0;


  SDL_Event event;
  while(SDL_PollEvent(&event))
  { 
      if      (event.type==SDL_MOUSEBUTTONDOWN)
      {
        buttonState[LP_BUTTON_LEFT] = true;
      }
      else if (event.type==SDL_MOUSEBUTTONUP)
      {
        buttonState[LP_BUTTON_LEFT] = false;
      }
      else if (event.type==SDL_QUIT)
      {
         windowCloseRequested = true;
         break;
      }
  }
  
  SDL_GetMouseState(&mouseX,&mouseY);

  /*  
  MSG message;

  while(PeekMessage(&message,NULL,0,0,PM_REMOVE))
  {
    DispatchMessage(&message);
    
    UINT msg = message.message;

    if (msg == WM_LBUTTONDOWN ||
        msg == WM_RBUTTONDOWN ||
        msg == WM_MBUTTONDOWN ||
        msg == WM_LBUTTONUP   ||
        msg == WM_RBUTTONUP   ||
        msg == WM_MBUTTONUP   ||
        msg == WM_KEYDOWN     || 
        msg == WM_KEYUP       ||
        msg == WM_SYSKEYUP    ||
        msg == WM_SYSKEYDOWN)
    {
      break;
    }    
  }*/

}

void lpCleanUp()
{
  SDL_Quit();
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

bool lpKeyWentDown(int key)
{
  return (keyState[key]==true && lastKeyState[key]==false);
}

bool lpKeyIsDown(int key)
{
  return keyState[key];
}

bool lpKeyWentUp(int key)
{
  return (keyState[key]==false && lastKeyState[key]==true);
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

int lpMouseWheelDelta()
{
  return wheelDelta;
}

void lpBlit(int width,int height,unsigned char* data)
{      
  SDL_Surface* screen = SDL_GetVideoSurface();

  SDL_LockSurface(screen);
  
  unsigned char* pixels = (unsigned char*)screen->pixels;
  
  memcpy(pixels,data,1024*576*4);
  
  SDL_UnlockSurface(screen);

  SDL_UpdateRect(screen,0,0,0,0);        
}