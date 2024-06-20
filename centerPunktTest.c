#include <stdio.h>

#include </opt/homebrew/Cellar/glew/2.2.0_1/include/GL/glew.h>
#include </opt/homebrew/Cellar/glfw/3.4/include/GLFW/glfw3.h>

typedef struct {
    GLfloat v1, v2, v3, t1, t2, n1, n2, n3;
} Flaeche8f;

void objectCenter(Flaeche8f* objVbo, long* objVboLen, GLfloat* centerPunkt) {
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

int main() {
    // 示例数据
    Flaeche8f objVbo[] = {
        {1.0f, 2.0f, 3.0f, 1.0f, 2.0f, 3.0f, 1.0f, 2.0f},
        {4.0f, 5.0f, 6.0f, 1.0f, 2.0f, 3.0f, 1.0f, 2.0f},
        {7.0f, 8.0f, 9.0f, 1.0f, 2.0f, 3.0f, 1.0f, 2.0f},
        {10.0f, 11.0f, 12.0f, 1.0f, 2.0f, 3.0f, 1.0f, 2.0f}
    };
    long objVboLen = 32;  // 计算数据长度

    GLfloat centerPunkt[3];
    objectCenter(objVbo, &objVboLen, centerPunkt);

    // 打印中心点坐标: 5.5, 6.5, 7.5
    printf("objVboLen: %ld\n", objVboLen);
    printf("Object Center: (%f, %f, %f)\n", centerPunkt[0], centerPunkt[1], centerPunkt[2]);

    return 0;
}
