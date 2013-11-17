varying vec3 normal;
varying vec3 position;

void main( void ) {
	normal = gl_NormalMatrix * gl_Normal;

	position = ( gl_ModelViewMatrix * gl_Vertex ).xyz;

	//gl_Position = gl_ModelViewProjectionMatrix  * gl_Vertex;
	gl_Position = ftransform();
}