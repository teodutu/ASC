#! /usr/local/bin/python3.8

"""
Coffee Factory: A multiple producer - multiple consumer approach

Generate a base class Coffee which knows only the coffee name
Create the Espresso, Americano and Cappuccino classes which inherit the base class knowing that
each coffee type has a predetermined size.
Each of these classes have a get message method

Create 3 additional classes as following:
    * Distributor - A shared space where the producers puts coffees and the consumers takes them
    * CoffeeFactory - An infinite loop, which always sends coffees to the distributor
    * User - Another infinite loop, which always takes coffees from the distributor

The scope of this exercise is to correctly use threads, classes and synchronization objects.
The size of the coffee (ex. small, medium, large) is chosen randomly everytime.
The coffee type is chosen randomly everytime.

Example of output:

Consumer 65 consumed espresso
Factory 7 produced a nice small espresso
Consumer 87 consumed cappuccino
Factory 9 produced an italian medium cappuccino
Consumer 90 consumed americano
Consumer 84 consumed espresso
Factory 8 produced a strong medium americano
Consumer 135 consumed cappuccino
Consumer 94 consumed americano
"""

import argparse
import random
from threading import BoundedSemaphore, Semaphore, Thread
import time

class Coffee:
    """ Base class """
    def __init__(self, name, size):
        self.name = name
        self.size = size

    def get_name(self):
        """ Returns the coffee name """
        return self.name

    def get_size(self):
        """ Returns the coffee size """
        return self.size

class Espresso(Coffee):
    """ Espresso implementation """
    def __init__(self, size):
        Coffee.__init__(self, "espresso", size)

    def get_message(self):
        """ Output message """
        return "nice {} {}".format(self.get_size(), self.get_name())

class Americano(Coffee):
    """ Espresso implementation """
    def __init__(self, size):
        Coffee.__init__(self, "americano", size)

    def get_message(self):
        """ Output message """
        return "strong {} {}".format(self.get_size(), self.get_name())

class Cappuccino(Coffee):
    """ Cappuccino implementation """
    def __init__(self, size):
        Coffee.__init__(self, "cappuccino", size)

    def get_message(self):
        """ Output message """
        return "italian {} {}".format(self.get_size(), self.get_name())

class Distributor:
    """Distributor - the buffer"""
    def __init__(self, buffer_size):
        self.buffer_size = buffer_size
        self.mutex_prod = BoundedSemaphore(1)
        self.mutex_cons = BoundedSemaphore(1)
        self.empty = Semaphore(value=buffer_size)
        self.full = Semaphore(value=0)
        self.buffer = [None] * buffer_size
        self.last = 0
        self.first = 0
    
    def put(self, elem):
        self.empty.acquire()

        with self.mutex_prod:
            self.buffer[self.last] = elem
            self.last = (self.last + 1) % self.buffer_size

        self.full.release()

    def get(self):
        self.full.acquire()
        elem = None

        with self.mutex_cons:
            elem = self.buffer[self.first]
            self.first = (self.first + 1) % self.buffer_size

        self.empty.release()

        return elem

class CoffeeFactory(Thread):
    """CoffeeFactory - the producer"""
    def __init__(self, id, buff, sizes, coffees):
        Thread.__init__(self)
        self.id = id
        self.buff = buff
        self.sizes = sizes
        self.coffees = coffees

    def run(self):
        while True:
            coffee_type = random.choice(self.coffees)
            coffee_size = random.choice(self.sizes)

            coffee = coffee_type(coffee_size)
            print("Factory %d produced %s" % (self.id, coffee.get_message()))

            self.buff.put(coffee)

class User(Thread):
    """User - the consumer"""
    def __init__(self, id, buff):
        Thread.__init__(self)
        self.id = id
        self.buff = buff

    def run(self):
        while True:
            coffee = self.buff.get()
            print("Consumer %d consumed %s" % (self.id, coffee.get_message()))

def main():
    parser = argparse.ArgumentParser(description="Showcases the producer - "
                                     + "consumer model via a coffee machine.")
    parser.add_argument("-p", type=int, required=True, dest="NUM_PRODUCERS",
                        help="the number of producers")
    parser.add_argument("-c", type=int, required=True, dest="NUM_CONSUMERS",
                        help="the number of consumers")
    parser.add_argument("-b", type=int, required=True, dest="BUFFER_SIZE",
                        help="the size of the communication buffer")
    args = parser.parse_args()

    random.seed(time.time())

    sizes = ["small", "medium", "large"]
    coffees = [Espresso, Americano, Cappuccino]

    buff = Distributor(args.BUFFER_SIZE)

    producers = [CoffeeFactory(i, buff, sizes, coffees)
                    for i in range(args.NUM_PRODUCERS)]
    users = [User(i, buff)
                for i in range(args.NUM_CONSUMERS)]

    for producer in producers:
        producer.start()

    for user in users:
        user.start()

if __name__ == '__main__':
    main()
