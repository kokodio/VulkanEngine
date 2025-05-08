#include "App.h"
#include <vector>
#include <vulkan/vulkan.h>

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

bool checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    return false;
}

void App::Init() {
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

    uint32_t sdlExtensionCount = 0;
    const auto sdlExtensions = SDL_Vulkan_GetInstanceExtensions(&sdlExtensionCount);

    createInfo.enabledExtensionCount = sdlExtensionCount;
    createInfo.ppEnabledExtensionNames = sdlExtensions;
    createInfo.enabledLayerCount = 0;

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    SDL_Log("Vulkan extensions available:");

    for (const auto& extension : extensions) {
        SDL_Log("\t %s", extension.extensionName);
    }

    const std::vector validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    // if (!SDL_Vulkan_CreateSurface(window_.getWindow(), instance, NULL, &surface)) {
    //     fprintf(stderr, "failed to create window surface!");
    //     exit(1);
    // }
}

void App::Event(const SDL_Event *event) {
    if (event->type == SDL_EVENT_QUIT) running = false;
    else if (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_ESCAPE) running = false;
}

void App::Iterate() {

}
