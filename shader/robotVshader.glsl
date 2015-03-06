#version 130 

in vec3 position;
in vec3 normal;

out vec3 Normal;
out vec3 FragPos;

//uniform mat3 normMatrix;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	gl_Position = projection * view * model * vec4(position,1.0f);
	//Normal = normMatrix * normal;
	//Normal = normal;
	Normal = mat3(transpose(inverse(model))) * normal;  

	FragPos = vec3 (model * vec4(position,1.0f)) ;
}
