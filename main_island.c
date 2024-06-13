#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdlib.h>
#include "vertexShader_island.h"
#include "fragmentShader_island.h"
#include "vertexShader_restTeil.h"
#include "fragmentShader_restTeil.h"
#include "vertexShader_energyObject.h"
#include "fragmentShader_energyObject.h"

#define STB_IMAGE_IMPLEMENTATION
#include "shader_Programm/stb_image.h"

//Learn OpenGL Bloom

GLuint program_island; //selbst definierte Shader-Programm
GLuint program_restTeil;
GLuint program_energyObject;
GLuint vao_island;     //vao 是 OpenGL 中一种状态对象，用于存储顶点数组相关的状态，包括顶点属性指针的设置和启用状态等
GLuint vbo_island;     //创建缓冲对象标识符/变量(用来存储缓冲对象)，此时该变量相当于NULL
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
float rotationAngleFan; //Um objekt umdrehen zu können 
float rotationAngleGanzIsland;
float rotationAngleEnergieObject;
GLuint texture_stone1;
GLuint texture_stone2;
GLuint texture_stone3;
GLuint texture_platform;
GLuint texture_tree;
GLuint texture_fan;
GLuint texture_haus;

//Hilfefunktion: Matrix verarbeiten

//(1) Matrix normieren
void normierung(GLfloat *out, GLfloat* in){
    GLfloat vactorX = in[0];
    GLfloat vactorY = in[1];
    GLfloat vactorZ = in[2];
    GLfloat gross = sqrtf(vactorX*vactorX + vactorY*vactorY + vactorZ*vactorZ);
    out[0] = vactorX / gross;
    out[1] = vactorY / gross;
    out[2] = vactorZ / gross;
}
//(2) Matrix korpieren
void kopieren (GLfloat* neu, GLfloat* alt) {
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            neu[4*i+j]=alt[4*i+j];
        }
    }
}
//(3) Matrix multiplikation
void  matrix_multiply(GLfloat* result, GLfloat* a, GLfloat* b) {
    GLfloat m1[16];
    GLfloat m2[16];
    kopieren(m1,a);
    kopieren(m2,b);
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result[i * 4 + j] = 0.0f; //inizialisieren
            for (int k = 0; k < 4; k++) {
                result[i * 4 + j] += m1[i * 4 + k] * m2[k * 4 + j];
            }
        }
    }
}

//Funktion: Zu World-Koordinatensystem gehen
//   Szene transformieren, um Kamera im Ursprung zu bleiben
//   就是有 eye, center, up的偏差，要利用out把这些偏差纠正过来
void lookAt(GLfloat* out, GLfloat* eye, GLfloat* center, GLfloat* up) {
    GLfloat eyeX = eye[0]; //Eye
    GLfloat eyeY = eye[1]; 
    GLfloat eyeZ = eye[2]; 
    GLfloat lookX = center[0]; //Look
    GLfloat lookY = center[1]; 
    GLfloat lookZ = center[2]; 

    //1.Parameter: n = eye - look
    GLfloat n[3] = { eyeX-lookX, eyeY-lookY, eyeZ-lookZ};
    // GLfloat nn[] = {
    //     eyeX-lookX,       0,               0,
    //     0,           eyeY-lookY,           0,
    //     0,                0,          eyeZ-lookZ
    // };
    GLfloat nn[3];
    normierung(nn,n);
    GLfloat nnX = nn[0]; // ax
    GLfloat nnY = nn[1]; // ay
    GLfloat nnZ = nn[2]; // az

    //2.Parameter: u  = up • n
    //             up = (0,1,0)
    //  u'= (
    //     n'z, 
    //     0, 
    //     -n'x
    //  )
    // GLfloat unX = nnZ; 
    // GLfloat unY = 0.0f;
    // GLfloat unZ = -nnX;
    GLfloat upn[3];
    normierung(upn,up);
    GLfloat unX = upn[1] * nnZ - nnY * upn[2];
    GLfloat unY = upn[2] * nnX - upn[0] * nnZ;
    GLfloat unZ = upn[0] * nnY - nnX * upn[1];

    //3.Parameter: v
    //  v'= (
    //     -n'x•n'y, 
    //     (n'x)^2 + (n'y)^2, 
    //     -n'y • n'z
    //  )
    // GLfloat vnX = -nnX * nnY; //v[0] = n[1] * u[2] - u[1] * n[2];
    // GLfloat vnY = nnX*nnX + nnY*nnY;
    // GLfloat vnZ = -nnY * nnZ;
    //3.v=n*u
    GLfloat vnX = nnY * unZ - unY * nnZ; //v[0] = n[1] * u[2] - u[1] * n[2];
    GLfloat vnY = nnZ * unX - nnX * unZ; //v[1] = n[0] * u[2] - u[0] * n[2];
    GLfloat vnZ = nnX * unY - unX * nnY; //v[2] = n[0] * u[1] - u[0] * n[1];

    //3.Parameter: v
    //   tx=-u' • eye
    //   ty=-v' • eye
    //   tz=-n' • eye
    GLfloat tX = -unX * eyeX - unY * eyeY - unZ * eyeZ;
    GLfloat tY = -vnX * eyeX - vnY * eyeY - vnZ * eyeZ;
    GLfloat tZ = -nnX * eyeX - nnY * eyeY - nnZ * eyeZ;

    //Inverse Transformations
    //        out
    //| u'x  u'y  u'z  tx |  
    //| v'x  v'y  v'z  ty |  
    //| n'x  n'y  n'z  tz |  
    //|  0    0    0    1 |
    GLfloat result[] = {
        unX,  unY,  unZ,  tX,
        vnX,  vnY,  vnZ,  tY,
        nnX,  nnY,  nnZ,  tZ,
        0.0f, 0.0f, 0.0f, 1.0f
    };
    kopieren(out, result);
}

