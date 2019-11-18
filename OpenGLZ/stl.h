#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

#include <vector>

struct Triangle
{
	glm::vec3 p0, n0;
	glm::vec2 uv0;

	glm::vec3 p1, n1;
	glm::vec2 uv1;

	glm::vec3 p2, n2;
	glm::vec2 uv2;
};

std::vector<Triangle> ReadStl(const char* filename, const char* uvfilename);
std::vector<Triangle> ReadObj(const char* filename);
