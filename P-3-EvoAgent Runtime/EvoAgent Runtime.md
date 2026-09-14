---
number headings: auto, first-level 1, max 6, 1.1
---

# 1 FastAPI：从一次请求到一个 Agent 任务

这一章只沿着一条主线展开

```text
用户提交一个数据分析任务
        ↓
FastAPI 接收 HTTP 请求
        ↓
校验输入、识别用户、检查权限
        ↓
调用任务服务
        ↓
保存任务并交给 Agent Worker
        ↓
返回 task_id
        ↓
用户查询进度和结果
```

后面出现的路由、Pydantic、依赖注入、异步、任务队列和 Worker，都是为了让这条链路更清晰、更安全或更可靠。

## 1.1 为什么需要 FastAPI

### 1.1.1 先从问题开始：前端不能直接调用 Python 函数

假设后端已经写好了一个函数

```python
def create_agent_task(instruction: str) -> str:
    return "task-001"
```

同一个 Python 进程中的代码可以直接调用它

```python
task_id = create_agent_task("分析最近七天订单失败原因")
```

但浏览器、手机 App、Java 服务和命令行工具通常运行在不同机器或不同进程中。

它们不能直接调用这个 Python 函数。

双方需要共同遵守一种跨进程通信规则

```text
调用哪个地址？
使用 GET 还是 POST？
参数放在哪里？
数据采用什么格式？
成功和失败如何表达？
```

HTTP API 就是这套对外通信契约。

例如

```http
POST /api/tasks
Content-Type: application/json

{
  "task_type": "data_analysis",
  "instruction": "分析最近七天订单失败原因"
}
```

服务端返回

```http
HTTP/1.1 202 Accepted
Content-Type: application/json

{
  "id": "task-001",
  "status": "pending"
}
```

这时客户端不需要知道服务端使用 Python、LangGraph 还是其他框架。

它只需要遵守 API 契约。

### 1.1.2 FastAPI 是什么

FastAPI 是一个用于编写 Python Web API 的框架。

它主要解决五件事

```text
1. 路由：这个 URL 应该调用哪个 Python 函数？
2. 取值：参数来自路径、查询字符串、请求头还是 JSON？
3. 校验：客户端传来的数据是否合法？
4. 响应：Python 对象怎样转换为 HTTP 响应？
5. 文档：接口契约怎样生成 OpenAPI 文档？
```

FastAPI 不负责替代

- PostgreSQL
- Redis
- LangGraph
- 消息队列
- 大模型推理服务
- 完整的权限策略系统

在项目中，FastAPI 更像企业办事大厅

```text
用户来到办事大厅
      ↓
前台确认办什么业务                 路由
      ↓
检查申请表是否填写正确             Pydantic
      ↓
核验申请人身份                     Dependency
      ↓
把材料交给真正的业务部门           Service / Worker
      ↓
返回受理编号                       HTTP Response
```

所以 FastAPI 是系统入口，不是整个系统。

### 1.1.3 为什么 FastAPI 强调类型注解

不使用类型模型时，我们可能要手动写

```python
body = await request.json()

if "instruction" not in body:
    return error("instruction is required")

if not isinstance(body["instruction"], str):
    return error("instruction must be a string")

if len(body["instruction"]) > 2000:
    return error("instruction is too long")
```

当字段越来越多时，会出现三个问题

1. 校验代码重复
2. 实现和接口文档容易不一致
3. 编辑器不知道数据的准确类型

FastAPI 选择“类型驱动”的设计

```python
class TaskCreate(BaseModel):
    instruction: str = Field(min_length=1, max_length=2000)
```

同一份声明可以同时用于

```text
运行时数据校验
+ Python 类型提示
+ JSON Schema
+ OpenAPI 接口文档
```

这就是 FastAPI 的重要设计动机：尽量让接口契约只有一个可信来源，减少代码、文档和实际行为互相矛盾。

### 1.1.4 FastAPI、Uvicorn、Starlette、Pydantic 的关系

一次请求会经过多个组件

```text
客户端
  ↓ HTTP
Uvicorn
  ↓ ASGI
Starlette / FastAPI
  ↓
Pydantic + 业务代码
```

|    组件     |           是什么           |                  为什么需要                  |
| :-------: | :---------------------: | :-------------------------------------: |
|  Uvicorn  |       ASGI Server       |            监听端口、解析连接，把请求交给应用            |
|   ASGI    |   Server 与应用之间的异步接口规范   |          让服务器和框架可以相互替换、支持异步连接           |
| Starlette |     轻量 ASGI Web 基础库     | 提供路由、中间件、Request、Response、WebSocket 等能力 |
|  FastAPI  | 建立在 Starlette 上的 API 框架 |        增加类型驱动的参数解析、依赖注入和 OpenAPI        |
| Pydantic  |        数据校验与序列化库        |        把不可信输入转换为满足约束的 Python 对象         |

初学时不必研究每个组件的源码。

先记住一句话：Uvicorn 负责“接到请求”，FastAPI 负责“理解请求”，Pydantic 负责“检查数据”，业务服务负责“真正办事”。

### 1.1.5 FastAPI 是 Web Server 吗？

**参考回答**

FastAPI 是 ASGI Web 框架，不是直接监听网络端口的 Web Server。开发和部署时通常由 Uvicorn 等 ASGI Server 加载 FastAPI 应用。Uvicorn 负责连接和协议处理，FastAPI 负责路由、参数解析、依赖注入与响应。

**容易答错的地方**

把 FastAPI、Uvicorn、ASGI 当成三个不同名字的同一个东西。

## 1.2 一次请求到底经历了什么

先不急着学习很多装饰器。

先看一个最小程序

```python
from fastapi import FastAPI


app = FastAPI()


@app.get("/health")
async def health() -> dict[str, str]:
    return {"status": "ok"}
```

启动

```bash
uvicorn main:app --reload
```

访问

```http
GET /health
```

会得到

```json
{"status": "ok"}
```

### 1.2.1 从客户端到 Python 函数

完整过程可以简化为

```text
1. 客户端向 127.0.0.1:8000 发送 HTTP 请求
2. Uvicorn 接收请求
3. Uvicorn 按 ASGI 规范把请求交给 FastAPI
4. FastAPI 查找 GET + /health 对应的路由
5. FastAPI 解析该路由需要的参数和依赖
6. FastAPI 调用 health()
7. health() 返回 Python 字典
8. FastAPI 把字典序列化为 JSON 响应
9. Uvicorn 把 HTTP 响应发送给客户端
```

这一流程解释了代码中的每一部分。

```python
app = FastAPI()
```

创建应用对象。路由、异常处理器和中间件最终都会注册到这个对象上。

```text
@app.get("/health")
```

建立映射

```text
HTTP 方法 GET + 路径 /health → health 函数
```

为什么必须同时考虑方法和路径？

因为同一个路径可以表达不同动作

```http
GET  /api/tasks     查询任务
POST /api/tasks     创建任务
```

```text
async def health() -> dict[str, str]:
```

定义路径操作函数。

`async` 表示它可以使用异步等待；返回类型告诉编辑器和 FastAPI 预期结果是什么。

```text
return {"status": "ok"}
```

业务函数只返回 Python 对象，不需要手写完整 HTTP 报文。

框架负责 JSON 编码和响应头等工作。

### 1.2.2 FastAPI 怎样找到输入数据

下面的接口同时使用四种输入

```python
from typing import Annotated

from fastapi import FastAPI, Header
from pydantic import BaseModel


app = FastAPI()


class TaskUpdate(BaseModel):
    instruction: str


@app.put("/api/tasks/{task_id}")
async def update_task(
    task_id: str,
    body: TaskUpdate,
    dry_run: bool = False,
    authorization: Annotated[str | None, Header()] = None,
) -> dict:
    return {
        "task_id": task_id,
        "instruction": body.instruction,
        "dry_run": dry_run,
        "has_token": authorization is not None,
    }
```

对应请求

```http
PUT /api/tasks/task-001?dry_run=true
Authorization: Bearer token-value
Content-Type: application/json

{
  "instruction": "重新分析订单失败原因"
}
```

FastAPI 的判断规则

|      函数参数       |  数据来源  |         为什么          |
| :-------------: | :----: | :------------------: |
|    `task_id`    |  Path  | 名字出现在 `/{task_id}` 中 |
|    `dry_run`    | Query  |     简单类型，且不在路径中      |
|     `body`      |  Body  |   类型是 Pydantic 模型    |
| `authorization` | Header |  使用 `Header()` 明确声明  |

这种设计的原因是：函数签名本身就是接口契约，读函数定义就能知道请求需要什么。

### 1.2.3 为什么自动文档不是“额外赠品”

运行应用后访问

```text
/docs
/redoc
/openapi.json
```

FastAPI 根据路由、类型和 Pydantic 模型生成 OpenAPI。

OpenAPI 不只是给人看的页面，它还可以用于

- 前后端确认接口契约
- 生成客户端 SDK
- 做接口兼容性检查
- 驱动自动化测试
- 描述认证方式和错误响应

如果接口函数没有准确声明响应模型和错误状态码，自动文档也会不准确。

所以“能打开 `/docs`”不等于 API 设计良好。

### 1.2.4 请描述 FastAPI 的请求处理流程

**参考回答**

Uvicorn 接收 HTTP 请求并通过 ASGI 交给 FastAPI。FastAPI 根据请求方法和路径匹配路由，解析依赖和参数，调用 Pydantic 完成数据校验。校验与依赖都成功后，才调用路径操作函数。函数返回值再按响应模型过滤、序列化，最后由 Uvicorn 发送给客户端。

**面试官追问**

请求体校验失败以后，路径操作函数还会执行吗？

不会。参数解析和依赖解析发生在调用路径操作函数之前。

## 1.3 为什么需要 Pydantic：API 边界上的数据不可信

### 1.3.1 “客户端会按文档传参”是危险假设

我们希望客户端提交

```json
{
  "task_type": "data_analysis",
  "instruction": "分析最近七天订单失败原因",
  "priority": 5
}
```

实际可能收到

```json
{
  "task_type": 100,
  "instruction": "",
  "priority": 999,
  "admin": true
}
```

原因可能是

- 前端 Bug
- 旧版本客户端
- 调用方理解错误
- 恶意请求
- 字段拼写错误

如果这些数据直接进入业务层或数据库，错误会在更深处爆发，定位困难，还可能造成越权或脏数据。

因此系统需要在入口建立明确的数据边界。

### 1.3.2 Pydantic 模型是什么

Pydantic 模型是一种带运行时校验能力的数据模型。

```python
from pydantic import BaseModel, ConfigDict, Field


class TaskCreate(BaseModel):
    model_config = ConfigDict(extra="forbid")

    task_type: str = Field(pattern=r"^[a-z][a-z0-9_]{2,63}$")
    instruction: str = Field(min_length=1, max_length=2000)
    priority: int = Field(default=5, ge=1, le=10, strict=True)
```

它表达的不只是三个 Python 字段，还表达一份契约

```text
task_type
  必须是字符串，并符合命名格式

instruction
  必须是 1～2000 个字符的字符串

priority
  必须是 1～10 的整数，不传时为 5

extra="forbid"
  出现未知字段时直接拒绝
```

### 1.3.3 为什么要拒绝未知字段

假设调用方把

```json
{"priority": 10}
```

误写成

```json
{"priorty": 10}
```

如果框架静默忽略未知字段，任务仍然创建成功，但使用默认优先级 5。

调用方以为传入了高优先级，服务端以为没有传。

这种“看起来成功”的错误往往比直接报错更难发现。

所以对内部企业 API、配置 API 和治理 API，`extra="forbid"` 通常更安全。

但如果已有大量客户端依赖宽松行为，开启前需要评估兼容性。

### 1.3.4 为什么 Pydantic 默认会做类型转换

HTTP 请求中的路径和查询参数本质上常常是字符串。

例如

```http
GET /tasks?limit=20
```

`20` 从协议层读出来是文本，但业务代码希望得到整数。

因此 Pydantic 默认允许一部分合理转换。

这让常规 API 更方便，但也可能掩盖输入错误。

对金额、权限、优先级等敏感字段，可以使用严格模式

```python
priority: int = Field(ge=1, le=10, strict=True)
```

设计取舍是

```text
宽松模式 → 调用方便，但可能接受意外输入
严格模式 → 契约明确，但升级时可能影响旧客户端
```

不是所有字段都必须全局严格，要根据业务风险选择。

### 1.3.5 为什么输入模型和输出模型要分开

假设数据库中的任务记录包含

```text
id
owner_id
instruction
status
internal_policy_result
secret_tool_token
```

客户端创建任务时不应该提交 `owner_id` 和 `status`。

服务端返回任务时也不应该暴露内部策略结果和密钥。

因此分成

```python
class TaskCreate(BaseModel):
    instruction: str


class TaskResponse(BaseModel):
    id: str
    instruction: str
    status: str
```

三种模型的边界

```text
TaskCreate       客户端允许提供什么
Task             系统内部完整状态是什么
TaskResponse     客户端允许看到什么
```

这不是重复定义，而是三个不同的安全边界。

### 1.3.6 `response_model` 为什么重要

```python
@app.get("/api/tasks/{task_id}", response_model=TaskResponse)
async def get_task(task_id: str) -> dict:
    return {
        "id": task_id,
        "instruction": "分析订单",
        "status": "running",
        "secret_tool_token": "do-not-leak",
    }
```

FastAPI 会按照 `TaskResponse` 过滤和序列化返回值。

客户端不会得到 `secret_tool_token`。

因此响应模型同时服务于

- 输出格式校验
- JSON 序列化
- OpenAPI 文档
- 字段白名单

不要直接把 ORM 模型当作公开 API 契约。

数据库以后新增字段时，可能造成意外泄露。

### 1.3.7 为什么校验错误通常是 422

FastAPI 请求数据校验失败时通常返回 `422 Unprocessable Entity`。

它表示

```text
HTTP 请求能够被解析
但请求数据不满足接口声明
```

例如

- 必填字段缺失
- 字段类型不正确
- 字符串为空
- 数字超出范围
- 出现不允许的额外字段

业务状态冲突不应都返回 422。

例如“任务已经结束，不能再次取消”更适合 `409 Conflict`。

### 1.3.8 Python 类型注解本身会校验请求吗？

**参考回答**

不会。Python 类型注解本身主要提供类型信息，不会自动阻止错误输入。FastAPI 读取函数签名和模型声明，再调用 Pydantic 执行运行时校验、转换和序列化。

**面试官追问**

为什么还要定义响应模型？

响应也是 API 边界。响应模型可以验证和过滤输出，避免内部字段泄露，并保证文档与真实响应结构一致。

## 1.4 为什么需要依赖注入：公共准备工作不能到处复制

### 1.4.1 没有依赖注入会怎样

任务查询、取消、审批和 Trace 接口都需要识别当前用户。

最直接的写法是每个接口重复

```python
@app.get("/api/tasks/{task_id}")
async def get_task(task_id: str, authorization: str | None = Header()):
    token = parse_bearer_token(authorization)
    payload = verify_token(token)
    user = await load_user(payload["sub"])
    if user.disabled:
        raise HTTPException(status_code=403)
    # 查询任务……
```

取消接口再写一遍，审批接口继续写一遍。

很快会出现

- 某些接口忘记检查用户状态
- 某些接口还在使用旧 Token 校验逻辑
- 修改认证规则时需要改很多位置
- 测试每个接口都要准备一大段身份逻辑

真正的问题不是代码行数，而是安全规则无法保持一致。

### 1.4.2 依赖注入是什么

依赖注入的核心思想是：函数只声明自己需要什么，由框架负责准备并传入。

```python
from typing import Annotated

from fastapi import Depends, Header, HTTPException
from pydantic import BaseModel


class UserContext(BaseModel):
    user_id: str
    role: str


async def get_current_user(
    authorization: Annotated[str | None, Header()] = None,
) -> UserContext:
    if authorization is None:
        raise HTTPException(status_code=401, detail="missing credential")

    # 省略真实 Token 校验，只展示依赖的结构。
    return UserContext(user_id="user-001", role="analyst")


CurrentUser = Annotated[UserContext, Depends(get_current_user)]


@app.get("/api/tasks")
async def list_tasks(user: CurrentUser) -> dict:
    return {"owner_id": user.user_id}
```

请求到来后

```text
FastAPI 发现 list_tasks 需要 CurrentUser
        ↓
先调用 get_current_user
        ↓
失败：立即返回 401/403
        ↓
成功：把 UserContext 传给 list_tasks
```

接口函数不再关心 Token 是 JWT、Session 还是网关签发的内部凭证。

它只依赖一个已经确认的 `UserContext`。

### 1.4.3 为什么不在接口里主动调用依赖函数

当然可以手写

```python
user = await get_current_user(authorization)
```

但 `Depends` 额外提供

- 自动从请求中解析依赖所需参数
- 依赖还可以继续依赖其他依赖
- 同一次请求内复用依赖结果
- 把认证要求写入 OpenAPI
- 测试时统一替换依赖

例如

```text
路径操作函数
├── 当前用户
│   ├── Token 解析器
│   └── 用户仓库
├── TaskService
│   ├── 数据库 Session
│   └── Event Publisher
└── Request Context
```

FastAPI 会先解决底层依赖，再调用上层依赖和接口函数。

这是一棵依赖图，不是简单的全局变量查找。

### 1.4.4 为什么数据库 Session 常用 `yield` 依赖

数据库 Session 需要做到

```text
请求开始时创建
请求处理中使用
请求结束后关闭
出现异常也必须释放
```

可以使用

```python
from collections.abc import AsyncIterator
from typing import Annotated

from fastapi import Depends


async def get_session() -> AsyncIterator["AsyncSession"]:
    async with session_factory() as session:
        yield session


SessionDependency = Annotated["AsyncSession", Depends(get_session)]
```

可以把 `yield` 理解为一条分界线

```text
yield 前：获取资源
yield 值：交给路径操作函数
yield 后：清理资源
```

`async with` 即使遇到异常也会关闭 Session。

这是“资源生命周期跟随请求”的设计。

连接池本身通常是应用级资源，Session/事务通常是一次请求或一次业务用例的资源。

不要把同一个 Session 并发共享给多个请求。

### 1.4.5 依赖注入不能代替业务分层

适合放进依赖的内容

- 当前用户
- 数据库 Session
- 配置
- Service 构造
- 公共权限前置检查

不适合放进依赖的内容

- 完整创建任务流程
- 多步骤状态机
- 大量数据库写操作
- 隐蔽的模型调用
- 为了“使用 Depends”而包装所有函数

依赖注入用于准备调用条件，不应该让业务流程变成看不见的魔法。

### 1.4.6 FastAPI 的 `Depends` 是怎样工作的？

**参考回答**

FastAPI 根据路径操作函数和 `Depends` 声明建立依赖图。请求到来后，它先解析子依赖，再执行上层依赖，把返回结果注入路径操作函数。依赖可以是同步或异步函数，也可以通过 `yield` 管理请求级资源。测试时还可以用 `dependency_overrides` 替换依赖。

**面试官追问**

数据库连接池和 Session 都应该每个请求创建吗？

通常不是。连接池是进程级复用资源，Session 和事务通常是请求级或用例级资源。

## 1.5 为什么需要异步：一个线程不应傻等 I/O

