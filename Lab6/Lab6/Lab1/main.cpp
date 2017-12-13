
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
#define FLOWER_MESH "C:\\Users\\evakn\\Documents\\CS4052 Computer Graphics\\MyObjects\\daisy.obj"
#define FLOWER_TEXTURE "C:\\Users\\evakn\\Documents\\CS4052 Computer Graphics\\MyObjects\\Textures\\yellow.png"

#define COW_MESH "C:\\Users\\evakn\\Documents\\CS4052 Computer Graphics\\MyObjects\\cow2.obj"
#define COW_TEXTURE "C:\\Users\\evakn\\Documents\\CS4052 Computer Graphics\\MyObjects\\Textures\\cowskin.jpg"

#define GROUND_MESH "C:\\Users\\evakn\\Documents\\CS4052 Computer Graphics\\MyObjects\\bumpygrass.obj"
#define GROUND_TEXTURE "C:\\Users\\evakn\\Documents\\CS4052 Computer Graphics\\MyObjects\\Textures\\grass3.png"

#define SKY_MESH "C:\\Users\\evakn\\Documents\\CS4052 Computer Graphics\\MyObjects\\sky.obj"
#define SKY_TEXTURE "C:\\Users\\evakn\\Documents\\CS4052 Computer Graphics\\MyObjects\\Textures\\sky.png"

#define PIG_MESH "C:\\Users\\evakn\\Documents\\CS4052 Computer Graphics\\MyObjects\\piggy2.obj"
#define PIG_TEXTURE "C:\\Users\\evakn\\Documents\\CS4052 Computer Graphics\\MyObjects\\Textures\\pigskin.jpg"

#define BARN_GROUND_MESH "C:\\Users\\evakn\\Documents\\CS4052 Computer Graphics\\MyObjects\\ground2.obj"
#define BARN_MESH "C:\\Users\\evakn\\Documents\\CS4052 Computer Graphics\\MyObjects\\3dwalls.obj"
#define BARN_BOARDS_TEXTURE "C:\\Users\\evakn\\Documents\\CS4052 Computer Graphics\\MyObjects\\Textures\\barnboards.png"
#define BARN_GROUND_TEXTURE "C:\\Users\\evakn\\Documents\\CS4052 Computer Graphics\\MyObjects\\Textures\\dirt2.png"
#define BARN_ROOF_MESH "C:\\Users\\evakn\\Documents\\CS4052 Computer Graphics\\MyObjects\\Roof.obj"
#define BARN_ROOF_TEXTURE "C:\\Users\\evakn\\Documents\\CS4052 Computer Graphics\\MyObjects\\Textures\\rust.png"
//#define BARN_GROUND_TEXTURE "C:\\Users\\evakn\\Documents\\CS4052 Computer Graphics\\MyObjects\\Textures\\barnground.png"
#define BARN_RUST_TEXTURE "C:\\Users\\evakn\\Documents\\CS4052 Computer Graphics\\MyObjects\\Textures\\barnrust.png"
#define BARN_WOOD_TEXTURE "C:\\Users\\evakn\\Documents\\CS4052 Computer Graphics\\MyObjects\\Textures\\barnred.png"
//#define BARN_GREY_TEXTURE "C:\\Users\\evakn\\Documents\\CS4052 Computer Graphics\\MyObjects\\Textures\\cowskin.png"



  /*----------------------------------------------------------------------------
  ----------------------------------------------------------------------------*/
int cow_count = 0;
int ground_count = 0;
int flower_count = 0;
int barn_count = 0;
int barn_ground_count = 0;
int sky_count = 0;
int barn_roof_count = 0;
int pig_count = 0;
GLuint COW_ID = 1;
GLuint COW_SKIN_ID = 1;
GLuint GROUND_ID = 2;
GLuint GROUND_SKIN_ID = 2;
GLuint SKY_ID = 8;
GLuint SKY_SKIN_ID = 8;
GLuint BARN_ID = 3;
GLuint BARN_SKIN_ID = 3;
GLuint BARN_GROUND_ID = 6;
GLuint BARN_GROUND_SKIN_ID = 6;
GLuint BARN_ROOF_ID = 7;
GLuint BARN_ROOF_SKIN_ID = 7;
GLuint PIG_ID = 4;
GLuint PIG_SKIN_ID = 4;
GLuint FLOWER_ID = 5;
GLuint FLOWER_SKIN_ID = 5;

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

