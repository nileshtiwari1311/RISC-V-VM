all: tac_to_vm.cpp vm_to_riscv.cpp tac.txt
	g++ tac_to_vm.cpp -o tac_to_vm
	g++ vm_to_riscv.cpp -o vm_to_riscv
	./tac_to_vm
	./vm_to_riscv

clean:
	rm -rf tac_to_vm vm_to_riscv vm.txt riscv.txt riscv_opt.txt