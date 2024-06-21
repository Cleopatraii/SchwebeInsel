GLEW_LIBS=$(shell pkg-config glew --libs)
GLFW_LIBS=$(shell pkg-config glfw3 --libs)


# Ziel: Ausfuehrbare Datei
cgIsland: main_island.c main_funk.o vertexShader_island.h fragmentShader_island.h vertexShader_restTeil.h fragmentShader_restTeil.h vertexShader_energyObject.h fragmentShader_energyObject.h vertexShader_fullscreen.h fragmentShader_fullscreen.h fragmentShader_blur.h vertexShader_skybox.h fragmentShader_skybox.h 
	gcc -g -Wall -o cgIsland main_island.c main_funk.o $(GLEW_LIBS) $(GLFW_LIBS) -lm 

%.h: shader_Programm/%.glsl
	xxd -i $< > $@

main_funk.o: main_funk.c main_funk.h
	gcc -c main_funk.c -o main_funk.o $(GLEW_LIBS) $(GLFW_LIBS) $(CFLAGS)

test_funk: test_funk.c main_funk.o
	gcc -o test_funk test_funk.c main_funk.o $(GLEW_LIBS) $(GLFW_LIBS) -lm