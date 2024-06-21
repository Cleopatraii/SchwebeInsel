#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include "vertexShader_island.h"
#include "fragmentShader_island.h"
#include "vertexShader_restTeil.h"
#include "fragmentShader_restTeil.h"
#include "vertexShader_energyObject.h"
#include "fragmentShader_energyObject.h"
#include "vertexShader_fullscreen.h"
#include "fragmentShader_fullscreen.h"
#include "fragmentShader_blur.h"
#include "vertexShader_skybox.h"
#include "fragmentShader_skybox.h"
#include "main_funk.h"

#define STB_IMAGE_IMPLEMENTATION
#include "shader_Programm/stb_image.h"

// Globale Variablen für Shader-Programme und Geometriedaten
GLuint program_island;
GLuint program_restTeil;
GLuint program_energyObject;
GLuint program_fullscreen;
GLuint program_blur;
GLuint program_skybox;

GLuint vao_island;   
GLuint vbo_island;    
GLuint vao_platform;
GLuint vbo_platform; 
GLuint vao_haus;
GLuint vbo_haus;
GLuint vao_fan;
GLuint vbo_fan;
GLuint vao_tree;
GLuint vbo_tree;
GLuint vao_energyObject;
GLuint vbo_energyObject;
GLuint vao_skybox;
GLuint vbo_skybox;

float rotationAngleFan; 
float rotationAngleGanzIsland;
float rotationAngleEnergieObject;
float rotationAngleIslandAroundEnergie; 
float rotateAroundCrystal;


GLuint texture_stone1;
GLuint texture_stone2;
GLuint texture_stone3;
GLuint texture_platform;
GLuint texture_tree;
GLuint texture_fan;
GLuint texture_haus;
GLuint texture_skybox;

GLsizei width = 800;
GLsizei height = 600;



// (1) Datenstrukturen definieren
typedef struct {
    GLfloat v1, v2, v3, t1, t2, n1, n2, n3;
} Flaeche8f;

typedef struct {
    GLfloat x, y;
} Vector2f;

typedef struct {
    GLfloat x, y, z;
} Vector3f;

typedef struct {
    int v1, t1, n1, v2, t2, n2, v3, t3, n3;
} Vector9fi;

// (2)  OBJ-Datei einlesen und VBO erzeugen
// Eingabe: (1) Zu lesender OBJ-Datei-Name, (2) Leerer VBO-Objekt-Zeiger, (3) Leere VBO-Größe
// Ausgabe: (1) Ausgefüllter VBO-Objekt-Zeiger, (2) Ausgefüllte VBO-Größe
void readOBJ(const char* filename, Flaeche8f** vboOut, long* vboLen) {

    // Null prüfen
    assert(filename != NULL);
    assert(vboOut != NULL);
    assert(vboLen != NULL);

    // filename kann nicht leer sein
    assert(strlen(filename) > 0); 
    
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Datei konnte nicht geöffnet werden.");
        return;
    }

    // Listen für Vertex-Daten
    Vector3f* objV = NULL;  // Vertex 
    int vNumber = 0;
    // Listen für Vertex-Textur
    Vector2f* objVT = NULL;  // Textur 
    int vtNumber = 0;
    // Listen für Vertex-Normal
    Vector3f* objVN = NULL;  // Normal 
    int vnNumber = 0;
    // face
    Vector9fi* objVF = NULL;  // face 
    int vfNumber = 0;
    
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        //Ue6.2 bei Zeilen, die mit v, vn oder vt beginnen, die Daten in separate Listen ablegen
        if (line[0] == 'v') { 
            if (line[1] == ' ') {
                // Vertex
                vNumber++;
                objV = realloc(objV, vNumber * sizeof(Vector3f));
                sscanf(line, "v %f %f %f", &objV[vNumber - 1].x, &objV[vNumber - 1].y, &objV[vNumber - 1].z);
            } else if (line[1] == 't') {
                // VT: Textur
                vtNumber++;
                objVT = realloc(objVT, vtNumber * sizeof(Vector2f));
                sscanf(line, "vt %f %f", &objVT[vtNumber - 1].x, &objVT[vtNumber - 1].y);
            } else if (line[1] == 'n') {
                // VN: Normal
                vnNumber++;
                objVN = realloc(objVN, vnNumber * sizeof(Vector3f));
                sscanf(line, "vn %f %f %f", &objVN[vnNumber - 1].x, &objVN[vnNumber - 1].y, &objVN[vnNumber - 1].z);
            } 
        } else if (line[0] == 'f') {
                vfNumber++;
                objVF = realloc(objVF, vfNumber * sizeof(Vector9fi));
                sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", &objVF[vfNumber - 1].v1, &objVF[vfNumber - 1].t1, &objVF[vfNumber - 1].n1, &objVF[vfNumber - 1].v2, &objVF[vfNumber - 1].t2, &objVF[vfNumber - 1].n2, &objVF[vfNumber - 1].v3, &objVF[vfNumber - 1].t3, &objVF[vfNumber - 1].n3);
        } 
    }
  

    // Zum Lesen einer OBJ-Datei und Erstellen eines VBO-Objekts
    Flaeche8f* vbo = NULL;  
    long length = 0;
    for (int i = 0; i < vfNumber; i++) {
        //3 Punkten einmal
        length = length + 24; //3 * 8, 3 Punkten, Jede Punkten 8 Unit (3v+2t+3n)
        vbo = realloc(vbo, vfNumber * 3 * sizeof(Flaeche8f)); 
        //Erste Punkt
        vbo[i*3].v1 = objV[objVF[i].v1-1].x; //vertex
        vbo[i*3].v2 = objV[objVF[i].v1-1].y; //vertex
        vbo[i*3].v3 = objV[objVF[i].v1-1].z; //vertex
        vbo[i*3].t1 = objVT[objVF[i].t1-1].x; //textur
        vbo[i*3].t2 = objVT[objVF[i].t1-1].y; //textur
        vbo[i*3].n1 = objVN[objVF[i].n1-1].x; //normal
        vbo[i*3].n2 = objVN[objVF[i].n1-1].y; //normal
        vbo[i*3].n3 = objVN[objVF[i].n1-1].z; //normal
        //Zwiete Punkt
        vbo[i*3+1].v1 = objV[objVF[i].v2-1].x; //vertex
        vbo[i*3+1].v2 = objV[objVF[i].v2-1].y; //vertex
        vbo[i*3+1].v3 = objV[objVF[i].v2-1].z; //vertex
        vbo[i*3+1].t1 = objVT[objVF[i].t2-1].x; //textur
        vbo[i*3+1].t2 = objVT[objVF[i].t2-1].y; //textur
        vbo[i*3+1].n1 = objVN[objVF[i].n2-1].x; //normal
        vbo[i*3+1].n2 = objVN[objVF[i].n2-1].y; //normal
        vbo[i*3+1].n3 = objVN[objVF[i].n2-1].z; //normal
        //Dritte Punkt
        vbo[i*3+2].v1 = objV[objVF[i].v3-1].x; //vertex
        vbo[i*3+2].v2 = objV[objVF[i].v3-1].y; //vertex
        vbo[i*3+2].v3 = objV[objVF[i].v3-1].z; //vertex
        vbo[i*3+2].t1 = objVT[objVF[i].t3-1].x; //textur
        vbo[i*3+2].t2 = objVT[objVF[i].t3-1].y; //textur
        vbo[i*3+2].n1 = objVN[objVF[i].n3-1].x; //normal
        vbo[i*3+2].n2 = objVN[objVF[i].n3-1].y; //normal
        vbo[i*3+2].n3 = objVN[objVF[i].n3-1].z; //normal
    }
    *vboLen = length;
    *vboOut = vbo;

    fclose(file);
    free(objV);
    free(objVT);
    free(objVN);
}

