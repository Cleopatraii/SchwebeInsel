GLEW_LIBS=$(shell pkg-config glew --libs)
GLFW_LIBS=$(shell pkg-config glfw3 --libs)
<<<<<<< HEAD
INCLUDE_PATHS=$(shell pkg-config glew --cflags) $(shell pkg-config glfw3 --cflags) -I/usr/include -I/usr/local/include


# Ziel: Ausfuehrbare Datei
cgIsland: main_island.c main_funk.o vertexShader_island.h fragmentShader_island.h vertexShader_restTeil.h fragmentShader_restTeil.h vertexShader_energyObject.h fragmentShader_energyObject.h vertexShader_fullscreen.h fragmentShader_fullscreen.h fragmentShader_blur.h vertexShader_skybox.h fragmentShader_skybox.h 
	gcc -g -Wall -o cgIsland main_island.c main_funk.o $(GLEW_LIBS) $(GLFW_LIBS) -lm 

cgEnergy: main_energyObject.c fragmentShader_energyObject.h 
	gcc -o cgEnergy main_energyObject.c $(GLEW_LIBS) $(GLFW_LIBS) -lm 

cgT: centerPunktTest.c 
	gcc -o cgT centerPunktTest.c $(GLEW_LIBS) $(GLFW_LIBS) -lm -framework OpenGL

cgI2: main_island2.c vertexShader_island.h fragmentShader_island.h vertexShader_restTeil.h fragmentShader_restTeil.h vertexShader_energyObject.h fragmentShader_energyObject.h vertexShader_fullscreen.h fragmentShader_fullscreen.h
	gcc -g -Wall -o cgI2 main_island2.c $(GLEW_LIBS) $(GLFW_LIBS) -lm -framework OpenGL

cgB: backup.c vertexShader_island.h fragmentShader_island.h vertexShader_restTeil.h fragmentShader_restTeil.h vertexShader_energyObject.h fragmentShader_energyObject.h
	gcc -o cgB backup.c $(GLEW_LIBS) $(GLFW_LIBS) -lm -framework OpenGL

%.h: shader_Programm/%.glsl
	xxd -i $< > $@

main_funk.o: main_funk.c main_funk.h
	gcc -c main_funk.c -o main_funk.o $(GLEW_LIBS) $(GLFW_LIBS) $(CFLAGS)

test_funk: test_funk.c main_funk.o
	gcc -o test_funk test_funk.c main_funk.o $(GLEW_LIBS) $(GLFW_LIBS) -lm -lgsl -lgslcblas
=======

# Ziel: Ausfuehrbare Datei
cgIsland: main_island.c vertexShader_island.h fragmentShader_island.h vertexShader_restTeil.h fragmentShader_restTeil.h vertexShader_energyObject.h fragmentShader_energyObject.h
	gcc -g -Wall -o cgIsland main_island.c $(GLEW_LIBS) $(GLFW_LIBS) -lm

cgEnergy: main_energyObject.c fragmentShader_energyObject.h
	gcc -o cgEnergy main_energyObject.c $(GLEW_LIBS) $(GLFW_LIBS) -lm -framework OpenGL

%.h: shader_Programm/%.glsl
	xxd -i $< > $@
>>>>>>> Sprint2
