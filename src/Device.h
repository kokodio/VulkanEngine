#pragma once

#include <fstream>
#include <optional>
#include <set>

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

static SwapChainSupportDetails QuerySwapChainSupport(const VkPhysicalDevice &device, const VkSurfaceKHR &surface) {
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    uint32_t presentModesCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModesCount,nullptr);

    if (presentModesCount != 0) {
        details.presentModes.resize(presentModesCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModesCount, details.presentModes.data());
    }

    return details;
}

static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &formats) {
    for (const auto &format : formats) {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return format;
        }
    }

    return formats[0];
}

static VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    for (const auto &availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

static VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, SDL_Window* window) {
    if (capabilities.currentExtent.height != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    else {
        int width, height;
        SDL_GetWindowSizeInPixels(window, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height),
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
    return capabilities.currentExtent;
}

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    [[nodiscard]] bool isComplete() const {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

static constexpr std::array deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

static QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice &physical_device, const VkSurfaceKHR &surface) {
    QueueFamilyIndices indices;

    uint32_t graphicsFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &graphicsFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(graphicsFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &graphicsFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto &queueFamily: queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface, &presentSupport);

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

static bool CheckDeviceExtensionSupport(const VkPhysicalDevice &device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto &extension: availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

static bool IsDeviceSuitable(const VkPhysicalDevice &physical_device, const VkSurfaceKHR &surface) {
    const QueueFamilyIndices indices = FindQueueFamilies(physical_device, surface);
    const bool extensionsSupported = CheckDeviceExtensionSupport(physical_device);

    bool swapChainAdequate = false;
    if (extensionsSupported) {
        const SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(physical_device, surface);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }
    return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

static std::vector<char> ReadFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t size = file.tellg();
    std::vector<char> buffer(size);

    file.seekg(0);
    file.read(buffer.data(), size);

    file.close();
    return buffer;
}
