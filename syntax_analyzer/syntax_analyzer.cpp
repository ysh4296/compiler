#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stack>
#include <cstring>
#include <string>
#pragma warning(disable:4996) // for strtok error
using namespace std;
typedef pair<int, int> ii;
typedef pair<char*, int> ci;
struct token {// struct for token
	token(char* d , int t ,ii l) { // init the struct
		data = d;
		type = t;
		loc = l;
	}
	char* data;// data of token
	int type;// type of token for input of S_R_Table
	ii loc;// location of token
}; 
string S_R_table[87][41];// S_R table for shift-reduce operation
stack<int> local;// stack for save current state
vector<token> token_list;// out of lexical analizer and input of syntax analizer
vector<ci> trans_table;// table for translate tokens to push in token_list
ii Reduce_Rule[40];// reduce rule for reduce operation [reduced_type number]{ next state , # for erase at this reduce }
int cur_state = 1;// the start state is 0
int ind = 0; //index for current token position
void S_R_table_in() // Fill in the Shift_Reduce table from "S_R_table.txt" which is in same directory
{   
	ifstream is;
    is.open("S_R_Table.txt");
	string st;
	for (int r = 0; r < 87; r++) {
		for (int c = 0; c < 41; c++) {
			is >> st;
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
int Type_Trans(char* in) { // transform token_type_name to token number 
	for (int i = 0; i < (int)trans_table.size(); i++) { // search in trans_table
		if (strcmp(in, trans_table[i].first) == 0) {
			return trans_table[i].second;
		}
	}
	return;
}
void Reduce_Rule_in() // fill reduce rules for reduce
{
	ifstream is;
	is.open("Reduce_Rule.txt");
	int F,S;
	int cnt = 0;
	if (is.is_open()) {
		while (!is.eof()) {
			is >> F >> S;
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
	T = strtok(S, "|");
	while (T != NULL) {
		list.push_back(T);
		T = strtok(NULL, "|");
	}
	for (int i = 0 ; i < (int)list.size() ; i++) {
		int li = 0;
		while (list[i][li] == '\n') {
			li++;
			if (li == strlen(list[i]) - 1) return;
		}
		if (li != 0) { // next token is in next line
			line+= li;
			comp = 1;
			T = strtok(list[i], "~");
			T = strtok(NULL, ":");
			f = T;
			T = strtok(NULL, ":");
			s = T;
			token_list.push_back(token(s, Type_Trans(f), { line,comp })); // split and fill the token_list
			comp++;
		} else { // next token is in the same line
			T = strtok(list[i], "~");
			T = strtok(T, ":");
			f = T;
			T = strtok(NULL, ":");
			s = T;
			token_list.push_back(token(s, Type_Trans(f), {line,comp}));// split and fill the token_list
			comp++;
		}
	}
}
bool Reduce(token cur,string cmd) {// do reduce
	int next = stoi(cmd.substr(1)); // get reduce op num
	int erase = Reduce_Rule[next].second; // get erase num for pop item
	while (erase--) {// pop item
		local.pop();
	}
	int cu = local.top();// new stack top
	if (S_R_table[cu][Reduce_Rule[next].first][0] == 'x') return false;// if next_state is invalid
	next = stoi(S_R_table[cu][Reduce_Rule[next].first]); // next_state
	local.push(next); // push next_state
	return true;
}
void Shift(token cur,string cmd) {// do shift
	int next = stoi(cmd.substr(1));
	local.push(next); // push next
	ind++; // go next token
	return;
}
bool solve() { // do shift_reduce operation and get result
	while (1) { //  the acc is -1
		token cur = token_list[ind];
		string cmd = S_R_table[local.top()][cur.type]; // the command for shift reduce operation
		switch(cmd[0]) { // cmd[0] is determine next operation
			case 's': // do shift
				Shift(cur, cmd);
				break;
			case 'r': // do reduce
				if (!Reduce(cur, cmd)) return false; // reduce can fail
				break;
			case 'a': // the cmd is a means we accept tokens and syntax_analyze done
				return true;
			case 'x':
				return false; // default cmd means there's no operation next -> fail
		}
	}
}
int main(int argc, char* argv[]) {
	S_R_table_in(); //init S_R_Table
	Type_Trans_table_in(); // init trans table
	Reduce_Rule_in(); // init Reduce_table

	// get the out of lexical analyzer as input token
	ifstream readFile;
	readFile.open(argv[1]);
	string buf = string((std::istreambuf_iterator<char>(readFile)), std::istreambuf_iterator<char>());
	token_in(buf);// get token_list from 'output.txt file'
	char* end = new char[4]; // the last element '$'
	strcpy(end,"end");
	token_list.push_back(token(end, 21, { 0,0 })); // add ""
	// set token_list done
	local.push(0);
	if (solve()) // if solve is done with accept
		cout << "accept";
	else // if solve occur error
		cout << "syntax_error : there can't be token named ' " << token_list[ind].data << " ' in " << token_list[ind].loc.second <<"th token of line number" << token_list[ind].loc.first; // the error occur in latest token
	
	return 0;
}
