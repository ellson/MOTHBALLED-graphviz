//
//  GVTextLayout.h
//  
//
//  Created by Glen Low on 3/01/09.
//  Copyright 2009 Pixelglow Software. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface GVTextLayout : NSObject
{
	UIFont* _font;
	NSString* _text;
}

@property (readonly, nonatomic) UIFont* font;
@property (readonly, nonatomic) NSString* text;
@property (readonly, nonatomic) CGSize size;

- (id)initWithFontName:(char*)fontName fontSize:(CGFloat)fontSize text:(char*)text;

- (void)drawAtPoint:(CGPoint)point inContext:(CGContextRef)context;

- (void)dealloc;

@end