//Funktion: "view frustrum": Abbildung "Pyramidenstumpf" auf "Würfel"
//  投影: (x,y,z)->(1,1,1)
void perspective(GLfloat* out, GLfloat fovy, GLfloat aspect, GLfloat near, GLfloat far) {
    //1. wir haben schon 
    //   n, f
    //2. t = n•tan(fovy/2)
    GLfloat t = near * tanf(fovy/2.0f);
    //3. r = aspect * t
    GLfloat r = aspect * t;

    //                 M
    //| n/r   0        0            0      |  
    //|  0   n/t       0            0      |  
    //|  0    0   (f+n)/(n-f)  (2fn)/(n-f) |  
    //|  0    0        -1           0      |
    GLfloat result[] = {
        near/r,   0.0f,            0.0f,                     0.0f,
        0.0f,    near/t,           0.0f,                     0.0f,
        0.0f,     0.0f,   (far+near)/(near-far),  (2*far*near)/(near-far),
        0.0f,     0.0f,           -1.0f,                     0.0f
    };
    kopieren(out, result);
}

//Test
void printMatrix4x4(GLfloat* out) {
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            printf("%f ",out[i*4+j]);
        }
        printf("\n");
    }
}
void printMatrix3x3(GLfloat* out) {
    for (int i=0; i<3; i++) {
        for (int j=0; j<3; j++) {
            printf("%f, ",out[i*3+j]);
        }
        printf("\n");
    }
}

//Funktion: Zu View-Koordinatensystem gehen
//(0)Identify
void identity(GLfloat* out) {
    // out = 1,0,0,0
    //       0,1,0,0
    //       0,0,1,0
    //       0,0,0,1
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (i == j) {
                out[i * 4 + j] = 1.0f; //i==j时，为1.0
            } else {
                out[i * 4 + j] = 0.0f;
            }
        }
    }
}
//(1)Translation
void translate(GLfloat* out, GLfloat* in, GLfloat* v) {
    
    // Der Vektor v ist ein 3-Komponenten-Vektor
    GLfloat x = v[0]; // ax
    GLfloat y = v[1]; // ay
    GLfloat z = v[2]; // az
 
    //    v          T(v)            in              out
    //    ax     | 1 0 0 ax |    | x 0 0 0 |     x+ax  0 0 0
    //    by     | 0 1 0 ay |    | y 0 0 0 |     y+ay  0 0 0
    //    az     | 0 0 1 az |    | z 0 0 0 |     z+az  0 0 0
    //           | 0 0 0 1  |    | 1 0 0 0 |     1     0 0 0

    //T(v)
    GLfloat T[16] = {
       1.0f,0.0f,0.0f,x,
       0.0f,1.0f,0.0f,y,
       0.0f,0.0f,1.0f,z,
       0.0f,0.0f,0.0f,1.0f
    };

    // out = T(v) * in 
    matrix_multiply(out,T,in);
}
//(2)Skalierung
void scale(GLfloat* out, GLfloat* in, GLfloat* v) {

    // Der Vektor v ist ein 3-Komponenten-Vektor
    GLfloat x = v[0]; // ax
    GLfloat y = v[1]; // ay
    GLfloat z = v[2]; // az
 
    //    v          T(v)            in              out
    //    ax     | 1 0 0 ax |    | x 0 0 0 |     x+ax  0 0 0
    //    by     | 0 1 0 ay |    | y 0 0 0 |     y+ay  0 0 0
    //    az     | 0 0 1 az |    | z 0 0 0 |     z+az  0 0 0
    //           | 0 0 0 1  |    | 1 0 0 0 |     1     0 0 0

    //T(v)
    GLfloat S[16] = {
       x,0.0f,0.0f,0.0f,
       0.0f,y,0.0f,0.0f,
       0.0f,0.0f,z,0.0f,
       0.0f,0.0f,0.0f,1.0f
    };

    // out = T(v) * in 
    matrix_multiply(out,S,in);

}
//(3)Rotation
void rotatex(GLfloat* out, GLfloat* in, GLfloat angle) {

   //R(v)
    GLfloat R[16] = {
       1.0f,0.0f,0.0f,0.0f,
       0.0f,cosf(angle),-sinf(angle),0.0f,
       0.0f,sinf(angle),cosf(angle),0.0f,
       0.0f,0.0f,0.0f,1.0f
    };

    // out = T(v) * in 
    matrix_multiply(out,R,in);

}
void rotatey(GLfloat* out, GLfloat* in, GLfloat angle) {

   //R(v)
    GLfloat R[16] = {
       cosf(angle),0.0f,sinf(angle),0.0f,
       0.0f,1.0f,0.0f,0.0f,
       -sinf(angle),0.0f,cosf(angle),0.0f,
       0.0f,0.0f,0.0f,1.0f
    };

    // out = T(v) * in 
    matrix_multiply(out,R,in);

}
void rotatez(GLfloat* out, GLfloat* in, GLfloat angle) {

   //R(v)
    GLfloat R[16] = {
       cosf(angle),-sinf(angle),0.0f,0.0f,
       sinf(angle),cosf(angle),0.0f,0.0f,
       0.0f,0.0f,1.0f,0.0f,
       0.0f,0.0f,0.0f,1.0f
    };

    // out = T(v) * in 
    matrix_multiply(out,R,in);

}

