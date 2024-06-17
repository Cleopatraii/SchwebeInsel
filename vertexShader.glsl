#version 330 core 

//1.Input
layout (location = 0) in vec3 vInPosition; 
layout (location = 1) in vec3 vInNormalen;  
layout(location = 2) in vec2 aTexCoords; 

//2.Output
out vec3 FragPosition;  // Keine Input, sondern durch Schattierung berechnen
out vec3 vertexZuFragmentNormal; // Normalen benutzen, um Farbe zu berechen, das alles in "Fragment-Shader" stattfinden
out vec2 TexCoords; //Textur

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
   //  Ue7.2 Phong-Beleuchtung !!!!!!!!!!!!!!
   //   - 逆矩阵通常用于将光源的方向从世界坐标系变换到局部坐标系, 以便与法线向量进行计算; 
   //   - 这对于处理旋转和平移变换特别有用, 因为法线向量需要与光线方向在同一坐标系中进行计算;
   //vertexZuFragmentNormal  = (view * world * vec4(vInNormalen, 0.0)).xyz;   //Wenn 3D-Objekt umdrehen, dann drehen Normalen auch um (Für Rotation)

   vertexZuFragmentNormal = normalenMatrix * vInNormalen;
   TexCoords = aTexCoords;
   //test
   // mat3 Mat4x4zuMat3x3 = mat3(
   //    0.237196, 0.000000, 0.862402, 
   //    0.385678, 0.800000, -0.106077, 
   //    -0.862402, 0.447214, 0.237196
   // );
   // mat3 inverseMat3x3 = mat3(
   //    0.296495, 0.482098, -0.862402, 
   //    -0.000000, 1.000000, 0.447214, 
   //    1.078003, -0.132597, 0.237196,
   // );
   // mat3 transpMatrix = mat3(
   //    0.296495, -0.000000, 1.078003, 
   //    0.482098, 1.000000, -0.132597, 
   //    -0.862402, 0.447214, 0.237196
   // );
   //vertexZuFragmentNormal = Mat4x4zuMat3x3 * vInNormalen;
}



