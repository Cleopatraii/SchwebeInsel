#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <assert.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>

#include "main_funk.h"

#define M_PI 3.1415926

/*
extern void normierung(GLfloat *out, const GLfloat* in);
extern void kopieren (GLfloat* neu,  const GLfloat* alt);
extern void  matrix_multiply(GLfloat* result,  const GLfloat* a,  const GLfloat* b);
extern void crossMultiMat(GLfloat* out,GLfloat* a, GLfloat* b);
extern void lookAt(GLfloat* out, GLfloat* eye, GLfloat* center, GLfloat* up) ;
extern void perspective(GLfloat* out, GLfloat fovy, GLfloat aspect, GLfloat near, GLfloat far);
extern void printMatrix4x4(GLfloat* out);
extern void printMatrix3x3(GLfloat* out);
extern void identity(GLfloat* out);
extern void translate(GLfloat* out, GLfloat* in, GLfloat* v);
extern void scale(GLfloat* out, GLfloat* in, GLfloat* v);
extern void rotatex(GLfloat* out, GLfloat* in, GLfloat angle);
extern void rotatey(GLfloat* out, GLfloat* in, GLfloat angle);
extern void rotatez(GLfloat* out, GLfloat* in, GLfloat angle);
extern void inverseMatrix3x3(float* result, float* mat);
extern void normalMatrixFromMatrix4x4(float* normalMat, float* mat);
extern void transponieren(float* transpMatrix, float* mat);
*/



void test_Normierung(GLfloat *out, const GLfloat* in){
    printf("normierung-funktion: ");
    normierung(out, in);
    float length = sqrt(out[0]*out[0]+ out[1]*out[1]+out[2]*out[2]);
    if (length == 1.0f){
        printf("OK\n");
    }else{
        printf("nicht OK\n");
    }
} 


void test_matrix_multiply(const GLfloat* a, const GLfloat* b){
    printf("matrix_multiply-funktion:");
    GLfloat newresult[16];
    matrix_multiply(newresult, a, b);
    matrix_multi_logik(newresult, a, b);
}

void matrix_multi_logik(GLfloat* result,GLfloat* a, GLfloat* b){
    //使用GSL进行初始化
    gsl_matrix_float* gslMat1 = gsl_matrix_float_alloc(4, 4);
    gsl_matrix_float* gslMat2 = gsl_matrix_float_alloc(4, 4);
    gsl_matrix_float* gslResult = gsl_matrix_float_alloc(4, 4);

    //写入内容
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            gsl_matrix_float_set(gslMat1, i, j, a[i * 4 + j]);
                                //指针，索引，索引，给出矩阵某行某列的元素值
            gsl_matrix_float_set(gslMat2, i, j, b[i * 4 + j]);
        }
    }
    //使用GSL库函数，进行矩阵相乘
    //C = alpha * A * B + beta * C
    gsl_blas_sgemm(
        CblasNoTrans,   //第一个矩阵是否转置
        CblasNoTrans,   //第二个矩阵是否转置
        1.0,            //alpha
        gslMat1,        //第一个矩阵
        gslMat2,        //第二个矩阵
        0.0,            //beta
        gslResult);     //存储结果
    //==> C = A * B


    for(int i = 0; i<16; i++){
        if(gsl_matrix_float_get(gslResult, i / 4, i % 4) != result[i]){
            printf("nicht OK\n");
            return;
        }
    }
    printf("OK\n");

    gsl_matrix_float_free(gslMat1);
    gsl_matrix_float_free(gslMat2);
    gsl_matrix_float_free(gslResult);
}

void test_lookAt(){
    printf("lookAt-funktion: ");
    float eye[] = {1.0f, 2.0f, 3.0f};
    float center[] = {0.0f, 0.0f, 0.0f};
    float up[] = {0.0f, 1.0f, 0.0f};

    float expected[] = {
        0.801784, 0.000000, -0.267261, 0.000000, 
        -0.142857, 0.714286, -0.428571, 0.000000, 
        0.267261, 0.534522, 0.801784, -3.741657, 
        0.000000, 0.000000, 0.000000, 1.000000
    };

    GLfloat out[16];
    lookAt(out, eye, center, up);

    //printMatrix4x4(out);
    //printMatrix4x4(expected);
    for(int i = 0; i<16; i++){
        int resultScaled = (int)(out[i] * 100000);
        int expectedScaled = (int)(expected[i] * 100000);
        if(resultScaled != expectedScaled){
            printf("--%d--", i);
            printf("nicht OK\n");
            return;
        }
    }
    
    printf("OK\n");
}