//Hilfefunktion: Obj-Datei lesen

//(1) Benötigte Datenstruktur definieren
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
//(2) OBJ-Datei lesen
//Eingabe: (1) Zu ledense OBJ-Datei-Name, (2) leere vbo-objekt-Zeiger, (3) leere vbo-Große
//Ausgabe: (1) Ausgefüllte vbo-objekt-Zeiger, (2) Ausgefüllte vbo-Große
void readOBJ(const char* filename, Flaeche8f** vboOut, long* vboLen) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Datei konnte nicht geöffnet werden.");
        return;
    }

    // Listen für Vertex-Daten
    Vector3f* objV = NULL;  // Vertex 顶点
    int vNumber = 0;
    // Listen für Vertex-Textur
    Vector2f* objVT = NULL;  // Textur 纹理
    int vtNumber = 0;
    // Listen für Vertex-Normal
    Vector3f* objVN = NULL;  // Normal 法线
    int vnNumber = 0;
    // face
    Vector9fi* objVF = NULL;  // face 面
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
    //(3) 

    // printf("face: \n");
    // for (int i = 0; i < vfNumber; i++) {
    //     printf("%d %d %d\n", objVF[i].v1, objVF[i].t1, objVF[i].n1);
    //     printf("%d %d %d\n", objVF[i].v2, objVF[i].t2, objVF[i].n2);
    //     printf("%d %d %d\n", objVF[i].v3, objVF[i].t3, objVF[i].n3);
    //     printf("\n");
    // }

    // face 面
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

// (1) Inverse-Matrix3x3 Berechnen
void inverseMatrix3x3(float* result, float* mat) {
    float det = mat[0] * (mat[4]*mat[8] - mat[5]*mat[7]) -
                mat[1] * (mat[3]*mat[8] - mat[5]*mat[6]) +
                mat[2] * (mat[3]*mat[7] - mat[4]*mat[6]);
    if (det == 0.0) {
        return;
    }
    float invDet = 1.0f / det;

    result[0] = (mat[4]*mat[8] - mat[5]*mat[7]) * invDet;
    result[3] = (mat[5]*mat[6] - mat[3]*mat[8]) * invDet;
    result[6] = (mat[3]*mat[7] - mat[4]*mat[6]) * invDet;
    result[1] = (mat[2]*mat[7] - mat[1]*mat[8]) * invDet;
    result[4] = (mat[0]*mat[8] - mat[2]*mat[6]) * invDet;
    result[7] = (mat[1]*mat[6] - mat[0]*mat[7]) * invDet;
    result[2] = (mat[1]*mat[5] - mat[2]*mat[4]) * invDet;
    result[5] = (mat[2]*mat[3] - mat[0]*mat[5]) * invDet;
    result[8] = (mat[0]*mat[4] - mat[1]*mat[3]) * invDet;
}
// (2) Matrix4x4 -> Matrix3x3
void normalMatrixFromMatrix4x4(float* normalMat, float* mat) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            normalMat[i*3+j] = mat[i*4+j];
        }
    }
}
// (3) Transponieren
void transponieren(float* transpMatrix, float* mat) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            transpMatrix[j*3+i] = mat[i*3+j];
        }
    }
}

