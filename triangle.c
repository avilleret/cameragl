/*
Copyright (c) 2012, Broadcom Europe Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// A rotating cube rendered with OpenGL|ES. Three images used as textures on the cube faces.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>

#include "bcm_host.h"

#include "GLES/gl.h"
#include "GLES2/gl2.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"

#include "cube_texture_and_coords.h"
#include "triangle.h"
#include <pthread.h>

#include "state.h"

#define PATH "./"

#ifndef M_PI
   #define M_PI 3.141592654
#endif
	

static void init_ogl(CUBE_STATE_T *state);
static void init_model_proj(CUBE_STATE_T *state);
static void redraw_scene(CUBE_STATE_T *state);
static void init_textures(CUBE_STATE_T *state);
static void exit_func(void);
static volatile int terminate;
static CUBE_STATE_T _state, *state=&_state;

static pthread_t thread1;

/***********************************************************
 * Name: init_ogl
 *
 * Arguments:
 *       CUBE_STATE_T *state - holds OGLES model info
 *
 * Description: Sets the display, OpenGL|ES context and screen stuff
 *
 * Returns: void
 *
 ***********************************************************/
static void init_ogl(CUBE_STATE_T *state)
{
   int32_t success = 0;
   EGLBoolean result;
   EGLint num_config;

   static EGL_DISPMANX_WINDOW_T nativewindow;

   DISPMANX_ELEMENT_HANDLE_T dispman_element;
   DISPMANX_DISPLAY_HANDLE_T dispman_display;
   DISPMANX_UPDATE_HANDLE_T dispman_update;
   VC_RECT_T dst_rect;
   VC_RECT_T src_rect;

   static const EGLint attribute_list[] =
   {
      EGL_RED_SIZE, 8,
      EGL_GREEN_SIZE, 8,
      EGL_BLUE_SIZE, 8,
      EGL_ALPHA_SIZE, 8,
      EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
      EGL_NONE
   };

   static const EGLint context_attributes[] =
   {
      EGL_CONTEXT_CLIENT_VERSION, 2,
      EGL_NONE
   };
  
   EGLConfig config;

   // get an EGL display connection
   state->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
   assert(state->display!=EGL_NO_DISPLAY);

   // initialize the EGL display connection
   result = eglInitialize(state->display, NULL, NULL);
   assert(EGL_FALSE != result);

   // get an appropriate EGL frame buffer configuration
   result = eglChooseConfig(state->display, attribute_list, &config, 1, &num_config);
   assert(EGL_FALSE != result);

   // get an appropriate EGL frame buffer configuration
   result = eglBindAPI(EGL_OPENGL_ES_API);
   assert(EGL_FALSE != result);


   // create an EGL rendering context
   state->context = eglCreateContext(state->display, config, EGL_NO_CONTEXT, context_attributes);
   assert(state->context!=EGL_NO_CONTEXT);

   // create an EGL window surface
   success = graphics_get_display_size(0 /* LCD */, &state->screen_width, &state->screen_height);
   assert( success >= 0 );

   //state->screen_width = 800;
   //state->screen_height = 600;

   dst_rect.x = 0;
   dst_rect.y = 0;
   dst_rect.width = state->screen_width;
   dst_rect.height = state->screen_height;
      
   src_rect.x = 0;
   src_rect.y = 0;
   src_rect.width = state->screen_width << 16;
   src_rect.height = state->screen_height << 16;        

   dispman_display = vc_dispmanx_display_open( 0 /* LCD */);
   dispman_update = vc_dispmanx_update_start( 0 );
         
   dispman_element = vc_dispmanx_element_add ( dispman_update, dispman_display,
      0/*layer*/, &dst_rect, 0/*src*/,
      &src_rect, DISPMANX_PROTECTION_NONE, 0 /*alpha*/, 0/*clamp*/, (DISPMANX_TRANSFORM_T) 0/*transform*/);
      
   nativewindow.element = dispman_element;
   nativewindow.width = state->screen_width;
   nativewindow.height = state->screen_height;
   vc_dispmanx_update_submit_sync( dispman_update );
      
   state->surface = eglCreateWindowSurface( state->display, config, &nativewindow, NULL );
   assert(state->surface != EGL_NO_SURFACE);

   // connect the context to the surface
   result = eglMakeCurrent(state->display, state->surface, state->surface, state->context);
   assert(EGL_FALSE != result);

   // Set background color and clear buffers
   glClearColor(0.15f, 0.25f, 0.35f, 1.0f);

   // Enable back face culling.
   glEnable(GL_CULL_FACE);
   glEnable(GL_DEPTH_TEST);

//   glMatrixMode(GL_MODELVIEW);
}

