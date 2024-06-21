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
   resultColor += mat.ambient * light.ambient;       // 1.Umgebungslicht  ambient

   //  (2) Diffuse
   vec3 N = normalize(vertexZuFragmentNormal);                       // Normalenrichtung  normal
   vec3 L = normalize(light.position - FragPosition);    // Einfallsrichtung  light = QuelleLicht_Position - vertex_position (view)
   //vec3 L = normalize(vec3(4.0, 4.0, 0.0) - FragPosition);
   float diff = max(dot(N, L), 0.0);                     // Einfallswinkel  (l,n)
   resultColor += mat.diffuse * light.color * diff;      // diffuse

   //  (3) Spekular
   vec3 augenRichtung = vec3(0.0, 0.0, 1.0);             // Blickrichtung
   vec3 reflectRichtung = normalize(reflect(-L, N));     // Reflexionsrichtung
   float spec = pow(max(dot(augenRichtung, reflectRichtung), 0.0), mat.shininess); // Abweichungswinkel  (r,a)，Phone-Beleuchtungsmodellen
   //resultColor += mat.specular * light.color * spec; 

   FragColor = vec4(resultColor, 1.0);

   float heiligkeit = dot(FragColor.rgb, vec3(0.2126f, 0.7152f, 0.0722f)); // vec3 Dieser Vektor repräsentiert die Empfindlichkeit des menschlichen Auges gegenüber RGB-Farben
                                                                           // Die durch eine gewichtete Summe berechnete Helligkeit entspricht besser dem natürlichen Empfinden des menschlichen Sehens, wodurch das erzeugte Bild visuell realistischer wird.
   
   // (2) Nach Heiligkeit entscheidet man, ob man die zweite Texture benutzt oder nicht
   if (heiligkeit >= 0.00f) 
      BrightColor = vec4(1.0, FragColor.g*0.3, 0.0, 1.0f); //Emissivierte rot Licht, weniger Beeinflusst von Gelbe-Lichtquelle
   else
      BrightColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}
