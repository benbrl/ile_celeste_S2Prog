#pragma once

#include <functional>

#include "glm/ext/vector_float3.hpp"
#include "glm/ext/vector_uint3_sized.hpp"
#include <raylib.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

inline glm::vec2 glm_vec_from(Vector2 const& v) { return { v.x, v.y }; }
inline glm::vec3 glm_vec_from(Vector3 const& v) { return { v.x, v.y, v.z }; }

inline Vector3 vec_from_glm(glm::vec3 const& v) { return { v.x, v.y, v.z }; }

inline Color color_from(glm::u8vec3 const& v, unsigned char a = 255) { return { v.x, v.y, v.z, a }; }

// Generates an image by applying a noise function to each pixel. The noise function takes a 2D coordinate (normalized to the range [0, 1]) and returns a noise value of type NoiseResultType. The output image will have the specified width, height, and pixel format. The user is responsible for freeing the memory of the output image data.
template<typename NoiseResultType>
Image GenImageFromNoiseFunction(int width, int height, PixelFormat format, std::function<NoiseResultType(glm::vec2)> noiseFunction) {
    NoiseResultType * noiseValues = (NoiseResultType *)RL_MALLOC(width*height*sizeof(NoiseResultType));

    float const widthf = static_cast<float>(width);
    float const heightf = static_cast<float>(height);
    float aspectRatio = widthf / heightf;
    for (int y = 0.0f; y < heightf; y++)
    {
        for (int x = 0.0f; x < widthf; x++)
        {
            float nx = static_cast<float>(x)/widthf;
            float ny = static_cast<float>(y)/heightf;

            // Apply aspect ratio compensation to wider side
            if (width > height) nx *= aspectRatio;
            else ny /= aspectRatio;

            noiseValues[y*width + x] = noiseFunction({nx, ny});
        }
    }

    Image image = {
        .data = noiseValues,
        .width = width,
        .height = height,
        .mipmaps = 1,
        .format = format,
    };

    return image;
}

// Transforms an image by applying a function to each pixel. This function creates a new image with the same dimensions as the input image, user is responsible for freeing the memory of the output image data. The transform function takes the input pixel value and its coordinates as arguments and returns the transformed pixel value.
template <typename InputType, typename OutputType>
inline Image TransformImage(Image const& image, std::function<OutputType(InputType const&, int const x, int const y)> transformFunction, PixelFormat outputFormat) {
    OutputType* outputData = static_cast<OutputType*>(RL_MALLOC(image.width * image.height *sizeof(OutputType)));

    InputType const* InputData = static_cast<InputType*>(image.data);

    for (int y = 0; y < image.height; ++y)
    {
        for (int x = 0; x < image.width; ++x)
        {
            outputData[y*image.width + x] = transformFunction(InputData[y*image.width + x], x, y);
        }
    }

    return {
        .data = outputData,
        .width = image.width,
        .height = image.height,
        .mipmaps = 1,
        .format = outputFormat
    };
}