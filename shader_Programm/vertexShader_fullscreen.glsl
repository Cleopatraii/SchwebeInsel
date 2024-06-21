#version 330 core 

//Texture: 3
// in, out, out=in

//1.Input
layout (location = 0) in vec2 vInPosition;
layout (location = 2) in vec2 textureCoodi;  //texture_in

//2.Output
out vec2 texKoordinaten; //texture_out, Texture-Koordinaten


void main() {
   //1 TeaKanel-Vertex Position         
   gl_Position = vec4(vInPosition, 0.0, 1.0);

   //Texture-Mapping
   texKoordinaten = textureCoodi;
}



