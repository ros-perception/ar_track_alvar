#include "ar_track_alvar/Color.h"

namespace col
{
  color_t get_color(char color_name)
  {
    color_t color = COLOR_BLACK;
    switch(color_name)
    {
      case 'R': case 'r': color = COLOR_RED; break;
      case 'B': case 'b': color = COLOR_BLUE; break;
      case 'G': case 'g': color = COLOR_GREEN; break;
      case 'Y': case 'y': color = COLOR_YELLOW; break;
      case 'P': case 'p': color = COLOR_PINK; break;
      case 'S': case 's': color = COLOR_SKY; break;
      default: color = COLOR_BLACK; break;
    }
    return color;
  }

  unsigned char get_color(color_t p_color)
  {
    unsigned char result = 0;
    if((p_color == COLOR_RED) || (p_color == COLOR_YELLOW) || (p_color == COLOR_PINK))
      result |= 0x04;
    if((p_color == COLOR_GREEN) || (p_color == COLOR_YELLOW) || (p_color == COLOR_SKY))
      result |= 0x02;
    if((p_color == COLOR_BLUE) || (p_color == COLOR_PINK) || (p_color == COLOR_SKY))
      result |= 0x01;
    return result;
  }

  void change_color(IplImage *p_img, unsigned char p_color)
  {
    for( int y=0; y<p_img->height; y++ )
    {
      uchar* ptr = (uchar*) ( p_img->imageData + y * p_img->widthStep );
      for( int x=0; x<p_img->width; x++ )
      {
        if(ptr[3*x+2] == 0)
        {
          ptr[3*x+2] = ((p_color >> 2)&0x01)*150; //r
          ptr[3*x+1] = ((p_color >> 1)&0x01)*150; //g
          ptr[3*x+0] = ((p_color >> 0)&0x01)*200; //b
        }
      }
    }
  }
}
