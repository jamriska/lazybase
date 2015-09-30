// This software is in the public domain. Where that dedication is not
// recognized, you are granted a perpetual, irrevocable license to copy
// and modify this file as you see fit.

#include <cstdlib>
#include <cassert>
#include <cmath>
#include <cstring>

#include <stack>

#include "lg.h"

#define max(a,b)  ((a) > (b) ? (a) : (b))
#define min(a,b)  ((a) < (b) ? (a) : (b))

std::stack<LBitmap> targetStack;
std::stack<Rect> clipStack;

LBitmap::LBitmap() : w(0),h(0),d(0),rd(0),rc(0) { }

LBitmap::LBitmap(const LBitmap& b)
{
  w = b.w;
  h = b.h;
  s = b.s;
  d = b.d;
  rd = b.rd;
  rc = b.rc;
  if (rc!=0) { (*rc)++; }
}

LBitmap::LBitmap(const LBitmap& b,const Rect& r)
{
  assert(r.x1>r.x0);
  assert(r.y1>r.y0);

  w = r.x1-r.x0;
  h = r.y1-r.y0;
  s = b.s;
  d = b.d + r.y0*s + r.x0;
  rd = b.rd;
  rc = b.rc;
  if (rc!=0) { (*rc)++; }
}

LBitmap& LBitmap::operator=(const LBitmap& b)
{
  if (&b!=this)
  {
    int* orc = rc;
    ARGB8* ord = rd;

    w = b.w;
    h = b.h;
    s = b.s;
    d = b.d;
    rd = b.rd;
    rc = b.rc;
    if (rc!=0) { (*rc)++; }

    if (orc!=0)
    {
      (*orc)--;
      if (*orc==0)
      {
        delete orc;
        delete[] ord;
      }
    }
  }

  return *this; 
}

LBitmap::LBitmap(int width,int height)
{
  assert(width>0);
  assert(height>0);
  
  w = width;
  h = height;
  s = w;
  d = new ARGB8[w*h];
  rd = d;
  rc = new int;
  *rc = 1;
}

LBitmap::LBitmap(int width,int height,int stride,ARGB8* data)
{
  assert(width>0);
  assert(height>0);
  assert(data!=0);
  
  w = width;
  h = height;
  s = w;
  d = new ARGB8[w*h];
  rd = d;
  rc = new int;
  *rc = 1;

  if (stride==width)
  {
    memcpy(d,data,w*h*sizeof(ARGB8));
  }
  else
  {
    ARGB8* src = data;
    ARGB8* dst = d;
    for(int row=0;row<h;row++)
    {
      memcpy(dst,src,w*sizeof(ARGB8));
      src += stride;
      dst += w;
    }
  }
}

LBitmap::~LBitmap()
{
  if (rc!=0)
  {
    (*rc)--;
    if (*rc==0)
    {
      delete rc;
      delete[] rd;
    }
  }
}

int LBitmap::width() const { return w; }
int LBitmap::height() const { return h; }
int LBitmap::stride() const { return s; }
ARGB8* LBitmap::data() const { return d; }

ARGB8& LBitmap::operator()(int x,int y) { return d[x+y*s]; }
const ARGB8& LBitmap::operator()(int x,int y) const { return d[x+y*s]; }
/*
Bitmap lgAllocBitmap(int width,int height)
{
  Bitmap b;
  b.width() = width;
  b.height() = height;
  b.stride = b.width();
  b.data = (ARGB8*)malloc(b.width()*b.height()*sizeof(ARGB8));
  
  if (b.data==0)
  {
    b.width() = 0;
    b.height() = 0;
    b.stride = 0;
  }

  return b;
}

void lgFreeBitmap(Bitmap* b)
{
  if (b!=0)
  {
    if (b->data!=0)
    {
      free(b->data);
    }
    b->data = 0;
    b->width = 0;
    b->height = 0;
    b->stride = 0;
  }
}

Bitmap lgSubBitmap(Bitmap b,Rect r)
{
  Bitmap sb;
  sb.width() = r.x1-r.x0;
  sb.height() = r.y1-r.y0;
  sb.stride = b.stride;
  sb.data = &b.data[r.x0+r.y0*b.stride];

  return sb;
}
    
Bitmap lgCopyBitmap(Bitmap b)
{
  return b;
}

Bitmap lgCopySubBitmap(Bitmap b,Rect r)
{
  return b;
}
*/

