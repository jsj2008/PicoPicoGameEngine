#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include <termios.h>

#include "bcm_host.h"

#include "GLES/gl.h"
#include "EGL/egl.h"
#include "EGL/eglext.h"

#include "QBGame.h"
//#include "QBSoundWin.h"
#include "PPGameLinuxScene.h"
#include "PPGameUtil.h"
#include "PPLinuxKey.h"
#include "pcm.h"

PPLinuxKey hdkey;
PPLinuxKey mouse;

static PPGameLinuxScene* scene=NULL;

typedef struct
{
   uint32_t screen_width;
   uint32_t screen_height;

  // OpenGL|ES objects
   EGLDisplay display;
   EGLSurface surface;
   EGLContext context;
   GLuint tex[6];

} GAME_STATE_T;

static void init_opengl(GAME_STATE_T *state);
static void redraw_scene(GAME_STATE_T *state);
static void exit_func(void);
static volatile int _terminate;
static GAME_STATE_T _state, *state=&_state;

static PPGameSprite* g=NULL;

static void init_opengl(GAME_STATE_T *state)
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
   
   EGLConfig config;

   state->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
   assert(state->display!=EGL_NO_DISPLAY);

   result = eglInitialize(state->display, NULL, NULL);
   assert(EGL_FALSE != result);

   result = eglChooseConfig(state->display, attribute_list, &config, 1, &num_config);
   assert(EGL_FALSE != result);

   state->context = eglCreateContext(state->display, config, EGL_NO_CONTEXT, NULL);
   assert(state->context!=EGL_NO_CONTEXT);

   success = graphics_get_display_size(0 /* LCD */, &state->screen_width, &state->screen_height);
   assert( success >= 0 );
   
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
      &src_rect, DISPMANX_PROTECTION_NONE, 0 /*alpha*/, 0/*clamp*/, 0/*transform*/);
      
   nativewindow.element = dispman_element;
   nativewindow.width = state->screen_width;
   nativewindow.height = state->screen_height;
   vc_dispmanx_update_submit_sync( dispman_update );
      
   state->surface = eglCreateWindowSurface( state->display, config, &nativewindow, NULL );
   assert(state->surface != EGL_NO_SURFACE);

   result = eglMakeCurrent(state->display, state->surface, state->surface, state->context);
   assert(EGL_FALSE != result);

   glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

static void redraw_scene(GAME_STATE_T *state)
{
  unsigned long key=0;

  if (scene) {

  	scene->windowSize.width = state->screen_width;
  	scene->windowSize.height = state->screen_height;
  
    hdkey.key_idle();
    mouse.mouse_idle();
    
    scene->touchScreen=mouse.left_key;
    
    if (hdkey.key & PPLINUXKEY_SPACE) key |= PAD_A;
    if (hdkey.key & PPLINUXKEY_X) key |= PAD_B;
    if (hdkey.key & PPLINUXKEY_Z) key |= PAD_A;
    if (hdkey.key & PPLINUXKEY_UP) key |= PAD_UP;
    if (hdkey.key & PPLINUXKEY_DOWN) key |= PAD_DOWN;
    if (hdkey.key & PPLINUXKEY_LEFT) key |= PAD_LEFT;
    if (hdkey.key & PPLINUXKEY_RIGHT) key |= PAD_RIGHT;
  
  	if (scene->game) {
      scene->touchLocation_x = mouse.posx;
      scene->touchLocation_y = -mouse.posy;
  	}
  	scene->hardkey = key;
    scene->draw();
  }

  eglSwapBuffers(state->display, state->surface);
}

static void exit_func(void)
{
   glClear( GL_COLOR_BUFFER_BIT );
   eglSwapBuffers(state->display, state->surface);

   eglMakeCurrent( state->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT );
   eglDestroySurface( state->display, state->surface );
   eglDestroyContext( state->display, state->context );
   eglTerminate( state->display );

   free(state->tex_buf1);
   free(state->tex_buf2);
   free(state->tex_buf3);
}

//==============================================================================

int main (int argc, char** argv)
{
  playsound();

  mouse.open_device("/dev/input/mice");
  hdkey.open_device("/dev/input/event1");

  bcm_host_init();

  // Clear application state
  memset( state, 0, sizeof( *state ) );
      
  // Start OGLES
  init_opengl(state);

  // Setup the model world
  glViewport(0, 0, (GLsizei)state->screen_width, (GLsizei)state->screen_height);

  PPGameSetDataPath("Resources/");
  
  scene = new PPGameLinuxScene();
  scene->init();

  _terminate=0;

  while (!_terminate)
  {
    redraw_scene(state);
  }
  exit_func();
  return 0;
}
