

// MyCustomMTKView.m
#import "KeyboardMTKView.h"
#include <iostream>

@implementation KeyboardMTKView
{
    uint8_t keys[keyLengthBit];
    uint8_t keysRealTime[keyLengthBit];
    uint8_t keyCodes[keyLength];
    
    CGFloat deltaX;
    CGFloat deltaY;
    
    CGPoint mousePoint;
    
    CGFloat scrollingDeltaY;
    
    UInt16 lastKey;
}

// https://eastmanreference.com/complete-list-of-applescript-key-codes


#pragma mark Initializers, Getters & Setters

-(nonnull instancetype)init;
{
    
    self = [super init];
    if(self)
    {
        
        for(int i = 0; i<keyLengthBit; i++)
            keys[i]=0;
        
        for(int i = 0; i<keyLengthBit; i++)
            keysRealTime[i]=0;
        
        
        deltaX = 0;
        deltaY = 0;
        scrollingDeltaY = 0;
        lastKey = 1000;
        
    }

    return self;
    
}


// Initialize the keys and keyCodes variables
- (void)initKeys:(uint8_t[keyLength])keyCode {
    
    for(int i = 0; i<keyLength; i++)
        keyCodes[i]=keyCode[i];
    
}

// Getters and setters below should be self explenitory

- (void)modifyKeyCodesAtIndex:(int)index to:(uint8_t)keyCode {
    keyCodes[index] = keyCode;
}

- (float) getMouseDeltaX {
    return deltaX;
}

- (float) getMouseDeltaY {
    return deltaY;
}

- (void) clearDeltas {
    deltaX = 0;
    deltaY = 0;
}

// A getter for the mouse position, but inside Metal coodinates for ease of use
- (CGPoint) getMousePos {
    CGPoint metalMousePos = mousePoint;
    NSSize windowFrame = self.window.frame.size;
    
    // Handle the title bar being included inside of the window frame bounds
    if( (self.window.styleMask & NSWindowStyleMaskFullScreen) != NSWindowStyleMaskFullScreen )
        windowFrame.height -= 30;
    
    metalMousePos.x = (metalMousePos.x / windowFrame.width) * 2.0 - 1.0;
    metalMousePos.y = (metalMousePos.y / windowFrame.height) * 2.0 - 1.0;
    
    return metalMousePos;
}

- (CGFloat) getScrollingWheelDeltaY {
    return scrollingDeltaY;
}

- (void) resetLastKey {
    lastKey = 1000;
}

- (UInt16) getLastKey {
    return lastKey;
}

// Get the key using the specified index
- (bool) getKey:(uint8_t) index {
    
    bool temp = keys[index / 8] & (1 << (index % 8));
    
    if(!(keysRealTime[index / 8] & (1 << (index % 8))) && temp) {
        keys[index / 8] &= ~(1 << (index % 8));
    }
    
    return temp;
}




#pragma mark Handeling Keyboard Input



// Keyboard input for regular keys (ex. K/J)
- (void)keyDown:(NSEvent *)event {
    lastKey = event.keyCode;
    for(uint8_t i = 0; i<keyLength; i++) {
        if(keyCodes[i]==event.keyCode) {
            
            keys[i / 8] |= 1 << (i % 8);
            keysRealTime[i / 8] |= 1 << (i % 8);
            break;
        }
    }
}

- (void)keyUp:(NSEvent *)event {
    for(uint8_t i = 0; i<keyLength; i++) {
        if(keyCodes[i]==event.keyCode) {
            keysRealTime[i / 8] &= ~(1 << (i % 8));
            break;
        }
    }
}

// Map key codes to their respected modifier flags
NSInteger getModifierFlag(short keyCode) {
    switch (keyCode) {
        case 57:
            return(NSEventModifierFlagCapsLock);
        case 56:
            return(NSEventModifierFlagShift);
        case 60:
            return(NSEventModifierFlagShift);
        case 59:
            return(NSEventModifierFlagControl);
        case 62:
            return(NSEventModifierFlagControl);
        case 58:
            return(NSEventModifierFlagOption);
        case 61:
            return(NSEventModifierFlagOption);
        case 55:
            return(NSEventModifierFlagCommand);
        default:
            return(0);
    }
}

