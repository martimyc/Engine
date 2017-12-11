#ifndef SKELETON
#define SKELETON

#include "MathGeoLib\src\Math\float3x4.h"
#include "Globals.h"
#include "Resource.h"

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
			std::string name;
			std::vector<Weight> weights;
		};

		std::vector<Joint> joints;

		void Draw(const GLfloat* opengl_view_matrix) const;
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
};

#endif // !SKELETON

