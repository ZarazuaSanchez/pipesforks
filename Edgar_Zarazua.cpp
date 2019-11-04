#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <sys/wait.h>
#include <list>
#include <stack>
using namespace std;

class GraphWithFun {

public:

    map<string, list<string>> adjlist;
    map<string, bool> visited;

    void setAdjlist(map<string, list<string>> al){

        adjlist = al;

        for(auto it=adjlist.begin();it!=adjlist.end();++it){
            visited[it->first] = false;
        }
    }

    void resetVisited(){

        for(auto it=adjlist.begin();it!=adjlist.end();++it){
            visited[it->first] = false;
        }
    }

    void recTopSort(string v, stack<string> &stk){

        visited[v] = true;

        for(auto it=adjlist[v].begin();it!=adjlist[v].end();++it){
            if(!visited[*it]){
                recTopSort(*it, stk);
            }
        }

        stk.push(v);
    }

    stack<string> topSort(){

        stack<string> stk;

        //init
        resetVisited();

        for(auto it=visited.begin();it!=visited.end();++it){
            if(visited[it->first]==false){
                recTopSort(it->first, stk);
            }
        }

        return stk;

        while(stk.empty()==false){
            cout << stk.top() << " ";
            stk.pop();
        }
    }
};

void evaluate(stack<string> &opstk, map<string, string> &varsandvers);
void mergeMap(map<string, string> &map1, map<string, string> &map2);
void printAdjList(map<string, list<string>> m);
void printList(list<string> l);;
void printStack(stack<string> stk);
void printMap(map<string, string> m);
bool strToBool(string tval);
string boolToStr(bool tval);
int findArrow(string ln);
string stripFromEnd(string s, char c);
string stripFromBeg(string s, char c);
map<string, string> getVarsAndVals(ifstream &myfile);
GraphWithFun makeGraph(ifstream &myfile);
map<string, string> getOps(ifstream &myfile);

int main(int argc, char const *argv[]) {

    string line;
    ifstream myfile("sample_input.txt");
    map<string, string> varsandvals;
    map<string, string> versandops;
    map<string, string> varsandvers;

    //get 1st line:
    string seg;
    myfile >> seg;

    while(!myfile.eof()){

        if(seg=="input_var"){

            varsandvals = getVarsAndVals(myfile);

        }
        else if(seg=="vertex"){
            versandops = getOps(myfile);

            mergeMap(varsandvers, varsandvals);
            mergeMap(varsandvers, versandops);

            GraphWithFun G;
            G = makeGraph(myfile);
            stack<string> opstk;
            opstk = G.topSort();

            evaluate(opstk, varsandvers);

        }

        myfile >> seg;
    }


    return 0;
}

void evaluate(stack<string> &opstk, map<string, string> &varsandvers){

    stack<string> eval;
    vector<string> logicops{"NOT", "AND", "OR", "IMPLY"};

    string elem = "empty";

    bool res;

    while(opstk.empty()==false){

        elem = opstk.top();

        opstk.pop();
        auto found = varsandvers.find(elem);

        if(found!=varsandvers.end()){
            eval.push(found->second);
            auto valorop = found->second;

            //if operation then eval
            if(find(begin(logicops), end(logicops), valorop)!=logicops.end()){

                bool val1;
                bool val2;

                if(valorop=="NOT"){

                    val1 = strToBool(eval.top());
                    eval.pop();

                    res = !val1;

                    // cout << "NOT " << boolToStr(val1) << " = " << boolToStr(res) << endl;

                }
                else if(valorop=="AND"){

                    val1 = strToBool(eval.top());
                    eval.pop();
                    val2 = strToBool(eval.top());
                    eval.pop();

                    res = (val1 && val2);

                    // cout << boolToStr(val1) << " AND " << boolToStr(val2) << " = " << boolToStr(res) << endl;

                }
                else if(valorop=="OR"){

                    val1 = strToBool(eval.top());
                    eval.pop();
                    val2 = strToBool(eval.top());
                    eval.pop();

                    res = (val1 || val2);

                    // cout << boolToStr(val1) << " OR " << boolToStr(val2) << " = " << boolToStr(res) << endl;

                }
                else if(valorop=="IMPLY"){

                    val1 = strToBool(eval.top());
                    eval.pop();
                    val2 = strToBool(eval.top());
                    eval.pop();

                    res = ( (!val1) || val2);

                    // cout << boolToStr(val1) << " IMPLIES " << boolToStr(val2) << " = " << boolToStr(res) << endl;
                }
                else{
                    perror("No such operation");
                }

                //pipe result to another process store in variable
                int pipefd[2];
                int pipeid = pipe(pipefd);
                int pid = fork();
                char pipedres[1];

                if(pipeid == -1){
                    perror("Pipe error");
                }


                if(pid < 0){
                    perror("Fork error.");
                }
                //child process
                else if( pid == 0){

                    close(pipefd[0]);

                    write(pipefd[1], boolToStr(res).c_str(), 1);
                    close(pipefd[1]);

                    exit(0);
                }
                //parent process

                close(pipefd[1]);

                read(pipefd[0], pipedres,1);
                close(pipefd[0]);

                varsandvers[elem] = string(pipedres);

            }
            //if T or F
            else{
                eval.push(valorop);
            }
        }

    }

    printMap(varsandvers);

}

