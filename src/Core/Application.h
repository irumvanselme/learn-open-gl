//
// Created by anselme on 07/03/2026.
//

#ifndef OPENGL_TEMPLATE_APPLICATION_H
#define OPENGL_TEMPLATE_APPLICATION_H

#include <GLFW/glfw3.h>

class Application
{
private:
    int width_;
    int height_;
    std::string appTitle_;
    GLFWwindow* window_;

public:
    Application(int width, int height, std::string  appTitle);

    ~Application();

    void NewFrame() const;

    void PrintDebugInfo() const;

    void ClearFrame() const;

    bool IsClosed() const;

    GLFWwindow* GetWindow() const;

private:
    void Destroy() const;
};


#endif //OPENGL_TEMPLATE_APPLICATION_H
