// This software is in the public domain. Where that dedication is not
// recognized, you are granted a perpetual, irrevocable license to copy
// and modify this file as you see fit.

#include <cmath>
#include <cassert>

#include <vector>
#include <stack>
#include <algorithm>

#include "lw.h"
#include "lp.h"

using namespace std;

struct Panel
{
  Panel(Rect r,int z,LBitmap b,bool shadow) : r(r),z(z),b(b),shadow(shadow) { }
  Rect r;
  int z;  
  LBitmap b;
  bool shadow;
};

typedef stack<int> Trace;

template <typename T,int N>
struct ShortStack
{
  T elems[N];
  int size;
  ShortStack() : size(0) {}
  ShortStack(const ShortStack<T,N>& stack) { assert(stack.size<=N); size = stack.size; for(int i=0;i<size;i++) { elems[i] = stack.elems[i]; } }
  ShortStack(const ShortStack<T,N>& stack,const T& elem)
  {
    assert(stack.size<N);
    size = stack.size;
    for(int i=0;i<size;i++) { elems[i] = stack.elems[i]; }    
    push(elem); 
  }
  ShortStack& operator=(const ShortStack<T,N>& stack) { assert(stack.size<=N); size = stack.size; for(int i=0;i<size;i++) { elems[i] = stack.elems[i]; } return *this; }
  void push(const T& elem) { assert(size<N); elems[size]=elem; size++; }
  T& top() { assert(size>0); return elems[size-1]; }
  const T& top() const { assert(size>0); return elems[size-1]; }
  void pop() { assert(size>0); size--; }
  void clear() { size = 0; }
  bool empty() { return size==0; }
};

template<typename T,int N>
bool operator==(const ShortStack<T,N>& a,const ShortStack<T,N>& b)
{
  if (a.size!=b.size) { return false; }
  for(int i=0;i<a.size;i++) { if (a.elems[i]!=b.elems[i]) { return false; } }
  return true;
}

typedef ShortStack<int,16> IdStack;
typedef IdStack IdTrace;

IdTrace active;
IdTrace focused;

int frameCounter = -1;

vector<Panel> panels;
vector<Panel> lastPanels;

stack<Panel> panelStack;
//stack<Rect> clipStack;
//stack<LBitmap> outputStack;

IdStack idStack;

int modalLevel = 0;
int modalLevelMax = 0;
int lastModalLevelMax = 0;

void lwPushId(int id)
{
  idStack.push(id);
}

void lwPopId()
{
  assert(!idStack.empty());
  idStack.pop();
}

int lwGetTopId()
{
  assert(!idStack.empty());
  return idStack.top();
}

bool lwNoneActive()
{
  return active.empty();
}

bool lwIsActive(int id)
{
  return active==IdTrace(idStack,id);
}

void lwSetActive(int id)
{
  active = IdTrace(idStack,id);
}

bool lwNoneFocused()
{
  return focused.empty();
}

bool lwHasFocus(int id)
{
  return focused==IdTrace(idStack,id);
}

void lwSetFocus(int id)
{
  focused = IdTrace(idStack,id);
}

void lwKillFocus()
{
  focused.clear();
}

bool lwGainedFocus(int id)
{
  static IdTrace focused;

  const bool gainedFocus = (lwHasFocus(id) && !(focused==IdTrace(idStack,id)));
  if (lwHasFocus(id)) { focused = IdTrace(idStack,id); }
  if (lwNoneFocused()) { focused.clear(); }
  return gainedFocus;
}

void lwPanelBegin(Rect r,int z,bool shadow)
{
  const int w = r.x1-r.x0;
  const int h = r.y1-r.y0;

  LBitmap b = LBitmap(w,h);
  Panel p = Panel(r,z,b,shadow);
  
  panels.push_back(p);

  panelStack.push(p);

  lgPushTarget(LBitmap(b,RectWH(-r.x0,-r.y0,w,h)),r);
}

void lwPanelEnd()
{
  lgPopTarget();

  panelStack.pop();
}

bool lwMouseIsOver(Rect r)
{
  const int z = panelStack.empty() ? -1 : panelStack.top().z;
  
  for(size_t i=0;i<lastPanels.size();i++)
  {
    const Rect pr = lastPanels[i].r;
    if (z<lastPanels[i].z && 
        lpMouseX()>=pr.x0 && lpMouseX()<pr.x1 &&
        lpMouseY()>=pr.y0 && lpMouseY()<pr.y1)
    {
      return false;
    }
  }

  if (lpMouseX()>=r.x0 && lpMouseX()<r.x1 &&
      lpMouseY()>=r.y0 && lpMouseY()<r.y1)
  {
    return true;
  }

  return false;
}

void lwModalBegin()
{
  modalLevel++;
  modalLevelMax = max(modalLevel,modalLevelMax);
}

void lwModalEnd()
{
  modalLevel--;
}

