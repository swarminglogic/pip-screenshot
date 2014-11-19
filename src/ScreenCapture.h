#ifndef SCREENCAPTURE_H
#define SCREENCAPTURE_H

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>


#include <Magick++.h>


/**
 * ScreenCapture class.
 *
 * @author SwarmingLogic
 */
class ScreenCapture
{
 public:
  ScreenCapture();
  virtual ~ScreenCapture();
  Magick::Image getScreenShot(Magick::Geometry geometry) const;

 private:
  Display* display_;
  Window root_;
};


#endif  // SCREENCAPTURE_H
