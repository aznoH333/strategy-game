#include "entities.h"
#include "world.h"
#include "utils.h"
#include "stdio.h"


//------------------------------------------------------------------------------------
// Entity cursor
//------------------------------------------------------------------------------------
static EntityCursorHandle entityCursor = (EntityCursorHandle) {
	.selectedTileX = -1,
	.selectedTileY = -1,
	.isTileSelected = false,
	.selectedEntityIndex = 0,
	.selectedEntity = NULL
};

EntityCursorHandle* getEntityCursorHandle() {
	return &entityCursor;
}


//------------------------------------------------------------------------------------
// Controll functions
//------------------------------------------------------------------------------------
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






void initiateBoardUpdate() {
	updateBoardState();

	for ( unsigned int i = 0; i < entities.count; i++ ) {
		Entity* entity = &ArrayGet(entities, i);
		
		revealTilesInRadius(entity->x, entity->y, entity->revealRadius);
	}
}

void entityEnteredTile(int tileX, int tileY, Entity* entity) {
	if (entity->revealRadius > 0) {
		revealTilesInRadius(tileX, tileY, entity->revealRadius);	
	}

	initiateBoardUpdate();
}


void createEntity(
	int x, 
	int y, 
	char* sprite,
	char* name,
	int moveDistance,
	int revealRadius
) {
	ArrayPush(entities, ((Entity)
		{
			.x = x, 
			.y = y, 
			.sprite = sprite, 
			.name = name,
			.moveDistance = moveDistance,
			.revealRadius = revealRadius
		}));
	Entity* addedEntity = &entities.elements[entities.count-1];
	entityEnteredTile(x, y, addedEntity);
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

void selectTile(int x, int y) {
	if (!isInWorldBounds(x, y)) {
		entityCursor.isTileSelected = false;
		entityCursor.selectedEntity = NULL;
		return;
	}
	
	if (x != entityCursor.selectedTileX || y != entityCursor.selectedTileY) {

		entityCursor.selectedTileX = x;
		entityCursor.selectedTileY = y;
		entityCursor.selectedEntityIndex = 0;

	} else {

		entityCursor.selectedEntityIndex++;

	}

	entityCursor.selectedEntity = getEntityOnTile(x, y, entityCursor.selectedEntityIndex); 
	entityCursor.isTileSelected = true;
	
}

void moveEntity(int targetX, int targetY, Entity* entity) {
	if (!isInWorldBounds(targetX, targetY)) {
		return;
	}

	entity->x = targetX;
	entity->y = targetY;


	entityEnteredTile(targetX, targetY, entity);
}



void moveCommand(int targetX, int targetY) {
	if (!isInWorldBounds(targetX, targetY) || !entityCursor.isTileSelected) {
		return;
	}

	Entity* entity = getEntityOnTile(entityCursor.selectedTileX, entityCursor.selectedTileY, entityCursor.selectedEntityIndex);

	if (entity == NULL) {
		return;
	}

	int distance = getTileDistance(targetX, targetY, entity->x, entity->y);


	if (getTileDistance(entity->x, entity->y, targetX, targetY) > entity->moveDistance) {
		return;
	}

	moveEntity(targetX, targetY, entity);
	selectTile(targetX, targetY);
}

