#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>

using namespace std;

typedef pair<int, int> ii;                              //New type definition for token table
typedef pair<string, string> ss;                        //New type definition for Output String

ii DFA_table[15][39][127];     // DFA table         //in(current_automata, current_state,input_character) -> out(next_automata,next_state)
string EndState_table[15][39]; // EndState table    //in(current_automata, current_state) -> out(token_type)


void DFA_table_in() //Fill in the DFA table from "table.txt" which is in same directory
{
    ifstream is;
    is.open("table.txt");
    int input[5];   //"table.txt" is consist of five integer ( current automata, current state, input character, next automata, next state)
    if (is.is_open()) {
        while (!is.eof()) {
            for (int i = 0; i < 5; i++)
                is >> input[i];
            DFA_table[input[0]][input[1]][input[2]] = { input[3],input[4] };  //fill the table with certain order
        }
    }
    is.close();
}

void EndState_table_in()  //Fill in the EndState table from "type.txt" which is in same directory
{
    ifstream is;
    is.open("type.txt");
    //"type.txt" is consist of two integer and one string ( current automata, current state, token type)
    string re;
    int input[2];
    if (is.is_open()) {
        while (!is.eof()) {
            for (int i = 0; i < 2; i++)
                is >> input[i];
            is >> re;
            EndState_table[input[0]][input[1]] = re;      //fill the type with certain order
        }
    }
    is.close();
}

int main(int argc, char* argv[])
{
#pragma region Initialization
    int auto_num = 1;   //current automata, default==1
    /*
    automata_num [0]: end
    automata_num [1]: start point
    automata_num [2]: single charactor
    automata_num [3]: string
    automata_num [4]: signed integer + operator’-’
    automata_num [5]: semicolon
    automata_num [6]: id + special statement + boolean + Vtype
    automata_num [7]: ()
    automata_num [8]: []
    automata_num [9]: {}
    automata_num [10]: arithmetic operator(except for ‘-’)
    automata_num [11]: assign &comparison operator
    automata_num [12]: comma
    automata_num [13]: white space
    */
    int cur_state = 0;  //current state, default==0
    char input;         //input character for current index
    vector<ss> answer;  //output string for the result of parsing
    string str = "";    //temp valuable for literal string

    //open input file from parameter of main function
    ifstream readFile;
    readFile.open(argv[1]);
    //save input strings to buffer
    string buf = string((std::istreambuf_iterator<char>(readFile)), std::istreambuf_iterator<char>());
    // get the table for parsing
    DFA_table_in();
    EndState_table_in();
#pragma endregion

    for (int i = 0; i < (int)buf.size(); i++)   //iterate by lenghth of string
    {
        input = buf[i];     //get current character from buffer
        ii next = DFA_table[auto_num][cur_state][(int)input]; //by searching DFA_table with current automata, current state, input character, find the next state
        if ((next.first == 0 && next.second == 0))              //if the result==(0,0), which means not defined in DFA_table( end state)
        {
            if (EndState_table[auto_num][cur_state] == "") {// if the pair of automata and state is not defined in EndState_table when the token ends, break
                str += buf[i];//make the wrong array
                break;
            }
            else    //if the pair of automata and state is defined in the EndState_table = normal exit
            {
                //condition checking for '-'operator
                if (auto_num == 4 && cur_state != 2)//if current automata is 4(single integer), and state is not 2(operator '-')
                {
                    if (str[0] == '-') {//if the token next to '-'(except blanks) is one of identifier,integer, or right paren, push the token as operator '-'
                        if (answer.size() != 0) {
                            if (strcmp(answer[(int)answer.size() - 1].first.c_str(), "Identifier") == 0 || strcmp(answer[(int)answer.size() - 1].first.c_str(), "Signed_integer") == 0 || strcmp(answer[(int)answer.size() - 1].first.c_str(), "RSparen") == 0) {
                                answer.push_back({ EndState_table[4][2],"-" });    //push as operator
                                str.erase(0, 1);                        //erase '-'
                            }
                        }
                    }
                }
                //if the token is not white space(automata number==13), push token to result string
                if (auto_num != 13) answer.push_back({ EndState_table[auto_num][cur_state],str });
                //initialize variables
                auto_num = 1;
                cur_state = 0;
                i--;
                str = "";
                continue;   //continue the loop
            }
        }
        //if the token is not in end state
        //move the pointer to next stage and concatenate the input character
        auto_num = next.first;
        cur_state = next.second;
        str += buf[i];
    }
    if (EndState_table[auto_num][cur_state] == "") //if the last token is not defined in EndState_table after end of parsing, print error
    {
        answer.clear();
        answer.push_back({ "error", "there can't be token like " + str });
    }
    else {//condition checking for '-'operator
        if (auto_num == 4 && cur_state != 2)//if current automata is 4(single integer), and state is not 2(operator '-')
        {
            if (str[0] == '-') {
                //if the token next to '-'(except blanks) is one of identifier,integer, or right paren, push the token as operator '-'
                if (strcmp(answer[(int)answer.size() - 1].first.c_str(), "Identifier") == 0 || strcmp(answer[(int)answer.size() - 1].first.c_str(), "Signed_integer") == 0 || strcmp(answer[(int)answer.size() - 1].first.c_str(), "RSparen") == 0) {
                    answer.push_back({ EndState_table[4][2],"-" });    //push as operator
                    str.erase(0, 1);                        //erase '-'
                }
            }
        }
        //if the token is not white space(automata number==13), push token to result string
        if (auto_num != 13) answer.push_back({ EndState_table[auto_num][cur_state],str });
    }
    ofstream writeFile;
    writeFile.open("output.txt");
    for (int i = 0; i < (int)answer.size(); i++) //after end of parsing, print all the result on console screen
    {
        //if the token name is error, print error
        if (strcmp(answer[i].first.c_str(),"error") == 0)
        {
            writeFile.write(answer[i].second.c_str(), (int)answer[i].second.size());
            break;
        }
        //format : <Token_Type,Token_Name>
        string line = "<" + answer[i].first + "," + answer[i].second + ">\n";
        writeFile.write(line.c_str(), (int)line.size());
    }
    return 0;
}
