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
    
    // A utility function that reads the shader sources, compiles them and creates the program object
    // As everything in OpenGL we reference program by an integer "handle".
    std::cout << "Creating shader program..." << std::endl;
    auto vs_path = std::string(PROJECT_DIR) + "/shaders/base_vs.glsl";
    auto fs_path = std::string(PROJECT_DIR) + "/shaders/base_fs.glsl";
    std::cout << "Vertex shader: " << vs_path << std::endl;
    std::cout << "Fragment shader: " << fs_path << std::endl;
    
    auto program = xe::utils::create_program(
            {{GL_VERTEX_SHADER,   vs_path},
             {GL_FRAGMENT_SHADER, fs_path}});

    if (!program) {
        std::cerr << "ERROR: Failed to create shader program!" << std::endl;
        throw std::runtime_error("Failed to create shader program");
    }
    std::cout << "Shader program created successfully: " << program << std::endl;
    
    std::cout << "Initializing ColorMaterial..." << std::endl;
    xe::ColorMaterial::init();
    std::cout << "Setting shader..." << std::endl;
    xe::ColorMaterial::set_shader(program);
    std::cout << "Initializing uniform locations..." << std::endl;
    xe::ColorMaterial::init_uniform_locations();
    std::cout << "ColorMaterial initialized" << std::endl;
    
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
    // Position (x, y, z) + Texture coordinates (u, v)
    // BASE - maps to central diamond shape (rotated square)
    // Using exact UV coordinates from UV map: (0.1910, 0.5), (0.5, 0.8090), (0.8090, 0.5), (0.5, 0.1910)
    // Vertex order: 0=front-right, 1=front-left, 2=back-left, 3=back-right
    // Indices: Triangle 1: 0,1,2 (front-right, front-left, back-left)
    //         Triangle 2: 0,2,3 (front-right, back-left, back-right)
    // Mapping vertices to diamond: need to match spatial arrangement
    // Looking at base from above: front-right(top-right), front-left(top-left), back-left(bottom-left), back-right(bottom-right)
    // Diamond points: right(0.8090,0.5), left(0.1910,0.5), top(0.5,0.8090), bottom(0.5,0.1910)
     0.5f, -0.5f,  0.5f,  0.5f, 0.8090f,   // vertex 0 (front-right) -> top of diamond (0.5, 0.8090)
    -0.5f, -0.5f,  0.5f,  0.1910f, 0.5f,   // vertex 1 (front-left) -> left of diamond (0.1910, 0.5)
    -0.5f, -0.5f, -0.5f,  0.5f, 0.1910f,   // vertex 2 (back-left) -> bottom of diamond (0.5, 0.1910)
     0.5f, -0.5f, -0.5f,  0.8090f, 0.5f,   // vertex 3 (back-right) -> right of diamond (0.8090, 0.5)

    // SIDE 1 - maps to top-left red triangle
    // Using exact UV coordinates: (0, 1), (0.1910, 0.5), (0.5, 0.8090)
     0.5f, -0.5f,  0.5f,  0.5f, 0.8090f,   // bottom-right vertex -> top midpoint (0.5, 0.8090)
     0.0f,  0.5f,  0.0f,  0.0f, 1.0f,      // apex -> top-left corner (0, 1)
    -0.5f, -0.5f,  0.5f,  0.1910f, 0.5f,   // bottom-left vertex -> left midpoint (0.1910, 0.5)

    // SIDE 2 - maps to top-right blue triangle
    // Using exact UV coordinates: (1, 1), (0.5, 0.8090), (0.8090, 0.5)
    -0.5f, -0.5f,  0.5f,  0.5f, 0.8090f,   // bottom-left vertex -> top midpoint (0.5, 0.8090)
     0.0f,  0.5f,  0.0f,  1.0f, 1.0f,      // apex -> top-right corner (1, 1)
    -0.5f, -0.5f, -0.5f,  0.8090f, 0.5f,   // bottom-right vertex -> right midpoint (0.8090, 0.5)

    // SIDE 3 - maps to bottom-right orange triangle
    // Using exact UV coordinates: (1, 0), (0.8090, 0.5), (0.5, 0.1910)
    -0.5f, -0.5f, -0.5f,  0.8090f, 0.5f,   // bottom-left vertex -> right midpoint (0.8090, 0.5)
     0.0f,  0.5f,  0.0f,  1.0f, 0.0f,      // apex -> bottom-right corner (1, 0)
     0.5f, -0.5f, -0.5f,  0.5f, 0.1910f,   // bottom-right vertex -> bottom midpoint (0.5, 0.1910)

    // SIDE 4 - maps to bottom-left green triangle
    // Using exact UV coordinates: (0, 0), (0.5, 0.1910), (0.1910, 0.5)
     0.5f, -0.5f, -0.5f,  0.5f, 0.1910f,   // bottom-right vertex -> bottom midpoint (0.5, 0.1910)
     0.0f,  0.5f,  0.0f,  0.0f, 0.0f,      // apex -> bottom-left corner (0, 0)
     0.5f, -0.5f,  0.5f,  0.1910f, 0.5f,   // bottom-left vertex -> left midpoint (0.1910, 0.5)

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
    std::cout << "Loading texture..." << std::endl;
    stbi_set_flip_vertically_on_load(true);
    GLint width, height, channels;
    auto texture_file = std::string(ROOT_DIR) + "/Models/multicolor.png";
    std::cout << "Texture file path: " << texture_file << std::endl;
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

    // Multiple submeshes - each face uses the same texture but different UV coordinates
    // Base (indices 0-6): central grey square
    // Side 1 (indices 6-9): top-left red triangle
    // Side 2 (indices 9-12): top-right blue triangle
    // Side 3 (indices 12-15): bottom-right orange triangle
    // Side 4 (indices 15-18): bottom-left green triangle
    std::cout << "Creating ColorMaterial with texture: " << texture << std::endl;
    pyramid->add_submesh(0, 6, new xe::ColorMaterial(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), texture, 0));  // base
    pyramid->add_submesh(6, 9, new xe::ColorMaterial(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), texture, 0));  // side 1 (red)
    pyramid->add_submesh(9, 12, new xe::ColorMaterial(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), texture, 0)); // side 2 (blue)
    pyramid->add_submesh(12, 15, new xe::ColorMaterial(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), texture, 0)); // side 3 (orange)
    pyramid->add_submesh(15, 18, new xe::ColorMaterial(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), texture, 0)); // side 4 (green)

    add_submesh(pyramid);

    std::cout << "Setting clear color and viewport..." << std::endl;
    glClearColor(0.81f, 0.81f, 0.8f, 1.0f);

    glViewport(0, 0, w, h);
    std::cout << "SimpleShapeApplication::init() completed successfully" << std::endl;
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
