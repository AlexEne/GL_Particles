#version 420

in vec4 transformedColor;
out vec4 FragColor;

void main()
{
	FragColor = transformedColor; 
}
