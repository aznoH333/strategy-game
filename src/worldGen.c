#include "worldGen.h"
#include "world.h"
#include "raylib.h"
#include "math.h"
#include "utils.h"

//------------------------------------------------------------------------------------
// noise
//------------------------------------------------------------------------------------
float noise[20][20];

void genNoise() {
	for ( int x = 0; x < 20; x++ ) {
		for ( int y = 0; y < 20; y++ ) {
			noise[x][y] = randomFloat(0.0f, 1.0f); 
		}
	}
}


float getNoiseCell(int x, int y) {
	if (x < 0) x += 20;
	if (y < 0) y += 20;

	return noise[x % 20][y % 20];
}

float getNoise(float x, float y, float resolution) {
	x /= resolution;
	y /= resolution;

	int xFloor = (int) floor(x);
	int xCeil = xFloor + 1;
	int yFloor = (int) floor(y);
	int yCeil = yFloor + 1;

	float topLeft = getNoiseCell(xFloor, yFloor);
	float topRight = getNoiseCell(xCeil, yFloor);
	float bottomLeft = getNoiseCell(xFloor, yCeil);
	float bottomRight = getNoiseCell(xCeil, yCeil);



	float top = interpolate(topLeft, topRight, x - xFloor);
	float bottom = interpolate(bottomLeft, bottomRight, x - xFloor);

	return interpolate(top, bottom, y - yFloor);

}


//------------------------------------------------------------------------------------
// world
//------------------------------------------------------------------------------------

char* tiles[] = { "ground_tiles_0001", "ground_tiles_0002", "ground_tiles_0003", "ground_tiles_0004", "ground_tiles_0005" };
char* pieces[] = { "pieces_0001", "pieces_0002", "pieces_0003", "pieces_0004", "pieces_0005", "pieces_0006", "pieces_0007" };


void generateNewMap(int width, int height){

	initNewBoard(width, height);
	BoardHandle* boardHandle = getBoardHandle();

	genNoise();

	for ( int x = 0; x < boardHandle->boardWidth; x++ ) {
		for ( int y = 0; y < boardHandle->boardHeight; y++ ) {
			int noiseValue = (int) floor(getNoise(x, y, 8.0f) * 4.0f) + 0;

			char* piece = NULL;
			
			TerrainType type = LAND;

			if (noiseValue == 0) {
				type = WATER;
			}

			if (noiseValue == 2 && randomChance(0.65f)) {
				piece = pieces[GetRandomValue(2, 3)];

				if (randomChance(0.25f)) {
					piece = pieces[4];
				}
			}

			if (noiseValue == 3 && randomChance(0.85f)) {
				piece = pieces[GetRandomValue(0, 1)];
				type = MOUNTAIN;
			}


			setWorldTile(x, y, tiles[noiseValue], piece, type);
		}
	}
}
