
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <glm\glm\glm.hpp>
#include <glm\glm\gtc\matrix_transform.hpp>
#include <glm\glm\gtc\type_ptr.hpp>
#include <math.h>


//#include <fstream>
//#include "vertshader.txt"


// Macro for indexing vertex buffer
#define BUFFER_OFFSET(i) ((char *)NULL + (i))
 
using namespace std;

GLuint gScaleLocation;
//GLuint gTranslateLocation;
//GLuint gRotateLocation;
GLuint gChangeLocation;
GLuint gChange2Location;
GLuint Choiceint;
glm::mat4 Cmat;
glm::mat4 Bmat;
float degrees = 22.5 * 3.1415926535897932 / 180.0;


// Vertex Shader (for convenience, it is defined in the main here, but we will be using text files for shaders in future)
// Note: Input to this shader is the vertex positions that we specified for the triangle. 
// Note: gl_Position is a special built-in variable that is supposed to contain the vertex position (in X, Y, Z, W)
// Since our triangle vertices were specified as vec3, we just set W to 1.0.
/*const char* ReadShaderFile() {
	ifstream vshader("vertshader.txt");
	char* pVS;
	if (!vshader.is_open()) {
		cout << "Could not read file " << endl;

	}
	//string line = "";
	/*vshader.seekg(0, vshader.end);
	unsigned long fileLength = (unsigned long)inSdrFileStream.tellg() + 1;
	inSdrFileStream.seekg(0, inSdrFileStream.beg)
	while (!vshader.eof()) {
		vshader.read(pVS, sizeof(vshader));
		//getline(vshader, line);
		//pVS.append(line + "\n");
	}

	vshader.close();
	return pVS;
}

static const char *pVS = ReadShaderFile(); */ 

//gl_Position = TranslationM*vec4(gScale*vPosition.x, gScale*vPosition.y, gScale*vPosition.z, 2.0);  \n\

static const char* pVS = "                                                    \n\
#version 330                                                                  \n\
                                                                              \n\
in vec3 vPosition;															  \n\
in vec4 vColor;																  \n\
out vec4 color;																 \n\
\n\
\n\
uniform mat4 Cmat; \n\
void main()                                                                     \n\
{   gl_Position = Cmat* vec4(vPosition, 5.0f);  \n\
	\n\
	color = vColor;															\n\
}";

// Fragment Shader
static const char* pFS = "                                              \n\
#version 330                                                            \n\
in vec4 color   ;                                                                  \n\
out vec4 FragColor;                                                      \n\
                                                                          \n\
void main()                                                               \n\
{                                                                          \n\
FragColor = vec4(color.x, color.y, color.z, 1.0);									 \n\
}";


// Shader Functions- click on + to expand
#pragma region SHADER_FUNCTIONS
static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	// create a shader object
    GLuint ShaderObj = glCreateShader(ShaderType);

    if (ShaderObj == 0) {
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        exit(0);
    }
	// Bind the source code to the shader, this happens before compilation
	glShaderSource(ShaderObj, 1, (const GLchar**)&pShaderText, NULL);
	// compile the shader and check for errors
    glCompileShader(ShaderObj);
    GLint success;
	// check for shader related errors using glGetShaderiv
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
        exit(1);
    }
	// Attach the compiled shader object to the program object
    glAttachShader(ShaderProgram, ShaderObj);
}

