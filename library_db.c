#include <mysql/mysql.h>	//This includes the MySQL Connector/C library.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//A function to setup the connection to the MySQL database.
void connect_to_db(MYSQL **conn){
	*conn = mysql_init(NULL); //Initizalizses the MySQL connection object.
	if(*conn == NULL){
		fprintf(stderr, "mysql_init() failed \n");
		exit(EXIT_FAILURE);	//Exit program if initialization fails
	}

	if(mysql_real_connect(*conn, "localhost", "root", "password", "library_db.sql", 0 , NULL, 0) == NULL){
		fprintf(stderr, "mysql_real_connect() failed: %s\n", mysql_error(*conn));
		mysql_close(*conn);	//Closes the mySQL connection.
		exit(EXIT_FAILURE);	//Exit the program if the connections fails.
	}
}

//This is a function to add a new book to the databse.
void add_book(MYSQL *conn){
	char title[255], author[255]; // Variables to store the book details.
	int year;

	printf("Enter the book title: ");
	scanf(" %[^\n]s", title);	//This reads the users input for the title of the book.
	printf("Enter the author: ");
	scanf(" %[^\n]s", author);	//Reads the input for the author.
	printf("Enter the year: ");
	scanf("%d", &year);	//Read the input for the year.

	char query[512];
	snprintf(query, sizeof(query),"INSERTING into books (title, author, year) VALUES ('%s', '%s', %d)", title, author, year);

	if(mysql_query(conn, query)){
		fprintf(stderr, "INSERT has failed: %s\n", mysql_error(conn)); //Provide and error message.
	}else{
		printf("Book has been added successfully.\n");
	}
}

//This is a function to view the books
void view_books(MYSQL *conn){
	if(mysql_query(conn, "Select * FROM books")){
		fprintf(stderr, "Select failed: %s\n", mysql_error(conn));
		return;
	}

	MYSQL_RES *res = mysql_store_result(conn);	//Store the query result.
	if(res == NULL){
		fprintf(stderr, "mysql_store_results() failed: %s\n", mysql_error(conn));
		return;
	}

	MYSQL_ROW row;
	printf("\n%-5s %-30s %-30s %-5s\n", "ID", "Title", "Author", "Year");
	printf("**********************************************************************\n");

	while((row = mysql_fetch_row(res))){
		printf("%-5s %-30s %-30s %-5s\n", row[0], row[1], row[2], row[3]);
	}
	mysql_free_result(res);
}

//This is a function to search for a book.
void search_book(MYSQL *conn){
	char title[255];
	printf("Enter the title to search:");
	scanf(" %[^\n]s", title);

	char query[512];
	snprintf(query, sizeof(query), "SELECT * FROM books WHERE title LIKE '%%%s%%'", title);

	//Execute the SQL query and check for errors.
	if(mysql_query(conn, query)){
		fprintf(stderr, "SELECT failed: %s\n", mysql_error(conn));
		return;
	}

	MYSQL_RES *res = mysql_store_result(conn);
	if(res == NULL){
		fprintf(stderr, "mysql_store_result() failed: %s\n", mysql_error(conn));
		return;
	}

	MYSQL_ROW row;
	printf("\n%-5s %-30s %-30s %-5s\n", "ID", "Title", "Author", "Year");
	printf("******************************************************************\n");
	//Loop through and print each row in the results.
	while((row = mysql_fetch_row(res))){
		printf("%-5s %-30s %-30s %-5s\n", row[0], row[1], row[2], row[3]);
	}
	mysql_free_result(res); //Free up the result to prevent memory leaks.
}


int main(){
	MYSQL *conn;
	connect_to_db(&conn);

	int choice;
	do{
		printf("\nLibrary Database Management Sytem\n");
		printf("Enter: 1, 2, 3, 4\n");
		printf("1. Add a new book to the database\n");
		printf("2. View all books in the database\n");
		printf("3. Search for a book by title\n");
		printf("4. Exit\n");
		scanf("%d", &choice);

		//Handle the choice made by the user.
		switch(choice){
			case 1: add_book(conn); break;	 //Add a new book.
			case 2: view_books(conn); break; // View all books
			case 3: search_book(conn); break; // Search a book.
			case 4: printf("Exiting  \n"); break; //Exits the program.
			default: printf("Please select a valid choice. \n");
		}
	}while(choice != 4); // Loop back.

	mysql_close(conn);
	return 0;
}
