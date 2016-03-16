#Dune阅读笔记
##如何实现safe user-level access to privileged cpu features?
我们有两种方式去达到该目的:
第一从kernel层面进行改进，但是kernel改进在现实中是不太现实的。

第二，我们可以将应用绑定到虚拟机上。

但是有些时候为了实现这些目的去做的改变或许不值得问题本身。本篇论文首先在相关工作领域做了一些研究，其中包括Azul system通过分页机制明显提高了garbage collection的效率，而且user 模式下的program上的进程迁移会从page fault and syscall中获益。我们在本篇论文中研究了一种新的方式，即通过virtual hardware实现了user模式下的application即可以成功的访问kernel，而且不会损坏kernel的状态。


本文主要从virtualization和 hardware、kernel、user mode environment 和applications四个方面，详细的介绍了实现application can access to privileged cpu 的条件支持，限制以及application的特点。

##virtualization and hardware
首先在第二部分首先介绍了virtualization and hardware，为了提高虚拟化的性能以及简化VMM的实现，Intel实现了VT-x，而且VTX采取可以将cpu分为两种root VMX和non-root VMX的设计模式，这种设计模式可以更好的实现virtual hardware。

##kernel for dune
在接下来的第三部分介绍了kernel支持dune的设计，process using dune可以使其在VMX non-root模式下安全访问系统的硬件部分。

同时Dune使用VT-x与标准的VMMs隔离开来，所以dune会把一个process环境当成一个machine环境，这样就会导致dune并不支持普通的guest OS，但同时保证了dune的轻量级和灵活性。

##mermory management
在dune的实现过程中面临一个最大的问题就是内存管理，因为dune的实现在阻止任意访问物理内存的时候将页表直接暴露给了user programs，但是我们只是想让用户可以灵活的在正确的process memory address添加自己想要的功能，而不是直接可以管理kernel-level memory。Dune通过查询kernel来匹配process memory并且手动更新EPT去映射。

##user-mode environment
与普通的user-mode不同的是，使用dune的user code在ring 0下运行。Ring 3模式同样也可以去跑，但是通常是去跑一些不被信任的code。另外一个不同之处是syscall一定要使用超级调用的模式。

##evaluation
最后该篇论文从sandbox、wedge、garbage collector三个方面对application performance进行了验证。Dune的实现支持在Intel x86 64-bit long模式下，而且该实现可以灵活的支持一次运行上千条进程。

##总结
通过阅读本篇论文首先使我对虚拟化部分有了更进一步的了解，但是本篇论文中同样提到了，这种实现方式同样会产生一些安全方面的威胁，而且dune并不能支持普通的guest os，所以在现实运用方面仍然不够完备，仍值得进一步去探索。