LBitmap lgSubBitmap(LBitmap b,Rect r)
{
  return LBitmap(b,r);
}
    

// [u0,v0] are uv coordinates at the top-left corner of dst pixel [0,0]
// [dudx,dudy], [dvdx,dvdy] are derivatives of uv coordinates with respect to xy coordiantes of dst pixels
// both src and dst samples are pixel-centered
void lgResampleNearest(LBitmap src,float u0,float v0,float dudx,float dudy,float dvdx,float dvdy,ARGB8 bkg,LBitmap* dst)
{
  for(int y=0;y<dst->height();y++)
  for(int x=0;x<dst->width();x++)
  {
    const float u = u0+(float(x)+0.5f)*dudx+(float(y)+0.5f)*dudy;
    const float v = v0+(float(x)+0.5f)*dvdx+(float(y)+0.5f)*dvdy;

    if (u>=0.0f &&
        v>=0.0f &&
        u<=(float(src.width())) && 
        v<=(float(src.height())))
    {
      const int iu = (int)floorf(u);
      const int iv = (int)floorf(v);

      #define CLAMP(x,xmin,xmax) min(max((x),(xmin)),(xmax))
      //#define PIXEL(x,y) src.data[x+y*src.stride]
      #define PIXEL(x,y) src(x,y)

      dst->data()[x+y*dst->stride()] = PIXEL(CLAMP(iu,0,src.width()-1),CLAMP(iv,0,src.height()-1));

      #undef PIXEL
      #undef CLAMP
    }
    else
    {
      dst->data()[x+y*dst->stride()] = bkg;
    }
  }
}

// [u0,v0] are uv coordinates at the top-left corner of dst pixel [0,0]
// [dudx,dudy], [dvdx,dvdy] are derivatives of uv coordinates with respect to xy coordiantes of dst pixels
// both src and dst samples are pixel-centered
void lgResampleBilinear(LBitmap src,float u0,float v0,float dudx,float dudy,float dvdx,float dvdy,ARGB8 bkg,LBitmap* dst)
{
  u0 = u0-0.5f;
  v0 = v0-0.5f;

  for(int y=0;y<dst->height();y++)
  for(int x=0;x<dst->width();x++)
  {
    const float u = u0+(float(x)+0.5f)*dudx+(float(y)+0.5f)*dudy;
    const float v = v0+(float(x)+0.5f)*dvdx+(float(y)+0.5f)*dvdy;

    if (u>=-0.5f &&
        v>=-0.5f &&
        u<=(float(src.width()-1)+0.5f) && 
        v<=(float(src.height()-1)+0.5f))
    {
      const int iu = (int)floorf(u);
      const int iv = (int)floorf(v);

      #define CLAMP(x,xmin,xmax) min(max((x),(xmin)),(xmax))
      //#define PIXEL(x,y) src.data[x+y*src.stride]
      #define PIXEL(x,y) src(x,y)
      
      const ARGB8 c00 = PIXEL(CLAMP(iu  ,0,src.width()-1),CLAMP(iv  ,0,src.height()-1));
      const ARGB8 c10 = PIXEL(CLAMP(iu+1,0,src.width()-1),CLAMP(iv  ,0,src.height()-1));
      const ARGB8 c01 = PIXEL(CLAMP(iu  ,0,src.width()-1),CLAMP(iv+1,0,src.height()-1));
      const ARGB8 c11 = PIXEL(CLAMP(iu+1,0,src.width()-1),CLAMP(iv+1,0,src.height()-1));

      #undef PIXEL
      #undef CLAMP

      const float tu = (u-iu);
      const float tv = (v-iv);

      const float w00 = (1.0f-tu)*(1.0f-tv);
      const float w10 = (     tu)*(1.0f-tv);
      const float w01 = (1.0f-tu)*(     tv);
      const float w11 = (     tu)*(     tv);

      const int r = float((c00 & 0x00FF0000) >> 16)*w00 + float((c10 & 0x00FF0000) >> 16)*w10 + float((c01 & 0x00FF0000) >> 16)*w01 + float((c11 & 0x00FF0000) >> 16)*w11;
      const int g = float((c00 & 0x0000FF00) >>  8)*w00 + float((c10 & 0x0000FF00) >>  8)*w10 + float((c01 & 0x0000FF00) >>  8)*w01 + float((c11 & 0x0000FF00) >>  8)*w11;
      const int b = float((c00 & 0x000000FF)      )*w00 + float((c10 & 0x000000FF)      )*w10 + float((c01 & 0x000000FF)      )*w01 + float((c11 & 0x000000FF)      )*w11;
      
      dst->data()[x+y*dst->stride()] = Color(r,g,b);
    }
    else
    {
      dst->data()[x+y*dst->stride()] = bkg;
    }
  }
}

