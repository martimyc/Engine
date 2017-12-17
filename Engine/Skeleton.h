#ifndef SKELETON
#define SKELETON

#include "MathGeoLib\src\Math\float3x4.h"
#include "MathGeoLib\src\Geometry\Sphere.h"
#include "Globals.h"
#include "Resource.h"

#define JOINT_SPHERE_RADIUS 0.5f

typedef float GLfloat;
class Animation;
class Mesh;
struct MeshSource;

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

			float3x4 offset;
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

			bool Inspector();
			void Hirarchy(Joint** selected);

			void UpdateTransforms(const float3x4& parent_transform, const Joint* selected);

			void Draw(const float3x4& mesh_global_transform, const Joint* selected) const;
			void DrawBindPos(const float3x4& mesh_global_transform) const;

			void SetWorldPositions(const float3x4& mesh_world_transform);

			void ChangeTransforms(Animation* anim, double anim_time, bool interpolation = true);

			void GetVertices(const MeshSource * original, GLfloat * vertices, const float3x4& parent_mesh);
		};

		Joint root_joint;
		float3x4 transform;

		//inspector
		Joint* selected_joint;
		unsigned int num_joints;
		bool skeleton_hirarchy;

		Rigg();
		~Rigg();

		bool Inspector();

		void Draw(const float3x4& mesh_global_transform) const;
		void DrawBindPos(const float3x4& mesh_global_transform) const;

		void UpdateJointTransforms(const float3x4& mesh_world_transform);

		void SetWorldPositions(const float3x4& mesh_world_transform);

		void ChangeJointTransforms(Animation* anim, double anim_time, bool interpolation = true);

		unsigned int GetNumJoints() const;

		void GetVertices(const MeshSource* original, GLfloat* vertices);
	};

	Rigg* skeleton;
	MeshSource* deformable_mesh;

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

	void UnLoad();

	void Draw(const float3x4& mesh_global_transform) const;
	void DrawBindPos(const float3x4& mesh_global_transform) const;

	void UpdateJointTransforms(const float3x4& mesh_world_transform);

	void SetWorldPositions(const float3x4& mesh_world_transform);

	void ChangeJointTransforms(Animation* anim, double anim_time, bool interpolation = true);

	unsigned int GetNumJoints() const;

	void DeformableMesh(const Mesh* mesh);

	void UpdateMesh(const Mesh* original);
};

#endif // !SKELETON