### 1.5.1 请求的大部分时间可能都在等待

一个 Agent API 可能执行

```text
查询 PostgreSQL       等待网络和数据库
读取 Redis            等待网络
调用模型 API          等待网络和模型生成
调用企业 Tool         等待网络和下游系统
```

CPU 真正执行 Python 代码的时间可能不长，大量时间是在等待外部资源。

如果一个线程等待模型返回时什么都不做，资源利用率很低。

异步编程允许它在等待期间处理其他请求。

### 1.5.2 用餐厅服务员理解事件循环

同步阻塞像这样

```text
服务员给 A 桌点菜
→ 站在厨房门口等菜做好
→ 菜做好后才去服务 B 桌
```

异步像这样

```text
服务员给 A 桌点菜
→ 厨房做菜期间去服务 B 桌
→ 厨房通知 A 桌菜好了
→ 再回来继续处理 A 桌
```

服务员没有同时切菜和点菜。

他只是没有把等待时间浪费掉。

这说明：异步擅长处理 I/O 等待，不等于 CPU 并行。

### 1.5.3 `await` 做了什么

```python
@app.get("/model-result")
async def model_result() -> dict:
    result = await model_client.generate("hello")
    return {"result": result}
```

执行到 `await` 时

```text
当前协程暂时挂起
       ↓
控制权交回事件循环
       ↓
事件循环处理其他可运行任务
       ↓
模型结果到达
       ↓
恢复当前协程
```

`await` 不是“让函数更快”。

它是“等待时允许别人先运行”。

### 1.5.4 为什么 `async def` 中不能调用阻塞函数

错误示例

```python
import time


@app.get("/bad")
async def bad() -> dict:
    time.sleep(5)
    return {"ok": True}
```

`time.sleep(5)` 不会把控制权交还事件循环。

这五秒内，事件循环线程被卡住，其他协程也会受到影响。

正确的异步等待

```python
import asyncio


@app.get("/good")
async def good() -> dict:
    await asyncio.sleep(5)
    return {"ok": True}
```

真实项目中应选择支持异步的库，例如异步 HTTP Client 和异步数据库驱动。

### 1.5.5 普通 `def` 路由为什么还能工作

如果路径操作函数是普通 `def`，FastAPI/Starlette 通常会在线程池中执行它，避免直接阻塞事件循环。

```python
@app.get("/legacy-system")
def call_legacy_system() -> dict:
    result = blocking_client.call()
    return {"result": result}
```

这让同步库可以接入异步应用。

但线程池不是无限的。

大量慢同步调用会耗尽线程池，后续请求仍然需要排队。

所以线程池是兼容手段，不是无限并发方案。

### 1.5.6 CPU 密集任务为什么不能只靠异步

例如

- 本地大模型推理
- 大型矩阵计算
- 图像处理
- 大量压缩和加密计算

这些任务不是在等待，而是在持续使用 CPU/GPU。

即使写成 `async def`，计算期间也不会自动让出 CPU。

通常需要

- 多进程
- 进程池
- 独立 Worker
- 专门的模型推理服务

### 1.5.7 一张表做选择

|           场景            |         常见选择          |        原因        |
| :---------------------: | :-------------------: | :--------------: |
| 异步数据库、Redis、HTTP Client | `async def` + `await` |    等待时让出事件循环     |
|      必须使用同步阻塞 SDK       |    普通 `def` 或受控线程池    |      隔离事件循环      |
|         轻量内存计算          |   可直接放在 `async def`   |   执行很快，不值得切线程    |
|        CPU 密集计算         |        进程或专用服务        |   需要真正并行或资源隔离    |
|      长时间 Agent 工作流      |    持久化队列 + Worker     | 生命周期不应绑定 HTTP 请求 |

### 1.5.8 ASGI 为什么适合异步连接

FastAPI 底层遵守 ASGI 规范。

ASGI 应用可简化为

```python
async def application(scope, receive, send):
    ...
```

- `scope` 描述连接
- `receive` 接收事件
- `send` 发送事件

这种事件模型能够自然表达普通 HTTP、流式响应和 WebSocket。

初学阶段不需要手写 ASGI 应用，但理解这一层有助于解释 FastAPI 为什么能够支持异步和长连接。

### 1.5.9 FastAPI 为什么快？

**参考回答**

FastAPI 基于 ASGI 和 Starlette，I/O 等待期间事件循环可以处理其他连接；Uvicorn 负责运行 ASGI 应用；Pydantic 负责高效的数据校验和序列化。但框架开销通常不是整个业务的主要瓶颈，数据库、模型、下游 API、阻塞调用和连接池往往更重要。

**容易答错的地方**

- 认为 `async def` 会自动创建新线程
- 认为异步可以加速 CPU 计算
- 认为只要用了 FastAPI 就一定高并发

## 1.6 为什么路由不能承载所有业务逻辑

### 1.6.1 “能运行”不等于“结构合理”

初学者很容易写出

```python
@app.post("/api/tasks")
async def create_task(body: TaskCreate, user: CurrentUser):
    # 检查业务权限
    # 查询数据库
    # 创建事务
    # 调用模型
    # 发送消息
    # 更新状态
    # 写审计日志
    # 处理所有异常
    ...
```

代码少时很直接，业务变复杂后会出现

- 路由函数几百行
- HTTP 细节和业务规则混在一起
- Worker 无法复用创建任务逻辑
- 单元测试必须模拟整个 Web 请求
- 事务边界不清楚
- 修改一个流程影响很多接口

### 1.6.2 Router、Service、Repository 分别是什么

可以把后端分成三层

```text
Router
接收 HTTP 请求，转换协议
       ↓
Service
执行一个完整业务用例
       ↓
Repository
读写数据库
```

**Router：负责协议**

它应该关心

- URL 和 HTTP 方法
- 请求模型
- 当前用户
- HTTP 状态码
- 响应模型
- 业务异常如何映射为 HTTP 错误

**Service：负责业务用例**

它应该关心

- 当前状态能否执行动作
- 需要调用哪些 Repository
- 需要发布什么事件
- 事务在哪里开始和结束
- 失败时怎样回滚或转换错误

**Repository：负责数据访问**

它应该关心

- SQL 或 ORM 查询
- 如何保存和读取任务
- 如何做条件更新
- 如何隐藏具体数据库实现

### 1.6.3 分层后的代码是什么样

Router

```python
@router.post("", response_model=TaskResponse, status_code=202)
async def create_task(
    body: TaskCreate,
    user: CurrentUser,
    service: TaskServiceDependency,
) -> TaskResponse:
    return await service.create_task(body=body, user=user)
```

Service

```python
class TaskService:
    def __init__(self, repository, publisher):
        self.repository = repository
        self.publisher = publisher

    async def create_task(self, body, user):
        task = Task.create(
            owner_id=user.user_id,
            instruction=body.instruction,
        )
        await self.repository.add(task)
        await self.publisher.publish_task_created(task.id)
        return TaskResponse.from_task(task)
```

这样设计不是为了“套架构模板”。

它解决的是依赖方向

```text
HTTP 可以调用业务逻辑
Worker 也可以调用同一业务逻辑
业务逻辑不必知道 FastAPI Request 是什么
```

### 1.6.4 为什么业务层不应该抛 `HTTPException`

如果 `TaskService` 直接抛 `HTTPException`，它就知道自己正在被 HTTP API 调用。

以后 CLI、Worker 或定时任务复用它时，也被迫依赖 FastAPI。

业务层只应该表达“发生了什么业务问题”，不应该决定“这个问题在 HTTP 协议里要变成什么状态码”。

更合理的方式

```python
class TaskNotFoundError(Exception):
    pass
```

Service 抛业务异常

```python
raise TaskNotFoundError(task_id)
```

API 层转换

```python
@app.exception_handler(TaskNotFoundError)
async def handle_task_not_found(request, exc):
    return JSONResponse(
        status_code=404,
        content={"code": "TASK_NOT_FOUND"},
    )
```

这使业务语义和传输协议解耦。

### 1.6.5 `APIRouter` 为什么不只是拆文件

```python
router = APIRouter(
    prefix="/api/tasks",  # 公共 URL 前缀
	tags=["tasks"],       # 用于 OpenAPI / Swagger API 文档分组
)
```

`APIRouter` 可以围绕一个业务领域聚合

- 路由前缀
- 文档标签
- 公共依赖
- 错误响应
- API 版本

它让应用按业务模块组合，而不是把所有接口注册在一个 `main.py` 中。

### 1.6.6 Lifespan 为什么用于应用级资源

HTTP Client、数据库连接池和 Redis Client 建立连接有成本。

如果每个请求都重新创建

```text
请求到来
→ 新建连接池
→ 执行业务
→ 关闭连接池
```

会浪费连接和延迟。

这些资源应在应用启动时创建，在应用关闭时释放

```python
from collections.abc import AsyncIterator
from contextlib import asynccontextmanager

from fastapi import FastAPI


@asynccontextmanager
async def lifespan(app: FastAPI) -> AsyncIterator[None]:
    app.state.http_client = await create_http_client()
    app.state.redis = await create_redis_client()
    try:
        yield
    finally:
        await app.state.redis.aclose()
        await app.state.http_client.aclose()


app = FastAPI(lifespan=lifespan)
```

生命周期关系

```text
应用级：连接池、Client、只读配置
请求级：Session、事务、当前用户、request_id
任务级：task_id、状态、Trace、取消标记
```

不同生命周期的对象不应混在一起。

### 1.6.7 为什么要把 Router 和 Service 分开？

**参考回答**

Router 负责 HTTP 协议，Service 负责业务用例。分开后业务逻辑可以被 API、Worker 和 CLI 复用，也能在不启动 Web Server 的情况下单元测试。HTTP 状态码和 `HTTPException` 留在 API 层，领域错误和事务规则留在业务层。

## 1.7 为什么 Agent 长任务必须脱离 HTTP 请求

### 1.7.1 HTTP 请求和 Agent 任务的生命周期不同

一个普通查询接口可能几十毫秒完成

```text
请求 → 查询数据库 → 返回
```

一个 Agent 任务可能经历

```text
任务分类
→ 检索 Skill
→ 制定计划
→ 多次调用模型
→ 查询 SQL / RAG / Logs
→ 调用外部 Tool
→ 等待人工审批
→ 检查结果
→ 保存 Trajectory
```

它可能运行几分钟，甚至因为审批暂停数小时。

如果一直占住最初的 HTTP 请求，会遇到

- 浏览器、网关或负载均衡器超时
- 客户端断线后不知道任务是否仍在执行
- Web 进程重启时任务丢失
- API Worker 被长任务占满
- 无法独立扩容 Agent 执行能力
- 很难恢复等待审批的任务

本质问题是

```text
HTTP 请求生命周期：通常短、连接驱动
Agent 任务生命周期：可能很长、状态驱动
```

两者不应该强行绑定。

### 1.7.2 为什么返回 `202 Accepted`

合理的任务提交接口

```text
POST /api/tasks
       ↓
校验输入
       ↓
认证与权限检查
       ↓
保存 PENDING 任务
       ↓
投递任务消息
       ↓
202 Accepted + task_id
```

`202` 的准确含义是：服务端已经接受请求，但处理尚未完成。

它不表示

- Agent 已经执行成功
- 消息一定不会丢
- 最终结果一定可用

客户端拿到 `task_id` 后，通过另一个接口查询最终状态

```http
GET /api/tasks/task-001
```

### 1.7.3 为什么需要任务队列和 Worker

队列把“接受请求”和“执行任务”分开

```text
FastAPI API Worker
  短时间完成校验、保存、投递
             ↓
        Durable Queue
             ↓
Agent Worker
  执行模型、工具、重试和状态更新
```

这样可以

- API 和 Agent 独立扩容
- Agent Worker 重启后恢复任务
- 对任务做重试、优先级和死信处理
- 控制模型和 Tool 的并发量
- 避免离线演进任务拖慢在线 API

队列不是为了“显得架构复杂”。

它解决的是生命周期、可靠性和资源隔离问题。

### 1.7.4 为什么 `BackgroundTasks` 不等于任务队列

FastAPI 的 `BackgroundTasks` 可以在响应返回后继续执行函数

```python
from fastapi import BackgroundTasks


@app.post("/notify")
async def notify(background_tasks: BackgroundTasks) -> dict:
    background_tasks.add_task(send_email, "user@example.com")
    return {"accepted": True}
```

它适合

- 短小的非关键后处理
- 与当前进程共享少量数据的工作
- 失败后允许简单记录或忽略的任务

核心 Agent 任务通常需要

- 持久化
- 多进程或多机器消费
- 重试与死信
- 租约与超时
- 任务恢复
- 独立扩缩容

这些不是 `BackgroundTasks` 的职责。

可以用一句话区分

```text
BackgroundTasks：响应以后继续做一点事
任务队列：把一项业务工作可靠地交给另一个执行单元
```

### 1.7.5 为什么任务状态要用状态机

只保存

```json
{"done": false}
```

无法表达任务正在运行、等待审批、失败还是被取消。

更合理的状态

```text
PENDING
   ↓
RUNNING
   ├──→ WAITING_APPROVAL ──→ RUNNING
   ├──→ SUCCEEDED
   ├──→ FAILED
   └──→ CANCELLED
```

状态机的价值不是多几个枚举值，而是限制非法迁移。

例如

```text
SUCCEEDED → RUNNING      不允许
CANCELLED → SUCCEEDED    通常不允许
WAITING_APPROVAL → RUNNING
                        只有审批通过才允许
```

多个 Worker 或用户请求同时更新时，应使用数据库条件更新或版本号

```sql
UPDATE tasks
SET status = 'RUNNING', version = version + 1
WHERE id = :task_id
  AND status = 'PENDING'
  AND version = :expected_version;
```

影响行数为 0，说明任务状态已经被其他执行者修改。

### 1.7.6 为什么取消通常是“请求取消”

用户调用

```http
POST /api/tasks/task-001/cancel
```

服务端通常先记录

```text
cancel_requested = true
```

Worker 在安全点检查

- 调用模型前
- 调用 Tool 前
- 每个 LangGraph 节点结束后
- 开始重试前
- 等待审批时

为什么不直接“杀死任务”？

因为任务可能正在执行外部副作用。

例如邮件已经发出、工单已经创建，强行停止本地协程不能撤销外部动作。

因此要区分

```text
停止后续步骤
取消可取消的下游请求
对已经发生的副作用执行补偿
记录无法撤销的部分结果
```

### 1.7.7 轮询、SSE、WebSocket 为什么都存在

任务在 Worker 中运行后，客户端需要观察进度。

**轮询**

```text
客户端每隔 2 秒：GET /api/tasks/{id}
```

优点：简单、稳定、容易经过代理。

缺点：实时性有限，可能产生空查询。

**SSE**

```text
客户端建立一个 HTTP 长连接
服务端不断向客户端推送单向事件
```

适合任务进度、Trace 和模型 Token 流。

**WebSocket**

```text
客户端和服务端在长连接上双向发送消息
```

适合强交互式会话。

为什么初学项目通常先做轮询？

因为任务可靠性来自持久化状态，而不是来自长连接。

先保证 `GET /tasks/{id}` 返回正确状态，再增加 SSE 改善体验，系统更容易调试。

SSE 断开也不应该自动取消任务。

观察连接和业务任务是两个不同生命周期。

### 1.7.8 为什么创建任务还要考虑幂等

客户端提交请求后，响应可能在网络中丢失。

客户端不知道服务端是否创建成功，于是重试

```text
第一次请求：服务端成功创建 task-001，响应丢失
第二次请求：服务端又创建 task-002
```

同一业务意图产生两个任务，可能导致重复 Tool 调用和重复成本。

客户端可以发送

```http
Idempotency-Key: request-20260829-001
```

服务端保存

```text
tenant/user + endpoint + idempotency_key
→ request_fingerprint + task_id
```

相同 Key 和相同请求再次出现时，返回原任务。

相同 Key 对应不同请求时，返回冲突。

生产中应依赖数据库唯一约束和事务，而不是只使用进程内字典。

### 1.7.9 为什么“保存数据库后发送消息”仍然可能出错

代码看起来很自然

```python
await repository.add(task)
await queue.publish(task.id)
```

如果第一行成功、第二行之前进程崩溃

```text
数据库里有 PENDING 任务
队列里没有消息
任务永远没人执行
```

这叫数据库与消息队列的双写一致性问题。

常见解决思路是 Transactional Outbox

```text
同一个数据库事务
├── 写 tasks
└── 写 outbox_events
        ↓
独立 Relay 读取 Outbox
        ↓
发布到消息队列
        ↓
消费者按 event_id / task_id 幂等处理
```

初学版可以暂时不实现，但面试时要知道：`202` 之前保存了任务并不自动证明消息可靠投递。

### 1.7.10 为什么 Agent 长任务不直接在 FastAPI 路由中执行？

**参考回答**

HTTP 请求通常是短连接生命周期，而 Agent 任务可能运行很久、等待审批或需要重试恢复。把任务放在路由中会受到网关超时、客户端断线和 Web 进程重启影响，也无法独立扩缩容。更合理的方式是 API 持久化任务并投递队列，返回 `202 + task_id`，由独立 Worker 执行。

**面试官追问**

数据库写成功、消息发布失败怎么办？

可以使用 Transactional Outbox，把任务记录和待发布事件写入同一数据库事务，再由 Relay 发布；消费者仍要保证幂等。

## 1.8 生产设计不是功能清单，而是在处理失败

开发环境中，一次请求通常很顺利

```text
请求正确
用户合法
数据库可用
模型及时返回
进程不重启
```

生产设计要假设这些条件都会被破坏。

下面按“会出什么问题”理解常见机制。

### 1.8.1 客户端可能不是它声称的用户：需要认证

认证回答

```text
你是谁？
```

客户端传来的

```http
X-User-Id: admin
```

不能作为可信身份，因为任何人都能伪造。

真实系统通常验证网关凭证、OAuth2/OIDC Token 或服务身份。

如果使用 JWT，至少要验证

- 签名算法
- 过期时间 `exp`
- 签发方 `iss`
- 受众 `aud`
- 主体 `sub`
- 必要的吊销或会话状态

JWT 通常是签名，不等于内容加密。

不要把密码和密钥放进 Payload。

### 1.8.2 用户身份合法，但不一定能访问该资源：需要授权

授权回答

```text
你能做什么？
```

资源范围回答

```text
你能对哪些具体资源做？
```

例如 Analyst 可以执行 SQL 查询，不等于可以查询所有租户和所有表。

权限判断至少包含

```text
subject    谁
action     做什么
resource   对哪个资源
context    在什么环境和风险级别下
```

在 Agent 系统中，API 入口检查还不够。

因为 Agent 最终生成的 Tool 参数可能和最初计划不同。

真正产生副作用前应再次检查

```text
API 粗粒度权限
      ↓
Agent 规划
      ↓
生成 Tool 参数
      ↓
Tool 执行前细粒度 Policy Check
```

### 1.8.3 用户有权限，但高风险动作仍需要确认：需要审批

审批回答

```text
这一次具体操作是否被批准？
```

Operator 可能有重启服务的权限，但生产环境重启仍要求人工审批。

所以

```text
Permission：原则上可不可以做
Approval：这一次是否批准做
```

审批不能让原本无权限的用户突然拥有权限。

