# 🚀 TinyWebServer (量化低延迟行情重构版)

Linux下C++轻量级Web服务器，**并在原版基础上，面向量化交易（Quant）高频、低延迟（Low-Latency）场景进行了深度底层重构，构建了微秒级的极速行情分发网关。**

## 🔥 核心升级亮点 (Quant-Edition Features)

在原有主从 Reactor 架构基础上，引入了以下硬核底层优化，以满足量化系统对极致性能的压榨：

- **SPSC 无锁环形队列 (Lock-Free Ring Buffer)：** 彻底剥离传统的 `std::mutex` 锁竞争，使用 C++11 `std::atomic` 实现单生产者单消费者无锁队列。
- **内存模型与屏障优化：** 弃用默认的强一致性模型，采用 `memory_order_acquire` 和 `memory_order_release` 内存屏障，防止指令重排的同时降低同步开销。
- **消除伪共享 (False Sharing)：** 通过 `alignas(64)` 对齐关键游标，将高频读写变量隔离在不同的 CPU 缓存行 (Cache Line) 中。
- **WebSocket 全双工协议拦截：** 深度侵入原版 HTTP 状态机，实现 `101 Switching Protocols` 握手与数据帧（Frame）掩码解密，构建服务端实时主动推流通道。
- **旁路 Epoll 极速物理直推：** 放弃传统的 “Epoll 写事件就绪触发” 机制，采用独立后台绑核线程，轮询获取数据后直接调用 `send` 强穿系统网络栈，实现物理极限的极低延迟。
- **内核网络协议栈调优：** 针对行情 Socket 强制开启 `TCP_NODELAY` 关闭 Nagle 算法，防止小包合并延迟；并扩大 `SO_SNDBUF` 防止高并发爆发行情导致丢包。

---

## 🛠️ 基础功能 (Base Features)

- 使用 **线程池 + 非阻塞socket + epoll(ET和LT均实现) + 事件处理(Reactor和模拟Proactor均实现)** 的并发模型
- 使用状态机解析HTTP请求报文，支持解析GET和POST请求
- 访问服务器数据库实现web端用户注册、登录功能，可以请求服务器图片和视频文件
- 实现同步/异步日志系统，记录服务器运行状态
- 经Webbench压力测试可以实现上万的并发连接数据交换

## 🔀 架构重构：双轨数据流设计

本项目重构后，系统内存在两条互不干扰的数据链路：
1. **控制流 (低频/高并发)：** Client -> `Epoll` -> 原有线程池 -> HTTP状态机/WebSocket握手 -> `TopicManager` 注册路由表。
2. **数据流 (极高频/低延迟)：** 行情上游 -> `LockFreeQueue` 无锁入队 -> 独立 `PushThread` 无锁出队 -> 查读写锁路由表 -> 绕过 Epoll 直接 `send` 到网卡。

## 📊 Demo演示

### 1. 量化极速行情看板 (新增)
在终端运行服务器后，浏览器访问 `http://ip:port/quant.html`。
系统将自动升级为 WebSocket 协议，并通过无锁队列每秒推送十次以上的高频 Tick 行情数据，实现毫秒级 UI 价格红绿闪烁更新。
*(黑客帝国/暗黑风格的高频交易监控面板)*

### 2. 基础 Web 演示
- 注册/登录演示
- 请求图片文件演示(6M)
- 请求视频文件演示(39M)

## ⏱️ 压力测试
在关闭日志后，使用Webbench对服务器进行压力测试，对listenfd和connfd分别采用ET和LT模式，均可实现上万的并发连接.

- Proactor，LT + LT，93251 QPS
- Proactor，LT + ET，97459 QPS
- Proactor，ET + LT，80498 QPS
- Proactor，ET + ET，92167 QPS
- Reactor，LT + ET，69175 QPS

*(注：量化推送引擎的直接 `send` 性能上限取决于物理网卡带宽与客户端接收窗口，不受上述传统 HTTP 压测模型限制)*

---

## 🚀 快速运行

**服务器测试环境**
* Ubuntu / Debian
* MySQL版本 5.7+
* C++ 编译器支持 C++17 标准 (`g++ -std=c++17`)

**1. 测试前确认已安装MySQL数据库**
```sql
-- 建立yourdb库
create database yourdb;

-- 创建user表
USE yourdb;
CREATE TABLE user(
    username char(50) NULL,
    passwd char(50) NULL
)ENGINE=InnoDB;

-- 添加数据
INSERT INTO user(username, passwd) VALUES('name', 'passwd');
