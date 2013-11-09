#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <error.h>
#include <poll.h>

#include <linux/input.h>

#include "PPLinuxKey.h"

#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x)-1)/BITS_PER_LONG)+1)
#define OFF(x)  ((x)%BITS_PER_LONG)
#define BIT(x)  (1UL<<OFF(x))
#define LONG(x) ((x)/BITS_PER_LONG)
#define test_bit(bit, array)    ((array[LONG(bit)] >> OFF(bit)) & 1)

#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define ABS(a)     (((a) < 0) ? -(a) : (a))

PPLinuxKey::PPLinuxKey() : key(0),posx(0),posy(0)
{
}

PPLinuxKey::~PPLinuxKey()
{
}

int PPLinuxKey::get()
{
  return 99;
}

void PPLinuxKey::mouse_idle()
{
  struct input_event evts[64];
  int    i, j, ret;

  while (1) {
      ret = read (touch_fd, evts, sizeof (evts));
      if (ret < 0 && errno != EAGAIN)
        error (1, errno, "failed to read multi touch events");

      if (ret == 0 || (ret < 0 && errno == EAGAIN))
        break;

      unsigned char* t=(unsigned char*)evts;
      
      //for (int i=0;i<ret;i++) {
      //  printf("%02X,",(int)t[i]);
      //}
      //printf("\n");
      
      left_key = false;
      if (t[0] & 0x01) {
        left_key = true;
      }

      right_key = false;
      if (t[0] & 0x02) {
        right_key = true;
      }
      
      signed char* n=(signed char*)evts;

      posx+=(int)n[1];
      posy+=(int)n[2];
      
      //printf("posx %f,posy %f",posx,posy);

  }
}

void PPLinuxKey::key_idle()
{
  struct input_event evts[64];
  int    i, j, ret;
  struct input_event *ev;

  while (1) {
      ret = read (touch_fd, evts, sizeof (evts));
      if (ret < 0 && errno != EAGAIN)
        error (1, errno, "failed to read multi touch events");

      if (ret == 0 || (ret < 0 && errno == EAGAIN))
        break;

      char* t=(char*)evts;
      
      //for (int i=0;i<ret;i++) {
      //  printf("%02X,",(int)t[i]);
      //}
      //printf("\n");

      ret /= sizeof (struct input_event);

      for (i = 0; i < ret; i++) {
        ev = &evts[i];
        //fprintf (stderr, "evt: %d %d (%d)\n",
        //         ev->type, ev->code, ev->value);
        if (ev->type==1) {
          if (ev->value) {
            //key on
            if (ev->code == KEY_SPACE) key |= PPLINUXKEY_SPACE;
            if (ev->code == KEY_Z) key |= PPLINUXKEY_Z;
            if (ev->code == KEY_UP) key |= PPLINUXKEY_UP;
            if (ev->code == KEY_DOWN) key |= PPLINUXKEY_DOWN;
            if (ev->code == KEY_LEFT) key |= PPLINUXKEY_LEFT;
            if (ev->code == KEY_RIGHT) key |= PPLINUXKEY_RIGHT;
            if (ev->code == KEY_X) key |= PPLINUXKEY_X;
          } else {
            //key off
            if (ev->code == KEY_SPACE) key &= (~PPLINUXKEY_SPACE);
            if (ev->code == KEY_Z) key &= (~PPLINUXKEY_Z);
            if (ev->code == KEY_UP) key &= (~PPLINUXKEY_UP);
            if (ev->code == KEY_DOWN) key &= (~PPLINUXKEY_DOWN);
            if (ev->code == KEY_LEFT) key &= (~PPLINUXKEY_LEFT);
            if (ev->code == KEY_RIGHT) key &= (~PPLINUXKEY_RIGHT);
            if (ev->code == KEY_X) key &= (~PPLINUXKEY_X);
          }
        }
      }

  }
}

int PPLinuxKey::open_device(char *touch_filename)
{
  unsigned long bits[NBITS(KEY_MAX)];
  char name[256] = "Unknown";
  struct input_absinfo abs;
  int fd;

  touch_fd = fd = open (touch_filename, O_RDONLY | O_NONBLOCK);
  if (touch_fd < 0) {
    perror ("could not open touchscreen device");
  }

  ioctl (fd, EVIOCGNAME(sizeof(name)), name);
  fprintf (stderr, "Input device name: \"%s\"\n", name);

  ioctl (fd, EVIOCGBIT(0, EV_MAX), bits);
  if (!test_bit (EV_ABS, bits)) {
    fprintf (stderr, "   does not provide ABS events\n");
    return 1;
  }

  have_track_id = test_bit (ABS_MT_TRACKING_ID, bits) ? 1 : 0;

  return 0;
}
