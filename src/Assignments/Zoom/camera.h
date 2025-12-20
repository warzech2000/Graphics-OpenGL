#pragma once
#include <cmath>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"



class Camera {
public:


    glm::vec3 x() const { return x_; }
    glm::vec3 y() const { return y_; }
    glm::vec3 z() const { return z_; }
    glm::vec3 position() const { return position_; }
    glm::vec3 center() const { return center_; }

    
    void look_at(const glm::vec3 &eye, const glm::vec3 &center, const glm::vec3 &up) {
        V_ = glm::lookAt(eye, center, up);
    }

    void perspective(float fov, float aspect, float near, float far) {
        fov_ = fov;
        aspect_ = aspect;
        near_ = near;
        far_ = far;
    }

    void set_aspect(float aspect) {
        aspect_ = aspect;
    }

    glm::mat4 view() const { return V_; }

    void zoom(float y_offset) {
        auto x = fov_/glm::pi<float>();
        auto y = inverse_logistic(x);
        y += y_offset;
        x = logistic(y);

        fov_ = x*glm::pi<float>();
    }

    glm::mat4 projection() const { return glm::perspective(fov_, aspect_, near_, far_); }

    void rotate_around_point(float angle, const glm::vec3 &axis, const glm::vec3 &c) {
        auto R = rotation(angle, axis);
        x_ = R * x_;
        y_ = R * y_;
        z_ = R * z_;

        auto t = position_ - c;
        t = R * t;
        position_ = c + t;
    }

    glm::mat3 rotation(float angle, const glm::vec3 &axis) {
            auto u = glm::normalize(axis);
            auto s = std::sin(angle);
            auto c = std::cos(angle);

            return glm::mat3(
                c + u.x * u.x * (1.0f - c),
                u.y * u.x * (1.0f - c) + u.z * s,
                u.z * u.x * (1.0f - c) - u.y * s,

                u.x *u.y*(1.0f-c)-u.z *s,
                c + u.y*u.y *(1.0f-c),
                u.z*u.y*(1.0f-c)+u.x*s,

                u.x*u.z*(1.0f -c)+ u.y*s,
                u.y*u.z*(1.0f-c)-u.x*s,
                c+u.z*u.z*(1.0f -c)
            );
        }

    void rotate_around_center(float angle, const glm::vec3 &axis) {
            rotate_around_point(angle, axis, center_); 
        }
    
    private:
        float fov_;
        float aspect_;
        float near_;
        float far_;

        glm::vec3 position_;
        glm::vec3 center_;
        glm::vec3 x_, y_, z_;

        float logistic(float y) {
            return 1.0f/(1.0f+std::exp(-y));
        }

        float inverse_logistic(float x) {
            return std::log(x/(1.0f-x)); 
        }

    glm::mat4 V_;
};
