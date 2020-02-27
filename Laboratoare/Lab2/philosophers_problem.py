#! /usr/local/bin/python3.8

from threading import Thread, Lock
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
        while True:
            with self.left_fork:
                time.sleep(0.1)
                result = self.right_fork.acquire(False)
                
                if result == True:
                    break

            self.left_fork, self.right_fork = self.right_fork, self.left_fork

        print("Philosopher %d is eating" % self.id)

        self.right_fork.release()

def main():
    parser = argparse.ArgumentParser(description="Showcases the philosophers' "
                                     + "problem using locks.")
    parser.add_argument("-p", type=int, required=True, dest="NUM_PHILOSOPHERS",
                        help="the number of philosophers")
    args = parser.parse_args()

    forks = [Lock() for i in range(args.NUM_PHILOSOPHERS)]
    philosophers = [Philosopher(i, forks[i - 1], forks[i])
                        for i in range(args.NUM_PHILOSOPHERS)]

    for philosopher in philosophers:
        philosopher.start()

    for philosopher in philosophers:
        philosopher.join()

if __name__ == "__main__":
    main()
        