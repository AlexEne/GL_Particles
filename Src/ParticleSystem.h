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
    explicit			ParticleSystem(int count);
                        ~ParticleSystem();
    

    void				Draw(GLuint glDrawShaderID);

    void				Update(float dt);

    void				Init(unsigned int numWorkgroups_x, unsigned int numWorkgroups_y, unsigned int numWorkgroups_z);

private:
    void				RenderInit(const ParticlePos* particlesPos, const ParticleVelocity* particlesVelocity);

private:
    unsigned int		m_ParticleCount;        //Number of active particles.
    unsigned int		m_TotalParticleCount;   //Number of all available particles.
    int					m_csOutputIdx;		//Output index to identify for the buffer that 
    unsigned int		m_NumWorkGroups[3];

    GLShaderProgram		m_ComputeShader;

//  GLuint              m_glPointTexture;

    GLuint				m_glPositionBuffer[2];//Swap input and output alternatively between each render.
    GLuint				m_glVelocityBuffer[2];

    GLint				m_glUniformDT;		//We also need to send dt to the compute shader.
    GLint				m_glUniformSpheresOffset;	//Uniform for specifying the position of the spheres in our scene.
    GLint				m_glUniformSpheresRadius;	//Uniform for specifying the radius of the spheres in our scene.
    GLuint				m_glNumParticles;   //Total number of particles
    GLuint              m_glDrawVAO[2];     //Vertex array object used for drawing.

private:
    ParticleSystem(const ParticleSystem& other);
    ParticleSystem& operator=(const ParticleSystem& other);
};


#endif //GL_PARTICLE_SYSTEM