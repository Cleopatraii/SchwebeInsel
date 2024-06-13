GLEW_LIBS=$(shell pkg-config glew --libs)
GLFW_LIBS=$(shell pkg-config glfw3 --libs)

# Ziel: Ausfuehrbare Datei
cgIsland: main_island.c vertexShader_island.h fragmentShader_island.h vertexShader_restTeil.h fragmentShader_restTeil.h vertexShader_energyObject.h fragmentShader_energyObject.h
	gcc -g -Wall -o cgIsland main_island.c $(GLEW_LIBS) $(GLFW_LIBS) -lm

cgEnergy: main_energyObject.c fragmentShader_energyObject.h
	gcc -o cgEnergy main_energyObject.c $(GLEW_LIBS) $(GLFW_LIBS) -lm -framework OpenGL

%.h: shader_Programm/%.glsl
	xxd -i $< > $@