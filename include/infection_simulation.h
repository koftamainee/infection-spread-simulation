#pragma once

#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
#include <new>
#include <ostream>
#include <random>
#include <string>
#include <unordered_map>

#include "client_logger_builder.h"
#include "logger_guardant.h"
#include "nlohmann/json.hpp"
#include "person.h"

class InfectionSimulation final : private logger_guardant {
 public:
  InfectionSimulation(std::string const &filename);

  void print_people(std::ostream &out = std::cout);
  std::string params() const;
  InfectionSimulation &simulate();
  InfectionSimulation &print_results(std::ostream &out = std::cout);

 private:
  void parse_input_file(const std::string &file_path);
  void verify_graph() const;

  logger *get_logger() const override {
    if (log_ == nullptr) {
      static const char *const cstr_config_path = []() {
        const char *path = getenv("CONFIG_PATH");
        return path ? path : "";
      }();
      if (cstr_config_path[0] == '\0') {
        return nullptr;
      }
      std::string const configuration_file_path = cstr_config_path;
      try {
        logger *new_log = client_logger_builder()
                              .transform_with_configuration(
                                  configuration_file_path, "logging")
                              ->build();
        const_cast<std::unique_ptr<logger> &>(log_).reset(new_log);
      } catch (std::bad_alloc const &e) {
        throw;
      } catch (std::exception const &e) {
        return nullptr;
      }
    }
    return log_.get();
  }

  std::unordered_map<int, Person> people_by_id_;
  std::unordered_map<int, std::vector<int>> adjacency_list_;

  std::random_device rd;
  std::mt19937 gen;

  std::unique_ptr<logger> log_ = nullptr;
  size_t peoples_count_ = 0;
  size_t day_ = 1;

  double infection_probability;
  double recovery_probability;
  size_t simulation_duration_days;
  size_t treatment_duration_base;
  size_t incubation_period_days;
  bool has_immunity_after_recovery;

  int random_int(int lover, int upper);
  double random_double(double lover, double upper);

  void update_people_status();
  void infect_new_people();
};
