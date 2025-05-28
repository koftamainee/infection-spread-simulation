#include "infection_simulation.h"

#include <fstream>
#include <iterator>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>

InfectionSimulation::InfectionSimulation(std::string const& filename)
    : gen(rd()) {
  parse_input_file(filename);
}

void InfectionSimulation::print_people(std::ostream& out) {
  for (const auto& [id, person] : people_by_id_) {
    out << id << "," << person.name() << "," << person.surname();
    for (int acquaintance_id : adjacency_list_[person.id()]) {
      out << "," << acquaintance_id;
    }
    out << "\n";
  }
}

std::string InfectionSimulation::params() const {
  std::ostringstream param_stream;
  param_stream << infection_probability << " " << recovery_probability << " "
               << simulation_duration_days << " "
               << (has_immunity_after_recovery ? "true" : "false") << " "
               << treatment_duration_base << " " << incubation_period_days;
  return param_stream.str();
}

void InfectionSimulation::parse_input_file(const std::string& file_path) {
  information_with_guard("Begin to parse input file: " + file_path);
  std::ifstream file(file_path);
  if (!file.is_open()) {
    std::string const error_message = "Failed to open file: " + file_path;
    error_with_guard(error_message);
    throw std::runtime_error(error_message);
  }

  std::vector<std::string> lines;
  std::string line;

  while (std::getline(file, line)) {
    if (!line.empty()) {
      lines.push_back(line);
    }
  }

  if (lines.empty()) {
    std::string const error_message = "File is empty";
    throw std::runtime_error(error_message);
  }

  const std::string& param_line = lines.back();
  std::istringstream param_stream(param_line);

  std::string immunity_flag;
  if (!(param_stream >> infection_probability >> recovery_probability >>
        simulation_duration_days >> immunity_flag >> treatment_duration_base >>
        incubation_period_days)) {
    std::string const error_message = "Failed to parse params string.";
    error_with_guard(error_message);
    throw std::runtime_error(error_message);
  }

  if (immunity_flag == "true") {
    has_immunity_after_recovery = true;
  } else if (immunity_flag == "false") {
    has_immunity_after_recovery = false;
  } else {
    std::string const error_message =
        "Field has_immunity_after_recovery should true or false.";
    error_with_guard(error_message);
    throw std::runtime_error(error_message);
  }

  for (int i = 0; i < lines.size() - 1; ++i) {
    const std::string& person_line = lines[i];
    std::istringstream ss(person_line);
    std::string token;
    std::vector<std::string> tokens;

    while (std::getline(ss, token, ',')) {
      tokens.push_back(token);
    }

    if (tokens.size() < 3) {
      std::string const error_message =
          "Not enougth Person params: " + person_line;
      error_with_guard(error_message);
      throw std::runtime_error(error_message);
    }

    Person person(std::stoi(tokens[0]), tokens[1], tokens[2]);

    for (size_t j = 3; j < tokens.size(); ++j) {
      int friend_id = std::stoi(tokens[j]);
      adjacency_list_[person.id()].push_back(friend_id);
    }

    people_by_id_[person.id()] = std::move(person);
  }
  peoples_count_ = people_by_id_.size();
  information_with_guard("Input file succesfully parsed, people count: " +
                         std::to_string(peoples_count_));
  verify_graph();
  information_with_guard("Graph is correct");
}

void InfectionSimulation::verify_graph() const {
  information_with_guard("Started verifying the graph");
  for (auto const& man : adjacency_list_) {
    for (auto const& friend_id : man.second) {
      if (!people_by_id_.contains(friend_id)) {
        std::string const error_message = "Invalid graph format";
        error_with_guard(error_message);
        throw std::runtime_error(error_message);
      }
    }
  }
}

InfectionSimulation& InfectionSimulation::simulate() {
  if (simulation_duration_days == 0) {
    return *this;
  }

  if (day_ == 1) {
    auto infected = people_by_id_.begin();
    std::advance(infected, random_int(0, static_cast<int>(peoples_count_) - 1));
    infected->second.try_infect(1.0, day_);

    information_with_guard("Initial infection: " + infected->second.name() +
                           " " + infected->second.surname() +
                           " was infected on day 1");
  }

  for (; day_ <= simulation_duration_days; ++day_) {
    update_people_status();
    infect_new_people();
  }

  return *this;
}

