#pragma once

#include "glm/vec3.hpp"
#include <iostream>

std::ostream& operator<<(std::ostream& out, const glm::i32vec3& val) {
    return out << val.x << " " << val.y << " " << val.z;
}

std::ostream& operator<<(std::ostream& out, const glm::vec3& val) {
    return out << val.x << " " << val.y << " " << val.z;
}

