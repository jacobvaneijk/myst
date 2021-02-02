/**
 * Copyright (c) 2021-2021 Jacob van Eijk. All rights reserved.
 *
 * For the full copyright and license information, please view the LICENSE file
 * that was distributed with this source code.
 */

#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <utility>

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace Myst
{
    class GLShader
    {
    public:
        GLShader(GLenum type);
        ~GLShader();

        GLenum GetType() const
        {
            return mType;
        }

        GLuint GetID() const
        {
            return mID;
        }

        bool Compile(const char* src);

    private:
        GLenum mType;
        GLuint mID;

        std::string mSource;
    };

    class GLShaderProgram
    {
    public:
        GLShaderProgram();
        ~GLShaderProgram();

        void AttachShader(const GLShader& shader);
        bool Link();
        void Bind();
        void Unbind();

        void SetBool(const std::string& name, bool value) const;
        void SetFloat(const std::string& name, float value) const;
        void SetInt(const std::string& name, int value) const;
        void SetMat4(const std::string& name, const glm::mat4& value) const;

    private:
        GLuint mID;
    };
}
