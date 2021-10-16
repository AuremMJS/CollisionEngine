#include "Collider.h"
#include <fstream>
#include <string>

Collider::Collider(std::vector<Vec3> mVertices)
{
	vertices = mVertices;
	axisAlignedBoundingBox = new AxisAlignedBoundingBox(mVertices);
}

AxisAlignedBoundingBox* Collider::GetAABB()
{
	return axisAlignedBoundingBox;
}

std::vector<Vec3> ObjParser::ParseObjFile(const char* filename)
{
	std::vector<Vec3> positions;

	std::ifstream inputFile;
	inputFile.open(filename);
	if (!inputFile.is_open())
	{
		throw std::runtime_error("failed to open obj file!");
	}


	std::string tempString = "";
	int i;
	while (true)
	{
		inputFile >> tempString;

		if (tempString == "v")
		{
			float x, y, z;
			inputFile >> x >> y >> z;
			Vec3 v;
			v.x = x;
			v.y = y;
			v.z = z;

			positions.push_back(v);
		}
		else if (inputFile.eof())
			break;
	}

	return positions;
}

void ObjParser::SaveAABBObjFile(AxisAlignedBoundingBox aabb)
{
	std::ofstream outputFile("D:/works/Repos/CollisionEngine_Console/output.obj");

	outputFile << "#AABB obj for Duck.obj" << std::endl;
	outputFile << std::endl;

	for (auto v : aabb.GetVertices())
	{
		outputFile << "v " << v.x << " " << v.y << " " << v.z << std::endl;
	}
	outputFile << std::endl;

	for (auto tc : aabb.GetTexCoords())
	{
		outputFile << "vt " << tc.x << " " << tc.y << " " << tc.z << std::endl;
	}
	outputFile << std::endl;

	std::vector<VertexIndices> indices = aabb.GetIndices();
	for (int i = 0; i < indices.size(); i += 3)
	{
		outputFile << "f " << indices[i].positionIndex <<"/"<< indices[i].texCoordIndex << " " <<
			indices[i + 1].positionIndex << "/" << indices[i + 1].texCoordIndex << " " << 
			indices[i + 2].positionIndex << "/" << indices[i + 2].texCoordIndex << std::endl;
	}
	outputFile << std::endl;
	outputFile.close();
}

ObjParser::ObjParser()
{

}

