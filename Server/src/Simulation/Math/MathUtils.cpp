#include <cmath>
#include "LKZ/Simulation/Math/MathUtils.h"

//static 

float MathUtils::Distance(Vector2 a, Vector2 b)
{
	float num = a.x - b.x;
	float num2 = a.y - b.y;
	return (float)std::sqrt(num * num + num2 * num2);
}

float MathUtils::Distance(Vector3 a, Vector3 b)
{
    float num = a.x - b.x;
    float num2 = a.y - b.y;
    float num3 = a.z - b.z;
    return (float)std::sqrt(num * num + num2 * num2 + num3 * num3);
}
