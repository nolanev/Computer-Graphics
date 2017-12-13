
//Some Windows Headers (For Time, IO, etc.)
#define  _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <mmsystem.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include "maths_funcs.h"

// Assimp includes

#include <assimp/cimport.h> // C importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations
#include <stdio.h>
#include <math.h>
#include <vector> // STL dynamic memory.
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
/*----------------------------------------------------------------------------
                   MESH TO LOAD
  ----------------------------------------------------------------------------*/
// this mesh is a dae file format but you should be able to use any other format too, obj is typically what is used
// put the mesh in your project directory, or provide a filepath for it here
#define COW_MESH "C:\\Users\\evakn\\Documents\\CS4052 Computer Graphics\\MyObjects\\Textures\\Cow.obj"
//#define COW_MESH "C:\\Users\\evakn\\Documents\\CS4052 Computer Graphics\\MyObjects\\daisy2.obj"
#define GROUND_MESH "C:\\Users\\evakn\\Documents\\CS4052 Computer Graphics\\MyObjects\\ground1.obj"
#define COW_TEXTURE "C:\\Users\\evakn\\Documents\\CS4052 Computer Graphics\\MyObjects\\Textures\\cowskin.png"
#define GROUND_TEXTURE "C:\\Users\\evakn\\Documents\\CS4052 Computer Graphics\\MyObjects\\Textures\\grass2.png"
/*----------------------------------------------------------------------------
  ----------------------------------------------------------------------------*/
int cow_count = 0;
int ground_count = 0;
GLuint GROUND_ID = 2;
GLuint COW_ID = 1;
GLuint COW_SKIN_ID = 1;
GLuint GROUND_SKIN_ID = 1;


std::vector<float> g_vp, g_vn, g_vt;
int g_point_count = 0;


// Macro for indexing vertex buffer
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

using namespace std;
GLuint shaderProgramID;


unsigned int mesh_vao = 0;
int width = 800;
int height = 600; //!!

GLuint loc1, loc2, loc3;
GLfloat rotate_y = 0.0f;

GLfloat camerarotationy = 0.0f;

vec3 cameraPosition = vec3(0, 0, -1000.0f);
vec3 cameraDirection = vec3(0.0f, 0.0f, 0.0f); // looking in +z direction by default
vec3 cameraUpVector = vec3(0.0f, 1.0f, 0.0f);

#pragma region MESH LOADING
/*----------------------------------------------------------------------------
                   MESH LOADING FUNCTION
  ----------------------------------------------------------------------------*/

bool load_mesh (const char* file_name) {
  const aiScene* scene = aiImportFile (file_name, aiProcess_Triangulate); // TRIANGLES!
        //fprintf (stderr, "ERROR: reading mesh %s\n", file_name);			//this error is happening
  if (!scene) {
    fprintf (stderr, "ERROR: reading mesh %s\n", file_name);
    return false;
  }
  printf ("  %i animations\n", scene->mNumAnimations);
  printf ("  %i cameras\n", scene->mNumCameras);
  printf ("  %i lights\n", scene->mNumLights);
  printf ("  %i materials\n", scene->mNumMaterials);
  printf ("  %i meshes\n", scene->mNumMeshes);
  printf ("  %i textures\n", scene->mNumTextures);
  
  for (unsigned int m_i = 0; m_i < scene->mNumMeshes; m_i++) {
	  const aiMesh* mesh = scene->mMeshes[m_i];
	  printf("    %i vertices in mesh\n", mesh->mNumVertices); //TODO THIS IS RUNNING 17 TIMES
	  g_vp.clear();
	  g_vn.clear();
	  g_vt.clear();
	
	  if (file_name == COW_MESH) {
		  printf("hello cow \n");
		  cow_count = mesh->mNumVertices;
	  }
	  else if (file_name == GROUND_MESH) {
		  printf("hello ground \n");
		  ground_count = mesh->mNumVertices;
	  }
	  g_point_count = mesh->mNumVertices;
   
	for (unsigned int v_i = 0; v_i < mesh->mNumVertices; v_i++) {
		if (mesh->HasPositions ()) {
			//printf("yep");
			const aiVector3D* vp = &(mesh->mVertices[v_i]);
			//printf ("      vp %i (%f,%f,%f)\n", v_i, vp->x, vp->y, vp->z);
			g_vp.push_back (vp->x);
			g_vp.push_back (vp->y);
			g_vp.push_back (vp->z);
      }
      if (mesh->HasNormals ()) {
		const aiVector3D* vn = &(mesh->mNormals[v_i]);
        //printf ("      vn %i (%f,%f,%f)\n", v_i, vn->x, vn->y, vn->z);
        g_vn.push_back (vn->x);
        g_vn.push_back (vn->y);
        g_vn.push_back (vn->z);
      }
      if (mesh->HasTextureCoords (0)) {
        const aiVector3D* vt = &(mesh->mTextureCoords[0][v_i]);
        //printf ("      vt %i (%f,%f)\n", v_i, vt->x, vt->y);
        g_vt.push_back (vt->x);
        g_vt.push_back (vt->y);
      }
      if (mesh->HasTangentsAndBitangents ()) {
        // NB: could store/print tangents here
      }
    }
  }
  
  aiReleaseImport (scene);
  return true;
}

