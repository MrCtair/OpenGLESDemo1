//
//  CCView.h
//  OpenGL ES 001
//
//  Created by China on 2020/7/29.
//  Copyright © 2020 China. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <OpenGLES/ES2/gl.h>

NS_ASSUME_NONNULL_BEGIN

@interface CCView : UIView
@property (nonatomic, strong) CAEAGLLayer * myEagLayer;

@property(nonatomic,strong) EAGLContext *myContext;

@property(nonatomic,assign) GLuint myColorRenderBuffer;
@property(nonatomic,assign) GLuint myColorFrameBuffer;

@property(nonatomic,assign) GLuint myPrograme;
@end
@implementation CCView
- (void)layoutSubviews{
    [self setupLayer];
    [self setupContext];
    [self deletRenderBufferAndFrameBuffer];
    [self creatRenderBuffer];
    [self creatFrameBuffer];
}

+ (Class)layerClass{
    return [CAEAGLLayer class];
}

- (void)renderLayer{
    //
    glClearColor(0, 0, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    
    CGFloat scale = [[UIScreen mainScreen] scale];
    
    glViewport(self.frame.origin.x * scale, self.frame.origin.y * scale, self.frame.size.width * scale, self.frame.size.height * scale);
    
    NSString *vertPath = [[NSBundle mainBundle] pathForResource:@"shaderv" ofType:@"vsh"];
    
    NSString *fragPath = [[NSBundle mainBundle] pathForResource:@"shaderv" ofType:@"fsh"];
    
    self.myPrograme = [self loadShader:vertPath WithFrag:fragPath];
    
    // pragram链接
    glLinkProgram(self.myPrograme);
    
    GLint linkStatus;
    // 获取链接转台
    
    glGetProgramiv(self.myPrograme, GL_LINK_STATUS, &linkStatus);
    
    if (linkStatus == GL_FALSE) {
        // 获取link失败信息
        GLchar message[512];
        
        glGetProgramInfoLog(self.myPrograme, sizeof(message), 0, &message[0]);
        
        NSString * messageString = [NSString stringWithUTF8String:message];
        
        NSLog(@"Pragram Link Error: %@", messageString);
        
        return;
        
    }
    
    glUseProgram(self.myPrograme);
    
    // 准备顶点数据
    // 2个三角形构成~
    //前3个是顶点坐标，后2个是纹理坐标
    GLfloat attrArr[] ={
        0.5f, -0.5f, -1.0f,     1.0f, 0.0f,
        -0.5f, 0.5f, -1.0f,     0.0f, 1.0f,
        -0.5f, -0.5f, -1.0f,    0.0f, 0.0f,
        
        0.5f, 0.5f, -1.0f,      1.0f, 1.0f,
        -0.5f, 0.5f, -1.0f,     0.0f, 1.0f,
        0.5f, -0.5f, -1.0f,     1.0f, 0.0f,
    };
    
    // 顶点 -> 顶点缓存区
    GLuint attrBuffer;
    glGenBuffers(1, &attrBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, attrBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(attrBuffer), attrBuffer, GL_DYNAMIC_DRAW);
    
    
    //打开通道
    // 获取顶点数据通道ID
    GLuint position = glGetAttribLocation(self.myPrograme, "position");
    
    //打开
    glEnableVertexAttribArray(position);
    
    glVertexAttribPointer(position, 3, GL_FLOAT, false, sizeof(GL_FLOAT) * 5, NULL);
    
    //代开纹理坐标通道
    GLuint textCoordinate = glGetAttribLocation(self.myPrograme, "textCoordinate");
    
    //打开
    glEnableVertexAttribArray(textCoordinate);
    
    glVertexAttribPointer(textCoordinate, 2, GL_FLOAT, false, sizeof(GL_FLOAT) * 5, NULL + 3);
    
    // 加载纹理
    [self setUpTexture:@"myDesk"];
    
    //
    
    glUniform1i(glGetUniformLocation(self.myPrograme, "colorMap"), 0);
    
    //绘制
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    //
    [self.myContext presentRenderbuffer:GL_RENDERBUFFER];
    
}
// 设置纹理
- (GLuint)setUpTexture:(NSString *)fileName{
    
    // 1.纹理解压缩 coregraphic
    CGImageRef spriteImage = [UIImage imageNamed:fileName].CGImage;
    
    if (!spriteImage) {
        NSLog(@"Faild to Load Image~");
        exit(1);
    }
    
    size_t with = CGImageGetWidth(spriteImage);
    size_t height = CGImageGetHeight(spriteImage);
    GLubyte * spriteData = (GLubyte *)calloc(with * height * 4, sizeof(GLubyte));
    /*
    参数1：data,指向要渲染的绘制图像的内存地址
    参数2：width,bitmap的宽度，单位为像素
    参数3：height,bitmap的高度，单位为像素
    参数4：bitPerComponent,内存中像素的每个组件的位数，比如32位RGBA，就设置为8
    参数5：bytesPerRow,bitmap的没一行的内存所占的比特数
    参数6：colorSpace,bitmap上使用的颜色空间  kCGImageAlphaPremultipliedLast：RGBA
    */
    CGContextRef spriteContext = CGBitmapContextCreate(spriteData, with, height, 8, with * 4, CGImageGetColorSpace(spriteImage), kCGImageAlphaPremultipliedLast);
    
    CGRect rect = CGRectMake(0, 0, with, height);
    
    CGContextDrawImage(spriteContext, rect, spriteImage);
    
    CGContextRelease(spriteContext);
    
    // 纹理
    
    glGenTextures(GL_TEXTURE_2D, 0);
    
    //设置纹理属性
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    float fw = with, fh = height;
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fw, fh, 0, GL_RGBA, GL_UNSIGNED_BYTE, spriteData);
    
    //11.释放spriteData
    free(spriteData);
    return 0;
}
- (void)creatFrameBuffer{
    GLuint buffer;
    
    glGenFramebuffers(1, &buffer);
    
    self.myColorFrameBuffer = buffer;
    
    glBindFramebuffer(GL_FRAMEBUFFER, self.myColorFrameBuffer);
    
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, self.myColorFrameBuffer);
}

