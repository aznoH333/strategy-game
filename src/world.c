#include "world.h"
#include "utils.h"
#include "math.h"



// -------------------------------------------------------------------------------------
// Variables
// -------------------------------------------------------------------------------------
char* tileTextures[] = {"ground_tiles_0001", "ground_tiles_0002", "ground_tiles_0003", "ground_tiles_0004", "ground_tiles_0005"}; 
char* tileObjects[] = {"pieces_0001", "pieces_0002", "pieces_0003"};

#define BOARD_WIDTH 32
#define BOARD_HEIGHT 32

#define TILE_WIDTH 23
#define TILE_HEIGHT 20
#define ODD_TILE_OFFSET 10
#define TILE_GAP 0

WorldTile world[BOARD_WIDTH][BOARD_HEIGHT];

static GameCamera camera = (GameCamera){ .x = 0.0f, .y = 0.0f };

static WorldCursor cursor = (WorldCursor) { 0 };

// -------------------------------------------------------------------------------------
// World generation
// -------------------------------------------------------------------------------------
void initWorld() {
	// generate world
	for (int x = 0; x < BOARD_WIDTH; x++) {
		for (int y = 0; y < BOARD_HEIGHT; y++) {

			int type = 0;
			int tileObject = -1;

			if (randomChance(0.6f)) {
				type = 1;
				if (randomChance(0.6f)) {
					tileObject = 1;
				}
			}


			else if (randomChance(0.2f)) {
				type = 0;
			}

			else if (randomChance(0.4f)) {
				type = 2;
			}

			else {
				type = 3;
				if (randomChance(0.1f)) {
					tileObject = 0;
				}
			}

		


			world[x][y] = (WorldTile){ 
				.tileSprite = tileTextures[type], 
				.tileDecorationSprite = NULL,
				.x = x, 
				.y = y, 
				.discovered = false 
			};
			
		}
	}

}


// -------------------------------------------------------------------------------------
// World update
// -------------------------------------------------------------------------------------
void updateWorld() {
	// draw world
	
	for (int x = 0; x < BOARD_WIDTH; x++) {
		for (int y = 0; y < BOARD_HEIGHT; y++) {
			// draw main tile
			WorldTile* tile = &world[x][y];
		
			char* sprite;

			if (tile->discovered) {
				sprite = tile->tileSprite; 
			} else {
				sprite = "ground_tiles_0005";
			}

			spr(sprite, x * (TILE_WIDTH + TILE_GAP) - camera.x, y * TILE_HEIGHT + (x * ODD_TILE_OFFSET) - camera.y, 0);

			// draw object
			if (tile->tileDecorationSprite == NULL || !tile->discovered) {
				continue;
			}

			spr(tile->tileDecorationSprite, x * (TILE_WIDTH + TILE_GAP) - camera.x, y * TILE_HEIGHT + (x * ODD_TILE_OFFSET) - camera.y, 1);
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


GameCamera* getCamera() {
	return &camera;
}

WorldCursor* getWorldCursor() {
	return &cursor;
}


bool isInWorldBounds(int x, int y) {
	return x >= 0 && y >= 0 && x < BOARD_WIDTH && y < BOARD_HEIGHT;
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

void discoverTile(int x, int y) {
	if (!isInWorldBounds(x, y)) return;
	
	WorldTile* tile = getWorldTile(x, y);
	tile->discovered = true;
}