#pragma endregion MESH LOADING

// Shader Functions- click on + to expand
#pragma region SHADER_FUNCTIONS

// Create a NULL-terminated string by reading the provided file
char* readShaderSource(const char* shaderFile) {   
    FILE* fp = fopen(shaderFile, "rb"); //!->Why does binary flag "RB" work and not "R"... wierd msvc thing?

    if ( fp == NULL ) { return NULL; }

    fseek(fp, 0L, SEEK_END);
    long size = ftell(fp);

    fseek(fp, 0L, SEEK_SET);
    char* buf = new char[size + 1];
    fread(buf, 1, size, fp);
    buf[size] = '\0';

    fclose(fp);

    return buf;
}


static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	// create a shader object
    GLuint ShaderObj = glCreateShader(ShaderType);

    if (ShaderObj == 0) {
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        exit(0);
    }
	const char* pShaderSource = readShaderSource( pShaderText);

	// Bind the source code to the shader, this happens before compilation
	glShaderSource(ShaderObj, 1, (const GLchar**)&pShaderSource, NULL);
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
void generateTexture(GLuint& texture_id, char* file_name)
{
	//printf("got here");
	int width, height, n;
	unsigned char* image = stbi_load(file_name, &width, &height, &n, STBI_rgb);

	glGenTextures(1, &texture_id);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);
}



GLuint CompileShaders()
{
	//Start the process of setting up our shaders by creating a program ID
	//Note: we will link all the shaders together into this ID
    shaderProgramID = glCreateProgram();
    if (shaderProgramID == 0) {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }

	// Create two shader objects, one for the vertex, and one for the fragment shader
    AddShader(shaderProgramID, "../Shaders/testVertexShader.txt", GL_VERTEX_SHADER);
    AddShader(shaderProgramID, "../Shaders/PhongFragmentShader.txt", GL_FRAGMENT_SHADER);

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
	return shaderProgramID;
}
#pragma endregion SHADER_FUNCTIONS

// VBO Functions - click on + to expand
#pragma region VBO_FUNCTIONS

