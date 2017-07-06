#version 450 core
#pragma debug(on)
#pragma optimize(off)

#pragma include("lighting.vert.glsl")


layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec3 tangent;
layout(location=3) in vec3 bitangent;
layout(location=4) in vec4 color;
layout(location=5) in vec2 uv;


uniform mat4 V;
uniform mat4 P;
uniform float scale = 0.2;

layout(binding=0) uniform samplerBuffer color_tbo;
layout(binding=1) uniform samplerBuffer matrix_tbo;

mat4 model(){
	return mat4(
		texelFetch(matrix_tbo, gl_InstanceID * 4),
		texelFetch(matrix_tbo, gl_InstanceID * 4 + 1),
		texelFetch(matrix_tbo, gl_InstanceID * 4 + 2),
		texelFetch(matrix_tbo, gl_InstanceID * 4 + 3)
	);
}

void main(){
	mat4 M = model();
	vertex_out.texCoord = uv;
	vertex_out.color = texelFetch(color_tbo, gl_InstanceID);
	applyLight(V * M, V, position, normal, tangent, bitangent);
	gl_Position = P * V * M * vec4(position, 1);
}
