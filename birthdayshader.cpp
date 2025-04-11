/*******************************************************************
    Birthday Shader 2025
    April 11, 2025
    Happy birthday, Sam! From Uncle Brian

    Requires GLFW and GLEW which can be installed with homebrew on macOS:
            brew install glfw glew
        https://www.glfw.org/
        https://glew.sourceforge.net/

    Also available at: https://www.shadertoy.com/view/WXjGzV
*******************************************************************/
#define VERSION "v1.00 C++"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include <chrono>
#include <cstdio>

// Constant declarations
constexpr uint32_t DEFAULT_WINDOW_WIDTH = 800;
constexpr uint32_t DEFAULT_WINDOW_HEIGHT = 600;

constexpr float ANIM_START = 1.0;           // Seconds
constexpr float ANIM_DURATION = 9.0;

constexpr float SCALE_START = 10.0;
constexpr float SCALE_END = 1.5;

const char* defaultWindowTitle = "Happy Birthday Sam!";

// Vertex shader hard-coded GLSL
const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    out vec2 fragCoord;
    uniform vec2 iResolution;

    void main() {
        gl_Position = vec4(aPos, 0.0, 1.0);
        fragCoord = (aPos + 1.0) * 0.5 * iResolution; // Convert from [-1,1] to [0,screenSize]
    }
)";

// Global variables
GLFWwindow* window = nullptr;
int defaultWindowX = 0;
int defaultWindowY = 0;

uint32_t swapInterval = 1;                  // 0 = vsync OFF, 1 = vsync ON
bool showFPS = false;
double prevTime = 0.0;
uint32_t frameCounter = 0;
float scale = SCALE_START;

// Screw C++17 requirements just for a simple 'minmax' :-p
float clamp(float val, float min, float max) {
    return (val < min) ? min : (val > max) ? max : val;
}

std::string findShaderFile(const std::string& filename) {
    // First, try opening the file in the current working directory
    std::ifstream file(filename);
    if (file.good()) {
        return filename;
    }

    // If still not found, return an empty string to indicate failure
    return "";
}

std::string loadShaderSource(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Failed to open shader file: " << filename << std::endl;
        exit(1);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void checkShaderCompilation(GLuint shader) {
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader Compilation Error:\n" << infoLog << std::endl;
        exit(1);
    }
}

void setUniformRandom() {
    int shaderProgram = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &shaderProgram);
    int randomLocation = glGetUniformLocation(shaderProgram, "uRandom");

    // Seed with current time since the dawn of Mankind
    static std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());

    // Set uniform to random distribution between 0.0 and 1.0
    std::uniform_real_distribution<float> dist(0.0f, 1.0f);
    glUniform1f(randomLocation, dist(rng));
}

void resetAnim() {
    // Reset all required variables to make animation begin again from the beginning
    prevTime = 0.0;
    frameCounter = 0;
    glfwSetTime(0.0);
    scale = SCALE_START;
    setUniformRandom();
}

void setWindowTitle() {
    if (showFPS) {
        char title[64];
        snprintf(title, sizeof(title), "%s  (FPS: %d)", defaultWindowTitle, frameCounter);
        glfwSetWindowTitle(window, title);
    } else {
        glfwSetWindowTitle(window, defaultWindowTitle);
    }
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if ((action == GLFW_PRESS) || (action == GLFW_REPEAT)) {
        if (key == GLFW_KEY_Q) {
            // Press Q to close the app
            glfwSetWindowShouldClose(window, 1);
        } else if (key == GLFW_KEY_F) {
            // Press F to toggle maximized window size or not
            int maximized = glfwGetWindowAttrib(window, GLFW_MAXIMIZED);
            if (maximized) {
                glfwRestoreWindow(window);
            } else {
                glfwMaximizeWindow(window);
            }
        } else if (key == GLFW_KEY_R) {
            // Press R to reset window size and position to defaults
            glfwSetWindowPos(window, defaultWindowX, defaultWindowY);
            glfwSetWindowSize(window, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
            swapInterval = 1;
            glfwSwapInterval(swapInterval);
            showFPS = false;
            setWindowTitle();
            resetAnim();
        } else if (key == GLFW_KEY_SPACE) {
            // Press Space to rerun animation from time 0
            resetAnim();
        } else if (key == GLFW_KEY_V) {
            // Press V to enable/disable vsync
            swapInterval = 1 - swapInterval;
            glfwSwapInterval(swapInterval);
        } else if (key == GLFW_KEY_S) {
            // Press S to show/hide FPS
            showFPS = !showFPS;
            setWindowTitle();
            prevTime = glfwGetTime();
            frameCounter = 0;
        }
    }
} // keyCallback

void framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    // Update uniform
    int shaderProgram = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &shaderProgram);
    int resolutionLocation = glGetUniformLocation(shaderProgram, "iResolution");
    glUniform2f(resolutionLocation, static_cast<float>(width), static_cast<float>(height));
    // Update viewport size
    glViewport(0, 0, width, height);
}

