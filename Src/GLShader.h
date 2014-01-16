#ifndef GL_SHADER_CLASS
#define GL_SHADER_CLASS

#include "glm\glm.hpp"

class GLShaderProgram
{
public:
	enum ShaderType
	{
		Vertex,
		Fragment,
		Compute
	};

					GLShaderProgram();
					~GLShaderProgram();

	bool			CompileShaderFromFile(const char* file, ShaderType type);
	bool			CompileShaderFromString(const char* str, ShaderType type); 
	bool			Link();
	void			PrintActiveUniforms();
	void			PrintActiveAttribs();

	void			SetUniform(const char* name, float x, float y, float z);
	void			SetUniform(const char* name, float x, float y, float z, float w);
	void			SetUniform(const char* name, float val);
	void			SetUniform(const char* name, glm::mat4 mtx);
	void			SetUniform(const char* name, glm::mat3 mtx);

	void			SetUniform( int uniform, float x, float y, float z);
	void			SetUniform( int uniform, float x, float y, float z, float w);
	void			SetUniform( int uniform, glm::vec3 vec);
	void			SetUniform( int uniform, glm::vec4 vec);
	void			SetUniform( int uniform, float val);
	void			SetUniform( int uniform, glm::mat4 mtx);
	void			SetUniform( int uniform, glm::mat3 mtx);

	void			Init();

	int				GetHandle(){ return m_Handle; }
	int				GetAttribLocation(const char* name);
	int				GetUniformLocation(const char* name);

private:
	int				m_Handle;
	bool			m_bLinked;
};

#endif //GL_SHADER_CLASS