#include "person.h"

std::random_device Person::rd;
std::mt19937 Person::gen(Person::rd());

Person::Person(int id, std::string name, std::string surname)
    : id_(id), name_(std::move(name)), surname_(std::move(surname)) {}

bool Person::try_infect(double probability, size_t current_day) {
  if (probability <= 0.0 || probability > 1.0) {
    return false;
  }
  if (infected_ || (recovered_ && !can_be_reinfected_)) {
    return false;
  }

  std::bernoulli_distribution dist(probability);
  if (dist(gen)) {
    infected_ = true;
    day_infected_ = current_day;
    contagious_ = false;
    times_infected_++;
    return true;
  }
  return false;
}

void Person::update_contagious_status() {
  contagious_ = infected_ && !chronic_ && !isolated_;
}

bool Person::try_recover(double probability, bool immunity_after_recovery) {
  if (!infected_ || chronic_ || recovered_) {
    return false;
  }

  std::bernoulli_distribution dist(probability);
  if (dist(gen)) {
    recovered_ = true;
    infected_ = false;
    contagious_ = false;
    isolated_ = false;
    can_be_reinfected_ = !immunity_after_recovery;
    return true;
  }

  chronic_ = true;
  infected_ = false;
  contagious_ = false;
  return false;
}
void Person::start_treatment(size_t treatment_base, size_t current_day) {
  if (infected_ && !isolated_ && treatment_base > 3) {
    day_treatment_start_ = current_day;
    isolated_ = true;
    std::uniform_int_distribution<size_t> dist(treatment_base,
                                               treatment_base * 3);
    treatment_duration_ = dist(gen);
  }
}

void Person::update_isolation_status(size_t current_day) {
  if (isolated_ && current_day >= day_treatment_start_ + treatment_duration_) {
    isolated_ = false;
  }
}

bool Person::can_infect_others() const { return contagious_; }

bool Person::can_be_infected(bool immunity_after_recovery) const {
  return !infected_ && !chronic_ && !(recovered_ && immunity_after_recovery);
}