//Object1: vbo von IslandStone-Model
Flaeche8f* objVbo_island; 
long objVboLen_island=0; 
//Object2: vbo von GreenPlatform-Model
Flaeche8f* objVbo_platform; 
long objVboLen_platform=0; 
//Object3: vbo von Haus-Model
Flaeche8f* objVbo_haus; 
long objVboLen_haus=0; 
//Object4: vbo von Tree-Model
Flaeche8f* objVbo_tree; 
long objVboLen_tree=0; 
//Object5: vbo von Fan-Model
Flaeche8f* objVbo_fan; 
long objVboLen_fan=0; 
//Object6: vbo von energyObject-Model
Flaeche8f* objVbo_energyObject; 
long objVboLen_energyObject=0; 
//Object7: vbo von Skybox-Model
Flaeche8f* objVbo_skybox;
long objVboLen_skybox=0;

//Funktion: Beleuchtung mit Normalenmatrix
//Benötigte Datenstruktur definieren
typedef struct {
    GLfloat x;
    GLfloat y;
    GLfloat z;
} Vec3;

typedef struct {
    Vec3 ambient;
    Vec3 diffuse;
    Vec3 specular;
    float shininess;
} MaterialLichtParams;

typedef struct {
    Vec3 position;
    Vec3 color;
    Vec3 ambient;
} QuelleLightParams;

// Funktion: Texturierung
// (1) Bilddaten in Texturobjekt umwandeln
void textureMapping(const char *str, GLuint* texture){

    // Null prüfen
    assert(str != NULL);
    assert(texture != 0);
    // filename kann nicht leer sein
    assert(strlen(str) > 0);
     
    //1.Externer Bilddatei laden
    stbi_set_flip_vertically_on_load(1); //Das Bild soll vertikal gespiegelt werden.
    int width, height, nrChannels;
    unsigned char *image = stbi_load(str, &width, &height, &nrChannels, 4); // nrChannels speichert die Anzahl der Farbkanäle im geladenen Bild

    //2.Testur-Objekt erzeugen
    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);

    //3.Textur-Objekt mit Bilddaten füllen
    //  BildPixel -> TexturKoordinaten
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        width,
        height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        image
    );
    glGenerateMipmap(GL_TEXTURE_2D); //Mipmap

    //4.Bilddatei löschen und Binding freigeben
    stbi_image_free(image);
    glBindTexture(GL_TEXTURE_2D, 0);

}


// Hilfsfunktion: Shader-Text initialisieren
void shaderInit(GLuint *shader, unsigned char *shader_glsl, const unsigned int *shader_glsl_len, int shaderType) {
    //assert bis hier keine Zeiger prüfen
    //Bei shaderArten: VertexShader = 1
    //                 FragementShader = 2
    //Zurzeit darf nur VertexShader oder FragmentShader sein
    assert(shaderType == 1 || shaderType == 2);
    //ShaderText kann nicht leeer sein 
    assert(*shader_glsl_len > 0);

    if (shaderType == 1) {
        *shader = glCreateShader(GL_VERTEX_SHADER); 
    } else if (shaderType == 2) {
        *shader = glCreateShader(GL_FRAGMENT_SHADER); 
    } else {
        printf("Invalid Shader Type, shaderInit() break !");
        return; 
    }
    const GLchar *const *validShader_glsl = &shader_glsl;

    glShaderSource(*shader, 1, validShader_glsl, shader_glsl_len);
    glCompileShader(*shader); 
    GLint status;
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
    if (!status) {
        printf("Error compiling shader:");
        GLchar infoLog[1024];
        glGetShaderInfoLog(*shader, 1024, NULL, infoLog);
        printf("%s", infoLog);
        return;
    }
}

// Hilfsfunktion: Shader-Programm initialisieren
void shaderProgramInit(GLuint* program, GLuint* vertexShader, GLuint* fragmentShader) {
    GLint status;
    *program = glCreateProgram();// Erstellen eines Shader-Programms
    glAttachShader(*program, *vertexShader);   // Anfügen des Vertex-Shaders an das Shader-Programm
    glAttachShader(*program, *fragmentShader);// Anfügen des Fragment-Shaders an das Shader-Programm
    glLinkProgram(*program); // Verlinken des Shader-Programms
                            // OpenGL überprüft, ob die Shader korrekt zu einem Programm verlinkt werden können
                            // Bei erfolgreicher Verlinkung wird das Shader-Programm erstellt und kann für das Rendering verwendet werden
    glGetProgramiv(*program, GL_LINK_STATUS, &status);// Überprüfen des Link-Status des Shader-Programms
    if (!status) {
        printf("Error linking program:");
        GLchar infoLog[1024];
        glGetProgramInfoLog(*program, 1024, NULL, infoLog);
        printf("%s",infoLog);
        exit(-1);
    }
}

