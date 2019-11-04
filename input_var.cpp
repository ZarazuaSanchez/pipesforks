#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <iostream>
#include <unistd.h>
using namespace std;

int main2();
int findArrow(string);
string stripFromEnd(string, char);
string stripFromBeg(string, char);

class Node;

vector<string> getVars(string ln){

    vector<string> vars;

    ln = stripFromEnd(ln, ';');
    ln = stripFromEnd(ln, ' ');
    stringstream ss1(ln);
    string var;

    while(getline(ss1, var, ',')){
        var = stripFromBeg(var, ' ');
        var = stripFromEnd(var, ' ');
        vars.push_back(var);
    }

    return vars;
}

vector<bool> getTruthVals(string ln, int** fid){

    cout << "Line=" << ln << endl;

    vector<bool> values;

    ln = stripFromEnd(ln, ';'); //"T F T F"
    ln = stripFromBeg(ln, ' ');
    ln = stripFromEnd(ln, ' ');
    stringstream ss(ln);

    string tkn;

    int x=0;
    int pid;

    // fork();

    while( getline(ss, tkn, ' ') ){

        cout << "tkn=" << tkn << " & pid=" << pid << endl;

        if( tkn == "T"){
          values.push_back(true);
        }
        else if( tkn == "F"){
          values.push_back(false);
        }

        // cout << "fid[" << x << "]=" << "{" << fid[x][0] << ", " << fid[x][1] << "}" << endl;

        // cout << (pipe(fid[x]));
        // close(fid[x][0]);
        // write(fid[x][1], tkn.c_str(), 1);
        // close(fid[x][1]);

        x++;

    }

    return values;

}

map<string, string> getVertices(string ln){

    map<string, string> vtcs;
    string stmt;
    int i;

    ln = stripFromEnd(ln, ';');
    ln = stripFromBeg(ln, ' ');
    ln = stripFromEnd(ln, ' ');

    stringstream ss(ln);

    while(getline(ss, stmt, ',')){
        i = stmt.find('=');
        string rhs;
        string lhs;

        if (i != string::npos){
            lhs = stmt.substr(0, i);
            lhs = stripFromBeg(lhs, ' ');
            lhs = stripFromEnd(lhs, ' ');
            rhs = stmt.substr(i+1, ln.size()-i);
            rhs = stripFromBeg(rhs, ' ');
            rhs = stripFromEnd(rhs, ' ');
            vtcs.insert({lhs, rhs});
            cout << rhs << "=" << lhs << "\\0" << endl;
        }
    }

}

map<string, string> makeGraph(ifstream &myfile){

    string ln;
    string vertex;
    string rhs;
    map<string, string> edges;

    while(getline(myfile, ln)){

        ln = stripFromEnd(ln, ';');
        int arrowI = findArrow(ln);
        if(arrowI > -1){
            rhs = ln.substr(0, arrowI);
            vertex = ln.substr(arrowI+2, ln.size()-arrowI-2);

            rhs = stripFromBeg(rhs, ' ');
            rhs = stripFromEnd(rhs, ' ');
            cout << "var=" << rhs << "\\n" << endl;

            vertex = stripFromBeg(vertex, ' ');
            vertex = stripFromEnd(vertex, ' ');
            edges.insert({rhs, vertex});
            cout << "vertex=" << vertex << "\\n" << endl;
        }
    }
}

int main(int argc, char* argv[]) {

    string line;
    ifstream myfile("sample_input.txt");
    vector<string> vars;
    vector<bool> values;


    //get 1st line:
    string seg;
    myfile >> seg;

    while(!myfile.eof()){

        if(seg=="input_var"){

            cout << "seg=" << seg << endl;
            getline(myfile, seg);
            vars = getVars(seg);
            cout << "vars.size()=" << vars.size() << endl;

            int** fid = new int* [vars.size()];

            for(int i=0;i<vars.size();i++){
                fid[i] = new int[2];
            }

            getline(myfile, line);
            values = getTruthVals(line, fid);

            if(values.size() != vars.size())
                exit(1);

        }
        else if(seg=="vertex"){

            map<string, string> vtxs;
            map<string, string> edges;

            getline(myfile, seg); //"vertex v0 = OR, v1 = NOT, v2 = AND, v3 = IMPLY;"
            vtxs = getVertices(seg);
            edges = makeGraph(myfile); //parse lines i.e. a -> v0

        }

        myfile >> seg;
    }

    cout << "++++++++++++++++++++++++++++++++++++++++++++++" << endl;

    //get 3rd line
    //"vertex v0 = OR, v1 = NOT, v2 = AND, v3 = IMPLY;"
    //make the graph
    //line is regular expression var -> vertex
    //connect graph

    return 0;

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


class Node {

    Node* to;
    Node* from;
    string pay;
};
