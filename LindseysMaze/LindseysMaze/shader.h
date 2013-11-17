
void printShaderInfoLog(GLuint obj)
{
	int length = 0;
	int charsWritten  = 0;
	char *infoLog;
	
	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &length);
	
	if ( length > 0 )
	{
		infoLog = new char[ length + 1 ];

		glGetShaderInfoLog( obj, length, &charsWritten, infoLog );
		infoLog[ charsWritten ] = '\0';

		printf( "Shader Info : %s\n", infoLog );
		
		delete[] infoLog;
	}
}

void printProgramInfoLog(GLuint obj)
{
	int length = 0;
	int charsWritten  = 0;
	char *infoLog;
	
	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &length);
	
	if ( length > 0)
	{
		infoLog = new char[ length + 1 ];
		glGetProgramInfoLog(obj, length, &charsWritten, infoLog);
		infoLog[ charsWritten ] = '\0';

		printf( "Program Shader : %s\n", infoLog );

		delete[] infoLog;
	}
}

GLuint createShader(std::string file_name)
{	
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	GLuint pixel_shader = glCreateShader(GL_FRAGMENT_SHADER);
	
	// obtain file sizes:
	int fsize1, fsize2, bytes_read;
	std::string vert_shader = file_name + ".vert";
	std::string frag_shader = file_name + ".frag";
    
	struct stat statBuf;
	stat(vert_shader.c_str(), &statBuf);
	fsize1 = statBuf.st_size;
	stat(frag_shader.c_str(), &statBuf);
	fsize2 = statBuf.st_size;
	
	FILE *f1, *f2;
	errno_t err1, err2;
	err1 = fopen_s(&f1, vert_shader.c_str(), "r");
	err2 = fopen_s(&f2, frag_shader.c_str(), "r");

	if (err1 || err2)  {
		fprintf(stderr, "Error opening %s and/or %s", vert_shader.c_str(), frag_shader.c_str());
		return 0;
	}
	
	// allocate memory to contain the whole file:
	GLchar * shader_buffer[1];
	shader_buffer[0] = (GLchar*)malloc(max(fsize1, fsize2));
	
	// Read in the fragment shader
	bytes_read = fread ((void*)shader_buffer[0],1,fsize2,f2);
	glShaderSource(pixel_shader, 1, (const GLchar**)shader_buffer, &bytes_read);
	
	// Read in the vertex shader
	bytes_read = fread ((void*)shader_buffer[0],1,fsize1,f1);
	glShaderSource(vertex_shader, 1, (const GLchar**)shader_buffer, &bytes_read);
	
	free(shader_buffer[0]);
	
	GLint status;
	glCompileShader(vertex_shader);
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		printShaderInfoLog( vertex_shader );
		return 0;
	}
	glCompileShader(pixel_shader);
	glGetShaderiv(pixel_shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		printShaderInfoLog(pixel_shader);
		return 0;
	}
	
	GLuint shader_program = glCreateProgram();
	glAttachShader(shader_program,vertex_shader);
	glAttachShader(shader_program,pixel_shader);
    
	glLinkProgram(shader_program);
	glGetProgramiv(shader_program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		printProgramInfoLog(shader_program);
	}
    
	return shader_program;
}
