
#include "noise.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>

#include <cstdint>
#include <functional>
#include <numbers>

namespace
{

    // Fast integer hash function
    std::uint32_t hashU32(std::uint32_t v)
    {
        v ^= v >> 16;
        v *= 0x7feb352du;
        v ^= v >> 15;
        v *= 0x846ca68bu;
        v ^= v >> 16;
        return v;
    }

    glm::vec2 seedToOffset2D(int seed)
    {
        std::uint32_t const base{static_cast<std::uint32_t>(seed)};
        std::uint32_t const hx{hashU32(base ^ 0x9e3779b9u)};
        std::uint32_t const hy{hashU32(base ^ 0x85ebca6bu)};

        float const fx{static_cast<float>(hx & 0x00ffffffu) / 16777216.0f};
        float const fy{static_cast<float>(hy & 0x00ffffffu) / 16777216.0f};

        // Large translation range so seeds land on very different 2D Perlin regions.
        return {
            fx * 4096.0f - 2048.0f,
            fy * 4096.0f - 2048.0f};
    }

} // namespace

float perlinNoise(glm::vec2 const &position)
{
    return glm::perlin(position);
}

float perlinNoiseSeeded(glm::vec2 const &position, int seed)
{
    // Cache computed offset because the same seed is used for many samples per frame.
    static int cachedSeed{};
    static glm::vec2 cachedOffset{};

    if (seed != cachedSeed)
    {
        cachedSeed = seed;
        cachedOffset = seedToOffset2D(seed);
    }

    return glm::perlin(position + cachedOffset);
}

float radialMask(float x, float y)
{

    // center => comme c'est de 0 à 1 on initialise a 0,5

    const float center = 0.5f;
    const float valeur = 0.4f;
    // et maintenant il faut mettre x et y dans l'equation

    float gaussian_function = (1.0f / (valeur * std::sqrt(2.0f * std::numbers::pi))) * std::exp(-((x - center) * (y - center)) / (2.0f * valeur * valeur));

    return gaussian_function;

    // normalisation de la fonction
};

float octaveNoise(glm::vec2 const &position, std::function<float(glm::vec2 const &)> noiseFunction, int octaves, float lacunarity, float gain, float amplitude, float frequency)
{
    float y{0};
    for (int i = 0; i < octaves; i++)
    {
        y += amplitude * noiseFunction(frequency * position);
        frequency *= lacunarity;
        amplitude *= gain;

        /// amplitude
    }

    return y;
}
