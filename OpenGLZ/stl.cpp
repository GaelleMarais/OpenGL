#pragma once
#include "stl.h"
#include "objloader.hpp"
#include <fstream>

std::vector<Triangle> ReadStl(const char* filename, const char* uvfilename)
{
	std::ifstream file(filename, std::ios::in | std::ios::binary);
	if (file.is_open())
	{
		// skip header
		file.seekg(80);

		unsigned triCount;
		file.read((char*)&triCount, 4);

		std::vector<Triangle> tris;
		tris.reserve(triCount);

		for (unsigned i = 0; i < triCount; ++i)
		{
			glm::vec3 normal, p0, p1, p2;

			glm::vec2 uv{ i % 2,i % 2 };

			file.read((char*)&normal, sizeof(glm::vec3));
			file.read((char*)&p0, sizeof(glm::vec3));
			file.read((char*)&p1, sizeof(glm::vec3));
			file.read((char*)&p2, sizeof(glm::vec3));

			// skip attribute
			file.seekg(2, std::ios_base::cur);

			tris.push_back({ p0, normal, glm::vec2{0,0},p1, normal,glm::vec2{0,1}, p2, normal, glm::vec2{1,0} });
		}

		return tris;
	}
	else
	{
		return {};
	}
}


std::vector<Triangle> ReadObj(const char* filename)
{
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uv;
	std::vector<glm::vec3> normals;


	loadOBJ(filename, vertices, uv, normals);

	std::vector<Triangle> tris;

	tris.reserve(vertices.size());

	for (int i = 0; i < vertices.size() / 3; i++)
	{
		tris.push_back({ vertices[3 * i], normals[i], uv[3 * i], vertices[3 * i + 1], normals[i], uv[3 * i + 1], vertices[3 * i + 2], normals[i], uv[3 * i + 2] });
	}

	return tris;

}
