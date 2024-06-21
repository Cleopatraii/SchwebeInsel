#version 330 core 

//1.Input
layout (location = 0) in vec3 vInPosition; 
layout (location = 1) in vec3 vInNormalen;  

//2.Output
out vec3 FragPosition;  // Keine Input, sondern durch Schattierung berechnen
out vec3 vertexZuFragmentNormal; // Normalen benutzen, um Farbe zu berechen, das alles in "Fragment-Shader" stattfinden

//3.Transformation
uniform mat4 proj; 
uniform mat4 view;
uniform mat4 world;

//4.Phong-Beleuchtung: Matrix invertieren
uniform mat3 normalenMatrix;

void main() {
   //1 TeaKanel-Vertex Position         
   gl_Position = proj * view * world * vec4(vInPosition, 1.0);

   //2 Transformation für Beleuchtung
   vec4 FragPos = view * world * vec4(vInPosition, 1.0);            //Beleuchtung findet in View-Koordinaten statt
   FragPosition = FragPos.xyz / FragPos.w;                     //x,y,z durch w (Für Translation/Verschieben)
   //  Ue7.2 Phong-Beleuchtung !!!!
   vertexZuFragmentNormal  = (view * world * vec4(vInNormalen, 0.0)).xyz;   //Wenn 3D-Objekt umdrehen, dann drehen Normalen auch um (Für Rotation)

   //vertexZuFragmentNormal = normalenMatrix * vInNormalen;
}



