#include "imgui-master\imgui.h"
#include "MathGeoLib-master\src\MathGeoLib.h"
#include "UI_GeometryCollisionTest.h"

UI_GeometryCollisionTest::UI_GeometryCollisionTest(bool active) : UI_Element(UI_GEOMETRY_COLLISION_TEST, active)
{}

UI_GeometryCollisionTest::~UI_GeometryCollisionTest()
{}

bool UI_GeometryCollisionTest::Update()
{
	//A
	ImGui::Begin("A:", &active);

	ButtonsA();

	if(a_sphere)
		SphereA();
	if (a_capsule)
		CapsuleA();
	if (a_aabb)
		AABBA();
	if(a_obb)
		OBBA();
	if(a_plane)
		PlaneA();
	if(a_segment)
		SegmentA();
	if(a_ray)
		RayA();

	ImGui::End();

	//B
	ImGui::Begin("B:", &active);
	
	ButtonsB();

	if (b_sphere)
		SphereB();
	if (b_capsule)
		CapsuleB();
	if (b_aabb)
		AABBB();
	if (b_obb)
		OBBB();
	if (b_plane)
		PlaneB();
	if (b_segment)
		SegmentB();
	if (b_ray)
		RayB();

	ImGui::End();

	//Result
	ImGui::Begin("Geometry Collision Test Result:", &active);

	Collisions();

	ImGui::End();
	return true;
}

void UI_GeometryCollisionTest::ButtonsA()
{
	if (ImGui::Button("Sphere"))
	{
		ResetA();
		a_sphere = true;
	}
	if (ImGui::Button("Capsule"))
	{
		ResetA();
		a_capsule = true;
	}
	if (ImGui::Button("AABB"))
	{
		ResetA();
		a_aabb = true;
	}
	if (ImGui::Button("OBB"))
	{
		ResetA();
		a_obb = true;
	}
	if (ImGui::Button("Plane"))
	{
		ResetA();
		a_plane = true;
	}
	if (ImGui::Button("Segment"))
	{
		ResetA();
		a_segment = true;
	}
	if (ImGui::Button("Ray"))
	{
		ResetA();
		a_ray = true;
	}
}

void UI_GeometryCollisionTest::ButtonsB()
{
	if (ImGui::Button("Sphere"))
	{
		ResetB();
		b_sphere = true;
	}
	if (ImGui::Button("Capsule"))
	{
		ResetB();
		b_capsule = true;
	}
	if (ImGui::Button("AABB"))
	{
		ResetB();
		b_aabb = true;
	}
	if (ImGui::Button("OBB"))
	{
		ResetB();
		b_obb = true;
	}
	if (ImGui::Button("Plane"))
	{
		ResetB();
		b_plane = true;
	}
	if (ImGui::Button("Segment"))
	{
		ResetB();
		b_segment = true;
	}
	if (ImGui::Button("Ray"))
	{
		ResetB();
		b_ray = true;
	}
}

void UI_GeometryCollisionTest::SphereA()
{
	ImGui::Text("Center position:");
	ImGui::InputFloat("X", &a_x);
	ImGui::InputFloat("Y", &a_y);
	ImGui::InputFloat("Z", &a_z);
	ImGui::InputFloat("Radius", &a_r);
}

void UI_GeometryCollisionTest::CylinderA()
{
	ImGui::Text("Center position:");
	ImGui::InputFloat("X", &a_x);
	ImGui::InputFloat("Y", &a_y);
	ImGui::InputFloat("Z", &a_z);
	ImGui::InputFloat("Radius", &a_r);
	ImGui::InputFloat("Height", &a_h);
}

void UI_GeometryCollisionTest::CapsuleA()
{
	ImGui::Text("Point 1 position:");
	ImGui::InputFloat("X1", &a_x);
	ImGui::InputFloat("Y1", &a_y);
	ImGui::InputFloat("Z1", &a_z);
	ImGui::Text("Radius:");
	ImGui::InputFloat("", &a_r);
	ImGui::Text("Point 2 position:");
	ImGui::InputFloat("X2", &a_x2);
	ImGui::InputFloat("Y2", &a_y2);
	ImGui::InputFloat("Z2", &a_z2);
}