//Funktion: Texture
// (1) Bilddaten->Texturobjekt, image->texture
void textureMapping(const char *str, GLuint* texture){
    //1.Externer Bilddatei laden
    stbi_set_flip_vertically_on_load(1); //Das Bild soll vertikal gespiegelt werden.
    int width, height, nrChannels;
    unsigned char *image = stbi_load(str, &width, &height, &nrChannels, 4); //nrChannels 存储加载后图像的颜色通道数

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

//Hilfefunktion: Shader-Text-Identifizieren

//Bei shaderArten: VertexShader = 1
//                 FragementShader = 2
void shaderInit(GLuint *shader, unsigned char *shader_glsl, const unsigned int *shader_glsl_len, int shaderType) {
    if (shaderType == 1) {
        *shader = glCreateShader(GL_VERTEX_SHADER); 
    } else if (shaderType == 2) {
        *shader = glCreateShader(GL_FRAGMENT_SHADER); 
    } else {
        printf("Invalid Shader Type, shaderInit() break !");
        return; // 退出函数
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
        return; // 退出函数
    }
}

//Hilfefunktion: Shader-Program-Identifizieren
void shaderProgramInit(GLuint* program, GLuint* vertexShader, GLuint* fragmentShader) {
    GLint status;
    *program = glCreateProgram(); //创建着色器程序对象
    glAttachShader(*program, *vertexShader);  //附加：着色器对象-着色器程序
    glAttachShader(*program, *fragmentShader);
    glLinkProgram(*program); //将着色器程序对象(program)链接到当前 OpenGL 环境中
                            //OpenGL 将会检查各个着色器是否能够一起正确地链接成一个完整的着色器程序
                            //如果链接成功，着色器程序将会被创建，并且可以在渲染时使用
    glGetProgramiv(*program, GL_LINK_STATUS, &status); //检查：着色器程序的链接状态
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
    // for (long i=0; i<vboLen/8; i++) {
    //     if(i%3==0) {
    //         //printf("Punkt-%ld: \n", i/8);
    //         printf("Face-%ld: \n", i/3+1);
    //     }
    //     printf("v: %f %f %f, vt: %f %f, vn: %f %f %f,\n", TeaVertices[i*8], TeaVertices[i*8+1], TeaVertices[i*8+2], TeaVertices[i*8+3], TeaVertices[i*8+4], TeaVertices[i*8+5], TeaVertices[i*8+6], TeaVertices[i*8+7] );
    // }    

    GLuint VBOobject; //创建缓冲对象标识符/变量(用来存储缓冲对象)，此时该变量相当于NULL
    glGenBuffers(1, &VBOobject); //生成一个缓冲对象，并将其标识符存储到 triangleVertexBufferObject 变量中，此时该变量不为NULL了
    glBindBuffer(GL_ARRAY_BUFFER, VBOobject); //绑定缓冲对象，目标为GL_ARRAY_BUFFER(存储顶点数组数据的缓冲对象目标)
                                                               //在顶点渲染过程中，顶点着色器会从 GL_ARRAY_BUFFER 中读取顶点数据，并对顶点数据进行处理
    glBufferData(GL_ARRAY_BUFFER, sizeof(objectVertices), objectVertices, GL_STATIC_DRAW); //将顶点数据复制到当前绑定的缓冲对象中
    glBindBuffer(GL_ARRAY_BUFFER, 0); //解绑了当前绑定的缓冲对象

    //vao
    glGenVertexArrays(1, vao); //生成一个顶点数组对象，并将其标识符存储到 vao 变量中
                                //vao 用于存储顶点数组的配置
    glBindVertexArray(*vao); //绑定了顶点数组对象 vao 到 OpenGL 的当前上下文中
                            //这样后续的顶点属性指针设置和启用将会存储在这个 vao 中
    glBindBuffer(GL_ARRAY_BUFFER, VBOobject); //顶点缓冲对象triangleVertexBufferObject和该Buffer相连接，就包含了顶点数据
    //1.Vertex
    glVertexAttribPointer( //指定了顶点属性数组的位置和格式(位置)
        0,                         //指定了第一个顶点属性(索引为0), 先前顶点着色器源代码中的第一个输入参数：2D顶点位置
        3,                         //包含两个分量vec3(x,y,z)，即3D
        GL_FLOAT,                  //其中 x,y 数据类型为 GL_FLOAT
        GL_FALSE,                  
        8 * sizeof(GLfloat),       //跨度为 6 个 float (x,y,z,r,g,b)
        0                          //偏移量为 0
                                   //因为(x,y,r,g,b)中的颜色部分(x,y)是从第一个float开始的
    );
    glEnableVertexAttribArray(0);  //启用顶点属性数组，使得顶点属性数组可用于渲染过程中的顶点着色器
    //2.Normalen
    glVertexAttribPointer( //指定了顶点属性数组的位置和格式(颜色)
        1,                         //指定了第二个顶点属性(索引为1), 先前顶点着色器源代码中的第二个输入参数：3D顶点RGB颜色
        3,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(GLfloat),
        (GLvoid*)(5 * sizeof(GLfloat)) //偏移量为 3 个 GLfloat，并类型转换(指向空类型的指针)，为了保持参数的一致性 ？？？？？？？？？？？？？
                                       //在顶点数据缓冲中，当前顶点属性的数据从第三个 GLfloat 开始
                                       //因为(x,y,r,g,b)中的颜色部分(r,g,b)是从第三个float开始的                            
    );
    glEnableVertexAttribArray(1);  //启用了第二个顶点属性数组
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


    glBindBuffer(GL_ARRAY_BUFFER, 0); //解绑了当前绑定的顶点缓冲对象
    glBindVertexArray(0);
}

void init1(void) {
    //shader
    GLuint vertexShader_island;
    shaderInit(&vertexShader_island, shader_Programm_vertexShader_island_glsl, &shader_Programm_vertexShader_island_glsl_len, 1);   //VertexShader init
    GLuint fragmentShader_island;
    shaderInit(&fragmentShader_island, shader_Programm_fragmentShader_island_glsl, &shader_Programm_fragmentShader_island_glsl_len, 2); //FragmentShader init
    //shader program erzeugen und linken
    shaderProgramInit(&program_island, &vertexShader_island, &fragmentShader_island);

    //texture
    textureMapping("Ressouce/textureBild/2.Island_stone/stone1.jpeg", &texture_stone1);
    textureMapping("Ressouce/textureBild/2.Island_stone/stone2.jpeg", &texture_stone2);
    textureMapping("Ressouce/textureBild/2.Island_stone/stone3.jpeg", &texture_stone3);
    
    //obj-Datei lesen
    readOBJ("Ressouce/objDatei/islandStone.obj", &objVbo_island, &objVboLen_island);
    //VBO und VAO verarbeiten
    vboUndvao(&objVboLen_island, objVbo_island, &vao_island, 1);

    glClearColor(191.0f/255.0f,217.0f/255.0f,204.0f/255.0f,1.0); //设置窗口背景颜色
}
void init2(void) {
    //shader
    GLuint vertexShader_restTeil;
    shaderInit(&vertexShader_restTeil, shader_Programm_vertexShader_restTeil_glsl, &shader_Programm_vertexShader_restTeil_glsl_len, 1);   //VertexShader init
    GLuint fragmentShader_restTeil;
    shaderInit(&fragmentShader_restTeil, shader_Programm_fragmentShader_restTeil_glsl, &shader_Programm_fragmentShader_restTeil_glsl_len, 2); //FragmentShader init
    //shader program erzeugen und linken
    shaderProgramInit(&program_restTeil, &vertexShader_restTeil, &fragmentShader_restTeil);

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

void init3(void) {
    //shader
    GLuint vertexShader_energyObject;
    shaderInit(&vertexShader_energyObject, shader_Programm_vertexShader_energyObject_glsl, &shader_Programm_vertexShader_energyObject_glsl_len, 1);   //VertexShader init
    GLuint fragmentShader_energyObject;
    shaderInit(&fragmentShader_energyObject, shader_Programm_fragmentShader_energyObject_glsl, &shader_Programm_fragmentShader_energyObject_glsl_len, 2); //FragmentShader init
    //shader program erzeugen und linken
    shaderProgramInit(&program_energyObject, &vertexShader_energyObject, &fragmentShader_energyObject);

    //obj-Datei lesen
    readOBJ("Ressouce/objDatei/energyObject.obj", &objVbo_energyObject, &objVboLen_energyObject);
    //VBO und VAO verarbeiten
    vboUndvao(&objVboLen_energyObject, objVbo_energyObject, &vao_energyObject, 0);

    glClearColor(191.0f/255.0f,217.0f/255.0f,204.0f/255.0f,1.0); //设置窗口背景颜色
}


//Hilfefunktion: Koordinatensystem weckseln

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

//Hilfefunktion: Beleuchtung relevante uniformPara konfigurieren
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

    //Quelle Licht
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

//Hilfefunktion: NormalenMatrix
// • N = view * world = matrix_multiply(out,lookAtMatrix,arMatrix);
// • N(4x4) -> M(3x3) -> inverse -> transponieren 转置矩阵 
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

//Hilfefunktion: Object malen (opaque und transparente)

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
    glBlendColor(0.5f, 0.5f, 0.5f, 1.0f); //这个 alpha 通道决定了混合过程中源颜色的不透明度
                                          //这里的 alpha 分量为 1.0 表示完全不透明, 即源颜色完全不透明地混合到目标颜色中;
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // #define GL_SRC_ALPHA_SATURATE_EXT 0x0308
                                                       // 指定了混合的方式

    //draw                                                   
    //Buffer binden
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
    //Draw
    glDrawArrays(GL_TRIANGLES, 0, *vboLen/8); 
    //Buffer unbinden
    glBindBuffer(GL_ARRAY_BUFFER, 0); //解绑了当前绑定的缓冲对象  

    //Z-Buffer zurücksetzen
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);                                                
    glEnable(GL_DEPTH_TEST);
}

