// This software is in the public domain. Where that dedication is not
// recognized, you are granted a perpetual, irrevocable license to copy
// and modify this file as you see fit.

#ifndef LAZY_WIDGETS_H_
#define LAZY_WIDGETS_H_

#include "lg.h"
#include "lp.h"

void lwInit();
void lwUpdate();

void lwPushId(int id);
void lwPopId();
int lwGetTopId();

void lwPanelBegin(Rect r,int z,bool shadow=true);
void lwPanelEnd();

void lwDrawPanels();

void lwModalBegin();
void lwModalEnd();

bool lwNoneActive();
bool lwIsActive(int id);
void lwSetActive(int id);

bool lwFocusedId();
bool lwNoneFocused();
bool lwHasFocus(int id);
void lwSetFocus(int id);
void lwKillFocus();
bool lwGainedFocus(int id);

bool lwMouseIsOver(Rect r);

bool lwInputNotBlocked();

int lwFrameCounter();

int lwRemapRange(float x,float xmin,float xmax,int ymin,int ymax);
float lwRemapRange(float x,float xmin,float xmax,float ymin,float ymax);

bool lwButtonLogic(int id,Rect r,bool enabled,int* state);
bool lwButtonLogic(int id,Rect r,int* state);

template<typename T> bool lwSliderLogic(int id,Rect r,int start,int stop,int mouse,T vmin,T vmax,T* var,int* handle);
template<typename T> bool lwHSliderLogic(int id,Rect r,T vmin,T vmax,T* var,int* handle);
template<typename T> bool lwVSliderLogic(int id,Rect r,T vmin,T vmax,T* var,int* handle);
template<typename T> bool lwScrollBarLogic(int id,Rect r,int start,int stop,int size,int mouse,T vmin,T vmax,T* var,int* handle);

bool lwDragRectLogic(int id,Rect r,int* x,int* y);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T> bool lwSliderLogic(int id,Rect r,int start,int stop,int mouse,T vmin,T vmax,T* var,int* handle)
{
  const T old = *var;  

  if (lpMouseButtonWentDown(LP_BUTTON_LEFT) && lwMouseIsOver(r) && lwInputNotBlocked() && lwNoneActive())
  {
    lwSetActive(id);    
  }

  if (lpMouseButtonIsDown(LP_BUTTON_LEFT) && lwIsActive(id))
  {
    *var = lwRemapRange(mouse,start,stop-1,vmin,vmax);
  }

  *handle = lwRemapRange(*var,vmin,vmax,start,stop-1);

  return (*var != old);
}

template<typename T> bool lwHSliderLogic(int id,Rect r,T vmin,T vmax,T* var,int* handle)
{
  return lwSliderLogic(id,r,r.x0,r.x1,lpMouseX(),vmin,vmax,var,handle);
}

template<typename T> bool lwVSliderLogic(int id,Rect r,T vmin,T vmax,T* var,int* handle)
{
  return lwSliderLogic(id,r,r.y0,r.y1,lpMouseY(),vmin,vmax,var,handle);
}

template<typename T> bool lwScrollBarLogic(int id,Rect r,int start,int stop,int size,int mouse,T vmin,T vmax,T* var,int* handle)
{
  const T old = *var;

  static int dragStart;

  if (lpMouseButtonWentDown(LP_BUTTON_LEFT) && lwMouseIsOver(r) && lwInputNotBlocked() && lwNoneActive())
  {
    *handle = lwRemapRange(*var,vmin,vmax,start,stop-size);

    if (mouse<*handle || mouse>*handle+size-1)
    {
       *handle = mouse-size/2;
    }

    dragStart = mouse - *handle;
    lwSetActive(id);
  }

  if (lpMouseButtonIsDown(LP_BUTTON_LEFT) && lwIsActive(id))
  {
    *var = lwRemapRange(mouse-dragStart,start,stop-size,vmin,vmax);
  }

  *handle = lwRemapRange(*var,vmin,vmax,start,stop-size);
  
  return (*var != old);
}

#endif
