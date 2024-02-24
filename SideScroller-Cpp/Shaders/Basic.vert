#version 460

// This should correspond to the data stored
// for each vertex in the vertex buffer.
// For now, just a position.
in vec3 inPosition;

void main()
{
	// The vertex shader needs to output a 4D
	// coordinate.
	// For now set the 4th coordinate to 1.0
	gl_Position = vec4(inPosition, 1.0);
}
