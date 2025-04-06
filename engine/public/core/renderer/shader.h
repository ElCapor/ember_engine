#pragma once
#include "core/file_system.h"

// TODO: create OpenGL shader

/*!

    @brief Shader class
    - Compile the shader
    - Use the shader
    - Send uniforms

    @version 0.0.2
    @param string vertex The shader source
    @param string fragment The shader source
*/
class Shader {
public:
    Shader() = default;

    template <typename T>
    T GetValue(const std::string& name);

    Shader(const std::string& vertex, const std::string& fragment);

    void Use() const;

    void SetValue(const std::string& name, float value) const;

    void SetValue(const std::string& name, int value) const;

    void SetValue(const std::string& name, glm::mat4 value) const;

    void SetValue(const std::string& name, glm::vec2 value) const;

    void SetValue(const std::string& name, glm::vec3 value) const;

    void SetValue(const std::string& name, glm::vec4 value) const;

    void Destroy();

    unsigned int GetID() const;

    bool IsValid() const;

private:
    unsigned int id;

    unsigned int GetUniformLocation(const std::string& name) const;

    unsigned int CompileShader(unsigned int type, const char* source);

    mutable std::unordered_map<std::string, unsigned int> uniforms;
};

template <typename T>
inline T Shader::GetValue(const std::string& name) {
    unsigned int location = GetUniformLocation(name);
    if (location == -1) {
        LOG_ERROR("Shader variable not found: %s", name.c_str());
        return T();
    }

    if constexpr (std::is_same_v<T, float>) {
        float value;
        glGetUniformfv(id, location, &value);
        return value;
    } else if constexpr (std::is_same_v<T, int>) {
        int value;
        glGetUniformiv(id, location, &value);
        return value;
    } else if constexpr (std::is_same_v<T, glm::vec2>) {
        GLfloat data[2];
        glGetUniformfv(id, location, data);
        return glm::vec2(data[0], data[1]);
    } else if constexpr (std::is_same_v<T, glm::vec3>) {
        GLfloat data[3];
        glGetUniformfv(id, location, data);
        return glm::vec3(data[0], data[1], data[2]);
    } else if constexpr (std::is_same_v<T, glm::vec4>) {
        GLfloat data[4];
        glGetUniformfv(id, location, data);
        return glm::vec4(data[0], data[1], data[2], data[3]);
    } else if constexpr (std::is_same_v<T, glm::mat4>) {
        GLfloat data[16];
        glGetUniformfv(id, location, data);
        return glm::make_mat4(data);
    } else {
        LOG_ERROR("Unsupported type: %s", typeid(T).name());
    }

    return T();
}
