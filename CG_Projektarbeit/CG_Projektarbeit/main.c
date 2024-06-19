#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <math.h>
#include <stdlib.h>
#include "vertexShader.h"
#include "fragmentShader.h"


GLuint program; 
GLuint vaoFan;
GLuint vaoRest;
//GLuint colorLoc; //Uniform变量color的位置
GLfloat randonColor; //Animation der Farbe
GLuint count = 3;
float rotationAngle;

void normierung(GLfloat *out, GLfloat* in){
    GLfloat vactorX = in[0];
    GLfloat vactorY = in[1];
    GLfloat vactorZ = in[2];
    GLfloat gross = sqrtf(vactorX*vactorX + vactorY*vactorY + vactorZ*vactorZ);
    out[0] = vactorX / gross;
    out[1] = vactorY / gross;
    out[2] = vactorZ / gross;
}

void kopieren (GLfloat* neu, GLfloat* alt) {
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            neu[4*i+j]=alt[4*i+j];
        }
    }
}

//Ü5.1: Szene transformieren, um Kamera im Ursprung zu bleiben
//      就是有 eye, center, up的偏差，要利用out把这些偏差纠正过来
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

//Ü5.1: "view frustrum": Abbildung "Pyramidenstumpf" auf "Würfel"
//      投影: (x,y,z)->(1,1,1)
void perspective(GLfloat* out, GLfloat fovy, GLfloat aspect, GLfloat near, GLfloat far) {
    //1. wir haben schon 
    //   n, f
    //2. t = n•tan(fovy/2)
    GLfloat t = near * tanf(fovy/2.0f);
//    printf("t:%f \n", t);
    //3. r = aspect * t
    GLfloat r = aspect * t;
//    printf("r:%f \n", r);

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

//restult = a * b
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

//Ue4.1.1
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

//Ue4.1.2: out = T(v) * in
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

//Ue4.1.3: out = R(v) * in
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
//Ue6

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

//Obj-Datei lesen

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

Flaeche8f* vboFan;
long vboLenFan=0;

Flaeche8f* vboRest;
long vboLenRest=0;

long totalVboLen;    
//Ue7.1
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

//Ue7.2

// 1.Inverse-Matrix3x3 Berechnen
void inverseMatrix3x3(float* result, float* mat) {
    float det = mat[0] * (mat[4]*mat[8] - mat[5]*mat[7]) -
                mat[1] * (mat[3]*mat[8] - mat[5]*mat[6]) +
                mat[2] * (mat[3]*mat[7] - mat[4]*mat[6]);
    if (det == 0.0) {
        printf ("Warnung: det == 0 !!!!!\n");
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

// 2.Matrix4x4 -> Matrix3x3
void normalMatrixFromMatrix4x4(float* normalMat, float* mat) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            normalMat[i*3+j] = mat[i*4+j];
        }
    }
}

// 3.Transponieren
void transponieren(float* transpMatrix, float* mat) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            transpMatrix[j*3+i] = mat[i*3+j];
        }
    }
}

