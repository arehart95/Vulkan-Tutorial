#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
}   ubo;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main() {
	gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 0.0, 1,0); // MVP matrix
	fragColor = inColor;
}
    
/*	Note that the order of the uniform, in, and out declaration does not matter. The binding 
	directive is similar to the location directive for attributes. We will reference this binding
	in the descriptor layout. The line with gl_Position is changed to use the transformations to
	compute the final position in clip coordinates. Unlike 2D triangles, the last component of the
	clip coordinates may not be 1 which will result in a division when converted to final normalized
	device coordinates on the screen. This is used in perspective projection as the perpective
	division and is essential for making closer objects look larger than objects that are further
	away. 
*/
