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
    glEnable(GL_CULL_FACE);

    std::vector<GLushort> indices = {
        // Base
        0, 1, 2,
        0, 2, 3,
        // Side
        //1
        4, 5, 6,
        //2
        7, 8, 9,
        //3
        10,11,12,
        //4
        13,14,15
    };

    std::vector<GLfloat> vertices = {
    //BASE
     0.5f, -0.5f,  0.5f,  1,1,1,
    -0.5f, -0.5f,  0.5f,  1,1,1,
    -0.5f, -0.5f, -0.5f,  1,1,1,
     0.5f, -0.5f, -0.5f,  1,1,1,
    //1
     0.5f, -0.5f,  0.5f,  1,0,0, 
     0.0f,  0.5f,  0.0f,  1,0,0,
    -0.5f, -0.5f,  0.5f,  1,0,0,
    //2
    -0.5f, -0.5f,  0.5f,  0,1,0,
     0.0f,  0.5f,  0.0f,  0,1,0,
    -0.5f, -0.5f, -0.5f, 0,1,0,
    //3
    -0.5f, -0.5f, -0.5f, 0,0,1,
     0.0f,  0.5f,  0.0f, 0,0,1,
     0.5f, -0.5f, -0.5f, 0,0,1,
    //4
     0.5f, -0.5f, -0.5f, 1,1,0, 
     0.0f,  0.5f,  0.0f, 1,1,0, 
     0.5f, -0.5f,  0.5f, 1,1,0,
};

    UpdateCameraData();

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

    // auto[w, h] = frame_buffer_size();
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
    
    glm::mat4 Model = glm::mat4(1.0f); // identity 
    angle_ += 0.5f;
    Model = glm::rotate(Model, glm::radians(angle_), glm::vec3(0.5f, 1.0f, 0.0f));

    glm::mat4 PVM = P_ * V_ * Model;

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
    glViewport(0,0,w,h);
    //camera()->set_aspect((float) w / h);
    aspect_ = float(w)/float(h);
    P_ = glm::perspective(fov_, aspect_, near_, far_);
}

void SimpleShapeApplication::UpdateCameraData() {
    int w,h;
    std::tie(w,h) = frame_buffer_size();
    aspect_ = float(w)/float(h);
    fov_ = glm::pi<float>()/4.0f;
    near_ = 0.1f;
    far_ = 100.0f;
    P_ = glm::perspective(fov_, aspect_, near_, far_);
    V_ = glm::lookAt(
        glm::vec3(0.0f,0.0f,2.0f),
        glm::vec3(0.0f,0.0f,0.0f),
        glm::vec3(0.0f,1.0f,0.0f)
    );
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
