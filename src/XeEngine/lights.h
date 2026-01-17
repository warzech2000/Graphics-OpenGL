//
// Created by Piotr Białas on 13/12/2021.
//

#pragma once

#include <glm/glm.hpp>

namespace xe {
    struct PointLight {
        PointLight() = default;
        PointLight(const glm::vec3 &pos, const glm::vec3 &color, float intensity, float radius)
                : position_in_world_space(pos),
                  color(color), intensity(intensity), radius(radius) {}

        alignas(16) glm::vec3 position_in_world_space;
        alignas(16) glm::vec3 position_in_view_space;
        alignas(16) glm::vec3 color;
        float intensity;
        float radius;
    };
}