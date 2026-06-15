#pragma once

#include "Graphics/Vulkan/Wrappers/Buffer.h"
#include "Math/Lina64.h"

enum TerrainPreset {
    SOMALIA,
    COLUMBIA,
    LIBIA,
    YUGOSLAVIA,
    SEA // sprawdź te nazwy
};

struct TerrainTile {
    uint32_t tileId;
    Tile tileModel;
};


class Terrain {
public:


private:
    LoadSector();


};