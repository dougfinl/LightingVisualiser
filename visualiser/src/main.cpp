//
// Created by dgf6 on 28/10/15.
//

#include "libvisualiser/VisualiserCore.h"
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/assign.hpp>


struct VisualiserConfig {
    std::string showFilePath;
    unsigned int universe;
    unsigned int resX;
    unsigned int resY;
    bool fullscreen;
    bool debugMessages;
    bool realLighting;
};

namespace po = boost::program_options;

static void checkResolutionOption(const std::vector<unsigned int> &res) {
    if (res.size() != 2) {
        throw new po::validation_error(
            po::validation_error::invalid_option_value,
            "resolution",
            std::to_string(res.size())
        );
    }
}

VisualiserConfig parseConfigurationOptions(const int argc, const char **argv) {
    // Setup the option parser
    po::options_description desc("Allowed options:");
    desc.add_options()
        ("help,h", "display this message")
        ("debug,d", po::value<bool>()->zero_tokens(), "show debug messages")
        ("showfile", po::value<std::string>()->required(), "the show file to be visualised")
        ("universe,u", po::value < unsigned
    int > ()->default_value(0), "the universe ID upon which data will be received")
    ("resolution,r", po::value<std::vector<unsigned int>>()->
        multitoken()->notifier(&checkResolutionOption), "the resolution for the render window")
        ("fullscreen,f", po::value<bool>()->zero_tokens(), "enables fullscreen rendering")
        ("realLighting,l", po::value<bool>()->zero_tokens(), "enables realistic lighting");

    po::positional_options_description pdesc;
    pdesc.add("showfile", 1);

    po::variables_map vm;

    std::string showfile = "";
    unsigned int universe = 0;
    bool debug = false;
    unsigned int resX = 1024;
    unsigned int resY = 768;
    bool fullscreen = false;
    bool realLighting = false;

    try {
        po::store(po::command_line_parser(argc, argv).options(desc).positional(pdesc).run(), vm);
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << "Usage: visualiser [options] showfile" << std::endl;
            std::cout << desc << std::endl;
            exit(EXIT_FAILURE);
        }

        if (vm.count("showfile"))
            showfile = vm["showfile"].as<std::string>();

        if (vm.count("universe"))
            universe = vm["universe"].as < unsigned
        int > ();

        if (vm.count("debug"))
            debug = vm["debug"].as<bool>();

        if (!vm["resolution"].empty()) {
            std::vector<unsigned int> res = vm["resolution"].as<std::vector<unsigned int>>();
            resX = res.at(0);
            resY = res.at(1);
        }

        if (vm.count("fullscreen"))
            fullscreen = true;

        if (vm.count("realLighting"))
            realLighting = true;
    } catch (po::error &e) {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    VisualiserConfig config;
    config.showFilePath = showfile;
    config.universe = universe;
    config.resX = resX;
    config.resY = resY;
    config.fullscreen = fullscreen;
    config.realLighting = realLighting;
    config.debugMessages = debug;

    return config;
}

int main(int argc, const char **argv) {
    VisualiserConfig config = parseConfigurationOptions(argc, argv);

    auto v = visualiser::VisualiserCore::instance();
    v->loadShowFile(config.showFilePath);
    v->run();

    return EXIT_SUCCESS;
}
