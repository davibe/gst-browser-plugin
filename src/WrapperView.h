//
//  WrapperView.h
//  player
//
//  Created by Davide Bertola on 5/6/10.
//  Copyright 2010 Polito. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <Quartz/Quartz.h>
#import "fullscreen-window.h"

@interface WrapperView : NSView {
  NSView *parent_view;
  BOOL full_screen;
  gpointer instance;
}
- (id) initWithFrame:(NSRect)frameRect fromInstance:(gpointer)instance;
- (BOOL) acceptsFirstResponder;
- (void) mouseDown:(NSEvent*)theEvent;
@end
