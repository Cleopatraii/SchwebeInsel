#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include "main_funk.h"

//Hilfefunktion: Matrix verarbeiten

//(1) Matrix normieren
void normierung(GLfloat *out, const GLfloat* in){

    // Null prüfen
    assert(in != NULL); 
    //Verhinderung des illegalen Speicherzugriffs
    assert(out != NULL); 

    GLfloat vactorX = in[0];
    GLfloat vactorY = in[1];
    GLfloat vactorZ = in[2];
    GLfloat gross = sqrtf(vactorX * vactorX + vactorY * vactorY + vactorZ * vactorZ);

    assert(gross != 0.0f); // Sicherstellen, dass "gross" nicht Null ist

    out[0] = vactorX / gross;
    out[1] = vactorY / gross;
    out[2] = vactorZ / gross;
}

//(2) Matrix kopieren
void kopieren(GLfloat* neu, const GLfloat* alt) {

    // Null prüfen
    assert(alt != NULL); 
    //Verhinderung des illegalen Speicherzugriffs
    assert(neu != NULL); 

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            neu[4 * i + j] = alt[4 * i + j];
        }
    }
}

//(3) Matrix multiplikation
void matrix_multiply(GLfloat* result, const GLfloat* a, const GLfloat* b) {

    // Null prüfen
    assert(a != NULL);      
    assert(b != NULL);     
    //Verhinderung des illegalen Speicherzugriffs
    assert(result != NULL);  

    GLfloat m1[16];
    GLfloat m2[16];
    kopieren(m1, a);
    kopieren(m2, b);

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result[i * 4 + j] = 0.0f; // Initialisieren
            for (int k = 0; k < 4; k++) {
                result[i * 4 + j] += m1[i * 4 + k] * m2[k * 4 + j];
            }
        }
    }
}

//Funktion: Zu World-Koordinatensystem gehen
//Szene transformieren, um Kamera im Ursprung zu bleiben
void lookAt(GLfloat* out, GLfloat* eye, GLfloat* center, GLfloat* up) {

    // Null prüfen
    assert(eye != NULL);    
    assert(center != NULL); 
    assert(up != NULL);
    //Verhinderung des illegalen Speicherzugriffs
    assert(out != NULL); 

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
//  Projektion: (x,y,z)->(1,1,1)
void perspective(GLfloat* out, GLfloat fovy, GLfloat aspect, GLfloat near, GLfloat far) {

    // Daten prüfen
    assert(fovy != 0.0f);
    assert(aspect != 0.0f);
    assert(near != 0.0f);
    assert(far != 0.0f);
    //Verhinderung des illegalen Speicherzugriffs
    assert(out != NULL); 

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
void printMatrix4x4(GLfloat* in) {
    // Null prüfen
    assert(in != NULL);  

    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            printf("%f ",in[i*4+j]);
        }
        printf("\n");
    }
}
void printMatrix3x3(GLfloat* in) {
    // Null prüfen
    assert(in != NULL);  

    for (int i=0; i<3; i++) {
        for (int j=0; j<3; j++) {
            printf("%f, ",in[i*3+j]);
        }
        printf("\n");
    }
}

//Funktion: Zu View-Koordinatensystem gehen
//(0)Identify
void identity(GLfloat* out) {

    //Verhinderung des illegalen Speicherzugriffs
    assert(out != NULL); 

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
    
    // Null prüfen
    assert(in != NULL); 
    assert(v != NULL);  

    //Verhinderung des illegalen Speicherzugriffs
    assert(out != NULL); 

    // Der Vektor v ist ein 3-Komponenten-Vektor
    GLfloat x = v[0]; // ax
    GLfloat y = v[1]; // ay
    GLfloat z = v[2]; // az
 
    //    v          T(v)            in              out
    //    ax     | 1 0 0 ax |    | x 0 0 0 |     x+ax  0 0 0
    //    by     | 0 1 0 ay |    | y 0 0 0 |     y+ay  0 0 0
    //    az     | 0 0 1 az |    | z 0 0 0 |     z+az  0 0 0
    //           | 0 0 0 1  |    | 1 0 0 0 |     1     0 0 0
    //
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

    // Null prüfen
    assert(in != NULL);
    assert(v != NULL); 

    //Verhinderung des illegalen Speicherzugriffs
    assert(out != NULL); 

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

    // Null prüfen
    assert(in != NULL);
    //Verhinderung des illegalen Speicherzugriffs
    assert(out != NULL); 

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

    // Null prüfen
    assert(in != NULL);
    //Verhinderung des illegalen Speicherzugriffs
    assert(out != NULL); 

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

    // Null prüfen
    assert(in != NULL);
    //Verhinderung des illegalen Speicherzugriffs
    assert(out != NULL); 

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

// (1) Inverse-Matrix3x3 Berechnen
void inverseMatrix3x3(float* result, float* mat) {

    // Null prüfen
    assert(mat != NULL);
    //Verhinderung des illegalen Speicherzugriffs
    assert(result != NULL); 

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
// (2) Matrix4x4 -> Matrix3x3
void normalMatrixFromMatrix4x4(float* normalMat, float* mat) {

    // Null prüfen
    assert(mat != NULL);
    //Verhinderung des illegalen Speicherzugriffs
    assert(normalMat != NULL);

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            normalMat[i*3+j] = mat[i*4+j];
        }
    }
}
// (3) Transponieren
void transponieren(float* transpMatrix, float* mat) {

    // Null prüfen
    assert(mat != NULL);
    //Verhinderung des illegalen Speicherzugriffs
    assert(transpMatrix != NULL);

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            transpMatrix[j*3+i] = mat[i*3+j];
        }
    }
}

