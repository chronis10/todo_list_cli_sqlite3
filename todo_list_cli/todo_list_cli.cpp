#include <iostream>
#include <winsqlite/winsqlite3.h>
#include <fmt/format.h>
#include <windows.h>
#include <wincon.h>
using namespace std;

HANDLE color = GetStdHandle(STD_OUTPUT_HANDLE); //just once


class Db_manager {
private:
	char* zErrMsg = 0;	
	sqlite3* maindb;
	int resp;	
	bool error_handler(int resp) {
		if (resp!= SQLITE_OK) {
			return false;
		}
		else {
			return true;
		}
	}

	static int callback(void* data, int argc, char** argv, char** azColName) {
		int i;
		fprintf(stderr, "%s: ", (const char*)data);
		for (i = 0; i < argc; i++) {
			printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
		}
		printf("\n");
		return 0;
	}

public:	
	const char* data = "Callback function called";
	Db_manager() {
		resp = sqlite3_open("todo_db.db", &maindb);
		if (resp != SQLITE_OK) {
			cout << "error: " << sqlite3_errmsg(maindb) << endl;
		}
	}

	int drop_todo() {	
		const char* sql;
		sql = "DROP TABLE TODOLIST;";
		resp = sqlite3_exec(maindb, sql, callback, 0, &zErrMsg);
		return error_handler(resp);
	}

	int create_table() {
		const char* sql;
		sql = "CREATE TABLE IF NOT EXISTS TODOLIST("  \
				"ID INTEGER PRIMARY KEY AUTOINCREMENT," \
				"TITLE          TEXT    NOT NULL," \
				"MESSAGE        TEXT," \
				"STATUS         INT(1) );";
		resp = sqlite3_exec(maindb, sql, callback, 0, &zErrMsg);		
		return error_handler(resp);		
	}

	int insert_data(string title, string message) {
		const char* sql;
		string dyn_string = "INSERT INTO TODOLIST (TITLE,MESSAGE,STATUS) "  \
			"VALUES ('{0}', '{1}', 0); ";		
		dyn_string = fmt::format(dyn_string, title, message);
		sql = dyn_string.c_str();
		resp = sqlite3_exec(maindb, sql, callback, 0, &zErrMsg);		
		return error_handler(resp);
	}

	int update_data(int id) {
		const char* sql;	
		string dyn_string = "UPDATE TODOLIST SET STATUS=1 WHERE ID = {0} ; ";
		dyn_string = fmt::format(dyn_string,id);
		sql = dyn_string.c_str();
		resp = sqlite3_exec(maindb, sql, callback, 0, &zErrMsg);
		return error_handler(resp);
	}

	int delete_data(int id) {
		const char* sql;
		string dyn_string = "DELETE FROM TODOLIST WHERE ID = {0} ; ";
		dyn_string = fmt::format(dyn_string, id);
		sql = dyn_string.c_str();
		resp = sqlite3_exec(maindb, sql, callback, 0, &zErrMsg);
		return error_handler(resp);
	}

	int load_list() {
		sqlite3_stmt* stmt;
		const char* sql = "SELECT * from TODOLIST";
		int rc = sqlite3_prepare_v2(maindb, sql, -1, &stmt, NULL);
		if (rc != SQLITE_OK) {
			cout << "error: " << sqlite3_errmsg(maindb) <<endl;
			return 0;
		}
		SetConsoleTextAttribute(color, 2);
		cout << "####################################################" << endl;
		SetConsoleTextAttribute(color, 15);
		cout << "	           Task List              " << endl;
		SetConsoleTextAttribute(color, 2);
		cout << "####################################################" << endl;
		SetConsoleTextAttribute(color, 15);		
		while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
			cout << endl;
			int id = sqlite3_column_int(stmt, 0);
			const unsigned char* title = sqlite3_column_text(stmt, 1);
			const unsigned char* message = sqlite3_column_text(stmt, 2);
			bool status = sqlite3_column_int(stmt, 3);
			cout << "  [";
			if (status) {
				SetConsoleTextAttribute(color, 12);
				cout << "X";
				SetConsoleTextAttribute(color, 15);
			}
			else {
				cout << " ";
			}
			cout << "] ";
			int titles_color = 14;
			SetConsoleTextAttribute(color, titles_color);
			cout << "   ID: ";
			SetConsoleTextAttribute(color, 15);
			cout << id << endl;
			SetConsoleTextAttribute(color, titles_color);
			cout << "      Title: ";
			SetConsoleTextAttribute(color, 15);
			cout << title << endl;
			SetConsoleTextAttribute(color, titles_color);
			cout << "    Message: ";
			SetConsoleTextAttribute(color, 15);
			//40
			size_t size = strlen((char*)message);
			if (size <= 40) {
				cout << message << endl;
			}
			else {
				int count1 = 0;
				for (int i = 0; i < size; i++) {
					if (count1 > 38) {
						count1 = 0;
						cout << endl;
						cout << "             ";
					}
					cout << message[i];
					count1++;
				}
				cout << endl;

			}
			
			cout << "____________________________________________________" << endl;
		}
		if (rc == SQLITE_ROW) {
			printf("%s\n", sqlite3_column_text(stmt, 0));
		}
		sqlite3_finalize(stmt);
		return 1;
	}

	void close() {
		sqlite3_close(maindb);
	}
	


};

