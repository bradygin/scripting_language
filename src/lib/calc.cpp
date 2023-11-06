#include <algorithm>
#include <iostream>
#include <string>
#include <cctype>
#include <iterator>

using namespace std;

class Exp{
public:
//  Exp(){}
    virtual void print(){}
    virtual void release(){}
};
class Term: public Exp {
    string val;
public:
    Term(string v) : val(v){}
    void print(){
        cout << ' ' << val << ' ';
    }
    void release(){}
};

class Node: public Exp{
    Exp *l_exp;
    Exp *r_exp;
    char op;       // +, -, *, /
public:
    Node(char op, Exp* left, Exp* right) : op(op), l_exp(left), r_exp(right){}
    ~Node(){
    }
    void print(){
        cout << '(' << op << ' ';
        l_exp->print();
        r_exp->print();
        cout  << ')';
    }
    void release(){
        l_exp->release();
        r_exp->release();
        delete l_exp;
        delete r_exp;
    }
};

Exp* strToExp(string &str){
    int level = 0;  //inside parentheses check
    //case + or -
    //most right '+' or '-' (but not inside '()') search and split
    for (int i=str.size()-1; i>=0; --i){
        char c = str[i];
        if (c == ')') {
            ++level;
            continue;
        }
        if (c == '(') {
            --level;
            continue;
        }
        if (level>0) continue;
        if ((c == '+' || c == '-') && i!=0 ) {  //if i==0 then s[0] is sign
            string left(str.substr(0,i));
            string right(str.substr(i+1));
            return new Node(c, strToExp(left), strToExp(right));
        }
    }
    //case * or /
    //most right '*' or '/' (but not inside '()') search and split
    for (int i=str.size()-1; i>=0; --i) {
        char c = str[i];
        if (c == ')') {
            ++level;
            continue;
        }
        if (c == '(') {
            --level;
            continue;
        }
        if (level>0) continue;
        if (c == '*' || c == '/') {
            string left(str.substr(0,i));
            string right(str.substr(i+1));
            return new Node(c, strToExp(left), strToExp(right));
        }
    }
    if (str[0] == '(') {
    //case ()
    //pull out inside and to strToExp
        for (int i=0; i<str.size(); ++i){
            if (str[i]=='('){
                ++level;
                continue;
            }
            if (str[i]==')') {
                --level;
                if (level==0) {
                    string exp(str.substr(1, i-1));
                    return strToExp(exp);
                }
                continue;
            }
        }
    } else
    //case value
        return new Term(str);
cerr << "Error:never execute point" << endl;
    return NULL;//never
}

int main(){
    string exp(" ( a + b ) * c - ( d - e ) * f / g");
    //remove space character
    exp.erase(remove_if(exp.begin(), exp.end(), ::isspace), exp.end());
    Exp *tree = strToExp(exp);
    tree->print();
    tree->release();
    delete tree;
}
//output:(- (* (+  a  b ) c )(/ (* (-  d  e ) f ) g ))
