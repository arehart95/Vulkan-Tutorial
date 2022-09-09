#define GLFW_INCLUDE_VULKAN // INclude GLFW definitions and automatically load Vulkan header
#include <GLFW/glfw3.h>
 
#include <iostream> // for reporting and propogating errors
#include <stdexcept>
#include <cstdlib> // provides EXIT_SUCCESS and EXIT_FAILURE macros

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600; // values for window dimensions

// Application is wrapped into a class
class HelloTriangleApplication {
public:
  void run() {
    initWindow(); // initialize GLFW
    initVulkan(); // initialize Vulkan
    mainLoop(); // Keep window open until closed 
    cleanup(); // clean up resources
    }
  
private:
  GLFWwindow* window; // reference the window
  void initWindow() {
      // the first call in initWindow, tell it not to create an OpenGL context
      glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // initialize GLFW library
      // disable resizing windows
      glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
      // create the actual window
      window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr); // width, hight, title, specify a monitor, relevant to OpenGL
    }
  
    void initVulkan() {
      
    }
  
    void mainLoop() {
      while(!glfwWindowShouldClose(window)) {
        glfwPollEvents();
      }
      
    }
  
    void cleanup() { // clean up resources by destroying window and terminating GLFW
      glfwDestroyWindow(window);
      
      glfwTerminate();
      
    }
};

int main() {
  HelloTriangleApplication app;
  
  try {
    app.run();
  } catch (const std::exception& e) {
      std::cerr << e.what() << std::endl;
      return EXIT_FAILURE;
  }
  
  return EXIT_SUCCESS;
}
    
       
