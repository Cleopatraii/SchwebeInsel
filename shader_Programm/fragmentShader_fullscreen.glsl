#version 330 core

in vec2 texKoordinaten;
out vec4 FragColor;

uniform sampler2D screenTexture;
uniform sampler2D bloomBlurTexture;

void main() {

   //4.Final Color

   float gamma = 1.0;
   float exposure = 1.2f;
   
   // HDR-Farben und Bloom-Farben auslesen
   vec3 hdrColor = texture(screenTexture, texKoordinaten).rgb;
   vec3 bloomColor = texture(bloomBlurTexture, texKoordinaten).rgb;
   
   // Bloom-Farbe zu HDR-Farbe hinzufügen (additive Mischung)
   hdrColor += bloomColor;

   // Tonemapping durchführen
   vec3 result = vec3(1.0) - exp(-hdrColor * exposure);

   // Gamma-Korrektur durchführen
   result = pow(result, vec3(1.0 / gamma));

    // Ergebnis der Fragmentfarbe zuweisen
   FragColor = vec4(result, 1.0);

   //Test
//   FragColor = texture(screenTexture, texKoordinaten);
//   FragColor = texture(bloomBlurTexture, texKoordinaten);
//   FragColor = vec4(hdrColor, 1.0);
//   FragColor = vec4(bloomColor, 1.0);

}
