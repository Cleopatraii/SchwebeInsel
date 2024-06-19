#version 330 core

//1.Beleuchtung-Vorbereitung, Uniform-Variablen definieren
struct MaterialParams {
   vec3 ambient;
   vec3 diffuse;
   vec3 specular;
   float shininess;
};

struct LightParams {
   vec3 position;
   vec3 color;
   vec3 ambient;
};

uniform MaterialParams mat; // Matriale Licht
uniform LightParams light;  // Licht Quelle

//2.Input
in vec3 FragPosition;
in vec3 vertexZuFragmentNormal;

//3.Output
layout (location = 0) out vec4 FragColor;   //GL_COLOR_ATTACHMENT0
layout (location = 1) out vec4 BrightColor; //Bloom: GL_COLOR_ATTACHMENT1

void main() {

   //Beleuchtung berechnen und Schattierun implementieren

   //  (0) Emissiv
   vec3 resultColor = vec3(0.5, 0.0, 0.0);

   //  (1) Ambient
   resultColor += mat.ambient * light.ambient;       // 1.环境光照 ambient

   //  (2) Diffuse
   vec3 N = normalize(vertexZuFragmentNormal);                       // 法线方向 normal
   vec3 L = normalize(light.position - FragPosition);    // 入射光方向 light = QuelleLicht_Position - vertex_position (view)
   //vec3 L = normalize(vec3(4.0, 4.0, 0.0) - FragPosition);
   float diff = max(dot(N, L), 0.0);                     // 入射光角度 (l,n)
   resultColor += mat.diffuse * light.color * diff;      // 2.漫反射光照 diffuse

   //  (3) Spekular
   vec3 augenRichtung = vec3(0.0, 0.0, 1.0);             // 视线方向
   vec3 reflectRichtung = normalize(reflect(-L, N));     // 反射光方向
   float spec = pow(max(dot(augenRichtung, reflectRichtung), 0.0), mat.shininess); // 偏差夹脚 (r,a)，Phone-Beleuchtungsmodellen
//   resultColor += mat.specular * light.color * spec; 

   FragColor = vec4(resultColor, 1.0);

   float heiligkeit = dot(FragColor.rgb, vec3(0.2126f, 0.7152f, 0.0722f)); //vec3 的这个向量表示人眼对RGB颜色的敏感度
                                                                           //通过加权和方法计算出的亮度，更加符合人类视觉的自然感受，使得生成的图像在视觉上更加真实
   // (2) Nach Heiligkeit entscheidet man, ob man die zweite Texture benutzt oder nicht
   //     根据亮度决定是否接受第二个纹理片段
   if (heiligkeit >= 0.00f) 
      BrightColor = vec4(1.0, FragColor.g*0.3, 0.0, 1.0f); //Emissivierte rot Licht, weniger Beeinflusst von Gelbe-Lichtquelle
   else
      BrightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}
