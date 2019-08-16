### Semaphore 的使用场景 - 限流器
有些场景，可以允许N个并发使用，比如对象池、连接池。

Semaphore，现在普遍翻译为“信号量”，以前也曾被翻译成“信号灯”，因为类似现实生活里的红绿灯，车辆能不能通行，要看是不是绿灯。同样，在编程世界里，线程能不能执行，也要看信号量是不是允许。

```
public static void main(String[] args) {
    // 线程池
    ExecutorService exec = Executors.newCachedThreadPool();
    // 只能5个线程同时访问
    final Semaphore semp = new Semaphore(2);
    // 模拟20个客户端访问
    for (int index = 0; index < 8; index++) {
        final int NO = index;
        Runnable run = new Runnable() {
            public void run() {
                try {
                    // 获取许可
                    semp.acquire();

                    System.out.println("Accessing: " + NO + " time:" + System.currentTimeMillis() / 1000);
                    Thread.sleep(1000);
                    // 访问完后，释放
                    System.out.println("-->" + semp.availablePermits() + " time:" + System.currentTimeMillis() / 1000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                } finally {
                    semp.release();
                }

            }

        };
        exec.execute(run);
    }
    // 退出线程池
    exec.shutdown();
}
```





```
Accessing: 0 time:1566559787
Accessing: 1 time:1566559787
-->0 time:1566559788
-->0 time:1566559788
Accessing: 2 time:1566559788
Accessing: 3 time:1566559788
-->0 time:1566559789
-->0 time:1566559789
Accessing: 4 time:1566559789
Accessing: 5 time:1566559789
-->0 time:1566559790
-->0 time:1566559790
Accessing: 7 time:1566559790
Accessing: 6 time:1566559790
-->0 time:1566559791
-->0 time:1566559791
```

