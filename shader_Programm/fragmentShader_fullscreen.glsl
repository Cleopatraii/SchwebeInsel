#version 330 core

in vec2 texKoordinaten;
out vec4 FragColor;

uniform sampler2D screenTexture;
uniform sampler2D bloomBlurTexture;

void main() {

   //4.Final Color

   float gamma = 1.0;
   float exposure = 1.2f;
    
   // 读取HDR颜色和Bloom颜色
   vec3 hdrColor = texture(screenTexture, texKoordinaten).rgb;
   vec3 bloomColor = texture(bloomBlurTexture, texKoordinaten).rgb;
   
   // 将Bloom颜色加到HDR颜色上（加性混合）
   hdrColor += bloomColor;

   // 进行色调映射
   vec3 result = vec3(1.0) - exp(-hdrColor * exposure);

   // 进行伽玛校正
   result = pow(result, vec3(1.0 / gamma));

   // 将结果赋值给片段颜色
   FragColor = vec4(result, 1.0);

   //Test
//   FragColor = texture(screenTexture, texKoordinaten);
//   FragColor = texture(bloomBlurTexture, texKoordinaten);
//   FragColor = vec4(hdrColor, 1.0);
//   FragColor = vec4(bloomColor, 1.0);

}
