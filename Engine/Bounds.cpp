#include "Bounds.h"

Bounds::Bounds()
{
	sphere_bounding_box.SetNegativeInfinity();
	sphere_bounding_box.r = 0.0f;
	original_sphere_bounding_box.SetNegativeInfinity();
	original_sphere_bounding_box.r = 0.0f;
	aabb_bounding_box.SetNegativeInfinity();
	original_aabb_bb_points[0].zero;
	original_aabb_bb_points[1].zero;
	obb_bounding_box.SetNegativeInfinity();
	obb_bounding_box.r = math::vec::zero;
	original_obb_bounding_box.SetNegativeInfinity();
	original_obb_bounding_box.r = math::vec::zero;
}

Bounds::Bounds(const Bounds & copy) : sphere_bounding_box(copy.sphere_bounding_box), original_sphere_bounding_box(copy.original_sphere_bounding_box), aabb_bounding_box(copy.aabb_bounding_box), obb_bounding_box(copy.obb_bounding_box), original_obb_bounding_box(copy.original_obb_bounding_box)
{
	original_aabb_bb_points[0] = copy.original_aabb_bb_points[0];
	original_aabb_bb_points[1] = copy.original_aabb_bb_points[1];
}
