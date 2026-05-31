#ifndef _WORLD
#define _WORLD

#include "stdbool.h"



// -------------------------------------------------------------------------------------
// Structs
// -------------------------------------------------------------------------------------
typedef struct {
	char* tileSprite;
	char* tileDecorationSprite;
	int x;
	int y;
	bool discovered;
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



// -------------------------------------------------------------------------------------
// Functions
// -------------------------------------------------------------------------------------
void initWorld();
void updateWorld();
bool isInWorldBounds(int x, int y);
GameCamera* getCamera();
WorldCursor* getWorldCursor();
WorldTile* getWorldTile(int x, int y);
void setWorldTile(int x, int y, char* tileSprite, char* tileDecorationSprite);
void discoverTile(int x, int y);

#endif
