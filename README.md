<p align="center">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="resources/logo_dark.png">
    <source media="(prefers-color-scheme: light)" srcset="resources/logo_light.png">
    <img src="resources/logo-light.png" width="400" height="200" alt="VEXA">
  </picture>
</p>

**VEXA** is an x86-64 targeted symbolic execution and lifting framework built on LLVM. It mainly focuses on lifting single functions that obfuscated or virtualized, optimizing/deobfuscating and recompiling them back into the binary.

- Lifts the machine code to the LLVM IR using ***remill***
- Emulates the lifted IR code using ***Z3***
- Applies binary patches and modifications using ***LIEF***

***This is just a hobby project and maintained only by me! I dont promise it will deobf/devirt any kind of commercial protection.*** 

## API Overview
The code below shows an example how to load a binary and lift the function with VEXA.
```cpp
#include <vexa/vexa.h>

int main()
{
	// initialize
	vexa::init();
	vexa::engine engine;

	// load and map binary
	vexa::binary bin("example.bin");
	engine.map_binary(bin);

	// explore and lift until recovering the function at 0x140001000
	engine.run(0x140001000);

	// apply optimizations
	engine.optimize();
}

```
