#include "raylib.h"
#include "utils.h"
#include "string.h"
#include "math.h"
#include "world.h"
#include "worldGen.h"


//------------------------------------------------------------------------------------
// Variable declarations
//------------------------------------------------------------------------------------
GameCamera* camera;
WorldCursor* cursor;
BoardHandle* boardHandle;

//------------------------------------------------------------------------------------
// Entities
//------------------------------------------------------------------------------------
typedef struct {
	int x;
	int y;
	char* sprite;
	bool revealTiles;
} Entity;

#define MAX_ENTITIES 100
GenArrayDefinition(Entity, MAX_ENTITIES, EntityArray);

EntityArray entities = {0};

void updateEntities() {
	for ( unsigned int i = 0; i < entities.count; i++ ) {
		Entity* entity = &ArrayGet(entities, i);
		

		Vector2 screenPosition = resolveScreenPosition(entity->x, entity->y);
		spr(entity->sprite, screenPosition.x, screenPosition.y, 1);
	}
}


void revealTilesInRadius(int x, int y, int radius) {
	for (int xi = -radius; xi <= radius; xi++) {
		for (int yi = -radius; yi <= radius; yi++) {
			if (yi + xi <  -radius + (radius * (int)(3.0f / 4.0f))) {
				continue;
			}


			if (yi + xi > radius - (radius * (int)(3.0f / 4.0f))) {
				continue;
			}
			discoverTile(xi + x, yi + y);
		}
	}
}

void entityEnteredTile(int tileX, int tileY, Entity* entity) {
	if (entity->revealTiles) {
		revealTilesInRadius(tileX, tileY, 2);	
	}
}


void createEntity(int x, int y, char* sprite) {
	ArrayPush(entities, ((Entity){.x = x, .y = y, .sprite = sprite, .revealTiles = true}));
	Entity* addedEntity = &entities.elements[entities.count-1];
	entityEnteredTile(x, y, addedEntity);
}



int selectedTileX = -1;
int selectedTileY = -1;
bool isTileSelected = false;
int selectedEntityIndex = 0;
void selectTile(int x, int y) {
	if (!isInWorldBounds(x, y)) {
		isTileSelected = false;
		return;
	}
	
	if (x != selectedTileX || y != selectedTileY) {

		selectedTileX = x;
		selectedTileY = y;
		selectedEntityIndex = 0;

	} else {

		selectedEntityIndex++;

	}


	isTileSelected = true;
	
}

void moveEntity(int targetX, int targetY, Entity* entity) {
	if (!isInWorldBounds(targetX, targetY)) {
		return;
	}

	entity->x = targetX;
	entity->y = targetY;


	entityEnteredTile(targetX, targetY, entity);
}


Entity* getEntityOnTile(int tileX, int tileY, int entityIndex) {
	Entity* results[10] = {0};
	int resultCount = 0;

	for (unsigned int i = 0; i < entities.count; i++) {
		Entity* entity = &entities.elements[i];

		if (entity->x == tileX && entity->y == tileY) {
			results[resultCount] = entity;
			resultCount++;
			if (resultCount >= 10) {
				break;
			}
		}
	}
	if (resultCount == 0) {
		return NULL;
	}

	return results[entityIndex % resultCount];
}

void moveCommand(int targetX, int targetY) {
	if (!isInWorldBounds(targetX, targetY) || !isTileSelected) {
		return;
	}

	Entity* entity = getEntityOnTile(selectedTileX, selectedTileY, selectedEntityIndex);

	if (entity == NULL) {
		printf("selected entity is null\n");
		return;
	}

	moveEntity(targetX, targetY, entity);
	selectTile(targetX, targetY);
}


int cursorMode = 0;
char* modelLabels[] = {"reveal mode", "select mode", "move mode"};
void updateCursor() {
	
	// draw mouse
	spr("ground_tiles_0006", cursor->worldX - camera->x, cursor->worldY - camera->y, 2);
	
	// draw selected indicator
	if (isTileSelected) {
		Vector2 pos = resolveScreenPosition(selectedTileX, selectedTileY);
		 
		spr("ground_tiles_0006", pos.x, pos.y, 1);
	}

	drawText(modelLabels[cursorMode], 10, 10, 1, WHITE);


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
				revealTilesInRadius(cursor->boardX, cursor->boardY, 2);
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


	// init window
	InitTextureWindow(screenWidth, screenHeight, 256, 224, "empty project");	
	UseShader("./resources/shaders/shaderVert.vs", "./resources/shaders/shaderFrag.fs");
	SetTargetFPS(60);
	// Main game loop


	generateNewMap(20, 20);
	// spawn debug guy
	createEntity(0, 0, "pieces_0006");
	createEntity(0, 1, "pieces_0007");

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
