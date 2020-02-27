# Laborator 1 - introducer in Python
Se implementeaza un automat de cafea ce primeste comenzi de la `stdin`.

Tipurile de comenzi acceptate sunt:

- `list` - enumera tipurile de cafele posibile
- `status` - afiseaza cantitatile disponibile din fiecare ingredient
- `refill` - aduce un anumit ingredient (sau pe toate) la 100%
- `make` - prepara o anumita cafea dupa o reteta citita dintr-un fisier
- `exit` - opreste scriptul

## load_recipes.py
Expune functia `get_recipe` care cauta in directorul `recipes` fisierul dat
din care citeste cantitatile necesare din fiecare ingredient, iar apoi
returneaza un dictionar cu aceste cantitati si ingredientele respective

## make
Se apeleaza `get_recipe` descrisa mai sus, iar daca dictionarul nu este vid
se verifica sa existe cantitati suficiente din acele ingrediente. Daca sunt
suficiente, se scad cele necesare cafelei din cele totale si se afiseaza un
mesaj simbolizand finalizarea comenzii

## argparse_usage.py
Un script care numara frecventele cuvintelor dintr-un fisier si le scrie
in alt fisier. Numele fisierelor sunt luate din linia de comanda folosind
`argparse` care urmeaza sa fie folosit in (mai) toate laboratoarele de acum
inainte.