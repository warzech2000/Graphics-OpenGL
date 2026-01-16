#include "app.h"

#include <iostream>
#include <vector>
#include <tuple>


#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define STB_IMAGE_IMPLEMENTATION 1
#include "3rdParty/stb/stb_image.h"
#include "spdlog/spdlog.h"
#include "XeEngine/mesh_loader.h"


void SimpleShapeApplication::init() {
    std::cout << "SimpleShapeApplication::init() started" << std::endl;
    
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

    // Link Transformations uniform block to binding = 1 for ColorMaterial shader
    GLuint color_program = xe::ColorMaterial::program();
    GLuint tindex_color = glGetUniformBlockIndex(color_program, "Transformations");
    if (tindex_color != GL_INVALID_INDEX) {
        glUniformBlockBinding(color_program, tindex_color, 1);
    }
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // UBO
    glGenBuffers(1, &u_pvm_buffer_);
    glBindBuffer(GL_UNIFORM_BUFFER, u_pvm_buffer_);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, u_pvm_buffer_);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Link shader block to binding = 1
    GLuint tindex = glGetUniformBlockIndex(program, "Transformations");
    glUniformBlockBinding(program, tindex, 1);

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

    // Load pyramid from OBJ file
    auto pyramid_mesh = xe::load_mesh_from_obj(std::string(ROOT_DIR) + "/Models/pyramid.obj",
                                                std::string(ROOT_DIR) + "/Models");
    if (pyramid_mesh) {
        add_submesh(pyramid_mesh);
    }

    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);

    glViewport(0, 0, w, h);

}

void SimpleShapeApplication::frame() {
    
    auto PVM = camera()->projection() * camera()->view();
    glBindBuffer(GL_UNIFORM_BUFFER, u_pvm_buffer_);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &PVM[0]);
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
