#version 330 core
layout (location = 0) in vec3 position;

out vec2 uv;
out vec2 uvForest;

uniform sampler2D heightmap;

uniform int terrain_length;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	vec4 world_pos = model * vec4(position, 1.0f);
	uv = world_pos.xz / terrain_length;
	
	vec2 worVec = world_pos.xz;
	uvForest = worVec;

	world_pos.y = world_pos.y + texture(heightmap, uv).r * 12.0;
    gl_Position = projection * view * world_pos;
}