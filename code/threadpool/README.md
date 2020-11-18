### Threadpool 线程池
---
* 半同步/半反应堆(`half-sync/half-reactive`)模式
  通过`append`将处理事件放入请求队列中，
  工作线程竞争从请求队列取出任务并完成
* 使用一个工作队列完全解除了主线程和工作线程的耦合关系：主线程往工作队列中插入任务，工作线程通过竞争来取得任务并执行它。
* 同步I/O模拟`proactor`模式

![image](./half-sync-reactive.bmp)