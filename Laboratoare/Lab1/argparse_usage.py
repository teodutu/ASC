#! /opt/anaconda3/bin/python3

import argparse
import os
import re

def handle_input_file(file_name):
    frequency = {}

    if not os.path.isfile(file_name):
        print("File %s does not exist!" % file_name)
        return frequency, False

    file = open(file_name, "r")
    words = file.read().replace('\n', ' ').split(' ')

    for word in words:
        if word in frequency:
            frequency[word] += 1
        else:
            frequency[word] = 1

    file.close()

    return frequency, True

def handle_output_file(file_name, frequency):
    if bool(frequency) == False:
        print("The input file is empty!")
        return

    file = open(file_name, "w+")

    for key, value in frequency.items():
        file.write("%s : %d\n" % (key, value))

    file.close()

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Calculates the frequencies "
                                     + "of the words contained in the input file.")
    parser.add_argument("-i", type=str, required=True, dest="INPUT_FILE",
                        help="The file where the words are read from")
    parser.add_argument("-o", type=str, required=False, dest="OUTPUT_FILE",
                        help="The file where the frequencies will pe dumped")
    args = parser.parse_args()

    frequency, found_file = handle_input_file(args.INPUT_FILE)

    if found_file:
        handle_output_file(args.OUTPUT_FILE, frequency)