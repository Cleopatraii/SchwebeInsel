//two pass gaussian blur glsl

#version 330 core

in vec2 texKoordinaten;  // Eingehende Texturkoordinaten
out vec4 FragColor; //Ausgehende Fragmentfarbe

uniform sampler2D image; // Eingehende Textur
uniform bool horizontal; //  Steuerung für horizontales oder vertikales Weichzeichnen
const float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216); // Gaußsche Gewichtungen

void main(){     

    vec2 tex_offset = 1.0 / textureSize(image, 0);                // Größe eines Texels erhalten
    vec3 result = texture(image, texKoordinaten).rgb * weight[0]; // Beitrag des aktuellen Fragments

    // Horizontale Weichzeichnung
    if(horizontal)
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(image, texKoordinaten + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(image, texKoordinaten - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    }
     // Vertikale Weichzeichnung
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
