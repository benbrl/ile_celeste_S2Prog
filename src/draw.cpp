#include "draw.hpp"

#include "app.hpp"

#include "generation.hpp"

#include "imgui.h"
#include "raylib.h"
#include "raymath.h"

void draw3DScene(AppContext &context)
{
    ClearBackground(RAYWHITE);

    BeginMode3D(context.camera);

    Matrix const terrainCentering{getTerrainCenteringMatrix(context)};
    Vector3 const terrainCenterOffset{terrainCentering.m12, terrainCentering.m13, terrainCentering.m14};

    DrawModel(context.model, terrainCenterOffset, 1.0f, WHITE);
    drawCubes(context, terrainCentering);
    DrawGrid(20, 1.0f);

    EndMode3D();
}

void drawCubes(AppContext const &context, Matrix const &terrainCentering)
{
    if (context.objectPositions.empty())
    {
        return;
    }

    float const cubeHalfHeight{0.5f * context.cubeScale};

    for (glm::vec3 const &pos : context.objectPositions)
    {
        Matrix const objectTranslation{MatrixTranslate(
            pos.x * context.terrainSize.x,
            pos.z * context.terrainSize.y + cubeHalfHeight,
            pos.y * context.terrainSize.z)};
        Matrix const centeredTranslation{MatrixMultiply(objectTranslation, terrainCentering)};
        Matrix const scale{MatrixScale(context.cubeScale, context.cubeScale, context.cubeScale)};
        Matrix const transform{MatrixMultiply(scale, centeredTranslation)};
        Vector3 treePosition = {pos.x * context.terrainSize.x - context.terrainSize.x * 0.5f, pos.z * context.terrainSize.y, pos.y * context.terrainSize.z - context.terrainSize.z * 0.5f};
        DrawModel(context.tree, treePosition, 0.2F, WHITE);
    }
}

void drawImGui(AppContext &context)
{

    if (ImGui::CollapsingHeader("Colors Palettes", ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::Button("Palette Tropicale"))
        {
            context.env.biomeHumide = EnvironnementTropical.biomeHumide;
            context.env.biomeSec = EnvironnementTropical.biomeSec;
            context.env.biomeNormal = EnvironnementTropical.biomeNormal;
            generateHeightmap(context);
        }
        if (ImGui::Button("Palette Desert"))
        {
            context.env.biomeHumide = EnvironnementDesertique.biomeHumide;
            context.env.biomeSec = EnvironnementDesertique.biomeSec;
            context.env.biomeNormal = EnvironnementDesertique.biomeNormal;
            generateHeightmap(context);
        }
        if (ImGui::Button("Palette Ile celeste"))
        {
            context.env.biomeHumide = EnvironnementCeleste.biomeHumide;
            context.env.biomeSec = EnvironnementCeleste.biomeSec;
            context.env.biomeNormal = EnvironnementCeleste.biomeNormal;
            generateHeightmap(context);
        }
    }

    if (ImGui::CollapsingHeader("Custom Color", ImGuiTreeNodeFlags_DefaultOpen))
    {

        ImGui::ColorEdit3("Eau", &context.vecColEau.x);
        ImGui::ColorEdit3("Sable", &context.vecColSable.x);
        ImGui::ColorEdit3("Herbe", &context.vecColHerbe.x);
        ImGui::ColorEdit3("Roche", &context.vecColRoche.x);
        ImGui::ColorEdit3("Neige", &context.vecColNeige.x);
    }

    if (ImGui::CollapsingHeader("objects", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::SliderFloat("Cube Scale", &context.cubeScale, 0.01f, 1.0f);
    }

    if (ImGui::CollapsingHeader("Noise", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::SliderInt("Octave", &context.imageGenerationParameters.octaves, 1, 8);
        ImGui::SliderFloat("Lacunarity", &context.imageGenerationParameters.lacunarity, 0.1f, 3.0f);
        ImGui::SliderFloat("Gain", &context.imageGenerationParameters.gain, 0.01f, 1.0f),
            ImGui::SliderFloat("Amplitude", &context.imageGenerationParameters.amplitude, 0.01f, 1.0f);
        ImGui::SliderFloat("Frequency", &context.imageGenerationParameters.frequency, 0.01f, 5.0f);
    }

    if (ImGui::CollapsingHeader("Points generation", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::SliderFloat("Poisson disk radius", &context.pointsGenerationParameters.radius, 0.01, 0.5);
        ImGui::SliderInt("Rejet", &context.pointsGenerationParameters.numSampleRejection, 1, 100);
        ImGui::SliderFloat("minimum height", &context.pointsGenerationParameters.min_height, 0.f, 1.f);
        ImGui::SliderFloat("maximum height", &context.pointsGenerationParameters.max_height, 0.f, 1.f);
    }

    if (ImGui::CollapsingHeader("Mask", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::SliderFloat("Mask Amplitude", &context.maskGenerationParameters.mask_amplitude, 0.5, 1);
        ImGui::SliderFloat("Mask Scale", &context.maskGenerationParameters.mask_scale, 1, 5);
    }

    if (ImGui::Button("Generate random positions"))
    {
        generateObjectsPositions(context);
    }

    if (ImGui::Button("Appliquer"))
    {

        generateHeightmap(context);
        regenerateMeshFromImage(context);
    }
}

void drawRaylibUI(AppContext &context)
{
    int screenWidth{GetScreenWidth()};

    float wanted_size{400.f};
    float scale_factor{wanted_size / std::max(context.texture.width, context.texture.height)};
    float const preview_x{screenWidth - wanted_size - 20.f};
    float const preview_y{20.f};
    float const preview_w{context.texture.width * scale_factor};
    float const preview_h{context.texture.height * scale_factor};
    // DrawTexture(context.texture, screenWidth - context.texture.width - 20, 20, WHITE);
    DrawTextureEx(context.texture, {preview_x, preview_y}, 0.0f, scale_factor, WHITE);
    DrawRectangleLines(screenWidth - wanted_size - 20, 20, wanted_size, wanted_size, GREEN);

    // draw positions on top of the heightmap
    for (auto const &pos : context.objectPositions)
    {
        // Remap normalized coordinates [0..1] to the preview image in screen space.
        float const px{preview_x + Clamp(pos.x, 0.0f, 1.0f) * preview_w};
        float const py{preview_y + Clamp(pos.y, 0.0f, 1.0f) * preview_h};

        DrawCircleV({px, py}, 2.0f, RED);
    }

    DrawFPS(10, 10);
}