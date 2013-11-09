#ifndef __PPLINUXKEY_H__
#define __PPLINUXKEY_H__

#define PPLINUXKEY_SPACE 0x0001
#define PPLINUXKEY_UP    0x0002
#define PPLINUXKEY_DOWN  0x0004
#define PPLINUXKEY_LEFT  0x0008
#define PPLINUXKEY_RIGHT 0x0010
#define PPLINUXKEY_X     0x0020
#define PPLINUXKEY_Z     0x0040

class PPLinuxKey {
public:
  PPLinuxKey();
  virtual ~PPLinuxKey();
  
  char       *touch_filename;
  int         touch_fd;

  int         have_multitouch;
  int         have_track_id;
  int         next_numtouch;

  int get();
  
  unsigned long key;
  
  float posx;
  float posy;
  
  bool left_key;
  bool right_key;
  
  void key_idle();
  void mouse_idle();
  int open_device(char *touch_filename);
};

#endif
