# Knowledge Organization

**Du Guikang 的计算机学习记录｜基础知识 · 工程实践 · 深度学习 · Agent**

我是 **Du Guikang**。这个仓库用于持续整理我学习计算机相关技术时积累的笔记、代码示例和项目资料，也记录了学习重点从嵌入式、系统与网络编程，逐步拓展到 PyTorch、大模型和 Agent 的过程。

我按主题编号组织资料，通过 Git 保存各阶段的补充、修订与重编记录。希望把零散的知识逐渐连接起来，形成便于回顾、实践和交流的个人知识库。

## 学习路线

学习过程中，我主要沿着以下几个方向积累，并在接触新问题时回头补充基础知识：

- **语言与计算机基础**：学习 C、Rust、Python，以及操作系统、计算机网络、计算机组成等内容。
- **嵌入式与软硬件交互**：围绕 STM32、FreeRTOS、Qt、LVGL 和 PCB，整理设备采集、通信与界面开发知识。
- **Linux 与工程开发**：学习系统编程、Socket 网络编程，以及编译、构建、调试、版本管理与容器工具。
- **深度学习与大模型**：从 PyTorch 的训练流程与自动求导，延伸到 Transformer、模型训练、推理机制和评测。
- **Agent 与应用工程**：学习工具调用、任务编排、上下文与记忆，并结合项目理解状态管理、可靠性和成本控制。

## 近期学习重点

| 专题 | 主要整理内容 | 阅读入口 |
| --- | --- | --- |
| Agent | Tool Calling、Agent Loop、Harness、RAG、LangGraph、MCP，以及安全、恢复与评测 | [Agent 笔记](2-5-Agent/Agent（重编版）.md) |
| 大模型基础 | Tokenizer、Embedding、Transformer、自回归生成、KV Cache、推理性能与模型评测 | [LLM Base](3-3-LLM%20Base/LLM%20Base.md) |
| PyTorch | Tensor、Autograd、模型构建、数据加载、训练流程、显存、混合精度与部署 | [PyTorch 笔记](2-4-Pytorch/Pytorch.md) |
| Python | Python 语言与编程知识整理 | [Python 笔记](0-4-Python/Python.md) |
| Agent 配套工程 | 结合任务执行场景，学习 FastAPI、PostgreSQL、Redis 与运行时设计 | [EvoAgent Runtime 笔记](P-3-EvoAgent%20Runtime/EvoAgent%20Runtime.md) |

Agent 专题中另有独立整理的 [安全](2-5-Agent/13%20Agent%20安全.md)、[可靠性与恢复](2-5-Agent/14%20可靠性与恢复.md)、[性能与成本](2-5-Agent/15%20性能与成本.md) 和 [测试与评测](2-5-Agent/16%20测试与评测.md)；大模型专题另有 [模型选型笔记](3-3-LLM%20Base/10%20LLM%20模型选型（精简重写版）.md)，方便按具体问题查阅。

## 知识地图

目录沿用学习时建立的编号，便于长期积累和定位；编号用于分类，不代表能力等级或推荐阅读顺序。