//Hilfefunktion: vbo und vao verarbeiten
//Eingabe: vboLen (vbo-Object-Große), vbo(vbo-Object)
//         (1) Mit Texture: ObTexture=1
//         (2) Ohne Texture: ObTexture=0
void vboUndvao(long* vboLen, Flaeche8f* vbo, GLuint* vao, int ObTexture) {

    //ObTexture muss richtig angegeben
    assert(ObTexture == 0 || ObTexture == 1);

    //vbo
    GLfloat objectVertices[*vboLen];
    for (long i=0; i<*vboLen/8; i++) {
        objectVertices[i*8] = vbo[i].v1;
        objectVertices[i*8+1] = vbo[i].v2;
        objectVertices[i*8+2] = vbo[i].v3;
        objectVertices[i*8+3] = vbo[i].t1;
        objectVertices[i*8+4] = vbo[i].t2;
        objectVertices[i*8+5] = vbo[i].n1;
        objectVertices[i*8+6] = vbo[i].n2;
        objectVertices[i*8+7] = vbo[i].n3;
    }
   

    GLuint VBOobject; // // Erstellen eines Buffer-Objekts
    glGenBuffers(1, &VBOobject); //// Generieren eines Buffer-Objekts
    glBindBuffer(GL_ARRAY_BUFFER, VBOobject); // Binden des Buffer-Objekts
    glBufferData(GL_ARRAY_BUFFER, sizeof(objectVertices), objectVertices, GL_STATIC_DRAW); // Kopieren der Vertex-Daten in das Buffer-Objekt
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    //vao
    glGenVertexArrays(1, vao); // Generieren eines Vertex Array Objects
    glBindVertexArray(*vao); // Binden des VAO
    glBindBuffer(GL_ARRAY_BUFFER, VBOobject); // Binden des VBO
    //1.Vertex
    glVertexAttribPointer( 
        0,                         // Index des Vertex-Attributs
        3,                         // Anzahl der Komponenten pro Vertex-Attribut
        GL_FLOAT,                  // Datentyp der Komponenten
        GL_FALSE,                  
        8 * sizeof(GLfloat),       // Byte-Offset zwischen aufeinanderfolgenden Vertex-Attributen
        0                          // Offset des ersten Elements im Array
                                
    );
    glEnableVertexAttribArray(0);  // Aktivieren des Vertex-Attributs
    //2.Normalen
    glVertexAttribPointer( 
        1,                         
        3,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(GLfloat),
        (GLvoid*)(5 * sizeof(GLfloat))                     
    );
    glEnableVertexAttribArray(1); 
    //3.Texture
    if(ObTexture==1) {
        glVertexAttribPointer(
            2,
            2,
            GL_FLOAT,
            GL_FALSE,
            8 * sizeof(GLfloat),
            (GLvoid*)(3 * sizeof(GLfloat))
        );
        glEnableVertexAttribArray(2);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// Initialisierung von Shadern für eine Insel-Szene
void init1(void) {
    //shader
    GLuint vertexShader_island;
    shaderInit(&vertexShader_island, shader_Programm_vertexShader_island_glsl, &shader_Programm_vertexShader_island_glsl_len, 1);   //VertexShader init
    GLuint fragmentShader_island;
    shaderInit(&fragmentShader_island, shader_Programm_fragmentShader_island_glsl, &shader_Programm_fragmentShader_island_glsl_len, 2); //FragmentShader init
    //shader program erzeugen und linken
    shaderProgramInit(&program_island, &vertexShader_island, &fragmentShader_island);

    //add
    glDeleteShader(vertexShader_island);
    glDeleteShader(fragmentShader_island);

    //texture
    textureMapping("Ressouce/textureBild/2.Island_stone/stone1.jpeg", &texture_stone1);
    textureMapping("Ressouce/textureBild/2.Island_stone/stone2.jpeg", &texture_stone2);
    textureMapping("Ressouce/textureBild/2.Island_stone/stone3.jpeg", &texture_stone3);
    
    //obj-Datei lesen
    readOBJ("Ressouce/objDatei/islandStone.obj", &objVbo_island, &objVboLen_island);
    //VBO und VAO verarbeiten
    vboUndvao(&objVboLen_island, objVbo_island, &vao_island, 1);

}

 // Initialisierung von Shadern für zusätzliche Objekte
void init2(void) {
    //shader
    GLuint vertexShader_restTeil;
    shaderInit(&vertexShader_restTeil, shader_Programm_vertexShader_restTeil_glsl, &shader_Programm_vertexShader_restTeil_glsl_len, 1);   //VertexShader init
    GLuint fragmentShader_restTeil;
    shaderInit(&fragmentShader_restTeil, shader_Programm_fragmentShader_restTeil_glsl, &shader_Programm_fragmentShader_restTeil_glsl_len, 2); //FragmentShader init
    //shader program erzeugen und linken
    shaderProgramInit(&program_restTeil, &vertexShader_restTeil, &fragmentShader_restTeil);

    //add
    glDeleteShader(vertexShader_restTeil);
    glDeleteShader(fragmentShader_restTeil);

    //texture
    textureMapping("Ressouce/textureBild/1.Green_Plattform/Greenplattern.jpg", &texture_platform);
    textureMapping("Ressouce/textureBild/4.ReineFarbe/haus.png", &texture_haus);
    textureMapping("Ressouce/textureBild/4.ReineFarbe/fan.png", &texture_fan);
    textureMapping("Ressouce/textureBild/4.ReineFarbe/tree.png", &texture_tree);
    
    //obj-Datei lesen
    readOBJ("Ressouce/objDatei/greenPlatform.obj", &objVbo_platform, &objVboLen_platform);
    readOBJ("Ressouce/objDatei/haus.obj", &objVbo_haus, &objVboLen_haus);
    readOBJ("Ressouce/objDatei/fan.obj", &objVbo_fan, &objVboLen_fan);
    readOBJ("Ressouce/objDatei/tree.obj", &objVbo_tree, &objVboLen_tree);
    //VBO und VAO verarbeiten
    vboUndvao(&objVboLen_platform, objVbo_platform, &vao_platform, 1);
    vboUndvao(&objVboLen_haus, objVbo_haus, &vao_haus, 1);
    vboUndvao(&objVboLen_fan, objVbo_fan, &vao_fan, 1);
    vboUndvao(&objVboLen_tree, objVbo_tree, &vao_tree, 1);

}

// Initialisierung von Shadern für ein Energieobjekt
void init3(void) {
    //shader
    GLuint vertexShader_energyObject;
    shaderInit(&vertexShader_energyObject, shader_Programm_vertexShader_energyObject_glsl, &shader_Programm_vertexShader_energyObject_glsl_len, 1);   //VertexShader init
    GLuint fragmentShader_energyObject;
    shaderInit(&fragmentShader_energyObject, shader_Programm_fragmentShader_energyObject_glsl, &shader_Programm_fragmentShader_energyObject_glsl_len, 2); //FragmentShader init
    //shader program erzeugen und linken
    shaderProgramInit(&program_energyObject, &vertexShader_energyObject, &fragmentShader_energyObject);

    //add
    glDeleteShader(vertexShader_energyObject);
    glDeleteShader(fragmentShader_energyObject);

    //obj-Datei lesen
    readOBJ("Ressouce/objDatei/energyObject.obj", &objVbo_energyObject, &objVboLen_energyObject);
    //VBO und VAO verarbeiten
    vboUndvao(&objVboLen_energyObject, objVbo_energyObject, &vao_energyObject, 0);

    //glClearColor(191.0f/255.0f,217.0f/255.0f,204.0f/255.0f,1.0); 
}

 // Initialisierung von Shadern für Bloom-Effekte
void initBloom(void) {
    //shader
    GLuint vertexShader_fullscreen;
    shaderInit(&vertexShader_fullscreen, shader_Programm_vertexShader_fullscreen_glsl, &shader_Programm_vertexShader_fullscreen_glsl_len, 1);   //VertexShader init

    //shader-program1
    GLuint fragmentShader_fullscreen;
    shaderInit(&fragmentShader_fullscreen, shader_Programm_fragmentShader_fullscreen_glsl, &shader_Programm_fragmentShader_fullscreen_glsl_len, 2); //FragmentShader init
    shaderProgramInit(&program_fullscreen, &vertexShader_fullscreen, &fragmentShader_fullscreen);

    glDeleteShader(fragmentShader_fullscreen);
    
    //shader-program2
    GLuint fragmentShader_blur;
    shaderInit(&fragmentShader_blur, shader_Programm_fragmentShader_blur_glsl, &shader_Programm_fragmentShader_blur_glsl_len, 2); //FragmentShader init
    shaderProgramInit(&program_blur, &vertexShader_fullscreen, &fragmentShader_blur);

    glDeleteShader(fragmentShader_blur);
    glDeleteShader(vertexShader_fullscreen);
}

 // Initialisierung von Shadern für Skybox
void init4(void){
    GLuint vertexShader_skybox;
    shaderInit(&vertexShader_skybox, shader_Programm_vertexShader_skybox_glsl, &shader_Programm_vertexShader_skybox_glsl_len, 1);
    GLuint fragmentShader_skybox;
    shaderInit(&fragmentShader_skybox, shader_Programm_fragmentShader_skybox_glsl, &shader_Programm_fragmentShader_skybox_glsl_len, 2);
    
    shaderProgramInit(&program_skybox, &vertexShader_skybox, &fragmentShader_skybox);

    glDeleteShader(vertexShader_skybox);
    glDeleteShader(fragmentShader_skybox);

    textureMapping("Ressouce/textureBild/3.Hintergrund/skyHintergrund.jpeg", &texture_skybox);
    readOBJ("Ressouce/objDatei/earth.obj", &objVbo_skybox, &objVboLen_skybox);
    vboUndvao(&objVboLen_skybox, objVbo_skybox, &vao_skybox, 1);
    //glClearColor(191.0f/255.0f,217.0f/255.0f,204.0f/255.0f,1.0);
    
}


// Hilfsfunktion: Wechsel des Koordinatensystems
//(1) Lokale -> World
void setUniformWorldKoordi() {

}
//(2) World -> View (ohne Perspektion)
void setUniformViewKoordi() { 

}
//(3) View (ohne Perspektion) -> View (mit Perspektion)
void setUniformPerspKoordi() {
   
}
//Zusammenfassung: Lokale Koordinatensystem -> View Koordinatensystem
void transf_World_View_Persp_Unit() {

}

// Hilfsfunktion: Konfiguration der Beleuchtungsparameter
void uniformParaFuerBeleuchtung(GLuint* program) {
    
    //Matriale Licht
    GLint matAmbLoc = glGetUniformLocation(*program, "mat.ambient");
    GLint matDifLoc = glGetUniformLocation(*program, "mat.diffuse");
    GLint matSpeLoc = glGetUniformLocation(*program, "mat.specular");
    GLint matShLoc = glGetUniformLocation(*program, "mat.shininess");
    MaterialLichtParams materialLicht;
    materialLicht.ambient.x = 0.5f;
    materialLicht.ambient.y = 0.5f;
    materialLicht.ambient.z = 0.5f;
    materialLicht.diffuse.x = 0.5f;
    materialLicht.diffuse.y = 0.5f;
    materialLicht.diffuse.z = 0.5f;
    materialLicht.specular.x = 0.9f;
    materialLicht.specular.y = 0.9f;
    materialLicht.specular.z = 0.9f;
    materialLicht.shininess = 1.0f;
    glUniform3f(matAmbLoc, materialLicht.ambient.x, materialLicht.ambient.y, materialLicht.ambient.z);
    glUniform3f(matDifLoc, materialLicht.diffuse.x, materialLicht.diffuse.y, materialLicht.diffuse.z); 
    glUniform3f(matSpeLoc, materialLicht.specular.x, materialLicht.specular.y, materialLicht.specular.z);  
    glUniform1f(matShLoc, materialLicht.shininess);

    // Lichtquelle
    GLint quePosLoc = glGetUniformLocation(*program, "light.position");
    GLint queColLoc = glGetUniformLocation(*program, "light.color");
    GLint queAmbLoc = glGetUniformLocation(*program, "light.ambient");
    QuelleLightParams quelleLicht;
    quelleLicht.ambient.x = 0.5f;
    quelleLicht.ambient.y = 0.5f;
    quelleLicht.ambient.z = 0.5f;
    quelleLicht.color.x = 0.0f; //r
    quelleLicht.color.y = 1.0f; //g
    quelleLicht.color.z = 0.0f; //b
    quelleLicht.position.x = 20.0f;
    quelleLicht.position.y = 20.0f;
    quelleLicht.position.z = 0.0f;
    glUniform3f(queAmbLoc, quelleLicht.ambient.x, quelleLicht.ambient.y, quelleLicht.ambient.z);
    glUniform3f(queColLoc, quelleLicht.color.x, quelleLicht.color.y, quelleLicht.color.z); 
    glUniform3f(quePosLoc, quelleLicht.position.x, quelleLicht.position.y, quelleLicht.position.z); 
}

// Hilfsfunktion: Normalenmatrix berechnen
// • N = view * world = matrix_multiply(out,lookAtMatrix,arMatrix);
// • N(4x4) -> M(3x3) -> inverse -> transponieren 
void normalenMatrix(GLuint* program, GLfloat* lookAtMatrix, GLfloat* transfMatrix) {
    GLint normalenMatrixLoc = glGetUniformLocation(*program, "normalenMatrix");

    //(1) view * world
    GLfloat viewWordMatrix[16];
    matrix_multiply(viewWordMatrix, lookAtMatrix, transfMatrix);
    printf("viewWordMatrix: \n");
    printMatrix4x4(viewWordMatrix);

    //(2) 4->3
    GLfloat Mat4x4zuMat3x3[9];
    normalMatrixFromMatrix4x4(Mat4x4zuMat3x3, viewWordMatrix);
    printf("Mat4x4zuMat3x3: \n");
    printMatrix3x3(Mat4x4zuMat3x3);

    //(3) Inverse
    GLfloat inverseMat3x3[9];
    inverseMatrix3x3(inverseMat3x3, Mat4x4zuMat3x3);
    printf("inverseMat3x3: \n");
    printMatrix3x3(inverseMat3x3);

    //(4) Transponieren
    GLfloat transpMatrix[9];
    transponieren(transpMatrix, inverseMat3x3);
    printf("transpMatrix: \n");
    printMatrix3x3(transpMatrix);
    printf("\n");

    glUniformMatrix3fv(normalenMatrixLoc, 1, GL_TRUE, transpMatrix);
}

// Hilfsfunktion: Zeichnen von undurchsichtigen und sortierten transparenten Objekten

//(1) Alle Opaque Objekts
void drawOpaqueObjects() {
    //Depth Test einschalten
    glEnable(GL_DEPTH_TEST);

}
//(2) Alle sortierte transparente Objekts
void drawSortetTransparenteObjects(long* vboLen, GLuint* vbo) {
    //Vorbereiten
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    //Transparente Fektoren und Funktion setzen
    glBlendColor(0.5f, 0.5f, 0.5f, 1.0f); // Der Alpha-Kanal bestimmt die Undurchsichtigkeit der Quellfarbe
                                         // Ein Alpha-Wert von 1.0 bedeutet, dass die Quellfarbe vollständig undurchsichtig ist
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // #define GL_SRC_ALPHA_SATURATE_EXT 0x0308
                                                       // Definiert die Art der Farbmischung

    //draw                                                   
    //Buffer binden
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
    //Draw
    glDrawArrays(GL_TRIANGLES, 0, *vboLen/8); 
    //Buffer unbinden
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    //Z-Buffer zurücksetzen
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);                                                
    glEnable(GL_DEPTH_TEST);
}

//Rechnen die Zentrumpunkts des Objekts
void objectCenterRechnen(Flaeche8f* objVbo, long* objVboLen, GLfloat* centerPunkt) {
    if (*objVboLen == 0) {
        centerPunkt[0] = 0.0f;
        centerPunkt[1] = 0.0f;
        centerPunkt[2] = 0.0f;
        printf("objVboLen == 0 !!!!\n");
        return;
    }

    long vertexNum = *objVboLen/8; //该 object 的顶点数量

    // Initialisierung des Zentrumpunkts
    centerPunkt[0] = 0.0f;
    centerPunkt[1] = 0.0f;
    centerPunkt[2] = 0.0f;

    // Akkumulieren aller Vertex-Koordinaten
    for (long i = 0; i < vertexNum; i++) {
        centerPunkt[0] += objVbo[i].v1;
        centerPunkt[1] += objVbo[i].v2;
        centerPunkt[2] += objVbo[i].v3;
    }

    // Berechnung des Durchschnitts
    centerPunkt[0] /= vertexNum;
    centerPunkt[1] /= vertexNum;
    centerPunkt[2] /= vertexNum;
}

// Verwenden des Shader-Programms für die Island-Rendering
void draw1(void) {

    //glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); // Löscht Farb- und Tiefenpuffer, füllt sie mit der Hintergrundfarbe
    glUseProgram(program_island);       // Verwendet das spezifizierte Shader-Programm für das Rendern

    //Uniform: Proj-Matrix (View ohne Perspektion -> View mit Perspektion)
    GLint projLoc = glGetUniformLocation(program_island, "proj");
    GLfloat perspectiveMatrix[16];
    perspective(perspectiveMatrix, 60.0f/180.f*M_PI, 4.0f/3.0f, 0.1f, 100.0f);
    glUniformMatrix4fv(projLoc, 1, GL_TRUE, perspectiveMatrix);

    //Uniform: View-Matrix (World -> View)
    GLint viewLoc = glGetUniformLocation(program_island, "view");
    GLfloat lookAtMatrix[16];
    GLfloat eye[3] = {0.0f,1.0f,8.0f};
    GLfloat center[3] = {0.0f,0.0f,0.0f};
    GLfloat up[3] = {0.0f,1.0f,0.0f};
    lookAt(lookAtMatrix, eye, center, up);
    glUniformMatrix4fv(viewLoc, 1, GL_TRUE, lookAtMatrix);    

    GLfloat radiusAroundCrystal = 3.0; // Rotationsradius um den Kristall
    GLfloat crystalCenter[3] = {-0.0f, 0.0f, 0.0f}; // Zentralposition des Kristalls

    rotationAngleIslandAroundEnergie += 0.01; // Aktualisiere den Rotationswinkel

    GLfloat islandPosX = crystalCenter[0] + radiusAroundCrystal * cos(rotationAngleIslandAroundEnergie);
    GLfloat islandPosZ = crystalCenter[2] + radiusAroundCrystal * sin(rotationAngleIslandAroundEnergie);
    GLfloat XZPosition[3] = {islandPosX, crystalCenter[1], islandPosZ};

    GLfloat islandTransformationMatrix[16];
    identity(islandTransformationMatrix);
    translate(islandTransformationMatrix, islandTransformationMatrix, XZPosition);

    
    //Uniform: World-Matrix (Lokale -> World)
    GLint worldLoc = glGetUniformLocation(program_island, "world");
    GLfloat iMatrix[16];
    GLfloat arMatrix[16];
    identity(iMatrix);
    //*Animation: rotation
    rotatey(arMatrix, iMatrix, -rotationAngleGanzIsland);
    matrix_multiply(islandTransformationMatrix, islandTransformationMatrix, arMatrix);
    
    glUniformMatrix4fv(worldLoc, 1, GL_TRUE, islandTransformationMatrix);
    rotationAngleGanzIsland += 0.01f;

    //Beleuchtung UniformPara Konfikurieren
    uniformParaFuerBeleuchtung(&program_island);

    //NormalenMatrix
    // • N = view * world = matrix_multiply(out,lookAtMatrix,arMatrix);
    // • N(4x4) -> M(3x3) -> inverse -> transponieren
    normalenMatrix(&program_island, lookAtMatrix, arMatrix);

    //vao
    glBindVertexArray(vao_island);// Bindet das Vertex Array Objekt vao, signalisiert OpenGL, dieses VAO für das Zeichnen zu verwenden
    GLint status;
    glValidateProgram(program_island);  // Überprüft die Gültigkeit des Shader-Programms
                          
    glGetProgramiv(program_island, GL_VALIDATE_STATUS, &status); 
    if (!status) {
        printf("Error validating program:");
        GLchar infoLog[1024];
        glGetProgramInfoLog(program_island, 1024, NULL, infoLog);
        printf("%s",infoLog);
    }

    glBindBuffer(GL_ARRAY_BUFFER,  vbo_island);
    
    //Transparent Object
    //drawSortetTransparenteObjects(&objVboLen_island, &vboBufferObject);
    
    //Opaque Object
    glEnable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLES, 0, objVboLen_island/8); 

    //texture_stone1
    GLint textureS1Loc = glGetUniformLocation(program_island, "FragTexture_stone1");
    glUniform1i(textureS1Loc, 8);
    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_2D, texture_stone1);
    //glDrawElements(GL_TRIANGLES, objVboLen_island/8, GL_UNSIGNED_INT, 0);
    glDrawArrays(GL_TRIANGLES, 0, objVboLen_island/8);
    //texture_stone2
    GLint textureS2Loc = glGetUniformLocation(program_island, "FragTexture_stone2");
    glUniform1i(textureS2Loc, 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture_stone2);
    //glDrawElements(GL_TRIANGLES, objVboLen_island/8, GL_UNSIGNED_INT, 0);
    glDrawArrays(GL_TRIANGLES, 0, objVboLen_island/8);
    //texture_stone3
    GLint textureS3Loc = glGetUniformLocation(program_island, "FragTexture_stone3");
    glUniform1i(textureS3Loc, 2);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, texture_stone3);
    //glDrawElements(GL_TRIANGLES, objVboLen_island/8, GL_UNSIGNED_INT, 0);
    glDrawArrays(GL_TRIANGLES, 0, objVboLen_island/8);

    glBindBuffer(GL_ARRAY_BUFFER, 0); 

}

