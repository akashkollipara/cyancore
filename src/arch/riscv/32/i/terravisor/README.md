# RV32 I [Terravisor]

* Home directory for RV32I Terravisor sources.
* I being baseline for RV cores, this is a common directory.

#### Highlights
* Bootstrap routine is mostly in C
* Reduced use of ASM
* Exception handling is unified by updated "mtvec" reg with handler address
* In few platforms, mtvec is hard coded to be used in vector mode, to enable that mode just define "RV_VEC_MODE" in build defines.
