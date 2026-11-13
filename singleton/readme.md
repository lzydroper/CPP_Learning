### 单例模式

> 学的`https://www.bilibili.com/video/BV1LwUeYnEiP`这个教程的
> 都是懒汉式的
> 主要有三个版本，一个是static的，一个是mutex的，一个是模板的
> 都是为了线程安全，static是最简单最方便的，因为static本身能够保证线程安全，不需要额外处理
> 模板是在static版本基础上写的，用的crtp
> mutex核心就是单例的唯一对象用原子操作，然后因为没直接用static，又要保证他能被析构掉，所以要加一个static的deletor对象来在static对象被析构的时候来取处理
> 创建的时候要保证整个过程是有序的，又要防止已经创建好了的时候冗余的加锁检查，所以设置了内存屏障，保证tmp被new的时候顺序执行，cpu缓存读写也得读干净的
> 