// Verwenden des Shader-Programms für die Restteil-Rendering
void draw2(void) {

    glUseProgram(program_restTeil);       

    //Uniform: Proj-Matrix (View ohne Perspektion -> View mit Perspektion)
    GLint projLoc = glGetUniformLocation(program_restTeil, "proj");
    GLfloat perspectiveMatrix[16];
    perspective(perspectiveMatrix, 60.0f/180.f*M_PI, 4.0f/3.0f, 0.1f, 100.0f);
    glUniformMatrix4fv(projLoc, 1, GL_TRUE, perspectiveMatrix);

    //Uniform: View-Matrix (World -> View)
    GLint viewLoc = glGetUniformLocation(program_restTeil, "view");
    GLfloat lookAtMatrix[16];
    GLfloat eye[3] = {0.0f,1.0f,8.0f};
    GLfloat center[3] = {0.0f,0.0f,0.0f};
    GLfloat up[3] = {0.0f,1.0f,0.0f};
    lookAt(lookAtMatrix, eye, center, up);
    glUniformMatrix4fv(viewLoc, 1, GL_TRUE, lookAtMatrix);    
    
    //Uniform: World-Matrix (Lokale -> World)
    GLint worldLoc = glGetUniformLocation(program_restTeil, "world");

    GLfloat iMatrix[16];
    GLfloat rMatrix[16];
    identity(iMatrix);
    rotatey(rMatrix, iMatrix, -90.0f/180.f*M_PI);

    //*Animation 1: fan

    // Modelltransformation für jedes spezifische Objekt (repräsentiert durch die worldLoc-Matrix)
    // Dies beinhaltet die eigene Rotation, Skalierung und Verschiebung des Objekts, also die lokale Transformation

    //(1) Initialisierung
    GLfloat translateToOrigin[16];   // Matrix zum Verschieben zum Ursprung
    GLfloat translateBack[16];       // Matrix zum Zurückverschieben zur Ausgangsposition
    GLfloat rotationMatrixFan[16];   // Rotationsmatrix
    identity(translateToOrigin);
    identity(translateBack);
    identity(rotationMatrixFan);

    // (2) Berechnung des Zentrumpunkts des Objekts
    GLfloat objectCenter[3];
    objectCenterRechnen(objVbo_fan, &objVboLen_fan, objectCenter);
    printf("\nFan centerPunkt: %f, %f, %f\n\n", objectCenter[0], objectCenter[1], objectCenter[2]);

    // (3) Erzeugung der Verschiebe- und Rotationsmatrizen für das Objekt
    GLfloat verschiebZuOrigin[3] = {-objectCenter[0], -objectCenter[1], -objectCenter[2]};
    GLfloat verschiebZurueck[3] = {objectCenter[0], objectCenter[1], objectCenter[2]};
    translate(translateToOrigin, iMatrix, verschiebZuOrigin); // Verschiebung zum Ursprung
    translate(translateBack, iMatrix, verschiebZurueck);      // Rückverschiebung zur Ausgangsposition
    rotatex(rotationMatrixFan, iMatrix, rotationAngleFan);    // Rotation
    rotationAngleFan += 0.04f;

     // (4) Gesamt-Transformationsmatrix für das Objekt (Verschiebung + Rotation)
    GLfloat fanTransformationMatrix[16];
    identity(fanTransformationMatrix);
    matrix_multiply(fanTransformationMatrix, translateToOrigin, fanTransformationMatrix);  // Verschiebung zum Ursprung
    matrix_multiply(fanTransformationMatrix, rotationMatrixFan, fanTransformationMatrix);  // Rotation
    matrix_multiply(fanTransformationMatrix, translateBack, fanTransformationMatrix);      // Rückverschiebung

    //*Animation 2: restTeil

    GLfloat ganzlandTransformationMatrix[16];
    identity(iMatrix);
    rotatey(ganzlandTransformationMatrix, iMatrix, -rotationAngleGanzIsland); // Rotation der gesamten Insel um die Y-Achse
    rotationAngleGanzIsland += 0.01f;

    //Beleuchtung UniformPara Konfikurieren
    uniformParaFuerBeleuchtung(&program_restTeil);

    //*Animation 3: Rotation around crystalCenter
    GLfloat radiusAroundCrystal = 3.0;  // Rotationsradius
    GLfloat crystalCenter[3] = {-0.0f, 0.0f, 0.0f};

    rotationAngleIslandAroundEnergie += 0.01; // Aktualisierung des Winkels

    GLfloat islandPosX = crystalCenter[0] + radiusAroundCrystal * cos(rotationAngleIslandAroundEnergie);
    GLfloat islandPosZ = crystalCenter[2] + radiusAroundCrystal * sin(rotationAngleIslandAroundEnergie);
    GLfloat XZPosition[3]={islandPosX, crystalCenter[1], islandPosZ};

    GLfloat islandTransformationMatrix[16];
    identity(islandTransformationMatrix);
    translate(islandTransformationMatrix, islandTransformationMatrix, XZPosition);
  
     GLfloat finalTransformationMatrix[16];
     identity(finalTransformationMatrix);
     matrix_multiply(finalTransformationMatrix, islandTransformationMatrix, ganzlandTransformationMatrix);
    

    //vao
    GLint status;
    glValidateProgram(program_restTeil);   
                                
    glGetProgramiv(program_restTeil, GL_VALIDATE_STATUS, &status); 
    if (!status) {
        printf("Error validating program_restTeil:");
        GLchar infoLog[1024];
        glGetProgramInfoLog(program_restTeil, 1024, NULL, infoLog);
        printf("%s",infoLog);
    }
    
   //Opaque Object
    glEnable(GL_DEPTH_TEST);
    //Bleuchtung mit umdrehen: NormalenMatrix für ganz Island
    // • N = view * world = matrix_multiply(out,lookAtMatrix,arMatrix);
    // • N(4x4) -> M(3x3) -> inverse -> transponieren 
    normalenMatrix(&program_restTeil, lookAtMatrix, finalTransformationMatrix);

    //(1) Platform
    glBindVertexArray(vao_platform);    
    glBindBuffer(GL_ARRAY_BUFFER, vbo_platform);
    glUniformMatrix4fv(worldLoc, 1, GL_TRUE, finalTransformationMatrix); //ganzlandTransformationMatrix
    //texture_platform
    GLint texturePLoc = glGetUniformLocation(program_restTeil, "FragTexture_restTeil");
    glUniform1i(texturePLoc, 3);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, texture_platform);
    //draw: platform mit texture
    glDrawArrays(GL_TRIANGLES, 0, objVboLen_platform/8); 
    //Ressource freigeben
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //(2) Haus
    glBindVertexArray(vao_haus);    
    glBindBuffer(GL_ARRAY_BUFFER, vbo_haus);
    glUniformMatrix4fv(worldLoc, 1, GL_TRUE, finalTransformationMatrix);
    //texture_haus
    GLint textureHausLoc = glGetUniformLocation(program_restTeil, "FragTexture_restTeil");
    glUniform1i(textureHausLoc, 4);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, texture_haus);
    //draw: haus mit texture
    glDrawArrays(GL_TRIANGLES, 0, objVboLen_haus/8); 
    //Ressource freigeben
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //(3) Tree
    glBindVertexArray(vao_tree);    
    glBindBuffer(GL_ARRAY_BUFFER, vbo_tree);
    glUniformMatrix4fv(worldLoc, 1, GL_TRUE, finalTransformationMatrix);
    //texture_tree
    GLint textureTreeLoc = glGetUniformLocation(program_restTeil, "FragTexture_restTeil");
    glUniform1i(textureTreeLoc, 5);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, texture_tree);
    //draw: tree mit texture
    glDrawArrays(GL_TRIANGLES, 0, objVboLen_tree/8); 
    //Ressource freigeben
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //Transparent Object
    //(4) Fan
    glBindVertexArray(vao_fan);    
    glBindBuffer(GL_ARRAY_BUFFER, vbo_fan);

    //Bleuchtung mit umdrehen: NormalenMatrix für fan
    // • N = view * world = matrix_multiply(out,lookAtMatrix,arMatrix);
    // • N(4x4) -> M(3x3) -> inverse -> transponieren 
    normalenMatrix(&program_restTeil, lookAtMatrix, fanTransformationMatrix);

    //WorldKoordinaten anpassen
    GLfloat fantransform[16];
    matrix_multiply(fantransform, finalTransformationMatrix, fanTransformationMatrix);
    glUniformMatrix4fv(worldLoc, 1, GL_TRUE, fantransform); //fanTransformationMatrix

    //texture_fan
    GLint textureFanLoc = glGetUniformLocation(program_restTeil, "FragTexture_restTeil");
    glUniform1i(textureFanLoc, 6);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, texture_fan);
    //draw: fan mit texture
    // * Transparante Object
    drawSortetTransparenteObjects(&objVboLen_fan, &vbo_fan);
    //Ressource freigeben
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}

