//
// Created by Piotr Białas on 20/11/2021.
//

#include "PhongMaterial.h"

#include <fstream>
#include "Application/utils.h"
#include "XeEngine/utils.h"
#include "spdlog/spdlog.h"

namespace xe {

    GLuint PhongMaterial::shader_ = 0u;
    GLuint PhongMaterial::material_uniform_buffer_ = 0u;
    GLint  PhongMaterial::uniform_map_Kd_location_ = 0;

    void PhongMaterial::bind() {
        GLuint prog = program();
        if (prog == 0) {
            spdlog::error("PhongMaterial::bind() - shader program is 0!");
            return;
        }
        glUseProgram(prog);
        spdlog::debug("PhongMaterial::bind() - using program {}", prog);
        int use_map_Kd = 0;
        if (map_Kd_ > 0) {
            OGL_CALL(glUniform1i(uniform_map_Kd_location_, map_Kd_unit_));
            OGL_CALL(glActiveTexture(GL_TEXTURE0 + map_Kd_unit_));
            OGL_CALL(glBindTexture(GL_TEXTURE_2D, map_Kd_));
            use_map_Kd = 1;
        }
        OGL_CALL(glBindBufferBase(GL_UNIFORM_BUFFER, 0, material_uniform_buffer_));

        glBindBuffer(GL_UNIFORM_BUFFER, material_uniform_buffer_);
        // Write Kd at offset 0 (matching Color uniform block layout)
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::vec4), &Kd_[0]);
        // Write use_map_Kd at offset 16 (after vec4 Kd)
        glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(float), sizeof(GLint), &use_map_Kd);
        OGL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, 0u));

    }

    void PhongMaterial::unbind() {
        glBindBuffer(GL_UNIFORM_BUFFER, 0u);
        glBindTexture(GL_TEXTURE_2D, 0u);
    }

    void PhongMaterial::init() {
        {
            std::ofstream logfile("phong_debug.log", std::ios::app);
            logfile << "[PHONG] PhongMaterial::init() started\n";
            logfile.flush();
        }
        
        std::string vs_path = std::string(PROJECT_DIR) + "/shaders/phong_vs.glsl";
        std::string fs_path = std::string(PROJECT_DIR) + "/shaders/phong_fs.glsl";
        
        {
            std::ofstream logfile("phong_debug.log", std::ios::app);
            logfile << "[PHONG] Shader paths: vs=" << vs_path << " fs=" << fs_path << "\n";
            logfile.flush();
        }

        auto program = xe::utils::create_program(
                {{GL_VERTEX_SHADER,   vs_path},
                 {GL_FRAGMENT_SHADER, fs_path}});
        
        {
            std::ofstream logfile("phong_debug.log", std::ios::app);
            logfile << "[PHONG] create_program returned: " << program << "\n";
            logfile.flush();
        }
        if (!program) {
            spdlog::error("PhongMaterial::init() - Failed to create shader program!");
            std::cerr << "Invalid program" << std::endl;
            exit(-1);
        } else {
            spdlog::info("PhongMaterial::init() - Shader program created successfully: {}", program);
        }

        shader_ = program;

        glGenBuffers(1, &material_uniform_buffer_);

        glBindBuffer(GL_UNIFORM_BUFFER, material_uniform_buffer_);
        // Buffer size: vec4 Kd (16 bytes) + int use_map_Kd (4 bytes) = 20 bytes, padded to 24 for alignment
        glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::vec4) + sizeof(GLint), nullptr, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0u);
#if __APPLE__
        uniform_block_binding(shader_, "Material",0);
#endif

#if __APPLE__
        uniform_block_binding(shader_, "Transformations",1);
#endif

#if __APPLE__
        uniform_block_binding(shader_, "Matrices",2);
#endif

#if __APPLE__
        uniform_block_binding(shader_, "Lights",3);
#endif


        uniform_map_Kd_location_ = glGetUniformLocation(shader_, "map_Kd");
        if (uniform_map_Kd_location_ == -1) {
            spdlog::warn("Cannot get uniform {} location", "map_Kd");
        }

    }


}