//
// Created by anselme on 10/03/2026.
//
#define GLM_ENABLE_EXPERIMENTAL
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "glm/gtx/string_cast.hpp"

int main()
{
    {
        using namespace glm;
        constexpr vec3 input_vector(1.0f, 0.0f, 0.0f);
        std::cout << "Input Vector: " << to_string(input_vector) << std::endl;

        constexpr vec3 translation_vector(1.f, 1.f, 0.f);
        std::cout << "Translation Vector: " << to_string(translation_vector) << std::endl;

        // First step is to create a translation matrix using translate
        constexpr mat4 translation_matrix = translate(mat4(1.0f), translation_vector);
        std::cout << "Translation Matrix: " << to_string(translation_matrix) << std::endl;

        // Then we need to convert the input vector to homogeneous coordinates (vec4)
        constexpr vec4 homogeneous_input_vector(input_vector, 1.0f); // w
        std::cout << "Homogeneous Input Vector: " << to_string(homogeneous_input_vector) << std::endl;

        // Now we can apply the translation by multiplying the translation matrix with the homogeneous input vector
        constexpr vec4 transformed_vector = translation_matrix * homogeneous_input_vector;

        std::cout << "Translated vector: " << to_string(transformed_vector) << std::endl;
    }
    return 0;
}
