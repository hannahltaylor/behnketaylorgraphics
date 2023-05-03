/*
    Modified from: https://learnopengl.com/Guest-Articles/2021/Tessellation/Height-map
             and   https://learnopengl.com/Guest-Articles/2021/Tessellation/Tessellation
*/

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb/stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "skybox.h"
#include "shader_t.h"
#include "camera.h"
#include <iostream>
#include <vector>
#include <glm/gtc/noise.hpp>
#include "texture.h"
#include "map.h"
using namespace std;



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int modifiers);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 1800;
const unsigned int SCR_HEIGHT = 1600;
const unsigned int NUM_PATCH_PTS = 4;
const unsigned int REZ = 40;
#define WIDTH 512
#define HEIGHT 512
// Select which noise you want
#define NOISE "DS"
//#define NOISE "Perlin"
//#define NOISE "Simplex"


Camera camera(glm::vec3(0.0f, 0.2f, 0.9f),
              glm::vec3(0.0f, -1.0f, 0.0f), 
              -128.1f, -42.4f);
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

float map(float value, float fromLow, float fromHigh, float toLow, float toHigh) {
    return (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
}

unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    // Load image with stbi load
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        // Bind and generate map
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
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

void loadAndBindTextures(const std::vector<std::string>& texturePaths, unsigned int program_handle)
{
    std::vector<GLuint> textureIDs(texturePaths.size());

    // Generate texture IDs
    glGenTextures(texturePaths.size(), &textureIDs[0]);

    // Load and bind each texture
    for (int i = 0; i < texturePaths.size(); ++i) {
        // Load texture data from file
        int width, height, numChannels;
        unsigned char* data = stbi_load(texturePaths[i].c_str(), &width, &height, &numChannels, 0);

        if (!data) {
            std::cerr << "Failed to load texture from file: " << texturePaths[i] << std::endl;
            continue;
        }

        // Bind texture to texture unit
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textureIDs[i]);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Upload texture data
        GLenum format = (numChannels == 3 ? GL_RGB : GL_RGBA);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Free texture data
        stbi_image_free(data);
    }

    // Set texture sampler uniforms in shader
    for (int i = 0; i < textureIDs.size(); ++i) {
        string temp;
        if(i == 1)
            temp = "rockTexture";
        else if( i == 2)
            temp = "waterTexture";
        else    
            temp = "snowTexture";
        GLuint location = glGetUniformLocation(program_handle, temp.c_str());
        glUniform1i(location, i);
    }
}

void fillAndBindMap(Shader &tessHeightMapShader) {
    int H = HEIGHT;
    int W = WIDTH;
    float heightMap[H][W];

	//Setting the corner values of the terrain to some arbitrary initial
	//values, these act as the basepoint for the terrain to be generated from
    if(NOISE == "DS") {
        const int mapsize = H; 
        const int start = 64;
        Map hM(mapsize+1,mapsize+1);
        hM.setElement(0, 0, start);
        hM.setElement(0, mapsize, start);
        hM.setElement(mapsize, 0, start);
        hM.setElement(mapsize, mapsize, start);
        hM.generate(mapsize + 1, 60, 0.59);
        hM.capData(0, 255);
        hM.smooth(1, 2);
        for (int i = 0; i < H; i++) {
            for (int j = 0; j < W; j++) {
                heightMap[i][j] = hM.elements[i][j];
            }
        }
    } else if(NOISE == "Perlin") {
        float yoff = 0.0f;
        for (int i = 0; i < H; i++) {
            float xoff = 0.0f;
            for (int j = 0; j < W; j++) {
                float noise = map(glm::perlin(glm::vec2(xoff, yoff)), -1,1, -100, 100 );
                heightMap[i][j] = noise;
                xoff += 0.1;
            }
            yoff+=0.1;
        }
    } else {
        float yoff = 0.0f;
        for (int i = 0; i < H; i++) {
            float xoff = 0.0f;
            for (int j = 0; j < W; j++) {
                float noise = map(glm::simplex(glm::vec2(xoff, yoff)), -1,1, -100, 100 );
                heightMap[i][j] = noise;
                xoff += 0.1;
            }
            yoff+=0.1;
        }
    }
    

    GLvoid* data = &(heightMap)[0][0];
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, W, H, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
       // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        tessHeightMapShader.setInt("heightMap", 0);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
}


