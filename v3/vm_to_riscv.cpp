/*
 * RISC-V Stack based VM code to RISC-V assembly language code: CS4100 Computer System Design RISC-V Project
 *
 *  Created on: November 26, 2020
 *     Authors: ABHISHEK KAUSHIK CS17B001 cs17b001@iitp.ac.in
                NILESH TIWARI CS17B022 cs17b022@iitp.ac.in
                VIKAS KUMAR CS17B030 cs17b030@iitp.ac.in
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

bool isVariable(char c)
{
	if (isalpha(c) || c =='_')
		return true;
	return false;
}

int main()
{

	ifstream vm;
	ofstream riscv;

	map<string, int> symtable;
	map<string, int>::iterator itr;

	string vm_line, riscv_line, temp1, temp2, temp3, temp4, prev_instr = "";
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

	unordered_set<string> cond_instr_1 = 
	{
		"eq",
		"neq",
		"lt",
		"gt",
	};

	unordered_set<string> cond_instr_2 = 
	{
		"lte",
		"gte"
	};

	unordered_set<string> cond_instr_3 =
	{
		"dand",
		"dor"
	};

	vm.open("vm.txt");
	riscv.open("riscv.txt");

	while(getline(vm, vm_line))
	{
		temp1 = "";
		temp2 = "";
		temp3 = "";
		temp4 = "";

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
		if(vm_keyword.count(temp1) > 0) // push/pop
		{
			while(i<vm_line.size())
			{
				temp2 += vm_line[i];
				i++;
			}

			if(symtable.count(temp2) == 0 && isVariable(temp2[0]))
			{
				symtable.insert(pair<string, int>(temp2, symtable_count));
				symtable_count += 4;
			}

			if(temp1 == "push" && isVariable(temp2[0])) // push variable
			{
				addr = symtable[temp2];
				riscv << "lw s3 " << addr << "(s1)" << endl; // base address of all variables in symbtable is stored in register s1
				riscv << "sw s3 0(s2)" << endl; // address of top of stack is stored in register s2
				riscv << "addi s2 s2 4" << endl;
				prev_instr = "push";
			}
			else if(temp1 == "push" && isdigit(temp2[0])) // push constant immediate
			{
				riscv << "addi s3 zero " << temp2 << endl; // zero register always contains zero
				riscv << "sw s3 0(s2)" << endl;
				riscv << "addi s2 s2 4" << endl;
				prev_instr = "push";
			}
			else // pop variable
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
		else if(instr.count(temp1) > 0) // instruction like add, sub, etc.
		{
			riscv << "addi s2 s2 -4" << endl;
			riscv << "lw s5 0(s2)" << endl;
			riscv << "addi s2 s2 -4" << endl;
			riscv << "lw s4 0(s2)" << endl;
			riscv << temp1 << " s3 s4 s5" << endl;
			prev_instr = temp1;
		}
		else if(cond_instr_1.count(temp1) > 0) // instructions like eq, neq, lt, gt
		{
			riscv << "addi s2 s2 -4" << endl;
			riscv << "lw s5 0(s2)" << endl;
			riscv << "addi s2 s2 -4" << endl;
			riscv << "lw s4 0(s2)" << endl;
			riscv << "sub s3 s4 s5" << endl;

			if(temp1 == "eq")
				riscv << "sltiu s3 s3 1" << endl;
			else if(temp1 == "neq")
				riscv << "sltu s3 zero s3" << endl;
			else if(temp1 == "lt")
				riscv << "slt s3 s3 zero" << endl;
			else
				riscv << "slt s3 zero s3" << endl;

			prev_instr = temp1;
		}
		else if(cond_instr_2.count(temp1) > 0) // instructions like lte, gte
		{
			riscv << "addi s2 s2 -4" << endl;
			riscv << "lw s5 0(s2)" << endl;
			riscv << "addi s2 s2 -4" << endl;
			riscv << "lw s4 0(s2)" << endl;
			riscv << "sub s3 s4 s5" << endl;

			if(temp1 == "lte")
				riscv << "slt s4 s3 zero" << endl;
			else
				riscv << "slt s4 zero s3" << endl;

			riscv << "sltiu s5 s3 1" << endl;
			riscv << "or s3 s4 s5" << endl;
			prev_instr = temp1;
		}
		else if(cond_instr_3.count(temp1) > 0) // instructions like dand, dor
		{
			riscv << "addi s2 s2 -4" << endl;
			riscv << "lw s5 0(s2)" << endl;
			riscv << "addi s2 s2 -4" << endl;
			riscv << "lw s4 0(s2)" << endl;
			riscv << "sltu s4 zero s4" << endl;
			riscv << "sltu s5 zero s5" << endl;
			
			if(temp1 == "dand")
				riscv << "and s3 s4 s5" << endl;
			else
				riscv << "or s3 s4 s5" << endl;

			prev_instr = temp1;
		}
		else if(temp1 == "goto") // goto _L1
		{
			while(i<vm_line.size())
			{
				temp2 += vm_line[i];
				i++;
			}

			riscv << "jal zero " << temp2 << endl;
			riscv << endl;
		}
		else if(temp1 == "If") // If _t1 goto _L1
		{
			while(i<vm_line.size())
			{
				if(vm_line[i] == ' ')
					break;
				temp2 += vm_line[i];
				i++;
			}

			i++;

			while(i<vm_line.size())
			{
				if(vm_line[i] == ' ')
					break;
				temp3 += vm_line[i];
				i++;
			}

			i++;

			while(i<vm_line.size())
			{
				temp4 += vm_line[i];
				i++;
			}

			if(symtable.count(temp2) == 0 && isVariable(temp2[0]))
			{
				symtable.insert(pair<string, int>(temp2, symtable_count));
				symtable_count += 4;
			}

			addr = symtable[temp2];
			riscv << "lw s3 " << addr << "(s1)" << endl;
			riscv << "bne s3 zero " << temp4 << endl;
			riscv << endl;
		}
		else if(temp1[0] == '_') // _L1:
		{
			riscv << vm_line << endl;
			riscv << endl;
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