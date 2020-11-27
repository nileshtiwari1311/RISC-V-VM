/*
 * RISC-V 3 address code to Stack based VM code: CS4100 Computer System Design RISC-V Project
 *
 *  Created on: November 26, 2020
 *     Authors: ABHISHEK KAUSHIK CS17B001 cs17b001@iitp.ac.in
                NILESH TIWARI CS17B022 cs17b022@iitp.ac.in
                VIKAS KUMAR CS17B030 cs17b030@iitp.ac.in
 */

#include<bits/stdc++.h>

using namespace std;

bool is_name_part(char c)
{
    if( isalpha(c) )
        return 1;
    if( isdigit(c) )
        return 1;
    if( c=='_' )
        return 1;
    if(c==':')
        return 1;
    return 0;
}

int main()
{
    string mytext;

    ofstream writefile("vm.txt"); 
    ifstream Myreadfile("tac1.txt");

    string push = "push ";
    string pop = "pop ";

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
        {">>","srl"},
        {"==","eq"},
        {"!=","neq"},
        {"<","lt"},
        {"<=","lte"},
        {">","gt"},
        {">=","gte"},
        {"&&","dand"},
        {"||","dor"} 
    };

    while( getline(Myreadfile,mytext) )
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
            else if(mytext[i]!=' ' && mytext[i]!=':' )
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

        if( names.size()==5 ) // x = y + z
        {
            writefile<< push<< names[2]<<endl;
            writefile<< push<< names[4]<<endl;
            writefile<<symbol_to_name[ names[3] ]<<endl;
            writefile<<pop<< names[0]<<endl;
        }
        else if( names.size()==3 ) // x = y
        {
            writefile<<push<< names[2]<<endl;
            writefile<<pop<< names[0]<<endl;
        }
        else if( names.size()==1 ) // _L1:
        {
            writefile<<names[0]<<endl;
        }
        else if( names.size()==2 ) // goto _L1
        {
            writefile<<names[0]<<" "<<names[1]<<endl;
        }
        else if(names.size()==4) // if _t1 goto _L1 
        {
            for(int i=0;i<names.size();i++)
            {
                writefile<<names[i]<<" ";
            }
            writefile<<endl;
        }
    }

    writefile.close();
    Myreadfile.close();
}