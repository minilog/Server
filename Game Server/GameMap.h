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
public:
	GameMap(char* filePath);
	void Update(float dt);
	Tmx::Map* GetMap();
	RECT GetWorldMapBound();
	QuadTree* GetQuadTree();
	int GetWidth();
	int GetHeight();
	int GetTileWidth();
	int GetTileHeight();

	std::vector<Brick*> GetListBrick();
	void eraseBrick(int i);
	~GameMap();
private:
	QuadTree * mQuadTree;
	void LoadMap(char* filePath);
	bool isContain(RECT rect1, RECT rect2);

	Tmx::Map                *mMap;
	std::vector<Brick*> mListBrick;

};

