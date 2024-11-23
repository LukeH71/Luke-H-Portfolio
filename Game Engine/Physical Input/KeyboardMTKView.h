
// MyCustomMTKView.h


#import <MetalKit/MTKView.h>
#import <Foundation/Foundation.h>


// The integer values for each key
#define keyLength 8
// How many keys you want / 8   ( rounded up )
#define keyLengthBit 1


@interface KeyboardMTKView : MTKView

- (void)initKeys:(uint8_t[keyLength])keyCode;

- (bool)getKey:(uint8_t)index;

- (void)modifyKeyCodesAtIndex:(int)index to:(uint8_t)keyCode;

- (float) getMouseDeltaX;

- (float) getMouseDeltaY;

- (void) clearDeltas;

- (CGPoint) getMousePos;

- (CGFloat) getScrollingWheelDeltaY;

- (void) resetLastKey;

- (UInt16) getLastKey;

@end
