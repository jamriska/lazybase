// This software is in the public domain. Where that dedication is not
// recognized, you are granted a perpetual, irrevocable license to copy
// and modify this file as you see fit.

#ifndef LAZY_GRAPHICS_H_
#define LAZY_GRAPHICS_H_

typedef unsigned int uint32_t;

typedef unsigned char uchar;
typedef uint32_t ARGB8;

inline ARGB8 Color(uchar a,uchar r,uchar g,uchar b)
{
  return (a << 24) | (r << 16) | (g << 8) | (b << 0);
}

inline ARGB8 Color(uchar r,uchar g,uchar b)
{
  return Color(255,r,g,b);
}

struct Rect
{
  Rect(int x0,int y0,int x1,int y1) : x0(x0),y0(y0),x1(x1),y1(y1) { }

  int  x0;
  int  y0;
  int  x1;
  int  y1;
};

inline Rect RectWH(int x,int y,int w,int h)
{
  return Rect(x,y,x+w,y+h);
}

class LBitmap
{
public:
  LBitmap();
  LBitmap(const LBitmap& b);
  LBitmap(const LBitmap& b,const Rect& r);
  LBitmap& operator=(const LBitmap& b);
  LBitmap(int width,int height);
  LBitmap(int width,int height,int stride,ARGB8* data);
  ~LBitmap();

  int width() const;
  int height() const;
  int stride() const;
  ARGB8* data() const;

  ARGB8& operator()(int x,int y);
  const ARGB8& operator()(int x,int y) const;

private:
  int    w;  // width
  int    h;  // height
  int    s;  // stride
  ARGB8* d;  // data - this is what data() returns
  ARGB8* rd; // ref data - this ptr gets deleted when rc drops to 0
  int*   rc; // ref count
};

struct Glyph
{
  int width;
  int height;
  int offsetX;
  int offsetY;
  int advanceX;
  const unsigned char* data;
};

struct Font
{
  int   lineHeight;
  int   base;
  Glyph glyphs[95];
};

LBitmap lgSubBitmap(LBitmap b,Rect r);

void lgResampleNearest(LBitmap src,float u0,float v0,float dudx,float dudy,float dvdx,float dvdy,ARGB8 bkg,LBitmap* dst);
void lgResampleBilinear(LBitmap src,float u0,float v0,float dudx,float dudy,float dvdx,float dvdy,ARGB8 bkg,LBitmap* dst);

void lgPushTarget(LBitmap b,Rect clip);
void lgPopTarget();
LBitmap lgGetTarget();

void lgPushClipRect(Rect r);
void lgPopClipRect();
Rect lgGetClipRect();

void lgPutPixel(int x,int y,ARGB8 c);
ARGB8 lgGetPixel(int x,int y);

void lgDrawHLine(int y,int x0,int x1,ARGB8 c);
void lgDrawVLine(int x,int y0,int y1,ARGB8 c);

void lgDrawHLine(int y,int x0,int x1,ARGB8 c0,ARGB8 c1);
void lgDrawVLine(int x,int y0,int y1,ARGB8 c0,ARGB8 c1);

void lgDrawRectOutline(Rect r,ARGB8 c);
void lgDrawRectSolid(Rect r,ARGB8 c);
void lgDrawRectSolid(Rect r,ARGB8 c00,ARGB8 c10,ARGB8 c01,ARGB8 c11);

void lgDrawCircle(int x,int y,int r,ARGB8 c);
void lgDrawDoubleCircle(int x,int y,int r,ARGB8 cout,ARGB8 cin);

void lgDrawBitmap(int x,int y,LBitmap b);
void lgDrawBitmapOpaque(int x,int y,LBitmap b);

void lgDrawNinePatch(Rect r,LBitmap patch);

int lgTextWidth(const char* text,const Font& font);

void lgDrawGlyph(int x,int y,const Glyph& glyph,ARGB8 color);
void lgDrawText(int x,int y,const char* text,const Font& font,ARGB8 color);
void lgDrawTextInRect(Rect r,const char* text,const Font& font,ARGB8 color);

#endif
