#include "GameMap.h"
#include "GameLog.h"
GameMap::GameMap(char* filePath)
{
	LoadMap(filePath);
}
void GameMap::Update(float dt)
{
	for (int i = 0; i < mListBrick.size(); ++i)
	{
		if (mListBrick[i]->isDelete)
		{


			vector<Entity*> list_of_brick_quad;
			this->GetQuadTree()->getAllEntities(list_of_brick_quad);
			for (auto brick2 : list_of_brick_quad)
			{
				if (mListBrick[i]->ID == brick2->ID)
				{
					brick2->isDelete = true;
					break;
				}
			}
			eraseBrick(i);
			i--;
		}
	}
}
void GameMap::LoadMap(char* filePath)
{
	mMap = new Tmx::Map();
	mMap->ParseFile(filePath);
	int cur_ID = 100;
	RECT r;
	r.left = 0;
	r.top = 0;
	r.right = this->GetWidth();
	r.bottom = this->GetHeight();
	mQuadTree = new QuadTree(1, r);
	for (size_t i = 0; i < mMap->GetNumTileLayers(); i++)
	{
		const Tmx::TileLayer *layer = mMap->GetTileLayer(i);
		if (layer->GetName() == "Brick" || layer->GetName() == "Metal Brick" || layer->GetName() == "Water" || layer->GetName() == "Tile Layer 1") {
			for (size_t j = 0; j < mMap->GetNumTilesets(); j++)
			{
				const Tmx::Tileset *tileSet = mMap->GetTileset(j);

				int tileWidth = mMap->GetTileWidth();
				int tileHeight = mMap->GetTileHeight();

				int tileSetWidth = tileSet->GetImage()->GetWidth() / tileWidth;
				int tileSetHeight = tileSet->GetImage()->GetHeight() / tileHeight;

				for (size_t m = 0; m < layer->GetHeight(); m++)
				{
					for (size_t n = 0; n < layer->GetWidth(); n++)
					{
						if (layer->GetTileTilesetIndex(n, m) != -1)
						{
							int tileID = layer->GetTileId(n, m);

							int y = tileID / tileSetWidth;
							int x = tileID - y * tileSetWidth;

							RECT sourceRECT;
							sourceRECT.top = y * tileHeight;
							sourceRECT.bottom = sourceRECT.top + tileHeight;
							sourceRECT.left = x * tileWidth;
							sourceRECT.right = sourceRECT.left + tileWidth;

							RECT bound;
							bound.left = n * tileWidth;
							bound.top = m * tileHeight;
							bound.right = bound.left + tileWidth;
							bound.bottom = bound.top + tileHeight;

							D3DXVECTOR3 position(n * tileWidth + tileWidth / 2, 770 - m * tileHeight + tileHeight / 2, 0);


							Brick* brick;
							if (layer->GetName() == "Brick")
							{
								brick = new BrickNormal(position);
								brick->ID = cur_ID++;

							}
							else if (layer->GetName() == "Metal Brick")
							{
								brick = new MetalBrick(position);
								brick->ID = cur_ID++;
							}
							else if (layer->GetName() == "Water")
							{
								brick = new Water(position);
								brick->ID = cur_ID++;
							}
							else if (layer->GetName() == "Tile Layer 1")
							{
								brick = new Boundary(position);
								brick->ID = cur_ID++;
							}
							else
							{
								brick = nullptr;
							}
							if (brick != nullptr)
							{
								mQuadTree->insertEntity(brick);
								mListBrick.push_back(brick);
							}
						}
					}
				}
			}
		}
	}
	//printf("%i", mListBrick.size());
}
bool GameMap::isContain(RECT rect1, RECT rect2)
{
	if (rect1.left > rect2.right || rect1.right < rect2.left || rect1.top > rect2.bottom || rect1.bottom < rect2.top)
	{
		return false;
	}

	return true;
}
Tmx::Map* GameMap::GetMap()
{
	return mMap;
}
int GameMap::GetWidth()
{
	return mMap->GetWidth() * mMap->GetTileWidth();
}

int GameMap::GetHeight()
{
	return mMap->GetHeight() * mMap->GetTileHeight();
}
int GameMap::GetTileWidth()
{
	return mMap->GetTileWidth();
}

int GameMap::GetTileHeight()
{
	return mMap->GetTileHeight();
}
std::vector<Brick*> GameMap::GetListBrick()
{
	return mListBrick;
}
void GameMap::eraseBrick(int i)
{
	mListBrick.erase(mListBrick.begin() + i);
}

RECT GameMap::GetWorldMapBound()
{
	RECT bound;
	bound.left = bound.top = 0;
	bound.right = mMap->GetWidth() * mMap->GetTileWidth();
	bound.bottom = mMap->GetHeight() * mMap->GetTileHeight();

	return bound;
}

QuadTree* GameMap::GetQuadTree()
{
	return mQuadTree;
}

GameMap::~GameMap()
{
	delete mMap;

}
