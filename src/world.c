#include "world.h"
#include "utils.h"
#include "math.h"
#include "stdlib.h"


// -------------------------------------------------------------------------------------
// Variables
// -------------------------------------------------------------------------------------
char* tileTextures[] = {"ground_tiles_0001", "ground_tiles_0002", "ground_tiles_0003", "ground_tiles_0004", "ground_tiles_0005"}; 
char* tileObjects[] = {"pieces_0001", "pieces_0002", "pieces_0003"};

#define MAX_BOARD_WIDTH 32
#define MAX_BOARD_HEIGHT 32

#define TILE_WIDTH 23
#define TILE_HEIGHT 20
#define ODD_TILE_OFFSET 10
#define TILE_GAP 0

WorldTile world[MAX_BOARD_WIDTH][MAX_BOARD_HEIGHT];

static GameCamera camera = (GameCamera){ .x = 0.0f, .y = 0.0f };

static WorldCursor cursor = (WorldCursor) { 0 };

static BoardHandle board = (BoardHandle) {
	.boardWidth = 0,
	.boardHeight = 0
};

// -------------------------------------------------------------------------------------
// World generation
// -------------------------------------------------------------------------------------
void initNewBoard(int width, int height){
	
	if (width > MAX_BOARD_WIDTH || height > MAX_BOARD_HEIGHT) {
		printf("invalid board size %d x %d \n", width, height);
		exit(1);
	}

	board.boardWidth = width;
	board.boardHeight = height;

	// generate world
	for (int x = 0; x < board.boardWidth; x++) {
		for (int y = 0; y < board.boardHeight; y++) {
			world[x][y] = (WorldTile){ 
				.tileSprite = "ground_tiles_0001", 
				.tileDecorationSprite = NULL,
				.x = x, 
				.y = y, 
				.visibility = UNDISCOVERED, 
			};
			
		}
	}

}



// -------------------------------------------------------------------------------------
// Utility functions
// -------------------------------------------------------------------------------------
bool isInWorldBounds(int x, int y) {
	return x >= 0 && y >= 0 && x < board.boardWidth && y < board.boardHeight;
}

WorldTile* getWorldTile(int x, int y) {
	return &world[x][y];
}

void setWorldTile(int x, int y, char* tileSprite, char* tileDecorationSprite) {
	if (!isInWorldBounds(x, y)) return;
		
	WorldTile* tile = getWorldTile(x, y);

	tile->tileSprite = tileSprite;
	tile->tileDecorationSprite = tileDecorationSprite;
}

void discoverTile(int x, int y, bool observe) {
	if (!isInWorldBounds(x, y)) return;
	
	WorldTile* tile = getWorldTile(x, y);
	
	
	tile->visibility = observe ? OBSERVED : max(DISCOVERED, tile->visibility);
}


bool isTileDiscovered(int x, int y){
	return getWorldTile(x, y)->visibility != UNDISCOVERED;
}

TileVisibility getTileVisibility(int x, int y) {
	return getWorldTile(x, y)->visibility;
}


Vector2 resolveScreenPosition(float gridX, float gridY) {
	return (Vector2) {
		.x = gridX * (TILE_WIDTH + TILE_GAP) - camera.x, 
		.y = gridY * TILE_HEIGHT + (gridX * ODD_TILE_OFFSET) - camera.y
	};
}

int getTileDistance(int startX, int startY, int endX, int endY) {

	// convert from axial coordinates to cube coordinates
	// then calculate the distance
	// this works because we asume that x+y+z=0

	int startZ = -startX -startY;
	int endZ = -endX -endY;
	

	return (
		abs(startX - endX) + 
		abs(startY - endY) +
		abs(startZ - endZ)
	) / 2;
}

// -------------------------------------------------------------------------------------
// World update
// -------------------------------------------------------------------------------------
void updateWorld() {
	// draw world
	for (int x = 0; x < board.boardWidth; x++) {
		for (int y = 0; y < board.boardHeight; y++) {
			// draw main tile
			WorldTile* tile = &world[x][y];
		
			char* sprite;
			
			Color tileColor = WHITE;

			if (tile->visibility != UNDISCOVERED) {
				sprite = tile->tileSprite;

				if (tile->visibility == DISCOVERED) {
					tileColor = GRAY;
				}

			} else {
				sprite = "ground_tiles_0005";
			}

			Vector2 screenPosition = resolveScreenPosition(x, y); 

			sprFRC(
				sprite, 
				screenPosition.x, 
				screenPosition.y, 
				false, 
				false, 
				0.0f, 
				tileColor, 
				0
			);

			// draw object
			if (tile->tileDecorationSprite == NULL || tile->visibility == UNDISCOVERED) {
				continue;
			}

			sprFRC(
				tile->tileDecorationSprite, 
				screenPosition.x, 
				screenPosition.y, 
				false, 
				false, 
				0.0f, 
				tileColor,
				1
			);
		}
	}

	// update cursor
	Vector2 mousePos = getMousePosition();

	int boardMouseX = round((mousePos.x + camera.x) / TILE_WIDTH);
	float yOffset = (boardMouseX * ODD_TILE_OFFSET); 
	int boardMouseY = round((mousePos.y - yOffset + camera.y) / TILE_HEIGHT);
	float worldMouseX = ((float)boardMouseX) * TILE_WIDTH;
	float worldMouseY = ((float)boardMouseY) * TILE_HEIGHT + yOffset;


	cursor.screenX = mousePos.x;
	cursor.screenY = mousePos.y;
	cursor.worldX = worldMouseX;
	cursor.worldY = worldMouseY;
	cursor.boardX = boardMouseX;
	cursor.boardY = boardMouseY;

	if (isInWorldBounds(cursor.boardX, cursor.boardY)) {
		cursor.hoveredTile = getWorldTile(cursor.boardX, cursor.boardY); 
	}
}


void updateBoardState() {
	for (int x = 0; x < board.boardWidth; x++) {
		for (int y = 0; y < board.boardHeight; y++) {
			WorldTile* tile = &world[x][y];

			if (tile->visibility == OBSERVED) {
				tile->visibility = DISCOVERED;
			}
		}
	}
}


// -------------------------------------------------------------------------------------
// Struct handles
// -------------------------------------------------------------------------------------
GameCamera* getCamera() {
	return &camera;
}

WorldCursor* getWorldCursor() {
	return &cursor;
}


BoardHandle* getBoardHandle(){
	return &board;
}
