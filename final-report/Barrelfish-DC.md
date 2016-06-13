# Decoupling Cores, Kernels, and Operating Systems

### 回新宁 2015210865 白帅 P15240002

> Zellweger, G., Gerber, S., Kourtis, K., & Roscoe, T. (2014). Decoupling cores, kernels, and operating systems. In 11th USENIX Symposium on Operating Systems Design and Implementation (OSDI 14) (pp. 17-31).

这篇论文中在Barrelfish多内核的基础上，通过对CPU核，操作系统内核的解偶，实现了更加灵活的Per-core内核操作系统，支持在线的内核升级、替换转移等。

硬件发展非常迅速，现在的CPU已经支持将多个异构的核心集成在单个CPU上，在不同的工作负载环境下，CPU核可以独立开启或关闭，达到更高效的计算和更少的能耗。对于操作系统来讲，现有系统大都是假定CPU中有固定数量的同构的物理核心，只有在最新的一些特性扩展上才提供对于动态CPU核心的支持。

这篇文章提出的Barrelfish/DC系统的设计前提就是认为所有的CPU核都是动态的。相比于Linux，Barrelfish/DC能够更加灵活地处理CPU核的变化并且只需要更小的系统消耗代价。动态物理核心的关键挑战就是怎么安全地处理每个核上的操作系统状态。Barrelfish/DC通过外化所有的Per-core操作系统和应用程序状态，将其保存在成为OSnodes的对象之中来解决这一挑战。在Barrelfish/DC中可以完整地替换一个物理核心上运行的系统内核，可以动态升级或打补丁，可以在一个核心上运行多核系统状态以及应用，可以运行实时操作系统，这些都不需要关机重启。

在Barrelfish/DC中将CPU核心视为一个物理外接设备，并且通过驱动来控制设备状态。启动一个新的物理核心的过程如下：
1. 首先由平台特定的工具检测到新的物理核心并且注册到设备管理子系统中。
2. Barrelfish/DC为这个新的物理核心选择一个合适的引导驱动。
3. Barrelfish/DC选择一个系统内核程序以及启动参数，并交由引导驱动来完成操作系统内核在这个物理核心上的启动。
4. 新的系统内核完成启动引导通过Barrelfish的协议和其他核上的内核融为一个整体的系统。

在Barrelfish/DC上引导新的核心只涉及到两个进程，管理引导驱动的核以及目标核心上的内核引导。不需要使用全局锁或者其他同步机制，不影响运行在其它核上的系统的性能。

和关闭一个物理核心比起来，引导启动要简单写。关闭CPU core主要的挑战是处理核上正在执行的系统以及程序状态。Barrelfish/DC中将这些都保存在OSnode对象中，包括系统调度状态、时钟偏移量、中断处理状态、内核控制块、进程控制快以及相关的内存信息。OSnode可以完整表示一个Per core系统的运行状态，通过迁移OSnode就可以完成物理核心上的内核状态迁移。在Barrelfish/DC中，还有一种特殊的内核控制块用来控制在单个核心上运行多个内核时的系统行为，只运行一个内核时，KCB基本上不会有额外的性能损失，在运行多个内核时，KCB使用轮转调度多核内核在物理核心上的执行。

最后论文中用实验说明，Barrelfish/DC在Per core内核管理上不仅更加灵活还比Linux更加高效。
