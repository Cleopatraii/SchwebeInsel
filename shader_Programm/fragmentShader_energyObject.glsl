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
out vec4 FragColor;
//out vec4 BrightColor; // 高亮输出

//layout(location = 0) out vec4 FragColor;  // 常规颜色输出
//layout(location = 1) out vec4 BrightColor; // 高亮颜色输出

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

 // 总亮度，可以调整阈值以获取不同的高亮效果
 //  float brightness = dot(resultColor, vec3(0.2126, 0.7152, 0.0722));
 //  BrightColor = brightness > 1.5 ? vec4(resultColor, 1.0) : vec4(0.0, 0.0, 0.0, 1.0);

   FragColor = vec4(resultColor, 1.0);
}