权限也不能让高风险动作自动绕过审批。

### 1.8.4 下游可能永远不返回：需要超时

没有超时的调用可能长期占用

- HTTP 连接
- 数据库连接
- Worker 并发槽位
- 用户的总体时间预算

应为数据库、Redis、模型和 Tool 设置超时。

但超时不能只写一个统一的 30 秒。

更合理的是时间预算

```text
整个任务剩余 60 秒
  ├── 模型最多 30 秒
  ├── SQL 最多 10 秒
  ├── Tool 最多 10 秒
  └── 留出结果整理和持久化时间
```

### 1.8.5 暂时错误可能恢复：需要受控重试

适合重试的例子

- 临时网络错误
- 下游返回 503
- 限流后允许稍后重试

不适合直接重试的例子

- 参数不合法
- 权限不足
- 明确业务冲突
- 非幂等副作用是否已经执行无法确认

重试需要

```text
最大次数
+ 总时间预算
+ 指数退避
+ 随机抖动
+ 幂等保证
```

否则下游故障时会发生重试风暴。

### 1.8.6 请求可能超过系统容量：需要背压

如果每秒提交 100 个任务，而 Worker 每秒只能完成 10 个

```text
队列会持续增长
→ 等待时间越来越长
→ 内存或存储压力上升
→ 用户仍然不断提交
```

系统需要根据

- 租户
- 用户
- 任务类型
- 模型 Token
- Tool 并发
- 队列长度

实施限流、并发控制和准入策略。

只限制 API 的请求数不够。

一个状态查询和一次合规扫描的成本完全不同。

### 1.8.7 进程可能重启：共享状态必须放到进程外

启动多个 Uvicorn Worker 时，每个 Worker 是独立进程。

它们不共享

- Python 全局字典
- `app.state`
- 内存缓存
- `asyncio.Queue`

所以

```text
进程内状态
适合连接池句柄、Client、本地只读缓存

跨进程业务事实
放 PostgreSQL、Redis 或消息系统
```

每个 Worker 也会创建自己的连接池。

假设每个进程最多 20 个数据库连接，4 个进程就是最多 80 个。

Worker 数增加可能先压垮数据库，并不一定让系统更快。

### 1.8.8 系统可能失败但没人知道：需要可观测性

一条访问日志

```text
POST /api/tasks 202 35ms
```

只能说明任务提交接口很快。

它不能说明 Agent 最终是否成功。

需要关联

```text
request_id
   ↓
task_id
   ↓
trajectory_id
   ↓
agent_run_id / node_run_id
   ↓
tool_call_id
```

三类可观测信号

|   信号    |    回答的问题     |
| :-----: | :----------: |
|  Logs   |  具体发生了什么事件？  |
| Metrics |  整体趋势是否异常？   |
| Traces  | 时间花在调用链的哪一段？ |

生产中常看的指标

- API P50/P95/P99
- 错误率
- 数据库连接池等待
- 队列长度与最老任务年龄
- Agent 成功率
- 模型与 Tool 超时率
- 重试次数
- Token 和成本

日志中不能记录完整 Token、密码、API Key、未脱敏文档和敏感 Prompt。

### 1.8.9 为什么要区分存活和就绪检查

```text
/health/live
进程是否仍在运行

/health/ready
当前实例是否准备好接收流量
```

如果 liveness 强依赖数据库，数据库短暂故障可能导致所有 API 实例一起重启，进一步放大故障。

Readiness 可以用于临时摘除无法服务的实例，但检查也不能过于昂贵。

### 1.8.10 多 Uvicorn Worker 会共享全局变量吗？

**参考回答**

不会。Worker 通常是独立进程，各自拥有 Python 内存、`app.state`、连接池和本地缓存。共享任务状态与幂等记录应放在 PostgreSQL、Redis 等外部系统。连接池容量也要按 Worker 和实例总数估算。

## 1.9 把主线落成一个最小任务 API

下面的示例把前面的概念连接起来

```text
Pydantic 校验请求
        ↓
Depends 获取用户和 Service
        ↓
Router 调用 TaskService
        ↓
Service 保存任务并投递队列
        ↓
Lifespan 启动的 Worker 执行任务
        ↓
客户端查询状态
```

为了让代码能够单文件运行，示例使用内存字典和 `asyncio.Queue`。

它用于理解流程，不是生产实现。

### 1.9.1 完整代码

保存为 `main.py`

```python
from __future__ import annotations

import asyncio
from collections.abc import AsyncIterator
from contextlib import asynccontextmanager, suppress
from enum import Enum
from typing import Annotated, Any
from uuid import uuid4

from fastapi import (
    APIRouter,
    Depends,
    FastAPI,
    Header,
    HTTPException,
    Request,
    status,
)
from pydantic import BaseModel, ConfigDict, Field


# ---------- 1. API 契约 ----------


class TaskStatus(str, Enum):
    PENDING = "pending"
    RUNNING = "running"
    SUCCEEDED = "succeeded"
    FAILED = "failed"


class TaskCreate(BaseModel):
    model_config = ConfigDict(extra="forbid")

    task_type: str = Field(pattern=r"^[a-z][a-z0-9_]{2,63}$")
    instruction: str = Field(min_length=1, max_length=2000)
    priority: int = Field(default=5, ge=1, le=10, strict=True)


class TaskResponse(BaseModel):
    id: str
    owner_id: str
    task_type: str
    instruction: str
    priority: int
    status: TaskStatus
    result: dict[str, Any] | None = None
    error_code: str | None = None


class UserContext(BaseModel):
    user_id: str
    role: str


class TaskNotFoundError(Exception):
    pass


# ---------- 2. 数据访问 ----------


class InMemoryTaskStore:
    def __init__(self) -> None:
        self._tasks: dict[str, TaskResponse] = {}
        self._lock = asyncio.Lock()

    async def add(self, task: TaskResponse) -> TaskResponse:
        async with self._lock:
            self._tasks[task.id] = task.model_copy(deep=True)
            return task.model_copy(deep=True)

    async def get(self, task_id: str) -> TaskResponse | None:
        async with self._lock:
            task = self._tasks.get(task_id)
            return task.model_copy(deep=True) if task else None

    async def update(
        self,
        task_id: str,
        **changes: Any,
    ) -> TaskResponse:
        async with self._lock:
            task = self._tasks[task_id]
            updated = task.model_copy(update=changes, deep=True)
            self._tasks[task_id] = updated
            return updated.model_copy(deep=True)


# ---------- 3. 业务服务 ----------


class TaskService:
    def __init__(
        self,
        store: InMemoryTaskStore,
        queue: asyncio.Queue[str],
    ) -> None:
        self._store = store
        self._queue = queue

    async def submit(
        self,
        body: TaskCreate,
        user: UserContext,
    ) -> TaskResponse:
        task = TaskResponse(
            id=str(uuid4()),
            owner_id=user.user_id,
            task_type=body.task_type,
            instruction=body.instruction,
            priority=body.priority,
            status=TaskStatus.PENDING,
        )

        await self._store.add(task)
        await self._queue.put(task.id)
        return task

    async def get_visible(
        self,
        task_id: str,
        user: UserContext,
    ) -> TaskResponse:
        task = await self._store.get(task_id)

        if task is None or (
            task.owner_id != user.user_id and user.role != "admin"
        ):
            raise TaskNotFoundError(task_id)

        return task

    async def execute(self, task_id: str) -> None:
        await self._store.update(task_id, status=TaskStatus.RUNNING)

        try:
            # 模拟 Planner、Tool 和 Reviewer 等异步步骤。
            await asyncio.sleep(0.3)
            await self._store.update(
                task_id,
                status=TaskStatus.SUCCEEDED,
                result={"summary": "demo agent task completed"},
            )
        except Exception:
            await self._store.update(
                task_id,
                status=TaskStatus.FAILED,
                error_code="EXECUTION_FAILED",
            )
            raise


async def worker_loop(
    service: TaskService,
    queue: asyncio.Queue[str],
) -> None:
    while True:
        task_id = await queue.get()
        try:
            await service.execute(task_id)
        except Exception:
            # execute() 已经把任务更新为 FAILED。
            # 生产环境还应记录结构化日志并按策略决定是否重试。
            pass
        finally:
            queue.task_done()


# ---------- 4. FastAPI 依赖 ----------


async def get_current_user(
    x_user_id: Annotated[str | None, Header()] = None,
    x_role: Annotated[str, Header()] = "analyst",
) -> UserContext:
    # 只用于学习。生产环境必须验证可信 Token。
    if x_user_id is None:
        raise HTTPException(status_code=401, detail="missing credential")
    if x_role not in {"viewer", "analyst", "operator", "admin"}:
        raise HTTPException(status_code=403, detail="unknown role")
    return UserContext(user_id=x_user_id, role=x_role)


def get_task_service(request: Request) -> TaskService:
    return request.app.state.task_service


CurrentUser = Annotated[UserContext, Depends(get_current_user)]
TaskServiceDependency = Annotated[TaskService, Depends(get_task_service)]


# ---------- 5. HTTP 路由 ----------


router = APIRouter(prefix="/api/tasks", tags=["tasks"])


@router.post(
    "",
    response_model=TaskResponse,
    status_code=status.HTTP_202_ACCEPTED,
)
async def create_task(
    body: TaskCreate,
    user: CurrentUser,
    service: TaskServiceDependency,
) -> TaskResponse:
    return await service.submit(body, user)


@router.get("/{task_id}", response_model=TaskResponse)
async def get_task(
    task_id: str,
    user: CurrentUser,
    service: TaskServiceDependency,
) -> TaskResponse:
    try:
        return await service.get_visible(task_id, user)
    except TaskNotFoundError as exc:
        raise HTTPException(status_code=404, detail="task not found") from exc


# ---------- 6. 应用生命周期 ----------


@asynccontextmanager
async def lifespan(app: FastAPI) -> AsyncIterator[None]:
    queue: asyncio.Queue[str] = asyncio.Queue(maxsize=100)
    store = InMemoryTaskStore()
    service = TaskService(store, queue)
    worker = asyncio.create_task(worker_loop(service, queue))

    app.state.task_service = service

    try:
        yield
    finally:
        worker.cancel()
        with suppress(asyncio.CancelledError):
            await worker


def create_app() -> FastAPI:
    application = FastAPI(
        title="EvoAgent Task API Demo",
        lifespan=lifespan,
    )
    application.include_router(router)
    return application


app = create_app()
```

安装与运行

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install "fastapi[standard]"
uvicorn main:app --reload
```

提交任务

```bash
curl -X POST 'http://127.0.0.1:8000/api/tasks' \
  -H 'Content-Type: application/json' \
  -H 'X-User-Id: user-001' \
  -H 'X-Role: analyst' \
  -d '{
    "task_type": "data_analysis",
    "instruction": "分析最近七天订单失败原因",
    "priority": 5
  }'
```

查询任务

```bash
curl -H 'X-User-Id: user-001' \
  'http://127.0.0.1:8000/api/tasks/<task_id>'
```

### 1.9.2 按请求流程理解代码，而不是逐行背诵

**第一步：`TaskCreate` 建立输入边界**

```text
不可信 JSON
→ Pydantic 校验
→ 可信的 TaskCreate 对象
```

额外字段会被拒绝，指令不能为空，优先级必须在范围内。

**第二步：依赖准备当前用户和 Service**

```text
get_current_user
→ 形成 UserContext

get_task_service
→ 从 app.state 获取进程内 Service
```

路由只声明自己需要这两个对象。

**第三步：Router 把 HTTP 请求交给 Service**

```python
return await service.submit(body, user)
```

路由不知道任务怎样保存，也不知道 Worker 怎样执行。

它只负责协议转换。

查询任务时，Service 抛出 `TaskNotFoundError` 这一业务异常，Router 再把它转换成 HTTP `404`。这样 Service 不依赖 HTTP 协议，也可以被 Worker 或 CLI 复用。

**第四步：Service 先保存，再投递**

```python
await self._store.add(task)
await self._queue.put(task.id)
```

先保存是为了让客户端拿到 `task_id` 后可以查询到任务。

但如前所述，生产中这两步存在双写一致性问题，需要 Outbox 等方案。

**第五步：Worker 从队列取任务**

```python
task_id = await queue.get()
```

队列为空时协程会等待，不会一直循环占满 CPU。

取到任务后调用 `service.execute()`，把状态从 `PENDING` 更新到 `RUNNING`，最后进入终态。

**第六步：Lifespan 管理 Worker**

应用启动时

```text
创建 Store
→ 创建 Queue
→ 创建 Service
→ 启动 Worker 协程
```

应用关闭时

```text
取消 Worker
→ 等待 Worker 退出
```

这展示了应用级资源的创建和清理位置。

### 1.9.3 为什么这个示例仍然不是生产实现

|      示例设计       |     生产问题     |              生产替换              |
| :-------------: | :----------: | :----------------------------: |
|      内存字典       | 重启丢失，多进程不共享  |           PostgreSQL           |
| `asyncio.Queue` | 消息不持久化，不能跨机器 | Redis Streams、RabbitMQ、Kafka 等 |
|     请求头模拟用户     |   客户端可以伪造    |        OIDC/JWT/可信网关身份         |
|     直接覆盖状态      |   并发更新可能丢失   |            条件更新或乐观锁            |
|     保存后直接入队     | 进程崩溃产生双写不一致  |      Transactional Outbox      |
|   单 Worker 协程   |  无租约、重试、死信   |         独立任务 Worker 系统         |

最小示例的价值是解释控制流，而不是假装已经具备生产可靠性。

## 1.10 怎样验证这套设计

### 1.10.1 为什么不能只在 `/docs` 手动点击

手动测试适合快速体验，但无法稳定覆盖

- 输入边界
- 权限分支
- 状态冲突
- 重复提交
- Worker 失败
- 进程重启
- 并发更新

自动化测试把预期行为固定下来，防止后续修改破坏已有契约。

### 1.10.2 最小 API 测试

保存为 `test_main.py`

```python
import time

from fastapi.testclient import TestClient

from main import create_app


HEADERS = {
    "X-User-Id": "test-user",
    "X-Role": "analyst",
}


def test_rejects_invalid_instruction() -> None:
    with TestClient(create_app()) as client:
        response = client.post(
            "/api/tasks",
            headers=HEADERS,
            json={
                "task_type": "data_analysis",
                "instruction": "",
                "priority": 5,
            },
        )

        assert response.status_code == 422


def test_hides_another_users_task() -> None:
    with TestClient(create_app()) as client:
        created = client.post(
            "/api/tasks",
            headers=HEADERS,
            json={
                "task_type": "data_analysis",
                "instruction": "analyze orders",
            },
        )
        task_id = created.json()["id"]

        response = client.get(
            f"/api/tasks/{task_id}",
            headers={"X-User-Id": "another-user"},
        )

        assert response.status_code == 404


def test_task_eventually_succeeds() -> None:
    with TestClient(create_app()) as client:
        created = client.post(
            "/api/tasks",
            headers=HEADERS,
            json={
                "task_type": "data_analysis",
                "instruction": "analyze orders",
            },
        )

        assert created.status_code == 202
        task_id = created.json()["id"]

        deadline = time.monotonic() + 2
        while time.monotonic() < deadline:
            found = client.get(
                f"/api/tasks/{task_id}",
                headers=HEADERS,
            )
            assert found.status_code == 200

            if found.json()["status"] == "succeeded":
                break

            time.sleep(0.05)
        else:
            raise AssertionError("task did not finish before deadline")
```

使用

```text
with TestClient(create_app()) as client:
```

是为了让测试正确进入和退出应用 Lifespan，保证 Worker 被启动和清理。

### 1.10.3 为什么要分不同层次测试

**Schema Test**

验证

- 字段缺失
- 长度限制
- 额外字段
- 序列化结构

**Service Unit Test**

验证

- 状态迁移
- 业务权限
- 幂等
- 取消
- 异常分支

这层可以使用 Fake Repository，不需要启动 FastAPI。

**API Contract Test**

验证

- URL
- 状态码
- Header
- 请求和响应格式
- 依赖是否正确连接

**Integration Test**

使用真实 PostgreSQL、Redis 或队列，验证

- SQL
- 事务
- 唯一约束
- 消息投递
- 连接和序列化行为

**End-to-End Test**

验证

```text
API → Queue → Worker → Agent → Trace → Result
```

全部使用 Mock 的测试不能证明真实事务和消息系统一定正确。

### 1.10.4 性能测试为什么不能只看平均值

平均延迟可能很好，但少量请求非常慢。

生产中通常同时观察

- P50
- P95
- P99
- 吞吐量
- 错误率
- CPU 和内存
- 事件循环延迟
- 数据库连接池等待
- 队列积压

优化前后还要固定数据集、并发量、机器规格、超时和重试策略。

否则无法判断变化来自代码，还是来自测试条件。

## 1.11 面试问题

面试官通常不是只想听定义。

一个高质量回答应该包含

```text
它是什么
→ 它在解决什么问题
→ 它怎样工作
→ 它的边界和代价是什么
→ 在项目中怎样使用
```

### 1.11.1 FastAPI 是什么，为什么选择它

**参考回答**

FastAPI 是一个基于 ASGI 和 Starlette 的 Python API 框架，通过 Python 类型注解和 Pydantic 声明请求、响应与校验规则，并自动生成 OpenAPI。它适合类型契约清晰、I/O 调用较多的 API 服务。

选择它不能只因为“性能高”，还因为项目需要请求校验、依赖注入、异步模型/数据库调用和自动接口文档。但它不提供完整 ORM、可靠任务队列或业务权限系统，这些仍需单独设计。

**面试官追问**

- FastAPI 为什么不是整个 Agent Runtime？

FastAPI 负责 HTTP 接入与协议边界；Agent 的计划、状态机、Tool 调用、执行恢复和 Skill 演进属于业务运行时与 Worker。

**容易答错的地方**

把“使用 FastAPI、RAG、LangGraph”当成系统架构本身，没有说明组件边界。

### 1.11.2 为什么 FastAPI 使用 Pydantic，而不是只使用 `dict`

**参考回答**

外部输入不可信，直接使用字典会把字段存在性、类型、范围和嵌套校验分散到业务代码中。Pydantic 把这些约束集中成模型，FastAPI 再用同一模型做运行时校验、类型提示、JSON Schema 和 OpenAPI。响应模型还能够过滤内部字段，形成输出白名单。

**面试官追问**

- 为什么输入模型和输出模型不要共用 ORM 模型？

因为客户端可提交字段、内部完整状态和允许公开字段是不同边界。ORM 新增敏感字段时，直接暴露 ORM 还可能造成数据泄露。

### 1.11.3 `Depends` 解决了什么问题

**参考回答**

`Depends` 用来声明路径操作函数运行前所需的公共依赖，例如当前用户、数据库 Session 和 Service。FastAPI 会解析依赖图、执行子依赖并注入结果。这避免认证和资源准备逻辑在每个接口重复，也便于测试替换。

**面试官追问**

- 依赖注入是否等于把业务流程都写进依赖？

不是。依赖主要准备调用条件；业务用例仍应放在 Service 中，否则流程会隐藏在复杂依赖图里。

### 1.11.4 `async def` 为什么可以提高并发

**参考回答**

当协程等待数据库、HTTP 或模型 API 时，`await` 会把控制权交回事件循环，让同一线程处理其他可运行任务。因此它提高的是 I/O 等待场景下的资源利用率。它不会让 CPU 密集计算自动并行，也不能修复 `async def` 内部的同步阻塞调用。

**面试官追问**

- 普通 `def` 路由怎样执行？

FastAPI/Starlette 通常把同步路径操作函数放入线程池，避免直接阻塞事件循环。但线程池容量有限，只是兼容同步代码的方式。

### 1.11.5 为什么 Agent 任务返回 `202` 而不是等待结果

**参考回答**

Agent 任务可能多次调用模型和 Tool，甚至等待人工审批，它的生命周期远长于普通 HTTP 请求。API 应在完成校验、鉴权、持久化和可靠投递后返回 `202 + task_id`，由独立 Worker 执行，客户端通过状态接口或事件流观察。

**面试官追问**

- `202` 是否代表任务一定成功？

不代表。它只表示已接受处理，最终可能成功、失败或取消。

### 1.11.6 `BackgroundTasks` 和消息队列怎样选择

**参考回答**

`BackgroundTasks` 在当前 Web 进程内、响应返回后执行，适合短小且非关键的后处理。核心 Agent 任务要求重启恢复、多机消费、重试、死信、租约和独立扩缩容，应使用持久化队列与独立 Worker。选择依据是可靠性和生命周期，而不只是任务耗时。

**面试官追问**

- 发送邮件应该用哪一个？

取决于邮件是否关键、是否允许丢失、失败后是否要重试和审计。不能只根据“它是邮件”判断。

### 1.11.7 为什么不能把任务保存在 `app.state`

**参考回答**

`app.state` 属于当前应用进程。多个 Uvicorn Worker 各自拥有独立内存，进程重启后内容也会消失。它适合保存当前进程的连接池句柄和 Client，不适合作为跨进程任务状态的事实来源。共享状态应放在 PostgreSQL、Redis 等外部系统。

**面试官追问**

- 四个 Worker 会执行几次 Lifespan？

通常每个 Worker 进程各执行一次，所以通常是四次；连接池和内存资源也要按进程数估算。

### 1.11.8 为什么要分 Router、Service、Repository

**参考回答**

Router 负责 HTTP 协议和数据转换，Service 负责一个业务用例，Repository 负责数据访问。这样业务逻辑不会依赖 FastAPI Request 或 HTTP 状态码，可以被 API、Worker 和 CLI 复用，也更容易单元测试和管理事务边界。

**面试官追问**

- 小项目也必须分很多层吗？

不必机械拆分。代码量小时可以简单，但当业务逻辑需要跨接口复用、涉及事务或需要被 Worker 调用时，应及时抽出 Service。分层是为了解决变化和复用，不是追求文件数量。

### 1.11.9 认证、授权和审批有什么区别

**参考回答**

认证确认主体是谁；授权判断主体能否执行某类动作；Resource Scope 进一步限定能操作哪些具体资源；审批则判断这一次高风险操作是否被批准。Agent 还应在 Tool 真正执行前根据最终参数再次检查权限，不能只依赖 API 入口校验。

**容易答错的地方**

认为 JWT 验证成功就能访问所有资源，或者认为审批可以授予用户原本没有的权限。

### 1.11.10 复习总结

```text
客户端与 Python 进程无法直接调用
→ 需要 HTTP API 契约

