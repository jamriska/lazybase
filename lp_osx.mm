// This software is in the public domain. Where that dedication is not
// recognized, you are granted a perpetual, irrevocable license to copy
// and modify this file as you see fit.

#import <Cocoa/Cocoa.h>

#include <cstdio>

#include "lp.h"

bool modalLoop = false;

bool asyncAppTerminateRequest = false;
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
bool keyDownRepeat[256] = { false };
std::string keyChars;

int wheelDelta = 0;

unsigned int modifierFlags = 0;

std::string fileOpenRequested;

const int vk_to_lpk[128] = {
LP_KEY_A, // kVK_ANSI_A             
LP_KEY_S, // kVK_ANSI_S             
LP_KEY_D, // kVK_ANSI_D             
LP_KEY_F, // kVK_ANSI_F             
LP_KEY_H, // kVK_ANSI_H             
LP_KEY_G, // kVK_ANSI_G             
LP_KEY_Z, // kVK_ANSI_Z             
LP_KEY_X, // kVK_ANSI_X             
LP_KEY_C, // kVK_ANSI_C             
LP_KEY_V, // kVK_ANSI_V             
-1,
LP_KEY_B, // kVK_ANSI_B             
LP_KEY_Q, // kVK_ANSI_Q             
LP_KEY_W, // kVK_ANSI_W             
LP_KEY_E, // kVK_ANSI_E             
LP_KEY_R, // kVK_ANSI_R             
LP_KEY_Y, // kVK_ANSI_Y             
LP_KEY_T, // kVK_ANSI_T             
LP_KEY_1, // kVK_ANSI_1             
LP_KEY_2, // kVK_ANSI_2             
LP_KEY_3, // kVK_ANSI_3             
LP_KEY_4, // kVK_ANSI_4             
LP_KEY_6, // kVK_ANSI_6             
LP_KEY_5, // kVK_ANSI_5             
-1, // kVK_ANSI_Equal         
LP_KEY_9, // kVK_ANSI_9             
LP_KEY_7, // kVK_ANSI_7             
-1, // kVK_ANSI_Minus         
LP_KEY_8, // kVK_ANSI_8             
LP_KEY_0, // kVK_ANSI_0             
-1, // kVK_ANSI_RightBracket  
LP_KEY_O, // kVK_ANSI_O             
LP_KEY_U, // kVK_ANSI_U             
-1, // kVK_ANSI_LeftBracket   
LP_KEY_I, // kVK_ANSI_I             
LP_KEY_P, // kVK_ANSI_P             
LP_KEY_ENTER, // kVK_Return             
LP_KEY_L, // kVK_ANSI_L             
LP_KEY_J, // kVK_ANSI_J             
-1, // kVK_ANSI_Quote         
LP_KEY_K, // kVK_ANSI_K             
-1, // kVK_ANSI_Semicolon     
-1, // kVK_ANSI_Backslash     
-1, // kVK_ANSI_Comma         
-1, // kVK_ANSI_Slash         
LP_KEY_N, // kVK_ANSI_N             
LP_KEY_M, // kVK_ANSI_M             
-1, // kVK_ANSI_Period        
LP_KEY_TAB, // kVK_Tab                
LP_KEY_SPACE, // kVK_Space              
-1, // kVK_ANSI_Grave         
LP_KEY_BACKSPACE, // kVK_Delete             
-1,
LP_KEY_ESCAPE, // kVK_Escape             
-1,
LP_KEY_CONTROL, // kVK_Command            
LP_KEY_SHIFT, // kVK_Shift              
-1, // kVK_CapsLock           
LP_KEY_ALT, // kVK_Option             
-1, // kVK_Control            
LP_KEY_SHIFT, // kVK_RightShift         
-1, // kVK_RightOption        
-1, // kVK_RightControl       
-1, // kVK_Function           
-1, // kVK_F17                
-1, // kVK_ANSI_KeypadDecimal 
-1,
-1, // kVK_ANSI_KeypadMultiply
-1,
-1, // kVK_ANSI_KeypadPlus    
-1,
-1, // kVK_ANSI_KeypadClear   
-1, // kVK_VolumeUp           
-1, // kVK_VolumeDown         
-1, // kVK_Mute               
-1, // kVK_ANSI_KeypadDivide  
-1, // kVK_ANSI_KeypadEnter   
-1,
-1, // kVK_ANSI_KeypadMinus   
-1, // kVK_F18                
-1, // kVK_F19                
-1, // kVK_ANSI_KeypadEquals  
-1, // kVK_ANSI_Keypad0       
-1, // kVK_ANSI_Keypad1       
-1, // kVK_ANSI_Keypad2       
-1, // kVK_ANSI_Keypad3       
-1, // kVK_ANSI_Keypad4       
-1, // kVK_ANSI_Keypad5       
-1, // kVK_ANSI_Keypad6       
-1, // kVK_ANSI_Keypad7       
-1, // kVK_F20                
-1, // kVK_ANSI_Keypad8       
-1, // kVK_ANSI_Keypad9       
-1,
-1,
-1,
-1, // kVK_F5                 
-1, // kVK_F6                 
-1, // kVK_F7                 
-1, // kVK_F3                 
-1, // kVK_F8                 
-1, // kVK_F9                 
-1,
-1, // kVK_F11                
-1,
-1, // kVK_F13                
-1, // kVK_F16                
-1, // kVK_F14                
-1,
-1, // kVK_F10                
-1,
-1, // kVK_F12                
-1,
-1, // kVK_F15                
-1, // kVK_Help               
LP_KEY_HOME, // kVK_Home               
-1, // kVK_PageUp             
LP_KEY_DELETE, // kVK_ForwardDelete      
-1, // kVK_F4                 
LP_KEY_END, // kVK_End                
-1, // kVK_F2                 
-1, // kVK_PageDown           
-1, // kVK_F1                 
LP_KEY_LEFT, // kVK_LeftArrow          
LP_KEY_RIGHT, // kVK_RightArrow         
LP_KEY_DOWN, // kVK_DownArrow          
LP_KEY_UP, // kVK_UpArrow            
-1,
};


