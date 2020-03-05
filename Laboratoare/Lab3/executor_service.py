#! /usr/local/bin/python3.8

import random
from concurrent.futures import ThreadPoolExecutor, as_completed

SAMPLE_LEN = 10000
NUM_SAMPLES = 100
SEED = 42
MAX_WORKERS = 30

def find_substr(DNA_samples, sequence, index):
    if DNA_samples[index].find(sequence) != -1:
        return "DNA sequence found in sample {}.".format(index)

    return ""

def main():
    random.seed(SEED)

    DNA_samples = ["".join([random.choice("ATGC") for i in range(SAMPLE_LEN)])
                        for i in range(NUM_SAMPLES)]
    sequence = "TCCGGCCCGGGTT"

    with ThreadPoolExecutor(max_workers=MAX_WORKERS) as executor:
        results = [executor.submit(find_substr, DNA_samples, sequence, i)
                        for i in range(NUM_SAMPLES)]

        for res in as_completed(results):
            result_str = res.result()

            if (result_str != ""):
                print(result_str)

if __name__ == "__main__":
    main()