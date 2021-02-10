/**
 * Copyright (c) 2021-2021 Jacob van Eijk. All rights reserved.
 *
 * For the full copyright and license information, please view the LICENSE file
 * that was distributed with this source code.
 */

#pragma once

#include <iostream>
#include <string>
#include <utility>

#include <glad/glad.h>

namespace Myst
{
    class GLTexture
    {
    public:
        struct Parameters
        {
            GLenum DataFormat{GL_RED};
            GLenum StorageFormat{GL_RGB};
            GLenum FilterMin{GL_LINEAR_MIPMAP_LINEAR};
            GLenum FilterMax{GL_LINEAR};
            GLenum WrapS{GL_REPEAT};
            GLenum WrapT{GL_REPEAT};
            GLenum WrapR{GL_REPEAT};
        };

        GLTexture(const std::string& filepath, GLenum target);
        GLTexture(
            const std::string& filepath, GLenum target, Parameters params);
        ~GLTexture();

        GLuint GetID() const
        {
            return mID;
        }

        GLenum GetTarget() const
        {
            return mTarget;
        }

        unsigned int GetWidth() const
        {
            return mWidth;
        }

        unsigned int GetHeight() const
        {
            return mHeight;
        }

        bool Generate();
        bool Generate(GLint mipmap);
        bool Generate(GLint mipmap, GLint depth);

        void Bind();
        void Bind(GLint unit);
        void Unbind();

    private:
        GLenum DetermineFormat(int channels);

    private:
        GLuint mID;
        GLenum mTarget;
        GLint mMaxTextureImageUnits;

        std::string mFilepath;
        unsigned int mWidth;
        unsigned int mHeight;

        Parameters mParams;
    };
}
