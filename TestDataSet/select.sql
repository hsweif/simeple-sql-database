CREATE DATABASE selectDB
USE DATABASE selectDB
create table school (id INT,name CHAR(10))
create table price (id INT,value INT)
insert into school values (1,'qinghua'),(2,'beida'),(3,'fudan')
insert into price values (1,233),(2,666)
select * from school
select * from school,price where (school.id=price.id)
select * from school,price where (price.value<300)
select * from school,price where (school.id=price.id) AND (price.value<300)
