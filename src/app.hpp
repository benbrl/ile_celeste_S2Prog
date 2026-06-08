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
    int octaves{7};
    float lacunarity{2.0f};
    float gain{0.574f};
    float amplitude{0.570f};
    float frequency {2.832f};
};

struct PointsGenerationParameters
{
    // TODO(student): add parameters for points generation (ex: poisson disk radius, etc).
    int numSampleRejection{30};
    float radius{0.04};

    float min_height{0.157f};
    float max_height{0.626f};

    glm::vec2 sampleRegionSize{1.0f, 1.0f};
};

struct MaskGenerationParameters
{
    float mask_amplitude{1.f};
    float mask_scale{3.5f};
};

struct PaletteduTerrain
{
    Color eau;
    Color sable;
    Color herbe;
    Color roche;
    Color neige;
};

struct Environnement
{
    PaletteduTerrain biomeSec;
    PaletteduTerrain biomeNormal;
    PaletteduTerrain biomeHumide;
};

inline const Environnement EnvironnementTropical = {
    PaletteduTerrain{
        Color{60, 110, 160, 255},
        Color{240, 220, 180, 255},
        Color{155, 165, 80, 255},
        Color{120, 110, 100, 255},
        Color{255, 255, 255, 255}},
    PaletteduTerrain{
        Color{70, 130, 180, 255},
        Color{238, 214, 175, 255},
        Color{34, 139, 34, 255},
        Color{100, 100, 100, 255},
        Color{255, 255, 255, 255}},
    PaletteduTerrain{
        Color{30, 140, 160, 255},
        Color{220, 200, 160, 255},
        Color{10, 90, 45, 255},
        Color{80, 90, 85, 255},
        Color{240, 250, 250, 255}}};

inline const Environnement EnvironnementDesertique = {
    PaletteduTerrain{
        Color{25, 70, 120, 255},
        Color{225, 195, 100, 255},
        Color{210, 180, 80, 255},
        Color{140, 100, 70, 255},
        Color{245, 235, 210, 255}},
    PaletteduTerrain{
        Color{30, 80, 140, 255},
        Color{210, 180, 80, 255},
        Color{180, 140, 60, 255},
        Color{120, 100, 80, 255},
        Color{240, 240, 220, 255}},
    PaletteduTerrain{
        Color{40, 120, 180, 255},
        Color{200, 170, 80, 255},
        Color{76, 133, 76, 255},
        Color{110, 100, 90, 255},
        Color{255, 255, 255, 255}}};

inline const Environnement EnvironnementCeleste = {
    PaletteduTerrain{
        Color{30, 40, 90, 255},
        Color{190, 200, 230, 255},
        Color{130, 110, 180, 255},
        Color{110, 110, 150, 255},
        Color{230, 240, 255, 255}},
    PaletteduTerrain{
        Color{40, 60, 110, 255},
        Color{210, 230, 245, 255},
        Color{110, 210, 180, 255},
        Color{140, 150, 190, 255},
        Color{255, 255, 255, 255}},
    PaletteduTerrain{
        Color{50, 90, 160, 255},
        Color{220, 240, 250, 255},
        Color{40, 180, 160, 255},
        Color{150, 140, 200, 255},
        Color{255, 255, 255, 255}}};

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
    Model tree = LoadModel("../../resources/low-poly-tree.obj");

    std::vector<glm::vec3> objectPositions{};

    // A simple cube mesh and material we use to draw objects on the terrain.
    Mesh cube{};
    Material cubeMaterial{};
    float cubeScale{0.1f};

    // Parameters for object positions generation
    PointsGenerationParameters pointsGenerationParameters;

    // Parameters for island generation
    ImageGenerationParameters imageGenerationParameters;

    MaskGenerationParameters maskGenerationParameters;

    glm::vec3 vecColEau{
        0.275f,
        0.51f,
        0.706f};
    glm::vec3 vecColSable{0.933, 0.839, 0.686};
    glm::vec3 vecColHerbe{0.133, 0.545, 0.133};
    glm::vec3 vecColRoche{0.392, 0.392, 0.392};
    glm::vec3 vecColNeige{1, 1, 1};

    Color colEau{color_from(vecColEau * 255.f)},
        colSable{color_from(vecColSable * 255.f)},
        colHerbe{color_from(vecColHerbe * 255.f)},
        colRoche{color_from(vecColRoche * 255.f)},
        colNeige{color_from(vecColNeige * 255.f)};

    Environnement EnvironnementCustom = {

        PaletteduTerrain{
            Color{40, 60, 110, 255},
            Color{210, 230, 245, 255},
            Color{110, 210, 180, 255},
            Color{140, 150, 190, 255},
            Color{255, 255, 255, 255}},
        PaletteduTerrain{
            colEau, colSable, colHerbe, colRoche, colNeige},
        PaletteduTerrain{
            Color{50, 90, 160, 255},
            Color{220, 240, 250, 255},
            Color{40, 180, 160, 255},
            Color{150, 140, 200, 255},
            Color{255, 255, 255, 255}}};

    Environnement env = EnvironnementCustom;
};

Matrix getTerrainCenteringMatrix(AppContext const &context);
float sampleHeightmap(AppContext const &context, float u, float v);
void unload(AppContext &context);
void regenerateMeshFromImage(AppContext &context);