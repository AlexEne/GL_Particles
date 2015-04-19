#include "GLShader.h"
#include "gl\glew.h"
#include "stdio.h"
#include "Helpers.h"

#include "windows.h"

GLShaderProgram::GLShaderProgram() : 
m_bLinked(false),
m_Handle(0)
{
}

char *file_contents(const char *filename, GLint *length)
{
	FILE *f = fopen(filename, "r");
	char *buffer;
	if (!f)
	{
		SetCurrentDirectory("d:\\Graphics\\GL_Particle\\Particles\\");
		f = fopen(filename, "r");
	}

	if (!f) {
		char dir[256];
		//GetCurrentDirectory(256, dir);
		fprintf(stderr, "Unable to open %s for reading\n. CurrDir: %s", filename, dir);
		return NULL;
	}

	fseek(f, 0, SEEK_END);
	*length = ftell(f);
	fseek(f, 0, SEEK_SET);

	buffer = (char*)malloc(*length+1);
	*length = fread(buffer, 1, *length, f);
	fclose(f);
	buffer[*length] = '\0';

	return buffer;
}


static void show_info_log(GLuint object, PFNGLGETSHADERIVPROC glGet__iv, PFNGLGETSHADERINFOLOGPROC glGet__InfoLog)
{
	GLint log_length;
	char *log;

	glGet__iv(object, GL_INFO_LOG_LENGTH, &log_length);
	log = (char*)malloc(log_length);
	glGet__InfoLog(object, log_length, NULL, log);
	fprintf(stderr, "%s", log);
	free(log);
}


bool	GLShaderProgram::CompileShaderFromFile(const char* file, ShaderType type)
{
	int size;
	const char* ch = file_contents(file, &size);
	if (ch)
	{
		return CompileShaderFromString(ch, type);
	}
	return false;
}

//Compile a shader and attach it to our program.
bool	GLShaderProgram::CompileShaderFromString(const char* str, ShaderType type)
{
	if(!str)
		return false;

	GLuint shaderHandle = -1;
	switch(type)
	{
	case Vertex:
		{
			shaderHandle = glCreateShader(GL_VERTEX_SHADER);
		}
		break;
	case Fragment:
		{
			shaderHandle = glCreateShader(GL_FRAGMENT_SHADER);
		}
		break;
	case Compute:
		{
			shaderHandle = glCreateShader(GL_COMPUTE_SHADER);
		}
		break;
	default:
		printf("GLShaderProgram::CompileShaderFromString Unknown ShadeType %d\n", type);
	}

	int length = strlen(str);
	glShaderSource(shaderHandle, 1, (const GLchar**)&str, &length);
	glCompileShader(shaderHandle);
	GLint shaderOk;
	glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &shaderOk);
	
	if (!shaderOk)
	{
		fprintf(stderr, "Failed to compile: %s\n", str);
		show_info_log(shaderHandle, glGetShaderiv, glGetShaderInfoLog);
		glDeleteShader(shaderHandle);
		return false;
	}

	glAttachShader(m_Handle, shaderHandle);

	return true;
}

//Link the shader program.
bool	GLShaderProgram::Link()
{
	glLinkProgram(m_Handle);
	GLint programOK;
	glGetProgramiv(m_Handle, GL_LINK_STATUS, &programOK);
	if (!programOK)
	{
		fprintf(stderr, "Failed to link: \n");
		show_info_log(m_Handle, glGetProgramiv, glGetProgramInfoLog);
		glDeleteShader(m_Handle);
	}
	else
	{
		m_bLinked = true;
	}

	return m_bLinked;
}

void GLShaderProgram::Init()
{
	m_Handle = glCreateProgram();
}


void	GLShaderProgram::SetUniform(const char* name, float x, float y, float z)
{
	GLint uniformLocation = GetUniformLocation(name);
	if (uniformLocation >= 0)
	{
		glUniform3f(uniformLocation, x, y, z);

	}
}


void	GLShaderProgram::SetUniform(const char* name, float x, float y, float z, float w)
{
	GLint uniformLocation = GetUniformLocation(name);
	if (uniformLocation >= 0)
	{
		glUniform4f(uniformLocation, x, y, z, w);
	}
}


void	GLShaderProgram::SetUniform(const char* name, float val)
{
	GLint uniformLocation = GetUniformLocation(name);
	if (uniformLocation >= 0)
	{
		glUniform1f(uniformLocation, val);
	}
}


void	GLShaderProgram::SetUniform(const char* name, glm::mat4 mtx)
{
	GLint uniformLocation = GetUniformLocation(name);
	if( uniformLocation >= 0)
	{
		glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, &mtx[0][0]);
	}
}


void	GLShaderProgram::SetUniform(const char* name, glm::mat3 mtx)
{
	GLint uniformLocation = GetUniformLocation(name);
	if( uniformLocation >= 0)
	{
		glUniformMatrix3fv(uniformLocation, 1, GL_FALSE, &mtx[0][0]);
	}
}


GLint GLShaderProgram::GetUniformLocation(const char* name)
{
	return glGetUniformLocation(m_Handle, name);
}


GLint GLShaderProgram::GetAttribLocation(const char* name)
{
	return glGetAttribLocation(m_Handle, name);
}

void GLShaderProgram::SetUniform(int uniform, glm::vec3 vec)
{
	SetUniform(uniform, vec.x, vec.y, vec.z);
}


void GLShaderProgram::SetUniform(int uniform, float x, float y, float z)
{
	glUniform3f(uniform, x, y, z);
}


void GLShaderProgram::SetUniform(int uniform, glm::vec4 vec)
{
	SetUniform(uniform, vec.x, vec.y, vec.z, vec.w);
}


void GLShaderProgram::SetUniform( int uniform, float x, float y, float z, float w)
{
	glUniform4f(uniform, x, y, z, w);
}


void GLShaderProgram::SetUniform( int uniform, float val)
{
	glUniform1f(uniform, val);
}


void GLShaderProgram::SetUniform( int uniform, glm::mat4 mtx)
{
	glUniformMatrix4fv(uniform, 1, GL_FALSE, &mtx[0][0]);
}


void GLShaderProgram::SetUniform( int uniform, glm::mat3 mtx)
{
	glUniformMatrix3fv(uniform, 1, GL_FALSE, &mtx[0][0]);
}

void GLShaderProgram::SetUniform( int uniform, int val )
{
	glUniform1i(uniform, val);
}

void GLShaderProgram::SetUniform1fv(int uniform, int count, float* values)
{
	glUniform1fv(uniform, count, values);
}

void GLShaderProgram::SetUniform2fv(int uniform, int count, float* values)
{
	glUniform2fv(uniform, count, values);
}

void GLShaderProgram::SetUniform3fv(int uniform, int count, float* values)
{
	glUniform3fv(uniform, count, values);
}

void GLShaderProgram::SetUniform4fv(int uniform, int count, float* values)
{
	glUniform4fv(uniform, count, values);
}

GLShaderProgram::~GLShaderProgram()
{
	glDeleteProgram(m_Handle);
}