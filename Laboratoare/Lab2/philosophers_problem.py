#! /usr/local/bin/python3.8

from threading import Thread, Lock
import random
import time
import argparse

class Philosopher(Thread):
    """Implements a philosopher"""
    def __init__(self, id, left_fork, right_fork):
        Thread.__init__(self)
        self.id = id
        self.left_fork = left_fork
        self.right_fork = right_fork

    def run(self):
        time_to_wait = random.uniform(0, 0.3)
        time.sleep(time_to_wait)

        with self.left_fork:
            time.sleep(0.1)
            with self.right_fork:
                print("Philosopher %d is eating" % self.id)

def main():
    parser = argparse.ArgumentParser(description="Showcases the philosophers' "
                                     + "problem using locks.")
    parser.add_argument("-p", type=int, required=True, dest="NUM_PHILOSOPHERS",
                        help="the number of philosophers")
    args = parser.parse_args()

    random.seed(time.time())

    forks = []
    philosophers = [None] * args.NUM_PHILOSOPHERS

    for i in range(args.NUM_PHILOSOPHERS):
        forks.append(Lock())

    for i in range(args.NUM_PHILOSOPHERS):
        philosophers[i] = Philosopher(i, forks[i - 1], forks[i])
        philosophers[i].start()

    for i in range(args.NUM_PHILOSOPHERS):
        philosophers[i].join()

if __name__ == "__main__":
    main()
        