id window;
id windowDelegate;
id appDelegate;

@interface LPWindowDelegate : NSObject
@end

//bool waitForEvent;

@implementation LPWindowDelegate

- (BOOL)windowShouldClose:(id)window
{ 
  windowCloseRequested = true;  
  
  return NO;
}

- (void)windowDidResize:(NSNotification *)notification
{
  //[_LPWin.context update];
  //waitForEvent=false;
}

- (void)windowDidMove:(NSNotification *)notification
{
}

- (void)windowDidMiniaturize:(NSNotification *)notification
{
}

- (void)windowDidDeminiaturize:(NSNotification *)notification
{
}

- (void)windowDidBecomeKey:(NSNotification *)notification
{
  NSRect contentRect = [window contentRectForFrameRect:[window frame]];

  NSPoint p = [window mouseLocationOutsideOfEventStream];

  mouseX = p.x;
  mouseY = contentRect.size.height - p.y;

    // windowActivated
}

- (void)windowDidResignKey:(NSNotification *)notification
{
  // XXX!
  // windowDeactivated
}

@end

@interface LPContentView : NSView
@end

CGImageRef image = NULL;
int imageWidth;
int imageHeight;

std::string asyncFileOpenRequest;

@implementation LPContentView

- (id)init 
{
  if (self = [super init]) 
  {
    [self registerForDraggedTypes:[NSArray arrayWithObjects:NSURLPboardType,nil]];
  }
  return self;
}

// XXX rect width and height might be in "points" instead of pixels
- (void)drawRect:(NSRect)rect
{
  if (image!=NULL)
  {
    CGContextRef context = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGContextDrawImage(context,CGRectMake(0,rect.size.height-imageHeight,imageWidth,imageHeight),image);
  }
}

- (BOOL)isOpaque
{
    return YES;
}

- (BOOL)canBecomeKeyView
{
    return YES;
}

- (BOOL)acceptsFirstResponder
{
    return YES;
}

- (BOOL)acceptsFirstMouse:(NSEvent *)event
{
  return YES;
}

- (void)mouseDown:(NSEvent *)event
{
  buttonState[LP_BUTTON_LEFT] = true; 
}

