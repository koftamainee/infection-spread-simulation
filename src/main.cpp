#include <cstdlib>
#include <fstream>
#include <nlohmann/json.hpp>

#include "infection_simulation.h"

int main(int argc, char **argv) {
  char const *cstr_config_path = getenv("CONFIG_PATH");
  if (cstr_config_path == nullptr) {
    std::cerr << "CONFIG_PATH env is not set.";
    return 1;
  }
  std::string const configuration_file_path = cstr_config_path;
  std::ifstream config_file(configuration_file_path);

  nlohmann::json parsed_config;
  config_file >> parsed_config;

  std::stringstream configuration_path_stream("input_file");

  std::string configuration_path_part;
  while (
      std::getline(configuration_path_stream, configuration_path_part, ':')) {
    if (std::isdigit(configuration_path_part[0]) != 0) {
      parsed_config = parsed_config.at(std::stoi(configuration_path_part));
    } else {
      parsed_config = parsed_config.at(configuration_path_part);
    }
  }

  InfectionSimulation(parsed_config.at("file_path")).simulate().print_results();
}