/***********************************************************
 * Name: init_model_proj
 *
 * Arguments:
 *       CUBE_STATE_T *state - holds OGLES model info
 *
 * Description: Sets the OpenGL|ES model to default values
 *
 * Returns: void
 *
 ***********************************************************/
static void init_model_proj(CUBE_STATE_T *state)
{
   float farp = 500.0f;
   float hht;
   float hwd;
   glViewport(0, 0, (GLsizei)state->screen_width, (GLsizei)state->screen_height);
      
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   hht = 10;//nearp * (float)tan(45.0 / 2.0 / 180.0 * M_PI);
   hwd = hht * (float)state->screen_width / (float)state->screen_height;

//   glFrustumf(-hwd, hwd, -hht, hht, nearp, farp);
   
	 glOrthof(-hwd, hwd, -hht, hht, 0, farp);
   glEnableClientState( GL_VERTEX_ARRAY );
   glVertexPointer( 3, GL_BYTE, 0, quadx );
}

/***********************************************************
 * Name: destroy_array
 *
 * Arguments:
 *       CUBE_STATE_T *state - holds OGLES model info
 *
 * Description: Create array to store uniform variables
 *
 * Returns: void
 *
 ***********************************************************/
void destroy_array(CUBE_STATE_T *state) {
  if (state->param)
    {
      int i;
      for (i = 0; i < state->uniformCount; i++){
        if(state->param[i])free(state->param[i]);
        if(state->name[i]) free(state->name[i]);
        //~state->param[i]=NULL;
      }
    }

  if (state->size)   free(state->size);  // state->size   =NULL;
  if (state->type)   free(state->type);  // state->type   =NULL;
  if (state->flag)   free(state->flag);  // state->flag   =NULL;
  if (state->loc)    free(state->loc);   // state->loc    =NULL;
  if (state->loc)    free(state->name);   // state->loc    =NULL;
  if (state->param)  free(state->param); // state->param  =NULL;
}

/***********************************************************
 * Name: create_array
 *
 * Arguments:
 *       CUBE_STATE_T *state - holds OGLES model info
 *
 * Description: Create array to store uniform variables
 *
 * Returns: void
 *
 ***********************************************************/
static void create_array(CUBE_STATE_T *state){
  int i;

  state->size   = (GLint*) malloc(sizeof( GLint)*state->uniformCount);
  state->type   = (GLenum*) malloc(sizeof( GLenum)*state->uniformCount);
  state->param  = (GLfloat**) malloc(sizeof( GLfloat*)*state->uniformCount);
  state->flag   = (int*) malloc(sizeof( int)*state->uniformCount);
  state->loc    = (GLint*) malloc(sizeof( GLint)*state->uniformCount);
  state->name    = (GLchar**) malloc(sizeof( GLchar*)*state->uniformCount);

  // allocate maximum size for a param, which is a 4x4 matrix of floats
  // in the future, only allocate for specific type
  // also, technically we should handle arrays of matrices, too...sheesh!
  for (i = 0; i < state->uniformCount; i++) {
    int j=0;
    state->size   [i] = 0;
    state->type   [i] = 0;
    state->loc    [i] = 0;
    state->param  [i] = (float*) malloc(sizeof(float)*16);
    state->name   [i] = (GLchar*) malloc(sizeof(GLchar)*state->maxLength);;
    state->flag   [i] = 0;
    for(j=0; j<16; j++)state->param[i][j]=0;
  }
}

/***********************************************************
 * Name: expose_uniform
 *
 * Arguments:
 *       CUBE_STATE_T *state - holds OGLES model info
 *
 * Description: Expose shader uniform to OSC
 *
 * Returns: void
 *
 ***********************************************************/
static void expose_uniform(CUBE_STATE_T *state){
  glGetProgramiv( state->programObject, GL_ACTIVE_UNIFORM_MAX_LENGTH, &state->maxLength);
  glGetProgramiv( state->programObject, GL_ACTIVE_UNIFORMS, &state->uniformCount);
  printf("found %d uniform variable(s) : \n",state->uniformCount);
  
  create_array(state);
  
  int i;
  GLchar *name= (GLchar*) malloc(sizeof(GLchar)*state->maxLength);
  GLsizei    length=0;
  for(i=0;i<state->uniformCount;i++){
    glGetActiveUniform(state->programObject, i, state->maxLength, &length, &state->size[i], &state->type[i], name);
    state->loc[i] = glGetUniformLocation( state->programObject, name );
    strncpy(state->name[i],name,length);
    printf("\t%s\n",name);
  }
  free(name);
}