void UI_GeometryCollisionTest::AABBA()
{
	ImGui::Text("Point 1 position:");
	ImGui::InputFloat("X1", &a_x);
	ImGui::InputFloat("Y1", &a_y);
	ImGui::InputFloat("Z1", &a_z);
	ImGui::Text("Point 2 position:");
	ImGui::InputFloat("X2", &a_x2);
	ImGui::InputFloat("Y2", &a_y2);
	ImGui::InputFloat("Z2", &a_z2);
}

void UI_GeometryCollisionTest::OBBA()
{
	ImGui::Text("Position:");
	ImGui::InputFloat("X1", &a_x);
	ImGui::InputFloat("Y1", &a_y);
	ImGui::InputFloat("Z1", &a_z);
	ImGui::Text("R:");
	ImGui::InputFloat("RX", &a_x2);
	ImGui::InputFloat("RY", &a_y2);
	ImGui::InputFloat("RZ", &a_z2);
	ImGui::Text("Axis 1:");
	ImGui::InputFloat("X2", &a_x3);
	ImGui::InputFloat("Y2", &a_y3);
	ImGui::InputFloat("Z2", &a_z3);
	ImGui::Text("Axis 2:");
	ImGui::InputFloat("X2", &a_x4);
	ImGui::InputFloat("Y2", &a_y4);
	ImGui::InputFloat("Z2", &a_z4);
	ImGui::Text("Axis 3:");
	ImGui::InputFloat("X2", &a_x5);
	ImGui::InputFloat("Y2", &a_y5);
	ImGui::InputFloat("Z2", &a_z5);
}

void UI_GeometryCollisionTest::PlaneA()
{
	ImGui::Text("Point:");
	ImGui::InputFloat("X", &a_x);
	ImGui::InputFloat("Y", &a_y);
	ImGui::InputFloat("Z", &a_z);
	ImGui::Text("Normal:");
	ImGui::InputFloat("X", &a_x2);
	ImGui::InputFloat("Y", &a_y2);
	ImGui::InputFloat("Z", &a_z2);
}

void UI_GeometryCollisionTest::SegmentA()
{
	ImGui::Text("Point1:");
	ImGui::InputFloat("X", &a_x);
	ImGui::InputFloat("Y", &a_y);
	ImGui::InputFloat("Z", &a_z);
	ImGui::Text("Point2:");
	ImGui::InputFloat("X", &a_x2);
	ImGui::InputFloat("Y", &a_y2);
	ImGui::InputFloat("Z", &a_z2);
}

void UI_GeometryCollisionTest::RayA()
{
	ImGui::Text("Point:");
	ImGui::InputFloat("X", &a_x);
	ImGui::InputFloat("Y", &a_y);
	ImGui::InputFloat("Z", &a_z);
	ImGui::Text("Direction:");
	ImGui::InputFloat("X", &a_x2);
	ImGui::InputFloat("Y", &a_y2);
	ImGui::InputFloat("Z", &a_z2);
}

void UI_GeometryCollisionTest::SphereB()
{
	ImGui::Text("Center position:");
	ImGui::InputFloat("X", &b_x);
	ImGui::InputFloat("Y", &b_y);
	ImGui::InputFloat("Z", &b_z);
	ImGui::InputFloat("Radius", &b_r);
}

void UI_GeometryCollisionTest::CylinderB()
{
	ImGui::Text("Center position:");
	ImGui::InputFloat("X", &b_x);
	ImGui::InputFloat("Y", &b_y);
	ImGui::InputFloat("Z", &b_z);
	ImGui::InputFloat("Radius", &b_r);
	ImGui::InputFloat("Height", &b_h);
}

