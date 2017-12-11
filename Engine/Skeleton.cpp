#include "glew\include\GL\glew.h"
#include "MathGeoLib\src\Math\float4x4.h"
#include "Skeleton.h"

Skeleton::Skeleton(const std::string & name, const UID & uid) : Resource(RT_SKELETON, name, uid)
{}

Skeleton::Skeleton(const std::string & name, Rigg * source) : Resource(RT_SKELETON, name), skeleton(source)
{}

Skeleton::~Skeleton()
{
	delete skeleton;
}

void Skeleton::SetRigg(Rigg* new_rigg)
{
	skeleton = new_rigg;
}

bool Skeleton::Inspector()
{
	return false;
}

bool Skeleton::IsLoaded() const
{
	return skeleton != nullptr;
}

void Skeleton::Rigg::Draw(const GLfloat* opengl_view_matrix) const
{
	float4x4 transform;

	glBegin(GL_LINES);

	for (int i = 0; i < joints.size(); i++)
	{
		glPushMatrix();
		glLoadMatrixf(opengl_view_matrix);

		transform = joints[i].inverse_bind_pose_transform; //Should inverse?
		transform.Inverse();
		glMultMatrixf(&transform.At(0, 0));

		//X
		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);

		//Y
		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);

		//Z
		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);

		glPopMatrix();
	}

	glEnd();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}