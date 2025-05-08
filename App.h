#pragma once

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan_core.h>
#include "Window.h"


class App {
public:
    const double TARGET_FPS = 60;
    double PerformancePeriod;
    double FrameTime = 1.0 / TARGET_FPS;
    double LastTime = 0;
    double CurrentTime = 0;

    App() : window_("Window", 800, 450, SDL_WINDOW_VULKAN | SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE, 0) {
        PerformancePeriod = 1.0 / static_cast<double>(SDL_GetPerformanceFrequency());
        LastTime = GetTime();
        renderer_ = window_.getRenderer();
        running = true;
        Init();

        // if (!SDL_Vulkan_CreateSurface(window_, instance, NULL, &surface)) {
        //     fprintf(stderr, "failed to create window surface!");
        //     exit(1);
        // }
        SDL_ShowWindow(window_.getWindow());
    }

    ~App() {
        vkDestroyInstance(instance, nullptr);
        SDL_Quit();
    }

    void Run() {
        SDL_Event event;

        while (running) {
            while (SDL_PollEvent(&event)) {
                Event(&event);
            }
            Iterate();

            CurrentTime = GetTime();
            double deltaTime = CurrentTime - LastTime;
            double sleepDuration = FrameTime - deltaTime;

            if (sleepDuration > 0) {
                const auto ms = static_cast<Uint32>(sleepDuration * 1000);
                if (!window_.VsyncEnabled) SDL_Delay(ms);
            }
            LastTime = CurrentTime;

            window_.SetTitle("FPS: %f", 1.0 / deltaTime);
        }
    }

    [[nodiscard]] double GetTime() const {
        return PerformancePeriod * static_cast<double>(SDL_GetPerformanceCounter());
    }

    void Init();

    void Iterate();

    void Event(const SDL_Event *event);

private:
    VkInstance instance;
    Window window_;
    SDL_Renderer *renderer_;
    bool running;
};