void UI_GeometryCollisionTest::CapsuleB()
{
	ImGui::Text("Center position:");
	ImGui::InputFloat("X1", &b_x);
	ImGui::InputFloat("Y1", &b_y);
	ImGui::InputFloat("Z1", &b_z);
	ImGui::InputFloat("Radius", &b_r);
	ImGui::InputFloat("X2", &b_x2);
	ImGui::InputFloat("Y2", &b_y2);
	ImGui::InputFloat("Z2", &b_z2);
}

void UI_GeometryCollisionTest::AABBB()
{
	ImGui::Text("Point 1 position:");
	ImGui::InputFloat("X1", &b_x);
	ImGui::InputFloat("Y1", &b_y);
	ImGui::InputFloat("Z1", &b_z);
	ImGui::Text("Radius:");
	ImGui::InputFloat("", &b_r);
	ImGui::Text("Point 2 position:");
	ImGui::InputFloat("X2", &b_x2);
	ImGui::InputFloat("Y2", &b_y2);
	ImGui::InputFloat("Z2", &b_z2);
}

void UI_GeometryCollisionTest::OBBB()
{
	ImGui::Text("Position:");
	ImGui::InputFloat("X1", &b_x);
	ImGui::InputFloat("Y1", &b_y);
	ImGui::InputFloat("Z1", &b_z);
	ImGui::Text("R:");
	ImGui::InputFloat("RX", &b_x2);
	ImGui::InputFloat("RY", &b_y2);
	ImGui::InputFloat("RZ", &b_z2);
	ImGui::Text("Axis 1:");
	ImGui::InputFloat("X2", &b_x3);
	ImGui::InputFloat("Y2", &b_y3);
	ImGui::InputFloat("Z2", &b_z3);
	ImGui::Text("Axis 2:");
	ImGui::InputFloat("X2", &b_x4);
	ImGui::InputFloat("Y2", &b_y4);
	ImGui::InputFloat("Z2", &b_z4);
	ImGui::Text("Axis 3:");
	ImGui::InputFloat("X2", &b_x5);
	ImGui::InputFloat("Y2", &b_y5);
	ImGui::InputFloat("Z2", &b_z5);
}

void UI_GeometryCollisionTest::PlaneB()
{
	ImGui::Text("Point:");
	ImGui::InputFloat("X", &b_x);
	ImGui::InputFloat("Y", &b_y);
	ImGui::InputFloat("Z", &b_z);
	ImGui::Text("Normal:");
	ImGui::InputFloat("X", &b_x2);
	ImGui::InputFloat("Y", &b_y2);
	ImGui::InputFloat("Z", &b_z2);
}

void UI_GeometryCollisionTest::SegmentB()
{
	ImGui::Text("Point1:");
	ImGui::InputFloat("X", &b_x);
	ImGui::InputFloat("Y", &b_y);
	ImGui::InputFloat("Z", &b_z);
	ImGui::Text("Point2:");
	ImGui::InputFloat("X", &b_x2);
	ImGui::InputFloat("Y", &b_y2);
	ImGui::InputFloat("Z", &b_z2);
}

void UI_GeometryCollisionTest::RayB()
{
	ImGui::Text("Point:");
	ImGui::InputFloat("X", &b_x);
	ImGui::InputFloat("Y", &b_y);
	ImGui::InputFloat("Z", &b_z);
	ImGui::Text("Direction:");
	ImGui::InputFloat("X", &b_x2);
	ImGui::InputFloat("Y", &b_y2);
	ImGui::InputFloat("Z", &b_z2);
}

void UI_GeometryCollisionTest::ResetA()
{
	if (a_sphere)
		a_sphere = false;
	if (a_capsule)
		a_capsule = false;
	if (a_aabb)
		a_aabb = false;
	if (a_obb)
		a_obb = false;
	if (a_plane)
		a_plane = false;
	if (a_segment)
		a_segment = false;
	if (a_ray)
		a_ray = false;
}