/***********************************************************
 * Name: init_shaders
 *
 * Arguments:
 *       CUBE_STATE_T *state - holds OGLES model info
 *
 * Description: Initiate shaders
 *
 * Returns: void
 *
 ***********************************************************/
static void init_shaders(CUBE_STATE_T *state)
{
	 const GLchar *vShaderStr = 
"attribute vec4 vPosition;"
"attribute vec2 TexCoordIn;"
"varying vec2 TexCoordOut;"
"void main() \n"
"{ \n"
" gl_Position = vPosition; \n"
"TexCoordOut = TexCoordIn;"
"} \n";

  GLchar *fShaderStr;

  FILE * pFile;
  long lSize;
  int result;
  const char * filename = "frag.glsl";

  pFile = fopen (filename,"rb");
  if (pFile==NULL) {printf ("can't read %s",filename); exit (1);}

  // obtain file size:
  fseek (pFile , 0 , SEEK_END);
  lSize = ftell (pFile);
  rewind (pFile);

  // allocate memory to contain the whole file:
  fShaderStr = (GLchar*) malloc (sizeof(GLchar)*lSize);
  if (fShaderStr == NULL) {fputs ("Memory error",stderr); exit (2);}

  // copy the file into the buffer:
  result = fread (fShaderStr,1,lSize,pFile);
  if (result != lSize) {fputs ("Reading error",stderr); exit (3);}

  /* the whole file is now loaded in the memory buffer. */

  // terminate


	GLuint vertexShader;
	GLuint fragmentShader;
	GLint compiled;
		 
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vShaderStr, NULL);


	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &compiled);

	if (!compiled) printf("It didn't compile\n");

	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(fragmentShader, 1, (const GLchar **) &fShaderStr, NULL);

	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &compiled);

	if (!compiled) printf("It didn't compile\n");

	state->programObject = glCreateProgram();

	glAttachShader(state->programObject, vertexShader);
	glAttachShader(state->programObject, fragmentShader);

	glBindAttribLocation(state->programObject, 0, "vPosition");
	glBindAttribLocation(state->programObject, 1, "TexCoordIn");

	glLinkProgram(state->programObject);

	state->unif_tex = glGetUniformLocation(state->programObject, "Texture");

	GLint linked;

	glGetProgramiv(state->programObject, GL_LINK_STATUS, &linked);

  if(!linked) 
  {
    printf("can't link shader !!\n");
    GLint infoLen = 0;
    glGetProgramiv(state->programObject, GL_INFO_LOG_LENGTH, &infoLen);

    if(infoLen > 1)
    {
      char* infoLog = (char *) malloc(sizeof(char) * infoLen);
      glGetProgramInfoLog(state->programObject, infoLen, NULL, infoLog);
      if ( infoLen ){
        printf("error log :\n");
        printf("%s\n",infoLog);
      }

      free(infoLog);
    }
    glDeleteProgram(state->programObject);
    free (fShaderStr);
    fclose (pFile);
    exit(1);
  } else {
    expose_uniform(state);
  }
  free (fShaderStr);
  fclose (pFile);
}

/***********************************************************
 * Name: osc_error
 *
 * Arguments:
 *       int - error #
 *       const char * - error message
 *       const char * - erronous path
 *
 * Description: Report OSC error to stdout
 *
 * Returns: void
 *
 ***********************************************************/
void osc_error(int num, const char *msg, const char *path)
{
    printf("liblo server error %d in path %s: %s\n", num, path, msg);
    fflush(stdout);
}

/***********************************************************
 * Name: osc_generic_handler
 *
 * Arguments:
 *       int - error #
 *       const char * - error message
 *       const char * - erronous path
 *
 * Description: catch any incoming messages and process them.
 * 
 * Returns: void
 *
 ***********************************************************/
