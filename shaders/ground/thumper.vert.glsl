#version 330 core

// Vertex shader - this code is executed for every vertex of the shape

// Inputs coming from VBOs
layout (location = 0) in vec3 vertex_position; // vertex position in local space (x,y,z)
layout (location = 1) in vec3 vertex_normal;   // vertex normal in local space   (nx,ny,nz)
layout (location = 2) in vec3 vertex_color;    // vertex color      (r,g,b)
layout (location = 3) in vec2 vertex_uv;       // vertex uv-texture (u,v)

// Output variables sent to the fragment shader
out struct fragment_data
{
    vec3 position; // vertex position in world space
    vec3 normal;   // normal position in world space
    vec3 color;    // vertex color
    vec2 uv;       // vertex uv
} fragment;

// Uniform variables expected to receive from the C++ program
uniform mat4 model; // Model affine transform matrix associated to the current shape
uniform mat4 view;  // View matrix (rigid transform) of the camera
uniform mat4 projection; // Projection (perspective or orthogonal) matrix of the camera

// Additional uniform variable representing the time for the deformation
uniform float time;
uniform float xm;
uniform float ym;
uniform bool statusm;

// Deformer function
//  Computes the procedural deformation z =0.02*cos(t) r2 (cos(r)+1) exp(-0.0r2);

float alt(float x, float y){
	float r = sqrt(x*x+y*y)*5;
	float le_cos = (-cos(r)+1);
	float l_exp = exp(-0.005*r*r);
	float le_time = cos(time*2-0.8);

	return 0.1*le_time*le_cos*l_exp;
}

vec3 deformer(vec3 p0)
{
	vec3 p = vec3(p0.x +xm, p0.y+ym, alt(p0.x, p0.y));
	return p;
}

// Deformer function for the normal
//  Compute the partial derivative of the function
vec3 deformer_normal(vec3 p0)
{
	float r = sqrt(p0.x*p0.x+p0.y*p0.y)*5;
	float le_cos = (-cos(r)+1);
	float l_exp = exp(-0.005*r*r);
	float le_time = cos(time*2-0.8);

	// Compute exact normals after deformation
	vec3 dpdx = vec3(1.0, 0.0, 	0.1*le_time*25*p0.x*l_exp*(sin(r)/r-0.005*2*le_cos)	);
	vec3 dpdy = vec3(0.0, 1.0,	0.1*le_time*25*p0.y*l_exp*(sin(r)/r-0.005*2*le_cos) );
	vec3 n = normalize(cross(dpdx,dpdy));


	return n;
}

void main() {

	vec3 p, n;

	if (statusm) {
		// Apply deformation
		p = deformer(vertex_position);
		n = deformer_normal(vertex_position);
	} else {
		p = vertex_position;
		n = vertex_normal;
	}

	// The position of the vertex in the world space
	vec4 position = model * vec4(p, 1.0);

	// The normal of the vertex in the world space
	mat4 modelNormal = transpose(inverse(model));
	vec4 normal = modelNormal * vec4(n, 0.0);

	// The projected position of the vertex in the normalized device coordinates:
	vec4 position_projected = projection * view * position;

	// Fill the parameters sent to the fragment shader
	fragment.position = position.xyz;
	fragment.normal   = normal.xyz;
	fragment.color = vertex_color;
	fragment.uv = vertex_uv;

	// gl_Position is a built-in variable which is the expected output of the vertex shader
	gl_Position = position_projected; // gl_Position is the projected vertex position (in normalized device coordinates)
}
