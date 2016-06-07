#include <iostream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <utility>

using namespace std;

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

    last_command = exec("bash -c \"sudo cat ~/.bash_history\" | awk -v n=2 '{x[NR%n]=$0}END{print x[(NR+1)%n]}'");


    last_command.erase(remove(last_command.begin(), last_command.end(), '|'), last_command.end());
    
    
    tokens = split(last_command.c_str());
    int tokensSize = tokens.size();

    for(int i  = 0; i < tokensSize; i++){

        len = tokens[i].length()-1;

        if(len > 1){

            for(int j = 0; j < (len - 1); j++){
                
                list = split((exec(("bash -c \"compgen -ac | grep -o -w '\\w\\{" + to_string(len) + "\\}' | grep '" + tokens[i][j] + tokens[i][j+1] + "' | sort -u\"").c_str()).c_str()), '\n');
               
                results.reserve(results.size() + distance(list.begin(), list.end()));
                results.insert(results.end(), list.begin(), list.end());
                
            }
        }

        for(string a:results){
            
            if(freqs.count(a) == 0 && a!=""){
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

        for(auto h:f) cout << h.second << endl;

    }




    return 0;

}

