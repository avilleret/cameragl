#ifndef __STATE_H
#define __STATE_H

#include "GLES/gl.h"
#include "GLES2/gl2.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"

#include "lo/lo.h"

typedef struct
{
  uint32_t screen_width;
  uint32_t screen_height;
  uint32_t camera_width;
  uint32_t camera_height;
  uint32_t camera_fps;
  // OpenGL|ES objects
  EGLDisplay display;
  EGLSurface surface;
  EGLContext context;
  GLuint tex;
  GLuint programObject;
  // model rotation vector and direction
  GLfloat rot_angle_x_inc;
  GLfloat rot_angle_y_inc;
  GLfloat rot_angle_z_inc;
  // current model rotation angles
  GLfloat rot_angle_x;
  GLfloat rot_angle_y;
  GLfloat rot_angle_z;
  // current distance from camera
  GLfloat distance;
  GLfloat distance_inc;
  // pointers to texture buffers
  char *tex_buf1;
  char *tex_buf2;
  char *tex_buf3;

  GLuint attr_vertex;
  GLuint attr_tex;
  GLuint unif_tex;
  GLuint unif_thresh;
  GLuint unif_color;
  void *eglImage;
   
  // Variables for the, uh, variables
  // stolen from puredata/gem glsl_program.h, thanks guys ;-)
  GLint	maxLength;
  GLint uniformCount;
  GLint *size;
  GLenum *type;
  GLint *loc;
  GLchar **name;
  GLfloat **param;
  int *flag;
  
  char* fragmentShaderFilename;
  char* vertexShaderFilename;
  
  int verbose;
  
  // OSC parameters
  lo_server_thread st;
  char* osc_inport;
  //~unsigned int osc_outport;
  //~char *destination;
} CUBE_STATE_T;

#endif
