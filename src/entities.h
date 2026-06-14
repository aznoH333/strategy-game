#ifndef ENTITIES
#define ENTITIES

#include "stdbool.h"

typedef struct {
	int x;
	int y;
	char* sprite;
	char* name;
	int revealRadius;
	int moveDistance;
} Entity;
	

//------------------------------------------------------------------------------------
// Controll functions
//------------------------------------------------------------------------------------
void updateEntities();
void createEntity(int x, int y, char* sprite, char* name, int moveDistance, int revealRadius);
Entity* getEntityOnTile(int tileX, int tileY, int entityIndex);
void selectTile(int x, int y);
void moveCommand(int targetX, int targetY);


//------------------------------------------------------------------------------------
// Entity cursor
//------------------------------------------------------------------------------------
typedef struct {
	int selectedTileX;
	int selectedTileY;
	bool isTileSelected;
	int selectedEntityIndex;
	Entity* selectedEntity;
} EntityCursorHandle;

EntityCursorHandle* getEntityCursorHandle();

#endif
