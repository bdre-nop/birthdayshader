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
#define VERSION "v1.00 C"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// Constant declarations
#define DEFAULT_WINDOW_WIDTH 800
#define DEFAULT_WINDOW_HEIGHT 600

// Max number of characters in shader buffer string
#define MAX_BUFFER 20000

// Seconds
#define ANIM_START 1.0f
#define ANIM_DURATION 9.0f

#define SCALE_START 10.0f
#define SCALE_END 1.5f

const char* defaultWindowTitle = "Happy Birthday Sam!";

// Vertex shader hard-coded GLSL
const char* vertexShaderSource = 
    "#version 330 core\n"
    "layout (location = 0) in vec2 aPos;\n"
    "out vec2 fragCoord;\n"
    "uniform vec2 iResolution;\n"
    "void main() {\n"
    "    gl_Position = vec4(aPos, 0.0, 1.0);\n"
    "    fragCoord = (aPos + 1.0) * 0.5 * iResolution;\n"
    "}\n";

// Global variables
GLFWwindow* window = NULL;
int defaultWindowX = 0;
int defaultWindowY = 0;

int swapInterval = 1;                  // 0 = vsync OFF, 1 = vsync ON
int showFPS = 0;
double prevTime = 0.0f;
int frameCounter = 0;
float scale = SCALE_START;

float clamp(float val, float min, float max) {
    return (val < min) ? min : (val > max) ? max : val;
}

void loadShaderSource(char *filename, char *buffer) {
    FILE *fptr;
    fptr = fopen(filename, "r");
    if (fptr == NULL) {
        fprintf(stderr, "Failed to open shader file: %s\n", filename);
        exit(1);
    }
    int len = fread(buffer, 1, MAX_BUFFER, fptr);
    if ((len > 0) && (len < MAX_BUFFER)) {
        buffer[len] = (char) 0;
    } else {
        fprintf(stderr, "Failed to read from shader file! (%d)\n", len);
        exit(1);
    }
    fclose(fptr);
}

void checkShaderCompilation(char *type, GLuint shader) {
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        fprintf(stderr, "Shader (%s) Compilation Error: \n%s\n", type, infoLog);
        exit(1);
    }
}

void setUniformRandom() {
    int shaderProgram = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &shaderProgram);
    int randomLocation = glGetUniformLocation(shaderProgram, "uRandom");

    // Set uniform to random distribution between 0.0 and 1.0
    glUniform1f(randomLocation, (float) rand() / (float) RAND_MAX);
}

void resetAnim() {
    // Reset all required variables to make animation begin again from the beginning
    prevTime = 0.0f;
    frameCounter = 0;
    glfwSetTime(0.0f);
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
            showFPS = 0;
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
    glUniform2f(resolutionLocation, (float) width, (float) height);
    // Update viewport size
    glViewport(0, 0, width, height);
}

float easeOutCubic(float t) {
    return 1 - pow(1.0f - t, 3);
}

int main() {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    // Ask for OpenGL version 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, defaultWindowTitle, NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
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
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    // Load and compile fragment shader from file
    char fragmentShaderSource[MAX_BUFFER];
    loadShaderSource("birthday.shader", fragmentShaderSource);
    const GLchar *src = fragmentShaderSource;

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    checkShaderCompilation("vertex", vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &src, NULL);
    glCompileShader(fragmentShader);
    checkShaderCompilation("fragment", fragmentShader);

    // Hide all errors from this point forward to prevent messages showing in terminal
    //  (some kind of bug in Sequoia since December 2024 apparently
    //  e.g. https://github.com/processing/processing4/issues/864 )
    freopen("/dev/null", "w", stderr);

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
    glUniform2f(resolutionLocation, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
    glUniform1f(scaleLocation, scale);
    srand((unsigned int)(time(NULL) ^ clock()));
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
    fprintf(stdout, "\n******** Birthday Shader 2025! ********       %s\n", VERSION);
    fprintf(stdout, "Happy Birthday, Sam!   from Uncle Brian\n");
    fprintf(stdout, "---------------------------------------\n");
    fprintf(stdout, "Press: ( Q )     to quit\n");
    fprintf(stdout, "       ( Space ) to replay the animation\n");
    fprintf(stdout, "       ( F )     to toggle full window size\n");
    fprintf(stdout, "       ( S )     to show/hide frames per second\n");
    fprintf(stdout, "       ( V )     to toggle vsync on/off\n");
    fprintf(stdout, "       ( R )     to reset everything back to default settings\n");

    prevTime = glfwGetTime();
    float time = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        // Show FPS if necessary
        double currentTime = glfwGetTime();
        if (showFPS) {
            frameCounter ++;
            if ((currentTime - prevTime) >= 1.0f) {
                //std::cout << "FPS = " << frameCounter << std::endl;
                setWindowTitle();
                frameCounter = 0;
                prevTime += 1.0f;
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
