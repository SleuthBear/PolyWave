#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <../Shader.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <imgui/imgui_internal.h>
#include <STB_IMAGE/stb_image.h>

#include "WaveMesh.h"

// Functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window, WaveMesh *wave);
void mouse_callback(GLFWwindow* window, double xPosIn, double yPosIn);
float wave_equation(float x, float y);
unsigned int loadTexture(char const * path);

constexpr unsigned SCR_WIDTH = 1200;
constexpr unsigned SCR_HEIGHT = 900;
auto cameraPos   = glm::vec3(0.0f, 2.0f,  5.0f);
auto cameraFront = glm::vec3(0.0f, -0.5f, -1.0f);
auto cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);
// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
float timeSinceUpdated = 0.0f;
// camera
bool firstMouse = true;
bool wireFrame = true;
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
float fov   =  45.0f;

glm::vec3 lightPos(0.0f, 2.0f, 3.0f);

struct WaveParams {
    float amplitude;
    float timeConstant;
    float xFrequency;
    float zFrequency;
    float offset;
};

int main() {
    //GLFW INIT BLOCK ---------------------------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    //---------------------------------------------------------------

    // CREATE WINDOW ------------------------------------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "PolyWave", nullptr, nullptr);

    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetCursorPosCallback(window, mouse_callback);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //----------------------------------------------------------------

    // LOAD FUNCTION POINTERS ----------------------------------------
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cout << "Failed to initialise GLAD" << std::endl;
        return -1;
    }
    //----------------------------------------------------------------
    // IMGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    const ImGuiIO &io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // SHADERS -------------------------------------------------------
    const Shader shader("../shaders/shader.vert", "../shaders/shader.frag");
    WaveMesh wave(500, 10);
    loadTexture("../sea.jpg"); // https://www.manytextures.com/texture/30/rough-sea/

    shader.use();
    shader.setVec3("objectColor", 0.0f, 0.5f, 1.0f);
    shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
    shader.setVec3("lightPos", lightPos);
    shader.setInt("tex", 0);

    int nWaves = 0;
    std::vector<WaveParams> waves;

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        const auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window, &wave);

        // render
        // ------
        glClearColor(0.6f, 0.8f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (wireFrame) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }


        shader.use();
        glm::mat4 projection = glm::perspective(glm::radians(fov), static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT), 0.1f, 100.0f);
        shader.setMat4("projection", projection);
        glm::mat4 view = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        shader.setMat4("view", view);
        auto model = glm::mat4(1.0f);
        shader.setMat4("model", model);
        shader.setFloat("time", static_cast<float>(glfwGetTime()));
        shader.setFloat("numVertices", static_cast<float>(wave.numVertices));
        shader.setVec3("viewPos", cameraPos);
        shader.setInt("nWaves", nWaves);
        for(int i=0; i <nWaves; i++) {
            shader.setFloat("waves[" + std::to_string(i) + "].amplitude", waves[i].amplitude);
            shader.setVec2("waves[" + std::to_string(i) + "].Frequency", glm::vec2(waves[i].xFrequency, waves[i].zFrequency));
            shader.setFloat("waves[" + std::to_string(i) + "].timeConstant", waves[i].timeConstant);
            shader.setFloat("waves[" + std::to_string(i) + "].offset", waves[i].offset);
        }

        wave.draw(&shader);


        ImGui::Begin("GUI Window");
        ImGui::Text("Random Text");
        //--------
        ImGui::Checkbox("Wireframe Mode", &wireFrame);
        ImGui::SliderInt("Vertices Per Side", &wave.numVertices, 10, 500);
        if (ImGui::Button("Update Vertices")) {
            wave.updateVertices();
            wave.updateIndices();
        }
        ImGui::Text("New Wave Equation");
        static float amplitude = 0.05f;
        ImGui::InputFloat("Amplitude", &amplitude, 0.01f, 1.0f, "%.3f");
        static float offset = 1.0f;
        ImGui::InputFloat("Offset", &offset, 0.01f, 1.0f, "%.3f");
        static float xFrequency = 5.f;
        ImGui::InputFloat("x Frequency", &xFrequency, 0.01f, 1.0f, "%.3f");
        static float zFrequency = 5.f;
        ImGui::InputFloat("z Frequency", &zFrequency, 0.01f, 1.0f, "%.3f");
        static float timeConstant = 1.0f;
        ImGui::InputFloat("Time Constant", &timeConstant, 0.01f, 1.0f, "%.3f");
        if (ImGui::Button("Add Wave") && nWaves < 10) {
            waves.push_back(WaveParams(amplitude, timeConstant, xFrequency, zFrequency, offset));
            nWaves++;
        }
        for(int i = 0; i<nWaves; i++) {
            if (ImGui::Button(("Delete Wave " + std::to_string(i)).c_str())) {
                waves.erase(waves.begin() + i);
                nWaves--;
            };
        }
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window, WaveMesh *wave)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    const auto cameraSpeed = static_cast<float>(2.5 * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    timeSinceUpdated += deltaTime;
    if (timeSinceUpdated > 0.1) {
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && wave->numVertices > 15) {
            wave->numVertices -= 10;
            wave->updateIndices();
            wave->updateVertices();
            timeSinceUpdated = 0.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && wave->numVertices < wave->maxVertices) {
            wave->numVertices += 10;
            wave->updateIndices();
            wave->updateVertices();
            timeSinceUpdated = 0.0f;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    if (glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS)  {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
}

void mouse_callback(GLFWwindow* window, double xPosIn, double yPosIn)
{
    if (!glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        firstMouse = true;
        return;
    }

    const auto xPos = static_cast<float>(xPosIn);
    const auto yPos = static_cast<float>(yPosIn);

    if (firstMouse)
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    float xOffset = xPos - lastX;
    float yOffset = lastY - yPos; // reversed since y-coordinates go from bottom to top
    lastX = xPos;
    lastY = yPos;

    constexpr float sensitivity = 0.3f; // change this value to your liking
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    yaw += xOffset;
    pitch += yOffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = normalize(front);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    if (unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0))
    {
        GLenum format = 0;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(format), width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}