void UI_GeometryCollisionTest::ResetB()
{
	if (b_sphere)
		b_sphere = false;
	if (b_capsule)
		b_capsule = false;
	if (b_aabb)
		b_aabb = false;
	if (b_obb)
		b_obb = false;
	if (b_plane)
		b_plane = false;
	if (b_segment)
		b_segment = false;
	if (b_ray)
		b_ray = false;
}

void UI_GeometryCollisionTest::Collisions()
{
	//Sphere
	if (a_sphere && b_sphere)
	{
		TwoSphereCollision();
		return;
	}
	if (a_sphere && b_capsule)
	{
		SphereCapsuleCollision();
		return;
	}
	if (a_sphere && b_aabb)
	{
		SphereAABBCollision();
		return;
	}
	if (a_sphere && b_obb)
	{
		SphereOBBCollision();
		return;
	}
	if (a_sphere && b_plane)
	{
		SpherePlaneCollision();
		return;
	}
	if (a_sphere && b_segment)
	{
		SphereSegmentCollision();
		return;
	}
	if (a_sphere && b_ray)
	{
		SphereRayCollision();
		return;
	}

	//Capsule
	if (a_capsule && b_capsule)
	{
		CapsuleSphereCollision();
		return;
	}
	if (a_capsule && b_capsule)
	{
		TwoCapsuleCollision();
		return;
	}
	if (a_capsule && b_aabb)
	{
		CapsuleAABBCollision();
		return;
	}
	if (a_capsule && b_obb)
	{
		CapsuleOBBCollision();
		return;
	}
	if (a_capsule && b_plane)
	{
		CapsulePlaneCollision();
		return;
	}
	if (a_capsule && b_segment)
	{
		CapsuleSegmentCollision();
		return;
	}
	if (a_capsule && b_ray)
	{
		CapsuleRayCollision();
		return;
	}

	//AABB
	if (a_aabb && b_aabb)
	{
		AABBSphereCollision();
		return;
	}
	if (a_aabb && b_capsule)
	{
		AABBCapsuleCollision();
		return;
	}
	if (a_aabb && b_aabb)
	{
		TwoAABBCollision();
		return;
	}
	if (a_aabb && b_obb)
	{
		AABBOBBCollision();
		return;
	}
	if (a_aabb && b_plane)
	{
		AABBPlaneCollision();
		return;
	}
	if (a_aabb && b_segment)
	{
		AABBSegmentCollision();
		return;
	}
	if (a_aabb && b_ray)
	{
		AABBRayCollision();
		return;
	}

	//OBB
	if (a_obb && b_obb)
	{
		OBBSphereCollision();
		return;
	}
	if (a_obb && b_capsule)
	{
		OBBCapsuleCollision();
		return;
	}
	if (a_obb && b_aabb)
	{
		OBBAABBCollision();
		return;
	}
	if (a_obb && b_obb)
	{
		TwoOBBCollision();
		return;
	}
	if (a_obb && b_plane)
	{
		OBBPlaneCollision();
		return;
	}
	if (a_obb && b_segment)
	{
		OBBSegmentCollision();
		return;
	}
	if (a_obb && b_ray)
	{
		OBBRayCollision();
		return;
	}

	//Plane
	if (a_plane && b_plane)
	{
		PlaneSphereCollision();
		return;
	}
	if (a_plane && b_capsule)
	{
		PlaneCapsuleCollision();
		return;
	}
	if (a_plane && b_aabb)
	{
		PlaneAABBCollision();
		return;
	}
	if (a_plane && b_obb)
	{
		PlaneOBBCollision();
		return;
	}
	if (a_plane && b_plane)
	{
		TwoPlaneCollision();
		return;
	}
	if (a_plane && b_segment)
	{
		PlaneSegmentCollision();
		return;
	}
	if (a_plane && b_ray)
	{
		PlaneRayCollision();
		return;
	}

	//Segment
	if (a_segment && b_segment)
	{
		SegmentSphereCollision(); 
		return;
	}
	if (a_segment && b_capsule)
	{
		SegmentCapsuleCollision();
		return;
	}
	if (a_segment && b_aabb)
	{
		SegmentAABBCollision();
		return;
	}
	if (a_segment && b_obb)
	{
		SegmentOBBCollision();
		return;
	}
	if (a_segment && b_plane)
	{
		SegmentPlaneCollision();
		return;
	}
	if (a_segment && b_segment)
	{
		TwoSegmentCollision();
		return;
	}
	if (a_segment && b_ray)
	{
		SegmentRayCollision();
		return;
	}

	//ray
	if (a_ray && b_ray)
	{
		RaySphereCollision();
		return;
	}
	if (a_ray && b_capsule)
	{
		RayCapsuleCollision();
		return;
	}
	if (a_ray && b_aabb)
	{
		RayAABBCollision();
		return;
	}
	if (a_ray && b_obb)
	{
		RayOBBCollision();
		return;
	}
	if (a_ray && b_plane)
	{
		RayPlaneCollision();
		return;
	}
	if (a_ray && b_segment)
	{
		RaySegmentCollision();
		return;
	}
	if (a_ray && b_ray)
	{
		TwoRayCollision();
		return;
	}
}

