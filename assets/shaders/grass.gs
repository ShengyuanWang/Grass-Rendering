#version 410 core
layout (points) in;
layout (triangle_strip, max_vertices = 36) out;

out GS_OUT {
    vec2 textCoord;
    vec3 fragPos;
    vec3 fragNormal;
    float colorIndex;
} gs_out;
 
uniform mat4 u_view;
uniform mat4 u_projection;
uniform mat4 u_model;
uniform vec3 u_cameraPosition;
uniform float u_time;

mat4 rotationX(in float angle);
mat4 rotationY(in float angle);
mat4 rotationZ(in float angle);
float rand(vec2 co);

const float pi = 3.1415926;
void createQuad(vec3 basePosition) {
    // Create the quad of points relative to the base position.
	vec4 vertexPosition[4];
	vertexPosition[0] = vec4(-0.2, 0.0, 0.0, 0.0); 	// down left
	vertexPosition[1] = vec4( 0.2, 0.0, 0.0, 0.0);	// down right
	vertexPosition[2] = vec4(-0.2, 0.2, 0.0, 0.0);	// up left
	vertexPosition[3] = vec4( 0.2, 0.2, 0.0, 0.0);	// up right

    // Add the texture coordinates
    vec2 textCoords[4];
    textCoords[0] = vec2(0.0, 0.0); // down left
    textCoords[1] = vec2(1.0, 0.0); // down right
    textCoords[2] = vec2(0.0, 1.0); // up left
    textCoords[3] = vec2(1.0, 1.0); // up right

    float random = rand(basePosition.xz);
    float randColor = mix(0, 1, random);
    float randAngle = mix(-180.0, 180.0, random);
    float randGrassSize = mix(0.5, 1.5, random);
    mat4 randomRotationMat = rotationY(randAngle);

    // Add Wind Effect
    float windStrength = 2;
    float windFrequency = 20;
    float windAngle = cos(radians(u_time * windFrequency))/(10/pi);
    vec3 windDirection = vec3(1.0, 0.0, 1.0);
    mat4 windTranslationMat =  (rotationX( windAngle *windDirection[0]) * rotationZ(windAngle * windDirection[2])); // Change the wind angle into matrix form

	for(int i = 0; i < 4; i++) {
		mat4 windEffect = mat4(1.0);
		if (i == 2) windEffect = windTranslationMat; 
	    vec4 worldPosition = gl_in[0].gl_Position + windEffect * randomRotationMat * (vertexPosition[i]) * randGrassSize;
        gl_Position = u_projection * u_view * worldPosition;
        gs_out.textCoord = textCoords[i];
        gs_out.colorIndex = randColor;
        gs_out.fragPos = worldPosition.xyz;
        gs_out.fragNormal = normalize(mat3(transpose(inverse(u_model))) * vec3(0.0, 0.0, 1.0));
		EmitVertex();
    }
    EndPrimitive();
}


// ------------------------------- UITLS -------------------------------
mat4 rotationX( in float angle ) {
	return mat4(	1.0,		0,			0,			0,
			 		0, 	cos(angle),	-sin(angle),		0,
					0, 	sin(angle),	 cos(angle),		0,
					0, 			0,			  0, 		1);
}

mat4 rotationY( in float angle )
{
	return mat4(	cos(angle),		0,		sin(angle),	0,
			 				0,		1.0,			 0,	0,
					-sin(angle),	0,		cos(angle),	0,
							0, 		0,				0,	1);
}

mat4 rotationZ( in float angle ) {
	return mat4(	cos(angle),		-sin(angle),	0,	0,
			 		sin(angle),		cos(angle),		0,	0,
							0,				0,		1,	0,
							0,				0,		0,	1);
}

// simple pseudo-random number generator to generate a random number between 0 and 1.
float rand(vec2 co) {
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

// helper to create grass based on the num of quads.
void createGrass(int numQuads) {
	for (int i = 0; i < numQuads; i++) {
		createQuad(gl_in[0].gl_Position.xyz);
	}
}
// ---------------------------------------------------------------------
 

void main() {
	// LOD distance definations
	const float LOD1 = 3.0f;
	const float LOD2 = 6.0f;
	const float LOD3 = 8.0f;

	float dist = length(gl_in[0].gl_Position.xyz - u_cameraPosition);

	// feather the edge between level of details.
	float randomFactor = 8.0f; 
	dist += randomFactor * mix(-1, 1, rand(gl_in[0].gl_Position.xz));

	// change number of quad function of distance
	int numQuads = 7;
	if (dist > LOD1) numQuads = 5;
	if (dist > LOD2) numQuads = 3;
	if (dist > LOD3) numQuads = 0;
	
	createGrass(numQuads);
} 
 