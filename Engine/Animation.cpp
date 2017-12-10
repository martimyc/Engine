#include "Animation.h"

void Animation::AnimationClip::AnimationSample::Draw(const GLfloat* opengl_view_matrix) const
{
	float4x4 transform;

	glBegin(GL_LINES);

	for (int i = 0; i < joint_poses.size(); i++)
	{
		glPushMatrix();
		glLoadMatrixf(opengl_view_matrix);

		transform = global_joint_poses[i];
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

Animation::AnimationClip::AnimationClip()
{
}

Animation::AnimationClip::~AnimationClip()
{
}

Animation::Animation(const std::string & name, const UID & uid): Resource(RT_ANIMATION, name, uid)
{}

Animation::Animation(const std::string & name, AnimationClip * source): Resource(RT_ANIMATION, name)
{}

Animation::~Animation()
{
	if (source != nullptr)
		delete source;
}
