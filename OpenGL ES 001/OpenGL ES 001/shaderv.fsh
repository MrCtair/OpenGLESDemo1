precision highp float;
// 纹理坐标
varying lowp vec2 varyTextCoord;
// 纹理
uniform sampler2D colorMap;

void main(){
    //拿到纹理对应坐标下的文素
    //纹理对应像素点的颜色值
    //内建函数：texture2D(纹理， 纹理坐标)
    lowp vec4 temp = texture2D(colorMap, varyTextCoord);
    
    //修改透明度
//    temp.a = 0.3;
    
    //重要且必须的内建边聊， gl_FragColor
    //片元着色器执行代码
    gl_FragColor = temp;
}