请求方法和路径需要映射到函数
→ 需要 FastAPI Router

外部输入可能错误或恶意
→ 需要 Pydantic 输入模型

内部字段不能全部暴露
→ 需要独立响应模型

认证和数据库 Session 不能到处复制
→ 需要 Depends 和 yield 依赖

数据库、模型和 Tool 大量时间在等待
→ 需要正确使用 async / await

HTTP 细节不应污染业务规则
→ 需要 Router / Service / Repository 分层

连接池和 Client 创建成本高
→ 需要 Lifespan 管理应用级资源

Agent 任务远长于 HTTP 请求
→ 需要 202 + task_id + Queue + Worker

网络重试可能创建重复任务
→ 需要幂等键和唯一约束

数据库与队列写入可能只成功一半
→ 需要 Outbox 等一致性方案

用户、下游、容量和进程都可能失败
→ 需要权限、超时、重试、背压和持久化

分布式链路出错难以定位
→ 需要 request_id、task_id、Trace、Metrics 和 Logs
```

1. FastAPI 是 HTTP API 框架，不是 Agent Runtime，也不是 Web Server
2. 类型模型的核心价值是建立可信的数据边界，而不只是少写校验代码
3. `Depends` 解决公共准备工作的复用和生命周期问题，不负责隐藏全部业务流程
4. 异步提高 I/O 等待场景下的并发利用率，不等于 CPU 并行
5. Router 负责协议，Service 负责业务，Repository 负责数据访问
6. 核心 Agent 任务必须与 HTTP 请求生命周期分离
7. `202` 只是已受理，不代表任务成功
8. `BackgroundTasks` 不是可靠任务队列
9. 多 Worker 不共享 Python 内存，业务事实必须放在外部存储
10. 生产可靠性来自对失败的系统性处理，而不是来自框架名称

# 2 PostgreSQL 与 Redis：从任务持久化到运行时协调

这一章只沿着一条主线展开

```text
用户提交一个 Agent 任务
        ↓
PostgreSQL 保存任务和待投递事件
        ↓
Relay 把事件写入 Redis Stream
        ↓
Agent Worker 消费并执行任务
        ↓
PostgreSQL 保存最终状态和结果
        ↓
Redis 缓存热点状态、承担限流和短期协调
        ↓
用户根据 task_id 查询进度
```

后面出现的表、事务、MVCC、索引、缓存、TTL、Streams、幂等和分布式锁，都是为了让这条链路更正确、更快或更容易恢复。

```text
PostgreSQL：保存不能丢、需要查询、需要约束的业务事实
Redis：保存可以重建、强调速度、生命周期较短的运行时数据
```

## 2.1 为什么一个 Agent Runtime 通常需要两种存储

### 2.1.1 先从问题开始：Python 字典为什么不够

最小任务接口可能这样保存状态

```python
tasks: dict[str, dict[str, str]] = {}


def save_task(task_id: str, instruction: str) -> None:
    tasks[task_id] = {
        "instruction": instruction,
        "status": "pending",
    }
```

单进程演示时它可以工作，但只要进入生产环境，马上会遇到四个问题。

```text
进程重启
→ 字典清空，全部任务消失

启动四个 Uvicorn Worker
→ 四个进程各有一份字典，彼此看不到

API 和 Agent Worker 分开部署
→ Worker 无法直接读取 API 进程的内存

需要查询昨天失败的任务
→ 字典没有可靠持久化、索引和审计能力
```

因此，共享业务状态必须离开 Python 进程，放到独立存储系统中。

### 2.1.2 PostgreSQL 是什么

PostgreSQL 是关系型数据库。

“关系型”不是指表之间有关系这么简单。它的核心是用表表达结构化数据，并通过类型、约束、事务和查询语言维护数据的一致性。

可以把 PostgreSQL 想成企业的正式账本

```text
任务是否存在？             查任务表
任务属于哪个租户？         看 tenant_id
任务当前是什么状态？       看 status
是否重复提交？             查唯一约束
任务和事件是否一起保存？   由事务保证
谁在什么时间修改过任务？   查事件表或审计表
```

它适合保存以下信息

- Agent 任务、运行记录和最终结果
- 用户、租户、权限和审批记录
- Tool 调用记录与审计日志
- 幂等请求记录
- Outbox 待投递事件
- Skill、版本和发布记录

这些信息的共同点是：即使服务重启，也不能凭空消失；出现冲突时，还要有明确规则。

### 2.1.3 Redis 是什么

Redis 是以内存为主要工作介质的数据结构服务器。

客户端不是只能存一个普通字符串，还可以直接操作 Hash、List、Set、Sorted Set 和 Stream 等结构。

可以把 Redis 想成办事大厅旁边的高速工作台

```text
最近频繁查询的任务状态     临时放在桌面，避免反复翻账本
一分钟内访问次数           用计数器快速累加
短期去重标记               设置过期时间，时间到自动清理
等待 Worker 处理的事件      放入 Stream
多个实例争抢短期执行权      使用带租约的锁
```

Redis 的主要优势是内存访问快、命令简单、数据结构丰富。

但“Redis 很快”不等于“所有数据都应放 Redis”。内存更昂贵，键可能因 TTL 到期或内存淘汰而消失，持久化和异步复制也有明确的数据丢失窗口。

### 2.1.4 为什么不全部放 PostgreSQL

PostgreSQL 也能做计数、队列和锁。

例如，可以用一张任务表和 `FOR UPDATE SKIP LOCKED` 让 Worker 抢任务。中小规模系统甚至可以先这样做，组件更少，事务也更容易推理。

但当热点状态被高频轮询、限流计数每秒大量更新、事件需要阻塞消费时，这些运行时流量会和核心业务事务争抢连接、CPU、I/O 与表膨胀空间。

Redis 可以把这类高频、短期、可重建的数据从主数据库中分担出去。

### 2.1.5 为什么不全部放 Redis

Redis 可以开启 RDB 或 AOF 持久化，但这不自动让它等价于关系型数据库。

PostgreSQL 更擅长

- 多表关联和复杂条件查询
- 外键、唯一约束和检查约束
- 跨多行、多表的 ACID 事务
- 长期数据、审计和备份恢复
- 用查询计划和多种索引优化复杂查询

如果把任务事实只放 Redis，需要自己补上更多约束、查询、历史、备份和一致性逻辑。

所以本项目采用清晰分工

| 数据或能力 | PostgreSQL | Redis | 原因 |
| :--- | :---: | :---: | :--- |
| 任务当前状态 | 主存储 | 短期缓存 | 状态不能丢，缓存可以重建 |
| 任务结果 | 主存储或对象存储索引 | 通常不放大结果 | 结果可能很大，需要长期保存 |
| 幂等键 | 唯一约束兜底 | 可做快速拦截 | 正确性不能只依赖过期键 |
| 待投递事件 | Outbox 表 | Stream 负责分发 | 先解决数据库与队列双写 |
| 限流计数 | 一般不承担热点计数 | 适合 | 原子计数和 TTL 成本低 |
| 短期分布式锁 | 数据库锁可选 | 常用租约锁 | Redis 延迟低，但锁有安全边界 |
| 审计历史 | 适合 | 不作为长期事实 | 需要可靠查询和保留策略 |

### 2.1.6 PostgreSQL 和 Redis 是替代关系吗？

**参考回答**

通常不是。PostgreSQL 负责持久化业务事实、事务、约束和复杂查询；Redis 负责缓存、短期状态、计数、消息分发和运行时协调。是否同时使用取决于业务规模和可靠性要求。早期系统可以只用 PostgreSQL，只有高频临时数据确实形成压力时再引入 Redis。

**容易答错的地方**

只回答“PostgreSQL 慢、Redis 快”。两者的差异首先是数据模型、事务与可靠性职责，不只是延迟数字。

## 2.2 PostgreSQL 怎样把业务规则变成数据结构

### 2.2.1 表、行和列到底是什么

假设系统要保存一个任务

```text
任务 ID：         6c8a...
租户：            tenant-a
用户指令：        分析最近七天订单失败原因
状态：            pending
创建时间：        2026-08-30 10:00:00+08
```

关系表把同一类实体放在一起

|   id    | tenant_id | instruction  | status  |       created_at       |
| :-----: | :-------: | :----------: | :-----: | :--------------------: |
| 6c8a... | tenant-a  | 分析最近七天订单失败原因 | pending | 2026-08-30 10:00:00+08 |

- 一张表表示一类数据，例如 `agent_tasks`
- 一行表示一个具体任务
- 一列表示一个稳定属性
- 每列的数据类型限制它能保存什么值

这不是为了把数据画成表格，而是为了让数据库理解数据结构并执行约束、排序、连接和索引。

### 2.2.2 为什么数据库约束不能只写在 Pydantic 中

FastAPI 的 Pydantic 模型可以拒绝空指令，但数据不一定只从 FastAPI 进入。

```text
FastAPI
后台管理脚本
数据修复脚本
定时任务
另一个微服务
数据库迁移
```

如果规则只在 API 层，其他入口可能写入非法数据。

因此常见做法是分层防守

```text
Pydantic：尽早给客户端友好错误
Service：校验跨字段和业务规则
数据库约束：阻止任何入口破坏底线
```

常用约束如下。

|      约束       |   解决的问题   |      Agent 项目示例       |
| :-----------: | :-------: | :-------------------: |
| `PRIMARY KEY` | 每行怎样被唯一定位 |       `task.id`       |
|  `NOT NULL`   | 字段是否允许缺失  |  `instruction` 不能为空   |
|   `UNIQUE`    |  哪些值不能重复  |      同租户的幂等键不能重复      |
|    `CHECK`    | 值必须满足什么条件 |       状态只能取规定集合       |
| `FOREIGN KEY` | 引用对象必须存在  | `task.tenant_id` 指向租户 |

数据库约束不是“多余的重复校验”，而是所有写入路径共享的最后防线。

### 2.2.3 一份适合任务主线的表设计

下面的 SQL 会在完整示例中直接使用。

```sql
CREATE TABLE agent_tasks (
    id UUID PRIMARY KEY,
    tenant_id TEXT NOT NULL,
    idempotency_key TEXT NOT NULL,
    instruction TEXT NOT NULL
        CHECK (char_length(instruction) BETWEEN 1 AND 2000),
    status TEXT NOT NULL DEFAULT 'pending'
        CHECK (status IN ('pending', 'running', 'succeeded', 'failed', 'cancelled')),
    result JSONB,
    error_message TEXT,
    attempt_count INTEGER NOT NULL DEFAULT 0
        CHECK (attempt_count >= 0),
    version INTEGER NOT NULL DEFAULT 0
        CHECK (version >= 0),
    lease_owner TEXT,
    lease_until TIMESTAMPTZ,
    created_at TIMESTAMPTZ NOT NULL DEFAULT now(),
    updated_at TIMESTAMPTZ NOT NULL DEFAULT now(),
    UNIQUE (tenant_id, idempotency_key)
);

CREATE TABLE task_events (
    id BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    task_id UUID NOT NULL REFERENCES agent_tasks(id),
    event_type TEXT NOT NULL,
    payload JSONB NOT NULL DEFAULT '{}'::jsonb,
    created_at TIMESTAMPTZ NOT NULL DEFAULT now()
);

CREATE TABLE outbox_events (
    id UUID PRIMARY KEY,
    aggregate_id UUID NOT NULL REFERENCES agent_tasks(id),
    event_type TEXT NOT NULL,
    payload JSONB NOT NULL,
    created_at TIMESTAMPTZ NOT NULL DEFAULT now(),
    published_at TIMESTAMPTZ,
    publish_attempts INTEGER NOT NULL DEFAULT 0
);
```

这里故意把数据分成三类。

```text
agent_tasks
→ 保存当前事实，回答“任务现在怎样”

task_events
→ 保存重要状态变化，回答“任务经历了什么”

outbox_events
→ 保存还要投递给队列的事件，解决数据库与队列双写
```

### 2.2.4 为什么状态既有当前值，又有事件历史

只保存事件也能通过重放计算当前状态，但每次查询都重放全部事件，代码和运维复杂度更高。

只保存当前状态查询很快，但无法回答任务何时开始、重试了几次、由谁取消等问题。

所以常见工程折中是

```text
主表保存当前状态
+
事件表保存关键变化
```

更新任务时，二者放在同一数据库事务中提交。

事件表并不自动等于完整的 Event Sourcing。Event Sourcing（事件溯源）会把事件作为唯一事实来源，并通过事件重建状态，设计和迁移成本更高。

### 2.2.5 `JSONB` 为什么有用，又为什么不能滥用

Agent 任务的输入和结果可能随任务类型变化

```json
{
  "summary": "订单失败主要来自库存不足",
  "evidence_ids": ["ev-101", "ev-203"],
  "token_usage": 1830
}
```

`JSONB` 适合保存结构变化较多的扩展字段。PostgreSQL 会以二进制形式存储并支持 JSON 运算和 GIN 索引。

但如果把所有字段都塞进一个 JSONB，会失去清晰的数据契约

```text
哪些字段必须存在？
状态能否随便写？
怎样建立外键？
怎样高效按租户和时间分页？
字段改名后谁负责迁移？
```

实用判断方法是

|        字段特点        |        建议        |
| :----------------: | :--------------: |
|  稳定、常过滤、常排序、需要约束   |       普通列        |
|    结构变化多、主要整体读取    |     `JSONB`      |
| JSON 内部经常按键或包含关系查询 |  评估 GIN 或表达式索引   |
|  大型文件、模型输出、二进制数据   | 对象存储，数据库只存地址和元数据 |

一句话：列保存系统明确理解的事实，JSONB 保存有边界的扩展信息。

### 2.2.6 为什么时间通常使用 `TIMESTAMPTZ`

`TIMESTAMPTZ` 表示一个确定的时间点。PostgreSQL 内部按统一时间语义保存，查询时根据会话时区显示。

Agent Runtime 可能跨地域部署，任务超时、租约和审计都要求比较同一时间线。只保存没有时区含义的本地时间，遇到跨时区或夏令时会变得难以解释。

工程上通常

- 数据库保存 `TIMESTAMPTZ`
- 服务间传输 ISO 8601 时间并带偏移或 `Z`
- UI 再转换为用户所在时区
- 超时计算优先使用服务器时间或单调时钟，不相信客户端时间

### 2.2.7 为什么幂等键要做联合唯一约束

幂等键通常由客户端生成。同一个租户重试相同请求，应返回同一个任务；不同租户即使碰巧使用同样的字符串，也不应互相冲突。

所以约束是

```sql
UNIQUE (tenant_id, idempotency_key)
```

而不是只对 `idempotency_key` 唯一。

这也体现多租户系统的重要原则：租户边界不仅写在查询条件里，也应尽可能体现在键、索引和约束中。

### 2.2.8 什么时候应该范式化，什么时候可以冗余

范式化的目标是减少同一事实的重复保存。

例如，租户名称应该保存在租户表，任务表只保存 `tenant_id`。否则租户改名时需要更新大量任务行，还可能出现一半新名称、一半旧名称。

但为了查询性能或保留历史语义，可以有意识地冗余

```text
任务创建时的模型名称
任务创建时的策略版本
Tool 调用时的参数快照
```

这些值即使以后配置变化，历史记录也不应跟着改变。

所以判断标准不是“能不能重复”，而是：它是同一个会变化的事实，还是当时不可变的历史快照？

### 2.2.9 为什么不能用字符串拼接 SQL

下面的写法把外部输入直接拼进 SQL

```text
sql = "SELECT * FROM agent_tasks WHERE id = '" + task_id + "'"
```

攻击者可能让输入改变 SQL 结构，形成 SQL 注入。

正确做法是参数化查询

```python
from sqlalchemy import text