- (void)mouseDragged:(NSEvent *)event
{
  [self mouseMoved:event];
}

- (void)mouseUp:(NSEvent *)event
{
  buttonState[LP_BUTTON_LEFT] = false;
}

- (void)mouseMoved:(NSEvent *)event
{ 
  NSRect contentRect = [window contentRectForFrameRect:[window frame]];

  NSPoint p = [event locationInWindow];

  mouseX = p.x;
  mouseY = contentRect.size.height - p.y;
}

- (void)rightMouseDown:(NSEvent *)event
{
  buttonState[LP_BUTTON_RIGHT] = true; 
}

- (void)rightMouseDragged:(NSEvent *)event
{
  // XXX [self mouseMoved:event] as in mouseDragged ??
}

- (void)rightMouseUp:(NSEvent *)event
{
  buttonState[LP_BUTTON_RIGHT] = false; 
}

- (void)otherMouseDown:(NSEvent *)event
{
}

- (void)otherMouseDragged:(NSEvent *)event
{
}

- (void)otherMouseUp:(NSEvent *)event
{
}

- (void)keyDown:(NSEvent *)event
{
  const unsigned int vk = [event keyCode];  
  if (vk < 128)
  {
    int key = vk_to_lpk[vk];
    if (key!=-1)
    {
      keyState[key] = true; 
      keyDownRepeat[key] = true;      
    }
  }

  keyChars = std::string([[event characters] UTF8String]);
}

- (void)keyUp:(NSEvent *)event
{
  const unsigned int vk = [event keyCode];
  if (vk < 128)
  {
    int key = vk_to_lpk[vk];
    if (key!=-1) { keyState[key] = false; }    
  }
}

- (void)scrollWheel:(NSEvent *)event
{
  // XXX on OSX the delta seems to be smaller compared to WIN !
  wheelDelta += [event deltaY]*120.0f;
}

- (void)flagsChanged:(NSEvent *)event
{
  const unsigned int newModifierFlags = [event modifierFlags] & (NSDeviceIndependentModifierFlagsMask);

  if (newModifierFlags!=modifierFlags)
  {    
    const unsigned int vk = [event keyCode];  
    if (vk < 128)
    {
      const int key = vk_to_lpk[vk];
      if (key!=-1) { keyState[key] = newModifierFlags > modifierFlags ? true : false; }      
    }
    modifierFlags = newModifierFlags;
  }
}

- (NSDragOperation)draggingEntered:(id <NSDraggingInfo>)sender 
{
  return NSDragOperationEvery;
}

- (void)draggingExited:(id <NSDraggingInfo>)sender 
{
}

- (BOOL)prepareForDragOperation:(id <NSDraggingInfo>)sender
{
  return YES;
}

- (BOOL)performDragOperation:(id <NSDraggingInfo>)sender
{
  NSPasteboard* pboard = [sender draggingPasteboard];

  if ([[pboard types] containsObject:NSURLPboardType])
  {
    NSURL* fileURL = [NSURL URLFromPasteboard:pboard];
    NSString* filePath = [fileURL path];

    asyncFileOpenRequest = [filePath UTF8String];

    lpWakeUp();
  }

  return YES;
}

@end


@interface LPApplicationDelegate : NSObject
@end


@implementation LPApplicationDelegate

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender
{
  asyncAppTerminateRequest = true;
  
  lpWakeUp();

  return NSTerminateCancel;
}

- (BOOL)application:(NSApplication *)theApplication openFile:(NSString *)filename
{ 
  asyncFileOpenRequest = [filename UTF8String];

  lpWakeUp();

  return YES;
}

@end

static NSString* findAppName(void)
{
    unsigned int i;
    NSDictionary* infoDictionary = [[NSBundle mainBundle] infoDictionary];

    NSString* LPNameKeys[] =
    {
        @"CFBundleDisplayName",
        @"CFBundleName",
        @"CFBundleExecutable",
    };

    for (i = 0;  i < sizeof(LPNameKeys) / sizeof(LPNameKeys[0]);  i++)
    {
        id name = [infoDictionary objectForKey:LPNameKeys[i]];
        if (name &&
            [name isKindOfClass:[NSString class]] &&
            ![name isEqualToString:@""])
        {
            return name;
        }
    }

    return @"App";
}