void objectCenterRechnen(Flaeche8f* objVbo, long* objVboLen, GLfloat* centerPunkt) {
    if (*objVboLen == 0) {
        centerPunkt[0] = 0.0f;
        centerPunkt[1] = 0.0f;
        centerPunkt[2] = 0.0f;
        printf("objVboLen == 0 !!!!\n");
        return;
    }

    long vertexNum = *objVboLen/8; //该 object 的顶点数量

    // 初始化中心点坐标
    centerPunkt[0] = 0.0f;
    centerPunkt[1] = 0.0f;
    centerPunkt[2] = 0.0f;

    // 累加所有顶点的坐标分量
    for (long i = 0; i < vertexNum; i++) {
        centerPunkt[0] += objVbo[i].v1;
        centerPunkt[1] += objVbo[i].v2;
        centerPunkt[2] += objVbo[i].v3;
    }

    // 计算平均值
    centerPunkt[0] /= vertexNum;
    centerPunkt[1] /= vertexNum;
    centerPunkt[2] /= vertexNum;
}

void draw1(void) {

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); //清除了颜色缓冲区，将其填充为背景色
    glUseProgram(program_island);        //使用指定的着色器程序 program 进行渲染

    //Uniform: Proj-Matrix (View ohne Perspektion -> View mit Perspektion)
    GLint projLoc = glGetUniformLocation(program_island, "proj");
    GLfloat perspectiveMatrix[16];
    perspective(perspectiveMatrix, 60.0f/180.f*M_PI, 4.0f/3.0f, 0.1f, 100.0f);
    glUniformMatrix4fv(projLoc, 1, GL_TRUE, perspectiveMatrix);

    //Uniform: View-Matrix (World -> View)
    GLint viewLoc = glGetUniformLocation(program_island, "view");
    GLfloat lookAtMatrix[16];
    GLfloat eye[3] = {0.0f,1.0f,5.0f};
    GLfloat center[3] = {0.0f,0.0f,0.0f};
    GLfloat up[3] = {0.0f,1.0f,0.0f};
    lookAt(lookAtMatrix, eye, center, up);
    glUniformMatrix4fv(viewLoc, 1, GL_TRUE, lookAtMatrix);    
    
    //Uniform: World-Matrix (Lokale -> World)
    GLint worldLoc = glGetUniformLocation(program_island, "world");
    GLfloat iMatrix[16];
    GLfloat tMatrix[16];
    GLfloat sMatrix[16];
    GLfloat rMatrix[16];
    GLfloat arMatrix[16];
    identity(iMatrix);
    //*Animation: rotation
    rotatey(arMatrix, iMatrix, -rotationAngleGanzIsland);
    glUniformMatrix4fv(worldLoc, 1, GL_TRUE, arMatrix);
    rotationAngleGanzIsland += 0.01f;

    //Beleuchtung UniformPara Konfikurieren
    uniformParaFuerBeleuchtung(&program_island);

    //NormalenMatrix
    // • N = view * world = matrix_multiply(out,lookAtMatrix,arMatrix);
    // • N(4x4) -> M(3x3) -> inverse -> transponieren 转置矩阵 
    normalenMatrix(&program_island, lookAtMatrix, arMatrix);

    //vao
    glBindVertexArray(vao_island);       //绑定了顶点数组对象 vao，指示OpenGL使用这个顶点数组对象来绘制图
    GLint status;
    glValidateProgram(program_island);   //验证着色器程序的有效性
                                  //告诉OpenGL去检查当前的着色器程序是否有效，是否可以被成功链接和使用
    glGetProgramiv(program_island, GL_VALIDATE_STATUS, &status); //检查：验证状态
    if (!status) {
        printf("Error validating program:");
        GLchar infoLog[1024];
        glGetProgramInfoLog(program_island, 1024, NULL, infoLog);
        printf("%s",infoLog);
    }

    glBindBuffer(GL_ARRAY_BUFFER,  vbo_island);
    

    //Transparent Object