GLuint CompileShaders()
{
	//Start the process of setting up our shaders by creating a program ID
	//Note: we will link all the shaders together into this ID
    GLuint shaderProgramID = glCreateProgram();
    if (shaderProgramID == 0) {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }

	// Create two shader objects, one for the vertex, and one for the fragment shader
    AddShader(shaderProgramID, pVS, GL_VERTEX_SHADER);
    AddShader(shaderProgramID, pFS, GL_FRAGMENT_SHADER);

    GLint Success = 0;
    GLchar ErrorLog[1024] = { 0 };


	// After compiling all shader objects and attaching them to the program, we can finally link it
    glLinkProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
    glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        exit(1);
	}		

	// program has been successfully linked but needs to be validated to check whether the program can execute given the current pipeline state
    glValidateProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
    glGetProgramiv(shaderProgramID, GL_VALIDATE_STATUS, &Success);
    if (!Success) {
        glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        exit(1);
    }
	// Finally, use the linked shader program
	// Note: this program will stay in effect for all draw calls until you replace it with another or explicitly disable its use
    glUseProgram(shaderProgramID);

	//here is where i can put in my things
	//gScaleLocation = glGetUniformLocation(shaderProgramID, "gScale");
	//assert(gScaleLocation != 0xFFFFFFFF);
	//gTranslateLocation = glGetUniformLocation(shaderProgramID, "Tmat");

	//gRotateLocation = glGetUniformLocation(shaderProgramID, "Rmat");
	gChangeLocation = glGetUniformLocation(shaderProgramID, "Cmat");
	gChange2Location = glGetUniformLocation(shaderProgramID, "Cmat");

	return shaderProgramID;
}
#pragma endregion SHADER_FUNCTIONS

// VBO Functions - click on + to expand
#pragma region VBO_FUNCTIONS
GLuint generateObjectBuffer(GLfloat vertices[], GLfloat colors[]) {
	GLuint numVertices =  6;
	// Genderate 1 generic buffer object, called VBO
	GLuint VBO;
 	glGenBuffers(1, &VBO);
	// In OpenGL, we bind (make active) the handle to a target name and then execute commands on that target
	// Buffer will contain an array of vertices 
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// After binding, we now fill our object with data, everything in "Vertices" goes to the GPU
	glBufferData(GL_ARRAY_BUFFER, numVertices*7*sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	// if you have more data besides vertices (e.g., vertex colours or normals), use glBufferSubData to tell the buffer when the vertices array ends and when the colors start
	glBufferSubData (GL_ARRAY_BUFFER, 0, numVertices*3*sizeof(GLfloat), vertices);
	glBufferSubData (GL_ARRAY_BUFFER, numVertices*3*sizeof(GLfloat), numVertices*4*sizeof(GLfloat), colors);
return VBO;
}

void linkCurrentBuffertoShader(GLuint shaderProgramID){
	GLuint numVertices = 6;
	// find the location of the variables that we will be using in the shader program
	GLuint positionID = glGetAttribLocation(shaderProgramID, "vPosition");
	GLuint colorID = glGetAttribLocation(shaderProgramID, "vColor");
	// Have to enable this
	glEnableVertexAttribArray(positionID);
	// Tell it where to find the position data in the currently active buffer (at index positionID)
    glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 0, 0);
	// Similarly, for the color data.
	glEnableVertexAttribArray(colorID);
	glVertexAttribPointer(colorID, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(numVertices*3*sizeof(GLfloat)));
}
#pragma endregion VBO_FUNCTIONS