GET_TASK_SQL = text(
    "SELECT id, status FROM agent_tasks WHERE id = :task_id"
)
```

调用时把 `task_id` 作为参数单独传入。数据库驱动负责按数据值处理，而不是把它解释成 SQL 语法。

参数化查询解决的是值注入。表名、列名和排序方向不能简单当作普通参数，应使用白名单映射，不能直接接受客户端字符串。

## 2.3 事务为什么是 PostgreSQL 的核心能力

### 2.3.1 没有事务会发生什么

创建任务需要两次写入

```text
1. INSERT agent_tasks
2. INSERT outbox_events
```

如果第一步成功、第二步失败，会得到一个永远没有被投递的 `pending` 任务。

如果第二步成功、第一步失败，事件又指向不存在的任务。

业务希望这两步只有两种结果

```text
全部成功
或者
全部不发生
```

事务就是数据库提供的这条边界。

```sql
BEGIN;

INSERT INTO agent_tasks (...);
INSERT INTO outbox_events (...);

COMMIT;
```

中途出错则 `ROLLBACK`，事务内的修改对外不生效。

### 2.3.2 ACID 分别在解决什么

不要只背四个英文单词，要把它们放回任务场景。

|       特性        |       通俗理解       |          任务场景          |
| :-------------: | :--------------: | :--------------------: |
|  Atomicity 原子性  |  一组操作要么全做，要么全不做  |    任务和 Outbox 一起写入     |
| Consistency 一致性 | 事务前后都满足约束和业务不变量  |      状态合法、幂等键不重复       |
|  Isolation 隔离性  | 并发事务不会随意看到彼此中间过程 | 两个 Worker 不能同时成功领取同一任务 |
| Durability 持久性  | 已提交结果在故障恢复后仍应存在  | API 返回已创建后，任务不能因进程重启消失 |

其中“一致性”并不表示数据库自动理解所有业务。

数据库只能维护已经声明的约束和事务规则。比如“余额不能为负”“任务只能按状态机迁移”，仍需要 Service、条件更新或数据库约束明确表达。

### 2.3.3 事务边界应该放在哪里

事务应该围绕一个完整业务用例，而不是围绕一个 Repository 方法。

```text
错误边界
Repository.insert_task()      自己提交
Repository.insert_outbox()    自己提交
→ 两次独立提交，仍可能只成功一半

正确边界
Service.create_task()
  ├─ insert_task
  └─ insert_outbox
→ Service 完成后统一提交
```

所以事务通常由 Service 或 Use Case 层控制，Repository 只执行数据访问。

### 2.3.4 为什么不能在数据库事务中调用大模型

下面的流程很危险

```text
BEGIN
→ 锁定任务行
→ 调用大模型，等待 30 秒
→ 调用 Tool，再等 20 秒
→ 更新结果
COMMIT
```

长事务会长期占用连接和行版本，可能阻塞其他更新，也会妨碍 VACUUM 清理旧版本。并发一高，连接池和锁等待会迅速堆积。

更合理的做法是使用短事务

```text
短事务 1：领取任务，写入 running 和租约，提交
        ↓
事务外：调用模型和 Tool
        ↓
短事务 2：检查租约所有者，保存结果，提交
```

数据库事务保护的是短时间的数据修改，不应该包住不可控的远程调用。

### 2.3.5 MVCC 是什么，为什么读写可以并发

MVCC 全称 Multi-Version Concurrency Control，多版本并发控制。

直观理解是：更新一行时，PostgreSQL 通常不是立刻在原位置抹掉旧值，而是产生新的行版本。不同事务根据自己的可见性规则看到合适版本。

```text
事务 A 开始读取 status = pending

事务 B 把 status 更新为 running 并提交

事务 A 是否立即看到 running
→ 取决于隔离级别和下一次语句的快照
```

这样普通读查询通常不需要因为另一事务正在写就一直阻塞，读写并发能力更好。

代价是旧行版本不会凭空消失。`UPDATE`、`DELETE` 产生的旧版本需要 VACUUM 清理，统计信息也需要 ANALYZE 更新。

因此高频更新任务状态时，要关注

- 长事务是否阻止旧版本清理
- 表和索引是否膨胀
- autovacuum 是否跟得上写入速度
- 是否把高频心跳全部写进同一行

### 2.3.6 默认的 Read Committed 到底能看到什么

PostgreSQL 默认隔离级别是 Read Committed。

在这个级别下，一个普通 `SELECT` 看到的是该条语句开始时已经提交的数据。相同事务中的下一条 `SELECT` 会取得新的语句快照，因此可能看到其他事务刚提交的新值。

```text
事务 A：SELECT status  → pending
事务 B：UPDATE status = running; COMMIT
事务 A：再次 SELECT     → 可能看到 running
```

它不会读到别的事务尚未提交的数据，所以没有脏读。

PostgreSQL 接受 `READ UNCOMMITTED` 名称，但内部行为与 `READ COMMITTED` 相同。

### 2.3.7 隔离级别越高越好吗

不是。隔离越强，应用需要承担的冲突检测、重试或资源成本通常也越高。

|      隔离级别       |       直观快照       |    适合场景    |       应用责任       |
| :-------------: | :--------------: | :--------: | :--------------: |
| Read Committed  |     每条语句一个快照     | 大多数简单 CRUD |  用条件更新和锁保护关键竞争   |
| Repeatable Read |    事务期间保持一致快照    | 一组读取必须相互一致 |   处理并发更新失败并重试    |
|  Serializable   | 成功提交的结果等价于某种串行顺序 |  复杂跨行不变量   | 必须捕获序列化失败并重试整个事务 |

Serializable 不是“打开后永不出错”。PostgreSQL 发现无法安全串行化时，会主动中止一个事务，应用必须重试。

正确原则是：先明确业务不变量和冲突模式，再选择隔离、显式锁或条件更新。

### 2.3.8 乐观锁：先假设冲突不多

任务状态迁移可以使用条件更新

```sql
UPDATE agent_tasks
SET status = 'running',
    version = version + 1,
    updated_at = now()
WHERE id = :task_id
  AND status = 'pending';
```

如果受影响行数是 1，说明领取成功。

如果是 0，可能是任务不存在，也可能已被其他 Worker 领取，需要继续查询判断。

也可以使用版本号

```sql
UPDATE agent_tasks
SET result = :result,
    version = version + 1
WHERE id = :task_id
  AND version = :old_version;
```

这种方式叫乐观并发控制。它先假设冲突较少，提交时再检查数据是否仍是原版本。

适合

- 冲突概率较低
- 不希望长时间持锁
- 可以在冲突后重试或提示用户

### 2.3.9 悲观锁：先把竞争资源锁住

Outbox Relay 可能有多个实例同时扫描未投递事件。

可以使用

```sql
SELECT id, payload
FROM outbox_events
WHERE published_at IS NULL
ORDER BY created_at
FOR UPDATE SKIP LOCKED
LIMIT 100;
```

- `FOR UPDATE` 锁定选中的行，防止其他事务同时修改
- `SKIP LOCKED` 遇到已被别的 Relay 锁住的行就跳过
- `LIMIT` 控制每批处理数量，避免一次锁太多

这适合多个消费者并行抢不同工作项。

但 `SKIP LOCKED` 返回的是一个刻意跳过锁定行的视图，不适合普通业务查询或要求完整一致结果的报表。

### 2.3.10 死锁是什么，数据库为什么不能完全替你避免

两个事务可能按相反顺序加锁

```text
事务 A 已锁任务 1，等待任务 2
事务 B 已锁任务 2，等待任务 1
```

双方互相等待，形成死锁。

PostgreSQL 会检测死锁并中止其中一个事务，但应用仍需要处理错误并重试。

常见预防方式

1. 所有代码按固定顺序锁资源
2. 事务尽量短
3. 不在事务中等待用户、模型或外部 HTTP
4. 建立合适索引，避免更新扫描过多行
5. 重试整个事务，而不是从失败语句中间继续

### 2.3.11 事务和锁的面试回答应该落到哪里

**参考回答**

事务用来保护一个业务用例内的多步数据修改，例如任务记录和 Outbox 事件必须原子提交。PostgreSQL 通过 MVCC 提供版本可见性，默认 Read Committed 为每条语句建立快照。并发写竞争可以用带旧状态或版本号的条件更新做乐观控制，也可以使用 `SELECT FOR UPDATE` 做悲观锁。无论 Serializable 还是显式锁，都要准备处理冲突、死锁和事务重试。

**面试官追问**

为什么不把 Agent 执行放进同一个事务？

因为模型和 Tool 调用耗时长且不可控，会长期占用连接、锁和旧版本。应使用短事务领取任务，通过租约表达执行权，在事务外运行，再用短事务提交结果。

## 2.4 索引为什么能加速查询，又为什么不能乱建

### 2.4.1 没有索引时数据库怎样找任务

假设表中有一千万个任务，要查询

```sql
SELECT id, status, created_at
FROM agent_tasks
WHERE tenant_id = 'tenant-a'
ORDER BY created_at DESC
LIMIT 20;
```

如果没有合适索引，数据库可能扫描大量行、逐行判断租户，再排序。

索引可以理解成书末目录

```text
没有目录：从第一页往后翻
有目录：先按关键词找到页码，再取正文
```

PostgreSQL 最常用的 B-tree 索引适合等值、范围和排序查询。

### 2.4.2 索引应该从查询反推，而不是从字段反推

不要看到一个字段就问“要不要建索引”。

先收集真实查询

```text
查询某租户最近创建的 20 个任务
查询某任务 ID
扫描尚未投递的 Outbox
查找正在运行且租约已过期的任务
```

再针对 `WHERE`、`JOIN`、`ORDER BY` 和返回行数设计索引。

例如

```sql
CREATE INDEX idx_tasks_tenant_created
ON agent_tasks (tenant_id, created_at DESC, id DESC);
```

它同时服务租户过滤和时间倒序分页。

### 2.4.3 联合索引的顺序为什么重要

索引 `(tenant_id, created_at, id)` 先按租户组织，再在同一租户内按时间和 ID 组织。

因此它通常很适合

```sql
WHERE tenant_id = :tenant_id
ORDER BY created_at DESC, id DESC
```

对只按 `created_at`、完全不限制 `tenant_id` 的查询，它通常没有同等效果。

常见设计思路

1. 高价值的等值过滤列放前面
2. 范围或排序列随后
3. 用稳定唯一列作为分页的最后排序键
4. 根据真实数据分布和执行计划验证

“最左前缀”是有用的入门心智模型，但不要答成“查询不带第一列，数据库绝不可能使用后续列”。新版 PostgreSQL 可能使用索引跳跃扫描，也可能组合多个索引；最终以执行计划和数据分布为准。

### 2.4.4 为什么索引不是越多越好

每增加一个索引，写入时都要维护它。

```text
INSERT 一行
→ 写表
→ 更新每个相关索引

UPDATE 被索引列
→ 产生新行版本
→ 更新相关索引
```

索引的代价包括

- 占用磁盘和缓存
- 降低插入、更新和删除速度
- 增加 VACUUM 和备份成本
- 让查询优化器面对更多候选计划

所以要删除长期未使用或重复覆盖的索引，但先确认统计周期和特殊任务，不能只看一天数据。

### 2.4.5 部分索引为什么适合运行中任务

历史任务可能有几千万条，但真正处于 `pending` 或 `running` 的只有少量。

可以只索引活跃子集

```sql
CREATE INDEX idx_tasks_active_lease
ON agent_tasks (lease_until, id)
WHERE status IN ('pending', 'running');
```

优点是索引更小，扫描租约过期任务更快。

限制是查询条件必须能让优化器判断它满足该索引谓词。参数化条件、写法差异和状态集合变化都要通过执行计划验证。

### 2.4.6 唯一索引不只是性能优化

`UNIQUE (tenant_id, idempotency_key)` 通常由唯一索引实现，但它首先是正确性约束。

即使两个 API 实例同时检查“幂等键不存在”，最终也只有一个插入能成功。

```text
先 SELECT 再 INSERT
→ 两个请求可能同时 SELECT 到不存在

直接依赖 UNIQUE + INSERT ... ON CONFLICT
→ 数据库在真正竞争点裁决
```

这也是为什么“应用已经检查过”不能替代数据库唯一约束。

### 2.4.7 JSONB 的 GIN 索引什么时候才有价值

如果经常执行

```sql
SELECT id
FROM task_events
WHERE payload @> '{"tool_name": "search_orders"}';
```

可以评估

```sql
CREATE INDEX idx_task_events_payload_gin
ON task_events USING GIN (payload);
```

GIN 适合查 JSONB 内的键、值或包含关系，但写入和存储成本也更高。

如果只查询 JSON 中某一个稳定字段，表达式索引或直接提升为普通列可能更清晰。

### 2.4.8 `EXPLAIN` 和 `EXPLAIN ANALYZE` 看什么

`EXPLAIN` 展示优化器预计采用的计划。

`EXPLAIN ANALYZE` 会真正执行语句，并显示实际耗时和实际行数。

```sql
EXPLAIN (ANALYZE, BUFFERS)
SELECT id, status, created_at
FROM agent_tasks
WHERE tenant_id = 'tenant-a'
ORDER BY created_at DESC, id DESC
LIMIT 20;
```

初学者先关注

|              字段              |       要问的问题       |
| :--------------------------: | :---------------: |
|           Scan 类型            | 是顺序扫描、索引扫描还是位图扫描？ |
| estimated rows / actual rows |    估算是否严重偏离实际？    |
|         actual time          |    时间主要花在哪个节点？    |
|            loops             |  子计划实际重复执行了多少次？   |
|           Buffers            |   读取了多少缓存页或磁盘页？   |
|             Sort             |    是否发生大排序或落盘？    |

不要看到 `Seq Scan` 就认定有问题。小表、返回比例很高或索引随机访问更贵时，顺序扫描可能更合理。

还要记住：`EXPLAIN ANALYZE` 会真正执行 `INSERT`、`UPDATE` 和 `DELETE`。分析写操作时应使用测试环境，或在明确的事务中执行后回滚。

### 2.4.9 为什么深分页不应一直使用 OFFSET

传统分页

```sql
SELECT id, created_at
FROM agent_tasks
WHERE tenant_id = :tenant_id
ORDER BY created_at DESC, id DESC
LIMIT 20 OFFSET 200000;
```

数据库仍要定位并跳过前面的二十万行。页数越深，成本越高；并发插入还可能让页面重复或遗漏。

Keyset Pagination 使用上一页最后一条记录作为游标

```sql
SELECT id, created_at
FROM agent_tasks
WHERE tenant_id = :tenant_id
  AND (created_at, id) < (:last_created_at, :last_id)
ORDER BY created_at DESC, id DESC
LIMIT 20;
```

配合 `(tenant_id, created_at DESC, id DESC)` 索引，成本通常更稳定。

它的代价是不能方便地直接跳到第 5000 页，适合时间流、任务列表和滚动加载。

### 2.4.10 常见数据库性能问题不是“没有索引”一种

|       症状       |      可能原因      |            优先检查             |
| :------------: | :------------: | :-------------------------: |
|     单条查询慢      |   扫描、排序、估算错误   |          执行计划和统计信息          |
|     并发一高就慢     |   连接池耗尽、锁等待    |       活跃连接、等待事件、事务时长        |
|     写入越来越慢     |    索引过多、表膨胀    | 索引数量、dead tuples、autovacuum |
| API 发出大量相似 SQL |     N+1 查询     |        ORM 日志和 Trace        |
|      偶发长尾      | I/O、检查点、锁、连接建立 |        P95/P99 和系统指标        |
|  读副本偶尔查不到新任务   |      复制延迟      |    是否要求 read-after-write    |

优化顺序通常是：先复现和测量，再读执行计划，最后改查询、索引或模型。不要先加缓存掩盖未知问题。

## 2.5 连接池、Session 和生产运维为什么同样重要

### 2.5.1 为什么每次请求都新建数据库连接很贵

建立连接需要网络握手、认证和数据库进程资源。每个请求都建立并立即关闭，会增加延迟并冲击数据库。

连接池提前维护一批可复用连接

```text
请求到达
→ 从池中借一个连接
→ 执行短事务
→ 把连接归还池
```

连接池不是越大越好。数据库能高效处理的并发连接有限，连接过多会增加内存和上下文切换。

假设

```text
4 个 Uvicorn Worker
每个进程 pool_size = 10
每个进程 max_overflow = 20
```

理论峰值可能达到

```text
4 × (10 + 20) = 120 个连接
```

还没有计算 Relay、Agent Worker、管理脚本和监控连接。

所以连接预算要按所有进程和副本相乘，而不是只看一份配置。

### 2.5.2 SQLAlchemy 的 Engine、连接和 Session 分别是什么

用通俗语言理解

|      对象      |         作用         |
| :----------: | :----------------: |
| AsyncEngine  |  管理数据库方言和连接池的长期对象  |
|  Connection  |   从池中借出的底层数据库连接    |
| AsyncSession | ORM 工作单元，跟踪对象并承载事务 |

标准异步写法

```python
from sqlalchemy.ext.asyncio import async_sessionmaker, create_async_engine


engine = create_async_engine(
    "postgresql+asyncpg://app:app@localhost:5432/evoagent",
    pool_size=10,
    max_overflow=10,
    pool_pre_ping=True,
)

SessionFactory = async_sessionmaker(
    engine,
    expire_on_commit=False,
)
```

应用启动时创建 Engine，关闭时 `await engine.dispose()`。每个请求或业务用例创建独立 Session，并用上下文管理器关闭。

不要在多个并发协程之间共享同一个 `AsyncSession`。Session 有事务和对象状态，不是无状态数据库客户端。

### 2.5.3 为什么“Session per request”不等于“事务覆盖整个请求”

一个请求可以拥有一个 Session，方便依赖注入和资源回收。

但事务仍应按业务用例明确开始和结束，不能因为 Session 活着就让事务一直打开。

```python
async def save_one_use_case() -> None:
    async with SessionFactory.begin() as session:
        # 这里是一个短事务
        ...
    # 正常离开时提交，异常时回滚，并关闭 Session
```

如果之后还要调用大模型，应先离开事务上下文。

### 2.5.4 为什么数据库连接池不能解决所有背压问题

连接池耗尽时，请求会排队等待连接。它保护数据库不被无限连接压垮，但等待队列本身也可能把 API 拖死。

生产中还需要

- 合理的连接获取超时
- API 并发限制
- Worker 并发和队列长度限制
- 慢查询超时，例如 `statement_timeout`
- 事务空闲超时，例如 `idle_in_transaction_session_timeout`
- 过载时快速失败或降级

连接池是容量闸门，不是自动扩容器。

### 2.5.5 数据库迁移为什么不能靠应用启动时 `create_all`

`create_all` 适合演示或测试，但生产结构变更需要可审计、可排序、可回滚或可修复的迁移工具，例如 Alembic。

生产迁移要考虑

```text
加列是否会长时间锁表？
旧代码和新结构能否短期兼容？
创建大索引是否使用 CONCURRENTLY？
回填数据是否分批？
失败后怎样恢复？
```

常见安全节奏是 Expand and Contract

```text
先增加兼容结构
→ 新旧代码并存
→ 分批回填
→ 切换读写
→ 最后删除旧结构
```

### 2.5.6 备份、WAL 和复制分别解决什么

- WAL 记录数据库修改，是崩溃恢复和复制的重要基础
- 主从复制提高读取能力和可用性，但不是备份
- 备份用于应对误删除、逻辑损坏和灾难恢复
- 高可用解决“服务尽快恢复”，备份解决“恢复到某个正确历史点”

只配置副本不能防止误删，因为错误也会被复制过去。

真正的恢复能力要定期验证 restore，而不是只检查“备份任务显示成功”。

### 2.5.7 读副本为什么可能查不到刚创建的任务

许多复制方案是异步的。

```text
主库：任务已经 COMMIT
        ↓ 复制仍有延迟