| 分类 | 内容与入口 |
| --- | --- |
| `0-*` 编程语言 | [C](0-1-C) · [Rust](0-3-Rust) · [Python](0-4-Python) |
| `1-*` 计算机基础 | [操作系统](1-1-Operation%20System) · [计算机网络](1-2-Computer%20Network) · [计算机组成](1-4-Computer%20Composition) · [信息与通信](1-5-Info%20Communication) |
| `2-*` 平台与框架 | [Linux](2-1-Linux) · [STM32](2-2-STM32) · [RTOS](2-3-RTOS) · [PyTorch](2-4-Pytorch) · [Agent](2-5-Agent) |
| `3-*` 系统编程与模型基础 | [系统编程](3-1-System%20Programming) · [网络编程](3-2-Network%20Programming) · [LLM Base](3-3-LLM%20Base) |
| `4-*` / `5-*` 界面与硬件 | [Qt](4-1-Qt) · [LVGL](4-2-LVGL) · [PCB](5-1-PCB) |
| `6-*` 开发工具 | [编译、构建与调试](6-0-Tools) · [Git](6-1-Git) · [Docker](6-2-Docker) |
| `7-*` 设计方法 | [系统设计](7-1-System%20Design) · [设计模式](7-2-Design%20Pattern) |
| `P-*` 项目资料 | [录音棚监控](P-1-Recording%20Room) · [服装仓库物联](P-2-Clothing%20Warehouse%20IoT%20System) · [EvoAgent Runtime](P-3-EvoAgent%20Runtime) |
| `W-*` 工作相关学习记录 | [Chinasofti](W-1-Chinasofti) · [Hirain](W-2-Hirain) |

## 项目中的学习记录

除了专题笔记，我也保留项目相关的技术整理，将知识点放回具体的数据流、执行流程和系统结构中理解。

| 项目 | 学习与整理内容 | 资料入口 |
| --- | --- | --- |
| EvoAgent Runtime | Agent 任务执行、工具权限、预算、检查点、经验与 Skill 管理，以及评测和设计取舍 | [项目解析](P-3-EvoAgent%20Runtime/PROJECT_LEARNING_GUIDE.md) · [复习速查](P-3-EvoAgent%20Runtime/INTERVIEW_CHEATSHEET.md) |
| 服装仓库物联系统 | STM32、ZigBee、RFID、MQTT，以及设备与业务端之间的数据交互 | [技术笔记](P-2-Clothing%20Warehouse%20IoT%20System/Clothing%20Warehouse%20IoT%20System.md) · [工程资料](P-2-Clothing%20Warehouse%20IoT%20System/基于ZigBee与RFID的智能服装管理销售系统) |
| 录音棚环境监控 | STM32 数据采集、滤波、UART 通信与 Qt 上位机 | [项目资料](P-1-Recording%20Room/Recording%20Room) |

EvoAgent 目录主要保存项目解析和学习文档，应用源码不在此目录中；具体实现与验证范围见对应文档。

## 学习方式与迭代记录

我习惯围绕 **问题是什么 → 为什么这样设计 → 如何工作 → 边界在哪里 → 怎样验证** 整理知识，并通过示例、项目解析和复习检查自己的理解。随着学习深入，我会继续补充遗漏、修正理解和调整章节结构。

Git 中保留了这些迭代。以下是近期几个代表性的整理节点：

| 代表性提交 | 学习迭代 |
| --- | --- |
| [Linux 编程 · 6b1e068](https://github.com/Kenvily/knowledge-organization/commit/6b1e068) | 系统编程基础整理 |
| [PyTorch · a55ddbd](https://github.com/Kenvily/knowledge-organization/commit/a55ddbd) | 深度学习框架专题整理 |
| [Python · 936ad09](https://github.com/Kenvily/knowledge-organization/commit/936ad09) | Python 专题更新 |
| [LLM Base · 4b4966f](https://github.com/Kenvily/knowledge-organization/commit/4b4966f) | 大模型基础专题整理 |
| [Agent + EvoAgent · a0586eb](https://github.com/Kenvily/knowledge-organization/commit/a0586eb) | Agent 专题与运行时项目资料更新 |

完整过程见 [提交历史](https://github.com/Kenvily/knowledge-organization/commits/master/)。

---

本仓库以个人学习整理为主，包含参考资料、第三方组件及历史工程文件；具体来源与许可请结合各目录说明阅读。代码示例的运行条件、项目实现边界与实验结论以对应文档为准。

欢迎围绕 **Agent 运行时、LLM 原理、训练推理工程与系统设计** 交流，也欢迎通过 [Issues](https://github.com/Kenvily/knowledge-organization/issues) 指出错误或提出讨论。