void UI_GeometryCollisionTest::TwoSphereCollision()
{
	Sphere a(vec(a_x, a_y, a_z), a_r);
	Sphere b(vec(b_x, b_y, b_z), b_r);

	collision = a.Intersects(b);

	if (!collision)
	{
		distance = a.Distance(b);
		ImGui::Text("Spheres do not collide\nDistance apart: %f", distance);
	}
	else
		ImGui::Text("Spheres do collide");
}

void UI_GeometryCollisionTest::SegmentRayCollision()
{
	ImGui::Text("Not in MathGeoLib");
}

void UI_GeometryCollisionTest::RaySphereCollision()
{
	Ray a(vec(a_x, a_y, a_z), vec(a_x2, a_y2, a_z2));
	Sphere b(vec(b_x, b_y, b_z), b_r);

	collision = a.Intersects(b);

	if (!collision)
	{
		distance = a.Distance(b);
		ImGui::Text("Ray does not collide with sphere\nDistance apart: %f", distance);
	}
	else
		ImGui::Text("Ray does collide with sphere");
}

void UI_GeometryCollisionTest::RayCapsuleCollision()
{
	Ray a(vec(a_x, a_y, a_z), vec(a_x2, a_y2, a_z2));
	Capsule b(vec(b_x, b_y, b_z), vec(b_x2, b_y2, b_z2), b_r);

	collision = a.Intersects(b);

	if (!collision)
	{
		distance = a.Distance(b);
		ImGui::Text("Ray does not collide with capsule\nDistance apart: %f", distance);
	}
	else
		ImGui::Text("Ray does collide with capsule");
}

void UI_GeometryCollisionTest::RayAABBCollision()
{
	Ray a(vec(a_x, a_y, a_z), vec(a_x2, a_y2, a_z2));
	AABB b(vec(b_x, b_y, b_z), vec(b_x2, b_y2, b_z2));

	collision = a.Intersects(b);

	if (!collision)
		ImGui::Text("Ray does not collide with AABB");
	else
		ImGui::Text("Ray does collide with AABB");
}

void UI_GeometryCollisionTest::RayOBBCollision()
{
	Ray a(vec(a_x, a_y, a_z), vec(a_x2, a_y2, a_z2));
	OBB b(vec(b_x, b_y, b_z), vec(b_x2, b_y2, b_z2), vec(b_x3, b_y3, b_z3), vec(b_x4, b_y4, b_z4), vec(b_x5, b_y5, b_z5));

	collision = a.Intersects(b);

	if (!collision)
		ImGui::Text("Ray does not collide with OBB");
	else
		ImGui::Text("Ray does collide with OBB");
}