//    drawSortetTransparenteObjects(&objVboLen_island, &vboBufferObject);
    
    //Opaque Object
    glEnable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLES, 0, objVboLen_island/8); 

    //texture_stone1
    GLint textureS1Loc = glGetUniformLocation(program_island, "FragTexture_stone1");
    glUniform1i(textureS1Loc, 0);
    glActiveTexture(GL_TEXTURE0);
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

    glBindBuffer(GL_ARRAY_BUFFER, 0); //解绑了当前绑定的缓冲对象

}

void draw2(void) {

    glUseProgram(program_restTeil);        //使用指定的着色器程序 program 进行渲染

    //Uniform: Proj-Matrix (View ohne Perspektion -> View mit Perspektion)
    GLint projLoc = glGetUniformLocation(program_restTeil, "proj");
    GLfloat perspectiveMatrix[16];
    perspective(perspectiveMatrix, 60.0f/180.f*M_PI, 4.0f/3.0f, 0.1f, 100.0f);
    glUniformMatrix4fv(projLoc, 1, GL_TRUE, perspectiveMatrix);

    //Uniform: View-Matrix (World -> View)
    GLint viewLoc = glGetUniformLocation(program_restTeil, "view");
    GLfloat lookAtMatrix[16];
    GLfloat eye[3] = {0.0f,1.0f,5.0f};
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

    //接下来, 针对每个具体的物体的模型变换 (由 worldLoc 矩阵表示)
    //这涉及到物体自身的旋转、缩放和平移, 即局部变换

    //(1)初始化
    GLfloat translateToOrigin[16];   // 平移到原点的矩阵
    GLfloat translateBack[16];       // 平移回初始位置的矩阵
    GLfloat rotationMatrixFan[16];   // 旋转矩阵
    identity(translateToOrigin);
    identity(translateBack);
    identity(rotationMatrixFan);

    //(2)获得该物体中点
    GLfloat objectCenter[3];
    objectCenterRechnen(objVbo_fan, &objVboLen_fan, objectCenter); //计算某个物体的中心坐标
    printf("\nFan centerPunkt: %f, %f, %f\n\n", objectCenter[0], objectCenter[1], objectCenter[2]);

    //(3)获得该物体的平移, 旋转矩阵
    GLfloat verschiebZuOrigin[3] = {-objectCenter[0], -objectCenter[1], -objectCenter[2]};
    GLfloat verschiebZurueck[3] = {objectCenter[0], objectCenter[1], objectCenter[2]};
    translate(translateToOrigin, iMatrix, verschiebZuOrigin); //初始位置挪到原点
    translate(translateBack, iMatrix, verschiebZurueck);      //原点挪到初始位置
    rotatex(rotationMatrixFan, iMatrix, rotationAngleFan);    //旋转
    rotationAngleFan += 0.04f;

    //(4)获得该物体总的 TransformationMatrix (平移+旋转)
    GLfloat fanTransformationMatrix[16];
    identity(fanTransformationMatrix);
    matrix_multiply(fanTransformationMatrix, translateToOrigin, fanTransformationMatrix);  // 平移到原点
    matrix_multiply(fanTransformationMatrix, rotationMatrixFan, fanTransformationMatrix);  // 旋转
    matrix_multiply(fanTransformationMatrix, translateBack, fanTransformationMatrix);      // 平移回去

    //*Animation 2: restTeil

    GLfloat ganzlandTransformationMatrix[16];
    identity(iMatrix);
    rotatey(ganzlandTransformationMatrix, iMatrix, -rotationAngleGanzIsland); //整个岛绕y轴旋转
    rotationAngleGanzIsland += 0.01f;

    //Beleuchtung UniformPara Konfikurieren
    uniformParaFuerBeleuchtung(&program_restTeil);

    //vao
    GLint status;
    glValidateProgram(program_restTeil);   //验证着色器程序的有效性
                                  //告诉OpenGL去检查当前的着色器程序是否有效，是否可以被成功链接和使用
    glGetProgramiv(program_restTeil, GL_VALIDATE_STATUS, &status); //检查：验证状态
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
    // • N(4x4) -> M(3x3) -> inverse -> transponieren 转置矩阵 
    normalenMatrix(&program_restTeil, lookAtMatrix, ganzlandTransformationMatrix);

    //(1) Platform
    glBindVertexArray(vao_platform);    
    glBindBuffer(GL_ARRAY_BUFFER, vbo_platform);
    glUniformMatrix4fv(worldLoc, 1, GL_TRUE, ganzlandTransformationMatrix); //ganzlandTransformationMatrix
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
    glUniformMatrix4fv(worldLoc, 1, GL_TRUE, ganzlandTransformationMatrix);
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
    glUniformMatrix4fv(worldLoc, 1, GL_TRUE, ganzlandTransformationMatrix);
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
    // • N(4x4) -> M(3x3) -> inverse -> transponieren 转置矩阵 
    normalenMatrix(&program_restTeil, lookAtMatrix, fanTransformationMatrix);

    //WorldKoordinaten anpassen
    GLfloat fantransform[16];
    matrix_multiply(fantransform, ganzlandTransformationMatrix, fanTransformationMatrix);
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

void draw3(void) {
    glUseProgram(program_energyObject);        //使用指定的着色器程序 program 进行渲染

    //Uniform: Proj-Matrix (View ohne Perspektion -> View mit Perspektion)
    GLint projLoc = glGetUniformLocation(program_energyObject, "proj");
    GLfloat perspectiveMatrix[16];
    perspective(perspectiveMatrix, 60.0f/180.f*M_PI, 4.0f/3.0f, 0.1f, 100.0f);
    glUniformMatrix4fv(projLoc, 1, GL_TRUE, perspectiveMatrix);
    
    //Uniform: View-Matrix (World -> View)
    GLint viewLoc = glGetUniformLocation(program_energyObject, "view");
    GLfloat lookAtMatrix[16];
    GLfloat eye[3] = {0.0f,2.5f,5.0f};
    GLfloat center[3] = {0.0f,0.0f,0.0f};
    GLfloat up[3] = {0.0f,1.0f,0.0f};
    lookAt(lookAtMatrix, eye, center, up);
    glUniformMatrix4fv(viewLoc, 1, GL_TRUE, lookAtMatrix);   

    //Uniform: World-Matrix (Lokale -> World)
    GLint worldLoc = glGetUniformLocation(program_energyObject, "world");
    GLfloat iMatrix[16];
    GLfloat tMatrix[16];
    GLfloat rMatrix[16];
    GLfloat arMatrix[16];
    identity(iMatrix);
    GLfloat verschieben[3] = {-4.0f, 0.0f, -3.0f};
    translate(tMatrix, iMatrix, verschieben);   

    //Animation: energieObject

    //(1)初始化
    GLfloat translateToOrigin[16];   // 平移到原点的矩阵
    GLfloat translateBack[16];       // 平移回初始位置的矩阵
    GLfloat rotationMatrixEnergieObject[16];   // 旋转矩阵
    identity(translateToOrigin);
    identity(translateBack);
    identity(rotationMatrixEnergieObject);

    //(2)获得该物体中点
    GLfloat objectCenter[3];
    objectCenterRechnen(objVbo_energyObject, &objVboLen_energyObject, objectCenter); //计算某个物体的中心坐标
    printf("\nEnergyObject centerPunkt: %f, %f, %f\n\n", objectCenter[0], objectCenter[1], objectCenter[2]);

    //(3)获得该物体的平移, 旋转矩阵
    GLfloat verschiebZuOrigin[3] = {-objectCenter[0], -objectCenter[1], -objectCenter[2]};
    GLfloat verschiebZurueck[3] = {objectCenter[0], objectCenter[1], objectCenter[2]};
    translate(translateToOrigin, iMatrix, verschiebZuOrigin); //初始位置挪到原点
    translate(translateBack, iMatrix, verschiebZurueck);      //原点挪到初始位置
    rotatey(rotationMatrixEnergieObject, iMatrix, rotationAngleEnergieObject);    //旋转
    rotationAngleEnergieObject += 0.02f;

    //(4)获得该物体总的 TransformationMatrix (平移+旋转)
    GLfloat energieObjectTransformationMatrix[16];
    identity(energieObjectTransformationMatrix);
    matrix_multiply(energieObjectTransformationMatrix, translateToOrigin, energieObjectTransformationMatrix);            // 平移到原点
    matrix_multiply(energieObjectTransformationMatrix, rotationMatrixEnergieObject, energieObjectTransformationMatrix);  // 旋转
    matrix_multiply(energieObjectTransformationMatrix, translateBack, energieObjectTransformationMatrix);                // 平移回去

    matrix_multiply(energieObjectTransformationMatrix, tMatrix, energieObjectTransformationMatrix);
    glUniformMatrix4fv(worldLoc, 1, GL_TRUE, energieObjectTransformationMatrix);

    //Beleuchtung UniformPara Konfikurieren
    uniformParaFuerBeleuchtung(&program_energyObject);

    //NormalenMatrix
    // • N = view * world = matrix_multiply(out,lookAtMatrix,arMatrix);
    // • N(4x4) -> M(3x3) -> inverse -> transponieren 转置矩阵 
    normalenMatrix(&program_energyObject, lookAtMatrix, energieObjectTransformationMatrix);
    
    glBindVertexArray(vao_energyObject);       //绑定了顶点数组对象 vao，指示OpenGL使用这个顶点数组对象来绘制图
    GLint status;
    glValidateProgram(program_energyObject);   //验证着色器程序的有效性
                                  //告诉OpenGL去检查当前的着色器程序是否有效，是否可以被成功链接和使用
    glGetProgramiv(program_energyObject, GL_VALIDATE_STATUS, &status); //检查：验证状态
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

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height); //x 和 y 是视口在窗口中的左下角位置的坐标
                                     //width 和 height 分别是视口的宽度和高度
                                     //以此来确保 OpenGL 渲染区域与新的窗口大小保持一致
}