void lgPushTarget(LBitmap b,Rect clip)
{
  targetStack.push(b);
  clipStack.push(clip);
}

void lgPopTarget()
{
  assert(!targetStack.empty());
  clipStack.pop();
  targetStack.pop();
}

LBitmap lgGetTarget()
{
  assert(!targetStack.empty());
  return targetStack.top();
}

void lgPushClipRect(Rect r)
{
  assert(!clipStack.empty());
  Rect q = clipStack.top();
  clipStack.push(Rect(max(r.x0,q.x0),max(r.y0,q.y0),
                      min(r.x1,q.x1),min(r.y1,q.y1)));
}

void lgPopClipRect()
{
  assert(clipStack.size()>1);
  clipStack.pop();
}

Rect lgGetClipRect()
{
  assert(!clipStack.empty());
  return clipStack.top();
}

/*
void lgSetOutput(LBitmap b)
{
  output = b;
  clip = RectWH(0,0,b.width(),b.height());
}

LBitmap lgGetOutput()
{
  return output;
}
*/
/*
void lgSetClipRect(Rect r)
{
  clip = r;
}

Rect lgGetClipRect()
{
  return clip;
}
*/
void lgPutPixel(int x,int y,ARGB8 c)
{
  const Rect clip = lgGetClipRect();

  if (x>=clip.x0 && x<clip.x1 &&
      y>=clip.y0 && y<clip.y1)
  {
    //output.data[x+y*output.stride] = c;
    lgGetTarget()(x,y) = c;
  }
}

ARGB8 lgGetPixel(int x,int y)
{
  //return output.data[x+y*output.stride];
  return lgGetTarget()(x,y);
}

void lgDrawHLine(int y,int x0,int x1,ARGB8 c)
{
  const Rect clip = lgGetClipRect();

  if (y>=clip.y0 && y<clip.y1)
  {
    x0 = max(x0,clip.x0);
    x1 = min(x1,clip.x1);

    //ARGB8* out = &lgGetTarget().data[x0+y*output.stride];
    ARGB8* out = &lgGetTarget()(x0,y);

    for(int x=x0;x<x1;x++)
    {
      *out = c;
      out++;
    }
  }
}

void lgDrawVLine(int x,int y0,int y1,ARGB8 c)
{  
  const Rect clip = lgGetClipRect();

  if (x>=clip.x0 && x<clip.x1)
  {
    y0 = max(y0,clip.y0);
    y1 = min(y1,clip.y1);

    const int stride = lgGetTarget().stride();
    //ARGB8* out = &lgGetTarget().data[x+y0*stride];
    ARGB8* out = &lgGetTarget()(x,y0);

    for(int y=y0;y<y1;y++)
    {
      *out = c;
      out += stride;
    }
  }
}


#define IS_OPAQUE(c)   ((c) >= 0xFF000000)
#define IS_INVIS(c)    ((c) <= 0x00FFFFFF)


static inline ARGB8 blend(ARGB8 dest, ARGB8 src,uchar a)
{
  if (a==255) return src;
   // accelerated blend computes r and b in parallel
   
   unsigned int rb_src  = src  & 0xff00ff;
   unsigned int rb_dest = dest & 0xff00ff;
   unsigned int rb      = rb_dest + ((rb_src - rb_dest) * a >> 8);
   unsigned int g_dest  = (dest & 0x00ff00);
   unsigned int g       = g_dest + (((src & 0xff00) - (dest & 0xff00)) * a >> 8);
   // note we do not compute a real dest alpha
   return (rb & 0xff00ff) + (g & 0x00ff00) + 0xff000000; 
}

