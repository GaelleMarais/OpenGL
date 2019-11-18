#include "stl.h"

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
