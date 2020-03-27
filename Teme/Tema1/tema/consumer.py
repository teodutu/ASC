"""
This module represents the Consumer.

Computer Systems Architecture Course
Assignment 1
March 2020
"""

from threading import Thread, currentThread
import time


class Consumer(Thread):
    """
    Class that represents a consumer.
    """

    def __init__(self, carts, marketplace, retry_wait_time, **kwargs):
        """
        Constructor.

        :type carts: List
        :param carts: a list of add and remove operations

        :type marketplace: Marketplace
        :param marketplace: a reference to the marketplace

        :type retry_wait_time: Time
        :param retry_wait_time: the number of seconds that a consumer must wait
        until the Marketplace becomes available

        :type kwargs:
        :param kwargs: other arguments that are passed to the Thread's __init__()
        """
        Thread.__init__(self, kwargs=kwargs)

        self.carts = carts
        self.marketplace = marketplace
        self.retry_wait_time = retry_wait_time
        self.operations = {"add": self.marketplace.add_to_cart,
                           "remove": self.marketplace.remove_from_cart}

    def run(self):
        for cart in self.carts:
            cart_id = self.marketplace.new_cart()

            for operation in cart:
                num_ops = 0

                while num_ops != operation["quantity"]:
                    print("{} face operatia {} pe produsul {}".format(
                        currentThread().getName(), operation["type"],
                        operation["product"]))
                    ret = self.operations[operation["type"]](cart_id,
                                                             operation["product"])

                    if ret == False:
                        print("{} a dat fail".format(currentThread().getName()))
                        time.sleep(self.retry_wait_time)
                    else:
                        print("{} a reusit".format(currentThread().getName()))
                        num_ops += 1

            products = self.marketplace.place_order(cart_id)

            for product in products:
                print("{} bought {}".format(currentThread().getName(), product))

        print("%s a terminat" % currentThread().getName())