void lgDrawHLine(int y,int x0,int x1,ARGB8 c0,ARGB8 c1)
{
  const Rect clip = lgGetClipRect();

  if (y>=clip.y0 && y<clip.y1)
  {
    const float r0 = (c0 & 0x00FF0000) >> 16;
    const float g0 = (c0 & 0x0000FF00) >> 8;
    const float b0 = (c0 & 0x000000FF);
    const float a0 = (c0 & 0xFF000000) >> 24;

    const float r1 = (c1 & 0x00FF0000) >> 16;
    const float g1 = (c1 & 0x0000FF00) >> 8;
    const float b1 = (c1 & 0x000000FF);
    const float a1 = (c1 & 0xFF000000) >> 24;

    const float dt = 1.0f/float(x1-x0-1);
    float t = x0<clip.x0 ? float(clip.x0-x0)/float(x1-x0) : 0;

    x0 = max(x0,clip.x0);
    x1 = min(x1,clip.x1);

    //ARGB8* out = &lgGetTarget().data[x0+y*output.stride];
    ARGB8* out = &lgGetTarget()(x0,y);//.data[x0+y*output.stride];

    for(int x=x0;x<x1;x++)
    {
      unsigned char r = (1.0f-t)*r0 + t*r1;
      unsigned char g = (1.0f-t)*g0 + t*g1;
      unsigned char b = (1.0f-t)*b0 + t*b1;
      unsigned char a = (1.0f-t)*a0 + t*a1;

      //*out = Color(r,g,b);
      *out = blend(*out,Color(r,g,b),a);
      
      out++;

      t += dt;
    }
  }
}

void lgDrawVLine(int x,int y0,int y1,ARGB8 c0,ARGB8 c1)
{  
  const Rect clip = lgGetClipRect();

  if (x>=clip.x0 && x<clip.x1)
  {
    const float r0 = (c0 & 0x00FF0000) >> 16;
    const float g0 = (c0 & 0x0000FF00) >> 8;
    const float b0 = (c0 & 0x000000FF);
    const float a0 = (c0 & 0xFF000000) >> 24;

    const float r1 = (c1 & 0x00FF0000) >> 16;
    const float g1 = (c1 & 0x0000FF00) >> 8;
    const float b1 = (c1 & 0x000000FF);
    const float a1 = (c1 & 0xFF000000) >> 24;

    const float dt = 1.0f/float(y1-y0-1);
    float t = y0<clip.y0 ? float(clip.y0-y0)/float(y1-y0) : 0;

    y0 = max(y0,clip.y0);
    y1 = min(y1,clip.y1);

    const int stride = lgGetTarget().stride();
    //ARGB8* out = &lgGetTarget().data[x+y0*stride];
    ARGB8* out = &lgGetTarget()(x,y0);

    for(int y=y0;y<y1;y++)
    {
      unsigned char r = (1.0f-t)*r0 + t*r1;
      unsigned char g = (1.0f-t)*g0 + t*g1;
      unsigned char b = (1.0f-t)*b0 + t*b1;

      unsigned char a = (1.0f-t)*a0 + t*a1;

      //*out = Color(r,g,b);
      *out = blend(*out,Color(r,g,b),a);
      
      out += stride;

      t += dt;
    }
  }
}

static inline float lerp(float a,float b,float t)
{
  return (1.0f-t)*a + t*b;
}