读副本：暂时还没有这行
```

如果创建任务后立刻查询副本，可能得到 404。

常见策略

- 对刚写入的数据短时间读主库
- 需要 read-after-write 的接口固定读主库
- 在可接受最终一致的报表、列表中读副本
- 监控复制延迟，超阈值停止路由读流量

“读写分离”不是把所有 SELECT 自动发往副本。

## 2.6 Redis 的数据结构为什么比“键值缓存”更重要

### 2.6.1 Redis 的 Key 和 Value 是什么

Redis 使用 Key 定位数据，但 Value 可以有不同结构。

```text
Key:   evo:tenant-a:task:6c8a:status
Value: {"status":"running","version":2}
```

Key 需要体现命名空间和业务对象

```text
产品:租户:实体:实体ID:用途
evo:tenant-a:task:6c8a:status
```

好处是可读、可统计、降低不同模块键名冲突。

不要把用户可控的超长文本直接放进 Key，也不要使用包含敏感信息的明文键名。

Redis Cluster 中花括号形成 hash tag，例如 `evo:{tenant-a}:task:6c8a`。同一花括号内容的键会落到同一槽，便于多键操作，但也可能让某个大租户成为热点槽，必须根据访问模式设计。

### 2.6.2 常用数据结构怎样选择

|     类型     |    它是什么    |       常见用途       |     不适合什么     |
| :--------: | :--------: | :--------------: | :-----------: |
|   String   |    字节序列    | JSON 缓存、计数器、幂等标记 | 频繁只改大对象中的一个字段 |
|    Hash    |   字段和值集合   |   小对象、多个字段独立更新   | 超大 Hash 集中成热点 |
|    List    |  有序字符串列表   |   简单栈、队列、阻塞弹出    | 需要确认、重放、消费者组  |
|    Set     |  不重复成员集合   |   权限集合、去重、成员判断   |    需要按分数排序    |
| Sorted Set | 成员带 score  |  延迟任务、排行榜、滑动窗口   |    可靠消息确认     |
|   Stream   | 追加日志和消息 ID |  事件流、消费者组、待确认消息  | 长期无限保留的大数据日志  |

选择数据结构，本质上是在选择服务器原生支持的操作。

例如限流需要原子自增，就用 `INCR`；延迟任务需要按到期时间取范围，就用 Sorted Set；需要 Worker 确认和重新领取，就用 Stream。

### 2.6.3 Redis 为什么快

常见原因包括

- 工作集主要在内存中
- 命令的数据结构和协议相对直接
- 典型命令执行路径短
- 普通命令在主执行路径上按顺序处理，减少复杂共享内存锁竞争
- 支持 Pipeline，减少网络往返

但不要把它简化成“Redis 单线程，所以快”。Redis 还会使用后台线程或进程处理持久化、释放内存等工作，新版本也有 I/O 线程能力。

更重要的是，命令顺序执行不代表业务流程没有并发竞争。

```text
客户端 A：GET counter → 9
客户端 B：GET counter → 9
客户端 A：SET counter 10
客户端 B：SET counter 10
```

单个 `GET` 和 `SET` 各自是原子的，但“先读再写”是多条命令，其他客户端可以在中间插入。

应优先使用 `INCR` 等原子命令，复杂逻辑使用 Lua、事务或重新设计数据模型。

### 2.6.4 Pipeline、事务和 Lua 分别解决什么

|      机制      |       主要解决的问题        |   是否自动回滚    |
| :----------: | :------------------: | :---------: |
|   Pipeline   |       减少多次网络往返       |      否      |
| `MULTI/EXEC` | 一组命令连续执行，不被其他客户端命令插入 |      否      |
|   `WATCH`    |    乐观检查键在提交前是否变化     | 不适用，冲突时不执行  |
|    Lua 脚本    |   把多步读写放到服务器端原子执行    | 脚本错误语义需谨慎处理 |

Redis 事务与 PostgreSQL 事务不是同一种能力。Redis `MULTI/EXEC` 不提供关系型数据库那样的通用回滚。

### 2.6.5 TTL 是什么，为什么缓存通常必须有 TTL

TTL 是 Time To Live，表示键还能存活多久。

```text
SET task:6c8a:status "running" EX 30
```

30 秒后该键逻辑上过期。

TTL 主要解决

- 缓存长期不更新
- 临时幂等标记永久占内存
- 限流窗口无法自动结束
- 锁持有者崩溃后永远不释放

Redis 通过访问时的被动过期和后台主动抽样清理过期键。TTL 到达后，客户端不应再读到有效值，但内存物理回收不等于精准定时任务调度。

所以不要用“设置 TTL”代替严格的任务调度器。

### 2.6.6 TTL 和内存淘汰有什么区别

它们是两个不同机制。

```text
TTL 到期
→ 这个键按业务设定应该失效

maxmemory 超限触发 eviction
→ Redis 按淘汰策略为了释放内存删除键
```

即使 TTL 还没到，键也可能被 `allkeys-lru` 等策略提前淘汰。

这再次说明：缓存键必须允许丢失并从事实来源重建。

如果同一个 Redis 同时存缓存和不能随意淘汰的 Stream/协调数据，淘汰策略会很难选择。生产上常把不同可靠性和内存策略的数据拆到不同 Redis 实例或集群。

### 2.6.7 RDB 和 AOF 是否让 Redis 数据绝对不丢

不会。

|    方式     |    原理     |     优点      |       主要窗口或代价        |
| :-------: | :-------: | :---------: | :------------------: |
|    RDB    | 定期生成时间点快照 |  文件紧凑、恢复快   |     快照之间的数据可能丢失      |
|    AOF    | 记录写命令并重放  | 通常更小的数据丢失窗口 | 文件更大，fsync 策略影响延迟和窗口 |
| RDB + AOF | 同时使用两种方式  |  兼顾恢复和耐久性   |      运维和资源成本更高       |
|   不持久化    |   只使用内存   |  简单，适合纯缓存   |       重启后全部丢失        |

即使开启 AOF，`appendfsync everysec` 也明确接受一个小的数据丢失窗口。Redis 复制通常是异步的，故障切换时仍可能丢少量已确认写入。

因此要先问业务允许多少数据丢失，再选择配置。不能只说“开了 AOF，所以和 PostgreSQL 一样可靠”。

## 2.7 Cache-Aside 为什么是最常见的缓存模式

### 2.7.1 不使用缓存会发生什么

客户端可能每秒查询一次任务状态。十万个正在运行的任务会形成大量重复查询

```sql
SELECT id, status, result, updated_at
FROM agent_tasks
WHERE tenant_id = :tenant_id AND id = :task_id;
```

单次主键查询很快，但大量重复流量仍会消耗数据库连接、CPU 和网络。

任务状态允许几十秒内从数据库重建，因此适合短 TTL 缓存。

### 2.7.2 Cache-Aside 的读取流程

```text
客户端查询任务
        ↓
先查 Redis
   ┌────┴────┐
 命中       未命中
  ↓           ↓
返回缓存    查询 PostgreSQL
              ↓
           写入 Redis，设置 TTL
              ↓
             返回
```

伪代码

```python
import json


async def get_task(task_id: str) -> dict[str, object] | None:
    key = f"evo:demo:task:{task_id}:status"
    cached = await redis_client.get(key)
    if cached is not None:
        return json.loads(cached)

    task = await task_repository.get(task_id)
    if task is None:
        return None

    await redis_client.set(key, json.dumps(task), ex=30)
    return task
```

它叫 Cache-Aside，是因为应用自己决定何时查缓存、何时回源和何时写缓存。

### 2.7.3 更新时为什么先提交数据库，再删除缓存

数据库是事实来源，所以先提交数据库

```text
BEGIN
→ UPDATE PostgreSQL
→ COMMIT
→ DELETE Redis cache key
```

如果先删缓存再更新数据库，另一个请求可能在数据库仍是旧值时回源，并把旧值重新写回缓存。

但“先更新数据库，再删缓存”也不是数学上的强一致。

一种竞态是

```text
读请求：缓存未命中，开始读取旧数据库值
写请求：提交新值，删除缓存
读请求：把刚才读到的旧值写回缓存
```

常见缓解方式

- 缓存使用短 TTL
- 缓存值携带数据库 `version`
- 写缓存时拒绝旧版本覆盖新版本
- 使用变更事件或 CDC 做失效通知
- 热点回源使用 singleflight，减少并发填充
- 强一致查询直接读主库，不走缓存

“延迟双删”有时能降低概率，但它依赖时间假设，不应被描述成通用强一致方案。

### 2.7.4 缓存穿透、击穿和雪崩分别是什么

**缓存穿透**

大量请求查询根本不存在的 ID，缓存永远未命中，每次都打到数据库。

可选措施

- 鉴权和输入校验先拦截明显非法请求
- 对“确实不存在”短时间缓存空结果
- 大规模固定集合可评估 Bloom Filter
- 对请求方限流

缓存空值的 TTL 应短，避免对象刚创建后仍长期返回不存在。

**缓存击穿**

一个热点 Key 到期，大量请求同时回源数据库。

可选措施

- singleflight 或互斥重建，只让一个请求回源
- 逻辑过期：先返回稍旧值，后台刷新
- 热点键提前刷新
- 给下游设置并发上限

**缓存雪崩**

大量 Key 在同一时间过期，或整个 Redis 不可用，流量同时落到数据库。

可选措施

- TTL 增加随机抖动
- 分批预热和刷新
- Redis 高可用
- 数据库限流、熔断和降级
- 应用能够绕过缓存继续工作，但控制回源并发

### 2.7.5 Redis 故障时系统应该怎样表现

对任务状态缓存，Redis 故障不应让任务事实消失。

```text
Redis GET 超时
→ 在严格超时和限流保护下查询 PostgreSQL
→ 暂时跳过缓存回填
```

但对限流和锁，故障策略需要业务选择

- Fail-open：Redis 故障时放行，可用性高但保护减弱
- Fail-closed：Redis 故障时拒绝，安全性高但可用性下降

例如普通查询限流可以短时 fail-open；高成本模型调用配额可能要 fail-closed 或使用本地保守额度。

没有一种策略适合所有接口。

### 2.7.6 缓存命中率越高越好吗

不一定。

缓存命中率高可能只是缓存了大量本来就很便宜的查询；命中率低也可能是数据访问本来就高度随机。

应一起观察

- 命中率
- Redis P95/P99 延迟
- 数据库减少了多少负载
- 回源并发
- 热 Key 和大 Key
- 缓存内存与淘汰率
- 数据陈旧时间

缓存的目标是降低整体成本并满足一致性要求，而不是追求一个孤立百分比。

## 2.8 Redis Stream 为什么比 Pub/Sub 更适合任务分发

### 2.8.1 Pub/Sub 的问题不是慢，而是没有消息记忆

Redis Pub/Sub 像现场直播

```text
发布时在线
→ 有机会收到

发布时断线
→ 消息不会等你回来
```

它是 at-most-once 语义：消息最多交付一次，网络断开或消费者处理失败后不会自动重放。

因此 Pub/Sub 适合

- 在线进度通知
- 可丢失的缓存失效提示
- 实时 UI 刷新信号

不适合把核心 Agent 任务的唯一副本交给它。

### 2.8.2 Stream 是什么

Redis Stream 是追加式日志结构。每条消息有唯一 ID 和字段集合。

```text
XADD evo:agent:tasks * task_id 6c8a event_type agent.task.created
```

与 Pub/Sub 相比，消息会保留在 Stream 中，消费者可以按 ID 读取历史。

生产时要设置裁剪或保留策略，否则 Stream 会持续增长。

### 2.8.3 消费者组怎样让多个 Worker 分工

假设有三个 Worker 属于同一个 `agent-workers` 组

```text
Redis Stream
  ├─ 消息 1 → Worker A
  ├─ 消息 2 → Worker B
  └─ 消息 3 → Worker C
```

同一个组内，每条新消息交给某个消费者处理；不同消费者组则可以各自读取同一 Stream。

例如

```text
agent-workers       执行任务
audit-indexers      建立审计索引
metrics-consumers   统计任务指标
```

三个组互不替代，各自维护消费进度。

### 2.8.4 PEL 和 ACK 为什么重要

Worker 用 `XREADGROUP` 收到消息后，消息进入 Pending Entries List，简称 PEL。

```text
收到消息
→ 进入 PEL
→ 执行业务
→ 数据库提交成功
→ XACK
→ 从该消费者组的 PEL 移除引用
```

如果 Worker 收到消息后崩溃，没有 ACK，消息仍在 PEL。其他 Worker 可以检查空闲过久的消息，并用 `XAUTOCLAIM` 或 `XCLAIM` 重新领取。

这比 List 的 `POP` 更适合需要确认和故障恢复的任务。

### 2.8.5 为什么有 ACK 仍然会重复执行

考虑这个时间点

```text
Worker 已经把任务结果提交 PostgreSQL
        ↓
还没来得及 XACK 就崩溃
        ↓
消息被另一个 Worker 重新领取
```

新 Worker 会再次看到同一消息。

所以可靠消费者必须幂等。常见方式是数据库条件更新

```sql
UPDATE agent_tasks
SET status = 'succeeded', result = :result
WHERE id = :task_id
  AND status = 'running'
  AND lease_owner = :worker_id;
```

如果任务已经是终态，重复消息只需 ACK，不重复产生外部副作用。

更准确的生产目标通常是

```text
消息至少投递一次
+
消费者幂等
≈
业务效果只发生一次
```

不要轻易宣称“消息绝对只消费一次”。

### 2.8.6 Stream 持久化后是否等于 Kafka

不等于。

Redis Stream 适合低延迟、保留期较短、已经运维 Redis 的任务流。Kafka 更擅长大规模分区日志、较长保留、吞吐和生态集成；RabbitMQ 更强调消息路由、确认、死信与传统工作队列能力。

选择时比较

|   维度    | Redis Streams | RabbitMQ  |        Kafka        |
| :-----: | :-----------: | :-------: | :-----------------: |
|  典型定位   |  内存数据平台中的事件流  | 消息代理和工作队列 |       分布式持久日志       |
|   延迟    |       低       |     低     |       通常偏吞吐型        |
|  消费者组   |      支持       |  支持队列消费者  |        核心能力         |
|  路由能力   |     相对简单      |    丰富     | 主要按 Topic/Partition |
| 长期保留与重放 |   可做但需控制内存    |  不是主要优势   |         强项          |
|  运维复杂度  | 已有 Redis 时较低  |    中等     |        通常更高         |

组件名称不是结论。要根据吞吐、保留期、丢失容忍度、顺序要求、积压规模和团队运维能力选择。

### 2.8.7 Redis Stream 也有可靠性边界

Stream 和消费者组状态可以进入 RDB/AOF 并复制到副本，但它们仍遵循 Redis 的持久化和复制语义。

```text
XADD 已返回成功
→ 不一定已经 fsync 到磁盘
→ 异步副本也不一定已经收到
```

如果任务消息非常关键，应明确

- AOF 和 fsync 策略
- 主从复制和故障切换窗口
- 是否使用 `WAIT` 降低复制丢失概率
- Redis 内存、淘汰和 Stream 裁剪策略
- Outbox 是否能够重投
- 是否需要专用消息系统

即使使用 `WAIT`，也不应承诺在所有故障条件下绝对不丢。

## 2.9 限流、幂等和分布式锁为什么不能混为一谈

### 2.9.1 限流在限制什么

限流保护的是单位时间内的请求或资源消耗。

```text
每个用户每分钟最多创建 20 个任务
每个租户最多同时运行 50 个任务
每个模型每天最多消耗一定 Token
```

它解决容量和公平性，不负责判断两个请求是不是同一次业务操作。

### 2.9.2 最简单的固定窗口限流

思路是按分钟生成 Key

```text
rate:create-task:tenant-a:202608301530
```

每次请求原子自增并设置过期时间。

如果 `INCR` 和 `EXPIRE` 分成两条普通命令，进程可能在中间崩溃，留下永不过期的 Key。因此应使用 Lua 或可靠的原子组合。

```lua
local current = redis.call('INCR', KEYS[1])
if current == 1 then
    redis.call('EXPIRE', KEYS[1], ARGV[1])
end
return current
```

固定窗口简单，但在窗口边界可能突发

```text
12:00:59 发 20 次
12:01:00 再发 20 次
→ 两秒内通过 40 次
```

更平滑的方案包括滑动窗口、漏桶和令牌桶。

### 2.9.3 幂等是在识别“是不是同一件事”

客户端超时后可能不知道创建请求是否成功，于是重试。

```text
第一次请求：服务端已创建任务，但响应丢失
第二次请求：如果再次创建，就出现重复任务
```

客户端提供 `Idempotency-Key`，服务端用

```sql
UNIQUE (tenant_id, idempotency_key)
```

确保同一个业务请求只创建一个任务。

Redis 的 `SET key value NX EX` 可以快速拦截重复请求，但键会过期、可能被淘汰，也可能在数据库提交前后发生故障。因此它适合快速路径，不应替代 PostgreSQL 唯一约束这个正确性底线。

### 2.9.4 分布式锁是在竞争一段时间的执行权

多个进程可能同时刷新同一个热点缓存，或者同时执行一个不能并发的短操作。

Redis 单实例锁常见获取方式

```text
SET lock:resource random-token NX PX 30000
```

- `NX`：Key 不存在时才成功
- `PX 30000`：30 秒后自动过期，避免持有者崩溃后永久死锁
- `random-token`：标识这次锁的所有者

释放时不能直接 `DEL`

```text
客户端 A 获得锁
→ A 卡顿，锁过期
→ B 获得新锁
→ A 恢复并直接 DEL
→ A 错删了 B 的锁
```

必须比较 Token 后再删除，Redis 8.4 之前通常使用 Lua 原子执行

```lua
if redis.call('GET', KEYS[1]) == ARGV[1] then
    return redis.call('DEL', KEYS[1])
end
return 0
```

### 2.9.5 有 TTL 的锁为什么仍然可能不安全

锁本质上是有期限的租约。

```text
Worker A 获得 30 秒锁
→ 发生长时间 GC、网络暂停或任务执行超时
→ 锁到期
→ Worker B 获得锁并开始执行
→ Worker A 恢复，也继续写资源
```

此时两个 Worker 都可能认为自己有执行权。

高风险外部资源可以使用 fencing token

```text
每次获得租约同时得到递增编号
A 的 token = 41
B 的 token = 42

