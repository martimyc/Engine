#ifndef SKELETON
#define SKELETON

#include "MathGeoLib\src\Math\float4x4.h"
#include "MathGeoLib\src\Geometry\Sphere.h"
#include "Globals.h"
#include "Resource.h"

#define JOINT_SPHERE_RADIUS 0.5f

typedef float GLfloat;

class Skeleton : public Resource
{
private:

	struct Rigg
	{
		struct Joint
		{
			struct Weight
			{
				unsigned int vertex_id;
				float influence;
			};

			float4x4 inverse_bind_pose_transform;
			float4x4 current_inverse_transform;
			Sphere sphere;
			std::string name;
			std::vector<Weight> weights;
			std::vector<Joint> child_joints;

			//mayebe better to work with world coordinates and not mesh

			Joint();

			void UpdateSpherePos();

			void Draw(const float4x4& mesh_global_transform) const;
			void DrawBindPos(const float4x4& mesh_global_transform) const;
		};

		Joint root_joint;

		void Draw(const float4x4& mesh_global_transform) const;
		void DrawBindPos(const float4x4& mesh_global_transform) const;

		void UpdateJointSpheres();
	};

	Rigg* skeleton;

	friend class SkeletonImporter;

public:
	Skeleton(const std::string& name, const UID& uid);
	Skeleton(const std::string& name, Rigg* source);
	~Skeleton();

private:

public:

	void SetRigg(Rigg* new_rigg);

	bool Inspector();

	bool IsLoaded() const;

	void Draw(const float4x4& mesh_global_transform) const;
	void DrawBindPos(const float4x4& mesh_global_transform) const;

	void UpdateJointSpheres();
};

#endif // !SKELETON

