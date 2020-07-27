//
//  ViewController.m
//  OpenGL ES 001
//
//  Created by China on 2020/7/27.
//  Copyright © 2020 China. All rights reserved.
//

#import "ViewController.h"
#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>

@interface ViewController (){
    EAGLContext * context;
}

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
    //判断context是否初始化成功
    if (!context) {
        NSLog(@"初始化context失败");
    }
    //状态机设置当前的上下文
    [EAGLContext setCurrentContext:context];
    
    //获取GLKView & 设置context
    GLKView *view = (GLKView *)self.view;
    view.context = context;
    
    //设置背景色
    glClearColor(0, 1, 0, 1);
    
}
#pragma mark — GLKViewDelegate
- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect{
    glClear(GL_COLOR_BUFFER_BIT);
}


@end
