/**
 * Copyright (c) 2021-2021 Jacob van Eijk. All rights reserved.
 *
 * For the full copyright and license information, please view the LICENSE file
 * that was distributed with this source code.
 */

#include "GLShader.hpp"

namespace Myst
{
    GLShader::GLShader(GLenum type) : mType(type)
    {
        mID = glCreateShader(type);
    }

    GLShader::~GLShader()
    {
        glDeleteShader(mID);
    }

    bool GLShader::Compile(const char* src)
    {
        GLint success{0};

        glShaderSource(mID, 1, &src, NULL);
        glCompileShader(mID);
        glGetShaderiv(mID, GL_COMPILE_STATUS, &success);

        if (success == GL_FALSE) {
            char info[1024];
            glGetShaderInfoLog(mID, sizeof(info), NULL, info);
            glDeleteShader(mID);
            std::cerr << "gl: shader compilation failed: " << info << std::endl;
            return false;
        }

        return true;
    }

    GLShaderProgram::GLShaderProgram()
    {
        mID = glCreateProgram();
    }

    GLShaderProgram::~GLShaderProgram()
    {
        // Nothing to do.
    }

    void GLShaderProgram::AttachShader(const GLShader& shader)
    {
        glAttachShader(mID, shader.GetID());
    }

    bool GLShaderProgram::Link()
    {
        GLint success{0};

        glLinkProgram(mID);
        glGetProgramiv(mID, GL_LINK_STATUS, &success);

        if (success == GL_FALSE) {
            char info[1024];
            glGetProgramInfoLog(mID, sizeof(info), NULL, info);
            std::cerr << "gl: failed to link program: " << info << std::endl;
            return false;
        }

        return true;
    }

    void GLShaderProgram::Bind()
    {
        glUseProgram(mID);
    }

    void GLShaderProgram::Unbind()
    {
        glUseProgram(0);
    }

    void GLShaderProgram::SetBool(const std::string& name, bool value) const
    {
        glUniform1i(glGetUniformLocation(mID, name.c_str()), (int)value);
    }

    void GLShaderProgram::SetFloat(const std::string& name, float value) const
    {
        glUniform1f(glGetUniformLocation(mID, name.c_str()), value);
    }

    void GLShaderProgram::SetInt(const std::string& name, int value) const
    {
        glUniform1i(glGetUniformLocation(mID, name.c_str()), value);
    }
}
