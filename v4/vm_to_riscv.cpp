/*
 * RISC-V Stack based VM code to RISC-V assembly language code: CS4100 Computer System Design RISC-V Project
 *
 *  Created on: November 30, 2020
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

string getreg(string varType)
{
	if(varType == "local")
		return "s6";
	if(varType == "arg")
		return "s7";
	if(varType == "temp")
		return "s8";
	return "constant";
}

int main()
{

	ifstream vm;
	ofstream riscv;

	string vm_line, temp1, temp2, temp3, temp4, temp5, prev_instr = "";
	int i, func_ret = 0;

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

	// implicit call to main
	riscv << "addi s6, zero, 16" << endl;
	riscv << "add s7, zero, zero" << endl;
	riscv << "add sp, zero, zero" << endl;
	riscv << "add s8, zero, zero" << endl;
	riscv << "addi ra, zero, _$End" << endl;
	riscv << "sw ra, 0(sp)" << endl;
	riscv << "addi sp, sp, 4" << endl;
	// push LCL
	riscv << "sw s6, 0(sp)" << endl;
	riscv << "addi sp, sp, 4" << endl;
	// push ARG
	riscv << "sw s7, 0(sp)" << endl;
	riscv << "addi sp, sp, 4" << endl;
	// push TMP
	riscv << "sw s8, 0(sp)" << endl;
	riscv << "addi sp, sp, 4" << endl;
	riscv << endl;

	while(getline(vm, vm_line))
	{
		temp1 = "";
		temp2 = "";
		temp3 = "";
		temp4 = "";
		temp5 = "";

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
			while(i<vm_line.size()) // type = local/arg/constant/temp
			{
				if(vm_line[i] == ' ')
					break;
				temp2 += vm_line[i];
				i++;
			}

			i++;

			while(i<vm_line.size()) // memory segment index
			{
				if(vm_line[i] == ' ')
					break;
				temp3 += vm_line[i];
				i++;
			}

			i++;

			if(temp1 == "push" && temp2 != "constant") // push variable
			{
				string reg = getreg(temp2);
				int idx = stoi(temp3);
				idx = idx * 4;
				riscv << "lw s3, " << idx << "(" << reg << ")" << endl;
				riscv << "sw s3, 0(sp)" << endl; // stack pointer is stored in register sp
				riscv << "addi sp, sp, 4" << endl;
				prev_instr = "push";
			}
			else if(temp1 == "push" && temp2 == "constant") // push constant immediate
			{
				riscv << "addi s3, zero, " << temp3 << endl; // zero register always contains zero
				riscv << "sw s3, 0(sp)" << endl;
				riscv << "addi sp, sp, 4" << endl;
				prev_instr = "push";
			}
			else // pop variable
			{
				if(prev_instr == "push")
				{
					riscv << "addi sp, sp, -4" << endl;
				}

				if(prev_instr == "call")
				{
					riscv << "addi sp, sp, -4" << endl;
					riscv << "lw s3, 0(sp)" << endl;
				}

				string reg = getreg(temp2);
				int idx = stoi(temp3);
				idx = idx * 4;
				riscv << "sw s3, " << idx << "(" << reg << ")" << endl;
				riscv << endl;
			}
		}
		else if(instr.count(temp1) > 0) // instruction like add, sub, etc.
		{
			riscv << "addi sp, sp, -4" << endl;
			riscv << "lw s5, 0(sp)" << endl;
			riscv << "addi sp, sp, -4" << endl;
			riscv << "lw s4, 0(sp)" << endl;
			riscv << temp1 << " s3, s4, s5" << endl;
			prev_instr = temp1;
		}
		else if(cond_instr_1.count(temp1) > 0) // instructions like eq, neq, lt, gt
		{
			riscv << "addi sp, sp, -4" << endl;
			riscv << "lw s5, 0(sp)" << endl;
			riscv << "addi sp, sp, -4" << endl;
			riscv << "lw s4, 0(sp)" << endl;
			riscv << "sub s3, s4, s5" << endl;

			if(temp1 == "eq")
				riscv << "sltiu s3, s3, 1" << endl;
			else if(temp1 == "neq")
				riscv << "sltu s3, zero, s3" << endl;
			else if(temp1 == "lt")
				riscv << "slt s3, s3, zero" << endl;
			else
				riscv << "slt s3, zero, s3" << endl;

			prev_instr = temp1;
		}
		else if(cond_instr_2.count(temp1) > 0) // instructions like lte, gte
		{
			riscv << "addi sp, sp, -4" << endl;
			riscv << "lw s5, 0(sp)" << endl;
			riscv << "addi sp, sp, -4" << endl;
			riscv << "lw s4, 0(sp)" << endl;
			riscv << "sub s3, s4, s5" << endl;

			if(temp1 == "lte")
				riscv << "slt s4, s3, zero" << endl;
			else
				riscv << "slt s4, zero, s3" << endl;

			riscv << "sltiu s5, s3, 1" << endl;
			riscv << "or s3, s4, s5" << endl;
			prev_instr = temp1;
		}
		else if(cond_instr_3.count(temp1) > 0) // instructions like dand, dor
		{
			riscv << "addi sp, sp, -4" << endl;
			riscv << "lw s5, 0(sp)" << endl;
			riscv << "addi sp, sp, -4" << endl;
			riscv << "lw s4, 0(sp)" << endl;
			riscv << "sltu s4, zero, s4" << endl;
			riscv << "sltu s5, zero, s5" << endl;
			
			if(temp1 == "dand")
				riscv << "and s3, s4, s5" << endl;
			else
				riscv << "or s3, s4, s5" << endl;

			prev_instr = temp1;
		}
		else if(temp1 == "goto") // goto _L1
		{
			while(i<vm_line.size())
			{
				temp2 += vm_line[i];
				i++;
			}

			riscv << "jal zero, " << temp2 << endl;
			riscv << endl;
		}
		else if(temp1 == "If") // If type 0 goto _L1
		{
			while(i<vm_line.size()) // type = temp/local/arg/comstant
			{
				if(vm_line[i] == ' ')
					break;
				temp2 += vm_line[i];
				i++;
			}

			i++;

			while(i<vm_line.size()) // memory segment index/offset
			{
				if(vm_line[i] == ' ')
					break;
				temp3 += vm_line[i];
				i++;
			}

			i++;

			while(i<vm_line.size()) // goto
			{
				if(vm_line[i] == ' ')
					break;
				temp4 += vm_line[i];
				i++;
			}

			i++;

			while(i<vm_line.size()) // _L1
			{
				if(vm_line[i] == ' ')
					break;
				temp5 += vm_line[i];
				i++;
			}

			i++;

			string reg = getreg(temp2);
			if(reg == "constant")
				riscv << "addi s3, zero, " << temp3 << endl;
			else
			{
				int idx = stoi(temp3);
				idx = idx * 4;
				riscv << "lw s3, " << idx << "(" << reg << ")" << endl;
			}
			riscv << "bne s3, zero, " << temp5 << endl;
			riscv << endl;
		}
		else if(temp1[0] == '_') // _L1:
		{
			riscv << vm_line << endl;
			riscv << endl;
		}
		else if(temp1 == "pushParam") // push/pop parameter of a function
		{
			while(i<vm_line.size()) // type = local/arg/constant/temp
			{
				if(vm_line[i] == ' ')
					break;
				temp2 += vm_line[i];
				i++;
			}

			i++;

			while(i<vm_line.size()) // memory segment index
			{
				if(vm_line[i] == ' ')
					break;
				temp3 += vm_line[i];
				i++;
			}

			i++;

			if(temp2 != "constant") // push variable
			{
				string reg = getreg(temp2);
				int idx = stoi(temp3);
				idx = idx * 4;
				riscv << "lw s3, " << idx << "(" << reg << ")" << endl;
				riscv << "sw s3, 0(sp)" << endl; // stack pointer is stored in register sp
				riscv << "addi sp, sp, 4" << endl;
				prev_instr = temp1;
			}
			else // push constant immediate
			{
				riscv << "addi s3, zero, " << temp3 << endl; // zero register always contains zero
				riscv << "sw s3, 0(sp)" << endl;
				riscv << "addi sp, sp, 4" << endl;
				prev_instr = temp1;
			}

			riscv << endl;
		}
		else if(temp1 == "call") // call _L1 2
		{
			while(i<vm_line.size()) // function label
			{
				if(vm_line[i] == ' ')
					break;
				temp2 += vm_line[i];
				i++;
			}

			i++;

			while(i<vm_line.size()) // nARG
			{
				if(vm_line[i] == ' ')
					break;
				temp3 += vm_line[i];
				i++;
			}

			i++;

			// push retAddr
			func_ret++;
			riscv << "addi ra, zero, _$Ret" << func_ret << endl;
			riscv << "sw ra, 0(sp)" << endl;
			riscv << "addi sp, sp, 4" << endl;
			// push LCL
			riscv << "sw s6, 0(sp)" << endl;
			riscv << "addi sp, sp, 4" << endl;
			// push ARG
			riscv << "sw s7, 0(sp)" << endl;
			riscv << "addi sp, sp, 4" << endl;
			// push TMP
			riscv << "sw s8, 0(sp)" << endl;
			riscv << "addi sp, sp, 4" << endl;

			int nARG = stoi(temp3);
			int nMemLoc = nARG + 4;
			int nMemBytes = nMemLoc * 4;
			riscv << "addi s7, sp, -" << nMemBytes << endl; // ARG = SP - ((nARG + 4) * 4)

			riscv << "add s6, sp, zero" << endl; // set LCL = SP

			riscv << "jal zero, " << temp2 << endl; // goto function label
			riscv << "_$Ret" << func_ret << ":" << endl;
			riscv << endl;

			prev_instr = temp1;
		}
		else if(temp1 == "function") // function func 2 2
		{
			while(i<vm_line.size()) // function name
			{
				if(vm_line[i] == ' ')
					break;
				temp2 += vm_line[i];
				i++;
			}

			i++;

			while(i<vm_line.size()) // nLCL
			{
				if(vm_line[i] == ' ')
					break;
				temp3 += vm_line[i];
				i++;
			}

			i++;

			while(i<vm_line.size()) // nTMP
			{
				if(vm_line[i] == ' ')
					break;
				temp4 += vm_line[i];
				i++;
			}

			i++;

			int nLCL = stoi(temp3);
			int nTMP = stoi(temp4);

			for(int j=0; j<nLCL; j++) // push 0 nLCL times
			{
				riscv << "sw zero, 0(sp)" << endl;
				riscv << "addi sp, sp, 4" << endl;
			}

			riscv << "add s8, sp, zero" << endl; // set TMP = SP

			for(int j=0; j<nTMP; j++) // push 0 nTMP times
			{
				riscv << "sw zero, 0(sp)" << endl;
				riscv << "addi sp, sp, 4" << endl;
			}

			riscv << endl;

			prev_instr = temp1;
		}
		else if(temp1 == "return") // return
		{
			//obtain endframe in register fp
			riscv << "add fp, s6, zero" << endl;

			//obtain retAddr in register ra
			riscv << "addi s4, fp, -16" << endl;
			riscv << "lw ra, 0(s4)" << endl;

			// *ARG = return value
			riscv << "addi sp, sp, -4" << endl;
			riscv << "lw s4, 0(sp)" << endl;
			riscv << "sw s4, 0(s7)" << endl;

			// SP = ARG + 4
			riscv << "addi sp, s7, 4" << endl;

			//TMP
			riscv << "addi s4, fp, -4" << endl;
			riscv << "lw s8, 0(s4)" << endl;

			//ARG
			riscv << "addi s4, fp, -8" << endl;
			riscv << "lw s7, 0(s4)" << endl;

			//LCL
			riscv << "addi s4, fp, -12" << endl;
			riscv << "lw s6, 0(s4)" << endl;

			// goto return address
			riscv << "jal ra, 0" << endl;
			riscv << endl;

			prev_instr = temp1;
		}
	}

	riscv << "_$End:" << endl;

	vm.close();
	riscv.close();

	return 0;
}