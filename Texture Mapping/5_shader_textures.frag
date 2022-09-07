#version 450
// A combined image sampler descriptor is represented in GLSL by a sampler uniform:
layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = texture(texSampler, fragTexCoord);
}
/*	Textures are sampled using the built in texture function. It takes a sampler and coordinates as arguments. The
	sampler automatically takes care of the filtering and transformations in the background. */