资源端只接受不小于已见最大 token 的写入
→ A 恢复后的旧 token 41 被拒绝
```

如果被保护资源不检查 fencing token，锁客户端单方面无法阻止过期持有者继续写。

因此 Redis 锁适合短期协调，不替代数据库唯一约束、条件更新和资源端校验。

### 2.9.6 三者怎样区分

|  机制  |      核心问题       |   典型 Key 或约束   |
| :--: | :-------------: | :------------: |
|  限流  |   这段时间是否请求太多    |   计数器 + 时间窗口   |
|  幂等  | 这是不是已经处理过的同一次请求 |   业务键 + 唯一约束   |
| 分布式锁 |  当前谁暂时有权执行临界区   | 所有者 Token + 租约 |

一次创建任务请求可能同时需要三者，但它们不能互相代替。

## 2.10 PostgreSQL 与 Redis 怎样组成一条可靠任务链

### 2.10.1 最危险的地方：数据库和队列双写

直觉代码是

```text
INSERT PostgreSQL task
XADD Redis Stream
```

但两个系统没有共同事务，会出现

|       失败点       |         结果         |
| :-------------: | :----------------: |
| 数据库提交成功，XADD 失败 | 任务存在但 Worker 永远收不到 |
|  XADD 成功，数据库回滚  | Worker 收到一个不存在的任务  |
| XADD 成功，响应前进程崩溃 |    调用方重试，可能重复创建    |

把顺序反过来也只是交换失败类型，不能消除双写问题。

### 2.10.2 Transactional Outbox 为什么有效

Outbox 的关键是：先把任务和“将来要发送的事件”写进同一个 PostgreSQL 事务。

```text
FastAPI / Service
        ↓
BEGIN PostgreSQL
  INSERT agent_tasks
  INSERT outbox_events
COMMIT
        ↓
Relay 扫描 outbox_events
        ↓
XADD Redis Stream
        ↓
标记 published_at
```

只要任务创建成功，Outbox 事件就一定存在。即使 Redis 暂时不可用，Relay 以后还能重试。

### 2.10.3 Outbox 为什么带来重复，而不是恰好一次

Relay 可能执行到这里崩溃

```text
XADD 已成功
→ 还没把 published_at 提交 PostgreSQL
→ Relay 重启后再次 XADD
```

所以 Outbox 解决的是“不丢失数据库已提交的发送意图”，通常提供至少一次投递，需要消费者幂等处理重复。

事件必须有稳定 `event_id`。消费者可以保存已处理事件，或通过任务状态条件更新确保重复消息不产生重复业务效果。

### 2.10.4 为什么 Worker 还需要租约

`running` 只说明曾有 Worker 开始执行，不能说明它现在还活着。

任务领取时写入

```text
lease_owner = worker-7
lease_until = 当前时间 + 60 秒
```

长任务需要心跳续租。

如果 Worker 崩溃，租约到期后其他 Worker 才能重新领取。

提交结果时检查 `lease_owner`，防止失去租约的旧 Worker 覆盖新 Worker 结果。

状态机可以简化为

```text
pending ──领取──> running ──成功──> succeeded
                       ├──失败──> failed
                       └──取消──> cancelled

running + lease 过期 ──重领──> running
```

状态迁移必须使用条件更新，而不是先查状态再无条件写。

### 2.10.5 一次查询怎样经过 Redis 和 PostgreSQL

```text
GET /tasks/{id}
        ↓
认证得到 tenant_id
        ↓
GET Redis cache key
   ┌────┴────┐
 命中       未命中或 Redis 超时
  ↓           ↓
返回       SELECT PostgreSQL
                ↓
        检查任务属于当前租户
                ↓
         短 TTL 写回 Redis
                ↓
              返回
```

缓存 Key 中必须包含租户，数据库查询也必须带 `tenant_id`。不能先按任务 ID 读取缓存，再在应用层补权限判断，否则容易发生跨租户数据泄漏。

### 2.10.6 故障发生时系统应该怎样恢复

|          故障           |         期望行为         |
| :-------------------: | :------------------: |
|     API 在事务提交前崩溃      |    事务回滚，客户端可安全重试     |
|      API 提交后响应丢失      |       幂等键返回原任务       |
|      Redis 暂时不可用      | Outbox 保留，Relay 稍后重试 |
|   Relay 在 XADD 后崩溃    |     可能重复投递，消费者幂等     |
|    Worker 收到消息后崩溃     |   消息留在 PEL，租约到期后认领   |
| Worker 提交结果后、ACK 前崩溃  |   重放发现任务已终态，只 ACK    |
|      Redis 缓存丢失       |   从 PostgreSQL 重建    |
| Redis Stream 因持久化窗口丢失 | 依赖持久化配置、对账重投或换更强消息系统 |
|    PostgreSQL 不可用     | 停止接受需要持久化的新任务，不能假装成功 |

可靠性不是某个框架开关，而是每个失败点都有明确的重试、幂等和恢复路径。

### 2.10.7 哪些数据是事实，哪些数据可以重建

```text
事实数据
├─ agent_tasks
├─ task_events
├─ 审批与审计
├─ 幂等请求
└─ 结果元数据

可重建或短期数据
├─ 任务状态缓存
├─ 限流计数
├─ 在线通知
├─ 短期锁
└─ 派生排行榜
```

这个分类会直接决定备份、TTL、淘汰、故障降级和监控策略。

## 2.11 把主线落成一个最小可运行示例

这个示例演示

```text
FastAPI 创建任务
→ PostgreSQL 原子写任务与 Outbox
→ Relay 投递 Redis Stream
→ Worker 消费并提交结果
→ 查询接口使用 Cache-Aside
```

它专门保留了生产系统最关键的边界，但为了便于学习，省略了真实认证、数据库迁移框架、Worker 心跳、死信队列和完整可观测性。

### 2.11.1 目录结构

```text
demo/
├── compose.yaml
├── requirements.txt
├── schema.sql
└── app.py
```

### 2.11.2 启动 PostgreSQL 和 Redis

`compose.yaml`

```yaml
services:
  postgres:
	image: postgres:16-alpine
    environment:
      POSTGRES_DB: evoagent
      POSTGRES_USER: app
      POSTGRES_PASSWORD: app
    ports:
      - "5432:5432"
    volumes:
      - postgres-data:/var/lib/postgresql/data
      - ./schema.sql:/docker-entrypoint-initdb.d/001-schema.sql:ro
    healthcheck:
      test: ["CMD-SHELL", "pg_isready -U app -d evoagent"]
      interval: 5s
      timeout: 3s
      retries: 10

  redis:
    image: redis:7.4-alpine
    command:
      - redis-server
      - --appendonly
      - "yes"
      - --appendfsync
      - everysec
    ports:
      - "6379:6379"
    volumes:
      - redis-data:/data
    healthcheck:
      test: ["CMD", "redis-cli", "ping"]
      interval: 5s
      timeout: 3s
      retries: 10

volumes:
  postgres-data:
  redis-data:
```

这里给 Redis 开启 AOF，是为了演示任务流不应使用纯缓存配置。但 `everysec` 仍然存在小的数据丢失窗口，不能理解成绝对持久。

### 2.11.3 创建表和索引

`schema.sql`

```sql
CREATE TABLE agent_tasks (
    id UUID PRIMARY KEY,
    tenant_id TEXT NOT NULL,
    idempotency_key TEXT NOT NULL,
    instruction TEXT NOT NULL
        CHECK (char_length(instruction) BETWEEN 1 AND 2000),
    status TEXT NOT NULL DEFAULT 'pending'
        CHECK (status IN ('pending', 'running', 'succeeded', 'failed', 'cancelled')),
    result JSONB,
    error_message TEXT,
    attempt_count INTEGER NOT NULL DEFAULT 0
        CHECK (attempt_count >= 0),
    version INTEGER NOT NULL DEFAULT 0
        CHECK (version >= 0),
    lease_owner TEXT,
    lease_until TIMESTAMPTZ,
    created_at TIMESTAMPTZ NOT NULL DEFAULT now(),
    updated_at TIMESTAMPTZ NOT NULL DEFAULT now(),
    UNIQUE (tenant_id, idempotency_key)
);

CREATE TABLE task_events (
    id BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY,
    task_id UUID NOT NULL REFERENCES agent_tasks(id),
    event_type TEXT NOT NULL,
    payload JSONB NOT NULL DEFAULT '{}'::jsonb,
    created_at TIMESTAMPTZ NOT NULL DEFAULT now()
);

CREATE TABLE outbox_events (
    id UUID PRIMARY KEY,
    aggregate_id UUID NOT NULL REFERENCES agent_tasks(id),
    event_type TEXT NOT NULL,
    payload JSONB NOT NULL,
    created_at TIMESTAMPTZ NOT NULL DEFAULT now(),
    published_at TIMESTAMPTZ,
    publish_attempts INTEGER NOT NULL DEFAULT 0
);

CREATE INDEX idx_tasks_tenant_created
ON agent_tasks (tenant_id, created_at DESC, id DESC);

CREATE INDEX idx_tasks_active_lease
ON agent_tasks (lease_until, id)
WHERE status IN ('pending', 'running');

CREATE INDEX idx_outbox_unpublished
ON outbox_events (created_at, id)
WHERE published_at IS NULL;

CREATE INDEX idx_task_events_task_created
ON task_events (task_id, created_at, id);
```

如果已经启动过 Compose 并保留了旧数据卷，初始化脚本不会再次自动执行。学习环境可以显式删除该演示数据卷后重建；生产环境必须使用迁移工具，不能靠删除数据卷。

### 2.11.4 安装依赖

`requirements.txt`

```text
fastapi>=0.115,<1
uvicorn[standard]>=0.30,<1
SQLAlchemy[asyncio]>=2.0,<3
asyncpg>=0.29,<1
redis>=5,<9
pydantic>=2,<3
```

范围版本方便展示，但正式项目应在测试后锁定完整依赖和哈希，避免部署时自动获得未经回归的新版本。

### 2.11.5 完整 Python 代码

`app.py`

```python
from __future__ import annotations

import asyncio
import json
import os
import sys
from contextlib import asynccontextmanager
from datetime import datetime
from typing import Any, AsyncIterator
from uuid import UUID, uuid4

import redis.asyncio as redis_async
from fastapi import FastAPI, Header, HTTPException, status
from pydantic import BaseModel, Field
from redis.exceptions import RedisError, ResponseError
from sqlalchemy import text
from sqlalchemy.ext.asyncio import (
    AsyncSession,
    async_sessionmaker,
    create_async_engine,
)


DATABASE_URL = os.getenv(
    "DATABASE_URL",
    "postgresql+asyncpg://app:app@127.0.0.1:5432/evoagent",
)
REDIS_URL = os.getenv("REDIS_URL", "redis://127.0.0.1:6379/0")

STREAM_KEY = "evo:agent:tasks"
CONSUMER_GROUP = "agent-workers"
DEMO_TENANT_ID = "tenant-demo"
CACHE_TTL_SECONDS = 30
LEASE_SECONDS = 60


engine = create_async_engine(
    DATABASE_URL,
    pool_size=5,
    max_overflow=5,
    pool_pre_ping=True,
)
SessionFactory = async_sessionmaker(engine, expire_on_commit=False)
redis_client = redis_async.from_url(REDIS_URL, decode_responses=True)


class TaskCreate(BaseModel):
    instruction: str = Field(min_length=1, max_length=2000)


class TaskView(BaseModel):
    id: UUID
    status: str
    result: dict[str, Any] | None = None
    error_message: str | None = None
    version: int
    created_at: datetime
    updated_at: datetime


def cache_key(tenant_id: str, task_id: UUID | str) -> str:
    return f"evo:{tenant_id}:task:{task_id}:status"


def row_to_json(row: dict[str, Any]) -> str:
    payload = {
        "id": str(row["id"]),
        "status": row["status"],
        "result": row["result"],
        "error_message": row["error_message"],
        "version": row["version"],
        "created_at": row["created_at"].isoformat(),
        "updated_at": row["updated_at"].isoformat(),
    }
    return json.dumps(payload, ensure_ascii=False)


async def fetch_task(
    session: AsyncSession,
    tenant_id: str,
    task_id: UUID,
) -> dict[str, Any] | None:
    result = await session.execute(
        text(
            """
            SELECT id, status, result, error_message,
                   version, created_at, updated_at
            FROM agent_tasks
            WHERE tenant_id = :tenant_id AND id = :task_id
            """
        ),
        {"tenant_id": tenant_id, "task_id": task_id},
    )
    row = result.mappings().one_or_none()
    return dict(row) if row is not None else None


async def create_task_in_transaction(
    tenant_id: str,
    idempotency_key: str,
    instruction: str,
) -> dict[str, Any]:
    task_id = uuid4()
    event_id = uuid4()

    async with SessionFactory.begin() as session:
        inserted = await session.execute(
            text(
                """
                INSERT INTO agent_tasks (
                    id, tenant_id, idempotency_key, instruction
                )
                VALUES (
                    :task_id, :tenant_id, :idempotency_key, :instruction
                )
                ON CONFLICT (tenant_id, idempotency_key) DO NOTHING
                RETURNING id
                """
            ),
            {
                "task_id": task_id,
                "tenant_id": tenant_id,
                "idempotency_key": idempotency_key,
                "instruction": instruction,
            },
        )
        created_id = inserted.scalar_one_or_none()

        if created_id is not None:
            await session.execute(
                text(
                    """
                    INSERT INTO outbox_events (
                        id, aggregate_id, event_type, payload
                    )
                    VALUES (
                        :event_id,
                        :task_id,
                        'agent.task.created',
                        CAST(:payload AS JSONB)
                    )
                    """
                ),
                {
                    "event_id": event_id,
                    "task_id": task_id,
                    "payload": json.dumps(
                        {"task_id": str(task_id), "tenant_id": tenant_id}
                    ),
                },
            )
        else:
            existing = await session.execute(
                text(
                    """
                    SELECT id
                    FROM agent_tasks
                    WHERE tenant_id = :tenant_id
                      AND idempotency_key = :idempotency_key
                    """
                ),
                {
                    "tenant_id": tenant_id,
                    "idempotency_key": idempotency_key,
                },
            )
            task_id = existing.scalar_one()

        task = await fetch_task(session, tenant_id, task_id)
        if task is None:
            raise RuntimeError("task disappeared inside transaction")
        return task


@asynccontextmanager
async def lifespan(_: FastAPI) -> AsyncIterator[None]:
    async with engine.connect() as connection:
        await connection.execute(text("SELECT 1"))
    try:
        await redis_client.ping()
    except RedisError:
        # Redis 只负责缓存和异步投递，API 仍可先把任务写入 Outbox。
        pass
    yield
    await redis_client.aclose()
    await engine.dispose()


app = FastAPI(lifespan=lifespan)


@app.post(
    "/tasks",
    response_model=TaskView,
    status_code=status.HTTP_202_ACCEPTED,
)
async def create_task(
    command: TaskCreate,
    idempotency_key: str = Header(
        min_length=8,
        max_length=128,
        alias="Idempotency-Key",
    ),
) -> TaskView:
    task = await create_task_in_transaction(
        tenant_id=DEMO_TENANT_ID,
        idempotency_key=idempotency_key,
        instruction=command.instruction,
    )
    return TaskView.model_validate(task)


@app.get("/tasks/{task_id}", response_model=TaskView)
async def get_task(task_id: UUID) -> TaskView:
    key = cache_key(DEMO_TENANT_ID, task_id)

    try:
        cached = await asyncio.wait_for(redis_client.get(key), timeout=0.05)
    except (asyncio.TimeoutError, RedisError):
        cached = None

    if cached is not None:
        return TaskView.model_validate_json(cached)

    async with SessionFactory() as session:
        task = await fetch_task(session, DEMO_TENANT_ID, task_id)

    if task is None:
        raise HTTPException(status_code=404, detail="task not found")

    try:
        await asyncio.wait_for(
            redis_client.set(key, row_to_json(task), ex=CACHE_TTL_SECONDS),
            timeout=0.05,
        )
    except (asyncio.TimeoutError, RedisError):
        pass

    return TaskView.model_validate(task)


async def relay_once() -> bool:
    async with SessionFactory.begin() as session:
        selected = await session.execute(
            text(
                """
                SELECT id, aggregate_id, event_type, payload
                FROM outbox_events
                WHERE published_at IS NULL
                ORDER BY created_at, id
                FOR UPDATE SKIP LOCKED
                LIMIT 1
                """
            )
        )
        event = selected.mappings().one_or_none()
        if event is None:
            return False

        await redis_client.xadd(
            STREAM_KEY,
            {
                "event_id": str(event["id"]),
                "task_id": str(event["aggregate_id"]),
                "event_type": event["event_type"],
                "payload": json.dumps(event["payload"]),
            },
        )

        await session.execute(
            text(
                """
                UPDATE outbox_events
                SET published_at = now(),
                    publish_attempts = publish_attempts + 1
                WHERE id = :event_id
                """
            ),
            {"event_id": event["id"]},
        )
        return True


async def run_relay() -> None:
    try:
        while True:
            try:
                published = await relay_once()
                if not published:
                    await asyncio.sleep(0.2)
            except RedisError:
                # relay_once 的数据库事务会回滚，事件仍可稍后重试。
                await asyncio.sleep(1)
    finally:
        await redis_client.aclose()
        await engine.dispose()


async def ensure_consumer_group() -> None:
    try:
        await redis_client.xgroup_create(
            STREAM_KEY,
            CONSUMER_GROUP,
            id="0-0",
            mkstream=True,
        )
    except ResponseError as exc:
        if "BUSYGROUP" not in str(exc):
            raise


async def claim_task(task_id: UUID, worker_id: str) -> bool:
    async with SessionFactory.begin() as session:
        result = await session.execute(
            text(
                """
                UPDATE agent_tasks
                SET status = 'running',
                    lease_owner = :worker_id,
                    lease_until = now() + (:lease_seconds * interval '1 second'),
                    attempt_count = attempt_count + 1,
                    version = version + 1,
                    updated_at = now()
                WHERE id = :task_id
                  AND (
                      status = 'pending'
                      OR (
                          status = 'running'
                          AND lease_until < now()
                      )
                  )
                RETURNING id
                """
            ),
            {
                "task_id": task_id,
                "worker_id": worker_id,
                "lease_seconds": LEASE_SECONDS,
            },
        )
        claimed = result.scalar_one_or_none() is not None

        if claimed:
            await session.execute(
                text(
                    """
                    INSERT INTO task_events (task_id, event_type, payload)
                    VALUES (
                        :task_id,
                        'task.running',
                        CAST(:payload AS JSONB)
                    )
                    """
                ),
                {
                    "task_id": task_id,
                    "payload": json.dumps({"worker_id": worker_id}),
                },
            )
        return claimed


async def task_status(task_id: UUID) -> str | None:
    async with SessionFactory() as session:
        result = await session.execute(
            text("SELECT status FROM agent_tasks WHERE id = :task_id"),
            {"task_id": task_id},
        )
        return result.scalar_one_or_none()


async def execute_agent(task_id: UUID) -> dict[str, Any]:
    await asyncio.sleep(2)
    return {
        "summary": f"demo result for task {task_id}",
        "evidence_ids": [],
    }


async def finish_task(
    task_id: UUID,
    worker_id: str,
    result_payload: dict[str, Any],
) -> bool:
    async with SessionFactory.begin() as session:
        updated = await session.execute(
            text(
                """
                UPDATE agent_tasks
                SET status = 'succeeded',
                    result = CAST(:result AS JSONB),
                    lease_owner = NULL,
                    lease_until = NULL,
                    version = version + 1,
                    updated_at = now()
                WHERE id = :task_id
                  AND status = 'running'
                  AND lease_owner = :worker_id
                RETURNING id
                """
            ),
            {
                "task_id": task_id,
                "worker_id": worker_id,
                "result": json.dumps(result_payload, ensure_ascii=False),
            },
        )
        finished = updated.scalar_one_or_none() is not None

        if finished:
            await session.execute(
                text(
                    """
                    INSERT INTO task_events (task_id, event_type, payload)
                    VALUES (
                        :task_id,
                        'task.succeeded',
                        '{}'::jsonb
                    )
                    """
                ),
                {"task_id": task_id},
            )
        return finished


