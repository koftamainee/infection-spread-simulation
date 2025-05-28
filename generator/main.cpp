#include <ctime>
#include <fstream>
#include <iostream>
#include <random>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

const std::vector<std::string> FIRST_NAMES = {
    "James",   "Mary",      "John",        "Patricia",  "Robert",   "Jennifer",
    "Michael", "Linda",     "William",     "Elizabeth", "David",    "Barbara",
    "Richard", "Susan",     "Joseph",      "Jessica",   "Thomas",   "Sarah",
    "Charles", "Karen",     "Christopher", "Nancy",     "Daniel",   "Lisa",
    "Matthew", "Margaret",  "Anthony",     "Betty",     "Mark",     "Sandra",
    "Donald",  "Ashley",    "Steven",      "Kimberly",  "Paul",     "Emily",
    "Andrew",  "Donna",     "Joshua",      "Michelle",  "Kenneth",  "Dorothy",
    "Kevin",   "Carol",     "Brian",       "Amanda",    "George",   "Melissa",
    "Edward",  "Deborah",   "Ronald",      "Stephanie", "Timothy",  "Rebecca",
    "Jason",   "Sharon",    "Jeffrey",     "Laura",     "Ryan",     "Cynthia",
    "Jacob",   "Kathleen",  "Gary",        "Amy",       "Nicholas", "Shirley",
    "Eric",    "Angela",    "Jonathan",    "Helen",     "Stephen",  "Anna",
    "Larry",   "Brenda",    "Justin",      "Pamela",    "Scott",    "Nicole",
    "Brandon", "Emma",      "Benjamin",    "Samantha",  "Samuel",   "Katherine",
    "Frank",   "Christine", "Gregory",     "Debra"};

const std::vector<std::string> LAST_NAMES = {
    "Smith",     "Johnson",   "Williams",  "Brown",    "Jones",     "Garcia",
    "Miller",    "Davis",     "Rodriguez", "Martinez", "Hernandez", "Lopez",
    "Gonzalez",  "Wilson",    "Anderson",  "Thomas",   "Taylor",    "Moore",
    "Jackson",   "Martin",    "Lee",       "Perez",    "Thompson",  "White",
    "Harris",    "Sanchez",   "Clark",     "Ramirez",  "Lewis",     "Robinson",
    "Walker",    "Young",     "Allen",     "King",     "Wright",    "Scott",
    "Torres",    "Nguyen",    "Hill",      "Flores",   "Green",     "Adams",
    "Nelson",    "Baker",     "Hall",      "Rivera",   "Campbell",  "Mitchell",
    "Carter",    "Roberts",   "Gomez",     "Phillips", "Evans",     "Turner",
    "Diaz",      "Parker",    "Cruz",      "Edwards",  "Collins",   "Reyes",
    "Stewart",   "Morris",    "Morales",   "Murphy",   "Cook",      "Rogers",
    "Gutierrez", "Ortiz",     "Morgan",    "Cooper",   "Peterson",  "Bailey",
    "Reed",      "Kelly",     "Howard",    "Ramos",    "Kim",       "Cox",
    "Ward",      "Richardson"};

struct Person {
  int id;
  std::string name;
  std::string surname;
  std::set<int> friends;
};

void print_usage(const std::string& program_name) {
  std::cout
      << "Usage: " << program_name << " [options]\n"
      << "Options:\n"
      << "  -n NUM       Number of people (10000-1000000, default: 10000)\n"
      << "  -m MIN_FRIENDS Minimum friends per person (default: 3)\n"
      << "  -o FILE      Output file name (default: people.txt)\n"
      << "  -p PARAMS    Simulation parameters (default: \"0.15 0.825 50 true "
         "7 4\")\n"
      << "  -h           Show this help message\n";
}

std::vector<Person> generate_people(int n) {
  std::vector<Person> people;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> first_dist(
      0, static_cast<int>(FIRST_NAMES.size()) - 1);
  std::uniform_int_distribution<> last_dist(
      0, static_cast<int>(LAST_NAMES.size()) - 1);

  people.reserve(n);
  for (int i = 0; i < n; ++i) {
    people.push_back({i, FIRST_NAMES[first_dist(gen)],
                      LAST_NAMES[last_dist(gen)], std::set<int>()});
  }
  return people;
}

void connect_people(std::vector<Person>& people, int min_friends) {
  if (people.empty()) return;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dist(0, static_cast<int>(people.size()) - 1);

  // Ensure connectivity (chain)
  for (int i = 0; i < static_cast<int>(people.size()) - 1; ++i) {
    people[i].friends.insert(i + 1);
    people[i + 1].friends.insert(i);
  }

  // Add random connections
  for (int i = 0; i < static_cast<int>(people.size()); ++i) {
    while (people[i].friends.size() < static_cast<size_t>(min_friends)) {
      int j = dist(gen);
      if (j != i && !people[i].friends.contains(j)) {
        people[i].friends.insert(j);
        people[j].friends.insert(i);
      }
    }
  }
}

void write_to_file(const std::string& filename,
                   const std::vector<Person>& people,
                   const std::string& params_line) {
  std::ofstream out(filename);
  if (!out.is_open()) {
    throw std::runtime_error("Failed to open file: " + filename);
  }

  for (const auto& person : people) {
    out << person.id + 1 << "," << person.name << "," << person.surname;
    for (int friend_id : person.friends) {
      out << "," << friend_id + 1;
    }
    out << "\n";
  }
  out << params_line << "\n";
  out.close();
}

int main(int argc, char* argv[]) {
  // Default parameters
  int num_people = 10000;
  int min_friends = 3;
  std::string output_file = "people.txt";
  std::string simulation_params = "0.15 0.825 50 true 7 4";

  // Parse command line arguments
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "-n" && i + 1 < argc) {
      num_people = std::stoi(argv[++i]);
      if (num_people < 10000 || num_people > 1000000) {
        std::cout
            << "Error: Number of people must be between 10,000 and 1,000,000\n";
        return 1;
      }
    } else if (arg == "-m" && i + 1 < argc) {
      min_friends = std::stoi(argv[++i]);
      if (min_friends < 1) {
        std::cout << "Error: Minimum friends must be at least 1\n";
        return 1;
      }
    } else if (arg == "-o" && i + 1 < argc) {
      output_file = argv[++i];
    } else if (arg == "-p" && i + 1 < argc) {
      simulation_params = argv[++i];
    } else if (arg == "-h") {
      print_usage(argv[0]);
      return 0;
    } else {
      std::cout << "Error: Unknown option or missing argument: " << arg << "\n";
      print_usage(argv[0]);
      return 1;
    }
  }

  try {
    std::cout << "Generating " << num_people << " people with " << min_friends
              << " minimum friends each...\n";

    std::vector<Person> people = generate_people(num_people);
    connect_people(people, min_friends);
    write_to_file(output_file, people, simulation_params);

    std::cout << "Successfully generated " << output_file << " with "
              << num_people << " people.\n";
    return 0;
  } catch (const std::exception& e) {
    std::cout << "Error: " << e.what() << std::endl;
    return 1;
  }
}