void mergeMap(map<string, string> &map1, map<string, string> &map2){

    for(auto it=map2.begin();it!=map2.end();++it){
        map1.insert({it->first, it->second});
    }
}

void printAdjList(map<string, list<string>> m){

    for(auto it=m.begin();it!=m.end();++it){
        cout << it->first << ": ";
        printList(it->second);
    }
}

void printList(list<string> l){

    for(auto it=l.cbegin();it!=l.cend();++it){
        cout << *it << " ";
    }
}

void printStack(stack<string> stk){

    while(stk.empty()==false){
        cout << stk.top() << " ";
        stk.pop();
    }
}

void printMap(map<string, string> m){

    for(auto it=m.begin();it!=m.end();++it){
        cout << it->first << "=" << it->second;
        cout << endl;
    }

}

bool strToBool(string tval){

    if(tval=="T"){
        return true;
    }
    else if(tval=="F"){
        return false;
    }
    else{
        return false;
    }
}

string boolToStr(bool tval){

    if(tval==true){
        return "T";
    }
    else if(tval==false){
        return "F";
    }
}

int findArrow(string ln) {

    int myi = -1;

    for(int i=0; i < ln.size(); i++) {
        char c = ln.at(i);
        if( c == '-'){
            if( ln.at(i+1) == '>')
                return (myi = i);
        }
    }

    //use operation
    //and evaluation stacks

    return myi;
}

string stripFromEnd(string s, char c){

    int myi = s.length();

    for(int i=(s.size()-1);i>=0;i--){
        if(s.at(i) == c)
            myi--;
        else
            break;
    }

    if(myi <= s.length() - 1){
        s = s.substr(0, myi);
        return s;
    }
    else
        return s;

}

string stripFromBeg(string s, char c){

    int myi = -1;

    for(int i=0;i<s.size();i++){
        if(s.at(i) == c)
            myi++;
        else
            break;
    }

    if(myi > -1){
        s = s.substr(myi+1, s.size() - myi - 1);
        return s;
    }
    else{
        return s;
    }
}

map<string, string> getVarsAndVals(ifstream &myfile){

    string ln;
    map<string, string> varsandvals;
    vector<string> vars;
    vector<string> vals;

    getline(myfile, ln);

    ln = stripFromEnd(ln, ';');
    ln = stripFromEnd(ln, ' ');
    stringstream ss1(ln);
    string var;

    while(getline(ss1, var, ',')){
        var = stripFromBeg(var, ' ');
        var = stripFromEnd(var, ' ');
        vars.push_back(var);
    }

    getline(myfile, ln);

    ln = stripFromEnd(ln, ';'); //"T F T F"
    ln = stripFromBeg(ln, ' ');
    ln = stripFromEnd(ln, ' ');
    stringstream ss(ln);

    string tkn;

    int x=0;
    int pid;

    // fork();

    while( getline(ss, tkn, ' ') ){

        vals.push_back(tkn);

        x++;

    }

    if(vars.size() == vals.size() && vars.size()>0){

        for(int i=0;i<vars.size();i++){
            varsandvals.insert({vars[i], vals[i]});
        }
        return varsandvals;
    }
    else{
        exit(1);
    }

}

GraphWithFun makeGraph(ifstream &myfile){

    map<string, list<string>> adjlist;
    string line;
    string lhs;
    string rhs;

    getline(myfile, line);
    int arrowi = findArrow(line);

    while(arrowi != string::npos ){

        line = stripFromEnd(line, ';');
        lhs = line.substr(0, arrowi-1);
        lhs = stripFromBeg(lhs, ' ');
        lhs = stripFromEnd(lhs, ' ');
        rhs = line.substr(arrowi+2, line.size()-arrowi-2);
        rhs = stripFromBeg(rhs, ' ');
        rhs = stripFromEnd(rhs, ' ');
        adjlist[lhs].push_back(rhs);

        getline(myfile, line);
        arrowi = findArrow(line);
    }

    if( line.find('w') != string::npos){

    }

    GraphWithFun G;
    G.setAdjlist(adjlist);

    return G;
}

map<string, string> getOps(ifstream &myfile){

    string line;
    map<string, string> verandops;

    getline(myfile, line);
    line = stripFromEnd(line, ';');
    stringstream ss(line);
    string assign;
    string rhs;
    string lhs;
    int i;

    while(getline(ss, assign, ',')){
        i = assign.find('=');
        if(i != string::npos){
            lhs = assign.substr(0,i-1);
            lhs = stripFromBeg(lhs, ' ');
            lhs = stripFromEnd(lhs, ' ');
            rhs = assign.substr(i+1, assign.size()-i);
            rhs = stripFromBeg(rhs, ' ');
            rhs = stripFromEnd(rhs, ' ');
            verandops.insert({lhs, rhs});
        }
    }

    return verandops;
}
