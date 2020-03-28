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
        Registers a producer in the Marketplace si stores the ID returned by the
        Marketplace.

        @type products: List()
        @param products: a list of products that the producer will produce

        @type marketplace: Marketplace
        @param marketplace: a reference to the marketplace

        @type republish_wait_time: Time
        @param republish_wait_time: the number of seconds that a producer must
        wait until the marketplace becomes available

        @type kwargs:
        @param kwargs: other arguments that are passed to the Thread's
        __init__()
        """
        Thread.__init__(self, **kwargs)

        self.products = products
        self.marketplace = marketplace
        self.republish_wait_time = republish_wait_time

        self.prod_id = self.marketplace.register_producer()

    def run(self):
        """
        Adds all the products stored in the current producer to the Marketplace
        in an infinite loop. Once all products have been published, the product
        list is iterated again.

        If the publish operation succeeds, the producer sleeps for the time
        associated with the current poduct. Otherwise, it sleeps for
        `republish_wait_time` seconds.
        """
        while True:
            for (product, num_prod, wait_time) in self.products:
                i = 0

                while i < num_prod:
                    ret = self.marketplace.publish(str(self.prod_id), product)

                    if ret:
                        time.sleep(wait_time)
                        i += 1
                    else:
                        time.sleep(self.republish_wait_time)
