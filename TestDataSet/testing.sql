use database orderDB1
select * from restaurant where name='ocean star'
select * from restaurant where rate=4
select * from food where name='steamed rice'
select * from restaurant, food where (restaurant.id=food.restaurant_id) and (food.name='hot pot')
select restaurant.name, restaurant.phone, food.price from restaurant, food where (restaurant.id=food.restaurant_id) and (food.name='hot pot')
select food.name, orders.quantity from food, orders where (food.id=orders.food_id) and (orders.quantity>8)
insert into customer values (300001, 'CHAD CABELLO', 'F')
insert into orders values(1, 315000, 201015, '2018/10/9', 'eight')
delete from customer where id=315000
update restaurant set phone='8774017' where name='ocean star'
