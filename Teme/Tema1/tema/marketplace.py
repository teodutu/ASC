"""
This module represents the Marketplace.

Computer Systems Architecture Course
Assignment 1
March 2020
"""

from threading import Lock
from threading import currentThread


class Marketplace:
    """
    Class that represents the Marketplace. It's the central part of the implementation.
    The producers and consumers use its methods concurrently.
    """
    def __init__(self, queue_size_per_producer):
        """
        Constructor

        :type queue_size_per_producer: Int
        :param queue_size_per_producer: the maximum size of a queue associated with each producer
        """
        self.max_prod_q_size = queue_size_per_producer
        self.prod_q_sizes = []
        self.products = []
        self.carts = {}
        self.producers = {}
        self.cons_names = {}

        self.num_carts = 0
        self.cons_id = 0

        self.lock_sizes = Lock()
        self.lock_num_carts = Lock()
        self.lock_cons_id = Lock()
        self.lock_register = Lock()

    def register_producer(self):
        """
        Returns an id for the producer that calls this.
        """
        with self.lock_register:
            self.prod_q_sizes.append(0)
            return len(self.prod_q_sizes) - 1

    def publish(self, producer_id, product):
        """
        Adds the product provided by the producer to the marketplace

        :type producer_id: String
        :param producer_id: producer id

        :type product: Product
        :param product: the Product that will be published in the Marketplace

        :returns True or False. If the caller receives False, it should wait and then try again.
        """
        prod_id = int(producer_id)

        if self.prod_q_sizes[prod_id] >= self.max_prod_q_size:
            return False

        self.prod_q_sizes[prod_id] += 1

        self.products.append(product)
        self.producers[product] = prod_id

        return True

    def new_cart(self):
        """
        Creates a new cart for the consumer

        :returns an int representing the cart_id
        """
        with self.lock_num_carts:
            self.num_carts += 1
            cart_id = self.num_carts

        self.carts[cart_id] = []

        thread_name = currentThread().getName()

        if thread_name not in self.cons_names:
            with self.lock_cons_id:
                self.cons_id += 1
                crt_id = self.cons_id

            self.cons_names[thread_name] = "cons" + str(crt_id)
        
        return cart_id

    def add_to_cart(self, cart_id, product):
        """
        Adds a product to the given cart. The method returns

        :type cart_id: Int
        :param cart_id: id cart

        :type product: Product
        :param product: the product to add to cart

        :returns True or False. If the caller receives False, it should wait and then try again
        """
        with self.lock_sizes:
            if product not in self.products:
                return False

            self.prod_q_sizes[self.producers[product]] -= 1
            self.products.remove(product)

        self.carts[cart_id].append(product)

        return True

    def remove_from_cart(self, cart_id, product):
        """
        Removes a product from cart.

        :type cart_id: Int
        :param cart_id: id cart

        :type product: Product
        :param product: the product to remove from cart
        """
        self.carts[cart_id].remove(product)
        self.products.append(product)

        with self.lock_sizes:
            self.prod_q_sizes[self.producers[product]] += 1

    def place_order(self, cart_id):
        """
        Return a list with all the products in the cart.

        :type cart_id: Int
        :param cart_id: id cart
        """
        prod_list = self.carts.pop(cart_id, None)

        for product in prod_list:
            print("{} bought {}".format(self.cons_names[currentThread().getName()],
                                        product), flush = True)

        return prod_list
