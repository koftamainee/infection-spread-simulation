#pragma once

#include <random>
#include <string>

class Person final {
 public:
  Person() = default;
  Person(int id, std::string name, std::string surname);

  int id() const { return id_; }
  const std::string& name() const { return name_; }
  const std::string& surname() const { return surname_; }
  bool is_infected() const { return infected_; }
  bool is_recovered() const { return recovered_; }
  bool is_chronic() const { return chronic_; }
  bool is_isolated() const { return isolated_; }
  bool is_contagious() const { return contagious_; }
  size_t day_infected() const { return day_infected_; }
  size_t times_infected() const { return times_infected_; }
  size_t treatment_duration() const { return treatment_duration_; }
  size_t treatment_start_day() const { return day_treatment_start_; }

  bool try_infect(double probability, size_t current_day);
  void update_contagious_status();
  bool try_recover(double probability, bool immunity_after_recovery);
  void start_treatment(size_t treatment_base, size_t current_day);
  void update_isolation_status(size_t current_day);

  bool can_infect_others() const;
  bool can_be_infected(bool immunity_after_recovery) const;

 private:
  int id_ = 0;
  std::string name_;
  std::string surname_;

  bool infected_ = false;
  bool recovered_ = false;
  bool chronic_ = false;
  bool isolated_ = false;
  bool contagious_ = false;
  size_t day_infected_ = 0;
  size_t day_treatment_start_ = 0;
  size_t times_infected_ = 0;
  size_t treatment_duration_ = 0;
  bool can_be_reinfected_ = true;

  static std::random_device rd;
  static std::mt19937 gen;
};
