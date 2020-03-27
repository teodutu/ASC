"""
This module represents the Marketplace.

Computer Systems Architecture Course
Assignment 1
March 2020
"""

from threading import Lock

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
        self.products = set()
        self.carts = {}
        self.producers = {}

        self.lock_remove = Lock()
        self.lock_add = Lock()

    def register_producer(self):
        """
        Returns an id for the producer that calls this.
        """
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

        self.products.add(product)
        self.producers[product] = prod_id

        with self.lock_add:
            self.prod_q_sizes[prod_id] += 1

        return True

    def new_cart(self):
        """
        Creates a new cart for the consumer

        :returns an int representing the cart_id
        """
        cart_id = len(self.carts)
        self.carts[cart_id] = set()

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
        with self.lock_remove:
            if product not in self.products:
                return False

            self.products.remove(product)

            self.prod_q_sizes[self.producers[product]] -= 1

        self.carts[cart_id].add(product)

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
        self.products.add(product)

        with self.lock_add:
           self.prod_q_sizes[self.producers[product]] += 1

    def place_order(self, cart_id):
        """
        Return a list with all the products in the cart.

        :type cart_id: Int
        :param cart_id: id cart
        """
        prod_list = list(self.carts[cart_id])
        self.carts.remove(cart_id)

        for product in prod_list:
            with self.lock_add:
                self.prod_q_sizes[self.producers[product]] += 1

            self.products.remove(product)
            self.producers.remove(product)
