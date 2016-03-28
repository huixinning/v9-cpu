##Scalable Kernel TCP Design and Implementation for short-lived connections

随着移动终端和网络频度带宽的快速增长，在单一机器上的cpu核心上的快速增长，并且API模型的应用要求更短的及时连接，就会频繁产生大量的tcp建立和结束连接，这样就会产生很多共享资源比如说TCB和VFS的竞争，所以为了更好的提供服务，可扩展的TCP的性能显得极为重要。
####TCB
首先来看TCB的可扩展性，我们都知道目前TCP socket在两个表中进行管理，一个是listen table，另一个是establish table。

而且一个TCB连接和更新过程一般分为两个阶段
* 第一，传入的数据包通过cpu接收NIC中断的网络块放入到中断上下文；
* 第二，处理接收的数据包的数据，然后将结果传输出去。

但是在现在的linux TCP控制块上，这两个阶段运行在不同的cpu上，这样就严重的影响了可扩展性。
####VFS
打开或者关闭的VFS的文件描述器的性能对于TCP的建立和连接有着直接的影响，然而在处理VFS共同的节点和索引项时还会存在很多同步化开销，这就导致了VFS的可扩展性的瓶颈。


除此之外，我们还要考虑产品应用环境的需求，比如说和RFC相关的TCP/IP的兼容性、安全性、资源的连接和共享等问题，这些都是对目前TCP/IP协议进行扩展的制约性问题。

#### Fastsocket
本篇论文中我们提出了fastsocket来解决扩展性和兼容性问题，并且实现了每个core上的进程处理空间，不仅避免了锁之间的竞争同时保证了兼容性。Fastsocket的具体实现:
* 第一，把全局共享数据结构、listen table、establish table分开管理，由每个core管理自己table，这样保证了大量连接的正确性。
* 第二，正确控制任何传入数据包连接的本地性，使上面说两个阶段在同一个cpu上进行处理。
* 第三，对于VFS来说提供一个快捷路径来解决扩展性问题，这样可以避免socket间的大量锁竞争并且保证了。

####Conclusion
实验结果证明，fastsocket明显提高了BSD socket API的性能，并且有很好的可扩展性和兼容性，该应用已经很好地应用在了sina微博上。


