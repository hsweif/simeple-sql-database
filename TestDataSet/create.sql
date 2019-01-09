CREATE DATABASE orderDB
USE DATABASE orderDB
CREATE TABLE restaurant ( id INT NOT NULL, name CHAR(25) NOT NULL, address CHAR(100), phone CHAR(20), rate FLOAT, PRIMARY KEY (id))
CREATE TABLE customer( id INT NOT NULL, name CHAR(25) NOT NULL, gender CHAR(1) NOT NULL, PRIMARY KEY (id) )
CREATE TABLE food( id INT NOT NULL, restaurant_id INT, name CHAR(100) NOT NULL, price FLOAT NOT NULL, PRIMARY KEY (id), FOREIGN KEY (restaurant_id) REFERENCES restaurant(id))
CREATE TABLE orders( id INT NOT NULL, customer_id INT NOT NULL, food_id INT NOT NULL, date_ CHAR(10), quantity INT, PRIMARY KEY (id), FOREIGN KEY (customer_id) REFERENCES customer(id), FOREIGN KEY (food_id) REFERENCES food(id))
show TABLE orders
show TABLE food