unsigned int createMesh() {
    std::vector<float> vertices;
    int height = HEIGHT;
    int width = WIDTH;    
    for(unsigned i = 0; i <= REZ-1; i++)
    {
        for(unsigned j = 0; j <= REZ-1; j++)
        {
            vertices.push_back(-width/2.0f + width*i/(float)REZ); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height/2.0f + height*j/(float)REZ); // v.z
            vertices.push_back(i / (float)REZ); // u
            vertices.push_back(j / (float)REZ); // v

            vertices.push_back(-width/2.0f + width*(i+1)/(float)REZ); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height/2.0f + height*j/(float)REZ); // v.z
            vertices.push_back((i+1) / (float)REZ); // u
            vertices.push_back(j / (float)REZ); // v

            vertices.push_back(-width/2.0f + width*i/(float)REZ); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height/2.0f + height*(j+1)/(float)REZ); // v.z
            vertices.push_back(i / (float)REZ); // u
            vertices.push_back((j+1) / (float)REZ); // v

            vertices.push_back(-width/2.0f + width*(i+1)/(float)REZ); // v.x
            vertices.push_back(0.0f); // v.y
            vertices.push_back(-height/2.0f + height*(j+1)/(float)REZ); // v.z
            vertices.push_back((i+1) / (float)REZ); // u
            vertices.push_back((j+1) / (float)REZ); // v
        }
    }
    std::cout << "Loaded " << REZ*REZ << " patches of 4 control points each" << std::endl;
    std::cout << "Processing " << REZ*REZ*4 << " vertices in vertex shader" << std::endl;

    unsigned int terrainVAO, terrainVBO;
    glGenVertexArrays(1, &terrainVAO);
    glBindVertexArray(terrainVAO);

    glGenBuffers(1, &terrainVBO);
    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texCoord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    glPatchParameteri(GL_PATCH_VERTICES, NUM_PATCH_PTS);
    return terrainVAO;
}


int main()
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Procedural Terrain Generation", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    GLint maxTessLevel;
    glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &maxTessLevel);
    std::cout << "Max available tess level: " << maxTessLevel << std::endl;


    glEnable(GL_DEPTH_TEST);

    int width, height, nrChannels;
    width = WIDTH; 
    height = HEIGHT;
    
    // Compile and attach shader
    Shader tessHeightMapShader("height.vs","height.fs", nullptr, "height.tcs", "height.tes");
    
    // Make Height map
    fillAndBindMap(tessHeightMapShader);
    

    // Make Terrain Mesh
    unsigned int terrainVAO = createMesh();

    // Attach Textures
    vector<string> texturePaths;
    texturePaths.push_back("textures/grey-rock-texture-1.jpg");
    texturePaths.push_back("textures/water2.jpg");
    texturePaths.push_back("textures/snow.jpg");
    loadAndBindTextures(texturePaths, tessHeightMapShader.ID);


    // Make and load skybox
    SkyBox skyBox(1000.0f);
    Shader SkyboxShader("skybox.vs","skybox.fs", nullptr, nullptr, nullptr);
    SkyboxShader.use();
    GLuint cubeTex = Texture::loadCubeMap("skybox_images/cube");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);
    SkyboxShader.setInt("SkyBoxTex", 0);

    // Set the matrices in the render loop
    
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(135.0f/255.0f, 206.0f/255.0f, 235.0f/255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        // World Transformation
        glm::mat4 model = glm::mat4(1.0f);

        // Draw skybox
        SkyboxShader.use();
        SkyboxShader.setMat4("projection", projection);
        SkyboxShader.setMat4("view", view);
        SkyboxShader.setMat4("model", model);
        skyBox.render();


        // Draw terrain
        tessHeightMapShader.use();

        // Set Matricies 
        tessHeightMapShader.setMat4("projection", projection);
        tessHeightMapShader.setMat4("view", view);
        tessHeightMapShader.setMat4("model", model);

        // render the terrain
        glBindVertexArray(terrainVAO);
        glDrawArrays(GL_PATCHES, 0, NUM_PATCH_PTS*REZ*REZ);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &terrainVAO);
    //glDeleteBuffers(1, &terrainVBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever a key event occurs, this callback is called
// ---------------------------------------------------------------------------------------------
void key_callback(GLFWwindow* window, int key, int scancode, int action, int modifiers)
{
    int dir = 0;
    if(action == GLFW_REPEAT)
    {
        switch(key)
        {
            case GLFW_KEY_LEFT:
                camera.ProcessKeyboard(LEFT, deltaTime);
                break;
            case GLFW_KEY_UP:
                camera.ProcessKeyboard(FORWARD, deltaTime);
                break;
            case GLFW_KEY_RIGHT:
                camera.ProcessKeyboard(RIGHT, deltaTime);
                break;
            case GLFW_KEY_DOWN:
                camera.ProcessKeyboard(BACKWARD, deltaTime);
                break;
            default:
                break;
        }
    }
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}