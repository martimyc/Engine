#ifndef SKELETON
#define SKELETON

#include "MathGeoLib\src\Math\float3x4.h"
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

			float3x4 inverse_bind_pose_transform;
			float3x4 current_transform;
			Sphere sphere;
			std::string name;
			std::vector<Weight> weights;
			std::vector<Joint> child_joints;

			//imgui
			float3 position;
			float3 angles;
			float3 scaling;

			//maybe better to work with world coordinates and not mesh

			Joint();

			void SetTransform(const float3x4& new_transform);

			bool Inspector();
			void Hirarchy(Joint** selected);

			void UpdateSpherePos();
			void UpdateTransform(const float3x4& mesh_world_transform);
			void UpdateChildTransform(const float3x4 & old_parent_inverse, const float3x4 & new_parent);

			void Draw(const float3x4& mesh_global_transform, const Joint* selected) const;
			void DrawBindPos(const float3x4& mesh_global_transform) const;

			void SetWorldPositions(const float3x4& mesh_world_transform);

			void ChangeTransforms(std::vector<std::pair<std::string, float3x4>>& joint_transforms);
		};

		Joint root_joint;

		//inspector
		Joint* selected_joint;
		bool skeleton_hirarchy;

		Rigg();
		~Rigg();

		bool Inspector();

		void Draw(const float3x4& mesh_global_transform) const;
		void DrawBindPos(const float3x4& mesh_global_transform) const;

		void UpdateJointTransforms(const float3x4& mesh_world_transform);
		void UpdateJointSpheres();

		void SetWorldPositions(const float3x4& mesh_world_transform);

		void ChangeJointTransforms(std::vector<std::pair<std::string, float3x4>>& joint_transforms);
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

	void Draw(const float3x4& mesh_global_transform) const;
	void DrawBindPos(const float3x4& mesh_global_transform) const;

	void UpdateJointTransforms(const float3x4& mesh_world_transform);
	void UpdateJointSpheres();

	void SetWorldPositions(const float3x4& mesh_world_transform);

	void ChangeJointTransforms(std::vector<std::pair<std::string, float3x4>>& joint_transforms);
};

#endif // !SKELETON