// Verwenden des Shader-Programms für die energyObject-Rendering
void draw3(void) {
    glUseProgram(program_energyObject);        
    //Uniform: Proj-Matrix (View ohne Perspektion -> View mit Perspektion)
    GLint projLoc = glGetUniformLocation(program_energyObject, "proj");
    GLfloat perspectiveMatrix[16];
    perspective(perspectiveMatrix, 60.0f/180.f*M_PI, 4.0f/3.0f, 0.1f, 100.0f);
    glUniformMatrix4fv(projLoc, 1, GL_TRUE, perspectiveMatrix);
    
    //Uniform: View-Matrix (World -> View)
    GLint viewLoc = glGetUniformLocation(program_energyObject, "view");
    GLfloat lookAtMatrix[16];
    GLfloat eye[3] = {0.0f,2.5f,8.0f};
    GLfloat center[3] = {0.0f,0.0f,0.0f};
    GLfloat up[3] = {0.0f,1.0f,0.0f};
    lookAt(lookAtMatrix, eye, center, up);
    glUniformMatrix4fv(viewLoc, 1, GL_TRUE, lookAtMatrix);   

    //Uniform: World-Matrix (Lokale -> World)
    GLint worldLoc = glGetUniformLocation(program_energyObject, "world");
    GLfloat iMatrix[16];
    GLfloat tMatrix[16];
    identity(iMatrix);
    GLfloat verschieben[3] = {0.0f, 0.0f, 0.0f};
    translate(tMatrix, iMatrix, verschieben);   

    //Animation: energieObject

    //(1)Animation des Energieobjekts: Initialisierung
    GLfloat translateToOrigin[16];   // Matrix für die Verschiebung zum Ursprung
    GLfloat translateBack[16];       // Matrix für die Rückverschiebung
    GLfloat rotationMatrixEnergieObject[16];   // Rotationsmatrix
    identity(translateToOrigin);
    identity(translateBack);
    identity(rotationMatrixEnergieObject);

    //(2)Bestimmung des Mittelpunkts des Objekts
    GLfloat objectCenter[3];
    objectCenterRechnen(objVbo_energyObject, &objVboLen_energyObject, objectCenter); 
    printf("\nEnergyObject centerPunkt: %f, %f, %f\n\n", objectCenter[0], objectCenter[1], objectCenter[2]);

    //(3)Erstellen der Verschiebungs- und Rotationsmatrizen für das Objekt
    GLfloat verschiebZuOrigin[3] = {-objectCenter[0], -objectCenter[1], -objectCenter[2]};
    GLfloat verschiebZurueck[3] = {objectCenter[0], objectCenter[1], objectCenter[2]};
    translate(translateToOrigin, iMatrix, verschiebZuOrigin); 
    translate(translateBack, iMatrix, verschiebZurueck);      
    rotatey(rotationMatrixEnergieObject, iMatrix, rotationAngleEnergieObject);   
    rotationAngleEnergieObject += 0.02f;

    //(4)// Zusammensetzen der Gesamttransformationsmatrix für das Objekt
    GLfloat energieObjectTransformationMatrix[16];
    identity(energieObjectTransformationMatrix);
    matrix_multiply(energieObjectTransformationMatrix, translateToOrigin, energieObjectTransformationMatrix);            // Verschiebung zum Ursprung
    matrix_multiply(energieObjectTransformationMatrix, rotationMatrixEnergieObject, energieObjectTransformationMatrix);  // Rotation
    matrix_multiply(energieObjectTransformationMatrix, translateBack, energieObjectTransformationMatrix);                // Verschiebung zurück

    matrix_multiply(energieObjectTransformationMatrix, tMatrix, energieObjectTransformationMatrix);
    glUniformMatrix4fv(worldLoc, 1, GL_TRUE, energieObjectTransformationMatrix);

    // Konfiguration der Beleuchtungsparameter
    uniformParaFuerBeleuchtung(&program_energyObject);

    //// Berechnung der Normalenmatrix
    // • N = view * world = matrix_multiply(out,lookAtMatrix,arMatrix);
    // • N(4x4) -> M(3x3) -> inverse -> transponieren 
    normalenMatrix(&program_energyObject, lookAtMatrix, energieObjectTransformationMatrix);
    
    glBindVertexArray(vao_energyObject);       // Binden des Vertex-Array-Objekts für das Energieobjekt
    GLint status;
    glValidateProgram(program_energyObject);   
                              
    glGetProgramiv(program_energyObject, GL_VALIDATE_STATUS, &status); 
    if (!status) {
        printf("Error validating program:");
        GLchar infoLog[1024];
        glGetProgramInfoLog(program_energyObject, 1024, NULL, infoLog);
        printf("%s",infoLog);
    }

    //Opaque Object: Depth Test einschalten
    glEnable(GL_DEPTH_TEST);

    //draw: energyObject
    glBindBuffer(GL_ARRAY_BUFFER, vbo_energyObject);
    glDrawArrays(GL_TRIANGLES, 0, objVboLen_energyObject/8); 
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

}

    //Kamera Position
    GLfloat eye_skybox[3] = {
        1.0f, 1.0f, 1.0f
    };
    //Kamera Richtung
    GLfloat center_skybox[3] = {
        0.1f, 0.1f, 0.1f
    };
    //Kamera Up
    GLfloat up_skybox[3] = {
        0.0f, 1.0f, 0.0f
    };