void setUpMenuBar()
{
   NSString* appName = findAppName();

      NSMenu* bar = [[NSMenu alloc] init];
    [NSApp setMainMenu:bar];

    NSMenuItem* appMenuItem =
        [bar addItemWithTitle:@"" action:NULL keyEquivalent:@""];
    NSMenu* appMenu = [[NSMenu alloc] init];
    [appMenuItem setSubmenu:appMenu];

    [appMenu addItemWithTitle:[NSString stringWithFormat:@"About %@", appName]
                       action:@selector(orderFrontStandardAboutPanel:)
                keyEquivalent:@""];
    [appMenu addItem:[NSMenuItem separatorItem]];
    NSMenu* servicesMenu = [[NSMenu alloc] init];
    [NSApp setServicesMenu:servicesMenu];
    [[appMenu addItemWithTitle:@"Services"
                       action:NULL
                keyEquivalent:@""] setSubmenu:servicesMenu];
    [appMenu addItem:[NSMenuItem separatorItem]];
    [appMenu addItemWithTitle:[NSString stringWithFormat:@"Hide %@", appName]
                       action:@selector(hide:)
                keyEquivalent:@"h"];
    [[appMenu addItemWithTitle:@"Hide Others"
                       action:@selector(hideOtherApplications:)
                keyEquivalent:@"h"]
        setKeyEquivalentModifierMask:NSAlternateKeyMask | NSCommandKeyMask];
    [appMenu addItemWithTitle:@"Show All"
                       action:@selector(unhideAllApplications:)
                keyEquivalent:@""];
    [appMenu addItem:[NSMenuItem separatorItem]];
    [appMenu addItemWithTitle:[NSString stringWithFormat:@"Quit %@", appName]
                       action:@selector(terminate:)
                keyEquivalent:@"q"];

    NSMenuItem* windowMenuItem =
        [bar addItemWithTitle:@"" action:NULL keyEquivalent:@""];
    NSMenu* windowMenu = [[NSMenu alloc] initWithTitle:@"Window"];
    [NSApp setWindowsMenu:windowMenu];
    [windowMenuItem setSubmenu:windowMenu];

    [windowMenu addItemWithTitle:@"Miniaturize"
                          action:@selector(performMiniaturize:)
                   keyEquivalent:@"m"];
    [windowMenu addItemWithTitle:@"Zoom"
                          action:@selector(performZoom:)
                   keyEquivalent:@""];
    [windowMenu addItem:[NSMenuItem separatorItem]];
    [windowMenu addItemWithTitle:@"Bring All to Front"
                          action:@selector(arrangeInFront:)
                   keyEquivalent:@""];

    [NSApp performSelector:@selector(setAppleMenu:) withObject:appMenu];
}

id pool;
NSModalSession modalSession;

NSDate* dateStart;

//char* appDataDirectory = 0;

void lpInit(void* parentWindow)
{
  for(int button=0;button<3;button++) { buttonState[button] = false; lastButtonState[button] = false; }
  for(int key=0;key<256;key++) { keyState[key] = false; lastKeyState[key] = false; }
  for(int key=0;key<256;key++) { keyDownRepeat[key] = false; }

  pool = [[NSAutoreleasePool alloc] init];
    
  if (!NSApp)
  {
    [NSApplication sharedApplication];

    appDelegate = [[LPApplicationDelegate alloc] init];
    [NSApp setDelegate:appDelegate];

    setUpMenuBar();

    [NSApp finishLaunching];

    // http://stackoverflow.com/questions/8238473/cant-get-nsmousemoved-events-from-nexteventmatchingmask-with-an-nsopenglview  
    ProcessSerialNumber psn;
    GetCurrentProcess(&psn);
    TransformProcessType(&psn,kProcessTransformToForegroundApplication);
    SetFrontProcess(&psn);
  }


  // XXX!   
  dateStart = [[NSDate alloc] init];

  windowDelegate = [[LPWindowDelegate alloc] init];

  // if(windowDelegate == nil) { return false; }

  unsigned int styleMask = 0;

  styleMask = NSTitledWindowMask | NSClosableWindowMask;// | NSMiniaturizableWindowMask; XXX!!
  styleMask |= NSResizableWindowMask;    

  window = [[NSWindow alloc]
            initWithContentRect:NSMakeRect(0,0,800,600)
                      styleMask:styleMask
                        backing:NSBackingStoreBuffered
                          defer:NO];

  [window setContentView:[[LPContentView alloc] init]];
  [window setDelegate:windowDelegate];
  [window setAcceptsMouseMovedEvents:YES];
  [window center];
  [window zoom:nil];

  [window makeKeyAndOrderFront:nil];

  windowWidth = 800;
  windowHeight = 600;
  
  modifierFlags = 0;

//  [NSCursor hide];
  modalLoop = false;
  
  asyncAppTerminateRequest = false;
  asyncFileOpenRequest.clear();

  fileOpenRequested.clear();  
}

