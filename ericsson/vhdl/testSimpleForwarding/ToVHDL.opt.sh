# functions in prog.c to be mapped to vhdl
# generate llvm-byte-code
clang -std=gnu89 -I../../../v2/iolib/ -emit-llvm -c prog.c
opt -O3 prog.o > prog.opt.o
# disassemble byte-code for human readibility.
llvm-dis prog.opt.o
# llvm-bytecode to Aa
llvm2aa prog.opt.o | vcFormat >  prog.o.opt.aa
# map external memory references from system to
# internal object mempool (of size 1 byte).
#AaLinkExtMem -I 1 -E mempool prog.o.aa | vcFormat > prog.o.linked.aa
# take linked Aa and convert to virtual circuit.
Aa2VC -O -I mempool -C prog.o.opt.aa | vcFormat > prog.o.opt.aa.vc
#
# take virtual circuit and convert to VHDL
# NOTE: the -T option tells vc2vhdl that ctrl_module is to be an ever-running
# top-level module, which will not need to be started from the outside, but
# will be free-running in the system (such modules cannot have input/output
# arguments, but can only listen/send on pipes).
#
# change -s ghdl to -s modelsim if you are using modelsim
 vc2vhdl -C -s ghdl -e ahir_system -w -T ctrl_module  -T data_module   -f prog.o.opt.aa.vc 
vhdlFormat < ahir_system.unformatted_vhdl > ahir_system.vhdl
vhdlFormat < ahir_system_test_bench.unformatted_vhdl > ahir_system_test_bench.vhdl