float easeOutCubic(float t) {
    return 1 - pow(1.0 - t, 3);
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Ask for OpenGL version 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, defaultWindowTitle, nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // std::cout << "GL_VERSION: " << glGetString(GL_VERSION) << std::endl;

    // Enable/disable vsync
    glfwSwapInterval(swapInterval);

    // Save starting window position so we can reset it later if necessary
    glfwGetWindowPos(window, &defaultWindowX, &defaultWindowY);

    // Set callback functions
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    glfwSetKeyCallback(window, keyCallback);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Load and compile fragment shader from file
    std::string shaderFile = findShaderFile("birthday.shader");
    if (shaderFile.empty()) {
        std::cerr << "Failed to find shader file: birthday.shader\n";
        return 1;
    }
    std::string fragmentShaderStr = loadShaderSource(shaderFile);
    const char* fragmentShaderSource = fragmentShaderStr.c_str();
    
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    checkShaderCompilation(vertexShader);

    // Hide all errors from this point forward to prevent messages showing in terminal
    //  (some kind of bug in Sequoia since December 2024 apparently
    //  e.g. https://github.com/processing/processing4/issues/864 )
    freopen("/dev/null", "w", stderr);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    checkShaderCompilation(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Initialize shader uniforms
    int resolutionLocation = glGetUniformLocation(shaderProgram, "iResolution");
    int scaleLocation = glGetUniformLocation(shaderProgram, "uScale");
    int timeLocation = glGetUniformLocation(shaderProgram, "iTime");
    glUseProgram(shaderProgram);

    // Set resolution uniform for shaders
    glUniform2f(resolutionLocation, static_cast<float>(DEFAULT_WINDOW_WIDTH), 
        static_cast<float>(DEFAULT_WINDOW_HEIGHT));
    glUniform1f(scaleLocation, scale);
    setUniformRandom();

    float vertices[] = { -1.0f, -1.0f,  1.0f, -1.0f,  -1.0f, 1.0f,  1.0f, 1.0f };
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Print usage instructions to stdout
    std::cout << std::endl << "******** Birthday Shader 2025! ********       " << VERSION << std::endl <<
        "Happy Birthday, Sam!   from Uncle Brian" << std::endl <<
        "---------------------------------------" << std::endl <<
        "Press: ( Q )     to quit" << std::endl <<
        "       ( Space ) to replay the animation" << std::endl <<
        "       ( F )     to toggle full window size" << std::endl <<
        "       ( S )     to show/hide frames per second" << std::endl <<
        "       ( V )     to toggle vsync on/off" << std::endl <<
        "       ( R )     to reset everything back to default settings" << std::endl;

    prevTime = glfwGetTime();
    float time = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        // Show FPS if necessary
        double currentTime = glfwGetTime();
        if (showFPS) {
            frameCounter ++;
            if ((currentTime - prevTime) >= 1.0) {
                //std::cout << "FPS = " << frameCounter << std::endl;
                setWindowTitle();
                frameCounter = 0;
                prevTime += 1.0;
            }
        }

        // Update necessary uniforms each frame
        glUniform1f(timeLocation, currentTime);

        float t = (glfwGetTime() - ANIM_START) / ANIM_DURATION;
        t = clamp(t, 0.0f, 1.0f);
        float easedScale = SCALE_START + (SCALE_END - SCALE_START) * easeOutCubic(t);
        glUniform1f(scaleLocation, easedScale);

        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glfwTerminate();
    return 0;
} // main
