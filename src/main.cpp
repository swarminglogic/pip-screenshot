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
      writeConfigFile(config);
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
