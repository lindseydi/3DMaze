varying vec3 normal;
varying vec3 position;

void main( void ) {
	vec3 light = gl_LightSource[0].position.xyz;

	vec3 pl = normalize( light - position );
	vec3 n = normalize( normal );
	vec3 pe = normalize( -position );
	vec3 ref = dot( pl, n ) * n * 2.0 - pl;

	float diffuse = clamp( dot( pl, n ), 0.0, 1.0 );
	float specular = pow( clamp( dot( ref, pe ), 0.0, 1.0 ), 16.0 );

	gl_FragColor = vec4( diffuse * vec3( 0.2, 0.2, 0.2 ) + specular * vec3( 10.0, 10.0, 10.0 ), 1.0 );
}