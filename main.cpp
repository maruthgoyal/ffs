#include <iostream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <utility>
#include <ncurses.h>

using namespace std;

/* Takes command to execute as input. Executes the command in the terminal and returns the output */
string exec(const char* cmd) {

    char buffer[128];
    string result = "";
    shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);

    if(!pipe) throw runtime_error("popen() failed!");

    while(!feof(pipe.get())) {
            if (fgets(buffer, 128, pipe.get()) != NULL)
                result += buffer;
    }
    return result;
}

/* Split a string with the char c as the denominator  */
vector<string> split(const char *str, char c = ' '){
    
    vector<string> result;

    do{
        const char *begin = str;

        while(*str != c && *str)
            str++;

        result.push_back(string(begin,str));
    } while (0 != *str++);

    return result;
}

int main(){

    string last_command;
    vector<string> tokens, results, list;
    int len;

    unordered_map<string, int> freqs; 
    vector< pair<int, string> > f;

    last_command = exec("bash -c \"sudo cat ~/.bash_history\" | awk -v n=2 '{x[NR%n]=$0}END{print x[(NR+1)%n]}'"); // Get last command executed


    last_command.erase(remove(last_command.begin(), last_command.end(), '|'), last_command.end()); // remove pipe symbol. We want to remove other symbols too, like ; > < etc. to create better tokens
    
    
    tokens = split(last_command.c_str()); // create the tokens
    int tokensSize = tokens.size();

    for(int i  = 0; i < tokensSize; i++){

        len = tokens[i].length()-1;

        if(len > 1){

            for(int j = 0; j < (len - 1); j++){
                
                list = split((exec(("bash -c \"compgen -ac | grep -o -w '\\w\\{" + to_string(len) + "\\}' | grep '" + tokens[i][j] + tokens[i][j+1] + "' | sort -u\"").c_str()).c_str()), '\n');

                // Get list of all commands. Take those of the same length as the command entered. Match every length-2 subsequence of the users input and filter out copies. 
                // Generates list of possible matches
               
                results.reserve(results.size() + distance(list.begin(), list.end()));
                results.insert(results.end(), list.begin(), list.end());
                
            }
        }

        for(string a:results){
            
            if(freqs.count(a) == 0){
                if(a!="")
                    freqs[a] = 1;
            }

            else{
                freqs[a] += 1;
            }
        
        }

        for(auto k:freqs){

            f.push_back(make_pair(k.second, k.first));
        
        }

        sort(f.begin(), f.end());
        reverse(f.begin(), f.end());



        /**********************************************
         * IT'S PRINTING TIME
         *
         ******************************************/

        int ch, optionNo;

        optionNo = 0;

        initscr();
        noecho();
        keypad(stdscr, TRUE);
        cbreak();

        while(ch != 10 && ch != 3){

            mvprintw(0,0,f[optionNo].second.c_str());
            refresh();
            ch = getch();

            if(ch == KEY_DOWN){
                if(optionNo < (f.size() -1))
                    optionNo++;
            }
            
            else if(ch == KEY_UP){
                if(optionNo > 0)
                    optionNo--;
            }

        }

        endwin();

        if(ch == 10)
            cout << exec(f[optionNo].second.c_str()) << endl;

        else if(ch == 3){
            cout << "BYE." << endl;
        }




    return 0;

}
}
