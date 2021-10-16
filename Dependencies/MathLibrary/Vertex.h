#include "Matrix3.h"
#include <vector>
#include <array>

// Vertex
struct Vertex
{
	Vec3 position;
	Vec3 color;
	Vec3 tex;
	Vec3 normal;
};

// Instance Data
struct InstanceData
{
	Vec3 instancePosition;
	Vec3 rotation;
	float scale;
};