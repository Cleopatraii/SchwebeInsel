#ifndef MAIN_FUNK_H
#define MAIN_FUNK_H

void normierung(GLfloat *out, const GLfloat* in);
void kopieren (GLfloat* neu,  const GLfloat* alt);
void  matrix_multiply(GLfloat* result,  const GLfloat* a,  const GLfloat* b);
void lookAt(GLfloat* out, GLfloat* eye, GLfloat* center, GLfloat* up) ;
void perspective(GLfloat* out, GLfloat fovy, GLfloat aspect, GLfloat near, GLfloat far);
void printMatrix4x4(GLfloat* out);
void printMatrix3x3(GLfloat* out);
void identity(GLfloat* out);
void translate(GLfloat* out, GLfloat* in, GLfloat* v);
void scale(GLfloat* out, GLfloat* in, GLfloat* v);
void rotatex(GLfloat* out, GLfloat* in, GLfloat angle);
void rotatey(GLfloat* out, GLfloat* in, GLfloat angle);
void rotatez(GLfloat* out, GLfloat* in, GLfloat angle);
void inverseMatrix3x3(float* result, float* mat);
void normalMatrixFromMatrix4x4(float* normalMat, float* mat);
void transponieren(float* transpMatrix, float* mat);
#endif