// Verwendung des Skybox-Shader-Programms
void draw4(void){
    glUseProgram(program_skybox);
    
    // Definition von Sicht- und Projektionsmatrizen
    GLfloat viewMatrix[16], perMatrix[16];
    float identityMatrix[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    // Berechnung der Sicht- und Projektionsmatrizen
    lookAt(viewMatrix, eye_skybox, center_skybox, up_skybox);
    perspective(perMatrix, 45.0f, 1.0f, 0.1f, 100.0f);

    // Binden des Skybox-Vertex-Array-Objekts
    glBindVertexArray(vao_skybox);

    // Überprüfung des Shader-Programms
    GLint status;
    glValidateProgram(program_skybox);
    glGetProgramiv(program_skybox, GL_VALIDATE_STATUS, &status);
    if (!status) {
        printf("Error validating program:");
        GLchar infoLog[1024];
        glGetProgramInfoLog(program_skybox, 1024, NULL, infoLog);
        printf("%s", infoLog);
    }

    // Binden des Skybox-Vertex-Buffer-Objekts
    glBindBuffer(GL_ARRAY_BUFFER, vbo_skybox);

    // Aktivierung des Tiefentests
    glEnable(GL_DEPTH_TEST);

    // Einstellen und Binden der Uniform-Matrix-Variablen
    int modelLoc = glGetUniformLocation(program_skybox, "modelMatrix");
    int viewLoc = glGetUniformLocation(program_skybox, "viewMatrix");
    int projLoc = glGetUniformLocation(program_skybox, "projectionMatrix");

    glUniformMatrix4fv(modelLoc, 1, GL_TRUE, identityMatrix);
    glUniformMatrix4fv(viewLoc, 1, GL_TRUE, viewMatrix);
    glUniformMatrix4fv(projLoc, 1, GL_TRUE, perMatrix);

    // Einstellung der Textur
    glActiveTexture(GL_TEXTURE9);
    glBindTexture(GL_TEXTURE_2D, texture_skybox);
    glUniform1i(glGetUniformLocation(program_skybox, "texture_sky"), 9);

    // Deaktivierung des Tiefenschreibens
    glDepthMask(GL_FALSE);

    // Zeichnen der Skybox
    glDrawArrays(GL_TRIANGLES, 0, objVboLen_skybox / 8);

    // Reaktivierung des Tiefenschreibens
    glDepthMask(GL_TRUE);

    // Aufräumen
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}

// Zeichnet ein Quad für Post-Processing-Effekte
void RenderQuad() {
    float quadVertices[] = {
        // Positionen    // Texturkoordinaten
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    unsigned int quadVBO, quadVAO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    // Position Attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Texturkoordinaten Attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // Entbinden von VAO und VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Zeichnen eines Rechtecks aus zwei Dreiecken
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6); 
    glBindVertexArray(0);
}

// Hauptzeichnungsfunktion, die verschiedene Rendering-Prozesse durchführt
void draw(void) {

    // Bloom-Effekt

    // 1. Initialisierung von Framebuffer-Objekt (FBO) und Texturen
    GLuint hdrFBO;
    glGenFramebuffers(1, &hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

    // Erstellung der ersten Textur
    GLuint texture_screen;
    glGenTextures(1, &texture_screen);
    glBindTexture(GL_TEXTURE_2D, texture_screen);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    // Setzen der Texturparameter für die erste Textur
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_screen, 0);

    // Erstellung der zweiten Textur für Helligkeitseffekte
    GLuint texture_brightness;
    glGenTextures(1, &texture_brightness);
    glBindTexture(GL_TEXTURE_2D, texture_brightness);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Attach texture to framebuffer 2
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, texture_brightness, 0);
    // Erstellung eines Renderbuffers für die Tiefenkomponente
    GLuint rbo;
    glGenRenderbuffers(1, &rbo); //glCreateRenderbuffers durch glGenRenderbuffers erstatten
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // Überprüfung, ob der Framebuffer vollständig ist
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("Framebuffer is not complete!\n");
    }

    // 2. Konfiguration von OpenGL, um in mehrere Farbpuffer zu rendern
    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    // 3. Speichern des gerenderten Frames in die Texturen
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO); 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw4();  // Ausführen der Zeichnungsoperation
    draw1(); 
    draw2();
    draw3();
    

    // Ping-pong Technik, um die Textur für Helligkeitseffekte zu verwischen
    glActiveTexture(GL_TEXTURE0);
    GLuint pingpongFBO[2];
    GLuint pingpongBuffers[2]; 
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongBuffers);
    for (int i = 0; i < 2; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongBuffers[i]);
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffers[i], 0
        ); 
    }

    // Unschärfe der Helligkeitstextur
    bool horizontal = true, first_iteration = true;
    int amount = 20;
    glUseProgram(program_blur);
    for (unsigned i = 0; i < amount; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]); 
        glUniform1i(glGetUniformLocation(program_blur, "horizontal"), horizontal);
        GLint textureBlurLoc = glGetUniformLocation(program_blur, "image");
        glUniform1i(textureBlurLoc, 0);
        glActiveTexture(GL_TEXTURE0);
        if (first_iteration) {
            glBindTexture(GL_TEXTURE_2D, texture_brightness);
            first_iteration = false;
        } else {
            glBindTexture(GL_TEXTURE_2D, pingpongBuffers[!horizontal]);
        }
        glClear(GL_COLOR_BUFFER_BIT);
        // glBindTexture(
        //     GL_TEXTURE_2D, first_iteration ? texture_brightness : pingpongBuffers[!horizontal]
        // );
        RenderQuad();
        horizontal = !horizontal;
        // if (first_iteration)
        //     first_iteration = false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0); 

    // 3. Bloom-Effekt anwenden
    glUseProgram(program_fullscreen);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Binden der Textur: Originalbildschirm
    GLint textureScrLoc = glGetUniformLocation(program_fullscreen, "screenTexture");
    glUniform1i(textureScrLoc, 10);
    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D, texture_screen);
    GLint textureBloomLoc = glGetUniformLocation(program_fullscreen, "bloomBlurTexture"); 
    glUniform1i(textureBloomLoc, 11);
    glActiveTexture(GL_TEXTURE11);
    glBindTexture(GL_TEXTURE_2D, pingpongBuffers[!horizontal]);  //!!!!
    RenderQuad();

    //Ressource freigeben
    glDeleteFramebuffers(1, &hdrFBO);
    glDeleteTextures(1, &texture_screen);
    glDeleteTextures(1, &texture_brightness);
    glDeleteFramebuffers(2, pingpongFBO);
    glDeleteTextures(2, pingpongBuffers);
 
}

