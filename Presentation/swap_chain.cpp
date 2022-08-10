/* Vulkan requires an infrastructure that will own the buffers we will render to before
	visualizing them on screen. This is called the swap chain, and it must be created explicitly.
	Image presentation is heavily tied to the window system and the surfaces associated with
	windows, it is not part of the Vulkan core. You have to enable the VK_KHR_swapchain device
	extension after querying for its support. 

*/

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <vector>
#include <cstring>
#include <cstdint> // necessary for uint32_t
#include <limits> // necessary for std::numeric_limits
#include <algorithm> // necessary for std::clamp
#include <cstdlib>
#include <optional>
#include <set>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
	};
// 1. Check if the swapchain extension is supported. Declare a list of required device extensions:
const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
	

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;  
    std::optional<uint32_t> presentFamily; 

    bool isComplete() {
        return graphicsFamily.has_value();
    }
};

// Just checking for swapchain support is not sufficient because it might not be compatible with
// our window surface. It also involves more settings than instance and device creation, so we
// need to query for some more details:
// 1. Basic surface capabilities (min/max number of images in swapchain, height and width of img
// 2. Surface formats (pixel format, color space)
// 3. Available presentation modes
// Use a struct to pass these details around once they have been queried

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentMode;
};

class HelloTriangleApplication {
public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    GLFWwindow* window;

    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
   
    VkSurfaceKHR surface;

	VkDevice device;
	
	VkQueue graphicsQueue;
    VkQueue presentQueue;

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	
	VkSwapChainKHR swapChain;
	std::vector<VkImage> swapChainImage;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

