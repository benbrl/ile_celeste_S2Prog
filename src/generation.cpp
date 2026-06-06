#include "generation.hpp"

#include "noise.hpp"
#include "raylib.h"

#include "utils/raylibUtils.hpp"
#include <algorithm> // for std::clamp
#include <numbers>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>
#include <iostream>

bool isValid(glm::vec2 candidate, glm::vec2 sampleRegionSize, float cellSize, float radius, std::vector<glm::vec2> &points, std::vector<std::vector<int>> &grid)
{
    if (candidate.x >= 0 && candidate.x < sampleRegionSize.x && candidate.y >= 0 && candidate.y < sampleRegionSize.y)
    {
        int cellX = static_cast<int>(candidate.x / cellSize);
        int cellY = static_cast<int>(candidate.y / cellSize);

        int searchStartX = std::max(0, cellX - 2);
        int searchEndX = std::min(cellX + 2, static_cast<int>(grid.size()) - 1);

        int searchStartY = std::max(0, cellY - 2);
        int searchEndY = std::min(cellY + 2, static_cast<int>(grid[0].size()) - 1);

        for (int x = searchStartX; x <= searchEndX; x++)
        {
            for (int y = searchStartY; y <= searchEndY; y++)
            {
                int pointIndex = grid[x][y] - 1;
                if (pointIndex != -1)
                {
                    float distance_carre = glm::length2(candidate - points[pointIndex]);
                    if (distance_carre < (radius * radius))
                    {
                        return false;
                    }
                }
            }
        }
    }
    else
    {
        return false;
    }
    return true;
}

std::vector<glm::vec2> generate2DPositions([[maybe_unused]] PointsGenerationParameters const &params)
{

    float cellSize = params.radius / std::sqrt(2.0f);

    int cellX = static_cast<int>(std::ceil(params.sampleRegionSize.x / cellSize));
    int cellY = static_cast<int>(std::ceil(params.sampleRegionSize.y / cellSize));

    std::vector<std::vector<int>> grid(cellX, std::vector<int>(cellY, 0));

    std::vector<glm::vec2> points{};
    std::vector<glm::vec2> spawnPoints{};

    spawnPoints.push_back(params.sampleRegionSize / static_cast<glm::vec2>(2.0));

    while (spawnPoints.size() > 0 && points.size() < 1000)
    {

        int spawnIndex = rand() % spawnPoints.size();

        glm::vec2 spawnCenter = spawnPoints[spawnIndex];

        bool candidateAccepted = false;

        for (int i = 0; i < params.numSampleRejection; i++)
        {

            float angle = std::rand() * std::numbers::pi * 2.0f;
            glm::vec2 dir = {std::sin(angle), std::cos(angle)};

            float min = params.radius;
            float max = params.radius * 2;

            float random = static_cast<float>(rand()) / RAND_MAX;
            float range = min + random * (max - min);

            glm::vec2 candidate = spawnCenter + dir * range;

            if (isValid(candidate, params.sampleRegionSize, cellSize, params.radius, points, grid))
            {
                points.push_back(candidate);
                spawnPoints.push_back(candidate);

                int cellX = static_cast<int>(candidate.x / cellSize);
                int cellY = static_cast<int>(candidate.y / cellSize);
                grid[cellX][cellY] = static_cast<int>(points.size());

                candidateAccepted = true;

                break;
            }
        }
        if (!candidateAccepted)
        {
            spawnPoints.erase(spawnPoints.begin() + spawnIndex);
        }
    };

    return points;
}

void generateObjectsPositions(AppContext &context)
{
    std::vector<glm::vec2> const positions{generate2DPositions(context.pointsGenerationParameters)};

    context.objectPositions.clear();
    context.objectPositions.reserve(positions.size());
    float min_height = context.pointsGenerationParameters.min_height; // en dessous de la mer
    float max_height = context.pointsGenerationParameters.max_height; // au dessus des nuages tah le dragon celeste
    for (glm::vec2 const &p : positions)
    {
        /*
        context.objectPositions.emplace_back(
            p.x, // x
            p.y, // y
                 // sample height from heightmap for each point (asuming positions are normalized in [0..1] range)

            sampleHeightmap(context, p.x, p.y));
        */
        float height = sampleHeightmap(context, p.x, p.y);
        if (height >= min_height && height <= max_height)
        {
            context.objectPositions.emplace_back(p.x, p.y, height);
        }
    }
    // TODO(student): extension - filter positions by sampled height range.
}

