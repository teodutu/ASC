#! /usr/local/bin/python3.8

"""
    Basic thread handling exercise:

    Use the Thread class to create and run more than 10 threads which print their name and a random
    number they receive as argument. The number of threads must be received from the command line.

    e.g. Hello, I'm Thread-96 and I received the number 42

"""

import sys
import random
import threading
import time

def print_thread(number):
    print("Hello, I'm %s and I received the number %d"
          % (threading.currentThread().getName(), number))

def main():
    if len(sys.argv) != 2:
        print("Usage:\n\t%s num_threads" % sys.argv[0])
        sys.exit()

    random.seed(time.time())

    num_threads = int(sys.argv[1])
    threads = [threading.Thread(target = print_thread,
                args = (random.randint(-0x80000000, 0x7fffffff), ))
                    for i in range (num_threads)]

    for thread in threads:
        thread.start()

    for thread in threads:
        thread.join()

if __name__ == "__main__":
    main()