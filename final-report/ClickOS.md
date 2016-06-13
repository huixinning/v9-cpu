# ClickOS and the art of network function virtualization

### 回新宁 2015210865 白帅 P15240002

> Martins, J., Ahmed, M., Raiciu, C., Olteanu, V., Honda, M., Bifulco, R., & Huici, F. (2014, April). ClickOS and the art of network function virtualization. In Proceedings of the 11th USENIX Conference on Networked Systems Design and Implementation (pp. 459-473). USENIX Association.

这篇文章主要是使用虚拟化的方式解决如今网络中 middleboxes 相关的一些问题。

middleboxes 已经成为目前网络中必不可少的组件。middleboxes 提供各种各样的网络功能，比如安全、路由、流量控制等。但是 middlebosex 也带来许多问题，管理以及维护的费用高、基于硬件的移植性差或者不能移植、middleboxes 的功能基本上不能改变。

middleboxes 是基于硬件的，所以要增加其可移植性以及可伸缩性，使其能够在通用的平台上运行，一种很自然地想法就是使用软件来实现。在 middleboxes 与硬件之间增加一个软件层，屏蔽硬件的不一致性。多了一个中间层当然会带来一些性能损失，文中的重点就是如何降低这一中间层的性能损失。另一方面，文中作者们对于软件重用也做了很大的努力。

这篇文章中提出使用网络功能虚拟化的方法解决这一问题。文中实现 了 ClickOS——一个高性能的、虚拟化的 middleboxes 软件平台。

首先，在虚拟环境的选择上，作者选择了 Xen，因为 Xen 支持半虚拟化功能，这对提供一个低延迟、高吞吐量的平台有着非常关键的作用。在这个之上，作者们选择 Click 作为对于 middleboxes 接又的抽象，Click 在这方面的工作已经比较成熟了，使用 Click 能够重用大量的代码减少工作量。有了这个工作之后，作者们考虑操作系统需要提供怎样的功能能使得 Click 成为一个可以处理 middleboxes 的平台呢?根据这一问题，作者们在 MiniOS 上进行了二次开发，和 Click 一起编译成新的系统，ClickOS 就诞生了。作者巧妙地将网络功能分为 netback 和 netfront，netback 负责网卡驱动，这一部分交给已有的系统去完成，在 netback 和 netfront 之间使用 Xen Bus 传输，每一个 ClickOS 的实例作为一个 netfront。

在这个之外，作者们分析了 Xen 中网络瓶颈，并对一些组件进行了优 化以达到更好的性能。包括重新设计了 Xen 的 I/O 系统。最终作者进行了实际的测试，ClickOS 在启动引导时间、网络延迟、吞吐量、以及可伸缩性等方面的表现都非常好。
