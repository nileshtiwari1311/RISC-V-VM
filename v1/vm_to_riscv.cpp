/*
 * RISC-V 3 address code to Stack based VM code: CS4100 Computer System Design RISC-V Project
 *
 *  Created on: November 9, 2020
 *     Authors: ABHISHEK KAUSHIK CS17B001
                NILESH TIWARI CS17B022
                VIKAS KUMAR CS17B030
 */

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

int main()
{

	ifstream vm;
	ofstream riscv;

	map<string, int> symtable;
	map<string, int>::iterator itr;

	string vm_line, riscv_line, temp1, temp2, prev_instr = "";
	int i, symtable_count = 0, addr;

	unordered_set<string> vm_keyword = 
	{
		"push",
		"pop"
	};

	unordered_set<string> instr = 
	{
		"add", 
		"sub", 
		"mul", 
		"div", 
		"rem", 
		"and", 
		"or", 
		"xor", 
		"sll", 
		"srl"
	};

	vm.open("vm.txt");
	riscv.open("riscv.txt");

	while(getline(vm, vm_line))
	{
		temp1 = "";
		temp2 = "";

		for(i=0; i<vm_line.size(); i++)
		{
			if(vm_line[i] == ' ')
				break;
			else
			{
				temp1 += vm_line[i];
			}
		}

		i++;
		if(vm_keyword.count(temp1) > 0)
		{
			while(i<vm_line.size())
			{
				temp2 += vm_line[i];
				i++;
			}

			if(symtable.count(temp2) == 0 && isalpha(temp2[0]))
			{
				symtable.insert(pair<string, int>(temp2, symtable_count));
				symtable_count += 4;
			}

			if(temp1 == "push" && isalpha(temp2[0]))
			{
				addr = symtable[temp2];
				riscv << "lw s3 " << addr << "(s1)" << endl; // base address of all variables in symbtable is stored in register s1
				riscv << "sw s3 0(s2)" << endl; // address of top of stack is stored in register s2
				riscv << "addi s2 s2 4" << endl;
				prev_instr = "push";
			}
			else if(temp1 == "push" && isdigit(temp2[0]))
			{
				riscv << "addi s3 zero " << temp2 << endl; // zero register always contains zero
				riscv << "sw s3 0(s2)" << endl;
				riscv << "addi s2 s2 4" << endl;
				prev_instr = "push";
			}
			else
			{
				if(prev_instr == "push")
				{
					riscv << "addi s2 s2 -4" << endl;
				}
				addr = symtable[temp2];
				riscv << "sw s3 " << addr << "(s1)" << endl;
				riscv << endl;
			}
		}
		else if(instr.count(temp1) > 0)
		{
			riscv << "addi s2 s2 -4" << endl;
			riscv << "lw s5 0(s2)" << endl;
			riscv << "addi s2 s2 -4" << endl;
			riscv << "lw s4 0(s2)" << endl;
			riscv << temp1 << " s3 s4 s5" << endl;
			prev_instr = temp1;
		}
	}

	cout << '\n' << "\t------------\n" << '\t' << "Symbol table\n" << '\t' << "------------" << endl;
	for(itr = symtable.begin(); itr != symtable.end(); itr++)
	{
		cout << '\t' << itr->first << '\t' << itr->second << endl;
	}

	vm.close();
	riscv.close();

	return 0;
}