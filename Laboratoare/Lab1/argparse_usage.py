#! /opt/anaconda3/bin/python3

import argparse

def handle_input_file():
    pass

def handle_output_file():
    pass

def handle_help():
    pass

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description = "Calculeaza frecventele"
                                     + "cuvintelor dintr-un fisier de input")
    parser.add_argument("-i", type=str, required=True,
                        help="The file where the words are read from")
    parser.add_argument("-o", type=str, required=False,
                        help="The file where the frequencies will pe dumped")
    parser.add_argument("-?", type=str, required=False,
                        help="Shows the usage of the script")
    args = parser.parse_args()