// Keyboard input for modifier keys (ex. Shift/Crtl)
- (void)flagsChanged:(NSEvent *)event {
    lastKey = event.keyCode;
    for(uint8_t i = 0; i<keyLength; i++) {
        if(keyCodes[i]==event.keyCode) {
            if(event.modifierFlags & getModifierFlag(event.keyCode)) {
                keys[i / 8] |= 1 << (i % 8);
                keysRealTime[i / 8] |= 1 << (i % 8);
            } else {
                keysRealTime[i / 8] &= ~(1 << (i % 8));
            }
            break;
        }
    }
    
}




#pragma mark Handeling Mouse Clicks


// Base code for all mouse button down events
- (void)mouseButtonDown:(NSInteger) buttonNumber {
    lastKey = buttonNumber;
    for(uint8_t i = 0; i<keyLength; i++) {
        if(keyCodes[i]==buttonNumber) {
            keys[i / 8] |= 1 << (i % 8);
            keysRealTime[i / 8] |= 1 << (i % 8);
            return;
        }
    }
}

// Base code for all mouse button up events
- (void)mouseButtonUp:(NSInteger) buttonNumber {
    for(uint8_t i = 0; i<keyLength; i++) {
        if(keyCodes[i]==buttonNumber) {
            keysRealTime[i / 8] &= ~(1 << (i % 8));
            return;
        }
    }
}

// 200: left mouse button
- (void)mouseDown:(NSEvent *)event {
    [self mouseButtonDown: event.buttonNumber+200];
}

// 200: left mouse button
-(void)mouseUp:(NSEvent *)event {
    [self mouseButtonUp: event.buttonNumber+200];
}

// 201: right mouse button
- (void)rightMouseDown:(NSEvent *)event {
    [self mouseButtonDown: event.buttonNumber+200];
}

// 201: right mouse button
-(void)rightMouseUp:(NSEvent *)event {
    [self mouseButtonUp: event.buttonNumber+200];
}

// 202 - 255: 202 = middle, rest is extra buttons
- (void)otherMouseDown:(NSEvent *)event {
    [self mouseButtonDown: event.buttonNumber+200];
}

// 202 - 255: 202 = middle, rest is extra buttons
-(void)otherMouseUp:(NSEvent *)event {
    [self mouseButtonUp: event.buttonNumber+200];
}


#pragma mark Other Mouse Events



// Mouse moving
- (void)mouseMoved:(NSEvent *)event {
    deltaX += event.deltaX;
    deltaY += event.deltaY;
    
    
    mousePoint = event.locationInWindow;
}

- (void)mouseDragged:(NSEvent *)event {
    mousePoint = event.locationInWindow;
}
- (void)rightMouseDragged:(NSEvent *)event {
    mousePoint = event.locationInWindow;
}
- (void)otherMouseDragged:(NSEvent *)event {
    mousePoint = event.locationInWindow;
}


// Scroll Wheel
- (void)scrollWheel:(NSEvent *)event {
    scrollingDeltaY += event.scrollingDeltaY;
}







// Allow keyboard input
- (BOOL)acceptsFirstResponder {
    return YES;
}

@end








//[NSCursor hide];
//CGAssociateMouseAndMouseCursorPosition(false);

//NSRect windowFrame = self.window.frame;
//NSRect screenFrame = self.window.screen.frame;
//
//CGPoint center = CGPointMake(windowFrame.origin.x + windowFrame.size.width / 2.0,
//                             NSHeight(screenFrame) - windowFrame.origin.y - windowFrame.size.height / 2.0);
//
//if( !NSEqualRects(windowFrame, screenFrame) ) center.y += 15 ;
//
//CGWarpMouseCursorPosition(center);
//
//
////[NSCursor hide];
////CGAssociateMouseAndMouseCursorPosition(false);
