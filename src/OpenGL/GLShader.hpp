/**
 * Copyright (c) 2021-2021 Jacob van Eijk. All rights reserved.
 *
 * For the full copyright and license information, please view the LICENSE file
 * that was distributed with this source code.
 */

#pragma once

#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace Myst
{
    class GLShader
    {
    public:
        GLShader(const std::string& filepath, GLenum type);
        ~GLShader();

        GLenum GetType() const
        {
            return mType;
        }

        GLuint GetID() const
        {
            return mID;
        }

        bool Compile();

    private:
        bool ReadFile();

    private:
        GLenum mType;
        GLuint mID;

        std::string mFilepath;
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
        void SetMat3(const std::string& name, const glm::mat3& value) const;
        void SetMat4(const std::string& name, const glm::mat4& value) const;
        void SetVec3(const std::string& name, const glm::vec3& value) const;

    private:
        GLuint mID;
    };
}