void init(void) {
    const char *vertexText = vertexShader_glsl;
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER); //(1)定义顶点着色器"vertexShader"：create vertex shader
    glShaderSource(vertexShader, 1, &vertexText, &vertexShader_glsl_len); //将源代码绑定到着色器对象vertexShader上
                                                        //这里只有一个存储了源代码的源代码字符串vertexText，该源代码存于地址“&vertexText”中
    glCompileShader(vertexShader); //(2)编译顶点着色器"vertexShader": compile vertex shader
    GLint status;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status); //查询顶点着色器对象vertexShader的编译状态(GL_COMPILE_STATUS)，并将结果存储在status变量
    if (!status) { //若顶点着色器编译失败
        printf("Error comiling vertex shader:");
        GLchar infoLog[1024]; //定义一个数组infoLog用于存储编译错误信息
        glGetShaderInfoLog(vertexShader, 1024, NULL, infoLog); //获取顶点着色器的编译日志信息，并将其存储在infoLog数组中
                                                               //1024 表示 infoLog 数组的大小，NULL 表示不返回实际写入的字符数
        printf("%s",infoLog);
    }

    const char *fragmentText = fragmentShader_glsl;
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); //着色器：对象
    glShaderSource(fragmentShader, 1, &fragmentText, &fragmentShader_glsl_len); //着色器：对象-源代码
    glCompileShader(fragmentShader); //着色器：编译
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status); //检查：着色器编译状态
    if (!status) {
        printf("Error comiling fragment shader:");
        GLchar infoLog[1024];
        glGetShaderInfoLog(fragmentShader, 1024, NULL, infoLog);
        printf("%s",infoLog);
    }

    // create and link shader program
    // 3.着色器程序
    program = glCreateProgram(); //创建着色器程序对象
    glAttachShader(program, vertexShader);  //附加：着色器对象-着色器程序
    glAttachShader(program, fragmentShader);
    glLinkProgram(program); //将着色器程序对象(program)链接到当前 OpenGL 环境中
                            //OpenGL 将会检查各个着色器是否能够一起正确地链接成一个完整的着色器程序
                            //如果链接成功，着色器程序将会被创建，并且可以在渲染时使用
    glGetProgramiv(program, GL_LINK_STATUS, &status); //检查：着色器程序的链接状态
    if (!status) {
        printf("Error linking program:");
        GLchar infoLog[1024];
        glGetProgramInfoLog(program, 1024, NULL, infoLog);
        printf("%s",infoLog);
        exit(-1);
    }
    
    // Ue6
    // VBO
    readOBJ("fan.obj", &vboFan, &vboLenFan);
    readOBJ("restBestandteilen.obj", &vboRest, &vboLenRest);

    GLfloat fanVertices[vboLenFan];

    //GLfloat combinedVertices[totalVboLen];
    for (long i=0; i<vboLenFan/8; i++) {
        fanVertices[i*8] = vboFan[i].v1;
        fanVertices[i*8+1] = vboFan[i].v2;
        fanVertices[i*8+2] = vboFan[i].v3;
        fanVertices[i*8+3] = vboFan[i].t1;
        fanVertices[i*8+4] = vboFan[i].t2;
        fanVertices[i*8+5] = vboFan[i].n1;
        fanVertices[i*8+6] = vboFan[i].n2;
        fanVertices[i*8+7] = vboFan[i].n3;
    }

