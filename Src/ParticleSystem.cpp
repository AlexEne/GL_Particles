#include "ParticleSystem.h"
#include "Helpers.h"
#include <random>
#include <vector>
#include <ctime>


ParticleSystem::ParticleSystem(int count):
m_glUniformDT(-1),
m_glUniformSpheresOffset(-1),
m_glUniformSpheresRadius(-1),
m_csOutputIdx(1)
{
	m_ParticleCount = count;

	for (int i = 0; i < 2; ++i)
	{
		m_glVelocityBuffer[i] = 0;
		m_glPositionBuffer[i] = 0;
	}
}

ParticleSystem::~ParticleSystem()
{
	glDeleteVertexArrays(2, m_glDrawVAO);
	
	for (int i = 0; i < 2; ++i)
	{
		glDeleteBuffers(1, &m_glVelocityBuffer[i]);
		glDeleteBuffers(1, &m_glVelocityBuffer[i]);
	}
}

//Initialize the positions, velocities and lifetime for our particles.
void ParticleSystem::Init(unsigned int numWorkgroups_x, unsigned int numWorkgroups_y, unsigned int numWorkgroups_z)
{
	static const float cubeSize = 1400.0f;
	
	m_NumWorkGroups[0] = numWorkgroups_x;
	m_NumWorkGroups[1] = numWorkgroups_y;
	m_NumWorkGroups[2] = numWorkgroups_z;

	ParticlePos* particlesPos = new ParticlePos[m_ParticleCount];
	ParticleVelocity* particlesVelocity = new ParticleVelocity[m_ParticleCount];

	memset(particlesVelocity, 0, m_ParticleCount*sizeof(ParticleVelocity));

	ParticlePos* pParticlePos = particlesPos;
	ParticleVelocity* pParticleSpeed = particlesVelocity;
	for (unsigned int i = 0; i < m_ParticleCount; ++i)
	{
		//Assign random positions/velocities
		pParticlePos->x = -cubeSize/2.0f + fmod((float)rand(), cubeSize);
		pParticlePos->y = cubeSize/3.0f + fmod((float)rand(), cubeSize);
		pParticlePos->z = -cubeSize/2.0f + fmod((float)rand(), cubeSize);

		pParticleSpeed->vx = -2.5f + rand() % 5 + (rand()%10)/10.0f;
		pParticleSpeed->vz = -2.5f + rand() % 5 + (rand()%10)/10.0f;
		pParticleSpeed->vy = -5.0f + rand() % 10 + (rand()%10)/10.0f;
		pParticleSpeed->fTimeToLive = 20.0f + fmod((float)rand(), 10.0f);

		pParticlePos->w = 1.0;
		pParticlePos++;
		pParticleSpeed++;
	}

	RenderInit(particlesPos, particlesVelocity);
	
	delete[] particlesPos;
	delete[] particlesVelocity;
}


//Initialize members related to opengl.
void ParticleSystem::RenderInit(const ParticlePos* particlesPos,const ParticleVelocity* particlesVelocity)
{
	//Initialize and create the compute shader that will move the particles in the scene
	m_ComputeShader.Init();
	m_ComputeShader.CompileShaderFromFile("Shaders\\ComputeShader.glsl", GLShaderProgram::Compute);
	m_ComputeShader.Link();

	m_glPositionBuffer[0] = AllocateBuffer(GL_SHADER_STORAGE_BUFFER, (float*)particlesPos, m_ParticleCount*sizeof(ParticlePos));
	m_glPositionBuffer[1] = AllocateBuffer(GL_SHADER_STORAGE_BUFFER, (float*)particlesPos, m_ParticleCount*sizeof(ParticlePos));

	m_glVelocityBuffer[0] = AllocateBuffer(GL_SHADER_STORAGE_BUFFER, (float*)particlesVelocity, m_ParticleCount*sizeof(ParticleVelocity));
	m_glVelocityBuffer[1] = AllocateBuffer(GL_SHADER_STORAGE_BUFFER, (float*)particlesVelocity, m_ParticleCount*sizeof(ParticleVelocity));
    
	//Cache uniforms
	m_glUniformDT		= m_ComputeShader.GetUniformLocation("dt");
	m_glUniformSpheresOffset	= m_ComputeShader.GetUniformLocation("sphereOffsets");
	m_glUniformSpheresRadius = m_ComputeShader.GetUniformLocation("sphereRadius");
	m_glNumParticles	= m_ComputeShader.GetUniformLocation("g_NumParticles");

	//Create and set the vertex array objects
    glGenVertexArrays(2, m_glDrawVAO);

    glBindVertexArray(m_glDrawVAO[0]);
	glBindBuffer(GL_ARRAY_BUFFER, m_glPositionBuffer[0]);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(m_glDrawVAO[1]);
	glBindBuffer(GL_ARRAY_BUFFER, m_glPositionBuffer[1]);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	

    glBindVertexArray(0);
}

//Particles are sent to the compute shader and we compute there the new positions/velocities.
void ParticleSystem::Update(float dt)
{
	glUseProgram(m_ComputeShader.GetHandle()); 

	//Send the sphere positions/radius to the compute shader
	m_ComputeShader.SetUniform(m_glUniformDT, dt);

	if (m_glUniformSpheresOffset != -1 && m_glUniformSpheresRadius != -1 )
	{
		m_ComputeShader.SetUniform3fv(m_glUniformSpheresOffset, g_SpheresCount, (float*)g_Spheres.centers);
		m_ComputeShader.SetUniform1fv(m_glUniformSpheresRadius, g_SpheresCount, g_Spheres.radii);
	}

	m_ComputeShader.SetUniform(m_glNumParticles, (int)m_ParticleCount);

    //Bind the position and speed buffers
	//Input = !m_csOutputIDX
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, m_glPositionBuffer[!m_csOutputIdx], 0, m_ParticleCount*sizeof(ParticlePos));
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, m_glVelocityBuffer[!m_csOutputIdx], 0, m_ParticleCount*sizeof(ParticleVelocity));
    
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 2, m_glPositionBuffer[m_csOutputIdx], 0, m_ParticleCount*sizeof(ParticlePos));
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 3, m_glVelocityBuffer[m_csOutputIdx], 0, m_ParticleCount*sizeof(ParticleVelocity));

    //Setup and execute the compute shader
	glDispatchCompute(m_NumWorkGroups[0], m_NumWorkGroups[1], m_NumWorkGroups[2]);
    glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    glUseProgram(0);

	//Swap input and output
	m_csOutputIdx = !m_csOutputIdx;
}

//Render the particles.
void ParticleSystem::Draw(GLuint glDrawShaderID)
{
    glUseProgram(glDrawShaderID);
    
    //Set the active Vertex array object
	glBindVertexArray(m_glDrawVAO[m_csOutputIdx]);

    //Draw
    glDrawArrays(GL_POINTS, 0, m_ParticleCount);

    //Cleanup
    glBindVertexArray(0);
    glUseProgram(0);
}