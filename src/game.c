#include "raylib.h"
#include "utils.h"
#include "string.h"
#include "math.h"
#include "world.h"



//------------------------------------------------------------------------------------
// Variable declarations
//------------------------------------------------------------------------------------
GameCamera* camera;
WorldCursor* cursor;
BoardHandle* boardHandle;



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

void tempWorldGeneration() {
	
	genNoise();

	for ( int x = 0; x < boardHandle->boardWidth; x++ ) {
		for ( int y = 0; y < boardHandle->boardHeight; y++ ) {
			
			int noiseValue = (int) floor(getNoise(x, y, 8.0f) * 4.0f) + 0;
			

			char* piece = NULL;

			if (noiseValue == 2 && randomChance(0.65f)) {
				piece = pieces[GetRandomValue(2, 3)];

				if (randomChance(0.25f)) {
					piece = pieces[4];
				}
			}

			if (noiseValue == 3 && randomChance(0.85f)) {
				piece = pieces[GetRandomValue(0, 1)];
			}


			setWorldTile(x, y, tiles[noiseValue], piece);
		}
	}
	

}




void tempWorldStuff() {

	// moving camera
	if (IsKeyDown(KEY_W)) {
		camera->y -= 1.5f;
	}
		
	if (IsKeyDown(KEY_S)) {
		camera->y += 1.5f;
	}

	if (IsKeyDown(KEY_A)) {
		camera->x -= 1.5f;
	}
	
	if (IsKeyDown(KEY_D)) {
		camera->x += 1.5f;
	}


	// draw mouse
	spr("ground_tiles_0006", cursor->worldX - camera->x, cursor->worldY - camera->y, 2);
	

	// clicking
	
	if (IsMouseButtonPressed(0)) {
		// TODO : there is a bug here. target position isnt checked properly. can write to unalocated memmory
		// TODO : this is way too repetitive
		// above row
		for (int targetX = cursor->boardX; targetX <= cursor->boardX + 1; targetX++) {
			discoverTile(targetX, cursor->boardY - 1);
		}
		

		// middle row
		for (int targetX = cursor->boardX - 1; targetX <= cursor->boardX + 1; targetX++) {
			discoverTile(targetX, cursor->boardY);
		}

		
		// bottom row
		for (int targetX = cursor->boardX - 1; targetX < cursor->boardX + 1; targetX++) {
			discoverTile(targetX, cursor->boardY + 1);
		}
	}
}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void){
	SetTraceLogLevel(LOG_WARNING); 
	const int screenWidth = 800;
	const int screenHeight = 400;

	// fetch handles
	camera = getCamera();
	cursor = getWorldCursor();
	boardHandle = getBoardHandle();


	// init window
	InitTextureWindow(screenWidth, screenHeight, 256, 224, "empty project");	
	UseShader("./resources/shaders/shaderVert.vs", "./resources/shaders/shaderFrag.fs");
	SetTargetFPS(60);
	// Main game loop

	initNewBoard(20, 20);


	tempWorldGeneration();

	while (!WindowShouldClose()){
		updateWorld();
		
		if (IsKeyPressed(KEY_P)) {
			SwitchResolution(0,0, true);
		}

		
		tempWorldStuff();
		Render();
	}

	CloseWindow();
	return 0;
}
