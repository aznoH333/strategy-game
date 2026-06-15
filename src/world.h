#ifndef _WORLD
#define _WORLD

#include "stdbool.h"
#include "raylib.h"

// -------------------------------------------------------------------------------------
// Structs
// -------------------------------------------------------------------------------------
typedef enum {
	UNDISCOVERED,
	DISCOVERED,
	OBSERVED
} TileVisibility;


typedef enum {
	WATER,
	LAND,
	MOUNTAIN
} TerrainType;


typedef struct {
	char* tileSprite;
	char* tileDecorationSprite;
	int x;
	int y;
	TileVisibility visibility;
	TerrainType terrainType;
} WorldTile;


typedef struct {
	float x;
	float y;
} GameCamera;

typedef struct {
	float screenX;
	float screenY;
	float worldX;
	float worldY;
	int boardX;
	int boardY;
	WorldTile* hoveredTile;
} WorldCursor;


typedef struct {
	int boardWidth;
	int boardHeight;
} BoardHandle;



// -------------------------------------------------------------------------------------
// World updates
// -------------------------------------------------------------------------------------
void updateWorld(); // draws the world and updates the cursor position
void updateBoardState(); // updates the world observation state (makes previously observerd tiles undiscovered)


// -------------------------------------------------------------------------------------
// Functions
// -------------------------------------------------------------------------------------
void initNewBoard(int width, int height);
bool isInWorldBounds(int x, int y);
WorldTile* getWorldTile(int x, int y);
void setWorldTile(int x, int y, char* tileSprite, char* tileDecorationSprite, TerrainType terrainType);
void discoverTile(int x, int y, bool observe);
bool isTileDiscovered(int x, int y);
TileVisibility getTileVisibility(int x, int y);
Vector2 resolveScreenPosition(float gridX, float gridY);
int getTileDistance(int startX, int startY, int endX, int endY);
void revealTilesInRadius(int x, int y, int radius);

// -------------------------------------------------------------------------------------
// Struct handles
// -------------------------------------------------------------------------------------
GameCamera* getCamera();
WorldCursor* getWorldCursor();
BoardHandle* getBoardHandle();

#endif
