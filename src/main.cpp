#include <cstdio>
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/property_tree/exceptions.hpp>
#include <boost/property_tree/info_parser.hpp>

#include <Magick++.h>

#include <DefaultConfig.h>
#include <ScreenCapture.h>



namespace pt = boost::property_tree;

void makePip(Magick::Image& screen,
             const pt::ptree& config,
             const Magick::Geometry& pipGeom,
             const float sigma)
{
  Magick::Geometry rPip(pipGeom.width(),
                        pipGeom.height(),
                        0, 0);
  const int w = config.get<int>("pip.border.width");

  // Resize image and add border
  const Magick::Geometry border(w, w, 0, 0);
  screen.backgroundColor("none");
  screen.filterType(Magick::LanczosFilter);
  screen.resize(rPip);
  screen.borderColor(config.get<std::string>("pip.border.color"));
  screen.border(border);

  // Add shadow
  Magick::Image shadow = screen;
  shadow.backgroundColor(config.get<std::string>("pip.shadow.color"));
  shadow.shadow(config.get<int>("pip.shadow.opacity"),
                sigma, 0, 0);
  shadow.composite(screen,
                   pipGeom.xOff(), pipGeom.yOff(),
                   Magick::OverCompositeOp);
  screen = std::move(shadow);
}

bool writeConfigFile(const std::string& outfile)
{
  std::ofstream conffile(outfile, std::ofstream::out);
  if (conffile.good()) {
    std::cout << "Config file '" << outfile << "' created." << std::endl;
    conffile << pipfiles::defaultConfig;
    conffile.close();
  } else {
    std::cerr << "Failed to create config file: "
              << outfile << std::endl;
    return false;
  }
  return true;
}

void displayUsage(std::ostream& out, const std::string& argv0) {
  out << " Usage: \n"
      << " " << argv0 << " --help\n"
      << " " << argv0 << " --createconfig [CONFIG.info]\n"
      << " " << argv0 << " OUTFILE.FMT [CONFIG.info]\n"
      << pipfiles::helpOutupt << std::endl;
}

void parseParameters(int argc, char* argv[],
                     std::string& output,
                     std::string& config) {
  (void) argc;
  if (argc < 2) {
    std::cerr << "Invalid parameters." << std::endl;
    displayUsage(std::cerr, argv[0]);
    exit(EXIT_FAILURE);
  }

  // Check if --createconfig flag set.
  if (std::string(argv[1]) == std::string("--createconfig")) {
    if (argc > 2)
      config = argv[2];
    else
      config = "pip-screenshot-config.info";

    if (boost::filesystem::exists(config)) {
      std::cerr << "Specified config file already exists! Aborting."
                << std::endl;
      exit(EXIT_FAILURE);
    } else {
      if (writeConfigFile(config))
        exit(EXIT_SUCCESS);
      else
        exit(EXIT_FAILURE);
    }
  } else if (std::string(argv[1]) == std::string("--help")) {
    displayUsage(std::cout, argv[0]);
    exit(EXIT_SUCCESS);
  } else {
    if (argc > 1)
      output = argv[1];
    if (argc > 2)
      config = argv[2];
    else
      config = "pip-screenshot-config.info";
  }
}

int main(int argc, char* argv[])
{
  // Parameter parsing
  std::string configFile;
  std::string outputFile;
  parseParameters(argc, argv, outputFile, configFile);

  // Check if config file or create default
  if (!boost::filesystem::exists(configFile)) {
    std::cerr << "Couldn't find config file" << std::endl;
    return EXIT_FAILURE;
  }

  // Load configuration from config file
  pt::ptree config;
  boost::property_tree::read_info(configFile, config);

  // Read config values
  const int prinW = config.get<int>("monitor.principal.w");
  const int prinH = config.get<int>("monitor.principal.h");
  const int prinX = config.get<int>("monitor.principal.x");
  const int prinY = config.get<int>("monitor.principal.y");

  const int secW = config.get<int>("monitor.secondary.w");
  const int secH = config.get<int>("monitor.secondary.h");
  const int secX = config.get<int>("monitor.secondary.x");
  const int secY = config.get<int>("monitor.secondary.y");

  // How many pixels to offset the PiP, from bottom right
  const float sigma = config.get<float>("pip.shadow.sigma");
  const float pipShadowX = config.get<float>("pip.shadow.pos.x");
  const float pipShadowY = config.get<float>("pip.shadow.pos.y");
  const int pipPosX = config.get<int>("pip.pos.x");
  const int pipPosY = config.get<int>("pip.pos.y");
  const int pipW = config.get<int>("pip.size.w");
  const int pipH = config.get<int>("pip.size.h");

  // Where to overlay the PiP relative to shadow
  const int pipPosXShadow = static_cast<int>((-pipShadowX + 1.0f) *
                                             2.0f * sigma);
  const int pipPosYShadow = static_cast<int>((-pipShadowY + 1.0f) *
                                             2.0f * sigma);

  // Adjustment offset for change in PiP position within the pip image,
  // caused by the drop-shadow
  const int pipPosXShadowOff = static_cast<int>(2.0f * sigma *
                                                (1.0f + pipShadowX));
  const int pipPosYShadowOff = static_cast<int>(2.0f * sigma *
                                                (1.0f + pipShadowY));

  // Initalize image magick
  Magick::InitializeMagick(*argv);
  Magick::Geometry prinGeom(prinW, prinH, prinX, prinY);
  Magick::Geometry secGeom(secW, secH, secX, secY);
  Magick::Geometry pipGeom(pipW, pipH, pipPosXShadow, pipPosYShadow);

  // Get screen capture from X11
  ScreenCapture sc;
  Magick::Image principal = sc.getScreenShot(prinGeom);
  Magick::Image secondary = sc.getScreenShot(secGeom);

  // Convert secondary screen to PiP thumbnail
  makePip(secondary, config, pipGeom, sigma);
  principal.composite(secondary,
                      (prinGeom.width() - secondary.size().width() -
                       pipPosX + pipPosXShadowOff),
                      (prinGeom.height() - secondary.size().height() -
                       pipPosY + pipPosYShadowOff),
                      Magick::OverCompositeOp);

  // Write frame to file
  // Removes alpha channe
  principal.matte(false);
  principal.depth(8);
  principal.compressType(Magick::RunlengthEncodedCompression);
  principal.write(outputFile);

  return EXIT_SUCCESS;
}
