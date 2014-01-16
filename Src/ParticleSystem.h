#ifndef GL_PARTICLE_SYSTEM
#define GL_PARTICLE_SYSTEM

#include "GLShader.h"
#include "gl\glew.h"

struct ParticlePos
{
	float x;
	float y;
	float z;
	float w;          //Holds the speed so when we get this in the pixel shader we can color our particles based on that.
};


//ParticleVelocity are only used in the compute shader.
//This is why I keep the  fTimeToLive here.
struct ParticleVelocity
{
	float vx;
	float vy;
	float vz;
	float fTimeToLive; //Remaining time. 
};

class ParticleSystem
{
public:
						ParticleSystem();
						~ParticleSystem();
	explicit			ParticleSystem(int count);

	void				Draw(GLuint glDrawShaderID);

	void				Update(float dt);

	void				Init();

private:
	void				RenderInit();

private:
	ParticlePos*		m_ParticlesPos;		//Buffer holding the particle positions.
	ParticleVelocity*	m_ParticlesVelocity;	//Buffer holding the particle velocities.

	unsigned int		m_ParticleCount;    //Number of particles.

	GLShaderProgram		m_ComputeShader;

//  GLuint              m_glPointTexture;

	GLuint				m_glPositionBuffer;
	GLuint				m_glVelocityBuffer;

	GLuint				m_glUniformDT;		//We also need to send dt to the compute shader.
	GLuint				m_glUniformSpheres;	//uniform for specifying the position/radius of the spheres in our scene.
    GLuint              m_glDrawVAO;        //Vertex array object used for drawing.
};

#endif //GL_PARTICLE_SYSTEM