bool lwInputNotBlocked() { return (modalLevel<lastModalLevelMax) ? false : true; }

bool panelCmp(const Panel& a,const Panel& b)
{
  return a.z < b.z;
}

const ARGB8 bmpShadowData[256] = {
0x00000000,0x00000000,0x00000000,0x00000000,0x01000000,0x02000000,0x02000000,0x02000000,
0x02000000,0x02000000,0x02000000,0x01000000,0x00000000,0x00000000,0x00000000,0x00000000,
0x00000000,0x00000000,0x00000000,0x02000000,0x03000000,0x05000000,0x06000000,0x06000000,
0x06000000,0x05000000,0x04000000,0x03000000,0x02000000,0x00000000,0x00000000,0x00000000,
0x00000000,0x00000000,0x02000000,0x04000000,0x07000000,0x0a000000,0x0d000000,0x0e000000,
0x0e000000,0x0c000000,0x0a000000,0x06000000,0x04000000,0x02000000,0x00000000,0x00000000,
0x00000000,0x02000000,0x04000000,0x08000000,0x0d000000,0x12000000,0x17000000,0x1a000000,
0x1a000000,0x17000000,0x12000000,0x0c000000,0x08000000,0x04000000,0x02000000,0x00000000,
0x01000000,0x03000000,0x07000000,0x0d000000,0x15000000,0x1e000000,0x26000000,0x2b000000,
0x2b000000,0x26000000,0x1e000000,0x14000000,0x0c000000,0x06000000,0x03000000,0x01000000,
0x02000000,0x05000000,0x0a000000,0x13000000,0x1e000000,0x2b000000,0x37000000,0x3e000000,
0x3e000000,0x37000000,0x2b000000,0x1d000000,0x12000000,0x09000000,0x04000000,0x01000000,
0x02000000,0x06000000,0x0d000000,0x17000000,0x27000000,0x37000000,0x46000000,0x4f000000,
0x4f000000,0x45000000,0x36000000,0x26000000,0x17000000,0x0c000000,0x06000000,0x02000000,
0x02000000,0x06000000,0x0e000000,0x1a000000,0x2b000000,0x3e000000,0x4f000000,0x59000000,
0x58000000,0x4e000000,0x3d000000,0x2a000000,0x1a000000,0x0e000000,0x06000000,0x02000000,
0x02000000,0x06000000,0x0e000000,0x1a000000,0x2b000000,0x3e000000,0x4f000000,0x58000000,
0x58000000,0x4e000000,0x3d000000,0x2a000000,0x19000000,0x0d000000,0x06000000,0x02000000,
0x02000000,0x06000000,0x0c000000,0x17000000,0x26000000,0x37000000,0x45000000,0x4e000000,
0x4e000000,0x45000000,0x35000000,0x25000000,0x17000000,0x0c000000,0x06000000,0x02000000,
0x02000000,0x04000000,0x0a000000,0x12000000,0x1d000000,0x2a000000,0x36000000,0x3d000000,
0x3d000000,0x35000000,0x2a000000,0x1d000000,0x12000000,0x09000000,0x04000000,0x01000000,
0x01000000,0x03000000,0x06000000,0x0c000000,0x14000000,0x1d000000,0x26000000,0x2a000000,
0x2a000000,0x25000000,0x1d000000,0x14000000,0x0c000000,0x06000000,0x03000000,0x00000000,
0x00000000,0x02000000,0x04000000,0x07000000,0x0c000000,0x12000000,0x17000000,0x1a000000,
0x19000000,0x16000000,0x12000000,0x0c000000,0x07000000,0x03000000,0x02000000,0x00000000,
0x00000000,0x00000000,0x02000000,0x04000000,0x06000000,0x09000000,0x0c000000,0x0e000000,
0x0d000000,0x0c000000,0x09000000,0x06000000,0x03000000,0x02000000,0x00000000,0x00000000,
0x00000000,0x00000000,0x00000000,0x02000000,0x03000000,0x04000000,0x06000000,0x06000000,
0x06000000,0x05000000,0x04000000,0x03000000,0x01000000,0x00000000,0x00000000,0x00000000,
0x00000000,0x00000000,0x00000000,0x00000000,0x01000000,0x02000000,0x02000000,0x02000000,
0x02000000,0x02000000,0x01000000,0x01000000,0x00000000,0x00000000,0x00000000,0x00000000
};

const LBitmap bmpShadow = LBitmap(16,16,16,(ARGB8*)bmpShadowData);

