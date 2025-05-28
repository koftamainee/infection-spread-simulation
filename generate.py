import random

FIRST_NAMES = [
    "James",
    "Mary",
    "John",
    "Patricia",
    "Robert",
    "Jennifer",
    "Michael",
    "Linda",
    "William",
    "Elizabeth",
    "David",
    "Barbara",
    "Richard",
    "Susan",
    "Joseph",
    "Jessica",
    "Thomas",
    "Sarah",
    "Charles",
    "Karen",
    "Christopher",
    "Nancy",
    "Daniel",
    "Lisa",
    "Matthew",
    "Margaret",
    "Anthony",
    "Betty",
    "Mark",
    "Sandra",
    "Donald",
    "Ashley",
    "Steven",
    "Kimberly",
    "Paul",
    "Emily",
    "Andrew",
    "Donna",
    "Joshua",
    "Michelle",
    "Kenneth",
    "Dorothy",
    "Kevin",
    "Carol",
    "Brian",
    "Amanda",
    "George",
    "Melissa",
    "Edward",
    "Deborah",
    "Ronald",
    "Stephanie",
    "Timothy",
    "Rebecca",
    "Jason",
    "Sharon",
    "Jeffrey",
    "Laura",
    "Ryan",
    "Cynthia",
    "Jacob",
    "Kathleen",
    "Gary",
    "Amy",
    "Nicholas",
    "Shirley",
    "Eric",
    "Angela",
    "Jonathan",
    "Helen",
    "Stephen",
    "Anna",
    "Larry",
    "Brenda",
    "Justin",
    "Pamela",
    "Scott",
    "Nicole",
    "Brandon",
    "Emma",
    "Benjamin",
    "Samantha",
    "Samuel",
    "Katherine",
    "Frank",
    "Christine",
    "Gregory",
    "Debra",
]

LAST_NAMES = [
    "Smith",
    "Johnson",
    "Williams",
    "Brown",
    "Jones",
    "Garcia",
    "Miller",
    "Davis",
    "Rodriguez",
    "Martinez",
    "Hernandez",
    "Lopez",
    "Gonzalez",
    "Wilson",
    "Anderson",
    "Thomas",
    "Taylor",
    "Moore",
    "Jackson",
    "Martin",
    "Lee",
    "Perez",
    "Thompson",
    "White",
    "Harris",
    "Sanchez",
    "Clark",
    "Ramirez",
    "Lewis",
    "Robinson",
    "Walker",
    "Young",
    "Allen",
    "King",
    "Wright",
    "Scott",
    "Torres",
    "Nguyen",
    "Hill",
    "Flores",
    "Green",
    "Adams",
    "Nelson",
    "Baker",
    "Hall",
    "Rivera",
    "Campbell",
    "Mitchell",
    "Carter",
    "Roberts",
    "Gomez",
    "Phillips",
    "Evans",
    "Turner",
    "Diaz",
    "Parker",
    "Cruz",
    "Edwards",
    "Collins",
    "Reyes",
    "Stewart",
    "Morris",
    "Morales",
    "Murphy",
    "Cook",
    "Rogers",
    "Gutierrez",
    "Ortiz",
    "Morgan",
    "Cooper",
    "Peterson",
    "Bailey",
    "Reed",
    "Kelly",
    "Howard",
    "Ramos",
    "Kim",
    "Cox",
    "Ward",
    "Richardson",
]


def generate_people(n):
    return [
        {
            "id": i,
            "name": random.choice(FIRST_NAMES),
            "surname": random.choice(LAST_NAMES),
            "friends": set(),
        }
        for i in range(n)
    ]


def connect_people(people, min_friends=3):
    n = len(people)

    # Создать цепочку связности
    for i in range(n - 1):
        people[i]["friends"].add(i + 1)
        people[i + 1]["friends"].add(i)

    # Добавить случайные связи
    for i in range(n):
        while len(people[i]["friends"]) < min_friends:
            j = random.randint(0, n - 1)
            if j != i and j not in people[i]["friends"]:
                people[i]["friends"].add(j)
                people[j]["friends"].add(i)


def write_to_file(filename, people, params_line):
    with open(filename, "w", encoding="utf-8") as f:
        for person in people:
            line = f'{person["id"]+1},{person["name"]},{person["surname"]}'
            if person["friends"]:
                friend_ids = [str(fid + 1) for fid in sorted(person["friends"])]
                line += "," + ",".join(friend_ids)
            f.write(line + "\n")
        f.write(params_line + "\n")


def main():
    num_people = 100
    min_friends = 1

    simulation_params = "0.15 0.825 50 true 7 4"

    people = generate_people(num_people)
    connect_people(people, min_friends)
    write_to_file("people.txt", people, simulation_params)

    print("File people.txt generated.")


if __name__ == "__main__":
    main()