int InfectionSimulation::random_int(int lover, int upper) {
  std::uniform_int_distribution<int> distr(lover, upper);
  int result = distr(gen);
  return result;
}

double InfectionSimulation::random_double(double lover, double upper) {
  std::uniform_real_distribution<double> distr(lover, upper);
  double result = distr(gen);
  return result;
}

InfectionSimulation& InfectionSimulation::print_results(std::ostream& out) {
  out << "Never infected people:\n";
  size_t cnt = 1;
  for (const auto& [id, person] : people_by_id_) {
    if (!person.is_infected() && !person.is_recovered() &&
        !person.is_chronic()) {
      out << cnt++ << ". " << person.surname() << " " << person.name() << "\n";
    }
  }

  cnt = 1;
  out << "\nRecovered people:\n";
  for (const auto& [id, person] : people_by_id_) {
    if (person.is_recovered()) {
      out << cnt++ << ". " << person.surname() << " " << person.name() << "\n";
    }
  }

  cnt = 1;
  out << "\nRecovered people with non-recovered neighbors:\n";
  for (const auto& [id, person] : people_by_id_) {
    if (person.is_recovered()) {
      bool has_non_recovered_neighbor = false;
      for (int neighbor_id : adjacency_list_.at(id)) {
        const auto& neighbor = people_by_id_.at(neighbor_id);
        if (!neighbor.is_recovered()) {
          has_non_recovered_neighbor = true;
          break;
        }
      }
      if (has_non_recovered_neighbor) {
        out << cnt++ << ". " << person.surname() << " " << person.name()
            << "\n";
      }
    }
  }

  cnt = 1;
  if (!has_immunity_after_recovery) {
    out << "\nNever infected people with all neighbors infected more than "
           "once:\n";
    for (const auto& [id, person] : people_by_id_) {
      if (!person.is_infected() && !person.is_recovered() &&
          !person.is_chronic()) {
        bool all_neighbors_infected_multiple = true;
        for (int neighbor_id : adjacency_list_.at(id)) {
          const auto& neighbor = people_by_id_.at(neighbor_id);
          if (neighbor.times_infected() <= 1) {
            all_neighbors_infected_multiple = false;
            break;
          }
        }
        if (all_neighbors_infected_multiple) {
          out << cnt++ << ". " << person.surname() << " " << person.name()
              << "\n";
        }
      }
    }
  }

  cnt = 1;
  if (!has_immunity_after_recovery) {
    out << "\nPeople infected more than once:\n";
    for (const auto& [id, person] : people_by_id_) {
      if (person.times_infected() > 1) {
        out << cnt++ << ". " << person.surname() << " " << person.name()
            << "\n";
      }
    }
  }

  cnt = 1;
  out << "\nPeopgle with chronic condition:\n";
  for (const auto& [id, person] : people_by_id_) {
    if (person.is_chronic()) {
      out << cnt++ << ". " << person.surname() << " " << person.name() << "\n";
    }
  }

  return *this;
}

void InfectionSimulation::update_people_status() {
  for (auto& [id, person] : people_by_id_) {
    person.update_contagious_status();
    if (person.is_infected() &&
        day_ >= person.day_infected() + incubation_period_days) {
      if (!person.is_isolated()) {
        person.start_treatment(treatment_duration_base, day_);
        information_with_guard(person.name() + " " + person.surname() +
                               " started treatment on day " +
                               std::to_string(day_));
      }
      if (day_ >= person.treatment_start_day() + person.treatment_duration()) {
        if (person.try_recover(recovery_probability,
                               has_immunity_after_recovery)) {
          information_with_guard(person.name() + " " + person.surname() +
                                 " recovered on day " + std::to_string(day_));
        }
      }
    }

    person.update_isolation_status(day_);
  }
}

void InfectionSimulation::infect_new_people() {
  for (const auto& [id, person] : people_by_id_) {
    if (person.can_infect_others() && !person.is_isolated()) {
      for (int neighbor_id : adjacency_list_.at(id)) {
        auto& neighbor = people_by_id_.at(neighbor_id);
        if (neighbor.can_be_infected(has_immunity_after_recovery) &&
            !neighbor.is_isolated()) {
          if (neighbor.try_infect(infection_probability, day_)) {
            information_with_guard("Infection: " + person.name() + " " +
                                   person.surname() + " infected " +
                                   neighbor.name() + " " + neighbor.surname() +
                                   " on day " + std::to_string(day_));
          }
        }
      }
    }
  }
}
