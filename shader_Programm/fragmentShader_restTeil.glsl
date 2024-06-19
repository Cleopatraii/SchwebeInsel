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
uniform sampler2D FragTexture_restTeil;

//2.Input
in vec3 FragPosition;
in vec3 vertexZuFragmentNormal;
in vec2 texKoordinaten;

//3.Output
out vec4 FragColor;

void main() {

   //1.Beleuchtung berechnen und Schattierun implementieren

   //  (1) Ambient
   float ambi = light.ambient.r;                           // 1.环境光照 ambient

   //  (2) Diffuse
   //     2.1 Erstelichtquelle
   vec3 N = normalize(vertexZuFragmentNormal);                       // 法线方向 normal
   vec3 L = normalize(light.position - FragPosition);    // 入射光方向 light = QuelleLicht_Position - vertex_position (view)
   float diff = max(dot(N, L), 0.0);
   //     2.2 Zweite Lichtquelle
   vec3 N2 = normalize(vertexZuFragmentNormal);                       // 法线方向 normal
   vec3 L2 = normalize(vec3(-1000.0, 0.0, 0.0) - FragPosition);
   float diff2 = max(dot(N2, L2), 0.0);                     // 入射光角度 (l,n)                     // 入射光角度 (l,n)
   
   //  (3) Spekular
   vec3 augenRichtung = vec3(0.0, 0.0, 1.0);             // 视线方向
   vec3 reflectRichtung = normalize(reflect(-L, N));     // 反射光方向
   float spec = pow(max(dot(augenRichtung, reflectRichtung), 0.0), mat.shininess); // 偏差夹脚 (r,a)，Phone-Beleuchtungsmodellen

   // 2. Texture
   vec4 restTeilColor = texture(FragTexture_restTeil, texKoordinaten);

   // 3. Color berechnen
   vec3 restTeilMixColor = mix(vec3(0.2, 0.2, 0.2), restTeilColor.rgb, ambi + diff + spec).rgb; // Lichtquelle1 
   vec3 restTeilFinColor = mix(restTeilMixColor, vec3(0.2, 0.0, 0.0), diff2/2.0); // Lichtquelle2


   //4.Final Color
   FragColor = vec4(restTeilFinColor.rgb, 1.0);

}