void generateObjectBufferMesh(GLuint &vao, const char* file_name, int &count ) {
//void generateObjectBufferMesh() {
/*----------------------------------------------------------------------------
                   LOAD MESH HERE AND COPY INTO BUFFERS
  ----------------------------------------------------------------------------*/

	//Note: you may get an error "vector subscript out of range" if you are using this code for a mesh that doesnt have positions and normals
	//Might be an idea to do a check for that before generating and binding the buffer.

	load_mesh(file_name);
	unsigned int vp_vbo = 0;
	loc1 = glGetAttribLocation(shaderProgramID, "vertex_position");
	loc2 = glGetAttribLocation(shaderProgramID, "vertex_normal");
	//loc3 = glGetAttribLocation(shaderProgramID, "vertex_texture");

	glGenBuffers (1, &vp_vbo);
	glBindBuffer (GL_ARRAY_BUFFER, vp_vbo);
	glBufferData (GL_ARRAY_BUFFER, g_point_count * 3 * sizeof (float), &g_vp[0], GL_STATIC_DRAW);
	unsigned int vn_vbo = 0;
	glGenBuffers (1, &vn_vbo);
	glBindBuffer (GL_ARRAY_BUFFER, vn_vbo);
	glBufferData (GL_ARRAY_BUFFER, g_point_count * 3 * sizeof (float), &g_vn[0], GL_STATIC_DRAW);

//	This is for texture coordinates which you don't currently need, so I have commented it out
	unsigned int vt_vbo = 0;
	glGenBuffers (1, &vt_vbo);
	glBindBuffer (GL_ARRAY_BUFFER, vt_vbo);
	glBufferData (GL_ARRAY_BUFFER, g_point_count * 2 * sizeof (float), &g_vt[0], GL_STATIC_DRAW);
	
	//unsigned int vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray (vao);

	glEnableVertexAttribArray (loc1);
	glBindBuffer (GL_ARRAY_BUFFER, vp_vbo);
	glVertexAttribPointer (loc1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray (loc2);
	glBindBuffer (GL_ARRAY_BUFFER, vn_vbo);
	glVertexAttribPointer (loc2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

//	This is for texture coordinates which you don't currently need, so I have commented it out
	glEnableVertexAttribArray (loc3);
	glBindBuffer (GL_ARRAY_BUFFER, vt_vbo);
	glVertexAttribPointer (loc3, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindVertexArray(0);
}


#pragma endregion VBO_FUNCTIONS


void display(){

	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable (GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc (GL_LESS); // depth-testing interprets a smaller value as "closer"
	glClearColor (0.5f, 0.5f, 0.5f, 1.0f);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram (shaderProgramID);


	//Declare your uniform variables that will be used in your shader
	int matrix_location = glGetUniformLocation (shaderProgramID, "model");
	int view_mat_location = glGetUniformLocation (shaderProgramID, "view");
	int proj_mat_location = glGetUniformLocation (shaderProgramID, "proj");
	int texture_location = glGetUniformLocation(shaderProgramID, "tex");

	cameraDirection.v[0] = sin(camerarotationy);
	cameraDirection.v[2] = cos(camerarotationy);
	mat4 view1 = look_at(cameraPosition, cameraPosition + cameraDirection, cameraUpVector);

	//mat4 view1 = identity_mat4();
	mat4 persp_proj1 = perspective(45.0, (float)width / (float)height, 0.1, 3000.0);
	
	
	
	mat4 model1 = identity_mat4();
	model1= rotate_y_deg(model1,90);
	model1=translate(model1, vec3(0.0, -200.0f, 100.0f));
	mat4 Smat = { 0.50f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.50f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.50f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	model1 = model1*Smat;
	mat4 global1 = model1;

	// update uniforms & draw
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, persp_proj1.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view1.m);
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, global1.m);

	glBindTexture(GL_TEXTURE_2D, COW_SKIN_ID);
	glUniform1i(texture_location, 0);

	glBindVertexArray(COW_ID);
	glDrawArrays (GL_TRIANGLES, 0, cow_count);

	//cow 2
	mat4 model2 = identity_mat4();

	model2 = rotate_y_deg(model2, 145);
	model2 = translate(model2, vec3(600.0, -200.0f, 100.0f));
	model2 = model2*Smat;
	mat4 global2 = model2;

	// update uniforms & draw	
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, global2.m);
	glBindTexture(GL_TEXTURE_2D, COW_SKIN_ID);
	glUniform1i(texture_location, 0);
	glBindVertexArray(COW_ID);
	glDrawArrays(GL_TRIANGLES, 0, cow_count);

	//ground	
	mat4 model3 = identity_mat4();	
	model3 = translate(model3, vec3(0.0, -200.0f, 1600.0f));
	mat4 Smat3 = { 50.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 50.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 50.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	model3 = model3*Smat3;
	mat4 global3 = model3;

	// update uniforms & draw
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, global3.m);
	glBindTexture(GL_TEXTURE_2D, GROUND_SKIN_ID);
	glUniform1i(texture_location, 0);
	glBindVertexArray(GROUND_ID);
	glDrawArrays(GL_TRIANGLES, 0, ground_count);
    glutSwapBuffers();
}


void updateScene() {	

	// Placeholder code, if you want to work with framerate
	// Wait until at least 16ms passed since start of last frame (Effectively caps framerate at ~60fps)
	static DWORD  last_time = 0;
	DWORD  curr_time = timeGetTime();
	float  delta = (curr_time - last_time) * 0.001f;
	if (delta > 0.03f)
		delta = 0.03f;
	last_time = curr_time;

	// rotate the model slowly around the y axis
	rotate_y+=3.0f;
	// Draw the next frame
	glutPostRedisplay();
}


void init()
{
	// Set up the shaders
	GLuint shaderProgramID = CompileShaders();
	generateObjectBufferMesh(COW_ID, COW_MESH, cow_count);
	generateObjectBufferMesh(GROUND_ID, GROUND_MESH, ground_count);
	
	//generateObjectBufferMesh();
	// load mesh into a vertex buffer array
//	generateObjectBufferMesh();
	generateTexture(COW_SKIN_ID, COW_TEXTURE);
	generateTexture(GROUND_SKIN_ID, GROUND_TEXTURE);
}

// Placeholder code for the keypress
void keypress(unsigned char key, int x, int y) {

	if (key == 'l') {
		camerarotationy += 0.01f;
	}
	if (key == 'r') {
		camerarotationy -= 0.01f;
	
	}
	glutPostRedisplay();

}
void processSpecialKeys(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		cameraPosition = cameraPosition + vec3(0.0f, 0.0f, 20.0f); 
		break;
	case GLUT_KEY_DOWN:
		cameraPosition = cameraPosition - vec3(0.0f, 0.0f, 20.0f);
		break;
	case GLUT_KEY_LEFT:
		cameraPosition = cameraPosition + vec3(5.0f, 0.0f, -2.0f);
		break;
	case GLUT_KEY_RIGHT:
		cameraPosition = cameraPosition - vec3(5.0f, 0.0f, -2.0f);
	}
	glutPostRedisplay();
}

int main(int argc, char** argv){

	// Set up the window
	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(width, height);
    glutCreateWindow("Hello Cows!");

	// Tell glut where the display function is
	glutDisplayFunc(display);
	glutIdleFunc(updateScene);
	glutKeyboardFunc(keypress);
	glutSpecialFunc(processSpecialKeys);

	 // A call to glewInit() must be done after glut is initialized!
    GLenum res = glewInit();
	// Check for any errors
    if (res != GLEW_OK) {
      fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
      return 1;
    }
	// Set up your objects and shaders
	init();
	// Begin infinite event loop
	glutMainLoop();
    return 0;
}











