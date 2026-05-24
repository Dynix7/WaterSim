CFLAGS = -Wall -Wextra -g -Iinclude 
LIBS = -lraylib -lGL -lm -lpthread -dl -lrt -lX11 -DGRAPHICS_API_OPENGL_43

water: water.cpp vert.vs frag.fg
	g++ $(CFLAGS) water.cpp $(LIBS) -o water
	./water

rm: water
	rm water

run: water
	./water

