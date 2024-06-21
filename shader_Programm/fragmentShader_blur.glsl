//two pass gaussian blur glsl

#version 330 core

in vec2 texKoordinaten;  // 输入的纹理坐标
out vec4 FragColor; // 输出的片段颜色

uniform sampler2D image; // 输入的纹理
uniform bool horizontal; // 控制水平或垂直模糊的布尔变量
const float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216); // 高斯权重数组

void main(){     

    vec2 tex_offset = 1.0 / textureSize(image, 0);                // gets size of single texel // 获取纹理的大小
    vec3 result = texture(image, texKoordinaten).rgb * weight[0]; // current fragment's contribution 当前像素的颜色

    //水平模糊
    if(horizontal)
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(image, texKoordinaten + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(image, texKoordinaten - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    //垂直模糊
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(image, texKoordinaten + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(image, texKoordinaten - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }
    FragColor = vec4(result, 1.0);
}
