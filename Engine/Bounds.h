#ifndef BOUNDS
#define BOUNDS

#include "MathGeoLib\src\Geometry\AABB.h"
#include "MathGeoLib\src\Geometry\OBB.h"
#include "MathGeoLib\src\Geometry\Sphere.h"

struct Bounds
{
	Sphere sphere_bounding_box;
	Sphere original_sphere_bounding_box;

	AABB aabb_bounding_box;
	math::vec original_aabb_bb_points[2];

	OBB obb_bounding_box;
	OBB original_obb_bounding_box;

	Bounds();
	Bounds(const Bounds& copy);
};

#endif //BOUNDS