void osc_generic_handler(const char *path, const char *types, lo_arg ** argv,
                    int argc, void *data, void *user_data)
{
    int i;
    CUBE_STATE_T *state = user_data;
    
    for (i=0;i<state->uniformCount;i++){
      if ( strcmp(state->name[i], path+1) == 0 ){
        int j;
        for (j=0;j<argc;j++){
          if ( types[j] == 'f') {
            state->param[i][j]= (GLfloat) argv[j]->f;
          } else if ( types[j] == 'i' ){
            state->param[i][j]= (GLfloat) argv[j]->i;
          } else {
            printf("%s parameter #%d wrong type (%c)! only float or int are allowed !\n",path,j,types[j]);
          }
        }
        state->flag[i] = 1;
        break;
      }
    }
}

/***********************************************************
 * Name: init_osc
 *
 * Arguments:
 *       CUBE_STATE_T *state - holds OGLES model info
 *
 * Description: Initiate OSC communication
 *
 * Returns: void
 *
 ***********************************************************/
static void init_osc(CUBE_STATE_T *state)
{
  state->st = lo_server_thread_new(state->osc_inport, osc_error);
  /* add method that will match any path and args */
  lo_server_thread_add_method(state->st , NULL, NULL, (lo_method_handler) osc_generic_handler, state);
  lo_server_thread_start(state->st);
}

/***********************************************************
 * Name: redraw_scene
 *
 * Arguments:
 *       CUBE_STATE_T *state - holds OGLES model info
 *
 * Description:   Draws the model and calls eglSwapBuffers
 *                to render to screen
 *
 * Returns: void
 *
 ***********************************************************/
 int frame_id=0;
static void redraw_scene(CUBE_STATE_T *state)
{
  // Start with a clear screen
  glClear( GL_COLOR_BUFFER_BIT );

  glUseProgram(state->programObject);

  //~float invert = frame_id++/100.;
  //~frame_id%=100;
  //~glUniform1f(glGetUniformLocation(state->programObject,"invert"),invert);
  int i;
  for(i=0; i<state->uniformCount; i++)
  {
    if(state->flag[i])
    {
      switch (state->type[i])
      {
        /* float vectors */
        case GL_FLOAT:
          glUniform1f( state->loc[i], state->param[i][0] );
          break;
        case GL_FLOAT_VEC2:
          glUniform2f( state->loc[i], (state->param[i][0]), (state->param[i][1]) );
          break;
        case GL_FLOAT_VEC3:
          glUniform3f( state->loc[i], (state->param[i][0]), (state->param[i][1]),
          (state->param[i][2]) );
        break;
        case GL_FLOAT_VEC4:
          glUniform4f( state->loc[i], (state->param[i][0]), (state->param[i][1]),
          (state->param[i][2]), (state->param[i][3]) );
        break;
        /* int vectors */
        case GL_INT:
          glUniform1i( state->loc[i], (state->param[i][0]) );
          break;
        case GL_INT_VEC2:
          glUniform2i( state->loc[i], (state->param[i][0]), (state->param[i][1]) );
          break;
        case GL_INT_VEC3:
          glUniform3i(state->loc[i],
          (state->param[i][0]), (state->param[i][1]), (state->param[i][2]) );
          break;
        case GL_INT_VEC4:
          glUniform4i(state->loc[i],
          (state->param[i][0]), (state->param[i][1]),
          (state->param[i][2]), (state->param[i][3]) );
          break;
        /* bool vectors */
        case GL_BOOL:
          glUniform1f( state->loc[i], (state->param[i][0]) );
          break;
        case GL_BOOL_VEC2:
          glUniform2f( state->loc[i], (state->param[i][0]), (state->param[i][1]) );
          break;
        case GL_BOOL_VEC3:
          glUniform3f( state->loc[i],
          (state->param[i][0]), (state->param[i][1]),
          (state->param[i][2]) );
          break;
        case GL_BOOL_VEC4:
          glUniform4f( state->loc[i],
          (state->param[i][0]), (state->param[i][1]),
          (state->param[i][2]), (state->param[i][3]) );
          break;

        /* float matrices */
        case GL_FLOAT_MAT2:
          // GL_TRUE = row major order, GL_FALSE = column major
          glUniformMatrix2fv( state->loc[i], 1, GL_FALSE, state->param[i] );
          break;
        case GL_FLOAT_MAT3:
          glUniformMatrix3fv( state->loc[i], 1, GL_FALSE, state->param[i] );
          break;
        case GL_FLOAT_MAT4:
          glUniformMatrix4fv( state->loc[i], 1, GL_FALSE, state->param[i] );
          break;

        /* textures */
        case GL_SAMPLER_2D:
          glUniform1i(state->loc[i], state->param[i][0]);
          break;
        case GL_SAMPLER_CUBE: break;
          glUniform1i(state->loc[i], (state->param[i][0]));
          break;
        default:
        ;
      }
      // remove flag because the value is in GL's state now...
      state->flag[i]=0;  

    }
  }
  // Draw first (front) face:
  // Bind texture surface to current vertices
  glBindTexture(GL_TEXTURE_2D, state->tex);

  glVertexAttribPointer( 0, 3, GL_BYTE, GL_FALSE, 0, quadx );
  glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, texCoords );
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  // draw first 4 vertices

  glUniform1i(state->unif_tex,0);

  glDrawArrays( GL_TRIANGLE_STRIP, 0, 4);

  eglSwapBuffers(state->display, state->surface);

}

