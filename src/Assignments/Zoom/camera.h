#pragma once
#include <cmath>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"



class Camera {
public:


    glm::vec3 x() const { return x_; }
    glm::vec3 y() const { return y_; }
    glm::vec3 z() const { return z_; }
    
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
    
    glm::vec3 position() const { return position_; }
    glm::vec3 center() const { return center_; }

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
