"""
This module represents the Marketplace.

Computer Systems Architecture Course
Assignment 1
March 2020
"""

from threading import Lock, currentThread


class Marketplace:
    """
    Class that represents the Marketplace. It's the central part of the
    implementation. The producers and consumers use its methods concurrently.
    """

    def __init__(self, queue_size_per_producer):
        """
        Constructor.
        Stores the `queue_size_per_producer` and initialises the data
        structures used to store the information required by the functionality
        of the marketplace.

        @type queue_size_per_producer: Int
        @param queue_size_per_producer: the maximum size of a queue associated
        with each producer
        """
        self.max_prod_q_size = queue_size_per_producer
        self.prod_q_sizes = []  # a producer's number of items in the queue
        self.products = []  # the queue containing all available items
        self.carts = {}  # a dictionary of lists each representing a cart
        self.producers = {}  # a dictionary mapping products to their producers

        self.num_carts = 0

        self.lock_sizes = Lock()  # used to modify a producer's queue size
        self.lock_num_carts = Lock()  # used to modify the number of carts
        self.lock_register = Lock()  # used to register a producer atomically
        self.lock_print = Lock()  # used in order not to interleave the prints

    def register_producer(self):
        """
        Returns an id for the producer that calls this.
        Each id is the producer's index in the list of queue sizes.
        """
        with self.lock_register:
            prod_id = len(self.prod_q_sizes)
            self.prod_q_sizes.append(0)

        return prod_id

    def publish(self, producer_id, product):
        """
        Adds the product provided by the producer to the marketplace.
        Increases the producer's number of goods in the queue and marks
        it as the producer of the product given as parameter.

        @type producer_id: String
        @param producer_id: producer id

        @type product: Product
        @param product: the Product that will be published in the Marketplace

        @returns True or False. If the caller receives False, it should wait and
        then try again.
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
        Creates a new cart for the consumer.
        Each new cart receives a new id for which a new entry in the carts
        dictionary is created.

        @returns an int representing the cart_id
        """
        with self.lock_num_carts:
            self.num_carts += 1
            cart_id = self.num_carts

        self.carts[cart_id] = []

        return cart_id

    def add_to_cart(self, cart_id, product):
        """
        Adds a product to the given cart.
        Decrements the producer's number of goods in the queue and removes the
        given product from the queue.
        The method returns.

        @type cart_id: Int
        @param cart_id: id cart

        @type product: Product
        @param product: the product to add to cart

        @returns True or False. If the caller receives False, it should wait
        and then try again
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
        Removes a product from cart. Reintroduce it into the list of all
        available goods and increments its producer's queue size.

        @type cart_id: Int
        @param cart_id: id cart

        @type product: Product
        @param product: the product to remove from cart
        """
        self.carts[cart_id].remove(product)
        self.products.append(product)

        with self.lock_sizes:
            self.prod_q_sizes[self.producers[product]] += 1

    def place_order(self, cart_id):
        """
        Returns a list with all the products in the cart. Also prints the
        contents of said list using a lock in order not to interleave the
        printed strings.

        @type cart_id: Int
        @param cart_id: id cart
        """
        prod_list = self.carts.pop(cart_id, None)

        for product in prod_list:
            with self.lock_print:
                print("{} bought {}".format(currentThread().getName(), product))

        return prod_list