vec3 cameraPosition = vec3(0, 0, -1600.0f);
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
	  else if (file_name == SKY_MESH) {
		  printf("hello sky \n");
		  sky_count = mesh->mNumVertices;
	  }
	  else if (file_name == BARN_ROOF_MESH) {
		  printf("hello roof \n");
		  barn_roof_count = mesh->mNumVertices;
	  }
	  else if (file_name == FLOWER_MESH) {
		  printf("hello ground \n");
		  flower_count = mesh->mNumVertices;
	  }
	  else if (file_name == BARN_MESH) {
		  printf("hello barn \n");
		  barn_count = mesh->mNumVertices;
	  }
	  else if (file_name == BARN_GROUND_MESH) {
		  printf("hello barn GROUND\n");
		  barn_ground_count = mesh->mNumVertices;
	  }
	  else if (file_name == PIG_MESH) {
		  printf("hello pig \n");
		  pig_count = mesh->mNumVertices;
	  }
	  else if (file_name == FLOWER_MESH) {
		  printf("hello flower\n");
		  flower_count = mesh->mNumVertices;
	  }

	  g_point_count = mesh->mNumVertices;
   
	for (unsigned int v_i = 0; v_i < mesh->mNumVertices; v_i++) {
		if (mesh->HasPositions ()) { //true
			//printf("yep");
			const aiVector3D* vp = &(mesh->mVertices[v_i]);
			//printf ("      vp %i (%f,%f,%f)\n", v_i, vp->x, vp->y, vp->z);
			g_vp.push_back (vp->x);
			g_vp.push_back (vp->y);
			g_vp.push_back (vp->z);
      }
      if (mesh->HasNormals ()) { //true
		const aiVector3D* vn = &(mesh->mNormals[v_i]);
        //printf ("      vn %i (%f,%f,%f)\n", v_i, vn->x, vn->y, vn->z);
        g_vn.push_back (vn->x);
        g_vn.push_back (vn->y);
        g_vn.push_back (vn->z);
      }
      if (mesh->HasTextureCoords (0)) { //false
        const aiVector3D* vt = &(mesh->mTextureCoords[0][v_i]);
        //printf ("      vt %i (%f,%f)\n", v_i, vt->x, vt->y);
        g_vt.push_back (vt->x);
        g_vt.push_back (vt->y);
	//	g_vt.push_back(vt->z);	TODOD
      }
      if (mesh->HasTangentsAndBitangents ()) { //false
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
	glBufferData (GL_ARRAY_BUFFER, g_point_count * 2 * sizeof (float), &g_vt[0], GL_STATIC_DRAW);   //this line!!!
	
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
	mat4 persp_proj1 = perspective(45.0, (float)width / (float)height, 0.1, 30000.0);
	
	
	//pig
	mat4 model1 = identity_mat4();
	model1= rotate_y_deg(model1,240);
	mat4 Smat = { 02.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 02.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 02.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	model1 = model1*Smat;
	model1 = rotate_y_deg(model1, -80);
	model1 = translate(model1, vec3(-400.0, -340.0f, 50.0f));
	mat4 global1 = model1;

	// update uniforms & draw
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, persp_proj1.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view1.m);
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, global1.m);

	glBindTexture(GL_TEXTURE_2D, PIG_SKIN_ID);
	glUniform1i(texture_location, 0);
	glBindVertexArray(PIG_ID);
	glDrawArrays (GL_TRIANGLES, 0, pig_count);


	//cow
	mat4 model2 = identity_mat4();
	mat4 Smat6 = { 0.75f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.75f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.75f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	model2 = model2*Smat6;
	model2 = rotate_y_deg(model2, -120);
	model2 = translate(model2, vec3(100.0, -340.0f, -700.0f));	
	mat4 global2 = model2;
	// update uniforms & draw	
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, global2.m);
	glBindTexture(GL_TEXTURE_2D, COW_SKIN_ID);
	glUniform1i(texture_location, 0);
	glBindVertexArray(COW_ID);
	glDrawArrays(GL_TRIANGLES, 0, cow_count);



	//flower
	mat4 model12 = identity_mat4();
	
	model12 = model12*Smat6*Smat6*Smat6;
	model12 = rotate_x_deg(model12, -60);
	//model12 = translate(model12, vec3(-205.0, 300.0f, 0.0f));
	model12 = translate(model12, vec3(-115.0, 180.0f, 0.0f));
	mat4 global12 = global2*model12;

	// update uniforms & draw	
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, global12.m);
	glBindTexture(GL_TEXTURE_2D, FLOWER_SKIN_ID);
	glUniform1i(texture_location, 0);
	glBindVertexArray(FLOWER_ID);
	glDrawArrays(GL_TRIANGLES, 0, flower_count);

	//BARN
	mat4 model4 = identity_mat4();
	mat4 Scale_Barn = { 10.00f, 0.0f, 0.0f, 0.0f,
		0.0f, 10.00f, 0.0f, 0.0f,
		0.0f, 0.0f, 10.00f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	vec3 barnpos = { -700.0f, -370.0f, 100.0f };
	model4 = model4*Scale_Barn;
	model4 = rotate_y_deg(model4, 240);
	model4 = translate(model4, barnpos);

	mat4 global4 = model4;
	// update uniforms & draw	
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, global4.m);
	glBindTexture(GL_TEXTURE_2D, BARN_SKIN_ID);
	glUniform1i(texture_location, 0);
	glBindVertexArray(BARN_ID);
	glDrawArrays(GL_TRIANGLES, 0, barn_count);















	//barn floor
	mat4 model5 = identity_mat4();
	mat4 global5 =global4*model5;

	// update uniforms & draw	
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, global5.m);
	glBindTexture(GL_TEXTURE_2D, BARN_GROUND_SKIN_ID);
	glUniform1i(texture_location, 0);
	glBindVertexArray(BARN_GROUND_ID);
	glDrawArrays(GL_TRIANGLES, 0, barn_ground_count);








	//barn roof
	mat4 model6 = identity_mat4();
	mat4 global6 = global4* model6;

	// update uniforms & draw	
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, global6.m);
	glBindTexture(GL_TEXTURE_2D, BARN_ROOF_SKIN_ID);
	glUniform1i(texture_location, 0);
	glBindVertexArray(BARN_ROOF_ID);
	glDrawArrays(GL_TRIANGLES, 0, barn_roof_count);
	
	
	
	//ground	
	mat4 model3 = identity_mat4();	
	mat4 Smat3 = { 10.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 10.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 10.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	model3 = model3*Smat3;
	model3 = rotate_y_deg(model3, 90);
	model3 = translate(model3, vec3(-1000.0, -400.0f, 1600.0f));
	
	mat4 global3 = model3;
	// update uniforms & draw
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, global3.m);
	glBindTexture(GL_TEXTURE_2D, GROUND_SKIN_ID);
	glUniform1i(texture_location, 0);
	glBindVertexArray(GROUND_ID);
	glDrawArrays(GL_TRIANGLES, 0, ground_count);













	//sky	
	mat4 model7 = identity_mat4();
	model7 = model7*Smat3*Smat3;
	model7 = rotate_x_deg(model7, 90);
	model7 = rotate_z_deg(model7, 90);
	
	model7 = translate(model7, vec3(-1500.0, -200.0f, 1350.0f));

	mat4 global7 = model7;

	// update uniforms & draw
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, global7.m);
	glBindTexture(GL_TEXTURE_2D, SKY_SKIN_ID);
	glUniform1i(texture_location, 0);
	glBindVertexArray(SKY_ID);
	glDrawArrays(GL_TRIANGLES, 0, sky_count);


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
	generateObjectBufferMesh(SKY_ID, SKY_MESH, sky_count);
	generateObjectBufferMesh(FLOWER_ID, FLOWER_MESH, flower_count);
	generateObjectBufferMesh(PIG_ID, PIG_MESH, pig_count);
	generateObjectBufferMesh(BARN_ID, BARN_MESH, barn_count);
	generateObjectBufferMesh(BARN_GROUND_ID, BARN_GROUND_MESH, barn_ground_count);

	generateObjectBufferMesh(BARN_ROOF_ID, BARN_ROOF_MESH, barn_roof_count);
	//generateObjectBufferMesh();
	// load mesh into a vertex buffer array
