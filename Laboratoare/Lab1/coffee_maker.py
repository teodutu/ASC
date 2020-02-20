#! /opt/anaconda3/bin/python3

"""
A command-line controlled coffee maker.
"""

import sys
import load_recipes as recipes

# Implement the coffee maker's commands. Interact with the user via stdin and print to stdout.

# Requirements:
#    - use functions
#    - use __main__ code block
#    - access and modify dicts and/or lists
#    - use at least once some string formatting (e.g. functions such as strip(), lower(),
#    format()) and types of printing (e.g. "%s %s" % tuple(["a", "b"]) prints "a b"
#    - BONUS: read the coffee recipes from a file, put the file-handling code in another module
#    and import it (see the recipes/ folder)

# There's a section in the lab with syntax and examples for each requirement.

# Feel free to define more commands, other coffee types, more resources if you'd like and have time.

# Tips:
# *  Start by showing a message to the user to enter a command, remove our initial messages
# *  Keep types of available coffees in a data structure such as a list or dict
# e.g. a dict with coffee name as a key and another dict with resource mappings (resource:percent)
#as value

# Functii
def exit_coffee_maker():
    """Retruneaza True ca sa incheie loopul din run_commands."""
    return True

def list_coffees():
    """Listeaza toate tipurile de cafele pe care aparatul le poate face."""
    length = len(COFFEE_TYPES)

    for i in range(length):
        if i == length - 1:
            print("%s" % COFFEE_TYPES[i])
        else:
            print(str(COFFEE_TYPES[i]) + ", ", end="")

def make_coffee():
    """
    Citeste de la stdin un tip de cafea, dupa care obtine ingredientele necesare.

    In cazul in care tipul de cafea este valid si sunt disponibile destule
    ingrediente, scade cantitatile de ingrediente necesare prepararii cafelei din
    cele totale.
    In caz contrar, afiseaza mesaje de eroare corespunzatoare.
    """
    print("Which type?")

    coffee_type = sys.stdin.readline().strip("\n")
    used_ingredients = recipes.get_recipe(coffee_type)

    if not bool(used_ingredients):
        return

    can_make_coffee = True

    for ingredient in RESOURCES:
        if RESOURCES[ingredient] < used_ingredients[ingredient]:
            can_make_coffee = False
            print("Not enough %s!" % ingredient)
            break

    if can_make_coffee:
        for ingredient in RESOURCES:
            RESOURCES[ingredient] -= used_ingredients[ingredient]

        print("Here's your %s!" % coffee_type)

def coffee_help():
    """Listeaza comenzile suportate impreuna cu scurte descrieri ale acestora."""
    print("Available commands:\n" +
          "'list' - lists available coffee types\n" +
          "'status' - prints the available amount of each ingredient\n" +
          "'refill' - brings the amount of one or all of the ingredients to 100%\n" +
          "'make' - prepares a certain type of coffe\n" +
          "'exit' - exits the coffee maker")

def refill():
    """
    Aduce cantitatea dintr-un anumit ingredient la FULL, sau pe cea a tuturor
    ingredientelor in caz ca se specifica 'all'.
    """
    print("Which resource? Type 'all' for refilling everything")

    resource = sys.stdin.readline().strip("\n")

    if resource in RESOURCES:
        RESOURCES[resource] = FULL
    elif resource == ALL:
        for key in RESOURCES:
            RESOURCES[key] = FULL

def status():
    """Afiseaza cantitatile curente de ingrediente."""
    for ingredient, amount in RESOURCES.items():
        print("%s: %d%%" % (ingredient, amount))

def run_commands():
    """
    Citeste comenzi de la tastatura si apeleaza functiile corespunzatoare
    pana se citeste comanda exit.
    """
    should_close = None

    while should_close is None:
        print("\nEnter command")

        command = sys.stdin.readline().strip("\n")

        if command in COMMANDS:
            should_close = COMMANDS[command]()
        else:
            print("Unknown command!")

# Commands
EXIT = "exit"
LIST_COFFEES = "list"
MAKE_COFFEE = "make"  #!!! when making coffee you must first check that you have enough resources!
HELP = "help"
REFILL = "refill"
RESOURCE_STATUS = "status"
COMMANDS = {EXIT: exit_coffee_maker, LIST_COFFEES: list_coffees, MAKE_COFFEE: make_coffee,
            REFILL: refill, RESOURCE_STATUS: status, HELP: coffee_help}

# Coffee examples
ESPRESSO = "espresso"
AMERICANO = "americano"
CAPPUCCINO = "cappuccino"

COFFEE_TYPES = [ESPRESSO, AMERICANO, CAPPUCCINO]

# Resources examples
FULL = 100

WATER = "water"
COFFEE = "coffee"
MILK = "milk"
ALL = "all"

# Coffee maker's resources - the values represent the fill percents
RESOURCES = {WATER: FULL, COFFEE: FULL, MILK: FULL}

"""
Example result/interactions:

I'm a smart coffee maker
Enter command:
list
americano, cappuccino, espresso
Enter command:
status
water: 100%
coffee: 100%
milk: 100%
Enter command:
make
Which coffee?
espresso
Here's your espresso!
Enter command:
refill
Which resource? Type 'all' for refilling everything
water
water: 100%
coffee: 90%
milk: 100%
Enter command:
exit
"""

def main():
    """Afiseaza un mesaj initial, dupa care proceseaza comenzi."""
    print("I'm a smart coffee maker")
    run_commands()

if __name__ == "__main__":
    main()
