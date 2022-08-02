#define GLFW_INCLUDE_VULKAN // INclude GLFW definitions and automatically load Vulkan header
#include <GLFW/glfw3.h>
 
#include <iostream> // for reporting and propogating errors
#include <stdexcept>
#include <cstdlib> // provides EXIT_SUCCESS and EXIT_FAILURE macros
#include <vector>
#include <cstring>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600; // values for window dimensions

// Add two config variables to specify the layers to enable and whether to enable them or not
const std::vector<const char*> validationLayers = {
  	"VK_LAYER_KHRONOS_validation"
	}; 

#ifdef NDEBUG
  	const bool enableValidationLayers = false;
#else
  	const bool enableValidationLayers = true;
#endif


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
  	VkInstance instance; // data member to hold the handle to the instance
  
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
		/* The first thing needed to initialize the Vulkan library is to create an instance.
		It is the connection between the application and the library, and needs specific details. */
      	vkcreateInstance(); 
      
    }
  
    void mainLoop() {
      	while(!glfwWindowShouldClose(window)) {
        	glfwPollEvents();
      	}
      
    }
  
 	void cleanup() { // clean up resources by destroying window and terminating GLFW
  		vkDestroyInstance(instance, nullptr); 
  
  		glfwDestroyWindow(window);
  
  		glfwTerminate();
      
    }

	void createInstance() { // Create an instance by filling in a struct with some info about the application
  		// Use validation layers function
  		if (enableValidationLayers && !checkValidationLayerSupport()) {
   			throw std::runtime_error("validation layers requested, but not available!");
  		}
  
  
  		VKApplicationInfo appInfo{}; // VkApplicationInfo is the struct
  		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  		app.Info.pApplicationName = "Hello Triangle";
  		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  		appInfo.pEngineName = "No Engine";
  		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  		appInfo.apiVersion = VK_API_VERSION_1_0;
  
  		VkInstanceCreateInfo createInfo{};
  		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  		createInfo.pApplicationInfo = &appInfo;
  		// Include validation names if they are enabled
  		if (enableValidationLayers) {
    		createInfo.enableLayerCount = static_cast<uint32_t>(validationLayers.size());
    		createInfo.ppEnableLayerNames = validationLayers.data();
  		} else {
    		createInfo.enabledLayerCount = 0;
  		}
  
  		createInfo.enabledExtensionCount = glfwExtensionCount;
  		createInfo.ppEnabledExtensionNames = glfwExtension;
  
  		// Determine the global validation layers to enable
  		createInfo.enabledLayerCount = 0;
  
  		// Issue the vkCreateInstance call
  		VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
  
  		if(vkCreateInstance(&createInfo, nullptr, &instance != VK_SUCCESS)) {
    		throw std::runtime_error("failed to create instance!");
  		}
 	 
 		// Checking for extension support
 		// Allocate an array to hold  the extension details. Request the number of extensions by leaving the
 		// Latter parameter empty
 		uint32_t extensionCount = 0;
 		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
 
 		// Allocate an array to hold the extension details (#include <vector>)
 		std::vector<VkExtensionProperties> extensions(extensionCount);
 
 		// Query the extension details
 		vkEnumerateInstanceExtensionProperties(nullptr, &glfwextensionCount, extensions.data());
 
 		// Each VkExtensionProperties struct contains the name and version of an extensions.
 		// List them with a simple loop
 		std::cout << "Available extensions:\n";
		
		for (const auto& extension : extensions) {
  		std::cout << '\t' << extension.extensionName << '\n';
 		}
 
  		// If the check was successful there should not be an error. 
  
  		// Global extensions to interface with the window system
 		std::vector<const char*> getRequiredExtensions() { 
  		uint32_t glfwExtensionCount = 0;
  		const char** glfwExtensions;
  
  		// Return the needed extensions and pass to the struct
  		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
  
  		std::vector <const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
  
  		if (enableValidationLayers) {
	   		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  		}
  
  		return extensions;
 	}
 
 	// Add a new function called checkValidationLayerSupport that
	// checks if all requested layers are available. 
	bool checkValidationLayerSupport() {
  		uint32_t layerCount;
  		// List all available layers
  		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
  
  		std::vector<VkLayerProperties> availableLayers(layerCount);
  		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
  
  		return false;
	}

	// Check if all the layers in validationLayers exist in the availableLayers list
	for (const char* layerName : validationLayers) {
  		bool layerFound = false;
  
  		for (const auto& layerProperties : availableLayers) {
    		if (strcmp(layerName, layerProperties.layerName) == 0) {
      		layerFound = true;
      		break;
    	}
  }
  
  	if (!layerFound) {
    	return false;
  	}
}

return true;
  
  createInfo.enabledExtensionCount = glfwExtensionCount;
  createInfo.ppEnabledExtensionNames = glfwExtension;
  
  // Determine the global validation layers to enable
  createInfo.enabledLayerCount = 0;
  
  // Issue the vkCreateInstance call
  VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
  
  if(vkCreateInstance(&createInfo, nullptr, &instance != VK_SUCCESS)) {
    throw std::runtime_error("failed to create instance!");
  }
  
 	// Checking for extension support
 	// Allocate an array to hold  the extension details. Request the number of extensions by leaving the
 	// Latter parameter empty
 	uint32_t extensionCount = 0;
 	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
 
 	// Allocate an array to hold the extension details (#include <vector>)
 	std::vector<VkExtensionProperties> extensions(extensionCount);
 
 	// Query the extension details
 	vkEnumerateInstanceExtensionProperties(nullptr, &glfwextensionCount, extensions.data());
 
	// Each VkExtensionProperties struct contains the name and version of an extensions.
 	// List them with a simple loop
 	std::cout << "Available extensions:\n";
 	for (const auto& extension : extensions) {
  		std::cout << '\t' << extension.extensionName << '\n';
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
