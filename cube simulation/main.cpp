#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Vertex Shader source code
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

// Fragment Shader source code
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(0.8f, 0.3f, 0.02f, 1.0f);\n"
"}\n\0";

// Callback to resize the OpenGL viewport when the window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height); // Set the viewport to the window size
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Set GLFW context hints (OpenGL version 3.3, core profile)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a windowed OpenGL context
    GLFWwindow* window = glfwCreateWindow(800, 600, "Cube Sim", NULL, NULL);
    if (!window) {
        std::cout << "Could not create GLFW window" << std::endl;
        glfwTerminate();  // Clean up and exit if window creation fails
        return -1;
    }
    glfwMakeContextCurrent(window); // Make the window the current OpenGL context

    // Load OpenGL function pointers using GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Set the viewport size when the window is resized
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Set the initial viewport
    glViewport(0, 0, 800, 600);

    // Define the vertices of the triangle
    GLfloat vertices[] = {
        -0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,  // Bottom-left vertex
         0.5f, -0.5f * float(sqrt(3)) / 3, 0.0f,  // Bottom-right vertex
         0.0f,  0.5f * float(sqrt(3)) * 2 / 3, 0.0f,  // Top vertex
    };

    // Create and compile the Vertex Shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Create and compile the Fragment Shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Create a Shader Program to link vertex and fragment shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Delete the shaders now that they've been linked into the program
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Generate Vertex Array Object (VAO) and Vertex Buffer Object (VBO)
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Bind the VAO (stores configuration of vertex input attributes)
    glBindVertexArray(VAO);

    // Bind the VBO (holds actual vertex data)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Copy vertex data into the VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Define how the vertex data is laid out in memory
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0); // Enable the attribute at location 0 (matches `layout(location=0)` in the vertex shader)

    // Unbind VBO and VAO to avoid accidental modification
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        // Set the background color (dark gray)
        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Use the shader program and draw the triangle
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3); // Draw the triangle using the vertices

        glfwSwapBuffers(window); // Display the rendered frame
        glfwPollEvents();        // Process user input and window events
    }

    // Clean up allocated resources
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // Destroy the window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0; // Exit the program
}
