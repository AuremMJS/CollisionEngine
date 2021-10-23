#include "OctreeNode.h"

OctreeNode::OctreeNode()
{
	subNodes = nullptr;
	hasMeshVertex = false;
}

OctreeNode::OctreeNode(Vec3 pos, Vec3 size, int depth)
{
	this->depth = depth;
	this->position = pos;
	this->size = size;
	minPosition = position - (size * 0.5);
	maxPosition = position + (size * 0.5);

	subNodes = nullptr;
	hasMeshVertex = false;
}

void OctreeNode::GetVertices(std::vector<Vec3> &vertices)
{
	vertices.push_back(Vec3(position.x - size.x / 2, position.y - size.y / 2, position.z - size.z / 2));
	vertices.push_back(Vec3(position.x - size.x / 2, position.y + size.y / 2, position.z - size.z / 2));
	vertices.push_back(Vec3(position.x - size.x / 2, position.y - size.y / 2, position.z + size.z / 2));
	vertices.push_back(Vec3(position.x - size.x / 2, position.y + size.y / 2, position.z + size.z / 2));
	vertices.push_back(Vec3(position.x + size.x / 2, position.y - size.y / 2, position.z - size.z / 2));
	vertices.push_back(Vec3(position.x + size.x / 2, position.y + size.y / 2, position.z - size.z / 2));
	vertices.push_back(Vec3(position.x + size.x / 2, position.y - size.y / 2, position.z + size.z / 2));
	vertices.push_back(Vec3(position.x + size.x / 2, position.y + size.y / 2, position.z + size.z / 2));
}

OctreeNode::OctreeNode(int depth, Vec3 minPos, Vec3 maxPos)
{
	this->minPosition = minPos;
	this->maxPosition = maxPos;
	this->depth = depth;
	this->position = (maxPos + minPos) * 0.5f;
	this->size = (maxPos - minPos);
	subNodes = nullptr;
	hasMeshVertex = false;
}

void OctreeNode::Subdivide(int depth)
{
	subNodes = new OctreeNode[8];
	for (int i = 0; i < 8; ++i)
	{
		Vec3 newPos = position;
		if ((i & 4) == 4)
		{
			newPos.y += size.y * 0.25f;
		}
		else
		{
			newPos.y -= size.y * 0.25f;
		}

		if ((i & 2) == 2)
		{
			newPos.x += size.x * 0.25f;
		}
		else
		{
			newPos.x -= size.x * 0.25f;
		}

		if ((i & 1) == 1)
		{
			newPos.z += size.z * 0.25f;
		}
		else
		{
			newPos.z -= size.z * 0.25f;
		}

		subNodes[i] = OctreeNode(newPos, size * 0.5f, depth - 1);
		if (depth-1  > 1)
			subNodes[i].Subdivide(depth - 1);
	}
}

void OctreeNode::GetLeafVertices(std::vector<Vec3> &vertices)
{
	if (IsLeaf())
	{
		if (hasMeshVertex)
		{
			GetVertices(vertices);
		}
		return;
	}

	for (int i = 0; i < 8; i++)
	{
		subNodes[i].GetLeafVertices(vertices);
	}
}

void OctreeNode::InitMeshVertexInfo(std::vector<Vec3>& meshVertices)
{
	for (auto meshVertex : meshVertices)
	{
		hasMeshVertex = CheckIfPointInOctree(meshVertex);
		if (hasMeshVertex)
		{
			break;
		}
	}

	if (hasMeshVertex && !IsLeaf())
	{
		for (int i = 0; i < 8; i++)
		{
			subNodes[i].InitMeshVertexInfo(meshVertices);
		}
	}
}

bool OctreeNode::CheckIfPointInOctree(Vec3 point)
{

	if ((minPosition.x > point.x) || (point.x > maxPosition.x)
		|| (minPosition.y > point.y) || (point.y > maxPosition.y)
		|| (minPosition.z > point.z) || (point.z > maxPosition.z))
	{
		return false;
	}
	return true;
}

inline bool OctreeNode::IsPointInLine(float point, float lineStart, float lineEnd)
{
	return (lineStart <= point && point <= lineEnd);
}

inline bool OctreeNode::IsLinesIntersecting(float lineOneStart, float lineOneEnd, float lineTwoStart, float lineTwoEnd)
{
	bool one = IsPointInLine(lineTwoStart, lineOneStart, lineOneEnd);
	bool two = IsPointInLine(lineTwoEnd, lineOneStart, lineOneEnd);
	bool three = IsPointInLine(lineOneStart, lineTwoStart, lineTwoEnd);
	bool four = IsPointInLine(lineOneEnd, lineTwoStart, lineTwoEnd);

	return one || two || three || four;
}

bool OctreeNode::IsLeaf()
{
	return subNodes == nullptr;
}

void OctreeNode::Translate(Vec3 translateVec)
{
	this->position = this->position + translateVec;
	this->minPosition = this->minPosition + translateVec;
	this->maxPosition = this->maxPosition + translateVec;
	if (!IsLeaf())
	{
		for (int i = 0; i < 8; i++)
		{
			subNodes[i].Translate(translateVec);
		}
	}
}

bool OctreeNode::CheckCollision(OctreeNode *otherNode, Vec3 currentNodePosition, Vec3 otherNodePosition, std::vector<OctreeNode*> *collidedLeaves)
{
	/*if (!hasMeshVertex)
	{
		return false;
	}*/
	Vec3 currNodeMinPosition = minPosition;
	Vec3 currNodeMaxPosition = maxPosition;
	Vec3 otherNodeMinPosition = otherNode->minPosition;
	Vec3 otherNodeMaxPosition = otherNode->maxPosition;

	bool isCollidingInXAxis = IsLinesIntersecting(currNodeMinPosition.x, currNodeMaxPosition.x, otherNodeMinPosition.x, otherNodeMaxPosition.x);
	bool isCollidingInYAxis = IsLinesIntersecting(currNodeMinPosition.y, currNodeMaxPosition.y, otherNodeMinPosition.y, otherNodeMaxPosition.y);
	bool isCollidingInZAxis = IsLinesIntersecting(currNodeMinPosition.z, currNodeMaxPosition.z, otherNodeMinPosition.z, otherNodeMaxPosition.z);
	
	bool isColliding = isCollidingInXAxis && isCollidingInYAxis && isCollidingInZAxis;

	if (!isColliding || IsLeaf())
	{
		if (isColliding && IsLeaf())
		{
			collidedLeaves->push_back(this);
		}
		return isColliding;
	}
	isColliding = false;
	for (int i = 0; i < 8; i++)
	{
		isColliding = isColliding || subNodes[i].CheckCollision(otherNode,
			currentNodePosition,otherNodePosition, collidedLeaves);
	}
	return isColliding;
}
