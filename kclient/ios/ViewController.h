#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

@class ViewController;

@interface ViewController : GLKViewController <UIGestureRecognizerDelegate>

+ (ViewController*)getLastInstance;

@end
