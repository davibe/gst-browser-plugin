#import "WrapperView.h"
#import "fullscreen-window.h"

@implementation WrapperView

- (id) initWithFrame:(NSRect)frameRect fromInstance:(gpointer) inst {
  id result;
  
  self->instance = inst;
  self->full_screen = FALSE; 
  result = [super initWithFrame:frameRect];
  [self setAutoresizesSubviews: YES];
  [self setAutoresizingMask:2|16];
  
  return result;
}

- (BOOL)acceptsFirstResponder{
  return NO;
}

- (void)mouseDown:(NSEvent*)theEvent {
  if ([theEvent clickCount] < 2) 
    return;
  
  fullscreen_window_emit_clicked_signal(self->instance);
}

@end
