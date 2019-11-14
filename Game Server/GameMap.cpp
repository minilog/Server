#include "GameMap.h"
#include "GameLog.h"
GameMap::GameMap(char* filePath)
{
	LoadMap(filePath);
}

void GameMap::LoadMap(char* filePath)
{
	map = new Tmx::Map();
	map->ParseFile(filePath);

	// duyệt các layer của map
	for (int i = 0; i < map->GetNumTileLayers(); i++)
	{
		const Tmx::TileLayer *layer = map->GetTileLayer(i);
		if (layer->GetName() == "Brick" || 
			layer->GetName() == "Metal Brick" || 
			layer->GetName() == "Water" || 
			layer->GetName() == "Tile Layer 1") 
		{
			for (int j = 0; j < map->GetNumTilesets(); j++)
			{
				const Tmx::Tileset *tileSet = map->GetTileset(j);

				int tileWidth = map->GetTileWidth();
				int tileHeight = map->GetTileHeight();

				for (int m = 0; m < layer->GetHeight(); m++)
				{
					for (int n = 0; n < layer->GetWidth(); n++)
					{
						if (layer->GetTileTilesetIndex(n, m) != -1)
						{
							// tọa độ object
							D3DXVECTOR2 pos((float)(n * tileWidth + tileWidth / 2),
								(float)(m * tileHeight + tileHeight / 2));

							// khởi tạo brick
							Brick* brick;
							if (layer->GetName() == "Brick")
							{
								brick = new BrickNormal(pos);
								brickNorList.push_back((BrickNormal*)brick);
							}
							else if (layer->GetName() == "Metal Brick")
							{
								brick = new MetalBrick(pos);
							}
							else if (layer->GetName() == "Water")
							{
								brick = new Water(pos);
							}
							else if (layer->GetName() == "Tile Layer 1")
							{
								brick = new Boundary(pos);
							}

							brickList.push_back(brick);
						}
					}
				}
			}
		}
	}
}