/**
 * Copyright (c) 2021-2021 Jacob van Eijk. All rights reserved.
 *
 * For the full copyright and license information, please view the LICENSE file
 * that was distributed with this source code.
 */

#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Myst
{
    class Camera
    {
    public:
        Camera(glm::vec3 position);
        ~Camera();

        float GetZoom() const;
        glm::mat4 GetViewMatrix() const;

        void OnKeyPress(int key, float deltaTime);
        void OnMouseMove(float xDelta, float yDelta, bool clamp = true);
        void OnMouseScroll(float yOffset, bool clamp = true);

    private:
        void Update();

    private:
        glm::vec3 mPosition;
        glm::vec3 mUp;
        glm::vec3 mFront;
        glm::vec3 mRight;

        glm::vec3 mWorldUp;

        float mYaw;
        float mPitch;
        float mZoom;

        float mMouseSensivity;
        float mMovementSpeed;
    };
}
