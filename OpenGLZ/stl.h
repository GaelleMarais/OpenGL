#pragma once

#include <glm/vec3.hpp>

#include <vector>

struct Triangle
{
	glm::vec3 p0, n0, p1, n1, p2, n2;
};

std::vector<Triangle> ReadStl(const char* filename);
