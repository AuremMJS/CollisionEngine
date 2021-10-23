#include "Collider.h"

int main()
{
	const char* filename = "Duck.obj";
	ObjParser objParser;
	std::vector<Vec3> vertices = objParser.ParseObjFile(filename);
	Collider collider("Duck",vertices);
	return 0;
}