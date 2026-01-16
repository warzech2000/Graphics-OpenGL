#include "app.h"

#include <iostream>
#include <vector>
#include <tuple>

#include "Application/utils.h"

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
    // glEnable(GL_CULL_FACE); 




    std::vector<GLfloat> vertices = {
        -0.5f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
        0.5f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f,    1.0f, 0.0f, 0.0f,

        -0.5f, 0.0f, 0.0f,   0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
        0.5f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,

        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,
        0.5f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,

    };

    // glGenBuffers(1, &u_pvm_buffer_);
    // OGL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, u_pvm_buffer_));
    // glBufferData(GL_UNIFORM_BUFFER, 16 * sizeof(float), 0, GL_STATIC_DRAW);
    // glBindBufferBase(GL_UNIFORM_BUFFER, 1, u_pvm_buffer_);
    // glBindBuffer(GL_UNIFORM_BUFFER, u_pvm_buffer_);
    
    // Create UBO
    glGenBuffers(1, &u_pvm_buffer_);
    glBindBuffer(GL_UNIFORM_BUFFER, u_pvm_buffer_);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Bind shader block to binding point 0
    GLuint block_index = glGetUniformBlockIndex(program, "Transformations");
    glUniformBlockBinding(program, block_index, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, u_pvm_buffer_);
    
    GLuint v_buffer_handle;
    glGenBuffers(1, &v_buffer_handle);
    OGL_CALL(glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle));
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    #pragma region Camera
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    auto[w, h] = frame_buffer_size();
    set_camera(new Camera);

    camera()->look_at(
        glm::vec3(0, 0, 1),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 1, 0)
    );

    camera()->perspective(glm::pi<float>()/2.0, (float)w/h, 0.1f, 100.f);

    set_controler(new CameraControler(camera()));
#pragma endregion


    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, v_buffer_handle);

    glEnableVertexAttribArray(0); // position
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid *)0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(0));


    glEnableVertexAttribArray(1);
    //glVertexAttribPointer(1 , 3, GL_FLOAT, GL_FALSE, 6*sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<GLvoid *>(3* sizeof(GLfloat)));

    // Setting the background color of the rendering window,
    // I suggest not to use white or black for better debuging.
    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);

    // This setups an OpenGL vieport of the size of the whole rendering window.
    glViewport(0, 0, w, h);

    glUseProgram(program);
}

//This functions is called every frame and does the actual rendering.
void SimpleShapeApplication::frame() {
    
    auto PVM = camera()->projection() * camera()->view();

    glBindBuffer(GL_UNIFORM_BUFFER, u_pvm_buffer_);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &PVM[0][0]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(vao_);
    glDrawArrays(GL_TRIANGLES, 0, 3); 
    glDrawArrays(GL_TRIANGLES,3,3); 
    glDrawArrays(GL_TRIANGLES,6,3); 
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