void test_perspective(){
    printf("perspective-funktion: ");
    GLfloat fovy = M_PI / 4.0f; // 45 degrees
    GLfloat aspect = 1.0f;
    GLfloat near = 1.0f;
    GLfloat far = 10.0f;

    GLfloat t = near * tanf(fovy / 2.0f);
    GLfloat r = aspect * t;
    GLfloat out[16];
    perspective(out, fovy, aspect, near, far);

    GLfloat expected[] = {
        near/r,   0.0f,            0.0f,                     0.0f,
        0.0f,    near/t,           0.0f,                     0.0f,
        0.0f,     0.0f,   (far+near)/(near-far),  (2*far*near)/(near-far),
        0.0f,     0.0f,           -1.0f,                     0.0f
    };

    for(int i = 0; i<16; i++){
        if(expected[i] != out[i]){
            printf("nicht OK\n");
            return;
        }
    }
    printf("OK\n");

}

void test_identity(GLfloat* out){
    printf("identitiy-funktion test:");
    identity(out);
    for(int i = 0; i < 4; i++){
        for(int j = 0; j<4; j++){
            if(out[i*4+j]==0){
                if(i==j){
                    printf("nicht OK\n");
                    return;
                }
            }else if(out[i*4+j] == 1){
                if(i!=j){
                    printf("nicht OK\n");
                    return;
                }
            }else{
                printf("nicht OK\n");
                return;
            }
        }
    }
    printf("OK\n");
    
}

void test_translate(GLfloat* in, GLfloat* v){
    printf("translate-funktion: ");
    GLfloat out[16];
    GLfloat expected[] = {
        in[0], 0, 0, v[0]*in[15],
        0, in[5], 0, v[1]*in[15],
        0, 0, in[10], v[2]*in[15],
        0, 0, 0, in[15]
    };
    GLfloat T[16] = {
        1, 0, 0, v[0],
        0, 1, 0, v[1],
        0, 0, 1, v[2],
        0, 0, 0, 1
    };
    
    translate(out, in, v);
    matrix_multi_logik(out, T, in);
}

void test_scale(GLfloat* in, GLfloat* v){
    printf("scale-funktion: ");
    GLfloat out[16];
    GLfloat S[] = {
        v[0], 0, 0, 0, 
        0, v[1], 0, 0,
        0, 0, v[2], 0,
        0, 0, 0, 1
    };
    scale(out, in, v);
    matrix_multi_logik(out,S, in);
}
void test_rotatex(GLfloat* in, GLfloat angle){
    printf("rotatex-funktion: ");
    GLfloat out[16];
    GLfloat R[] = {
        1.0f,0.0f,0.0f,0.0f,
        0.0f,cosf(angle),-sinf(angle),0.0f,
        0.0f,sinf(angle),cosf(angle),0.0f,
        0.0f,0.0f,0.0f,1.0f
    };
    rotatex(out, in, angle);
    matrix_multi_logik(out, R, in);
}
void test_rotatey(GLfloat* in, GLfloat angle){
    printf("rotatey-funktion: ");
    GLfloat out[16];
    GLfloat R[] = {
        cosf(angle),0.0f,sinf(angle),0.0f,
        0.0f,1.0f,0.0f,0.0f,
        -sinf(angle),0.0f,cosf(angle),0.0f,
        0.0f,0.0f,0.0f,1.0f
    };
    rotatey(out, in, angle);
    matrix_multi_logik(out, R, in);
}
void test_rotatez(GLfloat* in, GLfloat angle){
    printf("rotatez-funktion: ");
    GLfloat out[16];
    GLfloat R[] = {
        cosf(angle),-sinf(angle),0.0f,0.0f,
        sinf(angle),cosf(angle),0.0f,0.0f,
        0.0f,0.0f,1.0f,0.0f,
        0.0f,0.0f,0.0f,1.0f
    };
    rotatez(out, in, angle);
    matrix_multi_logik(out, R, in);
}