int menu() {
	int input_menu;
	/*  1_____Blue          10______Light Green
		2_____Green         11______Light Aqua
		3_____Aqua          12______Light Red
		4_____Red           13______Light Purple
		5_____Purple        14______Light Yellow
		6_____Yellow        15______Bright White
		7_____White
		8_____Gray
		9_____Light Blue
		0_____Black
	*/

	
	SetConsoleTextAttribute(color, 2);
	cout << "####################################################" << endl;

	SetConsoleTextAttribute(color, 15);
	cout << "	              TODO List              " <<endl;
	SetConsoleTextAttribute(color, 2);
	cout << "####################################################" << endl;
	SetConsoleTextAttribute(color, 15);
	cout << "               1. New Task" << endl;
	cout << "               2. Show Tasks" << endl;
	cout << "               3. Task Done" << endl;
	cout << "               4. Delete Task" << endl;
	cout << "               5. Exit" << endl;
	SetConsoleTextAttribute(color, 2);
	cout << "####################################################" << endl;
	SetConsoleTextAttribute(color, 15);
	cin >> input_menu;
	return input_menu;
}

struct message_body
{
	string title;
	string message;
};

message_body read_input() {

	message_body msg_bd;
	cin.ignore();
	SetConsoleTextAttribute(color, 9);
	cout << "Tittle of the Task: ";
	SetConsoleTextAttribute(color,15);
	getline(cin,msg_bd.title);
	//cout << endl;
	SetConsoleTextAttribute(color, 9);
	cout << "Message of the Task: ";
	SetConsoleTextAttribute(color, 15);
	getline(cin, msg_bd.message);
	//cout << endl;
	return msg_bd;
}

int main(int argc, char** argv)
{

	SetConsoleTextAttribute(color, 15);
	int menu_inp;
	Db_manager todo;
	//todo.drop_todo();
	todo.create_table();	
	if (argc >1) {
		if (string(argv[1]) == "tasks") {
			system("cls");
			todo.load_list();			
		}
	}
	else {	
		while (true) {
			system("cls");
			menu_inp = menu();
			if (menu_inp == 1) {
				system("cls");
				message_body temp = read_input();
				todo.insert_data(temp.title, temp.message);
			}
			else if (menu_inp == 2) {
				system("cls");
				todo.load_list();
				system("pause");
			}
			else if (menu_inp == 3) {
				system("cls");
				int id;
				int resp;
				SetConsoleTextAttribute(color, 2);
				cout << "####################################################" << endl;
				SetConsoleTextAttribute(color, 15);
				cout << "	           Upadate Task    " << endl;
				SetConsoleTextAttribute(color, 2);
				cout << "####################################################" << endl;
				SetConsoleTextAttribute(color, 12);
				cout << "*Insert -1 to cancel. " << endl;;
				SetConsoleTextAttribute(color, 14);
				cout << "Insert the ID of the Task: ";
				SetConsoleTextAttribute(color, 15);
				
				cin >> id;
				if(id != -1){
					resp = todo.update_data(id);
					if (resp == 1) {
						cout << "Succes!!" << endl;
					}
					else {
						cout << "Wrong ID!!" << endl;
					}
					system("pause");
				}
			}	
			else if (menu_inp == 4) {
				system("cls");
				int id;
				int resp;
				SetConsoleTextAttribute(color, 2);
				cout << "####################################################" << endl;
				SetConsoleTextAttribute(color, 15);
				cout << "	           Delete Task    " << endl;
				SetConsoleTextAttribute(color, 2);
				cout << "####################################################" << endl;
				SetConsoleTextAttribute(color, 12);
				cout << "*Insert -1 to cancel. " << endl;;
				SetConsoleTextAttribute(color, 14);
				cout << "Insert the ID of the Task: ";
				SetConsoleTextAttribute(color, 15);
				cin >> id;
				if (id != -1) {
					resp = todo.delete_data(id);
					if (resp == 1) {
						cout << "Succes!!" << endl;
					}
					else {
						cout << "Wrong ID!!" << endl;
					}
					system("pause");
				}
			}
			else if (menu_inp == 5) {
				break;
			}
		}
	}
	todo.close();
	

}
