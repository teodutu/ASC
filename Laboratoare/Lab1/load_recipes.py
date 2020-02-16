"""
    Bonus task: load all the available coffee recipes from the folder 'recipes/'
    File format:
        first line: coffee name
        next lines: resource=percentage

    info and examples for handling files:
        http://cs.curs.pub.ro/wiki/asc/asc:lab1:index#operatii_cu_fisiere
        https://docs.python.org/3/library/io.html
        https://docs.python.org/3/library/os.path.html
"""

RECIPES_FOLDER = "recipes/"

def get_recipe(coffee_type):
    """
    Citeste cantitatile necesare de ingrediente din fiserul aferent tipului de
    cafea primit ca parametru.
    Daca fisierul exista, iar fisierul contine informatie pentru cafeaua dorita,
    returneaza un dictionar ce contine cantitatile folosite din fiecare
    ingredient.
    """
    file_name = RECIPES_FOLDER + coffee_type + ".txt"
    used_ingredients = {}

    try:
        file = open(file_name, "r")
        coffee_name = file.readline().strip('\n')

        if coffee_name == coffee_type:
            for line in file:
                tokens = line.split('=')
                used_ingredients[tokens[0]] = int(tokens[1])

        file.close()
    except FileNotFoundError:
        print("File %s does not exist!" % file_name)

    return used_ingredients
