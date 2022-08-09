/* Shader code in Vulkan has to be specified in bytecode format as opposed to human-readable
  	syntax like GLSL and HLSL. This format is called SPIR-V and is designed to be used with both
	Vulkan and OpenCL. 
	
	The advantage of using a bytecode format is that the compilers written by GPU vendors to turn
	shader code into native code are significantly less complex. Khronos has released their own
	vendor-independent compiler that compiles GLSL into SPIR-V. For this tutorial we will use
	glslc.exe by Google instead. It uses the same parameter format as well-known compilers like
	GCC and Clang and includes extra functionality like includes. 
	
	GLSL is a shading language with a C-like syntax. Programs have a main function that is invoked
	for every object. Instead of using parameters for input and a return value as output, GLSL uses
	global variables. The language includes many features such as matrix and vector primitives. 
	Functions for operations like cross-products, matrix-vector products, and reflections around a
	vector are included. 
	
	The vector type is called vec with a number indicating the amount of elements. A 3D position
	would be stored in vec3. It is possible to access single components with .x, but it is also
	possible to create a new vector from multiple components at the same time. For example, the 
	expression vec3(1.0, 2.0, 3.0).xy would result in vec2. The constructors of vectors can also
	take combinations of vecotr objects and scalar values. vec3 can be constructed with
	vec3(vec2(1.0, 2.0), 3.0).
	
	Vertex Shader
	
	The vertex shader processes each incoming vertex. It takes attriubtes, like world position,
	color, normal and texture coordinates, etc. as input. The output is the final position in the
	clip coordinates and the attributes that need to be passed on to the fragment shader, like
	color and texture coordinates. 
	
	A clip coordinate is s four dimensional vector from the vertex shader that is subsequently
	turned into a normalized device coordinate by diving the whole vector by its last component. 
	These normalized device coordinates are homogenous coordinates that map the framebuffer to a
	[-1, 1], by [-1, 1] coordinate system.
	
	We can directly output normalized device coordinates by outputting them as clip coordinates
	from the vertex shader with the last component set to 1. That way the division to transform
	clip coordinates will not change anything. 
	
	For this example we will include the coordinates directly in the vertex shader.

*/

// Vertex shader code

#version 450

vec2 positions[3] = vec2[](
	vec2(0.0, -0.5),
	vec2(0.5, 0.5),
	vec2(-0.5, 0.5)
);

void main() {
	gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
}

/* The main function is invoked for every vertex. The built-in gl_VertexIndex variable contains
	the index of the current vertex. The position of each vertex is accessed from the constant
	array in the shader and combined with the dummy z and w coordinates to produce a position
	in clip coordinates. The built in variable gl_Position functions as the output. 
*/

// Fragment shader code

#version 450

layout(location = 0) out vec4 outColor;

void main() {
	outColor = vec4(1.0, 0.0, 0.0, 1.0);
}

/* The fragment shader is invoked on the vertex shader fragments to produce a color and depth
	for the framebuffer. 
	
	The main function is called for every fragment. Colors in GLSL are 4-component vectors with
	the RGB and alpha channels within the [0, 1] range. Unlike gl_Position in the vertex shader, 
	there is no built-in variable to output a color for the current fragment. You have to specify
	your own output variable for each framebuffer where the layout(location = 0) modifier
	specifies the index of the framebuffer. The color red is written to this outColor variable
	that is linked to the first and only framebuffer at index 0. 
*/

// Pre-vertex Colors

/* We can also make the triangle more colorful, since red is not very interesting by itself. To
	do that we need to make a couple of changes to both shaders to accomplish this. 
*/

// Vertex shader

layout(location = 0) out vec3 fragColor; // Output for color
vec3 colors[3] = vec3[](
	vec3(1.0, 0.0, 0.0),
	vec3(0.0, 1.0, 0.0),
	vec3(0.0, 0.0, 1.0)
);

void main() {
	gl_Position = vec4 (positions[gl_VertexIndex], 0.0, 1.0);
	fragColor = colors[gl_VertexIndex];
}
// Now these pre-vertex colors need to be passed to the fragment shader. 

layout(location = 0) in vec3 fragColor;

void main(){
	outColor = vec4(fragColor, 1.0);
}

// Compiling the shaders

/* Create a directory called shaders in the root directory of your project and store the vertex
shader in a file called shader.vert and the fragment shader in a file called shader.frag. GLSL
shaders don't have an official extension but these two are commonly used.

The contents of shader.vert should be:
*/

#version 450

layout(position = 0) out vec3 fragColor;

vec2 positions[3] = vec2[](
	vec2(0.0, -0.5),
	vec2(0.5, 0.5),
	vec2(-0.5, 0.5)
);

vec3 colors[3] = vec3[](
	vec3(1.0, 0.0, 0.0),
	vec3(0.0, 1.0, 0.0),
	vec3(0.0, 0.0, 1.0)
);

void main(){
	gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
	fragColor = colors[gl_VertexINdex];
};

// The fragment shader should have:

#version 450

layour(location = 0) in vec3 fragColor;
layout(location = 0) out vec4 outColor;

void main(){
	outColor = vec4(fragColor, 1.0);
}

// Now we will compile these into SPIR-V bytecode using the glslc program:
		 
	