/***********************************************************
 * Name: init_textures
 *
 * Arguments:
 *       CUBE_STATE_T *state - holds OGLES model info
 *
 * Description:   Initialise OGL|ES texture surfaces to use image
 *                buffers
 *
 * Returns: void
 *
 ***********************************************************/
static void init_textures(CUBE_STATE_T *state)
{
   // the texture containing the video
   glGenTextures(1, &state->tex);

   glBindTexture(GL_TEXTURE_2D, state->tex);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, state->screen_width,
			 state->screen_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   /* Create EGL Image */
   state->eglImage = eglCreateImageKHR(
                state->display,
                state->context,
                EGL_GL_TEXTURE_2D_KHR,
                (EGLClientBuffer)state->tex,
                0);

   if (state->eglImage == EGL_NO_IMAGE_KHR)
   {
      printf("eglCreateImageKHR failed.\n");
      exit(1);
   }

   // Start rendering
   pthread_create(&thread1, NULL, video_decode_test, state);

   // setup overall texture environment
   glTexCoordPointer(4, GL_FLOAT, 0, texCoords);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);

   glEnable(GL_TEXTURE_2D);

   // Bind texture surface to current vertices
   glBindTexture(GL_TEXTURE_2D, state->tex);
}


//------------------------------------------------------------------------------

static void exit_func(void)
// Function to be passed to atexit().
{
   // clear screen
   glClear( GL_COLOR_BUFFER_BIT );
   eglSwapBuffers(state->display, state->surface);

   // Release OpenGL resources
   eglMakeCurrent( state->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
   eglDestroySurface( state->display, state->surface );
   eglDestroyContext( state->display, state->context );
   eglTerminate( state->display );
   
   destroy_array(state);
   
   // release texture buffers
   free(state->tex_buf1);
   free(state->tex_buf2);
   free(state->tex_buf3);

   printf("\nLongueVue closed\n");
} // exit_func()

//==============================================================================
int debug = 0;

int main (int argc, char **argv)
{
   
  extern char *optarg;
	extern int optind;
	int c, err = 0; 
  //~int outport=9001;
	char *fname=NULL;
  char *inport="9000";
	static char usage[] = "usage: %s [-i inport] -f fragment_shader \n";

	while ((c = getopt(argc, argv, "i:f:")) != -1)
		switch (c) {
    //~case 'a':
      //~printf("address :%s\n",optarg);
		case 'i':
      inport=optarg;
			break;
		//~case 'o':
      //~outport=atoi(optarg);
			//~break;
		case 'f':
			fname = optarg;
			break;
		case '?':
			err = 1;
			break;
		}
	if (fname == NULL) {	/* -f was mandatory */
		fprintf(stderr, "%s: missing -f option\n", argv[0]);
		fprintf(stderr, usage, argv[0]);
		exit(1);
	} else if (err) {
		fprintf(stderr, usage, argv[0]);
		exit(1);
	}
  
  state->osc_inport = inport;
  
	/* see what we have */
	//~printf("OSC destination = %s\n", address);
	printf("OSC input listening port :  = %d\n", inport);
	//~printf("OSC output port :  = %d\n", outport);
	printf("fragment shader file = \"%s\"\n", fname);
  
   atexit(exit_func);
   bcm_host_init();

   // Clear application state
   memset( state, 0, sizeof( *state ) );
      
   // Start OGLES
   init_ogl(state);

	 init_shaders(state);

   // Setup the model world
   init_model_proj(state);

   // initialise the OGLES texture(s)
   init_textures(state);
   
   // TODO : parse command line argument to set input/output port and IP
   state->osc_inport = inport;
   //~state->osc_outport = outport;
   //~state->osc_destination = address;
   // initialise OSC
   init_osc(state);

   while (1)
   {
      redraw_scene(state);
   }
   return 0;
}

