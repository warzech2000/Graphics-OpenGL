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

    std::vector<GLushort> indices = {
        // Base
        0, 1, 2,
        0, 2, 3,

        // Side
        4, 5, 6,
        7, 8, 9,
        10,11,12,
        13,14,15
    };

    std::vector<GLfloat> vertices = {
        0.5f, -0.5f,  0.5f,  0.5f, 0.8090f,
       -0.5f, -0.5f,  0.5f,  0.1910f, 0.5f,
       -0.5f, -0.5f, -0.5f,  0.5f, 0.1910f,
        0.5f, -0.5f, -0.5f,  0.8090f, 0.5f,
   
        0.5f, -0.5f,  0.5f,  0.5f, 0.8090f,
        0.0f,  0.5f,  0.0f,  0.0f, 1.0f,
       -0.5f, -0.5f,  0.5f,  0.1910f, 0.5f,
   
       -0.5f, -0.5f,  0.5f,  0.5f, 0.8090f,
        0.0f,  0.5f,  0.0f,  1.0f, 1.0f,
       -0.5f, -0.5f, -0.5f,  0.8090f, 0.5f,
   
       -0.5f, -0.5f, -0.5f,  0.8090f, 0.5f,
        0.0f,  0.5f,  0.0f,  1.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.5f, 0.1910f,
   
        0.5f, -0.5f, -0.5f,  0.5f, 0.1910f,
        0.0f,  0.5f,  0.0f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.1910f, 0.5f,
   };

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

    auto pyramid = new xe::Mesh;

    size_t indices_sizeof = sizeof(indices[0]) * indices.size();
    pyramid->allocate_index_buffer(indices_sizeof, GL_STATIC_DRAW);
    pyramid->load_indices(0, indices_sizeof, indices.data());

    size_t vertices_sizeof = sizeof(vertices[0]) * vertices.size();
    pyramid->allocate_vertex_buffer(vertices_sizeof, GL_STATIC_DRAW);
    pyramid->load_vertices(0, vertices_sizeof, vertices.data());

    pyramid->vertex_attrib_pointer(0, 3, GL_FLOAT, 5 * sizeof(vertices[0]), 0);
    pyramid->vertex_attrib_pointer(1, 2, GL_FLOAT, 5 * sizeof(vertices[0]), 3 * sizeof(vertices[0]));

    // Load texture
    stbi_set_flip_vertically_on_load(true);
    GLint width, height, channels;
    auto texture_file = std::string(ROOT_DIR) + "/Models/multicolor.png";
    auto img = stbi_load(texture_file.c_str(), &width, &height, &channels, 0);
    if (!img) {
        std::cerr << "WARNING: Could not read image from file: " << texture_file << std::endl;
        spdlog::warn("Could not read image from file `{}'", texture_file);
    } else {
        std::cout << "Texture loaded: " << width << "x" << height << ", channels: " << channels << std::endl;
    }

    GLuint texture = 0;
    if (img) {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        
        GLenum format;
        if (channels == 3)
            format = GL_RGB;
        else if (channels == 4)
            format = GL_RGBA;
        else
            format = GL_RGB;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, img);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        stbi_image_free(img);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    pyramid->add_submesh(0, 6, new xe::ColorMaterial(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), texture, 0));  // base
    pyramid->add_submesh(6, 9, new xe::ColorMaterial(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), texture, 0));  // side 1 (red)
    pyramid->add_submesh(9, 12, new xe::ColorMaterial(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), texture, 0)); // side 2 (blue)
    pyramid->add_submesh(12, 15, new xe::ColorMaterial(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), texture, 0)); // side 3 (orange)
    pyramid->add_submesh(15, 18, new xe::ColorMaterial(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), texture, 0)); // side 4 (green)

    add_submesh(pyramid);

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