void display(){

	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	//No change matrix
	 Bmat = { 1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};


	///SCALE
	glm::mat4 Smat = { 2.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 2.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};


	glm::mat4 Snumat = { 2.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};


	///TRANSLATE
	glm::mat4 Rmat = { 1.0f, 0.0f, 0.0f, 0.2f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	glm::mat4 Lmat = { 
		1.0f, 0.0f, 0.0f, -0.2f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	glm::mat4 YTmat = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.2f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	glm::mat4 ZTmat = { 
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.2f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	///ROTATE
	glm::mat4 Amat = { -cosf(degrees), sinf(degrees), 0.0f, 0.0f,
		-sinf(degrees), -cosf(degrees), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};



	glm::mat4 Clmat = { cosf(degrees), -sinf(degrees), 0.0f, 0.0f,
		sinf(degrees), cosf(degrees), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};




	glm::mat4 Xmat = { 1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, cosf(degrees), -sinf(degrees), 0.0f,
		0.0f, sinf(degrees), cosf(degrees), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};




	glm::mat4 Ymat = { cosf(degrees), 0.0f, sinf(degrees), 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		-sinf(degrees), 0.0f, cosf(degrees), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};



	
	//glUniformMatrix4fv(gRotateLocation, 1, GL_TRUE, &Rmat[0][0]);
	switch (Choiceint) {
	case(0): 
		break;
	case (1): //move right
		Cmat = Cmat*Rmat;
		break;
	case (2): //move left
		Cmat = Cmat*Lmat;
		break;
	case(3): //clockwise
		Cmat = Cmat*Clmat;
		break;
	case (4): //anticlockwise
		Cmat = Cmat*Amat;
		break;
	case (5): //scale up
		Cmat = Cmat*Smat;
		break;
	case(6): //scale down
		Cmat = Cmat/Smat;
		break;	
	case(7): //Rotate about Y
		Cmat = Cmat*Ymat;
		break;
	case(8): //Rotate about X
		Cmat = Cmat*Xmat;
		break;
	case(9): //funny scaling
		Cmat = Cmat*Snumat;
		break;
	case(10): //move the other triangle
		Cmat = Bmat*Rmat;
		break;
	case(11): //y translation
		Cmat = Cmat*YTmat;
		break;
	case(12): //x translation
		Cmat = Cmat*ZTmat;
		break;
	}



	glUniformMatrix4fv(gChangeLocation, 1, GL_TRUE, &Cmat[0][0]);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glUniformMatrix4fv(gChangeLocation, 1, GL_TRUE, &Bmat[0][0]);
	glDrawArrays(GL_TRIANGLES, 3, 6);

    glutSwapBuffers();
}

void KeyboardInput(unsigned char Key, int x, int y)
{
	switch (Key)
	{
	case 'r': Choiceint = 1; break;
	case 'l': Choiceint = 2; break;
	case 'c': Choiceint = 3; break;
	case 'a': Choiceint = 4; break;
	case 'u': Choiceint = 5; break;
	case 'd': Choiceint = 6; break;
	case 'y': Choiceint = 7; break;
	case 'x': Choiceint = 8; break;
	case 'p': Choiceint = 9; break;
	case 'o': Choiceint = 10; break;
	case 'q': Choiceint = 11; break;
	case 'w': Choiceint = 12; break;
	case 27: //esc key
		exit(1);
		break;
	};
	glutPostRedisplay();
}


void init()
{
	// Create 3 vertices that make up a triangle that fits on the viewport 
	
	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f, //a
		1.0f, -1.0f, 0.0f,  //b
		0.0f, 1.0f, 0.0f, //c
		1.0f, 1.5f, 0.0f, //b
		1.5f, 0.5f, 0.0f, //c
		0.5f, 0.5f, 0.0f
	};


	// Create a color array that identfies the colors of each vertex (format R, G, B, A)
	GLfloat colors[] = {0.0f, 1.0f, 0.0f, 1.0f,
			1.0f, 0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f
		 };



	// Set up the shaders
	GLuint shaderProgramID = CompileShaders();
	// Put the vertices and colors into a vertex buffer object
	generateObjectBuffer(vertices, colors);
	// Link the current buffer to the shader
	linkCurrentBuffertoShader(shaderProgramID);	
}

int main(int argc, char** argv){

	// Set up the window
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Hello Triangle");
	// Tell glut where the display function is
	glutDisplayFunc(display);

	 // A call to glewInit() must be done after glut is initialized!
    GLenum res = glewInit();
	// Check for any errors
    if (res != GLEW_OK) {
      fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
      return 1;
    }
	// Set up your objects and shaders
	init();
	glutKeyboardFunc(KeyboardInput);
	//glutIdleFunc(display);
	// Begin infinite event loop
	glutMainLoop();
    return 0;
}





					









