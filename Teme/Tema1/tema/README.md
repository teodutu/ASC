# Tema 1 - Marketplate
The homework implements the _Multiple Producers - Multiple Consumers_ problem,
with the mention that the Consumers may bcome producers themselves by removing
products form their order lists (carts).

## Producer
This thype of thread creates new products which are then publisted to the
marketplace. It receives a list of the products it can create which is interated
indefinitely.

If the marketplace accepts the publishment of a product, the producer waits an
amount of time specified in the list of products. Otherwise, the producer waits
a time given to the thread when created.

## Consumer
Creates carts on the market place, to which it adds and removes products. When
all operations have been finished, the order for the goods in the cart is placed
and the goods are returned to the consumer by the marketplace.

Simlarly to the producer, the consumer also waits different amounts of time upon
successful and unsuccessful operation performed on its on their cart.

## Marketplace
Provides synchronization and communication between the producers and the
consumers. Stores a list of all products available to the consumers, together
with a dictionary mapping cart ID's to their respective carts (lists of goods)
and a dictionary mapping products to their producers for use when a consumer
removes a product from its cart.

In order to account for the limit to how many products one producer can have
on the marketplace at one time, this number is kept in a list (one element per
producer) whose elements are modified upon `publish()`, `add_to_cart()` and
`remove_from_cart()` operations.

Makes use of the thread-safe containers implemented in _Python_ together with
`Locks` used to safely manipulate the operations which are not thread safe, such
as incrementing/decremnentig variables.

Whenerver an order is placed, the products contained in that cart are printed
before the list is returned to the consumer.
