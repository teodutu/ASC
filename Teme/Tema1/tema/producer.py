"""
This module represents the Producer.

Computer Systems Architecture Course
Assignment 1
March 2020
"""

from threading import Thread
import time


class Producer(Thread):
    """
    Class that represents a producer.
    """

    def __init__(self, products, marketplace, republish_wait_time, **kwargs):
        """
        Constructor.

        @type products: List()
        @param products: a list of products that the producer will produce

        @type marketplace: Marketplace
        @param marketplace: a reference to the marketplace

        @type republish_wait_time: Time
        @param republish_wait_time: the number of seconds that a producer must
        wait until the marketplace becomes available

        @type kwargs:
        @param kwargs: other arguments that are passed to the Thread's __init__()
        """
        Thread.__init__(self, **kwargs)

        self.products = products
        self.marketplace = marketplace
        self.republish_wait_time = republish_wait_time

        self.id = self.marketplace.register_producer()

    def run(self):
        while True:
            for (product, num_prod, wait_time) in self.products:
                i = 0

                while i < num_prod:
                    ret = self.marketplace.publish(str(self.id), product)

                    if ret == True:
                        time.sleep(wait_time)
                        i += 1
                    else:
                        time.sleep(self.republish_wait_time)

