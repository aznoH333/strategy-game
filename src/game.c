#include "raylib.h"
#include "utils.h"
#include "string.h"
#include "math.h"



//------------------------------------------------------------------------------------
// World map
//------------------------------------------------------------------------------------

char* tileTextures[] = {"ground_tiles_0001", "ground_tiles_0002", "ground_tiles_0003", "ground_tiles_0004", "ground_tiles_0005"}; 

typedef struct {
	int tileTexture;
	int x;
	int y;
	bool discovered;
} WorldTile;


#define BOARD_WIDTH 10
#define BOARD_HEIGHT 10

#define TILE_WIDTH 23
#define TILE_HEIGHT 20
#define ODD_TILE_OFFSET 10
#define TILE_GAP 0

WorldTile world[BOARD_WIDTH][BOARD_HEIGHT];
void initWorld() {
	// generate world
	for (int x = 0; x < BOARD_WIDTH; x++) {
		for (int y = 0; y < BOARD_HEIGHT; y++) {

			world[x][y] = (WorldTile){ .tileTexture = GetRandomValue(0, 3), .x = x, .y = y, .discovered = false };
			
		}
	}
}

void updateWorld() {
	// draw world
	
	for (int x = 0; x < BOARD_WIDTH; x++) {
		for (int y = 0; y < BOARD_HEIGHT; y++) {
			
			WorldTile* tile = &world[x][y];
		
			char* sprite;

			if (tile->discovered) {
				sprite = tileTextures[tile->tileTexture]; 
			} else {
				sprite = "ground_tiles_0005";
			}

			
			spr(sprite, x * (TILE_WIDTH + TILE_GAP), y * TILE_HEIGHT + (x * ODD_TILE_OFFSET), 0);


		}
	}

	// mouse highlight
	Vector2 mousePos = getMousePosition();

	int boardMouseX = round(mousePos.x / TILE_WIDTH);
	float yOffset = (boardMouseX * ODD_TILE_OFFSET); 
	int boardMouseY = round((mousePos.y - yOffset) / TILE_HEIGHT);
	float worldMouseX = ((float)boardMouseX) * TILE_WIDTH;
	float worldMouseY = ((float)boardMouseY) * TILE_HEIGHT + yOffset;


	spr("ground_tiles_0006", worldMouseX, worldMouseY, 2);

	// clicking
	
	if (IsMouseButtonPressed(0)) {
		
		// above row
		for (int targetX = boardMouseX; targetX <= boardMouseX + 1; targetX++) {
			if (targetX < BOARD_WIDTH && targetX >= 0 && boardMouseY < BOARD_HEIGHT && boardMouseY >= 0) {
				WorldTile* tile = &world[targetX][boardMouseY - 1];
				tile->discovered = true;
			}
		}
		

		// middle row
		for (int targetX = boardMouseX - 1; targetX <= boardMouseX + 1; targetX++) {
			if (targetX < BOARD_WIDTH && targetX >= 0 && boardMouseY < BOARD_HEIGHT && boardMouseY >= 0) {
				WorldTile* tile = &world[targetX][boardMouseY];
				tile->discovered = true;
			}
		}

		
		// bottom row
		for (int targetX = boardMouseX - 1; targetX < boardMouseX + 1; targetX++) {
			if (targetX < BOARD_WIDTH && targetX >= 0 && boardMouseY < BOARD_HEIGHT && boardMouseY >= 0) {
				WorldTile* tile = &world[targetX][boardMouseY + 1];
				tile->discovered = true;
			}
		}
	}
	

}


//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
	SetTraceLogLevel(LOG_WARNING); 
	const int screenWidth = 800;
	const int screenHeight = 400;

	InitTextureWindow(screenWidth, screenHeight, 256, 224, "empty project");	
	UseShader("./resources/shaders/shaderVert.vs", "./resources/shaders/shaderFrag.fs");
	SetTargetFPS(60);
	// Main game loop

	initWorld();
	while (!WindowShouldClose()){
		updateWorld();
		
		if (IsKeyPressed(KEY_P)) {
			SwitchResolution(0,0, true);
		}

		Render();
	}

	CloseWindow();
	return 0;
}
