#include "app.h"

#include <iostream>
#include <vector>
#include <tuple>

#include "Application/utils.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>


void SimpleShapeApplication::init() {
    // A utility function that reads the shader sources, compiles them and creates the program object
    // As everything in OpenGL we reference program by an integer "handle".
    auto program = xe::utils::create_program(
            {{GL_VERTEX_SHADER,   std::string(PROJECT_DIR) + "/shaders/base_vs.glsl"},
             {GL_FRAGMENT_SHADER, std::string(PROJECT_DIR) + "/shaders/base_fs.glsl"}});

    if (!program) {
        std::cerr << "Invalid program" << std::endl;
        exit(-1);
    }
    
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE); 

    std::vector<GLushort> indices = {
    0, 1, 2,
    3, 4, 5,

    6, 7, 8,
    9, 10, 11,
    12, 13, 14,
    15, 16, 17
    };

    // A vector containing the x,y,z vertex coordinates + texture coords
    std::vector<GLfloat> vertices = {

    // Base
     0.5f, -0.5f,  0.5f,   1, 0, 1,
    -0.5f, -0.5f,  0.5f,   1, 0, 1,
    -0.5f, -0.5f, -0.5f,   1, 0, 1,

     0.5f, -0.5f,  0.5f,   1, 0, 1,
    -0.5f, -0.5f, -0.5f,   1, 0, 1,
     0.5f, -0.5f, -0.5f,   1, 0, 1,


    // Side
    // 1
     0.5f, -0.5f,  0.5f,   1, 0, 0,
    -0.5f, -0.5f,  0.5f,   1, 0, 0,
     0.0f,  0.5f,  0.0f,   1, 0, 0,

    // 2
    -0.5f, -0.5f,  0.5f,   0, 1, 0,
    -0.5f, -0.5f, -0.5f,   0, 1, 0,
     0.0f,  0.5f,  0.0f,   0, 1, 0,

    // 3
    -0.5f, -0.5f, -0.5f,   0, 0, 1,
     0.5f, -0.5f, -0.5f,   0, 0, 1,
     0.0f,  0.5f,  0.0f,   0, 0, 1,
    // 4
     0.5f, -0.5f, -0.5f,   1, 1, 0,
     0.5f, -0.5f,  0.5f,   1, 1, 0,
     0.0f,  0.5f,  0.0f,   1, 1, 0,
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

    // vertex buffer
    GLuint v_buffer_handle;
    glGenBuffers(1, &v_buffer_handle);
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle);
    //OGL_CALL(glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle));
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    #pragma region Camera

    auto[w, h] = frame_buffer_size();
    // set_camera(new Camera);

    // camera()->look_at(
    //     glm::vec3(0, 0, 1),
    //     glm::vec3(0, 0, 0),
    //     glm::vec3(0, 1, 0)
    // );

    // camera()->perspective(glm::pi<float>()/2.0, (float)w/h, 0.1f, 100.f);

    // set_controler(new CameraControler(camera()));
#pragma endregion

    // VAO
    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

        // VBO
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // EBO
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), indices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(0));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    glBindVertexArray(0);

    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);

    // This setups an OpenGL vieport of the size of the whole rendering window.
    glViewport(0, 0, w, h);

    glUseProgram(program);
}

//This functions is called every frame and does the actual rendering.

void SimpleShapeApplication::frame() {
    //auto PVM = camera()->projection() * camera()->view();

    auto[w, h] = frame_buffer_size();

    glm::mat4 Model = glm::mat4(1.0f); // identity 
    angle_ += 0.5f;
    Model = glm::rotate(Model, glm::radians(angle_), glm::vec3(-0.5f, 1.0f, 0.0f));

    glm::mat4 View = glm::lookAt(
        glm::vec3(0.0f,0.0f,2.0f),
        glm::vec3(0.0f,0.0f,0.0f),
        glm::vec3(0.0f,1.0f,0.0f)
    );

    float aspect = float(w)/float(h);
    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

    glm::mat4 PVM = Projection * View * Model;

    glBindBuffer(GL_UNIFORM_BUFFER, u_pvm_buffer_);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(PVM));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
}



void SimpleShapeApplication::framebuffer_resize_callback(int w, int h) {
    Application::framebuffer_resize_callback(w, h);
    // glViewport(0,0,w,h);
    // camera()->set_aspect((float) w / h);
}

void SimpleShapeApplication::mouse_button_callback(int button, int action, int mods) {
    Application::mouse_button_callback(button, action, mods);

    // if (controler_) {
    //     double x, y;
    //     glfwGetCursorPos(window_, &x, &y);

    //     if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    //         controler_->LMB_pressed(x, y);

    //     if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    //         controler_->LMB_released(x, y);
    // }

}

void SimpleShapeApplication::cursor_position_callback(double x, double y) {
    Application::cursor_position_callback(x, y);
    // if (controler_) {
    //     controler_->mouse_moved(x, y);
    // }
}
