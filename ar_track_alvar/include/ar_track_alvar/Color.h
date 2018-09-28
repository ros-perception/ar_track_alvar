#include "ar_track_alvar/MultiMarker.h"
#include "highgui.h"
using namespace std;
using namespace alvar;

namespace col
{
  enum color_t
  {
    COLOR_BLACK,
    COLOR_RED,
    COLOR_BLUE,
    COLOR_GREEN,
    COLOR_YELLOW,
    COLOR_PINK,
    COLOR_SKY
  };

  color_t get_color(char color_name);

  unsigned char get_color(color_t p_color);

  void change_color(IplImage *p_img, unsigned char p_color = 0);
}