void lpBeginModal()
{
  // XXX
  modalSession = [[NSApplication sharedApplication] beginModalSessionForWindow:window];
  modalLoop = true;
}

void lpEndModal()
{ 
  [NSApp endModalSession:modalSession]; 
  modalLoop = false;
}

bool lpEventsPending()
{
  if ([NSApp nextEventMatchingMask:NSAnyEventMask
                         untilDate:[NSDate distantPast]
                            inMode:(modalLoop ? NSModalPanelRunLoopMode : NSDefaultRunLoopMode)
                           dequeue:NO]!=nil)
  {
    return true;
  }

  return false;
}

void lpWaitForEvent()
{  
  if (lpEventsPending())
  {
    return;
  }

  NSEvent* event = [NSApp nextEventMatchingMask:NSAnyEventMask
                                      untilDate:[NSDate distantFuture]
                                         inMode:(modalLoop ? NSModalPanelRunLoopMode : NSDefaultRunLoopMode)
                                        dequeue:YES];
  
  if (event) [NSApp postEvent:event atStart:YES];
}

void lpUpdate()
{
  //windowCloseRequested = false;
  windowCloseRequested = false || asyncAppTerminateRequest;
  asyncAppTerminateRequest = false;  

  fileOpenRequested.clear();
  
  if (!asyncFileOpenRequest.empty())
  {
    fileOpenRequested = asyncFileOpenRequest;
    asyncFileOpenRequest.clear();
  }

  windowResized = false;

  NSRect contentRect = [window contentRectForFrameRect:[window frame]];
  windowWidth = contentRect.size.width;
  windowHeight = contentRect.size.height;

  for(int button=0;button<3;button++) { lastButtonState[button] = buttonState[button]; }
  
  for(int key=0;key<256;key++) { lastKeyState[key] = keyState[key]; }
  for(int key=0;key<256;key++) { keyDownRepeat[key] = false; }
  keyChars.clear();

  wheelDelta = 0;
  
  // XXX
  //[[window contentView]setNeedsDisplay:YES];
  
  while(1)
  {
    NSEvent* event = [NSApp nextEventMatchingMask:NSAnyEventMask
                                        untilDate:[NSDate distantPast]
                                           inMode:(modalLoop ? NSModalPanelRunLoopMode : NSDefaultRunLoopMode)
                                          dequeue:YES];

    if (event==nil) break;

    if (modalLoop==true)
    {
      NSWindow* eventWindow = [event window];
      
      if (!(eventWindow == window || [eventWindow worksWhenModal] == YES || [event type] == NSMouseMoved || [event type] == NSAppKitDefined))
      {
        continue;
      }
    }

    [NSApp sendEvent:event];

    NSEventType eventType = [event type];
    
    if (eventType == NSLeftMouseDown  ||
        eventType == NSLeftMouseUp    ||
        eventType == NSRightMouseDown ||
        eventType == NSRightMouseUp   ||
        eventType == NSKeyDown        ||
        eventType == NSKeyUp          ||
        eventType == NSFlagsChanged)
    {
      break;
    }
  }
     
  [pool drain];
  pool = [[NSAutoreleasePool alloc] init];
}

