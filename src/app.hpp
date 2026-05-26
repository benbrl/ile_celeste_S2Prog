#pragma once

#include "raylib.h"
#include "utils/raylibUtils.hpp"
#include "glm/glm.hpp"
#include <vector>

struct ImageGenerationParameters
{
    int noiseSeed{0};
    float noiseScale{4.0f};
    int resolution{256};
    int octaves{4};
    float lacunarity{2.0f};
    float gain{0.5f};
    float amplitude{0.5f};
    float frequency = {1.0f};
};

struct PointsGenerationParameters
{
    // TODO(student): add parameters for points generation (ex: poisson disk radius, etc).
    int numSampleRejection{30};
    float radius{0.08};

    float min_height{0.f};
    float max_height{1.f};

    glm::vec2 sampleRegionSize{1.0f, 1.0f};
};

struct PaletteduTerrain 
{
    Color eau;
    Color sable;
    Color herbe;
    Color roche;
    Color neige;
};

inline const PaletteduTerrain PaletteTropicale
{
    color_from({70,130,180}),
    color_from({238,214,175}),
    color_from({34,139,34}),
    color_from({100,100,100}),
    color_from({255,255,255})
};

inline const PaletteduTerrain PaletteDesert
{
    color_from({30,80,140}),
    color_from({210,180,80}),
    color_from({180,140,60}),
    color_from({120,100,80}),
    color_from({240,240,220})
};

inline const PaletteduTerrain PaletteCeleste
{
    color_from({40, 60, 110}),    
    color_from({210, 230, 245}),  
    color_from({110, 210, 180}),  
    color_from({140, 150, 190}),  
    color_from({255, 255, 255})   
};

struct AppContext
{
    Camera camera{};

    // Store the heightmap as a raylib Image, which is easy to sample from CPU side when generating object positions.
    Image heightmapImage{};

    // This is the image we use for texturing the terrain. It can be the same as heightmapImage, but it doesn't have to be (for example, you could use a color image with RGB channels representing different material types instead of height).
    Image image{};

    // The generated texture from the image, stored here so we can easily bind it when generating the model.
    Texture2D texture{};

    glm::vec3 terrainSize{16.0f, 5.0f, 16.0f};

    // The generated terrain mesh and model.
    Mesh mesh{};
    Model model{};

    std::vector<glm::vec3> objectPositions{};

    // A simple cube mesh and material we use to draw objects on the terrain.
    Mesh cube{};
    Material cubeMaterial{};
    float cubeScale{0.1f};

    // Parameters for object positions generation
    PointsGenerationParameters pointsGenerationParameters;

    // Parameters for island generation
    ImageGenerationParameters imageGenerationParameters;

    PaletteduTerrain colorPalette = PaletteTropicale;
};

Matrix getTerrainCenteringMatrix(AppContext const &context);
float sampleHeightmap(AppContext const &context, float u, float v);
void unload(AppContext &context);
void regenerateMeshFromImage(AppContext &context);