void lwDrawShadow(Rect r)
{
  const int cw = (bmpShadow.width()-2)/2;
  const int ch = (bmpShadow.height()-2)/2;

  lgDrawBitmap(r.x0,   r.y0   ,lgSubBitmap(bmpShadow,RectWH(0   ,0   ,cw,ch)));
  lgDrawBitmap(r.x1-cw,r.y0   ,lgSubBitmap(bmpShadow,RectWH(cw+2,0   ,cw,ch)));
  lgDrawBitmap(r.x1-cw,r.y1-ch,lgSubBitmap(bmpShadow,RectWH(cw+2,ch+2,cw,ch)));
  lgDrawBitmap(r.x0,   r.y1-ch,lgSubBitmap(bmpShadow,RectWH(0   ,ch+2,cw,ch)));

  for(int y=0;y<ch;y++)
  {
    lgDrawHLine(r.y1-cw+y,r.x0+cw,r.x1-cw,bmpShadow.data()[cw+(ch+2+y)*bmpShadow.stride()],bmpShadow.data()[cw+1+(ch+2+y)*bmpShadow.stride()]);
  }

  for(int x=0;x<cw;x++)
  {
    lgDrawVLine(r.x0+x   ,r.y0+cw,r.y1-cw,bmpShadow.data()[x+(ch)*bmpShadow.stride()],bmpShadow.data()[x+(ch+1)*bmpShadow.stride()]);
    lgDrawVLine(r.x1-cw+x,r.y0+cw,r.y1-cw,bmpShadow.data()[x+cw+2+(ch)*bmpShadow.stride()],bmpShadow.data()[x+cw+2+(ch+1)*bmpShadow.stride()]);
  }
}

void lwDrawPanels()
{
  sort(panels.begin(),panels.end(),panelCmp);

  for(size_t i=0;i<panels.size();i++)
  {
    Panel& p = panels[i];
    if (p.shadow==true) { lwDrawShadow(Rect(p.r.x0-7,p.r.y0-2,p.r.x1+7,p.r.y1+7)); }
    lgDrawBitmapOpaque(p.r.x0,p.r.y0,p.b);
  }
}

void lwInit()
{
  active.clear();
  focused.clear();
  frameCounter = 0;
  //panelStack.clear();
  while(!panelStack.empty()) { panelStack.pop(); }
  panels.clear();
  lastPanels.clear();
}

void lwUpdate()
{
  assert(idStack.empty());
  assert(panelStack.empty());

  if (!lpMouseButtonIsDown(LP_BUTTON_LEFT)) { active.clear(); };

  /*
  XXX: explicitly release bitmaps ? (because the references are copied to lastPanels..)
  for(size_t i=0;i<panels.size();i++)
  {
    lgFreeBitmap(&panels[i].b);      
  }
  */

  lastPanels = panels;
  panels.clear();
  lastModalLevelMax = modalLevelMax; modalLevel = 0; modalLevelMax = 0;

  frameCounter++;
  //active==-1 when mouseUp ?
}

int lwRemapRange(float x,float xmin,float xmax,int ymin,int ymax)
{
  return max(min(ymin+int(floor((float(x-xmin)/float(xmax-xmin))*float(ymax-ymin)+0.5f)),max(ymin,ymax)),min(ymin,ymax));
}

float lwRemapRange(float x,float xmin,float xmax,float ymin,float ymax)
{
  return max(min(ymin+((float(x-xmin)/float(xmax-xmin))*float(ymax-ymin)),max(ymin,ymax)),min(ymin,ymax));
}

bool lwDragRectLogic(int id,Rect r,int* x,int* y)
{
  static int dragStartX;
  static int dragStartY;
  static int dragStartMouseX;
  static int dragStartMouseY;

  if (lpMouseButtonWentDown(LP_BUTTON_LEFT) && lwMouseIsOver(r) && lwNoneActive() && lwInputNotBlocked())
  {
    lwSetActive(id);    
    
    if (x!=0) dragStartX = *x;
    if (y!=0) dragStartY = *y;
    
    dragStartMouseX = lpMouseX();
    dragStartMouseY = lpMouseY();
  }

  if (lpMouseButtonIsDown(LP_BUTTON_LEFT) && lwIsActive(id))
  {
    if (x!=0) *x = dragStartX + (lpMouseX()-dragStartMouseX);
    if (y!=0) *y = dragStartY + (lpMouseY()-dragStartMouseY);
  }

  return true;
}

int lwFrameCounter()
{
  return frameCounter;
}

bool lwButtonLogic(int id,Rect r,bool enabled,int* out_state)
{
  int state = 0;

  bool clicked = false;

  if (lwMouseIsOver(r) && lwInputNotBlocked() && lwNoneActive() && enabled)
  {
    state = 1;

    if (lpMouseButtonWentDown(LP_BUTTON_LEFT))
    {
      lwSetActive(id);
    }
  }

  if (lwIsActive(id))
  {
    state = 2;
  }

  if (lpMouseButtonWentUp(LP_BUTTON_LEFT) && lwIsActive(id) && lwMouseIsOver(r))
  {
    clicked = true;
  }

  if (out_state!=0) { *out_state = state; }
  
  return clicked;
}

bool lwButtonLogic(int id,Rect r,int* out_state)
{
  return lwButtonLogic(id,r,true,out_state);
}
