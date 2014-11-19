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
             const pt::ptree& config)
{
  Magick::Geometry rPip(config.get<int>("pip.size.w"),
                        config.get<int>("pip.size.h"),
                        0, 0);
  const int w = config.get<int>("pip.border.width");

  // Resize image and add border
  const Magick::Geometry border(w, w, 0, 0);
  screen.backgroundColor("none");
  screen.resize(rPip);
  screen.borderColor(config.get<std::string>("pip.border.color"));
  screen.border(border);

  // Add shadow
  Magick::Image shadow = screen;
  shadow.backgroundColor(config.get<std::string>("pip.shadow.color"));
  shadow.shadow(config.get<int>("pip.shadow.opacity"),
                config.get<float>("pip.shadow.sigma"),
                config.get<int>("pip.shadow.size.w"),
                config.get<int>("pip.shadow.size.h"));
  shadow.composite(screen,
                   config.get<int>("pip.shadow.offset.x"),
                   config.get<int>("pip.shadow.offset.y"),
                   Magick::OverCompositeOp);
  screen = shadow;
}


void parseParameters(int argc, char* argv[],
                     std::string& output,
                     std::string& config) {
  (void) argc;
  if (argc < 2) {
    std::cerr << "Invalid parameters.\n"
              << "Usage: \n"
              << argv[0] << " OUTFILE.svgz [CONFIG.info]\n"
              << "\n"
              << "SVGZ format is highly recommended, but optional.\n"
              << "It was found to use half of CPU time as PNG encoding \n"
              << "and roughly the same disk space. \n"
              << "\n"
              << "If CONFIG.info file is not provided, file 'pip-timelapse-\n"
              << "config.info' with default configuration is either (used if \n"
              << "found), or created (if not found).\n"
              << "\n"
              << "If CONFIG.info file is provided and doesn't exist, it \n"
              << "creates this file with default configuration"
              <<  std::endl;
    exit(EXIT_FAILURE);
  }

  if (argc > 1)
    output = argv[1];
  if (argc > 2)
    config = argv[2];
  else
    config = "pip-timelapse-config.info";
}

int main(int argc, char* argv[])
{
  // Parameter parsing
  std::string configFile;
  std::string outputFile;
  parseParameters(argc, argv, outputFile, configFile);

  // Check if config file or create default
  if (!boost::filesystem::exists(configFile)) {
    std::ofstream conffile(configFile, std::ofstream::out);
    if (conffile.good()) {
      conffile << piptimelapse::defaultConfig;
      conffile.close();
    } else {
      std::cerr << "Failed to create config file: "
                << configFile << std::endl;
      return EXIT_FAILURE;
    }
  }
  // Load configuration from config file
  pt::ptree config;
  boost::property_tree::read_info(configFile, config);

  // Read config values
  const int pgW = config.get<int>("monitor.principal.w");
  const int pgH = config.get<int>("monitor.principal.h");
  const int pgX = config.get<int>("monitor.principal.x");
  const int pgY = config.get<int>("monitor.principal.y");

  const int sgW = config.get<int>("monitor.secondary.w");
  const int sgH = config.get<int>("monitor.secondary.h");
  const int sgX = config.get<int>("monitor.secondary.x");
  const int sgY = config.get<int>("monitor.secondary.y");

  const int pipOffX = config.get<int>("pip.pos.x");
  const int pipOffY = config.get<int>("pip.pos.y");

  // Initalize image magick
  Magick::InitializeMagick(*argv);
  Magick::Geometry principalGeom(pgW, pgH, pgX, pgY);
  Magick::Geometry secondaryGeom(sgW, sgH, sgX, sgY);

  // Get screen capture from X11
  ScreenCapture sc;
  Magick::Image principal = sc.getScreenShot(principalGeom);
  Magick::Image secondary = sc.getScreenShot(secondaryGeom);

  // Convert secondary screen to PiP thumbnail
  makePip(secondary, config);
  // Overlay PiP on principal screen
  principal.composite(secondary,
                      pgW - secondary.size().width() - pipOffX,
                      pgH - secondary.size().height() - pipOffY,
                      Magick::OverCompositeOp);
  // Write frame to file
  principal.write(outputFile);

  return EXIT_SUCCESS;
}