void lgDrawRectSolid(Rect r,ARGB8 c00,ARGB8 c10,ARGB8 c01,ARGB8 c11)
{
  const Rect clip = lgGetClipRect();
  //const int stride = output.stride;

  const float r00 = (c00 & 0x00FF0000) >> 16;
  const float g00 = (c00 & 0x0000FF00) >> 8;
  const float b00 = (c00 & 0x000000FF);

  const float r10 = (c10 & 0x00FF0000) >> 16;
  const float g10 = (c10 & 0x0000FF00) >> 8;
  const float b10 = (c10 & 0x000000FF);

  const float r01 = (c01 & 0x00FF0000) >> 16;
  const float g01 = (c01 & 0x0000FF00) >> 8;
  const float b01 = (c01 & 0x000000FF);

  const float r11 = (c11 & 0x00FF0000) >> 16;
  const float g11 = (c11 & 0x0000FF00) >> 8;
  const float b11 = (c11 & 0x000000FF);

  const float dty = 1.0f/float(r.y1-r.y0-1);
  const float dtx = 1.0f/float(r.x1-r.x0-1);

  const float tx0 = r.x0<clip.x0 ? float(clip.x0-r.x0)/float(r.x1-r.x0) : 0;
  float ty = r.y0<clip.y0 ? float(clip.y0-r.y0)/float(r.y1-r.y0) : 0;

  const int x0 = max(r.x0,clip.x0);
  const int y0 = max(r.y0,clip.y0);
  const int x1 = min(r.x1,clip.x1);
  const int y1 = min(r.y1,clip.y1);

  for(int y=y0;y<y1;y++)
  {
    //ARGB8* out = &lgGetTarget().data[x0+y*stride];
    ARGB8* out = &lgGetTarget()(x0,y);//.data[x0+y*stride];
    
    float tx = tx0;
    for(int x=x0;x<x1;x++)
    {
      unsigned char r = lerp(lerp(r00,r10,tx),lerp(r01,r11,tx),ty);
      unsigned char g = lerp(lerp(g00,g10,tx),lerp(g01,g11,tx),ty);
      unsigned char b = lerp(lerp(b00,b10,tx),lerp(b01,b11,tx),ty);

      *out = Color(r,g,b);
      out++;
      
      tx += dtx;
    }

    ty += dty;
  }
}

void lgDrawRectOutline(Rect r,ARGB8 c)
{
  lgDrawHLine(r.y0  ,r.x0,r.x1,c);
  lgDrawHLine(r.y1-1,r.x0,r.x1,c);
  lgDrawVLine(r.x0  ,r.y0,r.y1,c);
  lgDrawVLine(r.x1-1,r.y0,r.y1,c);
}

void lgDrawRectSolid(Rect r,ARGB8 c)
{
  const Rect clip = lgGetClipRect();

  const int x0 = max(r.x0,clip.x0);
  const int y0 = max(r.y0,clip.y0);
  const int x1 = min(r.x1,clip.x1);
  const int y1 = min(r.y1,clip.y1);
  
  //const int stride = output.stride;
  
  for(int y=y0;y<y1;y++)
  {
    //ARGB8* out = &lgGetTarget().data[x0+y*stride];
    ARGB8* out = &lgGetTarget()(x0,y);//.data[x0+y*stride];
    for(int x=x0;x<x1;x++)
    {
      *out = c;
      out++;
    }
  }
}

#include <cstdio>

void lgDrawBitmap(int x,int y,LBitmap b)
{
  const Rect clip = lgGetClipRect();

  const int bx0 = max(0,clip.x0-x);
  const int by0 = max(0,clip.y0-y);
  const int bx1 = min(b.width(),clip.x1-x);
  const int by1 = min(b.height(),clip.y1-y);
  /*
  if ((bx1-bx0)>0 && (by1-by0)>0)
  {  
    for(int by=by0;by<by1;by++)
    {
      memcpy(&lgGetTarget().data[x+bx0+(y+by)*output.stride],&b.data[bx0+by*b.stride],(bx1-bx0)*4);
    }
  }
  */
  
  for(int by=by0;by<by1;by++)  
  for(int bx=bx0;bx<bx1;bx++)
  {
    int ox = x+bx;
    int oy = y+by;
    {
      //if (b.data[bx+by*b.stride]>=0xFF000000) output.data[ox+oy*output.stride] = b.data[bx+by*b.stride];
        
        //output.data[ox+oy*output.stride] = blend(output.data[ox+oy*output.stride],b.data[bx+by*b.stride],(b.data[bx+by*b.stride] & 0xFF000000) >> 24);
        lgGetTarget()(ox,oy) = blend(lgGetTarget()(ox,oy),b(bx,by),(b(bx,by) & 0xFF000000) >> 24);

    //    if (b.data[bx+by*b.stride]>=0xFF000000) output.data[ox+oy*output.stride] = b.data[bx+by*b.stride];
    }
  }
  
  /*
  const int x0 = max(x,clip.x0);
  const int y0 = max(y,clip.y0);
  const int x1 = min(x+,clip.x1);
  const int y1 = min(r.y1,clip.y1);
  
  const int stride = output.stride;
  
  for(int y=y0;y<y1;y++)
  {
    ARGB8* out = &lgGetTarget().data[x0+y*stride];
    for(int x=x0;x<x1;x++)
    {
      *out = c;
      out++;
    }
  }*/
}

