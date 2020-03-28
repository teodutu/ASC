"""
This module represents the Consumer.

Computer Systems Architecture Course
Assignment 1
March 2020
"""

from threading import Thread
import time


class Consumer(Thread):
    """
    Class that represents a consumer.
    """

    def __init__(self, carts, marketplace, retry_wait_time, **kwargs):
        """
        Constructor.
        Creates a new consumer, which stores the given parameters and on top
        of them stores an additional dictionary that maps the types of
        operations that are available to a cart to their corresponding
        methods in the Marketplace.

        @type carts: List
        @param carts: a list of add and remove operations

        @type marketplace: Marketplace
        @param marketplace: a reference to the marketplace

        @type retry_wait_time: Time
        @param retry_wait_time: the number of seconds that a consumer must wait
        until the Marketplace becomes available

        @type kwargs:
        @param kwargs: other arguments that are passed to the Thread's
        __init__()
        """
        Thread.__init__(self, **kwargs)

        self.carts = carts
        self.marketplace = marketplace
        self.retry_wait_time = retry_wait_time
        self.operations = {"add": self.marketplace.add_to_cart,
                           "remove": self.marketplace.remove_from_cart}

    def run(self):
        """
        Creates each new cart and performs the specified operations on this
        cart. When all oprations have been performed, the order for the cart
        is placed and the returned list is ignored as it is not useful.

        When a cart operation fails, the consumer sleeps for `retry_wait_time`
        seconds.
        """
        for cart in self.carts:
            cart_id = self.marketplace.new_cart()

            for operation in cart:
                num_ops = 0

                while num_ops < operation["quantity"]:
                    ret = self.operations[operation["type"]](cart_id,
                                                             operation["product"])

                    if ret is None or ret:
                        num_ops += 1
                    else:
                        time.sleep(self.retry_wait_time)

            self.marketplace.place_order(cart_id)
