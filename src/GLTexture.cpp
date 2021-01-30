/**
 * Copyright (c) 2021-2021 Jacob van Eijk. All rights reserved.
 *
 * For the full copyright and license information, please view the LICENSE file
 * that was distributed with this source code.
 */

#include "GLTexture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Myst
{
    GLTexture::GLTexture(const std::string& filepath, GLenum target)
        : GLTexture(filepath, target, Parameters{})
    {
        // Nothing to do.
    }

    GLTexture::GLTexture(
        const std::string& filepath, GLenum target, Parameters params)
        : mTarget(target),
          mFilepath(filepath),
          mWidth(0),
          mHeight(0),
          mParams(params)
    {
        glGenTextures(1, &mID);
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &mMaxTextureImageUnits);
    }

    GLTexture::~GLTexture()
    {
        // Nothing to do.
    }

    bool GLTexture::Generate()
    {
        return Generate(0, 0);
    }

    bool GLTexture::Generate(GLint mipmap)
    {
        return Generate(mipmap, 0);
    }

    bool GLTexture::Generate(GLint mipmap, GLint depth)
    {
        // If we don't flip by the y-coordinate the image will be upside down.
        stbi_set_flip_vertically_on_load(true);

        int width, height, channels{0};
        unsigned char* data{
            stbi_load(mFilepath.c_str(), &width, &height, &channels, 0)};

        if (data == nullptr) {
            std::cerr << "stb: failed to load (" << mFilepath << ")"
                      << std::endl;
            return false;
        }

        mWidth = static_cast<unsigned int>(width);
        mHeight = static_cast<unsigned int>(height);

        GLenum format{DetermineFormat(channels)};

        Bind();

        glTexParameteri(mTarget, GL_TEXTURE_MIN_FILTER, mParams.FilterMin);
        glTexParameteri(mTarget, GL_TEXTURE_MAG_FILTER, mParams.FilterMax);
        glTexParameteri(mTarget, GL_TEXTURE_WRAP_S, mParams.WrapS);
        glTexParameteri(mTarget, GL_TEXTURE_WRAP_T, mParams.WrapT);
        glTexParameteri(mTarget, GL_TEXTURE_WRAP_R, mParams.WrapR);

        switch (mTarget) {
            case GL_TEXTURE_1D:
                glTexImage1D(
                    GL_TEXTURE_1D, mipmap, mParams.StorageFormat, mWidth, 0,
                    format, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_1D);
                break;

            case GL_TEXTURE_2D:
                glTexImage2D(
                    GL_TEXTURE_2D, mipmap, mParams.StorageFormat, mWidth,
                    mHeight, 0, format, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);
                break;

            case GL_TEXTURE_3D:
                glTexImage3D(
                    GL_TEXTURE_3D, mipmap, mParams.StorageFormat, mWidth,
                    mHeight, depth, 0, format, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_3D);
                break;

            default:
                std::cerr << "myst: unsupported texture type" << std::endl;
                Unbind();
                stbi_image_free(data);
                return false;
        }

        Unbind();
        stbi_image_free(data);

        return true;
    }

    void GLTexture::Bind()
    {
        Bind(0);
    }

    void GLTexture::Bind(GLint unit)
    {
        if (unit >= 0 && unit < mMaxTextureImageUnits) {
            glActiveTexture(GL_TEXTURE0 + unit);
        }

        glBindTexture(mTarget, mID);
    }

    void GLTexture::Unbind()
    {
        glBindTexture(mTarget, 0);
    }

    GLenum GLTexture::DetermineFormat(int channels)
    {
        switch (channels) {
            case 3: return GL_RGB;
            case 4: return GL_RGBA;
            default: break;
        }

        return mParams.DataFormat;
    }
}