int main(void) {
    printf("Hallo!\n");

    glfwInit(); //初始化 GLFW 库, 它必须在使用 GLFW 的任何其他函数之前调用。
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //设置OpenGL上下文的主版本号(OpenGL 3.x 版本)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //设置OpenGL上下文的次版本号(OpenGL x.3 版本)
                                                   //主次和起来就是 OpenGL 3.3 版本
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //指定 OpenGL-Profile 窗口配置文件：Core Profile

    GLFWwindow *window = glfwCreateWindow(800, 600, "Computergrafik 1", NULL, NULL); //创建窗口
    if (!window) { //若窗口指针为NULL，即窗口创建不成功
        printf("Failed to create Window\n");
        glfwTerminate(); //终止 GLFW 库的运行 (通常在程序结束时调用):
                         // - 释放 GLFW 所占用的资源
                         // - 清理 GLFW 库的状态
        return -1;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); //窗口大小改变时更新视口大小
                                                                       //确保渲染的内容能够正确地适应新的窗口大小
    glfwMakeContextCurrent(window); //将之前创建的窗口window设置为当前上下文
    glewInit(); //初始化GLEW库, 用于管理OpenGL的扩展
                //使得开发者能够更方便地在不同平台上使用OpenGL的最新特性

    init1();
    init2();
    init3();

    rotationAngleEnergieObject = 0.0f;
    rotationAngleFan = 0.0f;
    rotationAngleGanzIsland = 0.0f;
    while (!glfwWindowShouldClose(window)) { //若窗口没有接到关闭的指令
        draw1(); //执行绘制操作
        draw2();
        draw3();
        glfwSwapBuffers(window); //用于交换前后缓冲区，将已经渲染好的图像显示在屏幕上
        glfwPollEvents(); //处理窗口事件，比如鼠标移动、键盘按键等，保证窗口能够响应用户的输入操作
    }

    glfwTerminate();
    return 0;
}