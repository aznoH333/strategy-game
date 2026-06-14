#include "raylib.h"
#include "utils.h"
#include "string.h"
#include "math.h"
#include "world.h"
#include "worldGen.h"
#include "entities.h"

//------------------------------------------------------------------------------------
// Variable declarations
//------------------------------------------------------------------------------------
GameCamera* camera;
WorldCursor* cursor;
BoardHandle* boardHandle;
EntityCursorHandle* entityCursor;

int cursorMode = 0;
char* modelLabels[] = {"reveal mode", "select mode", "move mode"};
void updateCursor() {
	// draw mouse
	spr("ground_tiles_0006", cursor->worldX - camera->x, cursor->worldY - camera->y, 2);
	
	// draw selected indicator
	if (entityCursor->isTileSelected) {
		Vector2 pos = resolveScreenPosition(entityCursor->selectedTileX, entityCursor->selectedTileY);
		 
		spr("ground_tiles_0006", pos.x, pos.y, 1);
	}
	

	drawText(modelLabels[cursorMode], 10, 10, 1, WHITE);
	
	if ( entityCursor->selectedEntity != NULL ) {
		drawText(entityCursor->selectedEntity->name, 10, 32, 1, WHITE);
	}


	if (IsKeyPressed(KEY_ONE)) {
		cursorMode = 0;
	} else if (IsKeyPressed(KEY_TWO)) {
		cursorMode = 1;
	} else if (IsKeyPressed(KEY_THREE)) {
		cursorMode = 2;
	}

	// clicking
	if (IsMouseButtonPressed(0)) {
	
		switch (cursorMode) {
			case 0:
				revealTilesInRadius(cursor->boardX, cursor->boardY, 5);
				break;
			case 1:
				selectTile(cursor->boardX, cursor->boardY);
				break;
			case 2:
				moveCommand(cursor->boardX, cursor->boardY);
				break;

		}
	

	}
}

//------------------------------------------------------------------------------------
// Temp
//------------------------------------------------------------------------------------
void tempWorldStuff() {
	updateEntities();
	updateCursor();
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
	entityCursor = getEntityCursorHandle();

	// init window
	InitTextureWindow(screenWidth, screenHeight, 256, 224, "empty project");	
	UseShader("./resources/shaders/shaderVert.vs", "./resources/shaders/shaderFrag.fs");
	SetTargetFPS(60);
	// Main game loop


	generateNewMap(20, 20);
	// spawn debug guy
	createEntity(0, 0, "pieces_0006", "red guy", 2, 4);
	createEntity(0, 1, "pieces_0007", "blue guy", 2, 3);

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
