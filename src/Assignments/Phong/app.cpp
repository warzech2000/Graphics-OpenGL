#include "app.h"

#include <iostream>
#include <vector>
#include <tuple>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION 1
#include "3rdParty/stb/stb_image.h"
#include "XeEngine/mesh_loader.h"
#include "XeEngine/PhongMaterial.h"

void SimpleShapeApplication::init() {
    auto vs_path = std::string(PROJECT_DIR) + "/shaders/base_vs.glsl";
    auto fs_path = std::string(PROJECT_DIR) + "/shaders/base_fs.glsl";

    auto program = xe::utils::create_program(
            {{GL_VERTEX_SHADER,   vs_path},
             {GL_FRAGMENT_SHADER, fs_path}});

    if (!program) {
        std::cerr << "ERROR: Failed to create shader program!" << std::endl;
        throw std::runtime_error("Failed to create shader program");
    }

    xe::ColorMaterial::init();
    xe::ColorMaterial::set_shader(program);
    xe::ColorMaterial::init_uniform_locations();
    xe::PhongMaterial::init();

    GLuint color_program = xe::ColorMaterial::program();
    GLuint tindex_color = glGetUniformBlockIndex(color_program, "Transformations");
    if (tindex_color != GL_INVALID_INDEX) {
        glUniformBlockBinding(color_program, tindex_color, 1);
    }
    
    glEnable(GL_DEPTH_TEST);

    glGenBuffers(1, &u_pvm_buffer_);
    glBindBuffer(GL_UNIFORM_BUFFER, u_pvm_buffer_);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) + sizeof(glm::mat4) + 3 * sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, u_pvm_buffer_);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    GLuint tindex = glGetUniformBlockIndex(program, "Transformations");
    glUniformBlockBinding(program, tindex, 1);

    const int MAX_POINT_LIGHTS = 24;
    size_t lights_buffer_size = 16 + 16 + MAX_POINT_LIGHTS * 48;
    glGenBuffers(1, &u_lights_buffer_);
    glBindBuffer(GL_UNIFORM_BUFFER, u_lights_buffer_);
    glBufferData(GL_UNIFORM_BUFFER, lights_buffer_size, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, u_lights_buffer_);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    GLuint phong_program = xe::PhongMaterial::program();
    if (phong_program > 0) {
        GLuint tindex_phong = glGetUniformBlockIndex(phong_program, "Transformations");
        if (tindex_phong != GL_INVALID_INDEX) {
            glUniformBlockBinding(phong_program, tindex_phong, 1);
        }
        GLuint lights_index = glGetUniformBlockIndex(phong_program, "Lights");
        if (lights_index != GL_INVALID_INDEX) {
            glUniformBlockBinding(phong_program, lights_index, 2);
        }
    }

    #pragma region Camera

    set_camera(new Camera);

    camera()->look_at(
        glm::vec3(0, 1, 1),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 1, 0)
    );

    int w, h;
    std::tie(w, h) = frame_buffer_size();
    camera()->perspective(glm::pi<float>()/2.0, (float)w/h, 0.1f, 100.f);

    set_controler(new CameraControler(camera()));
#pragma endregion

    add_ambient(glm::vec3(0.1f, 0.1f, 0.1f));
    add_light(xe::PointLight(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.5f, 0.0f), 2.0f, 250.0f));
    
    auto square_mesh = xe::load_mesh_from_obj(std::string(ROOT_DIR) + "/Models/square.obj",
                                                std::string(ROOT_DIR) + "/Models");
    if (square_mesh) {
        add_submesh(square_mesh);
    }

    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);
    glViewport(0, 0, w, h);
}

void SimpleShapeApplication::frame() {
    auto PVM = camera()->projection() * camera()->view();
    auto VM = camera()->view();
    
    auto R = glm::mat3(VM);
    auto N = glm::mat3(glm::cross(R[1], R[2]), glm::cross(R[2], R[0]), glm::cross(R[0], R[1]));
    
    glBindBuffer(GL_UNIFORM_BUFFER, u_pvm_buffer_);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &PVM[0]);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), &VM[0]);
    size_t offset = 2 * sizeof(glm::mat4);
    for (int k = 0; k < 3; k++) {
        glm::vec4 col(N[k].x, N[k].y, N[k].z, 0.0f);
        glBufferSubData(GL_UNIFORM_BUFFER, offset + k * sizeof(glm::vec4), sizeof(glm::vec4), &col[0]);
    }
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
    glBindBuffer(GL_UNIFORM_BUFFER, u_lights_buffer_);
    size_t lights_offset = 0;
    
    glBufferSubData(GL_UNIFORM_BUFFER, lights_offset, sizeof(glm::vec3), &ambient_[0]);
    lights_offset += 12;
    
    uint32_t n_lights = static_cast<uint32_t>(p_lights_.size());
    glBufferSubData(GL_UNIFORM_BUFFER, lights_offset, sizeof(uint32_t), &n_lights);
    lights_offset += 4;
    
    static float time = 0.0f;
    time += 0.016f;
    
    if (!p_lights_.empty()) {
        float radius = 2.0f;
        p_lights_[0].position_in_world_space = glm::vec3(
            radius * glm::cos(time),
            radius * glm::sin(time),
            1.0f
        );
        
        float color_time = time * 0.5f;
        p_lights_[0].color = glm::vec3(
            0.5f + 0.5f * glm::cos(color_time),
            0.5f + 0.5f * glm::cos(color_time + 2.094f),
            0.5f + 0.5f * glm::cos(color_time + 4.189f)
        );
    }
    
    size_t base_light_offset = 16;
    
    for (size_t i = 0; i < p_lights_.size() && i < 24; i++) {
        glm::vec4 pos_ws = glm::vec4(p_lights_[i].position_in_world_space, 1.0f);
        glm::vec4 pos_vs = VM * pos_ws;
        glm::vec3 position_in_vs = glm::vec3(pos_vs.x / pos_vs.w, pos_vs.y / pos_vs.w, pos_vs.z / pos_vs.w);
        
        size_t light_offset = base_light_offset + i * 48;
        
        glm::vec4 pos_padded(position_in_vs, 0.0f);
        glBufferSubData(GL_UNIFORM_BUFFER, light_offset, sizeof(glm::vec4), &pos_padded[0]);
        glBufferSubData(GL_UNIFORM_BUFFER, light_offset + 16, sizeof(glm::vec3), &p_lights_[i].color[0]);
        glBufferSubData(GL_UNIFORM_BUFFER, light_offset + 28, sizeof(float), &p_lights_[i].intensity);
        glBufferSubData(GL_UNIFORM_BUFFER, light_offset + 32, sizeof(float), &p_lights_[i].radius);
    }
    
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    for (auto m: meshes_)
        m->draw();
}

void SimpleShapeApplication::framebuffer_resize_callback(int w, int h) {
    Application::framebuffer_resize_callback(w, h);
    glViewport(0,0,w,h);
    camera()->set_aspect((float) w / h);
}

void SimpleShapeApplication::UpdateCameraData() {
    int w,h;
    std::tie(w,h) = frame_buffer_size();
}

void SimpleShapeApplication::mouse_button_callback(int button, int action, int mods) {
    Application::mouse_button_callback(button, action, mods);

    if (controler_) {
        double x, y;
        glfwGetCursorPos(window_, &x, &y);

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
            controler_->LMB_pressed(x, y);

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
            controler_->LMB_released(x, y);
    }
}

void SimpleShapeApplication::cursor_position_callback(double x, double y) {
    Application::cursor_position_callback(x, y);
    if (controler_) {
        controler_->mouse_moved(x, y);
    }
}
