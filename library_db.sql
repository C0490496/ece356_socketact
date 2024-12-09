CREATE DATABASE library_db;

USE library_db;

CREATE TABLE books{
	id INT PRIMARY KEY AUTO_INCREMENT,
	title VARCHAR(255) NOT NULL,
	author VARCHAR(255) NOT NULL,
	year INT NOT NULL
};

//Example Books.
INSERT INTO books (titel, author, year) VALUES
('Harry Potter and the Philospher\'s Stone', 'J.K Rowling', 1997),
('Harry Potter and the Prisoner of Azkaban', 'J.K Rowling', 1999),
('Harry Potter and the Chamber of Secrets', 'J.K Rowling', 1998),
('Harry Potter and the Goblet of Fire', 'J.K Rowling', 2000),
('Harry Potter and the Order of the Phoenix', 'J.K Rowling", 2003);
