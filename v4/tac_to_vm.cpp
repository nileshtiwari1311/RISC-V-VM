/*
 * RISC-V 3 address code to Stack based VM code: CS4100 Computer System Design RISC-V Project
 *
 *  Created on: November 30, 2020
 *     Authors: ABHISHEK KAUSHIK CS17B001 cs17b001@iitp.ac.in
                NILESH TIWARI CS17B022 cs17b022@iitp.ac.in
                VIKAS KUMAR CS17B030 cs17b030@iitp.ac.in
 */

#include<bits/stdc++.h>

using namespace std;

string english_function="function", english_arg="arg", english_local="local", english_temp="temp", english_constant="constant", english_return="return", english_call="call", english_goto="goto", english_push_param="pushParam", push="push ", pop="pop ";

class function_class 
{
    public:
    string label,name;
    int local_variable_count=0, temp_variables_count=0;
    unordered_map<string,int> arguments, temp_variables, local_variables;
};

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

void filter_line(vector<string> &names)
{
    if(names.size()==6)
    {
        if( names[2]=="-" || names[2]=="+" )
        {
            names[2]+=names[3];
            names[3]=names[4];
            names[4]=names[5];
            names.pop_back();
        }
        else if(names[4]=="-" || names[4]=="+" )
        {
            names[4]+=names[5];
            names.pop_back();
        }        
    }
    if(names.size()==7)
    {
        names[2]+=names[3];
        names[3]=names[4];
        names[4]=names[5]+names[6];
        names.pop_back();
        names.pop_back();
    }
    if(names.size()==4)
    {
        if(names[2]=="-" || names[2]=="+")
        {
            names[2]+=names[3];
            names.pop_back();
        }
    }  
}

bool does_variable_exist(function_class &current_function, string &current_string)
{
    if(current_function.arguments.count(current_string))
        return 1;
    if(current_function.local_variables.count(current_string))
        return 1;
    if(current_function.temp_variables.count(current_string))
        return 1;                
    return 0;
}

string find_correct_string(function_class &current_function, string &current_string, int def=1)
{
    string final_string, space=" ";
    if( current_function.arguments.count(current_string) )
    {
        final_string=english_arg+space;
        final_string+=to_string(current_function.arguments[current_string]);
    }
    else if( current_function.local_variables.count(current_string) )
    {
        final_string =english_local+space;
        final_string=final_string+to_string(current_function.local_variables[current_string]);
    }      
    else if( current_function.temp_variables.count(current_string) )
    {
        final_string=english_temp+space;
        final_string+=to_string(current_function.temp_variables[current_string]);
    }                          
    else 
    {
        final_string=english_constant+space;
        final_string+=current_string;
    }
    if(def)
        final_string+="\n";
    return final_string;
}

int main()
{
    string mytext;

    ofstream writefile("vm.txt"); 
    ifstream Myreadfile("tac2.txt");

    unordered_map<string,string> symbol_to_name=
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


    unordered_map<string,string> function_name_to_label;

    string function_label;

    vector<function_class> functions;


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
            else if(mytext[i]!=' ' && mytext[i]!=':' )
            {
                if(i>0 && is_name_part( mytext[i-1] )==0 && mytext[i-1]!=' ' && mytext[i]!='-' && mytext[i]!='+' )
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

        filter_line(names); 
        if(names.size()==0)
        {
            continue;
        }

        else if(names.size()==1)
        {
            if(names[0][names[0].size()-1]==':')
                function_label=names[0];
            continue;
        }    

        else if(names.size()==2 || names.size()==4)
        {
            continue;
        }

        else if(names[0]==english_function)
        {
            function_name_to_label[names[1]]=function_label;
            function_class x;
            functions.push_back(x);
            functions.back().label=function_label;
            functions.back().name=names[1];
            functions.back().local_variable_count=stoi(names[2]);

            for(int i=3;i<names.size();i++)
            {
                functions.back().arguments[names[i]]=i-3;
            }
            continue;
        }
        else if(names.size()==5 && names[2]==english_call)
        {
            continue;
        }

        for(int i=0;i<names.size();i++)
        {
            if( does_variable_exist( functions.back(), names[i] ) )
            {
                continue;
            }

            if(names[i][0]=='_'&& names[i].back()!=':')
            {
                functions.back().temp_variables[names[i]]=functions.back().temp_variables.size();
            }

            else if( functions.back().arguments.count(names[i])==0 && isalpha(names[i][0]) )
            {
                functions.back().local_variables[names[i]]=functions.back().local_variables.size();
            }
        }
    }
    for(int i=0;i<functions.size();i++)
    {
        functions[i].local_variable_count=functions[i].local_variables.size();
        functions[i].temp_variables_count=functions[i].temp_variables.size();
    }
    Myreadfile.clear();
    Myreadfile.seekg(0, Myreadfile.beg);

    //cout<<"opening file again"<<endl;
    string main_name=function_name_to_label["main"];
    main_name.pop_back();
    writefile<<"goto "<<main_name<<endl;

    int current_function_number=-1;
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
            else if(mytext[i]!=' ' && mytext[i]!=':' )
            {
                if(i>0 && is_name_part( mytext[i-1] )==0 && mytext[i-1]!=' ' && mytext[i]!='-' && mytext[i]!='+' )
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

        if(names.size()==0)
        {
            continue;
        }

        //cout<<"before filter"<<endl;
        filter_line(names);
        //cout<<"after filter"<<endl;


        if(names[0]==english_function)
        {
            current_function_number++;
        }

        if(names.size()==1)
        {
            writefile<<names[0]<<endl;
        }

        else if(names[0]==english_function)
        {
            writefile<<english_function<<" "<<functions[current_function_number].name<<" "<<functions[current_function_number].local_variable_count<<" "<<functions[current_function_number].temp_variables_count<<endl;
        }

        else if(names.size()==2)
        {
            if(names[0]==english_goto)
            {
                writefile<<names[0]<<" ";
                writefile<<names[1]<<endl;
            }

            else if(names[0]==english_return)
            {
                writefile<<push<<find_correct_string(functions[current_function_number], names[1]);
                writefile<<english_return<<endl;
            }

            else if(names[0]==english_push_param)
            {
                writefile<<english_push_param<<" "<<find_correct_string(functions[current_function_number], names[1]);
            }
        }
        
        else if(names.size()==3)
        {
            writefile<<push<< find_correct_string(functions[current_function_number], names[2]);
            writefile<<pop<<  find_correct_string(functions[current_function_number], names[0]) ;
        }
        else if( names.size()==5 )
        {
            if(names[2]!=english_call)
            {
                //cout<<"calling is"<<names[2]<<endl;
                writefile<< push<< find_correct_string(functions[current_function_number], names[2]);
                writefile<< push<< find_correct_string(functions[current_function_number], names[4]);
                writefile<<symbol_to_name[ names[3] ]<<endl;
                writefile<<pop<< find_correct_string(functions[current_function_number], names[0]);
            }

            else
            {
                string label_for_it=function_name_to_label[names[3]];
                label_for_it.pop_back();
                //cout<<"names[3]="<<names[3]<<endl;
                writefile<<english_call<<" "<<label_for_it<<" "<<names[4]<<endl;
                writefile<<pop<<find_correct_string(functions[current_function_number], names[0]);
            }
        }
        else if(names.size()==4)
        {
            for(int i=0;i<names.size();i++)
            {   
                if(i==1)
                    writefile<<find_correct_string(functions[current_function_number], names[1],0)<<" ";
                else
                    writefile<<names[i]<<" ";
            }
            writefile<<endl;
        }
    }
    
    writefile.close();
    Myreadfile.close();
}