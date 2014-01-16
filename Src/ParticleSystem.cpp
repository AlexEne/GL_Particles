#include "ParticleSystem.h"
#include "Helpers.h"
#include <random>
#include <vector>
#include <ctime>


ParticleSystem::ParticleSystem():
m_glUniformDT(0),
m_glUniformSpheres(0),
m_glVelocityBuffer(0),
m_glPositionBuffer(0),
m_glDrawVAO(0)
{
	m_ParticleCount = 10;
	m_ParticlesPos = new ParticlePos[m_ParticleCount];
	m_ParticlesVelocity = new ParticleVelocity[m_ParticleCount];
}

ParticleSystem::ParticleSystem(int count):
m_glUniformDT(0),
m_glUniformSpheres(0),
m_glVelocityBuffer(0),
m_glPositionBuffer(0),
m_glDrawVAO(0)
{
	m_ParticleCount = count;
	m_ParticlesPos = new ParticlePos[m_ParticleCount];
	m_ParticlesVelocity = new ParticleVelocity[m_ParticleCount];
}

ParticleSystem::~ParticleSystem()
{
	delete[] m_ParticlesPos;
	delete[] m_ParticlesVelocity;
	glDeleteVertexArrays(1, &m_glDrawVAO);
	glDeleteBuffers(1, &m_glVelocityBuffer);
	glDeleteBuffers(1, &m_glPositionBuffer);
}

//Initialize the positions, velocities and lifetime for our particles.
void ParticleSystem::Init()
{
	static const float cubeSize = 1400.0f;

	memset(m_ParticlesVelocity, 0, m_ParticleCount*sizeof(ParticleVelocity));

	ParticlePos* pParticlePos = m_ParticlesPos;
	ParticleVelocity* pParticleSpeed = m_ParticlesVelocity;
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

	RenderInit();
}


//Initialize members related to opengl.
void ParticleSystem::RenderInit()
{
	//Initialize and create the compute shader that will move the particles in the scene
	m_ComputeShader.Init();
	m_ComputeShader.CompileShaderFromFile("Shaders\\ComputeShader.glsl", GLShaderProgram::Compute);
	m_ComputeShader.Link();

	m_glPositionBuffer = AllocateBuffer(GL_SHADER_STORAGE_BUFFER, (float*)m_ParticlesPos, m_ParticleCount*sizeof(ParticlePos));
    m_glVelocityBuffer = AllocateBuffer(GL_SHADER_STORAGE_BUFFER, (float*)m_ParticlesVelocity, m_ParticleCount*sizeof(ParticleVelocity));
    
	//Cache uniforms
	m_glUniformDT = m_ComputeShader.GetUniformLocation("dt");
	m_glUniformSpheres = m_ComputeShader.GetUniformLocation("spheres[0].sphereOffset");

    //Set the shader storage block indices
	GLuint SSblockIndex = glGetProgramResourceIndex(m_ComputeShader.GetHandle(), GL_SHADER_STORAGE_BLOCK, "buffer_Pos");
	glShaderStorageBlockBinding(m_ComputeShader.GetHandle(), SSblockIndex, SSblockIndex);
	SSblockIndex = glGetProgramResourceIndex(m_ComputeShader.GetHandle(), GL_SHADER_STORAGE_BLOCK, "buffer_Velocity");
	glShaderStorageBlockBinding(m_ComputeShader.GetHandle(), SSblockIndex, SSblockIndex);

	//Create and set the vertex array object
    glGenVertexArrays(1, &m_glDrawVAO);
    glBindVertexArray(m_glDrawVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_glPositionBuffer);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

//Particles are sent to the compute shader and we compute there the new positions/velocities.
void ParticleSystem::Update(float dt)
{
	//Bind input
	glUseProgram(m_ComputeShader.GetHandle()); 
	GLuint m_glRndSeed = m_ComputeShader.GetUniformLocation("rnd_Seed");
	m_ComputeShader.SetUniform(m_glRndSeed, (float)rand());

	//Send the sphere positions/radius to the compute shader
	m_ComputeShader.SetUniform(m_glUniformDT, dt);
	if(m_glUniformSpheres != 0)
	{
		for (int i = 0; i < g_SpheresCount; ++i)
		{
			GLuint uniformIdx = m_glUniformSpheres + i*2;
			m_ComputeShader.SetUniform(uniformIdx, g_Spheres[i].center);
			m_ComputeShader.SetUniform(uniformIdx+1, g_Spheres[i].radius);
		}
	}

    //Bind the position and speed buffers
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, m_glPositionBuffer, 0, m_ParticleCount*sizeof(ParticlePos));
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, m_glVelocityBuffer, 0, m_ParticleCount*sizeof(ParticleVelocity));
    
    //Setup and execute the compute shader
	const int workgroups_count = 1024/32;
	glDispatchCompute(workgroups_count, workgroups_count, 2);
    glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    glUseProgram(0);
}

//Render the particles.
void ParticleSystem::Draw(GLuint glDrawShaderID)
{
    glUseProgram(glDrawShaderID);
    
    //Set the active Vertex array object
    glBindVertexArray(m_glDrawVAO);

    //Draw
    glDrawArrays(GL_POINTS, 0, m_ParticleCount);

    //Cleanup
    glBindVertexArray(0);
    glUseProgram(0);
}