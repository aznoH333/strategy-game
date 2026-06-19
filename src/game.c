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

typedef struct {
	int x;
	int y;
} PathDirection;

typedef struct {
	int x;
	int y;
	int prevX;
	int prevY;
	int prevIndex;
	bool exhausted;
} SearchNode;

bool pathReached = false;
int pathStartX = 0;
int pathStartY = 0;
int pathEndX = 0;
int pathEndY = 0;
bool pathExists = false;
PathDirection pathNodes[20];
int pathNodeCount = 0;


void drawPath() {
	if (!pathExists) {
		return;
	}

	int x = pathStartX;
	int y = pathStartY;

	for (int i = 0; i < pathNodeCount; i++ ) {

		
		Vector2 pos = resolveScreenPosition(x, y);
		
		spr("ground_tiles_0007", pos.x, pos.y, 7);

		PathDirection direction = pathNodes[i];

		
		x += direction.x;
		y += direction.y;
	}
}

#define MAX_PATH_FIND_ATTEMPTS 100
void startPath(int startX, int startY, int endX, int endY) {
	pathExists = true;

	pathStartX = startX;
	pathStartY = startY;
	pathEndX = endX;
	pathEndY = endY;
	
	pathNodeCount = 0;

	// build path
	int searchNodeCount = 1;
	SearchNode searchNodes[MAX_PATH_FIND_ATTEMPTS];
	searchNodes[0] = (SearchNode) { .x = startX, .y = startY, .prevX = -1, .prevY = -1, .prevIndex = -1, .exhausted = false };
	int finalSearchNodeIndex = -1;


	while (searchNodeCount < MAX_PATH_FIND_ATTEMPTS) {
		// find best node to expand
		int bestExpansionCandidateIndex = -1;
		int bestDistance = 9999;

		for (int i = 0; i < searchNodeCount; i++) {
			if (searchNodes[i].exhausted) {
				continue;
			}

			int distance = getTileDistance(searchNodes[i].x, searchNodes[i].y, endX, endY);

			if (distance < bestDistance) {
				bestDistance = distance;
				bestExpansionCandidateIndex = i;
			}
		}


		if (bestExpansionCandidateIndex == -1) {
			goto failed_to_find_path; // no path found
		}


		// expand node
		SearchNode* currentNode = &searchNodes[bestExpansionCandidateIndex];
		currentNode->exhausted = true;


		if (currentNode->x == endX && currentNode->y == endY) {
			finalSearchNodeIndex = bestExpansionCandidateIndex;
			goto reached_end;
		}


		const int directions[6][2] = { // list of all the hexagonal directions (clockwise)
			{  0, -1 },
			{  1, -1 },
			{  1,  0 },
			{  0,  1 },
			{ -1,  1 },
			{ -1,  0 }
		};


		for (int i = 0; i < 6; i++) {
			int direction[2] = { directions[i][0], directions[i][1] };
			int newNodeX = currentNode->x + direction[0];
			int newNodeY = currentNode->y + direction[1];


			WorldTile* currentTile = getWorldTile(newNodeX, newNodeY);

			if (currentTile->terrainType != LAND) {
				skip_cycle:
				continue;
			}

			// look if node already exists
			for (int j = 0; j < searchNodeCount; j++) {
				if (searchNodes[j].x == newNodeX && searchNodes[j].y == newNodeY) {
					goto skip_cycle;
				}
			}


			// make new node
			searchNodes[searchNodeCount++] = (SearchNode) {
				.x = newNodeX,
				.y = newNodeY,
				.prevX = currentNode->x,
				.prevY = currentNode->y,
				.prevIndex = bestExpansionCandidateIndex, 
				.exhausted = false

			};

			if (searchNodeCount >= MAX_PATH_FIND_ATTEMPTS) {
				goto failed_to_find_path;
			}
			

		}


	}


	failed_to_find_path:
	pathExists = false;
	// failed to find node
	return;
	
	reached_end:
	SearchNode* currentNode = &searchNodes[finalSearchNodeIndex];
	
	PathDirection tempPathNodes[20];
	int tempPathNodeCount = 0;

	while (currentNode->prevIndex != -1) {
		tempPathNodes[tempPathNodeCount++] = (PathDirection) {
			.x = currentNode->x - currentNode->prevX,
			.y = currentNode->y - currentNode->prevY
		};

		currentNode = &searchNodes[currentNode->prevIndex];
	}

	// copy to output

	pathNodeCount = tempPathNodeCount;
	for (int i = 0; i < pathNodeCount; i++) {

		pathNodes[i] = (PathDirection) {
			.x = tempPathNodes[pathNodeCount - i - 1].x,
			.y = tempPathNodes[pathNodeCount - i - 1].y
		};
	}
}


int cursorMode = 0;
char* modelLabels[] = {"reveal mode", "select mode", "move mode"};
void updateCursor() {
	// draw mouse
	if (isInWorldBounds(cursor->boardX, cursor->boardY)) {
		
		WorldTile* tile = getWorldTile(cursor->boardX, cursor->boardY);

		char* cursorSprite = "ground_tiles_0006";

		if (tile->terrainType != LAND) {
			cursorSprite = "ground_tiles_0009";
		}

		spr(cursorSprite, cursor->worldX - camera->x, cursor->worldY - camera->y, 2);
	}
	drawPath();
	
	// draw selected indicator
	if (entityCursor->isTileSelected) {
		Vector2 pos = resolveScreenPosition(entityCursor->selectedTileX, entityCursor->selectedTileY);
		 
		spr("ground_tiles_0008", pos.x, pos.y, 1);
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


	if (cursorMode == 2) {
			startPath(entityCursor->selectedTileX, entityCursor->selectedTileY, cursor->boardX, cursor->boardY);
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
	createEntity(5, 5, "pieces_0006", "red guy", 2, 4);
	createEntity(10, 10, "pieces_0007", "blue guy", 3, 5);

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
