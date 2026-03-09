//
// Created by anselme on 07/03/2026.
//

/**
 * This is the Application file created to abstract OpenGL and GLFW functions and make it easier to use in the main file.
 * Documentations used:-
 *
 * 1. docs.gl
 */

#include <iostream>
#include <utility>
#include <GL/glew.h>

/**
 *  Wha is GLFW? -> `Graphics Library Framework`
 *  Framework for managing windows, cursors, OpenGL contexts, and receiving input and events.
 *  It is going to help us deal with the window and OpenGL context.
 */
#include <GLFW/glfw3.h>

#include "Application.h"

#include "Library.h"

Application::Application(const int width, const int height, std::string  appTitle) : width_(width),
                                                                                     height_(height), appTitle_(std::move(appTitle)), window_(nullptr)
{
    /**
     *  This function initializes the GLFW library.
     *  Before most GLFW functions can be used, GLFW must be initialized, and before an application terminates
     *  GLFW should be terminated in order to free any resources allocated during or after initialization.
     **/
    if (!glfwInit())
    {
        std::cout << "[ERROR] Failed to initialize! GLFW Library" << std::endl;
        return;
    }

    std::cout << "[DEBUG] GLFW version: " << glfwGetVersionString() << std::endl;

    // We are going to use OpenGL 3.3, so we need to set the major and minor version of the OpenGL context that we want to create.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // We are going to use the core profile, which means that we are going to use only the modern OpenGL functions and not the deprecated ones.
    // A small sub set of features. ref: https://learnopengl.com/Getting-started/Hello-Window
    // "Telling GLFW we want to use the core-profile means we'll get access to a smaller subset of OpenGL features without backwards-compatible features we no longer need."
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // Create the GLFW window with the specified width and height
    window_ = glfwCreateWindow(width_, height_, appTitle.c_str(), nullptr, nullptr);
    if (!window_)
    {
        std::cout << "Failed to create window!" << std::endl;
        Destroy();
        return;
    }

    glfwMakeContextCurrent(window_);

    // Initialize the GLEW, the Interface to help use the methods from the OpenGL specs, and check if it is initialized successfully or not.
    if (const GLenum err = glewInit(); GLEW_OK != err)
    {
        std::cout << "Failed to initialize GLEW!" << std::endl;
        Destroy();
        return;
    }

    gl_try(glViewport(0, 0, width_, height_));

    // TODO: Register a callback on resize to re-set the view port.
    //       ref: https://learnopengl.com/Getting-started/Hello-Window
}

Application::~Application()
{
    Destroy();
}


void DetectESCKey(GLFWwindow *window)
{
    // This will listen to the window and check if the ESC key is pressed, if it is, it will set the window to close.
    // This will make the IsClosed function return true and the render loop will stop running, after which we can close the application.
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        gl_try(glfwSetWindowShouldClose(window, true));
    }
}

void Application::NewFrame() const
{
    DetectESCKey(window_);
    gl_try(glClear(GL_COLOR_BUFFER_BIT));
}

void Application::PrintDebugInfo() const
{
    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    std::cout << "[DEBUG] Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;
}

void Application::ClearFrame() const
{
    // The glfwSwapBuffers will swap the color buffer (a large 2D buffer that contains color values for each pixel in GLFW's window)
    // that is used to render to during this render iteration and show it as output to the screen.
    gl_try(glfwSwapBuffers(window_));

    // The glfwPollEvents function checks if any events are triggered (like keyboard input or mouse movement events), updates the window state,
    // and calls the corresponding functions (which we can register via callback methods).
    gl_try(glfwPollEvents());
}


bool Application::IsClosed() const
{
    // The glfwWindowShouldClose function checks at the start of each loop iteration if GLFW has been instructed to close.
    // If so, the function returns true and the render loop stops running, after which we can close the application.
    return glfwWindowShouldClose(window_);
}

void Application::Destroy() const
{
    // This function is estimated to terminate the GLFW Library
    gl_try(glfwTerminate());
}

GLFWwindow *Application::GetWindow() const
{
    return window_;
}
