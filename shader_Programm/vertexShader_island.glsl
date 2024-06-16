#version 330 core 

//Texture: 3
// in, out, out=in

//1.Input
layout (location = 0) in vec3 vInPosition;
layout (location = 1) in vec3 vInNormalen;
layout (location = 2) in vec2 textureCoodi;  //texture_in

//2.Output
out vec3 FragPosition;  // Keine Input, sondern durch Schattierung berechnen
out vec3 vertexZuFragmentNormal; // Normalen benutzen, um Farbe zu berechen, das alles in "Fragment-Shader" stattfinden
out vec2 texKoordinaten; //texture_out, Texture-Koordinaten
out float fogFactor;// Fog

//3.Transformation
uniform mat4 proj; 
uniform mat4 view;
uniform mat4 world;

//4.Phong-Beleuchtung: Matrix invertieren
uniform mat3 normalenMatrix;

void main() {
   //1 TeaKanel-Vertex Position         
   gl_Position = proj * view * world * vec4(vInPosition, 1.0);
   vec4 viewPosition=view * world * vec4(vInPosition, 1.0);

   //2 Transformation für Beleuchtung
   vec4 FragPos = view * world * vec4(vInPosition, 1.0);            //Beleuchtung findet in View-Koordinaten statt
   FragPosition = FragPos.xyz / FragPos.w;                     //x,y,z durch w (Für Translation/Verschieben)
   //  Ue7.2 Phong-Beleuchtung !!!!!!!!!!!!!!
   //   - 逆矩阵通常用于将光源的方向从世界坐标系变换到局部坐标系, 以便与法线向量进行计算; 
   //   - 这对于处理旋转和平移变换特别有用, 因为法线向量需要与光线方向在同一坐标系中进行计算;
   //vertexZuFragmentNormal  = (view * world * vec4(vInNormalen, 0.0)).xyz;   //Wenn 3D-Objekt umdrehen, dann drehen Normalen auch um (Für Rotation)

   vertexZuFragmentNormal = normalenMatrix * vInNormalen;
   
   //Texture-Mapping
   texKoordinaten = textureCoodi;

   // Fog Faktor rechnen
    float height = viewPosition.y;
    float fogStart = -2; // 雾开始的高度
    float fogEnd = 2.3; // 雾结束的高度（更低）
   fogFactor = smoothstep(fogStart, fogEnd, height);
}