GLfloat restVertices[vboLenRest];
for (long j = 0; j < vboLenRest / 8; j++) {
    restVertices[j*8 + 0] = vboRest[j].v1;
    restVertices[j*8 + 1] = vboRest[j].v2;
    restVertices[j*8 + 2] = vboRest[j].v3;
    restVertices[j*8 + 3] = vboRest[j].t1;
    restVertices[j*8 + 4] = vboRest[j].t2;
    restVertices[j*8 + 5] = vboRest[j].n1;
    restVertices[j*8 + 6] = vboRest[j].n2;
    restVertices[j*8 + 7] = vboRest[j].n3;
}
   //VBO for Fan
    GLuint fanVBO; 
    glGenBuffers(1, &fanVBO); 
    glBindBuffer(GL_ARRAY_BUFFER, fanVBO); 
                                                              
    glBufferData(GL_ARRAY_BUFFER,  sizeof(fanVertices) , fanVertices, GL_STATIC_DRAW); 
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    //VBO for restBestandteilen
    GLuint restVBO; 
    glGenBuffers(1, &restVBO);
    glBindBuffer(GL_ARRAY_BUFFER, restVBO); 
                                                              
    glBufferData(GL_ARRAY_BUFFER,  sizeof(restVertices) , restVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 


    // VAO for fan
    glGenVertexArrays(1, &vaoFan);     
    glBindVertexArray(vaoFan);
                            
    glBindBuffer(GL_ARRAY_BUFFER, fanVBO); 
    glVertexAttribPointer( 
        0,                         //指定了第一个顶点属性(索引为0), 先前顶点着色器源代码中的第一个输入参数：2D顶点位置
        3,                         //包含两个分量vec3(x,y,z)，即3D
        GL_FLOAT,                  //其中 x,y 数据类型为 GL_FLOAT
        GL_FALSE,                  
        8 * sizeof(GLfloat),       //跨度为 6 个 float (x,y,z,r,g,b)
        0                          //偏移量为 0
                                   //因为(x,y,r,g,b)中的颜色部分(x,y)是从第一个float开始的
    );
    glEnableVertexAttribArray(0);  

    glVertexAttribPointer( 
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

    glVertexAttribPointer(
    2,                         // 纹理坐标属性索引为2
    2,                         // 纹理坐标包含两个分量(u, v)
    GL_FLOAT,                  
    GL_FALSE,
    8 * sizeof(GLfloat),       // 每个顶点数据的总跨度
    (void*)(3 * sizeof(GLfloat)) // 从每个顶点数据的第四个GLfloat开始
    );
    
    glEnableVertexAttribArray(2); // 启用纹理坐标顶点属性数组
    
    glBindBuffer(GL_ARRAY_BUFFER, 0); //解绑了当前绑定的顶点缓冲对象
    glBindVertexArray(0);


    //VAO for reastBestandteilen
    glGenVertexArrays(1, &vaoRest); 
                                
    glBindVertexArray(vaoRest); 

    glBindBuffer(GL_ARRAY_BUFFER, restVBO); //顶点缓冲对象triangleVertexBufferObject和该Buffer相连接，就包含了顶点数据 ？？？？Warum noch einmal? Aktualisieren?
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

    glVertexAttribPointer( //指定了顶点属性数组的位置和格式(颜色)
        1,                         //指定了第二个顶点属性(索引为1), 先前顶点着色器源代码中的第二个输入参数：3D顶点RGB颜色
        3,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(GLfloat),
        (GLvoid*)(5 * sizeof(GLfloat)) //偏移量为 3 个 GLfloat，并类型转换(指向空类型的指针)，为了保持参数的一致性 ？？？？？？？？？？？？？
                                       

    );
    glEnableVertexAttribArray(1);  //启用了第二个顶点属性数组
    
    glVertexAttribPointer(
    2,                         // 纹理坐标属性索引为2
    2,                         // 纹理坐标包含两个分量(u, v)
    GL_FLOAT,                  
    GL_FALSE,
    8 * sizeof(GLfloat),       // 每个顶点数据的总跨度
    (void*)(3 * sizeof(GLfloat)) // 从每个顶点数据的第四个GLfloat开始
    );
    
    glEnableVertexAttribArray(2); // 启用纹理坐标顶点属性数组

    glBindBuffer(GL_ARRAY_BUFFER, 0); //解绑了当前绑定的顶点缓冲对象
    glBindVertexArray(0);

    glClearColor(191.0f/255.0f,217.0f/255.0f,204.0f/255.0f,1.0); //设置窗口背景颜色
    //glClearColor(0.8f,0.8f,0.8f,1.0); //设置窗口背景颜色
    //glViewport(0, 0, 800, 600); //定义视口大小, 即显示的图形与窗口的大小关系
}
void draw(void) {
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); //清除了颜色缓冲区，将其填充为背景色
    glUseProgram(program);        //使用指定的着色器程序 program 进行渲染

//首先通过 lookAt 和 perspective 计算设置了全局视图和投影矩阵，这些矩阵用于所有的物体，它们将场景中的对象从世界坐标转换到屏幕坐标
    //GLfloat scaleVector[] = { 1.0f, 2.0f, 1.0f };
    GLint projLoc = glGetUniformLocation(program, "proj");
    GLint viewLoc = glGetUniformLocation(program, "view");
    GLint worldLoc = glGetUniformLocation(program, "world");

    //Depth Test einschalten
    glEnable(GL_DEPTH_TEST);

    GLfloat lookAtMatrix[16];
    GLfloat eye[3] = {-4.0f,0.0f,0.0f};
    GLfloat center[3] = {0.0f,0.0f,0.0f};
    GLfloat up[3] = {0.0f,1.0f,0.0f};
    lookAt(lookAtMatrix, eye, center, up);
    
    GLfloat perspectiveMatrix[16];
    perspective(perspectiveMatrix, 60.0f/180.f*M_PI, 4.0f/3.0f, 0.1f, 100.0f);
    
     glUniformMatrix4fv(projLoc, 1, GL_TRUE, perspectiveMatrix);
     glUniformMatrix4fv(viewLoc, 1, GL_TRUE, lookAtMatrix);    

     GLfloat iMatrix[16];
    // GLfloat tMatrix[16];
    // GLfloat rMatrix[16];
    // GLfloat arMatrix[16];
     identity(iMatrix);
    // rotatey(rMatrix, iMatrix, -90.0f/180.f*M_PI);
   
    //接下来，针对每个具体的物体的模型变换（由 worldLoc 矩阵表示），这涉及到物体自身的旋转、缩放和平移，即局部变换
    //Animation: rotation

    GLfloat translateToOrigin[16];   // 平移到原点的矩阵
    GLfloat translateBack[16];       // 平移回初始位置的矩阵
    GLfloat rotationMatrix[16];      // 旋转矩阵

    identity(translateToOrigin);
    identity(translateBack);
    identity(rotationMatrix);

    translate(translateToOrigin, iMatrix, (GLfloat[3]){0.007221, -0.403461, 0.079235});
    rotatex(rotationMatrix, iMatrix, rotationAngle); // 
    translate(translateBack, iMatrix, (GLfloat[3]){0.007221, 0.403461, -0.079235});
    
    GLfloat fanMatrix[16];
    identity(fanMatrix);

    matrix_multiply(fanMatrix, translateToOrigin, fanMatrix);  // 平移到原点
    matrix_multiply(fanMatrix, rotationMatrix, fanMatrix);  // 旋转
    matrix_multiply(fanMatrix, translateBack, fanMatrix);  // 平移回去

    glUniformMatrix4fv(worldLoc, 1, GL_TRUE, fanMatrix);

    //rotatey(arMatrix, rMatrix, -rotationAngle);
    //glUniformMatrix4fv(worldLoc, 1, GL_TRUE, arMatrix);
    rotationAngle += 0.001f;

    //Ue7.1
    GLint matAmbLoc = glGetUniformLocation(program, "mat.ambient");
    GLint matDifLoc = glGetUniformLocation(program, "mat.diffuse");
    GLint matSpeLoc = glGetUniformLocation(program, "mat.specular");
    GLint matShLoc = glGetUniformLocation(program, "mat.shininess");
    GLint quePosLoc = glGetUniformLocation(program, "light.position");
    GLint queColLoc = glGetUniformLocation(program, "light.color");
    GLint queAmbLoc = glGetUniformLocation(program, "light.ambient");

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
    materialLicht.shininess = 20.0f;
    QuelleLightParams quelleLicht;
    quelleLicht.ambient.x = 0.5f;
    quelleLicht.ambient.y = 0.5f;
    quelleLicht.ambient.z = 0.5f;
    quelleLicht.color.x = 1.0f;
    quelleLicht.color.y = 0.0f;
    quelleLicht.color.z = 0.0f;
    quelleLicht.position.x = 20.0f;
    quelleLicht.position.y = 20.0f;
    quelleLicht.position.z = 0.0f;

    glUniform3f(matAmbLoc, materialLicht.ambient.x, materialLicht.ambient.y, materialLicht.ambient.z);
    glUniform3f(matDifLoc, materialLicht.diffuse.x, materialLicht.diffuse.y, materialLicht.diffuse.z); 
    glUniform3f(matSpeLoc, materialLicht.specular.x, materialLicht.specular.y, materialLicht.specular.z);  
    glUniform1f(matShLoc, materialLicht.shininess);

    glUniform3f(queAmbLoc, quelleLicht.ambient.x, quelleLicht.ambient.y, quelleLicht.ambient.z);
    glUniform3f(queColLoc, quelleLicht.color.x, quelleLicht.color.y, quelleLicht.color.z); 
    glUniform3f(quePosLoc, quelleLicht.position.x, quelleLicht.position.y, quelleLicht.position.z);  

    //Ue7.2 Für InverseMatrix
    // • N = view * world = matrix_multiply(out,lookAtMatrix,arMatrix);
    // • N(4x4) -> M(3x3) -> inverse -> transponieren 转置矩阵 
    GLint normalenMatrixLoc = glGetUniformLocation(program, "normalenMatrix");

    //(1) view * world
    GLfloat viewWordMatrix[16];
    matrix_multiply(viewWordMatrix, lookAtMatrix, fanMatrix);
    //printf("viewWordMatrix: \n");
    //printMatrix4x4(viewWordMatrix);

    //(2) 4->3
    GLfloat Mat4x4zuMat3x3[9];
    normalMatrixFromMatrix4x4(Mat4x4zuMat3x3, viewWordMatrix);
    //printf("Mat4x4zuMat3x3: \n");
    //printMatrix3x3(Mat4x4zuMat3x3);

    //(3) Inverse
    GLfloat inverseMat3x3[9];
    inverseMatrix3x3(inverseMat3x3, Mat4x4zuMat3x3);
    //printf("inverseMat3x3: \n");
    //printMatrix3x3(inverseMat3x3);

    //(4) Transponieren
    GLfloat transpMatrix[9];
    transponieren(transpMatrix, inverseMat3x3);
    //printf("transpMatrix: \n");
    //printMatrix3x3(transpMatrix);
    //printf("\n");
    glUniformMatrix3fv(normalenMatrixLoc, 1, GL_TRUE, transpMatrix);

    GLfloat restMatrix[16];
    identity(restMatrix);  
    

    // 绑定rest VAO
    glBindVertexArray(vaoRest);  
    glUniformMatrix4fv(worldLoc, 1, GL_TRUE, restMatrix);
    glDrawArrays(GL_TRIANGLES, 0, vboLenRest / 8);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 


    glBindVertexArray(vaoFan);     
    glUniformMatrix4fv(worldLoc, 1, GL_TRUE, fanMatrix); 
    GLint status;
    glValidateProgram(program);   //验证着色器程序的有效性
                                  
    glGetProgramiv(program, GL_VALIDATE_STATUS, &status); 
    if (!status) {
        printf("Error validating program:");
        GLchar infoLog[1024];
        glGetProgramInfoLog(program, 1024, NULL, infoLog);
        printf("%s",infoLog);
    }
    
    glDrawArrays(GL_TRIANGLES, 0, vboLenFan /8); 
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height); 
}

int main(void) {
    printf("Hallo!\n");

    glfwInit(); 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); 
                                                   
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 

    GLFWwindow *window = glfwCreateWindow(800, 600, "Computergrafik 1", NULL, NULL); //创建窗口
    if (!window) { 
        printf("Failed to create Window\n");
        glfwTerminate(); 
                        
        return -1;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); 
                                                                      
    glfwMakeContextCurrent(window); 
    glewInit(); 

    init();

    rotationAngle = 0.0f;
    while (!glfwWindowShouldClose(window)) {
        draw(); 
        glfwSwapBuffers(window); 
        glfwPollEvents(); 
    }

    glfwTerminate();
    return 0;
}
