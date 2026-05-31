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

//------------------------------------------------------------------------------------
// world
//------------------------------------------------------------------------------------
void tempWorldGeneration() {
	printf("size %ld \n", sizeof(int));
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

	
	camera = getCamera();
	cursor = getWorldCursor();
	InitTextureWindow(screenWidth, screenHeight, 256, 224, "empty project");	
	UseShader("./resources/shaders/shaderVert.vs", "./resources/shaders/shaderFrag.fs");
	SetTargetFPS(60);
	// Main game loop

	initNewBoard(32, 32);


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