    void initWindow() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    }

    void initVulkan() {
        createInstance();
        setupDebugMessenger();
        createSurface();
        pickPhysicalDevice();
		createLogicalDevice();
		createSwapChain(); // add to initVulkan after createLogicalDevice
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }

    void cleanup() {
		vkDestroySwapchainKHR(device, swapChain, nullptr);
        vkDestroyDevice(device, nullptr);
        if (enableValidationLayers) {
            DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        }
        vkDestroySurfaceKHR(instance, surface, nullptr);

        vkDestroyInstance(instance, nullptr);
        glfwDestroyWindow(window);

        glfwTerminate();
    }

    void createInstance() {
        if (enableValidationLayers && !checkValidationLayerSupport()) {
            throw std::runtime_error("validation layers requested, but not available!");
        }

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        auto extensions = getRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();

            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
        } else {
            createInfo.enabledLayerCount = 0;

            createInfo.pNext = nullptr;
        }

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create instance!");
        }
    }

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
    }

    void setupDebugMessenger() {
        if (!enableValidationLayers) return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        populateDebugMessengerCreateInfo(createInfo);

        if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
            throw std::runtime_error("failed to set up debug messenger!");
        }
    }
    
    void createSurface() {
      
        if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create a window surface!");
        }
    }
    

    void pickPhysicalDevice() {
      
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

        if (deviceCount == 0) {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        } 
      
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
      
     
        for (const auto& device : devices) {
            if (isDeviceSuitable(device)) {
                physicalDevice = device;
                break;
            }
        }

        if (physicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
    }
  
	void createLogicalDevice() {
	
		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        atd::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};
		
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
		queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
		
		
		float queuePriority = 1.0f;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		
		VkPhysicalDeviceFeatures deviceFeatures{};
		
	
		
		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        
		
		createInfo.pQueueCreateInfos = &queueCreateInfo;
		createInfo.queueCreateInfoCount = 1;
		createInfo.pEnabledFeatures = &deviceFeatures;
		
		// 3. Enable the device extensions (VK_KHR_swapchain)
		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();
		
		if (enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		} else {
			createInfo.enabledLayerCount = 0;
		}
	
		if(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
			throw std::runtime_error("failed to create a logical device!");
		}
		
		vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
 
        vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
	}
	
	void createSwapChain() { 
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);
		
		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);
		
		// Specify the minimum number of images required to function. It is recommended to
		// request at least one more image than the minimum to speed up the driver.
		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		
		// We also want to avoid exceeding the maximum number of images, where 0 is a special
		// value meaning that there is no maximum
		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}
		
		// Creating the swap chain object requires filling in a large structure:
		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface;
		
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		// imageArrayLayers specifies the amount of layers each image consists of and will
		// generally always be 1.
		// imageUsage specifies what knid of operations the images will be used for.
		
		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
		uint32_t queueFamilyIndices [] = {indices.graphicsFamily.value(), indices.presentFamily.value()};
		
		if (indices.graphicsFamily != indices.presentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		} else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0; // optional
			createInfo.pQueueFamilyIndices = nullptr; // optional
		}
		/* Next we need to specify how to handle swap chain images that will be used across
		multiple queue families. That will be the case if the graphics queue family is different
		from the presentation one. There are two ways to handle images that are accessed from
		multiple queues:
		
		1. VK_SHARING_MODE_EXCLUSIVE:
			An image is owned by one queue family at a time and ownership must be explicitly
			transferred before using it in another queue family. This option offers the best
			performace. 
		2. VK_SHARING_MODE_CONCURRENT:
			Images can be used across multiple queue families without explicit ownership transfer.*/
		
		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		// We can specify that a certain transformation should be applied to images in the swap
		// chain if supported, like a 90 degree clockwise rotation. To specify that you do not
		// want a transformation, simply specify the current transformation.
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		// The compositeAlpha field specifies if the alpha channel should be used for blending
		// with other windows in the window system. We almost always want to ignore the alpha
		// channel, so we use VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR.
		
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		// If the clipped member is set to VK_TRUE that means that we don't care about the
		// color of pixels that are obscured, like if another window is in front of them.
		// Generally you will get the best performance by clipping. 
		
		createInfo.oldSwapChain = VK_NULL_HANDLE;
		// For now assume that we only need to create one swap chain and don't need a reference
		// to the old one. 
		
		if (VkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
			throw std::runtime_error("failed to create swap chain!");
		}
		
		vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
		swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());
		
		swapChainImageFormat = surfaceFormat.format;
		swapChainExtent = extent;
	}
		
	// We need to write functions to determine the settings for the best possible swap chain. There are three
	// settings to determine:
	// 1. Surface format (color depth)
	// 2. Presentation mode (conditions for swapping images to the screen)
	// 3. Swap extent (resolution of images in swap chain)
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
		
		for (const auto& availableFormat : availableFormats) { 
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}		
	/* Each VkSurfaceFormatKHR entry contains a format and a colorSpace member. The format member specifies the
	color channel and types. For example, VK_FORMAT_B8G8R8A8_SRGB means that we store the B, G, R, and alpha 
	channels in that order with an 8 bit unsigned integer for a total of 32 bits per pixel. The colorSpace member
	indicates if the SRGB color space is supported or not using the VK_COLOR_SPACE_SRGB_NONLINEAR_KHR flag.
	
	This color space will use SRGB because it is accurate and pretty much standard. We need an SRGB color format
	and a common one to use is VK_FORMAT_B8G8R8A8_SRGB.	*/
		
	/* Presentation Mode: Arguably the most important setting for the swap chain because it represents the
	actual conditions for showing images to the screen. There are four possible modes in Vulkan:
		1. VK_PRESENT_MODE_IMMEDIATE_KHR:
			Images submitted by your application are transferred to the screen right away which may end up
			tearing.
		2. VK_PRESENT_MODE_FIFO_KHR:
			The swap chain is a queue where the display takes an image from the front of the queue when the
			display is refreshed and the program inserts rendered images at the back of the queue. If the 
			queue is full then the program has to wait. The moment the display is refreshe is called the
			vertical blank.
		3. VK_PRESENT_MODE_FIFO_RELAXED_KHR:
			Instead of waiting for the next vertical blank, the image is transferred right away which may
			result in tearing.
		4. VK_PRESENT_MODE_MAILBOX_KHR:
			A variation of the second mode. Instead of blocking the application when the queue is full,
			the images that are queued are simply replaced with the newer ones. This mode can render frames
			as fast as possible while avoiding tearing, resulting in fewer latency issues. This is also called
			triple buffering. 
		
		Only VK_PRESENT_MODE_FIFO_KHR is guaranteed to be available, so we have to write a function that looks
		for the best mode that is available: */
		
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return availablePresentMode; 
			}
		}
		return VK_PRESENT_MODE_FIFO_KHR;
	}
	
	/* Swap Extent: The swap extent is the resolution of the swap chain images and is almost always exactly
	equal to the resolution of the window that we're drawing to in pixels. The range of the possible resolutions
	is defined in the VkSurfaceCapabilities structure. Vulkan tells us to match the resolution of the window
	by setting the width and height in the currentExtent member. However, some window managers allow us to 
	differ here, and this is indicated by setting the width and height in currentExtent to a special value:
	the maxmium value of uint32_t. We'll pick the resolution that best matches the window within the 
	minImageExtent and maxImageExtent bounds but we have to specify the resolution in the correct unit. 
	
	GLFW uses two units when measuring size: pixels and screen coordinates. The resolution we specified with
	{WIDTH, HEIGHT} is measured in screen coordinates, but Vulkan works in pixels, so the swap chain must be
	specified in pixels as well. When using a high DPI display like Apple's Retina display, the screen
	coordinates don't correspond to pixels. Due to the higher pixel density, the resolution of the window
	in pixels will be larger than the resolution in screen coordinates. So if Vullkan doesn't fix the swap
	extent, the original window values can't be used. Instead, we have to use glfwGetFramebufferSize to query
	 the resolution of the window in pixels before matching it against the min and max image extent. */
	
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		} else {
			int width, height;
			glfwGetFramebufferSize(window, &width, &height);
			
			vkExtent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
				};
			
			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilites.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
			
			return actualExtent;
		}
	}
		// The clamp function is used to bound the values of width and height between the allowed
		// minimum extents that are supported by the implementation.
		
	// The function that populates the struct
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) {
		SwapChainSupportDetails details;
		// This function takes the specified VkPhysicalDevice nd VkSurfaceKHR sruface into account
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
		// Query the supported surface formats
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatKHR(device, surface, &formatCount, nullptr);
		
		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
		}
		// Query the supported presentation modes
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModeKHR(device, surface, &presentModeCount, nullptr);
		
		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
		}
		
		return details;
	}
		
  
    bool isDeviceSuitable(VkPhysicalDevice device) {
        QueueFamilyIndices indices = findQueueFamilies(device);
	// 2. Create a new function called checkDeviceExtensionSupport to be called from 
	// isDeviceSuitable as an additional check:
	bool extensionsSupported = checkDeviceExtensionSupport(device);
	    
	bool swapChainAdequate = false;
	if (extensionsSupported) {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}
	    	

      	return indices.isComplete() extensionsSupported && swapChainAdequate;
    
    }
	bool  checkDeviceExtensionSupport(VkPhysicalDevice device) {
		return true;
	}
	
	// This uses a set of strings to represent the unconfirmed required extensions. 
	bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
		
		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
		
		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
		
		for(const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}
		
		return requiredExtensions.empty();
	}
	

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
 
        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }
       
            VkBool32 presentSupport  = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
            
         
            if (presentSupport) {
                indices.presentFamily = i;
            }

            if (indices.isComplete()) {
                break;
            }

            i++;
        }

        return indices;
    }

    std::vector<const char*> getRequiredExtensions() {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (enableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    bool checkValidationLayerSupport() {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

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
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
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
