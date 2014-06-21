#include <stdio.h>
#include <random>
#include <ctime>
#include "GL\glew.h"
#include "SDL\SDL.h"
#include "SDL\SDL_opengl.h"
#include "GLShader.h"
#include "ParticleSystem.h"
#include "Helpers.h"
#include "glm\gtc\matrix_transform.hpp"

//Uncomment this to enable OPENGL debug output.
#define DEBUG_OPENGL

using glm::mat4;
using glm::vec3;

//SDL window and context
SDL_Window* g_pWindow;
SDL_GLContext gContext;

//Simple shader program and 2 cached uniforms.
GLShaderProgram g_ShaderProgram;
GLuint			g_ViewUniform = 0;
GLuint			g_ProjUniform = 0;

ParticleSystem*	g_ParticleSystem = nullptr;

<<<<<<< HEAD
class Camera
{
public:
	Camera()
=======
struct Camera_t
{
	Camera_t()
>>>>>>> 5ce6b4d86cd87fc0d82a2cfecc7550ccc972a315
	{
		m_Pos = vec3(0.0f, 300.0f,-1500.0f);
		m_AngleYaw = 0;
		m_AnglePitch = 0;
	}

	void Update()
	{
		m_ViewMtx = glm::lookAt(m_Pos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
		m_ViewMtx = glm::rotate(m_ViewMtx, m_AnglePitch, vec3(1.0f, 0.0f, 0.0f));
		m_ViewMtx = glm::rotate(m_ViewMtx, m_AngleYaw, vec3(0.0f, 1.0f, 0.0f));
	}

	mat4		m_ViewMtx;
	mat4		m_ProjMtx;
	vec3		m_Pos;
	float		m_AngleYaw;
	float		m_AnglePitch;
};
<<<<<<< HEAD
Camera		g_Camera;
=======
Camera_t		g_Camera;
>>>>>>> 5ce6b4d86cd87fc0d82a2cfecc7550ccc972a315

//Spheres that will be randlomly spawned at random locations in our scene.
const int		g_SpheresCount = 20;
Sphere_t		g_Spheres[g_SpheresCount];


//Callback used by opengl when an error is encountered
void APIENTRY openglCallbackFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, void* userParam)
{
	//Just output it to console and stop execution.
	printf("%s\n", message);
	__debugbreak();
}

//Initialize SDL, create the opengl context and then we initialize glew.
static void InitSDL()
{
	int i = SDL_Init(SDL_INIT_VIDEO);

	g_pWindow = SDL_CreateWindow("GLParticles", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1600, 900, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

	//Specify context flags.
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

#ifdef DEBUG_OPENGL
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif

	//Create opengl context
	gContext = SDL_GL_CreateContext(g_pWindow);
	
	//Can use opengl functions after this is called.
	glewExperimental=GL_TRUE;
	GLenum err = glewInit();

#ifdef DEBUG_OPENGL
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(openglCallbackFunction, NULL);
#endif // DEBUG_OPENGL
	
	//Using vsync
	SDL_GL_SetSwapInterval(1);
}


void InitScene()
{
	//Init a simple shader program used to draw stuff.
	g_ShaderProgram.Init();
	g_ShaderProgram.CompileShaderFromFile("Shaders\\VertexShader.v.glsl", GLShaderProgram::Vertex);
	g_ShaderProgram.CompileShaderFromFile("Shaders\\PixelShader.p.glsl", GLShaderProgram::Fragment);
	g_ShaderProgram.Link();

	//Cache
	g_ViewUniform = g_ShaderProgram.GetUniformLocation("ViewMtx");
	g_ProjUniform = g_ShaderProgram.GetUniformLocation("ProjMtx");
	
	//Create the particle system and initialize it
	g_ParticleSystem = new ParticleSystem( 1024 * 1024 );
	g_ParticleSystem->Init();

	g_Camera.m_ProjMtx = glm::perspective(45.0f, 4.0f / 3.0f, 0.5f, 10000.0f);
	g_Camera.m_ViewMtx = glm::lookAt(g_Camera.m_Pos, vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

#ifdef DEBUG_OPENGL
	glEnable(GL_DEBUG_OUTPUT);
#endif

	glPointSize(1.6f);
	
	srand((unsigned int)time(0));

	for(int i = 0; i < g_SpheresCount; ++i)
	{
		g_Spheres[i].radius = 100.0f + fmod((float)rand(), 50.0f);
		g_Spheres[i].center.x = -700.0f + fmod((float)rand(), 1400.0f);
		g_Spheres[i].center.y = 0.0f; 
		g_Spheres[i].center.z = -700.0f + fmod((float)rand(), 1400.0f);
	}
}

void Render()
{
	//Clear color buffer
	glClearColor(0.2f, 0.2f, 0.25f, 1.0f);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glUseProgram(g_ShaderProgram.GetHandle());
	g_ShaderProgram.SetUniform(g_ViewUniform, g_Camera.m_ViewMtx);
	g_ShaderProgram.SetUniform(g_ProjUniform, g_Camera.m_ProjMtx);

	g_ParticleSystem->Draw(g_ShaderProgram.GetHandle()); 
	
	glFlush();
}


//Update 
//dt - in seconds.
void Update(float dt)
{
	g_ParticleSystem->Update(dt);
}

static float fVelocity = 50.0f;
void HandleKeys(char key, float dx, float dy, float dt)
{
	float accel = 40.0f;
	fVelocity += accel*dt;
	float dist = fVelocity*dt + 1/2*accel*dt*dt;

	g_Camera.m_AnglePitch -= dy/25.0f;
	g_Camera.m_AngleYaw += dx/25.0f;

	if (key == SDLK_s)
	{
		g_Camera.m_Pos.z -= dist;
	}
	if (key == SDLK_w)
	{
		g_Camera.m_Pos.z += dist;
	}

	g_Camera.Update();
}


void Cleanup()
{
	delete g_ParticleSystem;

	//Destroy SDL window	
	SDL_DestroyWindow( g_pWindow );
	g_pWindow = NULL;

	SDL_Quit();
}


int main(int argc, char *argv[])
{
	InitSDL();
	InitScene();
	bool quit = false;

	unsigned int prevTimeMS = SDL_GetTicks();
	while( !quit )
	{
		unsigned int currTimeMS = SDL_GetTicks();
		float dt = (float)(currTimeMS - prevTimeMS)/1000.0f;
		prevTimeMS = currTimeMS;

		//Handle events
		SDL_Event e;
		while( SDL_PollEvent( &e ) != 0 )
		{
			if( e.type == SDL_QUIT )
				quit = true;

			//Handle keypress with current mouse position
			char key = 0;
			if( e.type == SDL_TEXTINPUT )
			{
				if(e.type == SDL_KEYDOWN)
					fVelocity = 50.0f;
				 
				key = e.text.text[0];
			}

			int dx = 0, dy = 0;
			if(e.type == SDL_MOUSEMOTION && e.motion.state & SDL_BUTTON_LEFT)
			{
				dx = e.motion.xrel;
				dy = e.motion.yrel;
			}

			HandleKeys(key,	(float)dx, (float)dy, dt );
		}

		Update(dt);

		Render();

		SDL_GL_SwapWindow( g_pWindow );
	}

	Cleanup();
	return 0;
}