void framebuffer_size_callback(GLFWwindow *window, int w, int h) {
    width = w;
    height = h;
    glViewport(0, 0, width, height);// x und y sind die Koordinaten der unteren linken Ecke des Viewports im Fenster
                                     // width und height sind die Breite und Höhe des Viewports
                                     // um sicherzustellen, dass der OpenGL-Renderbereich mit der neuen Fenstergröße übereinstimmt
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    float cameraSpeed = 0.05;
    // Steuerung der Kamerabewegung
    //FORWARD
    if (key == GLFW_KEY_W) {
        eye_skybox[2] += cameraSpeed;
        
    }
    
    //BACKWORD
    if (key == GLFW_KEY_S) {
        eye_skybox[2] -= cameraSpeed;
    }

    //UPWARD
    if (key == GLFW_KEY_A) {
        eye_skybox[1] += cameraSpeed;
    }

    //DOWNWARD
    if (key == GLFW_KEY_D) {
        eye_skybox[1] -= cameraSpeed;
    }

    //LEFT
    if (key == GLFW_KEY_Q){
        eye_skybox[0] -= cameraSpeed;
    }

    //RIGHT
    if (key == GLFW_KEY_E){
        eye_skybox[0] += cameraSpeed;
    } 
}

int main(void) {
    printf("Hallo!\n");

    glfwInit(); 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); 
                                                   
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 

    GLFWwindow *window = glfwCreateWindow(800, 600, "Computergrafik 1", NULL, NULL); 
    if (!window) { 
        printf("Failed to create Window\n");
        glfwTerminate(); 
                        
        return -1;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); 
                                                                      
    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
    glewInit(); 

    init1();
    init2();
    init3();
    init4();
    initBloom();

    rotationAngleEnergieObject = 0.0f;
    rotationAngleFan = 0.0f;
    rotationAngleGanzIsland = 0.0f;
    while (!glfwWindowShouldClose(window)) { 
        draw();
        glfwSwapBuffers(window); 
        glfwPollEvents(); 
    }

    //Ressource freigeben
    glDeleteTextures(1, &texture_stone1);
    glDeleteTextures(1, &texture_stone2);
    glDeleteTextures(1, &texture_stone3);
    glDeleteTextures(1, &texture_fan);
    glDeleteTextures(1, &texture_haus);
    glDeleteTextures(1, &texture_platform);
    glDeleteTextures(1, &texture_tree);
    glDeleteTextures(1, &texture_skybox);

    glDeleteProgram(program_island);
    glDeleteProgram(program_restTeil);
    glDeleteProgram(program_energyObject);
    glDeleteProgram(program_fullscreen);
    glDeleteProgram(program_blur);
    glDeleteProgram(program_skybox);

    glfwTerminate();
    return 0;
}
