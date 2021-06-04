#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stack>
#include <cstring>
#include <string>
#pragma warning(disable:4996)
using namespace std;
typedef pair<int, int> ii;
typedef pair<char*, int> ci;
struct token {// struct for token
	token(char* d , int t ,ii l) {
		data = d;
		type = t;
		loc = l;
	}
	char* data;// data of token
	int type;// type of token for input of S_R_Table
	ii loc;// location of token
}; 
string S_R_table[86][40];// S_R table for shift-reduce operation
stack<int> st;// stack for save current state
vector<token> token_list;// out of lexical analizer and input of syntax analizer
vector<ci> trans_table;// table for translate tokens to push in token_list
ii Reduce_Rule[38];// reduce rule for reduce operation [reduced_type number]  # for erase at this reduce }
int cur_state = 1;// the start state is 0
int index = 0; //index for current token position
void S_R_table_in() // Fill in the Shift_Reduce table from "S_R_table.txt" which is in same directory
{   
	ifstream is;
    is.open("S_R_Table.txt");
	string st;
	int r, c;
	if (is.is_open()) {
		while (!is.eof()) {
			is >> r >> c >>st;
			S_R_table[r][c] = st;// fill the table
		}
	}
	is.close();
}
void Type_Trans_table_in() // fill trans table for use type in type_trans_table
{
	ifstream is;
	is.open("Trans_Table.txt");
	string st;	// first component of trans_table
	int input;  // second component of trans_table 
	if (is.is_open()) {
		while (!is.eof()) {
			is >> st;
			char* ch_s = new char[st.size()];
			is >> input;
			strcpy(ch_s, st.c_str());
			trans_table.push_back({ ch_s , input });// fill the table
		}
	}
	is.close();
}
int Type_Trans(char* in) {
	for (int i = 0; i < (int)trans_table.size(); i++) {
		if (strcmp(in, trans_table[i].first) == 0) {
			return trans_table[i].second;
		}
	}
}
void Reduce_Rule_in() // fill reduce rules for reduce
{
	ifstream is;
	is.open("Reduce_Rule.txt");
	int F,S;
	int cnt = 0;
	if (is.is_open()) {
		while (!is.eof()) {
			is >> F;
			is >> S;
			Reduce_Rule[cnt] = { F , S };// fill the table
			cnt++;
		}
	}
	is.close();
}
void token_in(string buf) {// get the input file in token vector to parse
	char* S = (char*)malloc(buf.size()*sizeof(char));
	strcpy(S,buf.c_str());
	char* T;
	char* f;
	char* s;
	int line = 0;// line no
	int comp = 1;// component no
	vector<char*> list;
	T = strtok(S, ">");
	while (T != NULL) {
		cout << T << endl;
		list.push_back(T);
		T = strtok(NULL, ">");
	}
	for (int i = 0 ; i < (int)list.size() ; i++) {
		cout << "list   " << list[i] << endl;
		if (list[i][0] == '\n') { // next token is in next line
			cout << "n" << endl;
			line++;
			comp = 1;
			T = strtok(list[i], "<");
			T = strtok(NULL, ",");
			cout << "token : " << T << endl;
			f = T;
			T = strtok(NULL, ",");
			s = T;
			cout << s << " "<< f << endl;
			token_list.push_back(token(s, Type_Trans(f), { line,comp }));
			comp++;
		} else { // next token is in the same line
			cout << "0" << endl;
			T = strtok(list[i], "<");
			T = strtok(T, ",");
			f = T;
			T = strtok(NULL, ",");
			s = T;
			cout << s << " " << f << endl << endl;
			token_list.push_back(token(s, Type_Trans(f), {line,comp}));
			comp++;
		}
	}
}
bool Reduce(token cur,string cmd) {
	int next = stoi(cmd.substr(1));
	int erase = Reduce_Rule[next].second;
	while (erase--) {
		st.pop();
	}
	int cu = st.top();
	next = stoi(S_R_table[cu][next]);
	if (next == 0) return false;
	st.push(next);
	return true;
}
bool Shift(token cur,string cmd) {
	int next = stoi(cmd.substr(1));
	if (next == 0) return false;
	st.push(next);
	index++;
	return true;
}
bool solve() {
	st.push(1);
	while (st.top() != -1) { //  the acc is -1
		token cur = token_list[index];
		string cmd = S_R_table[st.top()][cur.type];
		switch(cmd[0]) {
			case 's':
				if (Shift(cur, cmd)) return false;
				break;
			case 'r':
				if (Reduce(cur, cmd)) return false;
				break;
			default:
				return false;
				break;
		}
	}
	return true;
}
int main(int argc, char* argv[]) {
	cout << "hello " << endl;
	S_R_table_in(); //init S_R_Table
	Type_Trans_table_in(); // init trans table
	//Reduce_Rule_in(); // init Reduce_table
	// get the out of lexical analyzer as input token
	ifstream readFile;
	readFile.open(argv[1]);
	string buf = string((std::istreambuf_iterator<char>(readFile)), std::istreambuf_iterator<char>());
	token_in(buf);
	char* end = new char[4]; 
	strcpy(end,"end");
	token_list.push_back(token(end, 21, { 0,0 })); // add ""
	if (solve())
		cout << "accept";
	else
		cout << "syntax_error : there can't be token named ' " << token_list[index].data << " ' in " << token_list[index].loc.first <<"th token of line number" << token_list[index].loc.second;
	return 0;
}
