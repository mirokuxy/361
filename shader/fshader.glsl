#version 130

uniform sampler2D ourTexture0;
uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;
uniform sampler2D ourTexture3;
uniform sampler2D ourTexture4;

uniform int type;

uniform vec3 myColor;

in vec2 TexCoord;


out vec4 color;

void main()
{
	/*
	if(true){
		color = mix(texture(ourTexture0,TexCoord), texture(ourTexture1,vec2(TexCoord.x,1.0f - TexCoord.y)),0.2);
	}
	else;
	//color = vec4(myColor,1.0f);
	*/

	if(type == 0)
		color = texture(ourTexture0,vec2(TexCoord.x,1.0f - TexCoord.y)) * vec4(myColor,1.0f);
	else if(type == 1)
		color = texture(ourTexture1,vec2(TexCoord.x,1.0f - TexCoord.y)) * vec4(myColor,1.0f);
	else if(type == 2)
		color = texture(ourTexture2,vec2(TexCoord.x,1.0f - TexCoord.y)) * vec4(myColor,1.0f);
	else if(type == 3) 
		color = texture(ourTexture3,vec2(TexCoord.x,1.0f - TexCoord.y)) * vec4(myColor,1.0f);
	else if(type == 4)
		color = texture(ourTexture4,vec2(TexCoord.x,1.0f - TexCoord.y)) * vec4(myColor,1.0f);
	else
		color = vec4(myColor,1.0f);
}
