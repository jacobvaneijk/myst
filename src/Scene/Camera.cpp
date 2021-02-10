/**
 * Copyright (c) 2021-2021 Jacob van Eijk. All rights reserved.
 *
 * For the full copyright and license information, please view the LICENSE file
 * that was distributed with this source code.
 */

#include "Scene/Camera.hpp"

namespace Myst
{
    Camera::Camera(glm::vec3 position)
        : mPosition(position)
        , mUp(glm::vec3(0.0f))
        , mFront(glm::vec3(0.0f, 0.0f, -1.0f))
        , mRight(glm::vec3(0.0f))
        , mWorldUp(glm::vec3(0.0f, 1.0f, 0.0f))
        , mYaw(-90.0f)
        , mPitch(0.0f)
        , mZoom(45.0f)
        , mMouseSensivity(0.1f)
        , mMovementSpeed(2.5f)
    {
        Update();
    }

    Camera::~Camera()
    {
        // Nothing to do.
    }

    float Camera::GetZoom() const
    {
        return mZoom;
    }

    glm::vec3 Camera::GetPosition() const
    {
        return mPosition;
    }

    glm::mat4 Camera::GetViewMatrix() const
    {
        return glm::lookAt(mPosition, mPosition + mFront, mUp);
    }

    void Camera::OnKeyPress(int key, float deltaTime)
    {
        float velocity = 2.5f * deltaTime;

        switch (key) {
            case GLFW_KEY_W: mPosition += mFront * velocity; break;
            case GLFW_KEY_A: mPosition -= mRight * velocity; break;
            case GLFW_KEY_S: mPosition -= mFront * velocity; break;
            case GLFW_KEY_D: mPosition += mRight * velocity; break;
            default: break;
        }
    }

    void Camera::OnMouseMove(float xDelta, float yDelta, bool clamp)
    {
        xDelta *= mMouseSensivity;
        yDelta *= mMouseSensivity;

        mYaw += xDelta;
        mPitch += yDelta;

        if (clamp) {
            mPitch = glm::clamp(mPitch, -89.0f, 89.0f);
        }

        Update();
    }

    void Camera::OnMouseScroll(float yOffset, bool clamp)
    {
        mZoom -= yOffset;

        if (clamp) {
            mZoom = glm::clamp(mZoom, 1.0f, 45.0f);
        }
    }

    void Camera::Update()
    {
        float x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
        float y = sin(glm::radians(mPitch));
        float z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));

        mFront = glm::normalize(glm::vec3(x, y, z));
        mRight = glm::normalize(glm::cross(mFront, mWorldUp));
        mUp = glm::normalize(glm::cross(mRight, mFront));
    }
}
