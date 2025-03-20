#include "core/renderer/ember_gl.h"

#include <stb_image.h>


Renderer* CreateRenderer(SDL_Window* window, int view_width, int view_height) {

#if defined(SDL_PLATFORM_IOS) || defined(SDL_PLATFORM_ANDROID) || defined(SDL_PLATFORM_EMSCRIPTEN)

    /* GLES 3.0 -> GLSL: 300 */
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);

#elif defined(SDL_PLATFORM_WINDOWS) || defined(SDL_PLATFORM_LINUX) || defined(SDL_PLATFORM_MACOS)

    /* OPENGL 3.3 -> GLSL: 330*/
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

#endif

    SDL_GLContext glContext = SDL_GL_CreateContext(window);

    if (!glContext) {
        LOG_CRITICAL("Failed to create GL context");
        return nullptr;
    }

#if defined(SDL_PLATFORM_IOS) || defined(SDL_PLATFORM_ANDROID)

    if (!gladLoadGLLoader((GLADloadproc) SDL_GL_GetProcAddress)) {
        LOG_CRITICAL("Failed to initialize GLAD (GL_FUNCTIONS)");
        return nullptr;
    }

#else

    if (!gladLoadGLES2Loader((GLADloadproc) SDL_GL_GetProcAddress)) {
        LOG_CRITICAL("Failed to initialize GLAD (GLES_FUNCTIONS)");
        return nullptr;
    }

#endif

    Renderer* _renderer    = new Renderer;
    _renderer->viewport[0] = view_width;
    _renderer->viewport[1] = view_height;
    _renderer->window      = window;
    _renderer->gl_context  = glContext;

    unsigned int shaderProgram = CreateShaderProgram();
    _renderer->shaderProgram   = shaderProgram;

    float vertices[] = {
        // pos         // tex coords
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    };

    glGenVertexArrays(1, &_renderer->vao);
    glGenBuffers(1, &_renderer->vbo);
    glBindVertexArray(_renderer->vao);
    glBindBuffer(GL_ARRAY_BUFFER, _renderer->vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));

    glEnableVertexAttribArray(1);

    glViewport(0, 0, view_width, view_height);

    renderer = _renderer;

    return _renderer;
}

Renderer* GetRenderer() {
    return renderer;
}

void DestroyRenderer() {
    glDeleteProgram(renderer->shaderProgram);
    glDeleteVertexArrays(1, &renderer->vao);
    glDeleteBuffers(1, &renderer->vbo);

    SDL_GL_DestroyContext(renderer->gl_context);

    SDL_DestroyWindow(renderer->window);
}

unsigned int CompileShader(unsigned int type, const char* src) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(shader, 512, nullptr, info_log);
        const char* type_str = type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT";
        LOG_CRITICAL("[%s] - Shader compilation failed: %s", type_str, info_log);
    }

    LOG_INFO("Successfully compiled %s", type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT");

    return shader;
}

unsigned int CreateShaderProgram() {

    const auto vertexShaderSrc   = SHADER_HEADER + LoadAssetsFile("shaders/default_vert.glsl");
    const auto fragmentShaderSrc = SHADER_HEADER + LoadAssetsFile("shaders/default_frag.glsl");

    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShaderSrc.c_str());
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSrc.c_str());

    unsigned int program = glCreateProgram();

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    // WARN: since our project is simple, we don't need to delete shader's ( it help's with debugging )
    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}

void ClearBackground(Color color) {
    glm::vec4 norm_color = {color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f};

    glClearColor(norm_color.r, norm_color.g, norm_color.b, norm_color.a);
}

void BeginDrawing() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void EndDrawing() {
    SDL_GL_SwapWindow(renderer->window);
}

Texture LoadTexture(const std::string& file_path) {
    int w, h, channels;

    stbi_set_flip_vertically_on_load(false);

    auto path = (ASSETS_PATH + file_path);

    unsigned char* data = stbi_load(path.c_str(), &w, &h, &channels, 4);

    if (!data) {
        LOG_ERROR("Failed to load texture with path: %s", path.c_str());
        return {};
    }

    // TODO: we should create a simple texture if failed loading.
    GLuint texId;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // GL_NEAREST is better for pixel art style

    stbi_image_free(data);
    LOG_INFO("Loaded texture with ID: %d, path: %s", texId, path.c_str());
    LOG_INFO(" > Width %d, Height %d",w,h);
    LOG_INFO(" > Num. Channels %d",channels);

    return {texId, w, h};
}


void UnloadTexture(Texture2D texture) {
    glDeleteTextures(1, &texture.id);
}