void UI_GeometryCollisionTest::RayPlaneCollision()
{
	Ray a(vec(a_x, a_y, a_z), vec(a_x2, a_y2, a_z2));
	Plane b(vec(b_x, b_y, b_z), vec(b_x2, b_y2, b_z2));

	collision = a.Intersects(b);

	if (!collision)
		ImGui::Text("Ray does not collide with plain");
	else
		ImGui::Text("Ray collides with plain");
}

void UI_GeometryCollisionTest::RaySegmentCollision()
{
	ImGui::Text("Not in MathGeoLib");
}

void UI_GeometryCollisionTest::TwoRayCollision()
{
	ImGui::Text("Not in MathGeoLib");
}

void UI_GeometryCollisionTest::SegmentSphereCollision()
{
	LineSegment a(vec(a_x, a_y, a_z), vec(a_x2, a_y2, a_z2));
	Sphere b(vec(b_x, b_y, b_z), b_r);

	collision = a.Intersects(b);

	if (!collision)
	{
		distance = a.Distance(b);
		ImGui::Text("Segment does not collide with sphere\nDistance apart: %f", distance);
	}
	else
		ImGui::Text("Segment does collide with sphere");
}

void UI_GeometryCollisionTest::SegmentCapsuleCollision()
{
	LineSegment a(vec(a_x, a_y, a_z), vec(a_x2, a_y2, a_z2));
	Capsule b(vec(b_x, b_y, b_z), vec(b_x2, b_y2, b_z2), b_r);

	collision = a.Intersects(b);

	if (!collision)
	{
		distance = a.Distance(b);
		ImGui::Text("Segment does not collide with capsule\nDistance apart: %f", distance);
	}
	else
		ImGui::Text("Segment does collide with capsule");
}

void UI_GeometryCollisionTest::SegmentAABBCollision()
{
	LineSegment a(vec(a_x, a_y, a_z), vec(a_x2, a_y2, a_z2));
	AABB b(vec(b_x, b_y, b_z), vec(b_x2, b_y2, b_z2));

	collision = a.Intersects(b);

	if (!collision)
		ImGui::Text("Segment does not collide with AABB");
	else
		ImGui::Text("Segment does collide with AABB");
}

void UI_GeometryCollisionTest::SegmentOBBCollision()
{
	LineSegment a(vec(a_x, a_y, a_z), vec(a_x2, a_y2, a_z2));
	OBB b(vec(b_x, b_y, b_z), vec(b_x2, b_y2, b_z2), vec(b_x3, b_y3, b_z3), vec(b_x4, b_y4, b_z4), vec(b_x5, b_y5, b_z5));

	collision = a.Intersects(b);

	if (!collision)
		ImGui::Text("Segment does not collide with OBB");
	else
		ImGui::Text("Segment does collide with OBB");
}

void UI_GeometryCollisionTest::SegmentPlaneCollision()
{
	LineSegment a(vec(a_x, a_y, a_z), vec(a_x2, a_y2, a_z2));
	Plane b(vec(b_x, b_y, b_z), vec(b_x2, b_y2, b_z2));

	collision = a.Intersects(b);

	if (!collision)
	{
		distance = a.Distance(b);
		ImGui::Text("Segment does not collide with Plane\nDistance apart: %f", distance);
	}
	else
		ImGui::Text("Segment does collide with Plane");
}

void UI_GeometryCollisionTest::TwoSegmentCollision()
{
	LineSegment a(vec(a_x, a_y, a_z), vec(a_x2, a_y2, a_z2));
	LineSegment b(vec(b_x, b_y, b_z), vec(b_x2, b_y2, b_z2));

	collision = a.Intersects(b);

	if (!collision)
	{
		distance = a.Distance(b);
		ImGui::Text("Segment does not collide with Plane\nDistance apart: %f", distance);
	}
	else
		ImGui::Text("Segment does collide with Plane");
}