//逆矩阵与原矩阵相称为单位矩阵
void test_inverseMatrix3x3(float* result, float* mat){
    printf("inverseMatrix3x3-funktion: ");
    inverseMatrix3x3(result, mat);
    GLfloat identity[9];
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            identity[i*3+j] = 0.0f;
            for (int k = 0; k < 3; ++k) {
                identity[i * 3 + j] += mat[i * 3 + k] * result[k * 3 + j];
            }
        }
    }
    //printMatrix3x3(identity);
    for(int i = 0; i < 3; i++){
        for(int j = 0; j<3; j++){
            if(identity[i*3+j]==0){
                if(i==j){
                    printf("nicht OK\n");
                    return;
                }
            }else if(identity[i*3+j] == 1){
                if(i!=j){
                    printf("nicht OK\n");
                    return;
                }
            }else{
                printf("nicht OK\n");
                return;
            }
        }
    }
    printf("OK\n");
}

void test_normalMatrixFromMatrix4x4(float* normalMat, float* mat){
    printf("normalMatrixFromMatrix4x4-funktion: ");
    normalMatrixFromMatrix4x4(normalMat, mat);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if(normalMat[i * 3 + j] != mat[i * 4 + j]){
                printf("nicht OK\n");
                return;
            }
        }
    }
    printf("OK\n");
}

void test_transponieren(float* transpMatrix, float* mat){
    printf("transponieren-funktion: ");
    transponieren(transpMatrix, mat);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if(transpMatrix[j * 3 + i] != mat[i * 3 + j]){
                printf("nicht OK\n");
                return;
            }
        }
    }
    printf("OK\n");
}

int main(void){
    GLfloat in[] = {3.0f, 4.0f, 0.0f};
    GLfloat out[3] = {0.0f, 0.0f, 0.0f};
    GLfloat out1[3];
    test_Normierung(out1, in);
    

    GLfloat outMatrix1[16];
    test_identity(outMatrix1);


    GLfloat mat1[16] = {
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 6.0f, 7.0f, 8.0f,
        9.0f, 10.0f, 11.0f, 12.0f,
        13.0f, 14.0f, 15.0f, 16.0f
    };

    GLfloat mat2[16] = {
        16.0f, 15.0f, 14.0f, 13.0f,
        12.0f, 11.0f, 10.0f, 9.0f,
        8.0f, 7.0f, 6.0f, 5.0f,
        4.0f, 3.0f, 2.0f, 1.0f
    };
    GLfloat result[16] = {
        80.0f, 70.0f, 60.0f, 50.0f,
        240.0f, 214.0f, 188.0f, 162.0f,
        400.0f, 358.0f, 316.0f, 274.0f,
        560.0f, 502.0f, 444.0f, 386.0f
    };
    test_matrix_multiply(mat1, mat2);

    //LookAt
    test_lookAt();
    //perspective
    test_perspective();
    //
    GLfloat in_translate[16] = {
    2.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 3.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
    };
    GLfloat v[3] = {1.0f, 2.0f, 3.0f};
    test_translate(in_translate, v);
    //
    GLfloat in_scale[16] = {
    2.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 2.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 4.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
    };
    test_scale(in_translate, v);

    //
    GLfloat in_rotatex[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    GLfloat angle = M_PI / 4.0f;
    test_rotatex(in_rotatex, angle);
    test_rotatey(in_rotatex, angle);
    test_rotatez(in_rotatex, angle);

    float mat[9] = {
        1.0f, 2.0f, 3.0f,
        0.0f, 1.0f, 4.0f,
        5.0f, 6.0f, 0.0f
    };
    GLfloat outMatrix2[9];
    test_inverseMatrix3x3(outMatrix2,mat);


    GLfloat matrix4_4[16] = {
        1.0f, 2.0f, 3.0f, 4.0f,
        5.0f, 6.0f, 7.0f, 8.0f,
        9.0f, 10.0f, 11.0f, 12.0f,
        13.0f, 14.0f, 15.0f, 16.0f
    };
    test_normalMatrixFromMatrix4x4(outMatrix1, matrix4_4);
    
    GLfloat matrix3_3[9] = {
        1.2f, 2.3f, 3.4f,
        4.5f, 5.6f, 6.7f,
        7.8f, 8.9f, 9.0f
    };
    GLfloat outMatrix[9];
    test_transponieren(outMatrix, matrix3_3);
    return 0;
}