void lpCleanUp()
{
  //[NSApp endModalSession:modalSession];

  // release windowDelegate?
  // release appDelegate?

  asyncFileOpenRequest.clear();
 
  fileOpenRequested.clear();

  [window orderOut:nil];
  [window release];
  [pool release]; 
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

void lpSetWindowTitle(const std::string& text)
{
  [window setTitle:[NSString stringWithUTF8String:text.c_str()]];
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

int lpMouseWheelDelta()
{
  return wheelDelta;
}

void lpBlit(int width,int height,unsigned char* data)
{
  CGColorSpaceRef cs = CGColorSpaceCreateDeviceRGB();

  CGContextRef bitmapCtx = CGBitmapContextCreate(data,width,height,8,width*4,cs,kCGBitmapByteOrder32Little | kCGImageAlphaNoneSkipFirst);

  CGColorSpaceRelease(cs);

  if (image!=NULL)
  {
    CGImageRelease(image);
    image = NULL;
  }

  image = CGBitmapContextCreateImage(bitmapCtx);
  imageWidth = width;
  imageHeight = height;

  CFRelease(bitmapCtx);

  [[window contentView]setNeedsDisplay:YES];
}

void lpSetCursor(LPCursor cursor)
{
  [(NSCursor*)cursor set];
}

LPCursor lpMakeCursor(const void* data,int width,int height,int hotspotX,int hotspotY)
{  
  CGColorSpaceRef cs = CGColorSpaceCreateDeviceRGB();

  // XXX really premultiplied?
  CGContextRef bitmapCtx = CGBitmapContextCreate((void*)data,width,height,8,width*4,cs,kCGBitmapByteOrder32Little | kCGImageAlphaPremultipliedFirst);

  CGColorSpaceRelease(cs);

  CGImageRef cgImage = CGBitmapContextCreateImage(bitmapCtx);

  NSImage* image = [[NSImage alloc] initWithCGImage:cgImage size:NSZeroSize];

  NSCursor* cursor = [[NSCursor alloc] initWithImage:image hotSpot:NSMakePoint(hotspotX,hotspotY)];

  // XXX release nsimage!
  // XXX release cgimage!
  // XXX possibly release other things..
  //NSCursor* cursor = [NSCursor openHandCursor];

  return (LPCursor)cursor;
}

LPCursor lpArrowCursor()
{
  return [NSCursor arrowCursor];
}

int lpGetTime()
{
  return (-[dateStart timeIntervalSinceNow]*1000.0);
}

// XXX this should work from any thread
void lpWakeUp()
{
  NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
  
    NSEvent* event = [NSEvent otherEventWithType: NSApplicationDefined
                                        location: NSMakePoint(0,0)
                                   modifierFlags: 0
                                       timestamp: 0.0
                                    windowNumber: 0
                                         context: nil
                                         subtype: 0
                                           data1: 0
                                           data2: 0];

  [NSApp postEvent:event
           atStart:NO];

  [pool drain];

  /*
  [NSTimer scheduledTimerWithTimeInterval: 0
            target: window
            selector: nil
            userInfo: nil
            repeats: NO];
*/
//  PostMessage(window,(WM_USER+0),0,0);
}

void processTimer(CFRunLoopTimerRef theTimer,void *data)
{
  lpWakeUp();
}

void lpWakeUpAfter(int time)
{
  CFGregorianUnits gumilli ;
  memset(&gumilli,0,sizeof(gumilli) );
  gumilli.seconds = double(time) / 1000.0;

  CFRunLoopTimerContext ctx ;
  memset( &ctx, 0 , sizeof(ctx) );
  ctx.version = 0;
  ctx.info = nil;



  CFRunLoopRef runLoop = 0;
  //runLoop = CFRunLoopGetCurrent();
  runLoop = CFRunLoopGetMain();
  CFRunLoopAddTimer( runLoop, CFRunLoopTimerCreate(kCFAllocatorDefault,
                                                   CFAbsoluteTimeAddGregorianUnits( CFAbsoluteTimeGetCurrent() , NULL, gumilli ),
                                                   0/*IsOneShot() ? 0 : CFTimeInterval( double(time) / 1000.0 ) */,
                                                   0, 0, processTimer, &ctx), kCFRunLoopCommonModes);

  /*
  [[NSTimer alloc] scheduledTimerWithTimeInterval: (((double)time)/1000.0)
            target: nil
            selector: nil
            userInfo: nil
            repeats: NO];  
  */
//  SetTimer(window,NULL,time,NULL);
}


std::string lpFileOpenDialog()
{
  std::string fileName;

  NSOpenPanel* openPanel = [NSOpenPanel openPanel];

  //NSArray* fileTypesArray;
  //fileTypesArray = [NSArray arrayWithObjects:@"jpg", @"gif", @"png", nil];

  [openPanel setCanChooseDirectories:NO];
  [openPanel setCanChooseFiles:YES];
  [openPanel setAllowsMultipleSelection:NO];

  
  if ([openPanel runModal]==NSOKButton)
  {
    fileName = [[[[openPanel URLs] objectAtIndex:0] path] UTF8String];
  }

  [window makeKeyAndOrderFront:nil];
  
  lpWakeUp();

  return fileName;
}

std::string lpFileSaveDialog()
{ 
  std::string fileName;

  NSSavePanel* savePanel = [NSSavePanel savePanel];

  //NSArray* fileTypesArray;
  //fileTypesArray = [NSArray arrayWithObjects:@"jpg", @"gif", @"png", nil];

  //[savePanel setCanChooseDirectories:NO];
  //[savePanel setCanChooseFiles:YES];
  //[savePanel setAllowsMultipleSelection:NO];

  if ([savePanel runModal]==NSOKButton)
  {
    fileName = [[[savePanel URL] path] UTF8String];
  }

  [window makeKeyAndOrderFront:nil];
  
  lpWakeUp();

  return fileName;
}

bool lpMouseButtonDoubleClicked(int button)
{
  return false;
}

void lpMessageBox(const std::string& text)
{
  NSAlert* alert = [[NSAlert alloc] init];
  [alert setMessageText: [NSString stringWithUTF8String:text.c_str()]];
  [alert runModal];
  [alert release];

  [window makeKeyAndOrderFront:nil];
}

std::string lpFileOpenRequested()
{
  return fileOpenRequested;
}

std::string lpClipBoardText()
{
  std::string text;

  NSString* string = [[NSPasteboard generalPasteboard] stringForType:NSPasteboardTypeString];
  
  if (string!=nil)
  {
    text = [string UTF8String];
  }

  return text;      
}

void lpOpenURL(const std::string& url)
{
  [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:[NSString stringWithUTF8String:url.c_str()]]];
}

bool lpDirExists(const std::string& path)
{
  BOOL isDir;
  if ([[NSFileManager defaultManager] fileExistsAtPath:[NSString stringWithUTF8String:path.c_str()] isDirectory:&isDir] && isDir) { return true; }
  return false;
}

void lpMakeDir(const std::string& path)
{
  [[NSFileManager defaultManager] createDirectoryAtPath:[NSString stringWithUTF8String:path.c_str()] withIntermediateDirectories:NO attributes:nil error:NULL];
}

std::string lpAppDataDir(const std::string& appName)
{
  std::string dir;

  NSArray* paths = NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory,NSUserDomainMask,YES);

  if ([paths count]>0)
  {
    std::string path = [[paths objectAtIndex:0] UTF8String];
    dir = path+"/"+appName; 
    if (!lpDirExists(dir)) { lpMakeDir(dir); }
  }
     
  return dir;
}

std::string lpMakeTempFile()
{
  NSString* tempFileTemplate = [NSTemporaryDirectory() stringByAppendingPathComponent:@"LZY.XXXXXXXX"];
  const char* tempFileTemplateCString = [tempFileTemplate fileSystemRepresentation];
  char* tempFileNameCString = (char*)malloc(strlen(tempFileTemplateCString) + 1);
  strcpy(tempFileNameCString,tempFileTemplateCString);
  
  std::string tempFileName;
  if (mktemp(tempFileNameCString)!=0) { tempFileName = std::string(tempFileNameCString); }
  free(tempFileNameCString);

  return tempFileName;
}

FILE* lp_fopen(const std::string& filename,const std::string& mode)
{ 
  return fopen(filename.c_str(),mode.c_str());
}