Font LoadFont(const std::string& file_path, float font_size) {
    Font font{0};

    auto path = ASSETS_PATH + file_path;

    std::vector<unsigned char> ttf_buffer(1 << 20); // 1 mb

    SDL_IOStream* file_rw = SDL_IOFromFile(path.c_str(), "rb");

    if (!file_rw) {
        LOG_ERROR("Failed to open font file %s", path.c_str());
        return font;
    }

    Sint64 size = SDL_GetIOSize(file_rw);
    if (size <= 0) {
        LOG_ERROR("Failed to get file size %s", path.c_str());
        SDL_CloseIO(file_rw);
        return font;
    }

    if (SDL_ReadIO(file_rw, ttf_buffer.data(), size) != size) {
        LOG_ERROR("Failed to read file %s", path.c_str());
        SDL_CloseIO(file_rw);
        return font;
    }

    stbtt_fontinfo font_info;

    if (!stbtt_InitFont(&font_info, ttf_buffer.data(), 0)) {
        LOG_ERROR("Failed to init font %s", path.c_str());
        SDL_CloseIO(file_rw);
        return font;
    }


    SDL_CloseIO(file_rw);

    font.font_info      = font_info;
    font.texture.width  = 1024;
    font.texture.height = 1024;
    font.font_size      = font_size;

    std::vector<unsigned char> font_bitmap(font.texture.width * font.texture.height);

    stbtt_BakeFontBitmap(ttf_buffer.data(), 0, font_size, font_bitmap.data(), font.texture.width, font.texture.height,
                         32, 96, font.char_data); 

    std::vector<unsigned char> rgba_bitmap(font.texture.width * font.texture.height * 4);

    for (int i = 0; i < font.texture.width * font.texture.height; ++i) {
        unsigned char alpha    = font_bitmap[i];
        rgba_bitmap[i * 4 + 0] = 255; // R
        rgba_bitmap[i * 4 + 1] = 255; // G
        rgba_bitmap[i * 4 + 2] = 255; // B
        rgba_bitmap[i * 4 + 3] = alpha; // A
    }

    glGenTextures(1, &font.texture.id);
    glBindTexture(GL_TEXTURE_2D, font.texture.id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, font.texture.width, font.texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 rgba_bitmap.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    LOG_INFO("Loaded font with ID: %d, path: %s", font.texture.id, path.c_str());
    LOG_INFO(" > Width %d, Height %d", font.texture.width, font.texture.height);
    LOG_INFO(" > Num. Glyphs %d, Start %d, Kerning %d, Font Size %.2f", font_info.numGlyphs, font_info.fontstart,
             font_info.kern, font_size);

    return font;
}

void DrawText(Font& font, const std::string& text, glm::vec2 position, Color color, float scale, float kerning = 0.0f){

}


void DrawTexture(Texture2D tex, Rectangle rect, Color color) {
    glUseProgram(renderer->shaderProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex.id);
    glUniform1i(glGetUniformLocation(renderer->shaderProgram, "u_Tex"), 0);

    glm::vec4 norm_color = {
        color.r / 255.0f,
        color.g / 255.0f,
        color.b / 255.0f,
        color.a / 255.0f,
    };

    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(rect.x, rect.y, 0.0f));
    model           = glm::scale(model, glm::vec3(rect.width, rect.height, 1.0f));

    glm::mat4 projection = glm::ortho(0.0f, (float) renderer->viewport[0], (float) renderer->viewport[1], 0.0f);

    glUniformMatrix4fv(glGetUniformLocation(renderer->shaderProgram, "u_Model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(renderer->shaderProgram, "u_Projection"), 1, GL_FALSE,
                       glm::value_ptr(projection));
    glUniform4fv(glGetUniformLocation(renderer->shaderProgram, "u_Color"), 1, glm::value_ptr(norm_color));

    float vertices[] = {
        // pos         // tex coords
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    };

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    glBindVertexArray(renderer->vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}


void DrawTextureEx(Texture2D texture, Rectangle source, Rectangle dest, glm::vec2 origin, float rotation, Color color) {
    glUseProgram(renderer->shaderProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture.id);
    glUniform1i(glGetUniformLocation(renderer->shaderProgram, "u_Tex"), 0);

    glm::mat4 projection = glm::ortho(0.0f, (float) renderer->viewport[0], (float) renderer->viewport[1], 0.0f);

    glm::mat4 model = glm::mat4(1.0f);
    model           = glm::translate(model, glm::vec3(dest.x, dest.y, 0.0f));
    model           = glm::translate(model, glm::vec3(origin.x, origin.y, 0.0f));
    model           = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    model           = glm::translate(model, glm::vec3(-origin.x, -origin.y, 0.0f));
    model           = glm::scale(model, glm::vec3(dest.width, dest.height, 1.0f));

    glm::vec4 norm_color = {
        color.r / 255.0f,
        color.g / 255.0f,
        color.b / 255.0f,
        color.a / 255.0f,
    };

    float texLeft   = (float) source.x / texture.width;
    float texRight  = (float) (source.x + source.width) / texture.width;
    float texTop    = (float) source.y / texture.height;
    float texBottom = (float) (source.y + source.height) / texture.height;

    float vertices[] = {
        // pos         // tex coords
        0.0f, 0.0f, 0.0f, texLeft,  texTop,    1.0f, 0.0f, 0.0f, texRight, texTop,
        1.0f, 1.0f, 0.0f, texRight, texBottom, 0.0f, 1.0f, 0.0f, texLeft,  texBottom,
    };

    glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    glUniformMatrix4fv(glGetUniformLocation(renderer->shaderProgram, "u_Model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(renderer->shaderProgram, "u_Projection"), 1, GL_FALSE,
                       glm::value_ptr(projection));
    glUniform4fv(glGetUniformLocation(renderer->shaderProgram, "u_Color"), 1, glm::value_ptr(norm_color));

    glBindVertexArray(renderer->vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}
