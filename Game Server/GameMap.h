#pragma once
#include <d3dx9.h>
#include <d3d9.h>
#include <vector>


#include "MapReader\Tmx.h.in"
#include "Brick.h"
#include "BrickNormal.h"
#include "MetalBrick.h"
#include "Water.h"
#include "Boundary.h"
#include "GameCollision.h"
#include "QuadTree.h"

class GameMap
{
	std::vector<Brick*> brickList;
	Tmx::Map                *map;

public:
	GameMap(char* filePath);
	~GameMap() { delete map; }

	Tmx::Map* GetMap() { return map; }
	std::vector<Brick*> GetListBrick() { return brickList; }

// các hàm hỗ trợ
private:
	void LoadMap(char* filePath);
	int GetWidth() { return map->GetWidth() * map->GetTileWidth(); }
	int GetHeight() { return map->GetHeight() * map->GetTileHeight(); }
	int GetTileWidth() { return map->GetTileWidth(); }
	int GetTileHeight() { return map->GetTileHeight(); }
};