void lgDrawBitmapOpaque(int x,int y,LBitmap b)
{
  const Rect clip = lgGetClipRect();

  const int bx0 = max(0,clip.x0-x);
  const int by0 = max(0,clip.y0-y);
  const int bx1 = min(b.width(),clip.x1-x);
  const int by1 = min(b.height(),clip.y1-y);
  
  if ((bx1-bx0)>0 && (by1-by0)>0)
  {  
    for(int by=by0;by<by1;by++)
    {
      //memcpy(&lgGetTarget().data[x+bx0+(y+by)*output.stride],&b.data[bx0+by*b.stride],(bx1-bx0)*4);
      memcpy(&lgGetTarget()(x+bx0,(y+by)),&b(bx0,by),(bx1-bx0)*sizeof(ARGB8));
    }
  }  
}

/*                                             *\

   .---- cw ----.   .-2-.
  /              \ /     \                        
  +---+---+---+---+---+---+---+---+---+---+ 
  |   |   |   |   | O---O |   |   |   |   | \
  +---+---+---+---+---+---+---+---+---+---+  \ 
  |   |   |   |   | O---O |   |   |   |   |   ch
  +---+---+---+---+---+---+---+---+---+---+  /
  |   |   |   |   | O---O |   |   |   |   | /
  +---+---+---+---+---+---+---+---+---+---+ 
  | O | O | O | O | O---O | O | O | O | O | \
  +-|-+-|-+-|-+-|-+-| X |-+-|-+-|-+-|-+-|-+  2
  | O | O | O | O | O---O | O | O | O | O | /
  +---+---+---+---+---+---+---+---+---+---+
  |   |   |   |   | O---O |   |   |   |   |
  +---+---+---+---+---+---+---+---+---+---+
  |   |   |   |   | O---O |   |   |   |   |
  +---+---+---+---+---+---+---+---+---+---+
  |   |   |   |   | O---O |   |   |   |   |
  +---+---+---+---+---+---+---+---+---+---+

\*                                            */  

void lgDrawNinePatch(Rect r,LBitmap patch)
{
  // assert bitmap dimensions

  const int cw = (patch.width()-2)/2;
  const int ch = (patch.height()-2)/2;

  lgDrawBitmap(r.x0,   r.y0   ,lgSubBitmap(patch,RectWH(0   ,0   ,cw,ch)));
  lgDrawBitmap(r.x1-cw,r.y0   ,lgSubBitmap(patch,RectWH(cw+2,0   ,cw,ch)));
  lgDrawBitmap(r.x1-cw,r.y1-ch,lgSubBitmap(patch,RectWH(cw+2,ch+2,cw,ch)));
  lgDrawBitmap(r.x0,   r.y1-ch,lgSubBitmap(patch,RectWH(0   ,ch+2,cw,ch)));

  for(int y=0;y<ch;y++)
  {
    lgDrawHLine(r.y0+y,r.x0+cw,r.x1-cw,patch.data()[cw+y*patch.stride()],patch.data()[cw+1+y*patch.stride()]);
    lgDrawHLine(r.y1-cw+y,r.x0+cw,r.x1-cw,patch.data()[cw+(ch+2+y)*patch.stride()],patch.data()[cw+1+(ch+2+y)*patch.stride()]);
  }

  for(int x=0;x<cw;x++)
  {
    lgDrawVLine(r.x0+x   ,r.y0+cw,r.y1-cw,patch.data()[x+(ch)*patch.stride()],patch.data()[x+(ch+1)*patch.stride()]);
    lgDrawVLine(r.x1-cw+x,r.y0+cw,r.y1-cw,patch.data()[x+cw+2+(ch)*patch.stride()],patch.data()[x+cw+2+(ch+1)*patch.stride()]);
  }

  lgDrawRectSolid(Rect(r.x0+cw,r.y0+cw,r.x1-cw,r.y1-cw),
                  patch.data()[cw  +ch*patch.stride()],
                  patch.data()[cw+1+ch*patch.stride()],
                  patch.data()[cw  +(ch+1)*patch.stride()],
                  patch.data()[cw+1+(ch+1)*patch.stride()]);
}

