#version 330 core

// Vertex shader - this code is executed for every vertex of the shape

// Inputs coming from VBOs
layout (location = 0) in vec3 vertex_position; // vertex position in local space (x,y,z)
layout (location = 1) in vec3 vertex_normal;   // vertex normal in local space   (nx,ny,nz)
layout (location = 2) in vec3 vertex_color;    // vertex color      (r,g,b)
layout (location = 3) in vec2 vertex_uv;       // vertex uv-texture (u,v)
layout (location = 4) in vec3 pos; 
layout (location = 5) in vec2 alpha;

// Output variables sent to the fragment shader
out struct fragment_data
{
    vec3 position; // vertex position in world space
    vec3 normal;   // normal position in world space
    vec3 color;    // vertex color
    vec2 uv;       // vertex uv
	float alpha;   // alpha component
} fragment;

// Uniform variables expected to receive from the C++ program
uniform mat4 model; // Model affine transform matrix associated to the current shape
uniform mat4 view;  // View matrix (rigid transform) of the camera
uniform mat4 projection; // Projection (perspective or orthogonal) matrix of the camera
uniform float time; // Time used to animate the dust
uniform float crand ; // Random float in [0, 1] generated in display_frame
uniform vec3 pos_worm; // Position of the worm

// Random noise - https://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl
float PHI = 1.61803398874989484820459;
float gold_noise(in vec2 xy, in float seed) {return fract(tan(distance(xy * PHI, xy) * seed) * xy.x);}
float rand(in float n) { return gold_noise(vec2(n,n+0.5), 1.0); }


vec3 small_disturbance(in float t, in vec3 pos) {
	float rid = rand(gl_InstanceID);
	float x = sin(10 * rid * t);
	float y = cos(17 * rid * t);
	float z = 2 * cos(t + 10 * 3.14 * rid);
	return vec3(x, y, z);
}


void main()
{
	// The position of the vertex in the world space
	vec4 position = model * vec4(vertex_position, 1);
	position += vec4(pos_worm, 1);
	position += vec4(pos, 1);
	position += vec4(small_disturbance(time, pos), 1);

	// The normal of the vertex in the world space
	mat4 modelNormal = transpose(inverse(model));
	vec4 normal = modelNormal * vec4(vertex_normal, 0.0);

	// The projected position of the vertex in the normalized device coordinates:
	vec4 position_projected = projection * view * position;

	// Fill the parameters sent to the fragment shader
	// Random color - not using vertex_color
	vec3 light = vec3(1, 0.88, 0.36);
	vec3 dark = vec3(0.7, 0.48, 0.18);
	float lambda = pow(rand(floor(time + 10 * rand(gl_InstanceID)) + pos[0] + pos[1]), 4);

	fragment.position = position.xyz;
	fragment.normal   = normal.xyz;
	fragment.color = (1 - lambda) * light + lambda * dark;
	float eps_alpha = (rand(floor(time + 17 * rand(gl_InstanceID)) * gl_InstanceID) - 0.5) / 10;
	fragment.alpha = alpha[0] + eps_alpha;
	fragment.uv = vertex_uv;

	// gl_Position is a built-in variable which is the expected output of the vertex shader
	gl_Position = position_projected; // gl_Position is the projected vertex position (in normalized device coordinates)
}
