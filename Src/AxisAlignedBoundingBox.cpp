#include "AxisAlignedBoundingBox.h"

AxisAlignedBoundingBox::AxisAlignedBoundingBox(std::vector<Vec3> mVertices, int octreeDepth)
{
	Construct(mVertices);
	octree = new OctreeNode(octreeDepth, minVertex, maxVertex);
	octree->Subdivide(octreeDepth);
	octree->InitMeshVertexInfo(mVertices);
  	octree->GetLeafVertices(octreeVertices);
	InitOctreeIndices();
	collidedLeafNodes = new std::vector<OctreeNode*>;
}

void AxisAlignedBoundingBox::Construct(std::vector<Vec3> mVertices)
{
	FindMinMaxVertices(mVertices);

	InitAABBVertices();

	InitAABBTexCoords();

	DrawSides();
}


void AxisAlignedBoundingBox::FindMinMaxVertices(std::vector<Vec3> mVertices)
{
	float minX = mVertices[0].x;
	float maxX = mVertices[0].x;
	float minY = mVertices[0].y;
	float maxY = mVertices[0].y;
	float minZ = mVertices[0].z;
	float maxZ = mVertices[0].z;

	for (auto vertex : mVertices)
	{
		minX = (vertex.x < minX) ? vertex.x : minX;
		maxX = (vertex.x > maxX) ? vertex.x : maxX;
		minY = (vertex.y < minY) ? vertex.y : minY;
		maxY = (vertex.y > maxY) ? vertex.y : maxY;
		minZ = (vertex.z < minZ) ? vertex.z : minZ;
		maxZ = (vertex.z > maxZ) ? vertex.z : maxZ;
	}

	minVertex = Vec3(minX, minY, minZ);
	maxVertex = Vec3(maxX, maxY, maxZ);
}

void AxisAlignedBoundingBox::InitAABBVertices()
{
	// Vertex 0
	vertices.push_back(Vec3(minVertex.x, minVertex.y, minVertex.z));

	// Vertex 1
	vertices.push_back(Vec3(minVertex.x, minVertex.y, maxVertex.z));

	// Vertex 2
	vertices.push_back(Vec3(minVertex.x, maxVertex.y, minVertex.z));

	// Vertex 3
	vertices.push_back(Vec3(minVertex.x, maxVertex.y, maxVertex.z));

	// Vertex 4
	vertices.push_back(Vec3(maxVertex.x, minVertex.y, minVertex.z));

	// Vertex 5
	vertices.push_back(Vec3(maxVertex.x, minVertex.y, maxVertex.z));

	// Vertex 6
	vertices.push_back(Vec3(maxVertex.x, maxVertex.y, minVertex.z));

	// Vertex 7
	vertices.push_back(Vec3(maxVertex.x, maxVertex.y, maxVertex.z));
}

void AxisAlignedBoundingBox::InitAABBTexCoords()
{
	texCoords.push_back(Vec3(0, 0, 0));

	texCoords.push_back(Vec3(1, 0, 0));

	texCoords.push_back(Vec3(0, 1, 0));

	texCoords.push_back(Vec3(1, 1, 0));
}

void AxisAlignedBoundingBox::DrawSides()
{
	// Left Side
	DrawSide({ 1, 2, 0, 3 });

	// Right Side
	DrawSide({ 5, 6, 4, 7 });

	// Top Side
	DrawSide({ 6, 3, 2, 7 });

	// Bottom Side
	DrawSide({ 4, 1, 0, 5 });

	// Front Side
	DrawSide({ 4, 2, 0, 6 });

	// Back Side
	DrawSide({ 5, 3, 1, 7 });
}

void AxisAlignedBoundingBox::DrawSide(std::initializer_list <int> positions)
{
	int positionsLength = positions.size();
	if (positionsLength != 4)
	{
		return;
	}

	VertexIndices i0;
	i0.positionIndex = *(positions.begin() + 0);
	i0.texCoordIndex = 1;

	VertexIndices i1;
	i1.positionIndex = *(positions.begin() + 1);
	i1.texCoordIndex = 2;

	VertexIndices i2;
	i2.positionIndex = *(positions.begin() + 2);
	i2.texCoordIndex = 0;

	VertexIndices i3;
	i3.positionIndex = *(positions.begin() + 3);
	i3.texCoordIndex = 3;

	indices.push_back(i0);
	indices.push_back(i1);
	indices.push_back(i2);

	indices.push_back(i0);
	indices.push_back(i1);
	indices.push_back(i3);
}

