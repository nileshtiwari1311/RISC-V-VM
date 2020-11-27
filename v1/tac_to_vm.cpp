#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <map>
#include <set> 

using namespace std;

bool is_name_part(char c)
{
    if( isalpha(c) )
        return 1;

    if( isdigit(c) )
        return 1;

    return 0;
}

bool notalpha_notspace(char c)
{
    if(c == ' ')
        return 0;

    if(isalpha(c))
        return 0;

    if(isdigit(c))
        return 0;

    return 1;
}

int main()
{
    string mytext;

    ofstream writefile("vm.txt"); 
    ifstream Myreadfile("tac.txt");

    string push="push "; 
    string pop="pop ";

    unordered_map<string,string> symbol_to_name = 
    { 
        {"-","sub"},
        {"+","add"}, 
        {"*","mul"}, 
        {"/","div"}, 
        {"%","rem"}, 
        {"&","and"}, 
        {"|","or"}, 
        {"^","xor"}, 
        {"<<","sll"}, 
        {">>","srl"} 
     };

    unordered_set<char> special_char = 
    {
        '+',
        '-',
        '*',
        '/',
        '%',
        '&',
        '|',
        '^',
        '<',
        '>'
    };

    while( getline(Myreadfile,mytext))
    {
        vector<string> names;

        for(int i=0;i<mytext.size();i++)
        {
            if( is_name_part(mytext[i]) )
            {
                if(i>0 && is_name_part( mytext[i-1] ) )
                {
                    names.back().push_back( mytext[i] );
                }
                else
                {
                    string temp;
                    temp.push_back( mytext[i] );
                    names.push_back(temp);
                }
            }
            else if(mytext[i]!=' ')
            {
                if(i>0 && is_name_part( mytext[i-1] )==0 && mytext[i-1]!=' ' )
                {
                    names.back().push_back( mytext[i] );   
                }
                else
                {
                    string temp;
                    temp.push_back( mytext[i] );
                    names.push_back( temp );
                }
                
            }
        }

        if( names.size()==5 )
        {
            writefile<< push<< names[2]<<endl;
            writefile<< push<< names[4]<<endl;
            writefile<<symbol_to_name[ names[3] ]<<endl;
            writefile<<pop<< names[0]<<endl;
        }
        else if( names.size()==3 )
        {
            writefile<<push<< names[2]<<endl;
            writefile<<pop<< names[0]<<endl;
        }
    }

    writefile.close();
    Myreadfile.close();
}