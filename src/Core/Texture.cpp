//
// Created by anselme on 09/03/2026.
//

#include <iostream>
#include <glad/gl.h>
#include <stb_image.h>

#include "Library.h"
#include "Texture.h"


unsigned int getImageFormat(const unsigned int channelsPerPixel) {
    switch (channelsPerPixel)
    {
        case 1: return GL_RED;
        case 3: return GL_RGB;
        case 4: return GL_RGBA;
        default:
            std::cerr << "Unsupported number of channels per pixel: " << channelsPerPixel << std::endl;
            return 0; // Return 0 to indicate an error
    }
}

Texture::Texture(const std::string& path) : FilePath_(path)
{
    // Usually Images start from (0,0) in the top-left corner,
    // but OpenGL starts from (0,0) in the bottom-left corner,
    // so we need to flip the image vertically when loading it.
    stbi_set_flip_vertically_on_load(1);

    // Construct the full file path by concatenating the project root and the relative file path
    const auto fullFilePage = std::string(PROJECT_ROOT) + "/" + path;

    #ifdef ENABLE_DEBUG_LOG
        std::printf("[DEBUG] Loading texture from file %s\n", fullFilePage.c_str());
    #endif

    // Channels are the layers of the image in terms of coloring,
    // Each pixel of a digital image is made of pixels,
    //      And to determine the pixel of a color we look on different layers.
    //      1 - (Grayscale) - Only one layer, the intensity of the color (0-255)
    //      3 - (RGB) - Three layers, Red, Green and Blue (0-255 each)
    //      4 - (RGBA) - Four layers, Red, Green, Blue and the Alpha (transparency) (0-255 each)
    // Note: It is sometimes called bytes per pixel. if it is 1 then it is (0-255) grayscale, ...
    // ref: https://en.wikipedia.org/wiki/Channel_(digital_image)
    int channelsInFile;
    //  // We want to load the image with 4 channels (RGBA)
    //  regardless of the original format, so we can use it in OpenGL without worrying about the format.
    constexpr int DESIRED_CHANNELS = 4;

    const auto fileBuffer = stbi_load(fullFilePage.c_str(), &Width_, &Height_, &channelsInFile, DESIRED_CHANNELS);

    #ifdef ENABLE_DEBUG_LOG
        std::printf("[DEBUG] Loaded the file with width=%d, height=%d and channels in file=%d \n", Width_, Height_, channelsInFile);
    #endif

    if (!fileBuffer)
    {
        std::cerr << "[ERROR] Failed to load texture: " << fullFilePage << std::endl;
        return;
    }

    // Create the Object (TextureObject) and set the ObjectID
    gl_try(glGenTextures(1, &ObjectId_));

    // Mark it as active, the one to operate on.
    gl_try(glBindTexture(GL_TEXTURE_2D, ObjectId_));

    // Texture filtering:-
    // This becomes important if you have a large object with low resolution texture.
    // Like when magnifying or mining the texture.
    // Different options:
    //   1. GL_NEAREST (also known as nearest neighbor or point filtering) is the default texture filtering method of OpenGL.
    //                  When set to GL_NEAREST, OpenGL selects the texel that center is closest to the texture coordinate.
    //   2. GL_LINEAR (also known as bilinear filtering) is a texture filtering method that provides smoother results than GL_NEAREST.
    gl_try(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    gl_try(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));

    // S-> The x-axis of the texture
    // T-> The y-axis of the texture
    // We are telling OpenGL how to handle if we provide a smaller values than 1 or bigger values than 1 for the texture coordinates.
    // This is the case when we provide coordinates that are not between (0,0) and (1,1) so we need to tell OpenGL how to handle this case.
    // Other possible options are:-
    //      GL_REPEAT: The default behavior for textures. Repeats the texture image.
    //      GL_MIRRORED_REPEAT: Same as GL_REPEAT but mirrors the image with each repeat.
    //      GL_CLAMP_TO_EDGE: Clamps the coordinates between 0 and 1.
    //                        The result is that higher coordinates become clamped to the edge, resulting in a stretched edge pattern.
    //      GL_CLAMP_TO_BORDER: Coordinates outside the range are now given a user-specified border color.
    gl_try(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    gl_try(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    // Since we forced stbi_load to load 4 channels (RGBA), we need to tell OpenGL the data is in RGBA format
    gl_try(glTexImage2D(
        // The first argument specifies the texture target;
        // setting this to GL_TEXTURE_2D means this operation will generate a texture on the currently bound texture object
        // at the same target (so any textures bound to targets GL_TEXTURE_1D or GL_TEXTURE_3D will not be affected).
        GL_TEXTURE_2D,

        // The second argument specifies the mipmap level for which we want to create a texture for if you want to set
        // each mipmap level manually, but we'll leave it at the base level which is 0.
        // mipmap is a concept in which in 3D graphics, less resolution textures are used to render far away objects to improve performance and reduce aliasing.
        0,

        // The third argument tells OpenGL in what kind of format we want to store the texture.
        // Since we forced 4 channels when loading, we use RGBA.
        getImageFormat(DESIRED_CHANNELS),
        Width_,
        Height_,

        // The next argument should always be 0 (some legacy stuff).
        0,

        // The 7th and 8th argument specify the format and datatype of the source image.
        // We loaded the image with 4 channels (RGBA) forced, so we use GL_RGBA regardless of the original format.
        getImageFormat(DESIRED_CHANNELS),
        GL_UNSIGNED_BYTE,

        // The last argument is the actual image data.
        fileBuffer));

    gl_try(glBindTexture(GL_TEXTURE_2D, 0));

    // Generate the mipmap for this image (we can generate it manually by providing the mipmap levels, but we will let OpenGL do it for us).
    gl_try(glGenerateMipmap(GL_TEXTURE_2D));

    constexpr float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
    gl_try(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor));

    stbi_image_free(fileBuffer);
}

Texture::~Texture()
{
    // Delete the texture object by the ObjectID
    // This is the destruction of the image
    gl_try(glDeleteTextures(1, &ObjectId_));
}

void Texture::Bind(const unsigned int slot) const
{
    // Before binding, we need to activate the texture slot we want to bind to.
    gl_try(glActiveTexture(GL_TEXTURE0 + slot));
    gl_try(glBindTexture(GL_TEXTURE_2D, ObjectId_));
}

void Texture::Unbind() const
{
    gl_try(glBindTexture(GL_TEXTURE_2D, 0));
}
