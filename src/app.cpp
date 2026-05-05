#include "app.hpp"
#include "raymath.h"
#include "utils/raylibUtils.hpp"

void unload(AppContext& context) {
    if (context.texture.id > 0) {
        UnloadTexture(context.texture);
        context.texture = {};
    }

    if (context.image.data) {
        UnloadImage(context.image);
        context.image = {};
    }

    if (context.heightmapImage.data) {
        UnloadImage(context.heightmapImage);
        context.heightmapImage = {};
    }

    if (context.model.meshCount > 0) {
        UnloadModel(context.model);
        context.model = {};
        context.mesh = {};
    }

    if (context.cubeMaterial.shader.id > 0) {
        UnloadMaterial(context.cubeMaterial);
        context.cubeMaterial = {};
    }

    if (context.cube.vertexCount > 0) {
        UnloadMesh(context.cube);
        context.cube = {};
    }
}

Matrix getTerrainCenteringMatrix(AppContext const& context)
{
    glm::vec3 const half_size { context.terrainSize * 0.5f };
    return MatrixTranslate(-half_size.x, 0.0f, -half_size.z);
}

void regenerateMeshFromImage(AppContext& context) {

    if(context.model.meshCount > 0) {
        UnloadModel(context.model);
        context.model = {};
        context.mesh = {};
    }

    // We need to convert the heightmap image to a format that GenMeshHeightmap can use (uncompressed R8G8B8A8), so we create a new image and convert the height values to grayscale colors.
    Image const meshHeightImage = TransformImage<float, Color>(
        context.heightmapImage,
        [](float const& h, int const, int const) {
            unsigned char const v { static_cast<unsigned char>(Clamp(h, 0.0f, 1.0f)*255.0f)};
            return Color{ v, v, v, 255 };
        },
        PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
    );

    context.mesh = GenMeshHeightmap(meshHeightImage, vec_from_glm(context.terrainSize));
    UnloadImage(meshHeightImage);
    context.model = LoadModelFromMesh(context.mesh);

    context.model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = context.texture;
}