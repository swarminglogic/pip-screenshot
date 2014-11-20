#include <ScreenCapture.h>


ScreenCapture::ScreenCapture()
    : display_(XOpenDisplay(nullptr)),
      root_(ScreenOfDisplay(display_, DefaultScreen(display_))->root)
{
}

ScreenCapture::~ScreenCapture()
{
  XCloseDisplay(display_);
}


Magick::Image ScreenCapture::getScreenShot(Magick::Geometry g) const
{
  XImage* screencap = XGetImage(display_,
                                root_,
                                static_cast<int>(g.xOff()),
                                static_cast<int>(g.yOff()),
                                static_cast<uint32_t>(g.width()),
                                static_cast<uint32_t>(g.height()),
                                AllPlanes,
                                ZPixmap);
  Magick::Image image(g.width(), g.height(),
                      "BGRA",
                      Magick::CharPixel,
                      screencap->data);
  image.matte(false);
  image.depth(8);
  XDestroyImage(screencap);
  return std::move(image);
}
