CPP = g++
BIN = whitenoise
SRC = main.cpp
OBJ = WhiteNoise.o main.o PerlinNoise.o NormalNoise.o
LIBS = SDL_image
#CXXFLAGS = `pkg-config --cflags --libs $(LIBS)` -ljpeg -lnoise -Wall -O3 -march=native -std=c++11
CXXFLAGS = `pkg-config --cflags --libs $(LIBS)` -ljpeg -lnoise -Wall --std=c++11

all: $(BIN)

$(BIN): $(OBJ)
	$(CPP) -o $(BIN) $(OBJ) $(CXXFLAGS)

%.o: %.cpp %.h
	$(CPP) -c $(CXXFLAGS) $< -o $@

clean:
	rm $(BIN)
	rm $(OBJ)
	rm -r output