// XXX: the returned width can be smaller than the distance of the farthest visible pixel
int lgTextWidth(const char* text,const Font& font)
{
  const int len = strlen(text);
  
  if (len<1) return 0;

  int width = 0;

  for(int i=0;i<len;i++)
  {
    const Glyph& g = font.glyphs[text[i]-32];
    width += g.advanceX;
    //width += (i<len-1) ? g.advanceX : max(g.advanceX,(g.offsetX+g.width()));
  }

  return width;
}


/*
void lgDrawGlyph(int x,int y,const Glyph& glyph,ARGB8 color)
{  
  const uchar src_r = (color & 0x00FF0000) >> 16;
  const uchar src_g = (color & 0x0000FF00) >> 8;
  const uchar src_b = (color & 0x000000FF);
  
  const int gx0 = max(0,clip.x0-x-glyph.offsetX);
  const int gy0 = max(0,clip.y0-y-glyph.offsetY);
  const int gx1 = min(glyph.width(),clip.x1-x-glyph.offsetX);
  const int gy1 = min(glyph.height(),clip.y1-y-glyph.offsetY);

  const int ox = glyph.offsetX;
  const int oy = glyph.offsetY;

  for(int gy=gy0;gy<gy1;gy++)
  {
    for(int gx=gx0;gx<gx1;gx++)
    {   
      const uchar alpha = glyph.data[gx+gy*glyph.width()];
      ARGB8* out = &lgGetTarget().data[(x+gx+ox)+(y+gy+oy)*output.stride];

      const ARGB8 dst = *out;
      
      const uchar dst_r = (dst & 0x00FF0000) >> 16;
      const uchar dst_g = (dst & 0x0000FF00) >> 8;
      const uchar dst_b = (dst & 0x000000FF);

      const uchar r = ((255-alpha)*dst_r + alpha*src_r)/255;
      const uchar g = ((255-alpha)*dst_g + alpha*src_g)/255;
      const uchar b = ((255-alpha)*dst_b + alpha*src_b)/255;

      *out = Color(r,g,b);
    }
  }
}
*/

void lgDrawGlyph(int x,int y,const Glyph& glyph,ARGB8 color)
{  
  const Rect clip = lgGetClipRect();

  const int gx0 = max(0,clip.x0-x-glyph.offsetX);
  const int gy0 = max(0,clip.y0-y-glyph.offsetY);
  const int gx1 = min(glyph.width,clip.x1-x-glyph.offsetX);
  const int gy1 = min(glyph.height,clip.y1-y-glyph.offsetY);

  const int ox = glyph.offsetX;
  const int oy = glyph.offsetY;

  //lgDrawRectOutline(Rect(x+gx0+ox,y+gy0+oy,x+gx1+ox,y+gy1+oy),Color(0,255,0));

  for(int gy=gy0;gy<gy1;gy++)
  {
    const uchar* alpha = &glyph.data[gx0+gy*glyph.width];
    //ARGB8* out = &lgGetTarget().data[(x+gx0+ox)+(y+gy+oy)*output.stride];
    ARGB8* out = &lgGetTarget()(x+gx0+ox,y+gy+oy);

    for(int gx=gx0;gx<gx1;gx++)
    {   
      *out = blend(*out,color,*alpha);
      
      *alpha++;
      *out++;
    }
  }

}

void lgDrawText(int x,int y,const char* text,const Font& font,ARGB8 color)
{
  const int len = strlen(text);
  
  for(int i=0;i<len;i++)
  {
    lgDrawGlyph(x,y,font.glyphs[text[i]-32],color);
    x += font.glyphs[text[i]-32].advanceX;
  }
}

void lgDrawTextInRect(Rect r,const char* text,const Font& font,ARGB8 color)
{
  const int x = r.x0 + (r.x1-r.x0 - lgTextWidth(text,font))/2;
  const int y = r.y0 + (r.y1-r.y0 - font.lineHeight)/2;

  lgDrawText(x,y,text,font,color);
}
