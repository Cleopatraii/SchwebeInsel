GLEW_LIBS=$(shell pkg-config glew --libs)
GLFW_LIBS=$(shell pkg-config glfw3 --libs)
GLFW_CFLAGS=-I/usr/include/GLFW

# Ziel: Ausfuehrbare Datei
cg1: main.c vertexShader.h fragmentShader.h
	gcc -o cg1 main.c $(GLEW_LIBS) $(GLFW_LIBS) $(GLFW_CFLAGS) -lm -lGL
%.h: %.glsl
	xxd -i $< > $@

clean:
	rm -f cg1

