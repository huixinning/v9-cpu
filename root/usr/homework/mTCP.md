###A Highly Scalable User-level TCP Stack for Multicore systems

随着互联网的发展，文本传输变得越来越频繁，然而这些传输均是一些短的及时连接，所以Short-connection变得越来越多，这些短的传输占据了大部分TCP流的数量,为了很好的提供给用户交互体验，很好的处理这些短连接事务变得尤其重要。

但是现有的linux内核的TCP栈存在四个主要问题导致了short-connection比较低效:
* 第一，连接的局部性很差，这样会导致很多连接都向同一个端口发出请求，产生竞争和等待；
* 第二，共享文件描述空间。同样当连接数量变多时就会产生竞争和等待问题，而且通过VFS访问文件系统会产生额外更多的overhead。
* 第三，低效的单包处理机制。
* 第四，syscall overhead过高。当有许多短的及时连接时就会在user和kernel间频繁的进行切换，所以导致overhead过高。

现在已经有很多工作都试图去解决这个问题，有的工作通过改变API接口去均分syscall的花费去解决syscall overhead过高带来的低效问题，有的工作通过增量的改变现有的实现去解决多核间的竞争问题。


在该篇论文中提出了一种不需要改变现有的代码的情况下来解决该问题，我们发现现有的linux和Megapipe花费了大约80%到83%的CPU cycle在kernel，只留了很小的一部分给user-level的applications，所以我们想通过建立user-level TCP可以使applications直接访问packet，这样就可以使得TCP访问变得高效。

####本篇论文的主要工作分为以下两个方面：
* 通过把syscall转化为应用内部进程间的交流IPC可以大大降低syscall overhead，但是通过这种IPC的方式分享内存间的消息比如文本交换其实花费的要比syscall更多。但是我们采用packet和socket批处理的方式来解决了该问题。而且还综合采用了每个core单独监听socket和多核间任务流负载均衡的机制来提高性能。
* 只通过user-level的改变来解决问题，不存在kernel-level的改变，大大降低了工作的难度。mTCP提供了跟BSD和epoll很像的事件驱动接口，仅需要改变一点就可以实现。

####算法的实现
该篇论文通过11473行C代码实现了包括packet I/O，TCP流管理，user-level socket API和事件调用功能的mTCP功能，而且通过552行代码去修补PSIO库。

####验证
在该篇论文中主要从三个方面验证了mTCP:
* 性能，mTCP是否在短的事务方面提供了很好的性能。实验结果证明，mTCP在linux和MegaPipe上的性能分别提升了25倍和3倍
* 正确性，该论文从公平的角度验证了该方法的正确性和低延迟特性。
* 应用的性能，实验证明，通过采用mTCP将各种应用的性能提高了33%到320%。