std::vector<Vec3> AxisAlignedBoundingBox::GetVertices()
{
	return vertices;
}

std::vector<Vec3> AxisAlignedBoundingBox::GetTexCoords()
{
	return texCoords;
}

std::vector<VertexIndices> AxisAlignedBoundingBox::GetIndices()
{
	return indices;
}

std::vector<Vec3> AxisAlignedBoundingBox::GetOctreeVertices()
{
	return octreeVertices;
}

std::vector<VertexIndices> AxisAlignedBoundingBox::GetOctreeIndices()
{
	return octreeIndices;
}

bool AxisAlignedBoundingBox::CheckCollision(AxisAlignedBoundingBox *otherAABB)
{
	bool isCollided = false;
	OctreeNode* otherOctree = otherAABB->octree;
	collidedLeafNodes->clear();
	//if (octree->CheckCollision(otherOctree, position, otherAABB->position, collidedLeafNodes))
	//{
	//	std::vector<OctreeNode*> otherTreeCollidedLeafNodes;
	//	if (otherOctree->CheckCollision(octree,
	//		otherAABB->position, position, otherTreeCollidedLeafNodes))
	//	{
	//		return true;
	//	}
	//
	//}


	if (octree->CheckCollision(otherOctree, position, otherAABB->position, collidedLeafNodes))
	{
		isCollided = true;
	/*	std::vector<OctreeNode*> otherTreeCollidedLeafNodes;
		for (int i = 0; i < collidedLeafNodes.size(); i++)
		{
			isCollided = isCollided ||
				otherOctree->CheckCollision(collidedLeafNodes[i],
					otherAABB->position, position, &otherTreeCollidedLeafNodes);
			if (isCollided)
			{
				return isCollided;
			}
		}*/
	}
	return isCollided;
}

void AxisAlignedBoundingBox::Translate(Vec3 translateVec)
{
	this->position = this->position + translateVec;
	octree->Translate(translateVec);
}

Vec3 AxisAlignedBoundingBox::GetPosition()
{
	return position;
}

void AxisAlignedBoundingBox::InitOctreeIndices()
{
	for (int i = 0; i < octreeVertices.size(); i+=8)
	{
		InitOctreeNodeIndices(i);
	}
}

void AxisAlignedBoundingBox::InitOctreeNodeIndices(int startVertex)
{
	// Left Side
	DrawOctreeSide(startVertex,{ 1, 2, 0, 3 });

	// Right Side
	DrawOctreeSide(startVertex, { 5, 6, 4, 7 });

	// Top Side
	DrawOctreeSide(startVertex, { 6, 3, 2, 7 });

	// Bottom Side
	DrawOctreeSide(startVertex, { 4, 1, 0, 5 });

	// Front Side
	DrawOctreeSide(startVertex, { 4, 2, 0, 6 });

	// Back Side
	DrawOctreeSide(startVertex, { 5, 3, 1, 7 });
}

void AxisAlignedBoundingBox::DrawOctreeSide(int startVertex, std::initializer_list<int> positions)
{
	int positionsLength = positions.size();
	if (positionsLength != 4)
	{
		return;
	}

	VertexIndices i0;
	i0.positionIndex = startVertex + *(positions.begin() + 0);
	i0.texCoordIndex = 1;

	VertexIndices i1;
	i1.positionIndex = startVertex + *(positions.begin() + 1);
	i1.texCoordIndex = 2;

	VertexIndices i2;
	i2.positionIndex = startVertex + *(positions.begin() + 2);
	i2.texCoordIndex = 0;

	VertexIndices i3;
	i3.positionIndex = startVertex + *(positions.begin() + 3);
	i3.texCoordIndex = 3;

	octreeIndices.push_back(i0);
	octreeIndices.push_back(i1);
	octreeIndices.push_back(i2);

	octreeIndices.push_back(i0);
	octreeIndices.push_back(i1);
	octreeIndices.push_back(i3);
}