//	generateObjectBufferMesh();
	generateTexture(COW_SKIN_ID, COW_TEXTURE);
	generateTexture(GROUND_SKIN_ID, GROUND_TEXTURE);
	generateTexture(SKY_SKIN_ID, SKY_TEXTURE);

	generateTexture(PIG_SKIN_ID, PIG_TEXTURE);

	generateTexture(FLOWER_SKIN_ID, FLOWER_TEXTURE);

	generateTexture(BARN_SKIN_ID, BARN_WOOD_TEXTURE);
	generateTexture(BARN_GROUND_SKIN_ID, BARN_GROUND_TEXTURE);
	generateTexture(BARN_ROOF_SKIN_ID, BARN_ROOF_TEXTURE);
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
	vec3 Left_vec;
	vec3 Right_vec;
	switch (key)
	{
	case GLUT_KEY_UP:
		cameraPosition += (cameraDirection*20.0); 
		break;
	case GLUT_KEY_DOWN:
		cameraPosition -= (cameraDirection*20.0);
		break;
	case GLUT_KEY_LEFT:
		
		Left_vec = cross(cameraDirection, cameraUpVector);
		normalise(Left_vec);
		Left_vec = Left_vec *-10.0f;
		cameraPosition += Left_vec;
		break;
	case GLUT_KEY_RIGHT:
		Right_vec = cross(cameraDirection, cameraUpVector);
		normalise(Right_vec);
		Right_vec = Right_vec *10.0f;
		cameraPosition += Right_vec;
		break;
	}
	glutPostRedisplay();
}

void mouseWheel(int button, int dir, int x, int y)
{
	if (dir > 0)
		camerarotationy += 0.01f;
	else
		camerarotationy -= 0.01f;
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
	glutMouseWheelFunc(mouseWheel);
	//glutMouseFunc
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











