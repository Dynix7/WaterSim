CFLAGS = -Wall -Wextra -g -Iinclude -Llib -DGRAPHICS_API_OPENGL_43
LINK = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

water: src/water.cpp src/water.vs src/water.fs src/sky.vs src/sky.fs
	g++ $(CFLAGS) src/water.cpp $(LINK) -o water

rm: water
	rm water

run: water
	./water

