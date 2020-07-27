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
    GLKBaseEffect *cEffect;
}

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    [self setUpConfig];
    [self setUpTexture];
    [self setUpVertexData];
    
}
// 初始化OpenGL ES
- (void)setUpConfig{
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
    
    /*3.配置视图创建的渲染缓存区.
    
    (1). drawableColorFormat: 颜色缓存区格式.
    简介:  OpenGL ES 有一个缓存区，它用以存储将在屏幕中显示的颜色。你可以使用其属性来设置缓冲区中的每个像素的颜色格式。
    
    GLKViewDrawableColorFormatRGBA8888 = 0,
    默认.缓存区的每个像素的最小组成部分（RGBA）使用8个bit，（所以每个像素4个字节，4*8个bit）。
    
    GLKViewDrawableColorFormatRGB565,
    如果你的APP允许更小范围的颜色，即可设置这个。会让你的APP消耗更小的资源（内存和处理时间）
    
    (2). drawableDepthFormat: 深度缓存区格式
    
    GLKViewDrawableDepthFormatNone = 0,意味着完全没有深度缓冲区
    GLKViewDrawableDepthFormat16,
    GLKViewDrawableDepthFormat24,
    如果你要使用这个属性（一般用于3D游戏），你应该选择GLKViewDrawableDepthFormat16
    或GLKViewDrawableDepthFormat24。这里的差别是使用GLKViewDrawableDepthFormat16
    将消耗更少的资源
    
    */
    view.drawableColorFormat = GLKViewDrawableColorFormatRGBA8888;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat16;

    //设置背景色
    glClearColor(0, 1, 0, 1);
}

- (void)setUpVertexData{
    GLfloat vertexData[] = {
        -0.5, 0.5, 0.0, 0.0, 1.0,
        0.5, 0.5, 0.0, 1.0, 1.0,
        0.5, -0.5, 0.0, 1.0, 0.0,
        
        -0.5, 0.5, 0.0, 0.0, 1.0,
        0.5, -0.5, 0.0, 1.0, 0.0,
        -0.5, -0.5, 0.0, 0.0, 0.0,
    };
    
    //开辟顶点缓冲区
    //1.创建顶点缓存区标志符ID
    GLuint bufferID;
    glGenBuffers(1, &bufferID);
    //2.绑定顶点缓存区
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);
    //3.将顶点数组的顶点数据copy到顶点缓冲区中（GPU）
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
    
    //顶点坐标数据
    /*
      (1)在iOS中, 默认情况下，出于性能考虑，所有顶点着色器的属性（Attribute）变量都是关闭的.
      意味着,顶点数据在着色器端(服务端)是不可用的. 即使你已经使用glBufferData方法,将顶点数据从内存拷贝到顶点缓存区中(GPU显存中).
      所以, 必须由glEnableVertexAttribArrays 方法打开通道.指定访问属性.才能让顶点着色器能够访问到从CPU复制到GPU的数据.
      注意: 数据在GPU端是否可见，即，着色器能否读取到数据，由是否启用了对应的属性决定，这就是glEnableVertexAttribArray的功能，允许顶点着色器读取GPU（服务器端）数据。
    
     (2)方法简介
     glVertexAttribPointer (GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* ptr)
    
     功能: 上传顶点数据到显存的方法（设置合适的方式从buffer里面读取数据）
     参数列表:
         index,指定要修改的顶点属性的索引值,例如
         size, 每次读取数量。（如position是由3个（x,y,z）组成，而颜色是4个（r,g,b,a）,纹理则是2个.）
         type,指定数组中每个组件的数据类型。可用的符号常量有GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT,GL_UNSIGNED_SHORT, GL_FIXED, 和 GL_FLOAT，初始值为GL_FLOAT。
         normalized,指定当被访问时，固定点数据值是否应该被归一化（GL_TRUE）或者直接转换为固定点值（GL_FALSE）
         stride,指定连续顶点属性之间的偏移量。如果为0，那么顶点属性会被理解为：它们是紧密排列在一起的。初始值为0
         ptr指定一个指针，指向数组中第一个顶点属性的第一个组件。初始值为0
      */
    // 顶点坐标数据
    glEnableVertexAttribArray(GLKVertexAttribPosition);
    glVertexAttribPointer(GLKVertexAttribPosition, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 5, (GLfloat *)NULL + 0);
    
    // 纹理坐标数据
    glEnableVertexAttribArray(GLKVertexAttribTexCoord0);
    glVertexAttribPointer(GLKVertexAttribTexCoord0, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 5, (GLfloat *)NULL + 3);
    
}
- (void)setUpTexture{
    //1.获取纹理图片路径
    NSString * path = [[NSBundle mainBundle] pathForResource:@"myDesk" ofType:@"jpg"];
    
    //2.设置纹理参数
    //纹理坐标原点是左下角,但是图片显示原点应该是左上角.
    NSDictionary *options = [NSDictionary dictionaryWithObjectsAndKeys:@(1), GLKTextureLoaderOriginBottomLeft, nil];
    
    GLKTextureInfo *textureInfo = [GLKTextureLoader textureWithContentsOfFile:path options:options error:nil];
    
    // 使用苹果GLKit提供GLKBaseEffect 完成着色器工作(顶点/片元
    cEffect = [[GLKBaseEffect alloc] init];
    cEffect.texture2d0.enabled = GL_TRUE;
    cEffect.texture2d0.name = textureInfo.name;
}
#pragma mark — GLKViewDelegate
- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect{
    glClear(GL_COLOR_BUFFER_BIT);
    
    [cEffect prepareToDraw];
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
}


@end
