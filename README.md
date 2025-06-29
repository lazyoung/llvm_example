# LLVM Examples

llvm examples build with llvm13. It is suitable for beginners of LLVM.

### quick start
```
git clone git@github.com:awakeljw/llvm_example.git
git submodule update --init --recursive
cmake -B build
cmake --build build
```

### Examples
- `fibonacci.cpp`: llvm original example
- `jit.cpp`: learn mangle and demangle
- `jit_example.cpp`: how to use RTDyldMemoryManager
- `bitcode_example.cpp`: how to read bitcode and execute by ExecutionEngine
- `pass_example.cpp`: how to use PassManager
- `runtimedyld_example.cpp`: how to compile Module to ObjectFile and use runtimeDyld to getSymbol
- `orcjit`: how to use orcjit
- `llvmdis.cpp`: how to use llvm disam API, `6f 00 00 20` -> `j	512`

### build and package libLLVM.so
```
cmake -G Ninja -S llvm -B build/ -DLLVM_INSTALL_UTILS=ON -DCMAKE_INSTALL_PREFIX="../llvm_install/" -DLLVM_ENABLE_PROJECTS="clang;lld;lldb;mlir;clang-tools-extra;cross-project-tests" -DLLVM_BUILD_EXAMPLES=ON -DLLVM_ENABLE_RUNTIMES="openmp;compiler-rt" -DCMAKE_BUILD_TYPE=Release -DLLVM_BUILD_LLVM_DYLIB=ON -DLLVM_LINK_LLVM_DYLIB=ON -DLLVM_TARGETS_TO_BUILD="RISCV;AArch64;NVPTX;X86
```
- `LLVM_BUILD_EXAMPLES`: build '${LLVM_SRC}/llvm/examples'
- `LLVM_BUILD_LLVM_DYLIB`, `LLVM_LINK_LLVM_DYLIB`: generate '${LLVM_BUILD}/lib/libLLVM.so'
- `LLVM_TARGETS_TO_BUILD`: specify supported guest targets