float sampleHeightmap(AppContext const &context, float u, float v)
{
    if (!context.heightmapImage.data || context.heightmapImage.width <= 0 || context.heightmapImage.height <= 0)
        return 0.0f;

    int const px = std::clamp(static_cast<int>(u * static_cast<float>(context.heightmapImage.width - 1)), 0, context.heightmapImage.width - 1);
    int const py = std::clamp(static_cast<int>(v * static_cast<float>(context.heightmapImage.height - 1)), 0, context.heightmapImage.height - 1);

    // If the heightmap is in R32 format, we can directly read the height value as a float.
    if (context.heightmapImage.format == PIXELFORMAT_UNCOMPRESSED_R32)
    {
        float const *heightData = static_cast<float const *>(context.heightmapImage.data);
        int const idx = py * context.heightmapImage.width + px;
        return std::clamp(heightData[idx], 0.f, 1.0f);
    }

    // Otherwise, we assume it's in a color format and we read the red channel as height (with normalization from [0..255] to [0..1]).
    Color const c = GetImageColor(context.heightmapImage, px, py);
    return static_cast<float>(c.r) / 255.0f;
}

Color lerpColor(const Color &col1, const Color &col2, float ratio)
{

    Color result;

    result.r = static_cast<unsigned char>(
        col1.r + (col2.r - col1.r) * ratio);

    result.g = static_cast<unsigned char>(
        col1.g + (col2.g - col1.g) * ratio);

    result.b = static_cast<unsigned char>(
        col1.b + (col2.b - col1.b) * ratio);

    result.a = static_cast<unsigned char>(
        col1.a + (col2.a - col1.a) * ratio);

    return result;
}

float ratio(float inf, float sup, float num)
{
    return (num - inf) / (sup - inf);
}

void generateHeightmap(AppContext &context)
{

    if (context.texture.id > 0)
    {
        UnloadTexture(context.texture);
        context.texture = {};
    }

    if (context.image.data)
    {
        UnloadImage(context.image);
        context.image = {};
    }

    if (context.heightmapImage.data)
    {
        UnloadImage(context.heightmapImage);
        context.heightmapImage = {};
    }




    int const resolution = std::max(1, context.imageGenerationParameters.resolution);

    context.heightmapImage = GenImageFromNoiseFunction<float>(resolution, resolution, PIXELFORMAT_UNCOMPRESSED_R32,
                                                              [&](glm::vec2 const &p) -> float
                                                              {
                                                                  // TODO(student): implement stack based noise and island mask
                                                                  float octave_noise = octaveNoise(
                                                                      p,
                                                                      perlinNoise,
                                                                      context.imageGenerationParameters.octaves,
                                                                      context.imageGenerationParameters.lacunarity,
                                                                      context.imageGenerationParameters.gain,
                                                                      context.imageGenerationParameters.amplitude,
                                                                      context.imageGenerationParameters.frequency);

                                                                  return octave_noise * radialMask(p, context.maskGenerationParameters);
                                                                  //   perlinNoiseSeeded(p * context.imageGenerationParameters.noiseScale, context.imageGenerationParameters.noiseSeed) * 0.5f + 0.5f);
                                                              });

    // exemple conversion from heightmap to color image
    context.image = TransformImage<float, Color>(context.heightmapImage, [&](float const &v, int const, int const)
                                                 {


        context.colEau = color_from((context.vecColEau) * 255.f);
        context.colSable = color_from((context.vecColSable) * 255.f);
        context.colHerbe = color_from((context.vecColHerbe) * 255.f);
        context.colRoche = color_from((context.vecColRoche) * 255.f);
        context.colNeige = color_from((context.vecColNeige) * 255.f);

        context.env.biomeNormal = {
            context.colEau,
            context.colSable,
            context.colHerbe,
            context.colRoche,
            context.colNeige};


    const Environnement env = context.env;
    const PaletteduTerrain p = env.biomeNormal;

    const Color eau_profonde = color_from({ 10, 30, 120 });// eau profonde on garde la même partout

    if (v < 0.01){
        return eau_profonde;
    }
    if (v < 0.07f)
    {
        return lerpColor(
            eau_profonde, 
            p.eau,
            ratio(0.01f, 0.07f, v)
        );
    }

    else if (v < 0.15f)
    {
        return lerpColor(
            p.eau,
            p.sable,
            ratio(0.07f, 0.15f, v)
        );
    }

    else if (v < 0.45f)
    {
        return lerpColor(
            p.sable,
            p.herbe,
            ratio(0.15f, 0.45f, v)
        );
    }

    else if (v < 0.70f)
    {
        return lerpColor(
            p.herbe,
            p.roche,
            ratio(0.45f, 0.70f, v)
        );
    }

    else if (v <1.0f)
    {
        return lerpColor(
            p.roche,
            p.neige,
            ratio(0.70f, 1.0f, v)
        );
    }
    else {
        return p.neige;
    } }, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);

    context.texture = LoadTextureFromImage(context.image);
    if (context.model.meshCount > 0)
    {
        context.model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = context.texture;
    }
}