async def process_message(
    worker_id: str,
    message_id: str,
    fields: dict[str, str],
) -> None:
    task_id = UUID(fields["task_id"])
    claimed = await claim_task(task_id, worker_id)

    if not claimed:
        current_status = await task_status(task_id)
        if current_status in {"succeeded", "failed", "cancelled"}:
            await redis_client.xack(
                STREAM_KEY,
                CONSUMER_GROUP,
                message_id,
            )
        return

    result_payload = await execute_agent(task_id)
    finished = await finish_task(task_id, worker_id, result_payload)

    if finished:
        await redis_client.delete(cache_key(DEMO_TENANT_ID, task_id))
        await redis_client.xack(
            STREAM_KEY,
            CONSUMER_GROUP,
            message_id,
        )


async def run_worker() -> None:
    worker_id = f"worker-{uuid4()}"
    await ensure_consumer_group()

    try:
        while True:
            messages = await redis_client.xreadgroup(
                groupname=CONSUMER_GROUP,
                consumername=worker_id,
                streams={STREAM_KEY: ">"},
                count=10,
                block=5000,
            )

            for _, entries in messages:
                for message_id, fields in entries:
                    await process_message(worker_id, message_id, fields)
    finally:
        await redis_client.aclose()
        await engine.dispose()


def main() -> None:
    if len(sys.argv) != 2 or sys.argv[1] not in {"relay", "worker"}:
        raise SystemExit("usage: python app.py [relay|worker]")

    target = run_relay if sys.argv[1] == "relay" else run_worker
    asyncio.run(target())


if __name__ == "__main__":
    main()
```

### 2.11.6 按任务流程理解代码

**第一步：创建 Engine、Session 工厂和 Redis Client**

```text
engine
→ 当前进程长期持有的数据库连接池

SessionFactory
→ 每次业务操作创建独立 AsyncSession

redis_client
→ 复用 Redis 连接池，不在每次请求中重新连接
```

每个 Uvicorn 进程都会各自创建这些对象，所以总连接数要乘以进程数。

**第二步：API 创建任务**

`create_task_in_transaction()` 使用 `SessionFactory.begin()` 建立短事务。

```text
INSERT agent_tasks
→ ON CONFLICT 处理相同幂等键
→ 新任务才 INSERT outbox_events
→ 统一 COMMIT
```

相同幂等键重试时，查询并返回已有任务，不会创建第二个 Outbox 事件。

演示代码固定使用 `tenant-demo`。生产中 `tenant_id` 必须来自已验证身份，不能相信客户端在 Body 中随便提交的租户 ID。

**第三步：Relay 投递 Outbox**

Relay 使用 `FOR UPDATE SKIP LOCKED`，多个 Relay 可以并行处理不同事件。

它先 `XADD`，再标记 `published_at`。

如果 `XADD` 失败，数据库事务回滚，事件仍是未投递状态；如果 XADD 成功后 Relay 崩溃，事件可能重复投递，所以 Worker 必须幂等。

这里在数据库事务中等待一次 Redis 写入，是 Outbox Relay 的有意识折中。生产中应设置很短的 Redis 超时、批量大小和重试退避，避免长期持有 Outbox 行锁。

演示代码没有自动裁剪 Stream，避免在不了解消费者进度时误删未处理消息。生产环境必须监控 Stream 长度，并按保留期、所有消费者组的确认进度和 Redis 版本能力制定裁剪策略，不能只粗暴设置一个很小的 `MAXLEN`。

**第四步：Worker 领取任务**

`claim_task()` 不是先查后写，而是使用一条带条件的 `UPDATE`。

```text
pending
或者
running 但租约已过期
→ 才能被当前 Worker 领取
```

同一时刻只有成功更新的 Worker 获得数据库层执行权。

**第五步：事务外执行 Agent**

`execute_agent()` 用两秒等待模拟模型调用。

它不处于数据库事务中，因此不会在等待模型时占用一个打开的事务和行锁。

真实长任务需要定期续租，并让取消、超时和 Tool 副作用可恢复。演示任务短于租约，所以省略心跳。

**第六步：提交结果并 ACK**

`finish_task()` 要求

```text
status = running
+
lease_owner = 当前 Worker
```

旧 Worker 即使恢复，也不能覆盖新 Worker 的结果。

数据库提交成功后删除缓存，再 `XACK`。如果提交后、ACK 前崩溃，消息会重放，但终态任务不会再次执行。

### 2.11.7 怎样运行

启动基础设施

```bash
docker compose up -d
```

创建虚拟环境并安装依赖

```bash
python3 -m venv .venv
source .venv/bin/activate
python3 -m pip install -r requirements.txt
```

打开三个终端。

终端一启动 API

```bash
uvicorn app:app --reload
```

终端二启动 Relay

```bash
python3 app.py relay
```

终端三启动 Worker

```bash
python3 app.py worker
```

提交任务

```bash
curl -i \
  -X POST http://127.0.0.1:8000/tasks \
  -H 'Content-Type: application/json' \
  -H 'Idempotency-Key: order-analysis-001' \
  -d '{"instruction":"分析最近七天订单失败原因"}'
```

返回 `202` 和任务 ID 后查询

```bash
curl http://127.0.0.1:8000/tasks/替换为任务ID
```

重复提交同一个 `Idempotency-Key`，应返回同一个任务 ID。

### 2.11.8 这个示例离生产还差什么

|      演示中省略       |                 生产需要补充                  |
| :--------------: | :-------------------------------------: |
|       固定租户       |   JWT/OAuth2 认证、租户和 Resource Scope 授权   |
|    Worker 无心跳    |              续租、超时、取消和失联恢复              |
|      只读取新消息      |      `XAUTOCLAIM` 恢复长期 Pending 消息       |
|     没有失败状态代码     |             错误分类、受控重试、退避和死信             |
|   结果直接放 JSONB    |           大结果放对象存储，数据库存摘要和地址            |
|  单个 Outbox 事件处理  |             小批量、超时、积压指标和告警              |
|   Redis 只有单实例    |           高可用、内存预算、淘汰隔离和持久化演练           |
| `schema.sql` 初始化 |              Alembic 版本化迁移              |
|     没有 Trace     | request_id、task_id、event_id、trace_id 串联 |
|       没有对账       |      扫描长期 pending/running 和投递异常任务       |

尤其要注意：演示 Worker 没有 `XAUTOCLAIM`。如果 Worker 收到消息后崩溃，消息会留在 PEL，需要恢复协程认领空闲过久的消息。实现时还要让 Redis 的认领超时和 PostgreSQL 租约语义协调，不能只恢复其中一个。

## 2.12 怎样测试和排查这条数据链路

### 2.12.1 不要只测试“正常创建成功”

最少要覆盖

|         测试         |               期望结果                |
| :----------------: | :-------------------------------: |
|    两次相同幂等键并发提交     |        只有一个任务和一个 Outbox 事件        |
|   Redis 关闭后创建任务    | PostgreSQL 仍保存任务和 Outbox，Relay 重试 |
|  Relay 在 XADD 后崩溃  |          允许重复消息，任务效果不重复           |
|    Worker 执行中崩溃    |           消息保留，租约到期后可恢复           |
| Worker 提交后 ACK 前崩溃 |           重放只 ACK，不重复执行           |
|    Redis 缓存全部清空    |            查询回源数据库并重建             |
|      缓存保存旧版本       |       TTL 或版本机制最终纠正，不覆盖新版本        |
|    多租户查询同一任务 ID    |             非所属租户不能读取             |
|      数据库连接池耗尽      |          有超时和可观测错误，不无限等待          |
|    Outbox 大量积压     |           有积压指标、告警和扩容策略           |

### 2.12.2 数据库慢时按什么顺序排查

```text
1. 确认哪条 SQL 慢，以及 P95/P99
2. 查看是否在等连接或等锁
3. 查看事务是否过长
4. 执行 EXPLAIN (ANALYZE, BUFFERS)
5. 比较估算行数与实际行数
6. 检查索引、统计信息和数据分布
7. 检查 N+1、深分页和返回数据量
8. 检查 autovacuum、表膨胀和磁盘 I/O
9. 修改后用相同负载重新测量
```

不要一开始就扩大连接池。慢查询占用连接更久，盲目增加连接可能让数据库过载得更严重。

### 2.12.3 Redis 慢或内存高时看什么

```text
连接数和连接池等待
命令 P95/P99，而不只是平均值
慢命令和阻塞命令
Big Key：单个 Value 是否过大
Hot Key：流量是否集中在少数 Key
命中率、回源量和淘汰率
内存碎片和 fork 期间内存峰值
AOF rewrite / RDB save 对延迟的影响
Stream 长度、PEL 数量和最老 Pending 时间
```

禁止在生产大 Key 空间中随意使用阻塞式 `KEYS *`。扫描应使用 `SCAN`，并控制频率；更好的方式是提前建立指标和命名规范，不靠临时全库搜索管理 Redis。

### 2.12.4 关键监控指标应该围绕业务链路

PostgreSQL

- 连接使用率和等待时间
- 慢查询 P95/P99
- 锁等待与死锁次数
- 事务时长
- dead tuples 与 autovacuum 进度
- 主从复制延迟
- Outbox 未投递数量和最老事件年龄

Redis

- 内存使用率和 `evicted_keys`
- `expired_keys` 与缓存命中率
- 命令延迟和连接错误
- AOF/RDB 状态
- Stream 长度
- PEL 数量、最老 Pending 空闲时间
- 消费速率与生产速率差值

业务

- 任务从创建到开始的排队时延
- 任务执行耗时 P50/P95/P99
- 成功、失败、取消和重试率
- 租约过期次数
- 重复事件数量
- 每租户配额和拒绝量

只监控 CPU 和内存，无法回答用户的任务为什么一直 `pending`。

### 2.12.5 怎样分层验证代码

```text
静态层
→ Python 语法、类型检查、SQL 格式和迁移检查

单元层
→ 状态机、Key 生成、缓存序列化、重试判断

集成层
→ 使用真实 PostgreSQL 和 Redis 验证事务、唯一约束、TTL、Stream

故障层
→ 主动停止 Redis、终止 Worker、制造重复消息

性能层
→ 测吞吐、P95/P99、积压恢复速度和连接池上限
```

Mock 无法证明 PostgreSQL 锁和 Redis PEL 的真实行为。关键并发和故障语义必须使用真实组件做集成测试。

## 2.13 面试问题

### 2.13.1 PostgreSQL 的 MVCC 是什么

**参考回答**

MVCC 是多版本并发控制。更新通常产生新的行版本，不同事务根据快照和可见性规则读取适合自己的版本，因此普通读写可以较好并发。代价是旧版本需要 VACUUM 清理，长事务会阻碍回收并可能造成表膨胀。默认 Read Committed 是每条语句一个快照，同一事务的两次查询可能看到不同已提交结果。

**面试官追问**

- MVCC 是否意味着读永远不阻塞写？

不能绝对化。普通查询和普通更新通常减少相互阻塞，但显式行锁、DDL、冲突写、锁队列和某些维护操作仍可能等待。

**容易答错的地方**

只说“保存多个副本，所以没有锁”，忽略可见性、旧版本清理和写写冲突。

### 2.13.2 乐观锁和悲观锁怎样选择

**参考回答**

乐观锁用旧状态或版本号做条件更新，适合冲突较少、事务短且允许重试的场景。悲观锁用 `SELECT FOR UPDATE` 等方式先锁定资源，适合冲突明确且必须串行处理的短临界区。Agent 执行耗时长，不应长期持有数据库行锁，通常使用短事务领取加租约，再在事务外执行。

**面试官追问**

- `SKIP LOCKED` 适合普通订单查询吗？

通常不适合。它会跳过已锁行，得到的不是完整一致结果，主要用于多个消费者抢工作项。

### 2.13.3 联合索引的字段顺序怎样设计

**参考回答**

先从高频查询的过滤、排序和数据分布反推。常见模式是高价值等值过滤列在前，范围或排序列在后，并加入稳定唯一列支持 Keyset Pagination。例如租户任务列表可用 `(tenant_id, created_at DESC, id DESC)`。最后必须用 `EXPLAIN ANALYZE` 验证，不能只背最左前缀。

**面试官追问**

- 为什么有索引仍然走 Seq Scan？

表很小、条件返回比例很高、统计信息估算、随机访问成本或索引与查询不匹配时，顺序扫描可能更便宜。

**容易答错的地方**

认为建了索引就一定使用，或为每一列都建单列索引。

### 2.13.4 Cache-Aside 的一致性问题怎样处理

**参考回答**

读取时先查 Redis，未命中回源 PostgreSQL并设置短 TTL；写入时先提交 PostgreSQL，再删除或更新缓存。它默认是最终一致，仍存在旧读回填等竞态。可以用短 TTL、版本号、单飞回源、CDC 失效和强一致路径读主库来控制风险。缓存必须可丢失和可重建。

**面试官追问**

- 为什么不先删缓存再更新数据库？

删除后、数据库提交前，其他请求可能读到旧数据库值并重新填入缓存，旧值会继续存活。

### 2.13.5 缓存穿透、击穿和雪崩有什么区别

**参考回答**

穿透是查询不存在数据导致每次回源，可用短期空值缓存、Bloom Filter、鉴权和限流；击穿是单个热点 Key 到期后大量并发回源，可用 singleflight、逻辑过期或提前刷新；雪崩是大量 Key 同时到期或 Redis 整体故障，可用 TTL 抖动、分批预热、高可用、回源限流和降级。

**容易答错的地方**

只罗列方案，不说明三者分别是“不存在对象”“单个热点”和“大面积同时失效”。

### 2.13.6 Redis Pub/Sub、List 和 Stream 怎样选择

**参考回答**

Pub/Sub 是在线广播，at-most-once，断线期间消息丢失，适合可丢通知；List 能做简单阻塞队列，但消息弹出后的确认和重放能力有限；Stream 是追加日志，支持消费者组、PEL、ACK、历史读取和消息认领，更适合需要恢复的任务分发。Stream 的耐久性仍取决于 Redis 持久化、复制和裁剪配置。

**面试官追问**

- 有 Stream 和 ACK 是否就是 exactly-once？

不是。数据库提交后 ACK 前崩溃会导致重放。通常采用 at-least-once 投递加幂等消费者，追求业务效果不重复。

### 2.13.7 Redis 分布式锁怎样正确释放

**参考回答**

获取时使用 `SET key unique-token NX PX ttl`，Token 标识本次持有者，TTL 防止进程崩溃后永久死锁。释放时必须原子比较 Token 后删除，不能直接 `DEL`。还要考虑任务超过租约、续租、网络暂停和故障切换。高风险资源应配合 fencing token 或数据库条件更新，不能把 Redis 锁当作绝对正确性的唯一依据。

**面试官追问**

- 为什么设置了唯一 Token 仍不够？

Token 防止误删别人的新锁，但旧持有者在租约过期后仍可能继续操作外部资源。需要资源端识别递增 fencing token，或使用其他能验证所有权的条件写。

### 2.13.8 Outbox 解决了什么问题

**参考回答**

Outbox 解决数据库与消息系统无法共同提交的双写问题。业务数据和待发送事件先在同一 PostgreSQL 事务中提交，Relay 再异步投递 Redis Stream。这样数据库已提交的发送意图不会因 Redis 短时故障丢失。Relay 可能在发送后、标记前崩溃，所以通常是至少一次投递，消费者必须幂等。

**面试官追问**

- Outbox 是否保证 Redis 故障切换时消息绝不丢？

不自动保证。Relay 依赖消息系统对发布成功的耐久性语义。Redis 的 AOF、复制和 `WAIT` 只能在相应故障模型下降低风险；强可靠场景还要对账重投或选择更合适的消息系统。

### 2.13.9 怎样处理一个卡在 running 的 Agent 任务

**参考回答**

`running` 应配合 `lease_owner` 和 `lease_until`，Worker 周期性续租。监控发现租约过期后，恢复 Worker 先重新认领 Redis PEL 消息，再通过数据库条件更新获得新租约。结果提交必须检查租约所有者，避免旧 Worker 恢复后覆盖新结果。还要根据任务副作用决定自动重试、人工确认或进入死信。

**面试官追问**

- 所有任务都能自动重试吗？

不能。只读查询通常容易重试；付款、发邮件或执行生产变更等外部副作用需要幂等接口、操作记录、补偿或人工确认。

### 2.13.10 复习总结

```text
Python 内存会随进程重启消失，也不能跨进程共享
→ 业务状态需要外部存储

任务、权限、审计和结果不能丢
→ PostgreSQL 作为事实来源

外部入口不止 FastAPI
→ 数据库类型与约束守住最后底线

任务与待投递事件不能只成功一个
→ 使用 PostgreSQL 事务和 Outbox

并发事务需要看到合理的数据版本
→ PostgreSQL 使用 MVCC 和隔离级别

多个 Worker 可能同时领取任务
→ 使用条件更新、行锁或租约控制竞争

模型和 Tool 调用耗时不可控
→ 不把远程调用放进长数据库事务

数据量增长后全表扫描成本高
→ 从真实查询反推索引，并用执行计划验证

数据库连接建立和并发连接都有成本
→ 使用连接池，并按进程总数计算预算

热点状态反复查询会消耗数据库
→ Redis + Cache-Aside，但数据库仍是事实来源

缓存可能过期、淘汰或故障
→ 缓存必须有 TTL、可降级、可重建

Pub/Sub 不保存离线消息
→ 核心任务使用 Stream 或专用消息系统

Worker 提交结果后可能来不及 ACK
→ 接受至少一次投递，让消费者幂等

数据库与 Redis 无法共同事务
→ Outbox 保留发送意图，Relay 负责重试

running 不代表 Worker 仍然存活
→ 使用租约、心跳、PEL 恢复和条件提交

限流、幂等和锁解决不同问题
→ 分别设计，不能互相替代

Redis 持久化和复制都有明确边界
→ 按业务丢失容忍度配置，并进行故障演练
```

1. PostgreSQL 保存业务事实，Redis 保存可重建的高速运行时数据
2. 数据库约束是所有写入入口共享的最后防线
3. 事务边界应覆盖完整业务用例，但不覆盖模型和 Tool 等长耗时调用
4. MVCC 提高读写并发，也带来旧版本清理和长事务问题
5. 索引从查询和数据分布反推，不是越多越好
6. 连接池按所有进程和副本计算，池耗尽本身也是背压信号
7. Cache-Aside 默认是最终一致，缓存必须允许丢失和回源
8. TTL 到期和内存淘汰是两种不同的数据消失机制
9. Redis Streams 支持消费者组、PEL 和 ACK，但不自动提供 exactly-once
10. Outbox 把数据库已提交的发送意图可靠保存下来，重复由幂等消费者处理
11. Redis 锁是有期限的租约，不能替代唯一约束、条件更新和 fencing token
12. 生产可靠性来自对双写、重复、崩溃、积压、超时和恢复路径的逐一设计
