#ifndef _UI_GEOMETRY_COLLISION_TEST
#define _UI_GEOMETRY_COLLISION_TEST

#include "UI_Element.h"

//Probablly should be somewere else, here for test only
enum GEOMETRY
{
	GEO_NO_GEOMETRY = 0,
	GEO_SPHERE,
	GEO_CYLINDER,
	GEO_CAPSULE,
	GEO_AABB,
	GEO_OBB,
	GEO_FRUSTUM,
	GEO_PLANE,
	GEO_SEGMENT,
	GEO_RAY,
	GEO_CONVEX_HULL,
	GEO_MESH,
	GEO_TRIANGLE
};

class UI_GeometryCollisionTest : public UI_Element
{
private:
	bool collision;
	float distance;

	//A
	bool a_sphere = false;
	bool a_cylinder = false;
	bool a_capsule = false;
	bool a_aabb = false;
	bool a_obb = false;
	bool a_frustum = false;
	bool a_plane = false;
	bool a_segment = false;
	bool a_ray = false;

	float a_x = 0.0f;
	float a_y = 0.0f;
	float a_z = 0.0f;
	float a_r = 0.0f;
	float a_h = 0.0f;
	float a_x2 = 0.0f;
	float a_y2 = 0.0f;
	float a_z2 = 0.0f;
	float a_x3 = 0.0f;
	float a_y3 = 0.0f;
	float a_z3 = 0.0f;
	float a_x4 = 0.0f;
	float a_y4 = 0.0f;
	float a_z4 = 0.0f;
	float a_x5 = 0.0f;
	float a_y5 = 0.0f;
	float a_z5 = 0.0f;

	//B
	bool b_sphere = false;
	bool b_cylinder = false;
	bool b_capsule = false;
	bool b_aabb = false;
	bool b_obb = false;
	bool b_frustum = false;
	bool b_plane = false;
	bool b_segment = false;
	bool b_ray = false;

	float b_x = 0.0f;
	float b_y = 0.0f;
	float b_z = 0.0f;
	float b_r = 0.0f;
	float b_h = 0.0f;
	float b_x2 = 0.0f;
	float b_y2 = 0.0f;
	float b_z2 = 0.0f;
	float b_x3 = 0.0f;
	float b_y3 = 0.0f;
	float b_z3 = 0.0f;
	float b_x4 = 0.0f;
	float b_y4 = 0.0f;
	float b_z4 = 0.0f;
	float b_x5 = 0.0f;
	float b_y5 = 0.0f;
	float b_z5 = 0.0f;

public:
	UI_GeometryCollisionTest(bool active = true);
	~UI_GeometryCollisionTest();

	bool Update();

	void ButtonsA();
	void ButtonsB();

	void SphereA();
	void CylinderA();
	void CapsuleA();
	void AABBA();
	void OBBA();
	void PlaneA();
	void SegmentA();
	void RayA();

	void SphereB();
	void CylinderB();
	void CapsuleB();
	void AABBB();
	void OBBB();
	void PlaneB();
	void SegmentB();
	void RayB();

	void ResetA();
	void ResetB();

	void Collisions();

	void TwoSphereCollision();
	void SphereCylinderCollision();
	void SphereCapsuleCollision();
	void SphereAABBCollision();
	void SphereOBBCollision();
	void SphereFrustumCollision();
	void SpherePlaneCollision();
	void SphereSegmentCollision();
	void SphereRayCollision();

	void CylinderSphereCollision();
	void TwoCylinderCollision();
	void CylinderCapsuleCollision();
	void CylinderAABBCollision();
	void CylinderOBBCollision();
	void CylinderFrustumCollision();
	void CylinderPlaneCollision();
	void CylinderSegmentCollision();
	void CylinderRayCollision();

	void CapsuleSphereCollision();
	void CapsuleCylinderCollision();
	void TwoCapsuleCollision();
	void CapsuleAABBCollision();
	void CapsuleOBBCollision();
	void CapsuleFrustumCollision();
	void CapsulePlaneCollision();
	void CapsuleSegmentCollision();
	void CapsuleRayCollision();

	void AABBSphereCollision();
	void AABBCylinderCollision();
	void AABBCapsuleCollision();
	void TwoAABBCollision();
	void AABBOBBCollision();
	void AABBFrustumCollision();
	void AABBPlaneCollision();
	void AABBSegmentCollision();
	void AABBRayCollision();

	void OBBSphereCollision();
	void OBBCylinderCollision();
	void OBBCapsuleCollision();
	void OBBAABBCollision();
	void TwoOBBCollision();
	void OBBFrustumCollision();
	void OBBPlaneCollision();
	void OBBSegmentCollision();
	void OBBRayCollision();

	void FrustumSphereCollision();
	void FrustumCylinderCollision();
	void FrustumCapsuleCollision();
	void FrustumAABBCollision();
	void FrustumOBBCollision();
	void TwoFrustumCollision();
	void FrustumPlaneCollision();
	void FrustumSegmentCollision();
	void FrustumRayCollision();

	void PlaneSphereCollision();
	void PlaneCylinderCollision();
	void PlaneCapsuleCollision();
	void PlaneAABBCollision();
	void PlaneOBBCollision();
	void PlaneFrustumCollision();
	void TwoPlaneCollision();
	void PlaneSegmentCollision();
	void PlaneRayCollision();

	void SegmentSphereCollision();
	void SegmentCylinderCollision();
	void SegmentCapsuleCollision();
	void SegmentAABBCollision();
	void SegmentOBBCollision();
	void SegmentFrustumCollision();
	void SegmentPlaneCollision();
	void TwoSegmentCollision();
	void SegmentRayCollision();

	void RaySphereCollision();
	void RayCylinderCollision();
	void RayCapsuleCollision();
	void RayAABBCollision();
	void RayOBBCollision();
	void RayFrustumCollision();
	void RayPlaneCollision();
	void RaySegmentCollision();
	void TwoRayCollision();
};

#endif //_UI_GEOMETRY_COLLISION_TEST
