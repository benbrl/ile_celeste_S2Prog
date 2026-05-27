#pragma once

#include "app.hpp"

std::vector<glm::vec2> generate2DPositions(PointsGenerationParameters const &params);

void generateObjectsPositions(AppContext &context);

float sampleHeightmap(AppContext const &context, float u, float v);

void generateHeightmap(AppContext &context);

bool isValid(glm::vec2 candidate, glm::vec2 sampleRegionSize, float cellSize, int radius, std::vector<glm::vec2> &points, std::vector<std::vector<int>> &grid);

float radialMask(glm::vec2 const &p);