// 创建渲染缓存区
- (void)creatRenderBuffer{
    //定义一个bufferID
    GLuint buffer;
    
    //绑定
    glGenRenderbuffers(1, &buffer);
    
    self.myColorRenderBuffer = buffer;
    
    glBindRenderbuffer(GL_RENDERBUFFER, self.myColorRenderBuffer);
    
    [self.myContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:self.myEagLayer];
    
    
    
}
//清空缓存区
- (void)deletRenderBufferAndFrameBuffer{
    //
    // 颜色缓存区，深度缓存区, 模板缓存区
    glDeleteRenderbuffers(1, &_myColorRenderBuffer);
    self.myColorRenderBuffer = 0;
    
    glDeleteFramebuffers(1, &_myColorFrameBuffer);
    self.myColorRenderBuffer = 0;
}
// 设置上下文
- (void)setupContext{
    EAGLContext * context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    
    if (!context) {
        
    }
    
    //
    if (![EAGLContext setCurrentContext:context]) {
        NSLog(@"Set current context false");
    }
    
    self.myContext = context;
}
// 设置图层
- (void)setupLayer{
    self.myEagLayer = (CAEAGLLayer *)self.layer;
    
    [self setContentScaleFactor:[[UIScreen mainScreen] scale]];
    
    //3.设置描述属性，这里设置不维持渲染内容以及颜色格式为RGBA8
       /*
        kEAGLDrawablePropertyRetainedBacking  表示绘图表面显示后，是否保留其内容。
        kEAGLDrawablePropertyColorFormat
            可绘制表面的内部颜色缓存区格式，这个key对应的值是一个NSString指定特定颜色缓存区对象。默认是kEAGLColorFormatRGBA8；
        
            kEAGLColorFormatRGBA8：32位RGBA的颜色，4*8=32位
            kEAGLColorFormatRGB565：16位RGB的颜色，
            kEAGLColorFormatSRGBA8：sRGB代表了标准的红、绿、蓝，即CRT显示器、LCD显示器、投影机、打印机以及其他设备中色彩再现所使用的三个基本色素。sRGB的色彩空间基于独立的色彩坐标，可以使色彩在不同的设备使用传输中对应于同一个色彩坐标体系，而不受这些设备各自具有的不同色彩坐标的影响。


        */
    self.myEagLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:@false, kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
}
#pragma mark — shader
// 加载Shader
- (GLuint)loadShader:(NSString *)vert WithFrag:(NSString *)frag{
    
    //定义2个零时着色器
    GLuint verShader, fragShader;
    
    //创建program
    GLint program= glCreateProgram();
    
    
    
    
    return 1;
}

// 编译shader
//- (void)compileShader:(GLuint *)shader type:()
@end

NS_ASSUME_NONNULL_END
