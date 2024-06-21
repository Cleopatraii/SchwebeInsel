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
//texture_uniform
uniform sampler2D FragTexture_stone1;
uniform sampler2D FragTexture_stone2;
uniform sampler2D FragTexture_stone3;

//2.Input
in vec3 FragPosition;
in vec3 vertexZuFragmentNormal;
in vec2 texKoordinaten;
in float fogFactor;

//3.Output
layout (location = 0) out vec4 FragColor;   //GL_COLOR_ATTACHMENT0
layout (location = 1) out vec4 BrightColor; //Bloom: GL_COLOR_ATTACHMENT1

void main() {

   //1.Beleuchtung berechnen und Schattierun implementieren

   //  (1) Ambient
   float ambi = light.ambient.r;                           // 1.环境光照 ambient

   //  (2) Diffuse
   //     2.1 Erstelichtquelle
   vec3 N = normalize(vertexZuFragmentNormal);                       // 法线方向 normal
   vec3 L = normalize(light.position - FragPosition);    // 入射光方向 light = QuelleLicht_Position - vertex_position (view)
   float diff = max(dot(N, L), 0.0);                     // 入射光角度 (l,n)
   //     2.2 Zweite Lichtquelle
   vec3 N2 = normalize(vertexZuFragmentNormal);                       // 法线方向 normal
   vec3 L2 = normalize(vec3(0.0, -1.0, -8.0) - FragPosition);
   float diff2 = max(dot(N2, L2), 0.0);                     // 入射光角度 (l,n)
   
   //  (3) Spekular
   vec3 augenRichtung = vec3(0.0, 0.0, 1.0);             // 视线方向
   vec3 reflectRichtung = normalize(reflect(-L, N));     // 反射光方向
   float spec = pow(max(dot(augenRichtung, reflectRichtung), 0.0), mat.shininess); // 偏差夹脚 (r,a)，Phone-Beleuchtungsmodellen


   // 2. Multi-Texture
   vec4 baseColor = texture(FragTexture_stone3, texKoordinaten);
   vec4 detailColor = texture(FragTexture_stone1, texKoordinaten);
   vec4 shadowColor = texture(FragTexture_stone2, texKoordinaten);

   // 3. Color berechnen (混合三种纹理)
   vec4 stoneTexture = mix(detailColor, shadowColor, shadowColor.r);  // Stone Texture
   vec3 stoneColor = mix(stoneTexture, baseColor, step(0.5, detailColor)).rgb; // Stone Basis Farbe mit Texture
   vec3 stoneMixColor = mix(vec3(0.2, 0.2, 0.2), stoneColor, ambi + diff + spec).rgb; // Lichtquelle1
   vec3 stoneFinColor = mix(stoneMixColor, vec3(1.0, 0.0, 0.0), diff2); // Lichtquelle2

   //4.Fog Color 
   vec3 fogColor = vec3(0.5, 0.5, 0.5);
   vec3 finalColor = mix(stoneFinColor.rgb, fogColor, 1.0 - fogFactor); 


   //4.Final Color
   FragColor = vec4(finalColor.rgb, 1.0);

}