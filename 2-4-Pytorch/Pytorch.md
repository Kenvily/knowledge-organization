<font size = 6>Pytorch</font>

[toc]

# 1 PyTorch 整体认识

从工程角度看，PyTorch 主要解决以下问题

```text
如何表示数据
    ↓
如何描述模型计算
    ↓
如何自动计算梯度
    ↓
如何更新模型参数
    ↓
如何利用 CPU / GPU 高效执行
    ↓
如何进行分布式训练、编译和部署
```

## 1.1 PyTorch 是什么

### 1.1.1 基本定位

PyTorch 是一个以 Tensor 计算和自动求导为基础的机器学习框架。

它主要提供以下能力

1. 使用 Tensor 表示和处理多维数据
2. 在 CPU、GPU 等计算设备上执行数值计算
3. 动态记录模型的计算过程
4. 自动完成反向传播和梯度计算
5. 提供神经网络层、损失函数和优化器
6. 支持数据加载、混合精度和分布式训练
7. 支持模型编译、导出和推理部署

简单来说，PyTorch 将深度学习模型开发过程中最常见的步骤组织到了一套统一框架中。

```text
数据 → Tensor → 模型 → Loss → 梯度 → 参数更新
```

### 1.1.2 PyTorch 不只是神经网络层库

初学者通常首先接触以下代码

```python
import torch
from torch import nn

model = nn.Linear(10, 2)
```

这容易让人认为 PyTorch 只是一个提供 `Linear`、`Conv2d`、`LSTM` 等网络层的工具库。

实际上，神经网络层只是 PyTorch 的上层功能。它的底层还包括

- Tensor 存储与计算
- CPU、GPU 算子执行
- 自动求导引擎
- 参数和模块管理
- 显存管理
- 分布式通信
- 计算图捕获与编译
- C++ 和 CUDA 扩展机制

因此，PyTorch 既可以被看作一个深度学习框架，也可以被看作一个面向异构硬件的张量计算系统。

### 1.1.3 PyTorch 解决了什么问题

假设没有深度学习框架，要训练一个神经网络，开发者需要手动完成

- 实现矩阵乘法、卷积等运算
- 管理模型参数
- 推导每个操作的梯度公式
- 编写完整反向传播代码
- 将计算迁移到 GPU
- 管理中间结果和显存
- 实现优化器
- 处理多卡之间的参数和梯度同步

这些工作不仅复杂，而且容易出错。

PyTorch 将这些通用能力封装起来，让开发者可以更关注模型本身。

PyTorch 的重要价值，就是将这些底层过程组织成了清晰、可组合的编程接口。

### 1.1.4 PyTorch 的主要特点

**Python 使用体验自然**

PyTorch 的接口风格接近 Python 和 NumPy，控制流可以直接使用普通的 Python 语法。

```python
def forward(self, x):
    if x.mean() > 0:
        return self.positive_branch(x)
    return self.negative_branch(x)
```

这种写法便于开发、调试和快速修改模型。

**默认采用即时执行**

执行一行 PyTorch 代码时，对应操作通常会立即运行，而不是必须先定义一整张静态计算图。

```python
x = torch.tensor([1.0, 2.0])
y = x * 2

print(y)
```

这种模式通常称为 Eager Execution，即即时执行。

**支持自动求导**

只要计算过程由 PyTorch 可求导操作构成，框架就可以自动记录计算关系，并在反向传播时计算梯度。

**支持硬件加速**

相同的 Tensor 和模型代码可以在 CPU 或 GPU 上运行。

```python
device = torch.device(
    "cuda" if torch.cuda.is_available() else "cpu"
)

model = model.to(device)
inputs = inputs.to(device)
```

**兼顾灵活性与性能**

PyTorch 默认执行方式便于调试和研究，同时也提供混合精度、分布式训练、图编译和自定义算子等性能优化能力。

### 1.1.5 一个需要建立的基本认识

PyTorch 不会替开发者自动完成所有事情。

例如

- `loss.backward()` 只负责计算梯度，不负责更新参数
- `optimizer.step()` 负责更新参数，但不会自动清除旧梯度
- `model.eval()` 会切换部分模块的行为，但不会关闭自动求导
- 将模型放到 GPU 后，输入数据也必须放到同一设备
- 使用 PyTorch 不代表模型设计一定正确
- 代码可以运行，也不代表训练流程没有逻辑错误

因此，掌握 PyTorch 不只是会调用 API，还需要理解每个步骤的职责边界。

### 1.1.6 容易答错的地方

不要只回答：PyTorch 是一个深度学习框架。

这个答案没有错，但过于宽泛。更完整的回答应至少包含

- Tensor 计算
- 自动求导
- 神经网络训练
- GPU 加速
- 动态计算图或即时执行

## 1.2 PyTorch 的核心组成

PyTorch 不是一个单一功能的库，而是由多个相互协作的模块组成。

从一个完整训练流程来看，可以将其概括为

```text
torch.Tensor
    ↓
torch.utils.data
    ↓
torch.nn.Module
    ↓
torch.autograd
    ↓
torch.optim
    ↓
CUDA / AMP / Distributed
```

### 1.2.1 Tensor：数据和计算的基础

Tensor 是 PyTorch 最基础的数据结构，用于表示标量、向量、矩阵以及更高维数据。

```python
import torch

x = torch.tensor([
    [1.0, 2.0],
    [3.0, 4.0],
])

print(x.shape)   # torch.Size([2, 2])
print(x.dtype)   # torch.float32
print(x.device)  # cpu
```

一个 Tensor 通常包含以下关键信息

- 数据内容
- 形状 `shape`
- 数据类型 `dtype`
- 所在设备 `device`
- 内存布局
- 是否需要计算梯度

在模型训练中，以下内容通常都由 Tensor 表示

- 输入数据
- 标签
- 模型参数
- 中间激活值
- 损失值
- 梯度
- 优化器状态

因此，Tensor 是整个 PyTorch 系统的基础。

### 1.2.2 Autograd：自动求导系统

`torch.autograd` 负责记录 Tensor 之间的计算关系，并根据链式法则自动计算梯度。

```python
x = torch.tensor(
    2.0,
    requires_grad=True,
)

y = x ** 2 + 3 * x
y.backward()

print(x.grad)  # tensor(7.)
```

对应数学表达式为

```text
y = x² + 3x
dy/dx = 2x + 3
```

当 `x = 2` 时

```text
dy/dx = 2 × 2 + 3 = 7
```

开发者不需要手动编写导数计算代码，Autograd 会根据前向计算过程自动完成反向传播。

### 1.2.3 `torch.nn`：神经网络模块

`torch.nn` 提供模型构建所需的基础组件，包括

- 全连接层
- 卷积层
- 循环神经网络
- Transformer
- 归一化层
- Dropout
- 激活函数
- 损失函数

常见用法如下

```python
from torch import nn

model = nn.Sequential(
    nn.Linear(10, 32),
    nn.ReLU(),
    nn.Linear(32, 2),
)
```

`nn.Module` 不只是封装前向计算，还负责

- 注册模型参数
- 管理子模块
- 切换训练和推理状态
- 保存和加载参数
- 将模型迁移到不同设备

### 1.2.4 `torch.optim`：优化器

Autograd 只负责计算梯度，而优化器负责根据梯度更新参数。

```python
optimizer = torch.optim.AdamW(
    model.parameters(),
    lr=1e-3,
)
```

常见优化器包括

- SGD
- Momentum SGD
- Adam
- AdamW
- RMSprop

其基本工作流程是

```text
前向传播
    ↓
计算 loss
    ↓
backward 计算梯度
    ↓
optimizer.step 更新参数
```

需要特别注意

```python
loss.backward()
```

不会直接修改模型参数。真正更新参数的是

```python
optimizer.step()
```

### 1.2.5 `torch.utils.data`：数据加载

`torch.utils.data` 主要提供

- `Dataset`
- `DataLoader`
- `Sampler`
- 批处理
- 数据打乱
- 多进程加载

典型代码如下

```python
from torch.utils.data import DataLoader, TensorDataset

dataset = TensorDataset(inputs, targets)

loader = DataLoader(
    dataset,
    batch_size=4,
    shuffle=True,
)

for batch_inputs, batch_targets in loader:
    print(batch_inputs.shape)
```

DataLoader 将数据处理过程组织为

```text
原始样本
    ↓
Dataset 读取单个样本
    ↓
Sampler 决定读取顺序
    ↓
DataLoader 组成 Batch
    ↓
输入模型
```

### 1.2.6 CUDA 与设备管理

PyTorch 可以将 Tensor 计算交给 GPU 执行。

```python
device = torch.device("cuda")

model = model.to(device)
inputs = inputs.to(device)
targets = targets.to(device)
```

PyTorch 中的模型参数和输入数据必须位于兼容的设备上。下面的情况会报错

```text
模型参数：GPU
输入数据：CPU
```

这是因为 CPU 内存和 GPU 显存是不同的存储空间，不能直接参与同一个底层算子计算。

### 1.2.7 AMP：自动混合精度

AMP 用于在部分操作中使用 FP16 或 BF16 等低精度数据类型，从而

- 降低显存占用
- 提高 GPU 计算吞吐
- 利用 Tensor Core
- 尽量保持训练稳定性

典型训练形式为

```python
with torch.autocast(
    device_type="cuda",
    dtype=torch.float16,
):
    outputs = model(inputs)
    loss = criterion(outputs, targets)
```

混合精度不是简单地把所有数据都转换成 FP16，而是根据算子特点选择合适的精度。

### 1.2.8 `torch.distributed`：分布式训练

当单张 GPU 无法满足训练速度或显存需求时，可以使用分布式训练。

PyTorch 提供了

- DistributedDataParallel
- 集合通信
- NCCL 后端
- 参数和梯度同步
- 多进程训练
- 模型与状态分片能力

常见的数据并行过程为

```text
每张 GPU 保存一份模型
        ↓
每张 GPU 处理不同数据
        ↓
各自计算前向传播
        ↓
各自计算本地梯度
        ↓
通过 All-Reduce 同步梯度
        ↓
各自更新参数
```

### 1.2.9 编译、导出与扩展能力

除训练功能外，PyTorch 还提供以下工程能力。

**`torch.compile`**

尝试捕获和优化模型计算过程，减少 Python 调度开销，并进行算子融合等优化。

**`torch.export`**

将模型程序导出为更适合后续转换和部署的图表示。

**`C++ / CUDA Extension`**

当内置算子不能满足功能或性能需求时，可以编写自定义 C++ 或 CUDA 算子。

### 1.2.10 PyTorch 各组件如何协作

一次标准训练过程可以概括为

```text
Dataset / DataLoader
        ↓
读取一个 Batch
        ↓
Tensor 移动到 GPU
        ↓
nn.Module 前向传播
        ↓
计算 Loss
        ↓
Autograd 反向传播
        ↓
Optimizer 更新参数
        ↓
进入下一个 Batch
```

对应代码如下

```python
for inputs, targets in loader:
    inputs = inputs.to(device)
    targets = targets.to(device)

    optimizer.zero_grad()

    outputs = model(inputs)
    loss = criterion(outputs, targets)

    loss.backward()
    optimizer.step()
```

理解这段流程，是学习后续所有 PyTorch 知识的基础。

## 1.3 PyTorch 的动态图机制

### 1.3.1 什么是计算图

神经网络中的计算可以表示成一张有向图。

例如

```python
x = torch.tensor(
    2.0,
    requires_grad=True,
)

a = x * 3
b = a + 1
y = b ** 2
```

可以表示为

```text
x
│
├── × 3
│
a
│
├── + 1
│
b
│
├── 平方
│
y
```

图中的节点表示 Tensor 或运算，边表示数据之间的依赖关系。

在反向传播时，PyTorch 会从输出 `y` 开始，按照依赖关系反向计算梯度。

### 1.3.2 什么是动态图

PyTorch 的动态图通常指：计算图在程序实际执行前向计算时动态建立。

也就是说，开发者不需要先完整声明一张计算图。代码执行到哪个运算，PyTorch 就记录哪个运算。

```python
x = torch.randn(
    4,
    requires_grad=True,
)

if x.mean() > 0:
    y = x ** 2
else:
    y = x ** 3

loss = y.sum()
loss.backward()
```

如果条件不同，本次执行建立的计算图也可能不同。

```text
条件成立：x → 平方 → sum
条件不成立：x → 立方 → sum
```

这种模式也常被称为 Define-by-Run：运行的同时定义计算图。

### 1.3.3 动态图是如何建立的

当一个需要梯度的 Tensor 参与可求导运算时，PyTorch 会记录

- 当前执行了什么操作
- 操作的输入是什么
- 输出依赖哪些输入
- 反向传播时应调用什么梯度函数

例如

```python
x = torch.tensor(
    2.0,
    requires_grad=True,
)

y = x * x
z = y + 3

print(y.grad_fn)
print(z.grad_fn)
```

`y` 和 `z` 的 `grad_fn` 会记录它们由什么操作产生。

可以将其理解为

```text
前向过程：
一边计算结果，一边保存反向传播所需的路径

反向过程：
沿着保存的路径，按照链式法则计算梯度
```

### 1.3.4 动态图的反向传播流程

考虑下面的函数

```text
y = (3x + 1)²
```

其计算过程是

```text
x
↓
a = 3x
↓
b = a + 1
↓
y = b²
```

调用

```python
y.backward()
```

之后，Autograd 会反向执行

```text
dy/db = 2b
db/da = 1
da/dx = 3
```

根据链式法则

```text
dy/dx
= dy/db × db/da × da/dx
= 2b × 1 × 3
```

PyTorch 自动完成这一过程，并将最终结果累积到叶子 Tensor 的 `.grad` 中。

### 1.3.5 为什么采用动态图

**原因一：便于调试**

动态图中的前向计算就是普通 Python 程序，可以直接

- 打印变量
- 查看 Tensor shape
- 设置断点
- 使用条件分支
- 使用循环
- 检查中间结果

出现问题时，通常可以定位到具体代码行。

**原因二：适合动态模型结构**

某些模型的执行路径可能依赖输入，例如

- 不同长度的序列
- 树结构神经网络
- 强化学习
- 动态路由
- Mixture of Experts
- 条件计算

动态图可以自然表达这些逻辑。

**原因三：有利于快速实验**

研究人员经常需要调整

- 网络层数
- 前向逻辑
- 损失函数
- 多分支结构
- 模型输出

动态图通常不要求先重新定义和编译一整张静态图，因此实验迭代更直接。

### 1.3.6 动态图的代价

动态图也存在一定代价。

**Python 调度开销**

每次执行前向传播，都需要通过 Python 调用和调度多个算子。

如果模型由大量很小的算子组成，单个算子计算时间很短，Python 调度成本就可能变得明显。

**图级优化难度较高**

如果只逐个执行算子，框架较难提前进行

- 跨算子融合
- 内存复用
- 常量折叠
- 全局调度
- 设备相关优化

**编译和导出存在额外约束**

Python 中过于动态的逻辑、外部副作用或不支持的操作，可能导致图捕获困难。

### 1.3.7 动态图与 `torch.compile` 的关系

PyTorch 默认仍保留即时执行和动态图式的编程体验。

`torch.compile` 的目标不是要求开发者完全改写为传统静态图程序，而是尝试从普通 PyTorch 程序中捕获可优化的计算区域。

可以简单理解为

```text
普通 PyTorch 代码
        ↓
运行时观察和捕获
        ↓
提取可编译计算图
        ↓
进行图优化和算子融合
        ↓
生成更高效的执行代码
```

如果遇到难以捕获的 Python 逻辑，可能发生 Graph Break，部分代码回到普通即时执行模式。

因此，动态图和编译优化并不是完全对立的。现代 PyTorch 试图同时保留

- 动态编程的灵活性
- 图编译带来的性能优势

### 1.3.8 动态图的常见误区

**误区一：动态图表示模型参数每次都会变化**

不是。

“动态”主要指计算图在运行时构建，并不表示模型参数数量每次都必须改变。

**误区二：动态图会永久保存所有前向计算**

不是。

默认情况下，反向传播完成后，很多计算图中间状态会被释放。

**误区三：调用 `backward()` 后计算图仍可无限重复使用**

默认情况下不可以。

下面代码通常会报错

```python
loss.backward()
loss.backward()
```

因为第一次反向传播后，部分中间状态已经被释放。若确实需要重复使用同一张图，可能需要

```python
loss.backward(retain_graph=True)
```

但不应无理由长期保留计算图，否则可能增加显存占用。

**误区四：动态图一定比静态图慢**

不能简单下结论。

实际性能还取决于

- 模型结构
- 算子规模
- GPU 计算量
- 是否使用编译
- 是否存在大量小算子
- 数据加载和通信开销

对于计算量较大的矩阵乘法和卷积，Python 调度开销占比可能并不高。

## 1.4 PyTorch 与 TensorFlow / NumPy 的区别

PyTorch、TensorFlow 和 NumPy 都可以进行数值计算，但它们的定位并不相同。

### 1.4.1 PyTorch 与 NumPy

NumPy 是 Python 科学计算的基础库，PyTorch 的很多 Tensor API 风格与 NumPy 类似。

例如

```python
# NumPy
import numpy as np

a = np.array([1.0, 2.0])
b = a * 2
```

```python
# PyTorch
import torch

a = torch.tensor([1.0, 2.0])
b = a * 2
```

两者在基础数组操作上比较相似，但能力范围存在明显区别。

|   对比维度   |  NumPy  | PyTorch |
| :------: | :-----: | :-----: |
|  核心数据结构  | ndarray | Tensor  |
| CPU 数值计算 |   支持    |   支持    |
|  GPU 计算  |  默认不支持  |  原生支持   |
|   自动求导   |   不支持   |   支持    |
|  神经网络模块  |   不提供   |   提供    |
|   优化器    |   不提供   |   提供    |
|  分布式训练   |   不提供   |   提供    |
|  模型部署生态  |  非主要目标  |   支持    |

NumPy 更适合

- 通用科学计算
- 数据预处理
- 传统数值分析
- CPU 数组操作

PyTorch 更适合

- 深度学习训练
- 自动求导
- GPU 张量计算
- 模型推理
- 分布式机器学习

### 1.4.2 PyTorch Tensor 与 NumPy ndarray 的联系

CPU 上的 PyTorch Tensor 和 NumPy ndarray 可以相互转换。

```python
import numpy as np
import torch

array = np.array(
    [1.0, 2.0, 3.0],
    dtype=np.float32,
)

tensor = torch.from_numpy(array)
```

在部分情况下，它们可以共享底层内存。

```python
array[0] = 100

print(tensor)
```

Tensor 中的对应数据也可能发生变化。

这意味着相互转换时需要关注

- 是否共享内存
- 数据类型是否一致
- Tensor 是否位于 CPU
- 修改其中一方是否影响另一方

GPU Tensor 不能直接调用 `.numpy()`，通常需要先移回 CPU

```python
array = tensor.detach().cpu().numpy()
```

### 1.4.3 PyTorch 与 TensorFlow

TensorFlow 和 PyTorch 都是完整的机器学习框架，都支持

- Tensor 计算
- 自动求导
- 神经网络建模
- GPU 加速
- 分布式训练
- 模型部署

两者的差异主要体现在编程体验、图执行机制和生态工具等方面。

需要注意的是，不能简单地将二者概括为

```text
PyTorch = 动态图
TensorFlow = 静态图
```

这种说法更接近早期 TensorFlow 1.x 的情况。

现代 TensorFlow 支持 Eager Execution，也可以使用 `tf.function` 将代码转换为图执行。PyTorch 默认即时执行，同时可以通过 `torch.compile` 捕获和优化计算过程。

### 1.4.4 PyTorch 与 TensorFlow 的主要对比

|  对比维度  |       PyTorch        |           TensorFlow           |
| :----: | :------------------: | :----------------------------: |
| 默认编程体验 |       以即时执行为主        |            也支持即时执行             |
| 图优化方式  |  `torch.compile` 等   |        `tf.function` 等         |
|  调试体验  |     接近普通 Python      |          Eager 下也较直观           |
|  模型构建  |     `nn.Module`      |      Keras / `tf.Module`       |
|  自动求导  |       Autograd       |          GradientTape          |
|  数据管线  | Dataset / DataLoader |           `tf.data`            |
|  部署生态  |   Export、ONNX及其他后端   | SavedModel、TFLite、TF Serving 等 |
|  研究原型  |         使用广泛         |             同样具备能力             |
|  工程选型  |      依赖团队和现有系统       |           依赖团队和现有系统            |

不存在任何情况下都绝对更优的框架。选型需要考虑

- 团队技术栈
- 已有模型代码
- 部署环境
- 硬件后端
- 生态依赖
- 性能要求
- 维护成本

### 1.4.5 面试中应该如何回答框架对比

一个客观的回答应避免绝对化判断。

不建议回答

- PyTorch 一定比 TensorFlow 快
- TensorFlow 已经没有人使用
- PyTorch 只能做研究
- TensorFlow 只能做工业部署
- NumPy 就是不能用 GPU 的 PyTorch

更合理的回答是

1. NumPy 和深度学习框架的目标不同
2. PyTorch 和 TensorFlow 都支持现代训练与部署
3. PyTorch 的接口和执行体验通常更接近普通 Python
4. TensorFlow 具有自己的数据、部署和端侧生态
5. 最终选择取决于项目需求和已有基础设施

## 1.5 PyTorch 的典型工程应用场景

PyTorch 的应用范围不仅包括模型训练，还覆盖模型开发、推理、分布式计算和系统优化。

### 1.5.1 模型研究与原型开发

PyTorch 适合快速实现和验证新的模型结构，例如

- CNN
- RNN
- Transformer
- 图神经网络
- 扩散模型
- 多模态模型
- 自定义注意力模块
- 新型损失函数

研究人员可以直接使用 Python 控制流调整模型计算过程，并通过即时执行观察中间 Tensor。

### 1.5.2 监督学习模型训练

这是 PyTorch 最常见的应用场景之一。

例如

- 图像分类
- 目标检测
- 语义分割
- 文本分类
- 语音识别
- 时间序列分类
- 回归预测

一个典型项目会使用

```text
Dataset / DataLoader
        ↓
nn.Module
        ↓
Loss
        ↓
Optimizer
        ↓
训练与验证循环
        ↓
Checkpoint
```

在工程中，还需要额外考虑

- 数据增强
- 指标统计
- 随机种子
- 实验配置
- 断点恢复
- 日志记录
- 模型选择

### 1.5.3 预训练模型微调

实际项目通常不会从零训练大型模型，而是加载预训练权重后进行微调。

典型场景包括

- 使用预训练 CNN 完成行业图像分类
- 使用 BERT 完成文本分类
- 使用 Transformer 进行序列建模
- 使用 LoRA 微调大语言模型
- 微调 Embedding 或重排序模型

工程流程通常为

```text
加载预训练模型
        ↓
替换任务输出层
        ↓
冻结或解冻部分参数
        ↓
准备任务数据
        ↓
训练并验证
        ↓
保存微调权重
```

PyTorch 的参数管理和自动求导机制可以方便地控制哪些参数参与训练。

```python
for parameter in model.backbone.parameters():
    parameter.requires_grad = False
```

### 1.5.4 Transformer 与大语言模型工程

PyTorch 是 Transformer 和 LLM 工程中的基础框架之一。

常见应用包括

- 模型结构实现
- 预训练
- 指令微调
- LoRA 微调
- RLHF 相关训练
- Embedding 推理
- 文本生成
- 模型性能分析
- 推理框架的模型后端

在 LLM 场景中，需要重点理解

- Token Embedding
- Attention
- Mask
- FP16 / BF16
- 梯度累积
- Gradient Checkpointing
- DDP / FSDP
- KV Cache
- prefill 和 decode

即使使用 Transformers 等上层库，底层模型参数、输入、输出和梯度仍然主要由 PyTorch Tensor 管理。

### 1.5.5 GPU 加速训练

当模型计算量较大时，PyTorch 可以使用 GPU 加速

- 矩阵乘法
- 卷积
- Attention
- 归一化
- 激活函数
- 梯度计算

但将模型放到 GPU 并不意味着训练一定高效。实际性能还受到以下因素影响

- 数据加载速度
- CPU 预处理
- batch size
- CPU-GPU 数据拷贝
- 算子规模
- 显存带宽
- GPU 同步
- 多卡通信

因此，GPU 训练也是 PyTorch 工程能力的重要组成部分。

### 1.5.6 混合精度训练

对于支持低精度高吞吐计算的 GPU，可以使用 AMP

```text
部分算子使用 FP16 / BF16
          +
关键算子保留较高精度
          ↓
减少显存占用并提高吞吐
```

典型应用包括

- 图像模型训练
- Transformer 训练
- LLM 微调
- 大 batch 训练
- GPU 推理

使用 AMP 时仍需注意

- 数值溢出
- 数值下溢
- 算子精度
- 模型最终精度
- 硬件是否支持高效低精度计算

### 1.5.7 单机多卡与分布式训练

当单张 GPU 的速度或显存无法满足需求时，可以使用

- DistributedDataParallel
- FSDP
- 数据并行
- 模型分片
- 梯度分片
- 多机多卡训练

典型场景包括

- 大规模图像训练    
- Transformer 预训练
- LLM 微调
- 大 batch 训练
- 多节点训练集群

这一场景不仅涉及 PyTorch，还会涉及

- 多进程
- GPU 拓扑
- NCCL
- 网络通信
- Checkpoint
- 故障恢复
- 资源调度

### 1.5.8 模型推理

训练完成后，可以使用 PyTorch 直接执行推理。

```python
model.eval()

with torch.inference_mode():
    outputs = model(inputs)
```

典型场景包括

- 离线批量推理
- 在线模型服务
- Embedding 计算
- 图像检测服务
- 文本分类服务
- LLM 推理原型

推理阶段主要关注

- 单请求延迟
- 批量吞吐
- 显存占用
- 并发
- 精度
- 模型加载时间
- 冷启动
- 稳定性

### 1.5.9 模型导出与部署

在部分生产环境中，不会直接依赖 Python 运行完整 PyTorch 代码，而是将模型转换或导出到其他运行时。

常见流程为

```text
PyTorch 模型
    ↓
模型导出
    ↓
图表示或中间格式
    ↓
推理运行时
    ↓
CPU / GPU / 端侧设备
```

可能涉及

- `torch.export`
- ONNX
- ONNX Runtime
- TensorRT
- C++ Runtime
- 端侧推理框架

导出后需要验证

- 输出是否一致
- 是否支持动态输入
- 算子是否兼容
- 精度是否变化
- 性能是否真正提升

### 1.5.10 AI Infra 与框架研发

对于 AI Infra 和系统岗位，PyTorch 本身也是研究和优化对象。

典型工作包括

- 训练框架封装
- 分布式训练平台
- 性能监控
- 显存分析
- 自动并行
- 计算图编译
- 算子融合
- 自定义 C++ / CUDA 算子
- 模型导出系统
- 训练任务调度
- Checkpoint 管理

这类工作不仅要求会写模型，还需要理解

```text
Python API
    ↓
计算图
    ↓
Autograd
    ↓
Dispatcher
    ↓
CPU / CUDA 算子
    ↓
硬件执行
```

### 1.5.11 后端模型服务

PyTorch 模型可以作为后端服务中的一个计算组件。

一个典型架构如下

```text
客户端请求
    ↓
HTTP / RPC 服务
    ↓
输入预处理
    ↓
PyTorch 模型推理
    ↓
输出后处理
    ↓
返回结果
```

工程中需要考虑

- 模型是否重复加载
- 多请求并发
- 请求批处理
- GPU 资源竞争
- 超时
- OOM
- 模型版本
- 日志与监控
- 灰度发布
- 服务降级

此时，PyTorch 只是系统中的一层，必须与后端服务、资源管理和监控系统协作。

### 1.5.12 不同岗位关注的工程场景

|    岗位方向     |     主要 PyTorch 场景     |
| :---------: | :-------------------: |
|    算法工程     |     模型设计、训练、实验和调参     |
| Agent / RAG | 模型调用、Embedding、重排序和推理 |
|   LLM 工程    |     微调、分布式训练和生成推理     |
|  AI Infra   |    框架、分布式、编译和性能优化     |
|    推理部署     |    导出、量化、服务化和性能分析     |
|    后端开发     |    模型服务、并发、监控和版本管理    |

## 1.6 本章小节

理解 PyTorch 时，可以建立下面这张整体图。

```text
                     PyTorch
                        │
        ┌───────────────┼───────────────┐
        │               │               │
   Tensor 计算       模型训练        工程能力
        │               │               │
 shape / dtype      nn.Module       CUDA / AMP
 device / stride    Autograd        Distributed
 broadcasting       Optimizer       Compile
        │               │           Export
        └───────────────┼───────────────┘
                        │
                 训练、推理与部署
```

本章需要掌握的核心结论是

1. PyTorch 不只是神经网络层库，而是一套完整的张量计算和机器学习框架
2. Tensor 是基础，Autograd 负责梯度，`nn.Module` 负责模型组织，Optimizer 负责参数更新
3. PyTorch 默认在代码执行过程中动态构建计算图
4. PyTorch 与 NumPy 的定位不同，与现代 TensorFlow 都兼具即时执行和图优化能力
5. PyTorch 可以用于模型研究、训练、微调、分布式、推理、部署和 AI Infra
6. 真正掌握 PyTorch，需要同时理解 API、运行原理和工程问题

# 2 Tensor 基础与内存机制

Tensor 是 PyTorch 中最基本的数据结构。输入数据、模型参数、中间激活、损失值、梯度和优化器状态，最终都以 Tensor 的形式存在。

学习 Tensor 需要同时理解两个层面

```text
逻辑层：
Tensor 有多少维、每一维多大、元素是什么类型

物理层：
数据存在哪里、按照什么顺序存储、索引如何映射到内存
```

前者主要涉及

- `shape`
- `dtype`
- `device`

后者主要涉及

- Storage    
- stride
- storage offset
- contiguous
- view

## 2.1 Tensor 的基本概念

### 2.1.1 Tensor 是什么

在 PyTorch 的工程语境中，可以将 Tensor 理解为：具有统一数据类型、形状、设备和内存布局的多维数组。

PyTorch 官方将 `torch.Tensor` 定义为包含单一数据类型元素的多维矩阵。Tensor 不仅能够存储数据，还可以参与自动求导，并在 CPU、GPU 等设备上执行计算。

不同维度的 Tensor 可以表示不同类型的数据

| Tensor 维数 |     示例 shape      |   常见含义    |
| :-------: | :---------------: | :-------: |
|    0 维    |       `()`        |    标量     |
|    1 维    |      `(D,)`       |   向量、特征   |
|    2 维    |     `(N, D)`      |  矩阵、批量特征  |
|    3 维    |    `(B, T, D)`    |   批量序列    |
|    4 维    |  `(B, C, H, W)`   |   批量图像    |
|    5 维    | `(B, C, T, H, W)` | 视频或三维医学数据 |

示例

```python
import torch

scalar = torch.tensor(3.0)
vector = torch.tensor([1.0, 2.0, 3.0])
matrix = torch.tensor([
    [1.0, 2.0],
    [3.0, 4.0],
])

print(scalar.shape)  # torch.Size([])
print(vector.shape)  # torch.Size([3])
print(matrix.shape)  # torch.Size([2, 2])
```

需要注意

```python
scalar = torch.tensor(3.0)
```

创建的是一个 0 维 Tensor，而不是 shape 为 `(1,)` 的一维 Tensor。

```python
one_element_vector = torch.tensor([3.0])

print(scalar.shape)             # torch.Size([])
print(one_element_vector.shape) # torch.Size([1])
```

两者都只有一个元素，但逻辑形状不同。

### 2.1.2 Tensor 的维数不是矩阵的秩

在深度学习中，人们有时使用“rank”表示 Tensor 的维数，例如

```python
x = torch.randn(2, 3, 4)

print(x.dim())   # 3
print(x.ndim)    # 3
```

这里的 `3` 表示 Tensor 有三个维度。

它与线性代数中矩阵的“秩”不是同一个概念。线性代数中的矩阵秩表示线性无关行或列的最大数量。

因此，面试中最好使用

- Tensor 的维数
- Tensor 的阶数
- `ndim`

避免含糊地只说“秩”。

### 2.1.3 Tensor 的核心属性

从工程角度，可以将一个 Tensor 抽象为

```text
Tensor
├── 数据类型 dtype
├── 所在设备 device
├── 逻辑形状 shape
├── 步长 stride
├── 存储偏移 storage_offset
├── 底层存储 storage
└── 自动求导信息
```

例如

```python
x = torch.arange(
    12,
    dtype=torch.float32,
).reshape(3, 4)

print("shape:", x.shape)
print("dtype:", x.dtype)
print("device:", x.device)
print("stride:", x.stride())
print("storage_offset:", x.storage_offset())
print("numel:", x.numel())
```

可能得到

```text
shape: torch.Size([3, 4])
dtype: torch.float32
device: cpu
stride: (4, 1)
storage_offset: 0
numel: 12
```

这些信息共同决定了

- Tensor 有多少个元素
- 如何解释底层内存
- 一个索引对应底层存储中的哪个位置
- 应该调用 CPU 还是 GPU 算子

### 2.1.4 Tensor 是数据还是视图

Tensor 对象不一定拥有一块独立数据。

两个 Tensor 可能

- 拥有各自独立的存储
- 共享同一块底层存储
- 只是用不同 shape 和 stride 解释同一块数据

例如

```python
x = torch.tensor([1, 2, 3, 4])
y = x.view(2, 2)

y[0, 0] = 100

print(x)  # tensor([100,   2,   3,   4])
print(y)
```

`y` 是 `x` 的一个视图。创建 `y` 时没有复制底层元素，因此修改 `y` 也会影响 `x`。

PyTorch 的视图操作通过改变 Tensor 对底层数据的解释方式避免数据复制；视图 Tensor 与原 Tensor 共享底层数据。

### 2.1.5 Tensor 与自动求导

Tensor 还可以记录是否需要计算梯度

```python
x = torch.tensor(
    [1.0, 2.0, 3.0],
    requires_grad=True,
)

y = (x ** 2).sum()
y.backward()

print(x.grad)
```

输出

```text
tensor([2., 4., 6.])
```

Tensor 因而不只是一个多维数组，还可能是自动求导计算图中的节点。

## 2.2 Tensor 的 shape、dtype、device

Tensor 最重要的三个属性是

```text
shape：数据如何组织
dtype：每个元素如何表示
device：数据存储和计算发生在哪里
```

### 2.2.1 shape：Tensor 的逻辑形状

`shape` 表示每个维度的大小。

```python
x = torch.randn(8, 16, 32)

print(x.shape)     # torch.Size([8, 16, 32])
print(x.size())    # torch.Size([8, 16, 32])
print(x.size(0))   # 8
print(x.size(-1))  # 32
print(x.ndim)      # 3
print(x.numel())   # 8 * 16 * 32 = 4096
```

其中

- `shape` 和无参数的 `size()` 都返回整体形状
- `size(dim)` 返回某一维的大小
- `ndim` 或 `dim()` 返回维数
- `numel()` 返回元素总数

`torch.Size` 是 tuple 的子类，因此可以执行

```python
batch_size, seq_len, hidden_size = x.shape
```

### 2.2.2 深度学习中的常见 shape 约定

**表格数据**

```text
[B, D]
```

- `B`：batch size
- `D`：特征维度

**图像数据**

PyTorch 中卷积网络通常使用

```text
[B, C, H, W]
```

- `B`：批量大小
- `C`：通道数
- `H`：图像高度
- `W`：图像宽度

**序列数据**

Transformer 常见输入

```text
[B, T, D]
```

- `B`：批量大小
- `T`：序列长度
- `D`：隐藏维度

**多头注意力**

拆分多头之后常见

```text
[B, H, T, Dh]
```

其中

```text
D = H × Dh
```

面试和实际开发中，大量错误都来自 shape 理解不正确。因此阅读模型代码时，应主动标注每一步的形状变化。

### 2.2.3 dtype：元素的数据类型

一个普通 Tensor 中的所有元素使用同一种 `dtype`。

常见类型如下

|      dtype       | 每元素常见占用 |   典型用途    |
| :--------------: | :-----: | :-------: |
|   `torch.bool`   | 1 Byte  | Mask、逻辑条件 |
|   `torch.int8`   | 1 Byte  |    量化     |
|  `torch.int32`   | 4 Byte  |   整数计算    |
|  `torch.int64`   | 8 Byte  |  索引、类别标签  |
| `torch.float16`  | 2 Byte  |   混合精度    |
| `torch.bfloat16` | 2 Byte  | 大模型训练与推理  |
| `torch.float32`  | 4 Byte  |  默认浮点训练   |
| `torch.float64`  | 8 Byte  |  高精度数值计算  |

查看类型和元素大小

```python
x = torch.randn(2, 3, dtype=torch.float32)

print(x.dtype)        # torch.float32
print(x.element_size())  # 4
print(x.numel())         # 6
```

该 Tensor 的纯数据理论大小为

```python
memory_bytes = x.numel() * x.element_size()
print(memory_bytes)  # 24
```

这只是数据元素本身的大小，不包括 Tensor 元数据、缓存分配和其他运行时开销。

### 2.2.4 dtype 推断

PyTorch 可以根据输入推断 dtype

```python
a = torch.tensor([1, 2, 3])
b = torch.tensor([1.0, 2.0, 3.0])
c = torch.tensor([True, False])

print(a.dtype)  # 通常为 torch.int64
print(b.dtype)  # 默认浮点类型通常为 torch.float32
print(c.dtype)  # torch.bool
```

但工程代码中，关键 Tensor 最好显式指定类型

```python
features = torch.tensor(
    [[1.0, 2.0]],
    dtype=torch.float32,
)

labels = torch.tensor(
    [1],
    dtype=torch.long,
)
```

在分类任务中，类别索引通常使用 `torch.long`，即 `torch.int64`。

### 2.2.5 dtype 转换

常见方式

```python
x = torch.tensor([1, 2, 3])

x_float = x.float()
x_long = x.long()
x_half = x.half()

x_float32 = x.to(torch.float32)
```

更通用的标准写法是

```python
x = x.to(dtype=torch.float32)
```

`.to()` 可以同时指定 dtype 和 device

```python
x = x.to(
    device="cuda",
    dtype=torch.float16,
)
```

### 2.2.6 dtype 不匹配

模型参数和输入通常需要具有兼容的 dtype。

例如，模型参数默认通常是 `float32`

```python
import torch
from torch import nn

model = nn.Linear(4, 2)
x = torch.randn(3, 4, dtype=torch.float64)

# outputs = model(x)
```

这可能因为输入是 `float64`、权重是 `float32` 而报类型不匹配错误。

标准处理方式是

```python
x = x.to(
    device=next(model.parameters()).device,
    dtype=next(model.parameters()).dtype,
)
```

但在普通训练代码中，更常见的是在数据入口统一类型

```python
x = x.float()
```

### 2.2.7 device：Tensor 所在设备

查看设备

```python
x = torch.randn(2, 3)

print(x.device)  # cpu
```

将 Tensor 移到 GPU

```python
x_gpu = x.to("cuda")
print(x_gpu.device)
```

也可以指定 GPU 编号

```python
x_gpu = x.to("cuda:0")
```

模型参数和输入 Tensor 必须位于兼容设备上

```python
device = torch.device(
    "cuda" if torch.cuda.is_available() else "cpu"
)

model = nn.Linear(4, 2).to(device)
x = torch.randn(3, 4).to(device)

outputs = model(x)
```

### 2.2.8 `.to()` 的返回值

`.to()` 通常不是原地操作，应该接收返回值

```python
x = torch.randn(2, 3)

x.to("cuda")        # 返回了新 Tensor，但没有保存
print(x.device)     # 仍然是 cpu

x = x.to("cuda")    # 正确
print(x.device)     # cuda:0
```

如果目标 dtype 和 device 与原 Tensor 已经一致，`.to()` 可以直接返回原对象；也可以通过 `copy=True` 强制创建副本。

## 2.3 Tensor 的创建、索引与常用操作

### 2.3.1 从已有数据创建 Tensor

**`torch.tensor`**

```python
x = torch.tensor(
    [[1, 2], [3, 4]],
    dtype=torch.float32,
)
```

`torch.tensor()` 会根据给定数据创建一个新的 Tensor，并复制输入数据。

**`torch.as_tensor`**

```python
x = torch.as_tensor(data)
```

`torch.as_tensor()` 会尽可能避免复制，并在适用时保留已有自动求导历史。输入为兼容 NumPy 数组时，它可以复用对应内存。

可以先建立以下认识

|           创建方式            | 是否倾向复制 |      典型用途      |
| :-----------------------: | :----: | :------------: |
|   `torch.tensor(data)`    |   是    |  需要独立新 Tensor  |
|  `torch.as_tensor(data)`  | 尽可能不复制 |     尽量复用输入     |
| `torch.from_numpy(array)` |  共享内存  | NumPy 转 Tensor |

### 2.3.2 按指定内容创建

```python
zeros = torch.zeros(2, 3)
ones = torch.ones(2, 3)
full = torch.full((2, 3), 7.0)
```

结果

```text
zeros:
[[0, 0, 0],
 [0, 0, 0]]

ones:
[[1, 1, 1],
 [1, 1, 1]]

full:
[[7, 7, 7],
 [7, 7, 7]]
```

### 2.3.3 `torch.empty`

```python
x = torch.empty(2, 3)
print(x)
```

`empty()` 只分配内存，不负责将其初始化为零。

因此输出可能包含任意旧数据，不能假设

```python
torch.empty(...)
```

等价于

```python
torch.zeros(...)
```

`empty()` 适合后续会立即覆盖全部数据的场景，可以避免不必要的初始化。

### 2.3.4 创建数值序列

```python
a = torch.arange(0, 10, 2)
b = torch.linspace(0, 1, steps=5)

print(a)  # tensor([0, 2, 4, 6, 8])
print(b)  # tensor([0.0000, 0.2500, 0.5000, 0.7500, 1.0000])
```

区别

- `arange(start, end, step)`：按照步长生成
- `linspace(start, end, steps)`：按照元素数量均匀生成

`arange` 的 `end` 不包含在结果中，而 `linspace` 默认包含两端。

### 2.3.5 创建随机 Tensor

```python
uniform = torch.rand(2, 3)
normal = torch.randn(2, 3)
integers = torch.randint(0, 10, (2, 3))
```

含义

- `rand`：通常在 `[0, 1)` 上均匀采样
- `randn`：从标准正态分布采样
- `randint`：生成指定整数范围内的随机整数

### 2.3.6 `*_like` 系列

根据已有 Tensor 的 shape、dtype 和 device 创建新 Tensor

```python
x = torch.randn(
    2,
    3,
    device="cpu",
    dtype=torch.float32,
)

zeros = torch.zeros_like(x)
ones = torch.ones_like(x)
noise = torch.randn_like(x)
```

这种写法比手动重复指定 shape、dtype 和 device 更稳健。

例如

```python
mask = torch.zeros_like(x, dtype=torch.bool)
```

它复用 `x` 的 shape 和 device，但将 dtype 改为 `bool`。

### 2.3.7 基础索引与切片

```python
x = torch.arange(12).reshape(3, 4)

print(x)
print(x[0])       # 第 0 行
print(x[1, 2])    # 第 1 行、第 2 列
print(x[:, 1])    # 第 1 列
print(x[1:, :2])  # 切片 一段数据
```

正索引从 `0` 开始，负索引从末尾开始

```python
print(x[-1])     # 最后一行
print(x[:, -1])  # 最后一列
```

### 2.3.8 基础索引与高级索引的内存差异

PyTorch 的基础索引通常返回视图，高级索引通常返回副本。索引赋值无论使用基础索引还是高级索引，都会直接修改被索引的原 Tensor。

**基础索引：通常共享存储**

```python
x = torch.arange(12).reshape(3, 4)

row = x[1]
row[0] = -1

print(x)
```

修改 `row` 会影响 `x`。

**高级索引：通常生成副本**

```python
x = torch.arange(12).reshape(3, 4)

selected = x[[0, 2]]
selected[0, 0] = -1

print(x)
```

这次修改 `selected` 通常不会修改 `x`。

高级索引包括

- 使用索引 Tensor
- 使用 Python 索引列表
- 布尔 Mask

### 2.3.9 布尔索引

```python
x = torch.tensor([-2, -1, 0, 1, 2])

positive = x[x > 0]
print(positive)  # tensor([1, 2])
```

直接赋值

```python
x[x < 0] = 0
print(x)  # tensor([0, 0, 0, 1, 2])
```

布尔索引非常方便，但会生成临时 Mask，并且读取结果通常是副本。在大规模性能敏感代码中，应注意临时 Tensor 和内存开销。

### 2.3.10 常用逐元素运算

```python
x = torch.tensor([1.0, 2.0, 3.0])
y = torch.tensor([4.0, 5.0, 6.0])

print(x + y)
print(x - y)
print(x * y)
print(x / y)
print(x ** 2)
print(torch.sqrt(x))
print(torch.exp(x))
```

注意

```python
x * y
```

是逐元素乘法，不是矩阵乘法。

### 2.3.11 矩阵乘法

```python
a = torch.randn(2, 3)
b = torch.randn(3, 4)

c = a @ b
d = torch.matmul(a, b)

print(c.shape)  # torch.Size([2, 4])
print(d.shape)  # torch.Size([2, 4])
```

`@` 与 `torch.matmul()` 是常见矩阵乘法写法。

批量矩阵乘法也可以使用 `matmul`

```python
a = torch.randn(8, 2, 3)
b = torch.randn(8, 3, 4)

c = torch.matmul(a, b)
print(c.shape)  # [8, 2, 4]
```

### 2.3.12 归约操作

归约操作将一个或多个维度聚合起来

```python
x = torch.arange(
    12,
    dtype=torch.float32,
).reshape(3, 4)

print(x.sum())
print(x.mean())
print(x.sum(dim=0))
print(x.sum(dim=1))
```

假设

```text
x.shape = [3, 4]
```

则

```text
x.sum()          → 标量
x.sum(dim=0)     → [4]
x.sum(dim=1)     → [3]
```

保留归约维度

```python
row_sum = x.sum(
    dim=1,
    keepdim=True,
)

print(row_sum.shape)  # [3, 1]
```

`keepdim=True` 经常用于后续广播。

### 2.3.13 `cat` 与 `stack`

**`torch.cat`**

沿已有维度拼接

```python
a = torch.randn(2, 3)
b = torch.randn(4, 3)

c = torch.cat([a, b], dim=0)
print(c.shape)  # [6, 3]
```

**`torch.stack`**

创建一个新维度

```python
a = torch.randn(2, 3)
b = torch.randn(2, 3)

c = torch.stack([a, b], dim=0)
print(c.shape)  # [2, 2, 3]
```

区别

```text
cat：在已有维度上连接
stack：增加一个新维度
```

### 2.3.14 `squeeze` 与 `unsqueeze`

添加大小为 `1` 的维度

```python
x = torch.randn(3, 4)

y = x.unsqueeze(0)
z = x.unsqueeze(-1)

print(y.shape)  # [1, 3, 4]
print(z.shape)  # [3, 4, 1]
```

移除大小为 `1` 的维度

```python
x = torch.randn(1, 3, 1, 4)

y = x.squeeze()
print(y.shape)  # [3, 4]
```

工程中更推荐明确指定维度

```python
y = x.squeeze(0)
```

因为直接 `squeeze()` 会删除所有大小为 `1` 的维度。当 batch size 恰好为 `1` 时，可能意外删除 batch 维度。

`unsqueeze()` 返回与原 Tensor 共享底层数据的视图。

### 2.3.15 原地操作

以 `_` 结尾的 API 通常表示原地操作

```python
x = torch.tensor([1.0, 2.0])

x.add_(1)
x.mul_(2)

print(x)
```

原地操作直接修改原 Tensor，可能减少临时内存，但也可能

- 修改共享视图的数据
- 破坏后续仍需要的中间值
- 与自动求导产生冲突
- 增加代码理解难度

因此不要为了“看起来更节省内存”而到处使用原地操作。

## 2.4 Tensor 的广播机制

### 2.4.1 为什么需要广播

考虑一个形状为 `[B, D]` 的批量特征，需要给每个样本加上同一个偏置向量

```text
features.shape = [B, D]
bias.shape     = [D]
```

如果没有广播，需要手动复制 `bias`

```text
[D]
↓ 复制 B 次
[B, D]
```

广播机制允许直接写

```python
features = torch.randn(8, 16)
bias = torch.randn(16)

outputs = features + bias

print(outputs.shape)  # [8, 16]
```

PyTorch 会在逻辑上将 `bias` 应用于每个样本，而不要求先显式复制出完整的 `[8, 16]` 数据。PyTorch 广播规则与 NumPy 语义兼容，可广播参数会被自动扩展到兼容大小，并不要求实际复制全部数据。

### 2.4.2 广播规则

比较两个 Tensor 的 shape 时，从最右侧维度开始。

两个维度满足以下任意条件即可兼容

1. 两个维度大小相同
2. 其中一个维度大小为 `1`
3. 其中一个 Tensor 不存在该维度

例如

```text
A.shape = [2, 3, 4]
B.shape =       [4]
```

从右向左比较

```text
4 和 4：相同
3 和缺失：兼容
2 和缺失：兼容
```

结果

```text
[2, 3, 4]
```

### 2.4.3 大小为 1 的维度

```text
A.shape = [2, 3, 4]
B.shape = [1, 3, 1]
```

逐维比较

```text
4 和 1：B 扩展为 4
3 和 3：相同
2 和 1：B 扩展为 2
```

结果

```text
[2, 3, 4]
```

代码

```python
a = torch.randn(2, 3, 4)
b = torch.randn(1, 3, 1)

c = a + b
print(c.shape)  # [2, 3, 4]
```

### 2.4.4 无法广播的情况

```text
A.shape = [2, 3]
B.shape =    [2]
```

从右侧比较

```text
3 和 2
```

两者不相同，也都不是 `1`，因此不能广播。

```python
a = torch.randn(2, 3)
b = torch.randn(2)

# c = a + b  # shape 不兼容
```

很多初学者会误以为一维 Tensor 的大小 `2` 会自动对应第一维，实际上广播始终从最右侧对齐。

### 2.4.5 图像通道偏置

对于图像 Tensor

```text
x.shape = [B, C, H, W]
```

如果希望每个通道增加一个不同的偏置，偏置应整理为

```text
bias.shape = [1, C, 1, 1]
```

代码

```python
x = torch.randn(8, 3, 32, 32)
bias = torch.randn(3)

bias = bias.view(1, 3, 1, 1)
y = x + bias

print(y.shape)  # [8, 3, 32, 32]
```

直接使用 `[C]` 会与最右侧的 `W` 对齐，而不是自动与通道维对齐。

### 2.4.6 常见隐蔽 Bug：`[B]` 与 `[B, 1]`

```python
prediction = torch.randn(8, 1)
target = torch.randn(8)

difference = prediction - target

print(difference.shape)  # [8, 8]
```

原因是

```text
prediction: [8, 1]
target:        [8]
补齐后：    [1, 8]
```

广播结果为

```text
[8, 8]
```

而期望通常是

```text
[8, 1]
```

应将 target 改为

```python
target = target.unsqueeze(1)

difference = prediction - target
print(difference.shape)  # [8, 1]
```

这是回归任务中非常常见的错误，代码可能不报错，但 loss 的语义完全错误。

### 2.4.7 `expand` 与 `repeat`

**`expand`**

```python
x = torch.tensor([[1.0, 2.0, 3.0]])

expanded = x.expand(4, 3)

print(expanded)
print(expanded.stride())
```

`expand()` 通常不复制底层数据，而是通过特殊 stride 将同一份数据映射到多个逻辑位置。

可能得到

```text
stride = (0, 1)
```

第一维 stride 为 `0`，表示沿第一维移动时，底层存储位置不变。

**`repeat`**

```python
repeated = x.repeat(4, 1)
```

`repeat()` 会实际复制数据。

二者区别

|    操作    | 是否复制数据 |   适用情况   |
| :------: | :----: | :------: |
| `expand` | 通常不复制  |   只读广播   |
| `repeat` |   是    | 需要独立重复数据 |

### 2.4.8 为什么不能随意修改 expand 结果

多个逻辑位置可能指向同一个物理元素。

```text
expanded[0, 0]
expanded[1, 0]
expanded[2, 0]
expanded[3, 0]
```

这些位置可能都映射到原始 `x[0, 0]`。

因此，不应直接对 expand 结果执行依赖独立位置的原地写操作。确实需要修改时，应先复制

```python
expanded = x.expand(4, 3).clone()
expanded[0, 0] = 100
```

## 2.5 Tensor 的 view、reshape、permute、transpose

这四个操作都与 Tensor 形状有关，但解决的问题不同

```text
view / reshape：
改变维度大小的组织方式

permute / transpose：
改变维度顺序
```

### 2.5.1 `view`

`view()` 在不改变底层数据的情况下，以新 shape 解释同一块存储。

```python
x = torch.arange(12)

y = x.view(3, 4)
z = y.view(2, 6)

print(y)
print(z)
```

它们共享底层数据

```python
z[0, 0] = 100

print(x[0])  # tensor(100)
```

`view()` 前后的元素总数必须一致

```text
原 shape：[12]
新 shape：[3, 4]

12 = 3 × 4
```

### 2.5.2 使用 `-1` 自动推断维度

```python
x = torch.arange(24)

a = x.view(2, 3, 4)
b = x.view(6, -1)

print(a.shape)  # [2, 3, 4]
print(b.shape)  # [6, 4]
```

只能有一个维度使用 `-1`

```python
# x.view(-1, -1)  # 无法推断
```

### 2.5.3 `reshape`

`reshape()` 也用于改变 shape

```python
x = torch.arange(12)
y = x.reshape(3, 4)
```

它与 `view()` 的关键区别是

- 如果当前内存布局兼容，`reshape()` 可以返回视图
- 如果不兼容，`reshape()` 可以创建连续副本
- 调用者不应依赖它最终一定共享或一定复制数据

PyTorch 官方明确说明，`reshape()` 在 stride 兼容时可以不复制，否则可能复制；代码不应依赖具体是视图还是副本。

### 2.5.4 `transpose`

`transpose(dim0, dim1)` 交换两个维度

```python
x = torch.randn(2, 3, 4)

y = x.transpose(0, 1)

print(x.shape)  # [2, 3, 4]
print(y.shape)  # [3, 2, 4]
```

它只交换指定的两个维度。

对于二维矩阵，也可以使用

```python
x = torch.randn(2, 3)
y = x.t()

print(y.shape)  # [3, 2]
```

### 2.5.5 `permute`

`permute()` 可以一次重新排列全部维度

```python
x = torch.randn(2, 3, 4)

y = x.permute(2, 0, 1)

print(y.shape)  # [4, 2, 3]
```

参数

```text
(2, 0, 1)
```

表示

```text
新第 0 维 = 原第 2 维
新第 1 维 = 原第 0 维
新第 2 维 = 原第 1 维
```

`permute()` 返回原 Tensor 的视图，不会仅为了交换维度而立即重新排列全部底层数据。

### 2.5.6 为什么 transpose 后 view 可能报错

```python
x = torch.arange(12).reshape(3, 4)
y = x.transpose(0, 1)

print(x.stride())  # (4, 1)
print(y.stride())  # (1, 4)
```

此时

```python
# flattened = y.view(-1)
```

通常会报错，因为 `y` 的逻辑元素顺序已经不能通过一个简单连续区域的重新分组来表示。

解决方式一

```python
flattened = y.reshape(-1)
```

`reshape()` 必要时会复制。

解决方式二

```python
flattened = y.contiguous().view(-1)
```

先按照 `y` 的逻辑顺序生成连续副本，再执行 `view()`。

### 2.5.7 四者对比

|     操作      |   主要作用   | 是否可能共享存储 |     是否可能复制     |
| :---------: | :------: | :------: | :------------: |
|   `view`    | 改变 shape |    是     | 不主动复制，条件不满足时报错 |
|  `reshape`  | 改变 shape |    是     |       是        |
| `transpose` |  交换两个维度  |    是     |     通常不复制      |
|  `permute`  |  重排多个维度  |    是     |     通常不复制      |

选择原则

```text
只改变 shape，且确定布局兼容：
view

只改变 shape，不想手动判断连续性：
reshape

交换两个轴：
transpose

重新排列多个轴：
permute
```

## 2.6 contiguous、stride 与底层存储

这是 Tensor 内存机制中最重要的部分。

### 2.6.1 Tensor 的逻辑结构与物理存储

可以将 Tensor 理解为

```text
Tensor = 底层 Storage + 元数据
```

元数据主要包括

- shape
- stride
- storage offset
- dtype
- device

同一块底层数据可以被不同 shape 和 stride 的 Tensor 解释。

### 2.6.2 stride 是什么

stride 表示：在某一维索引增加 1 时，需要在底层存储中跨过多少个元素。

例如

```python
x = torch.arange(6).reshape(2, 3)

print(x)
print(x.shape)   # [2, 3]
print(x.stride())  # (3, 1)
```

底层数据可以理解为

```text
Storage:
[0, 1, 2, 3, 4, 5]
```

逻辑结构

```text
[[0, 1, 2],
 [3, 4, 5]]
```

stride 为

```text
(3, 1)
```

表示

- 第一维索引增加 1，需要跨过 3 个元素
- 第二维索引增加 1，需要跨过 1 个元素

### 2.6.3 索引如何映射到底层存储

忽略字节大小时，一个位置的存储偏移可以近似表示为

```text
physical_offset
=
storage_offset
+
i₀ × stride₀
+
i₁ × stride₁
+
...
+
iₙ × strideₙ
```

对于

```text
x.shape  = [2, 3]
x.stride = [3, 1]
```

访问

```text
x[1, 2]
```

对应

```text
offset = 0 + 1 × 3 + 2 × 1 = 5
```

因此读取底层存储第 5 个位置，即数值 `5`。

### 2.6.4 transpose 为什么不需要立即复制数据

```python
x = torch.arange(6).reshape(2, 3)
y = x.transpose(0, 1)

print(y.shape)   # [3, 2]
print(y.stride())  # (1, 3)
```

底层 Storage 仍然是

```text
[0, 1, 2, 3, 4, 5]
```

只是 `y` 使用新的 shape 和 stride 解释它

```text
y =
[[0, 3],
 [1, 4],
 [2, 5]]
```

访问

```text
y[2, 1]
```

对应

```text
offset = 2 × 1 + 1 × 3 = 5
```

仍然读到底层数值 `5`。

因此，转置的核心不是立即搬动数据，而是修改维度和 stride。

### 2.6.5 什么是 contiguous

在默认连续内存格式下，如果 Tensor 的逻辑元素顺序与底层内存中的连续排列一致，可以称其为 contiguous。

```python
x = torch.arange(12).reshape(3, 4)

print(x.is_contiguous())  # True
```

转置后

```python
y = x.transpose(0, 1)

print(y.is_contiguous())  # False
```

`y` 的元素仍然有效，只是按照其逻辑顺序遍历时，不再依次访问相邻存储位置。

PyTorch 官方指出，视图操作可能将连续 Tensor 变成非连续 Tensor，`transpose()` 是常见例子；非连续布局可能对性能产生影响。

### 2.6.6 `contiguous()` 做了什么

```python
z = y.contiguous()

print(z.is_contiguous())  # True
print(z.stride())
```

如果 `y` 非连续，`contiguous()` 会按照 `y` 当前的逻辑顺序创建一份连续存储的数据副本。

逻辑过程

```text
原 Storage：
[0, 1, 2, 3, 4, 5]

y 的逻辑内容：
[[0, 3],
 [1, 4],
 [2, 5]]

contiguous 后的新 Storage：
[0, 3, 1, 4, 2, 5]
```

如果 Tensor 已经符合指定内存格式，`contiguous()` 可以直接返回原 Tensor。

### 2.6.7 切片也可能产生非连续 Tensor

```python
x = torch.arange(16).reshape(4, 4)
y = x[:, ::2]

print(y)
print(y.shape)           # [4, 2]
print(y.stride())        # 可能为 (4, 2)
print(y.is_contiguous()) # False
```

`y` 只选择每行的第 0、2 列。

底层访问模式类似

```text
0, 2, 4, 6, 8, 10, 12, 14
```

逻辑上连续遍历时，需要跳过部分元素，所以默认格式下不是连续 Tensor。

### 2.6.8 expand 的 stride 为 0

```python
x = torch.tensor([[1.0, 2.0, 3.0]])
y = x.expand(4, 3)

print(y.shape)   # [4, 3]
print(y.stride())  # (0, 1)
```

第一维 stride 为 `0`，意味着

```text
y[0, 0]
y[1, 0]
y[2, 0]
y[3, 0]
```

都映射到同一个底层元素。

这说明 stride 不只是“正常连续数组中的步长”，还可以表达

- 转置
- 跳步切片
- 广播
- 共享存储视图

### 2.6.9 非连续 Tensor 能否参与运算

可以。

很多 PyTorch 算子能够直接处理任意 stride 的 Tensor

```python
x = torch.randn(3, 4)
y = x.transpose(0, 1)

z = y + 1
```

但不同算子的实现不同

- 有些算子能够高效处理非连续 stride
- 有些算子会在内部生成连续副本
- 有些操作要求特定布局
- 不规则访存可能降低缓存和显存带宽利用率

因此，不应形成两个极端结论

```text
错误一：非连续 Tensor 不能计算
错误二：连续性永远不影响性能
```

### 2.6.10 是否应该到处调用 contiguous

不应该。

```python
x = x.contiguous()
```

如果 `x` 已经连续，开销通常很小；如果不连续，则可能发生真实的数据复制。

无条件、频繁调用会导致

- 额外内存分配
- 额外数据拷贝
- GPU 带宽开销
- 更高峰值显存

合理做法是

1. 在要求连续布局的操作之前使用
2. 在 `permute()` 后需要 `view()` 时使用
3. 根据 profiler 判断非连续布局是否成为性能瓶颈
4. 不要仅为了“保险”而在每一步调用

### 2.6.11 内存格式不只有一种

对于普通二维或序列 Tensor，通常讨论默认连续格式。

对于卷积图像 Tensor，PyTorch 还支持例如 `channels_last` 的内存格式。因此严格来说，“是否连续”还可以相对于某种 memory format 判断

```python
x = torch.randn(2, 3, 32, 32)

x_channels_last = x.contiguous(
    memory_format=torch.channels_last
)

print(
    x_channels_last.is_contiguous(
        memory_format=torch.channels_last
    )
)
```

这说明连续性并不等价于“只能存在一种合法内存布局”。

## 2.7 Tensor 与 NumPy 的相互转换

PyTorch Tensor 与 NumPy ndarray 的接口非常接近，并且可以高效相互转换。

### 2.7.1 NumPy 转 Tensor：`torch.from_numpy`

```python
import numpy as np
import torch

array = np.array(
    [1.0, 2.0, 3.0],
    dtype=np.float32,
)

tensor = torch.from_numpy(array)

print(tensor)
print(tensor.dtype)
```

`torch.from_numpy()` 创建的 CPU Tensor 与原 NumPy ndarray 共享底层内存。

修改 NumPy

```python
array[0] = 100.0

print(tensor)
# tensor([100., 2., 3.])
```

修改 Tensor

```python
tensor[1] = 200.0

print(array)
# [100. 200.   3.]
```

### 2.7.2 需要独立副本时

使用

```python
tensor = torch.from_numpy(array).clone()
```

或者

```python
tensor = torch.tensor(array)
```

此后修改其中一方通常不会影响另一方。

### 2.7.3 `torch.as_tensor`

```python
tensor = torch.as_tensor(array)
```

如果 NumPy 数组的 dtype 等条件兼容，`as_tensor()` 会尽量复用底层数据，而不是强制复制。

因此

```text
torch.tensor(array)
```

更偏向“我要一份独立 Tensor”。

```text
torch.as_tensor(array)
```

更偏向“尽量以低开销将其视为 Tensor”。

### 2.7.4 Tensor 转 NumPy

CPU Tensor 可以通过 `.numpy()` 转换

```python
tensor = torch.tensor(
    [1.0, 2.0, 3.0]
)

array = tensor.numpy()
```

在满足条件时，返回的 NumPy ndarray 与原 Tensor 共享内存。

```python
array[0] = 100.0

print(tensor)
# tensor([100., 2., 3.])
```

### 2.7.5 GPU Tensor 转 NumPy

NumPy 数组位于 CPU 内存，不能直接共享 CUDA 显存中的数据。

标准写法

```python
array = tensor.detach().cpu().numpy()
```

各步骤含义

```text
detach()
切断自动求导关系

cpu()
将数据复制到 CPU 内存

numpy()
创建 NumPy ndarray
```

注意

```python
tensor.cpu()
```

涉及设备数据传输，可能产生明显开销。

### 2.7.6 requires_grad Tensor 转 NumPy

下面代码通常不符合标准转换要求

```python
x = torch.tensor(
    [1.0, 2.0],
    requires_grad=True,
)

# array = x.numpy()
```

常规写法是

```python
array = x.detach().cpu().numpy()
```

原因是 NumPy 操作不属于 PyTorch Autograd 计算图。转换到 NumPy 后，后续 NumPy 计算不会被 PyTorch 自动求导记录。

### 2.7.7 NumPy 操作会中断梯度链路

```python
x = torch.tensor(
    [1.0, 2.0],
    requires_grad=True,
)

array = x.detach().numpy()
array = array * 2

y = torch.from_numpy(array)
```

新得到的 `y` 不再与原始 `x` 保持可求导计算关系。

因此，模型前向传播和损失计算中，不应为了方便随意转到 NumPy 再计算。

错误思路

```text
PyTorch Tensor
    ↓
NumPy 计算
    ↓
重新转回 Tensor
    ↓
期待梯度自动回传
```

这条路径不会自动恢复原有计算图。

### 2.7.8 标准转换总结

|            场景            |               推荐写法                |     内存关系      |
| :----------------------: | :-------------------------------: | :-----------: |
|    NumPy 转 Tensor，共享     |       `torch.from_numpy(a)`       |      共享       |
|   NumPy 转 Tensor，尽量复用    |       `torch.as_tensor(a)`        |     可能共享      |
|     NumPy 转独立 Tensor     |         `torch.tensor(a)`         |      复制       |
|    CPU Tensor 转 NumPy    |            `x.numpy()`            |     通常共享      |
| GPU / 带梯度 Tensor 转 NumPy |    `x.detach().cpu().numpy()`     | GPU 到 CPU 会复制 |
|      需要完全独立 ndarray      | `x.detach().cpu().numpy().copy()` |      独立       |

## 2.8 CPU Tensor 与 GPU Tensor

### 2.8.1 CPU 和 GPU 的职责差异

CPU 更擅长

- 复杂控制逻辑
- 分支较多的程序
- 数据加载与预处理
- 小规模低并行计算
- 操作系统和服务逻辑

GPU 更擅长

- 大规模矩阵乘法
- 卷积
- Attention
- 大批量逐元素计算
- 高并行度数值运算

因此，不能简单认为

```text
GPU 永远比 CPU 快
```

对于很小的 Tensor，GPU 可能受到以下成本影响

- Kernel 启动开销
- CPU 到 GPU 的数据传输
- GPU 同步
- 显存分配
- Python 调度

### 2.8.2 创建 CPU 和 GPU Tensor

CPU Tensor

```python
x_cpu = torch.randn(
    2,
    3,
    device="cpu",
)
```

GPU Tensor

```python
if torch.cuda.is_available():
    x_gpu = torch.randn(
        2,
        3,
        device="cuda:0",
    )
```

也可以先在 CPU 创建，再移动

```python
x_gpu = x_cpu.to("cuda:0")
```

### 2.8.3 模型与数据必须位于同一设备

标准训练代码

```python
import torch
from torch import nn

device = torch.device(
    "cuda" if torch.cuda.is_available() else "cpu"
)

model = nn.Linear(10, 2).to(device)

inputs = torch.randn(8, 10).to(device)
targets = torch.randint(
    0,
    2,
    (8,),
    device=device,
)

outputs = model(inputs)
```

如果

```text
model：cuda
inputs：cpu
```

则底层算子不能直接同时访问 CPU 内存和 GPU 显存，通常会报 device mismatch。

### 2.8.4 `.cuda()` 与 `.to()`

```python
x = x.cuda()
```

表示移动到 CUDA 设备。

更通用的写法是

```python
x = x.to(device)
```

`.to()` 可以同时处理

- device
- dtype
- 是否强制复制
- 是否尝试非阻塞传输

```python
x = x.to(
    device=device,
    dtype=torch.float16,
)
```

因此，工程代码中通常优先使用 `.to()`。

### 2.8.5 CPU 与 GPU 之间的数据传输

```python
x_cpu = torch.randn(1024, 1024)
x_gpu = x_cpu.to("cuda")
x_back = x_gpu.cpu()
```

传输路径

```text
CPU RAM
   ↓ Host-to-Device
GPU VRAM
   ↓ Device-to-Host
CPU RAM
```

数据传输通常经过 PCIe 或其他互联通道，带宽和延迟与 GPU 内部显存访问不同。

因此，在循环中频繁执行

```python
x = x.cpu()
x = x.cuda()
```

通常会严重影响性能。

### 2.8.6 CUDA 的异步执行

CUDA 操作通常相对于 CPU 是异步提交的。

例如

```python
outputs = model(inputs)
```

Python 返回到下一行时，GPU 计算不一定已经完全结束。

这使 CPU 可以继续提交后续任务，从而形成流水线，但也会带来计时和调试上的误区。

### 2.8.7 GPU 代码的错误计时方式

```python
import time

start = time.perf_counter()
outputs = model(inputs)
elapsed = time.perf_counter() - start
```

这个结果可能主要测量了 CPU 提交 GPU 任务的时间，而不是 GPU 真正完成计算的时间。

基本修正

```python
torch.cuda.synchronize()
start = time.perf_counter()

outputs = model(inputs)

torch.cuda.synchronize()
elapsed = time.perf_counter() - start
```

更适合 GPU 的方式是 CUDA Event

```python
start_event = torch.cuda.Event(enable_timing=True)
end_event = torch.cuda.Event(enable_timing=True)

start_event.record()

outputs = model(inputs)

end_event.record()
torch.cuda.synchronize()

elapsed_ms = start_event.elapsed_time(end_event)
print(elapsed_ms)
```

### 2.8.8 隐式同步操作

某些操作需要 CPU 获取 GPU 结果，因此可能触发同步，例如

```python
loss_value = loss.item()
x_cpu = x_gpu.cpu()
array = x_gpu.cpu().numpy()
```

如果在训练循环中每一步都频繁调用

```python
loss.item()
```

就可能增加 CPU-GPU 同步次数。

并不是说不能调用，而是应根据日志频率合理使用

```python
if step % 100 == 0:
    loss_value = loss.item()
```

### 2.8.9 pinned memory 与 non_blocking

普通 CPU 内存通常是 pageable memory。为了提高 CPU 到 CUDA GPU 的传输效率，可以使用 pinned memory。

DataLoader 中常见

```python
loader = DataLoader(
    dataset,
    batch_size=64,
    pin_memory=True,
)
```

传输时

```python
inputs = inputs.to(
    device,
    non_blocking=True,
)
```

当源数据位于 pinned memory 并满足相应条件时，`non_blocking=True` 可以支持相对于主机的异步传输，并为数据传输与计算重叠提供可能。PyTorch 官方推荐在 DataLoader 中使用自动内存锁页，并说明 `to(..., non_blocking=True)` 可用于相关异步拷贝场景。

需要注意

```text
non_blocking=True
```

不等于任何情况下都会自动获得明显加速。还需要

- pinned source memory
- 合理的 CUDA Stream
- 足够大的传输任务
- 可重叠的计算
- 避免立即同步

### 2.8.10 标准 GPU 训练片段

```python
device = torch.device(
    "cuda" if torch.cuda.is_available() else "cpu"
)

model = model.to(device)

for inputs, targets in loader:
    # non_blocking 主要配合 DataLoader 的 pin_memory=True
    inputs = inputs.to(
        device,
        non_blocking=True,
    )
    targets = targets.to(
        device,
        non_blocking=True,
    )

    optimizer.zero_grad(set_to_none=True)

    outputs = model(inputs)
    loss = criterion(outputs, targets)

    loss.backward()
    optimizer.step()
```

关键职责

```text
model.to(device)
移动模型参数和 Buffer

inputs.to(device)
移动输入数据

targets.to(device)
移动标签

model(inputs)
在模型与输入所在设备执行计算
```

### 2.8.11 GPU 不一定更快的场景

**Tensor 太小**

计算量不足以抵消 Kernel 启动开销。

**频繁数据传输**

每一步都在 CPU 与 GPU 之间来回移动。

**大量同步**

频繁调用 `.item()`、`.cpu()` 或同步 API。

**数据加载过慢**

GPU 经常等待 CPU 准备数据。

**模型并行度低**

模型包含大量串行逻辑或极小算子。

因此，判断 GPU 性能不能只看“代码是否用了 CUDA”，而要看完整执行链路。

## 2.9 本章小节

```text
Tensor
│
├── shape
│   └── 决定逻辑维度
│
├── dtype
│   └── 决定精度与元素大小
│
├── device
│   └── 决定存储和执行设备
│
├── Storage
│   └── 保存底层元素
│
├── stride
│   └── 将逻辑索引映射到 Storage
│
└── View
    └── 使用不同 shape / stride 解释同一 Storage
```

形状操作之间的关系

```text
view / reshape
改变 shape

transpose / permute
改变维度顺序

contiguous
按当前逻辑顺序重新生成连续存储
```

数据转换关系

```text
NumPy ndarray
    │
    ├── torch.from_numpy
    │      └── 共享 CPU 内存
    │
    ├── torch.as_tensor
    │      └── 尽可能复用
    │
    └── torch.tensor
           └── 复制数据
```

设备关系

```text
CPU Tensor
    │
    ├── .to("cuda")
    │      └── Host-to-Device 数据复制
    │
    └── NumPy 可直接交互

GPU Tensor
    │
    ├── 执行 CUDA 算子
    ├── 默认异步提交
    └── 转 NumPy 前需要移动到 CPU
```

完成本章后，应能够独立解释

1. Tensor 的 shape、dtype、device 分别有什么作用
2. 0 维 Tensor 与单元素向量有什么区别
3. `torch.tensor`、`as_tensor` 和 `from_numpy` 的区别
4. 基础索引与高级索引是否共享内存
5. PyTorch 广播规则为什么从右侧开始比较
6. `[B]` 与 `[B, 1]` 为什么可能广播成 `[B, B]`
7. `expand` 和 `repeat` 的区别
8. `view` 和 `reshape` 的区别
9. `transpose` 和 `permute` 的区别
10. stride 如何将逻辑索引映射到底层存储
11. `transpose` 为什么通常不会立即复制数据
12. `contiguous()` 什么时候会复制数据
13. Tensor 与 NumPy 转换时什么时候共享内存
14. GPU Tensor 为什么不能直接转为普通 NumPy 数组
15. CUDA 异步执行为什么会导致计时不准确
16. GPU 为什么不一定比 CPU 快

# 3 Autograd 自动求导机制

Autograd 是 PyTorch 的自动微分系统，也是模型能够通过反向传播训练的核心基础。

一次典型训练过程可以概括为

```text
输入数据
   ↓
模型前向计算
   ↓
动态构建计算图
   ↓
计算损失
   ↓
Autograd 反向传播
   ↓
得到参数梯度
   ↓
优化器更新参数
```

需要首先区分三个职责

|         操作         |      主要职责       |
| :----------------: | :-------------: |
|     `forward`      | 根据输入计算输出并建立计算依赖 |
|     `backward`     |   根据链式法则计算梯度    |
| `optimizer.step()` |    根据梯度更新参数     |

因此

```python
loss.backward()
```

只负责计算梯度，并不会自动更新模型参数。

## 3.1 为什么需要自动求导

### 3.1.1 神经网络训练的本质

神经网络训练的目标，是找到一组参数，使损失函数尽可能小。

假设模型参数为

```text
θ = {w₁, w₂, ..., wₙ}
```

损失函数为

```text
L(θ)
```

优化器需要知道损失函数关于每个参数的梯度

```text
∂L/∂w₁
∂L/∂w₂
...
∂L/∂wₙ
```

以最简单的梯度下降为例

```text
w ← w - learning_rate × ∂L/∂w
```

所以，模型训练必须解决一个核心问题：如何高效、准确地计算损失函数相对于大量模型参数的梯度？

### 3.1.2 手动求导为什么不可行

对于简单函数

```text
y = x² + 3x
```

可以直接写出

```text
dy/dx = 2x + 3
```

但实际神经网络可能包含

- 数百层网络结构
- 数百万到数千亿个参数
- 矩阵乘法
- 卷积
- Attention
- 激活函数
- 归一化
- 残差连接
- 多分支计算
- 多个损失函数

如果要求开发者为每一个模型手动实现完整反向传播，会产生三个问题。

**问题一：开发成本高**

模型结构稍有变化，反向传播代码就可能需要同步修改。

**问题二：容易出错**

复杂计算图中的梯度依赖关系非常多，手动推导容易出现

- 遗漏梯度分支
- shape 错误
- 转置错误
- 广播维度错误
- 符号错误

**问题三：难以复用**

每种算子都需要重新处理其梯度逻辑，模型组合能力很差。

### 3.1.3 自动微分、符号微分与数值微分

求导方法大致可以分为三类。

|  方法  |     基本思想      |    主要问题     |
| :--: | :-----------: | :---------: |
| 符号微分 | 根据数学表达式推导新表达式 |  表达式可能急剧膨胀  |
| 数值微分 |  使用有限差分近似梯度   |  误差大、计算开销高  |
| 自动微分 | 记录基本操作并应用链式法则 | 需要维护计算图和中间值 |

数值微分可以近似为

```text
f'(x) ≈ [f(x + ε) - f(x - ε)] / 2ε
```

它通常需要对每个参数多次执行前向计算，因此不适合直接用于大规模神经网络训练。

自动微分并不是对完整表达式做符号化推导，也不是用有限差分近似，而是将复杂函数拆分成一系列基础操作，再通过链式法则精确组合局部导数。

### 3.1.4 为什么神经网络适合反向模式自动微分

神经网络训练通常具有以下特点

```text
大量参数 → 一个标量损失
```

例如

```text
10 亿个参数 → 1 个 loss
```

反向模式自动微分可以从这个标量输出出发，一次反向遍历计算图，得到损失相对于所有参数的梯度。

PyTorch Autograd 主要使用反向模式自动微分，记录产生 Tensor 的操作，并从输出向输入反向应用链式法则。

### 3.1.5 自动求导带来的工程价值

Autograd 让开发者只需要描述前向计算

```python
prediction = model(inputs)
loss = criterion(prediction, targets)
```

然后调用

```python
loss.backward()
```

框架即可计算所有参与计算的可训练参数梯度。

这样模型开发被分成两个层次

```text
开发者：
描述模型如何从输入得到输出

Autograd：
根据前向计算关系生成反向传播过程
```

只要前向过程由 PyTorch 支持的可微操作构成，模型结构发生变化时，梯度计算逻辑也会随计算图自动变化。

## 3.2 计算图的基本概念

### 3.2.1 什么是计算图

计算图是对数据依赖关系和计算过程的图结构表示。

例如

```python
import torch

x = torch.tensor(
    2.0,
    requires_grad=True,
)

a = x * 3
b = a + 1
y = b ** 2
```

对应的计算过程为

```text
x
│
├── 乘法：a = x × 3
│
a
│
├── 加法：b = a + 1
│
b
│
├── 平方：y = b²
│
y
```

计算图记录了

- `a` 依赖 `x`
- `b` 依赖 `a`
- `y` 依赖 `b`

反向传播时，PyTorch 可以按照相反方向计算

```text
∂y/∂b
   ↓
∂b/∂a
   ↓
∂a/∂x
```

### 3.2.2 计算图为什么是有向无环图

常见的前向计算图可以看作有向无环图，即 DAG

```text
Directed Acyclic Graph
```

“有向”表示计算依赖具有方向

```text
输入 → 中间结果 → 输出
```

“无环”表示一次前向计算中，某个结果不会沿计算依赖重新成为自己的祖先。

循环神经网络虽然在概念上存在循环，但实际执行时会按时间步展开

```text
h₀ → h₁ → h₂ → h₃
```

展开后的本次计算仍可表示为无环图。

### 3.2.3 图中的节点和边

从概念上看，计算图包含

- Tensor 数据
- 产生 Tensor 的操作
- Tensor 和操作之间的依赖关系

PyTorch 内部主要将反向操作表示为 Autograd `Node`。输出 Tensor 的 `grad_fn` 指向生成它的反向节点，节点之间通过边连接。反向传播时，Autograd Engine 根据这些依赖执行对应的反向函数。

可以简化理解为

```text
Tensor y
   │
grad_fn
   ↓
PowBackward
   ↓
AddBackward
   ↓
MulBackward
   ↓
AccumulateGrad(x)
```

其中

- `PowBackward`：负责平方操作的反向计算
- `AddBackward`：负责加法操作的反向计算
- `MulBackward`：负责乘法操作的反向计算
- `AccumulateGrad`：将最终梯度累积到叶子 Tensor 的 `.grad`

### 3.2.4 PyTorch 的计算图是动态建立的

PyTorch 默认采用 Define-by-Run 方式：程序实际执行了哪些操作，本次就建立怎样的计算图。

```python
x = torch.randn(
    4,
    requires_grad=True,
)

if x.mean() > 0:
    y = x.square()
else:
    y = x.abs()

loss = y.sum()
loss.backward()
```

本次前向执行哪个分支，Autograd 就记录哪个分支。

```text
条件成立：
x → square → sum

条件不成立：
x → abs → sum
```

下一批数据可能执行另一个分支，并建立不同的计算图。

### 3.2.5 每次前向都会重新建立图

考虑一个标准训练循环

```python
for inputs, targets in loader:
    outputs = model(inputs)
    loss = criterion(outputs, targets)

    loss.backward()
    optimizer.step()
```

每次执行

```python
outputs = model(inputs)
```

都会根据本轮实际运行的操作建立新的计算图。

默认情况下，一次反向传播结束后，PyTorch 会释放不再需要的中间缓冲区，以降低内存和显存占用；这也是同一张图通常不能直接执行第二次 `backward()` 的原因。

### 3.2.6 反向传播需要保存哪些信息

不同操作的反向计算需要不同的前向信息。

例如

```text
y = x²
```

梯度为

```text
dy/dx = 2x
```

反向计算需要知道前向输入 `x`。

而对于某些操作，反向过程可能需要保存

- 输入 Tensor
- 输出 Tensor
- shape
- mask
- 索引
- 归一化统计量
- 其他中间结果

这些为反向传播保存的 Tensor 通常称为 saved tensors。PyTorch 会尽量管理并在反向完成后释放它们。

这也是训练显存通常高于推理显存的重要原因之一

```text
训练：
参数 + 激活值 + 反向所需中间结果 + 梯度 + 优化器状态

推理：
主要是参数 + 当前计算所需中间结果
```

### 3.2.7 分支计算中的梯度汇合

考虑

```python
x = torch.tensor(
    2.0,
    requires_grad=True,
)

a = x * 3
b = x ** 2
y = a + b

y.backward()
```

数学上

```text
y = 3x + x²
dy/dx = 3 + 2x
```

当 `x = 2` 时

```text
dy/dx = 3 + 4 = 7
```

计算图

```text
       ┌── × 3 ──┐
x ─────┤         ├── + ── y
       └── x² ───┘
```

反向传播时，来自两个分支的梯度会在 `x` 处相加。

这体现了多变量链式法则

```text
总梯度 = 所有依赖路径贡献之和
```

### 3.2.8 原地操作为什么可能破坏计算图

假设某个反向操作需要使用前向时保存的 Tensor，但该 Tensor 后续被原地修改

```python
x.add_(1)
```

那么反向传播看到的值可能已经不是前向时的值。

PyTorch 会为 Tensor 维护版本信息，并在反向过程中检查保存的 Tensor 是否被不安全地原地修改；如果检测到可能导致错误梯度的修改，通常会抛出异常，而不是静默返回错误结果。

因此，训练代码中不应为了节省一点内存而随意使用原地操作。

## 3.3 `requires_grad`、`grad`、`grad_fn`

这三个属性描述的是不同问题

```text
requires_grad：
是否需要追踪与该 Tensor 相关的梯度计算

grad：
反向传播后累积得到的梯度值

grad_fn：
该 Tensor 是由哪个被记录的操作产生的
```

### 3.3.1 `requires_grad`

创建 Tensor 时可以指定

```python
x = torch.tensor(
    [1.0, 2.0, 3.0],
    requires_grad=True,
)
```

也可以后续原地设置

```python
x.requires_grad_(True)
```

当梯度模式开启，并且一个可微操作至少有一个输入需要梯度时，该操作会被 Autograd 记录，输出通常也会设置为需要梯度。`requires_grad` 只表示梯度计算是否需要经过该 Tensor，不等价于它的 `.grad` 一定会被保存。

示例

```python
x = torch.tensor(
    2.0,
    requires_grad=True,
)

y = x * 3
z = y ** 2

print(x.requires_grad)  # True
print(y.requires_grad)  # True
print(z.requires_grad)  # True
```

### 3.3.2 哪些 dtype 可以求梯度

Autograd 面向连续数值变化，因此只有浮点和复数类型的 Tensor 可以设置 `requires_grad=True`。

下面的整数 Tensor不能直接要求梯度

```python
# x = torch.tensor(
#     [1, 2, 3],
#     requires_grad=True,
# )
```

模型中的 token ID、类别标签等整数 Tensor 不需要梯度。

### 3.3.3 `requires_grad=False` 是否阻断整个计算图

不一定。

考虑

```python
x = torch.tensor(
    2.0,
    requires_grad=True,
)

constant = torch.tensor(
    3.0,
    requires_grad=False,
)

y = x * constant
y.backward()

print(x.grad)  # tensor(3.)
```

`constant` 不需要梯度，但 `x` 需要梯度，所以乘法操作仍然需要记录，以便计算 `∂y/∂x`。

因此

```text
某个输入 requires_grad=False
```

不代表整个操作都不会被记录。

只有在当前梯度模式下，所有相关输入都不需要梯度时，结果通常才不会建立反向计算关系。

### 3.3.4 冻结模型参数

模型参数通常默认设置为 `requires_grad=True`。

冻结部分网络

```python
for parameter in model.backbone.parameters():
    parameter.requires_grad_(False)
```

冻结之后

- 前向计算仍然会执行
- 这些参数不会累积梯度
- 优化器无法根据不存在的梯度更新它们
- 如果输入或其他参数仍需要梯度，相关计算仍可能参与计算图

解冻

```python
for parameter in model.backbone.parameters():
    parameter.requires_grad_(True)
```

如果参数分组发生变化，还应检查优化器是否已经包含这些参数。

### 3.3.5 `.grad`

执行反向传播后，叶子 Tensor 的梯度通常累积到 `.grad`

```python
x = torch.tensor(
    2.0,
    requires_grad=True,
)

y = x ** 2
y.backward()

print(x.grad)  # tensor(4.)
```

数学上

```text
y = x²
dy/dx = 2x = 4
```

在反向之前

```python
print(x.grad)  # None
```

这表示尚未有梯度写入，而不是梯度值为零。

### 3.3.6 `.grad` 与 `requires_grad` 的区别

```text
requires_grad=True
```

表示为了后续反向传播，需要追踪与该 Tensor 有关的计算。

```text
tensor.grad
```

表示反向传播完成后实际保存的梯度。

例如，中间 Tensor 可以

```text
requires_grad=True
```

但默认

```text
grad is None
```

因为 PyTorch 默认主要将梯度累积到需要梯度的叶子 Tensor。

### 3.3.7 `grad_fn`

非叶子 Tensor 如果由被 Autograd 记录的操作产生，通常具有 `grad_fn`

```python
x = torch.tensor(
    2.0,
    requires_grad=True,
)

y = x * 3
z = y.sum()

print(x.grad_fn)  # None
print(y.grad_fn)  # <MulBackward...>
print(z.grad_fn)  # <SumBackward...>
```

其中

- `x` 是用户直接创建的叶子 Tensor，没有生成它的被记录操作
- `y` 由乘法产生
- `z` 由求和产生

`grad_fn` 指向负责该结果反向计算的函数对象。

### 3.3.8 `backward()` 与 `torch.autograd.grad()`

两者都可以计算梯度，但使用方式不同。

**`backward()`**

```python
y.backward()
print(x.grad)
```

主要特点

- 将梯度累积到叶子 Tensor 的 `.grad`
- 适合标准模型训练
- 与优化器工作流自然配合

**`torch.autograd.grad()`**

```python
gradient = torch.autograd.grad(
    outputs=y,
    inputs=x,
)

print(gradient[0])
```

主要特点

- 直接返回指定输出相对于指定输入的梯度
- 默认不写入 `x.grad`
- 更适合高阶导数、梯度惩罚和精确控制求导对象

`torch.autograd.grad()` 返回指定输出相对于指定输入的梯度，而 `backward()` 通常将梯度累积到图中的叶子 Tensor。

## 3.4 叶子节点与非叶子节点

### 3.4.1 什么是叶子 Tensor

对于 `requires_grad=True` 的 Tensor，若它由用户直接创建，而不是某个被 Autograd 记录的操作产生，则通常是叶子 Tensor。

```python
x = torch.tensor(
    [1.0, 2.0],
    requires_grad=True,
)

print(x.is_leaf)   # True
print(x.grad_fn)   # None
```

模型参数通常也是叶子 Tensor

```python
import torch.nn as nn

layer = nn.Linear(4, 2)

for parameter in layer.parameters():
    print(parameter.is_leaf)  # True
```

从 PyTorch 的定义看，`requires_grad=False` 的 Tensor 按约定也属于叶子 Tensor；而对需要梯度的 Tensor，只有用户创建、没有被记录操作作为创建者的 Tensor 才是叶子。

### 3.4.2 什么是非叶子 Tensor

由可求导操作生成的中间结果通常是非叶子 Tensor

```python
x = torch.tensor(
    2.0,
    requires_grad=True,
)

y = x * 3
z = y ** 2

print(y.is_leaf)  # False
print(z.is_leaf)  # False
```

因为

```text
y 由乘法生成
z 由平方生成
```

它们都有对应的 `grad_fn`。

### 3.4.3 为什么默认只保存叶子梯度

训练模型时，优化器最终需要的是参数梯度

```text
∂loss/∂parameter
```

中间 Tensor 的梯度主要用于继续向前传播梯度，通常没有必要长期保存。

如果每个中间激活都自动保留 `.grad`，会增加

- 内存占用
- 写入开销
- Tensor 生命周期
- 显存压力

因此，PyTorch 默认将中间梯度用于反向计算，但不保存在非叶子 Tensor 的 `.grad` 字段中。

### 3.4.4 非叶子 Tensor 的 `.grad`

```python
x = torch.tensor(
    2.0,
    requires_grad=True,
)

y = x * 3
z = y ** 2

z.backward()

print(x.grad)  # tensor(36.)
print(y.grad)  # 通常为 None
```

数学上

```text
z = (3x)² = 9x²
dz/dx = 18x
```

当 `x = 2`

```text
dz/dx = 36
```

虽然反向传播过程中计算了

```text
dz/dy = 2y
```

但默认没有把它长期保存在 `y.grad`。

### 3.4.5 使用 `retain_grad()` 保留中间梯度

需要检查中间梯度时

```python
x = torch.tensor(
    2.0,
    requires_grad=True,
)

y = x * 3
y.retain_grad()

z = y ** 2
z.backward()

print(x.grad)  # tensor(36.)
print(y.grad)  # tensor(12.)
```

因为

```text
y = 6
dz/dy = 2y = 12
```

`retain_grad()` 允许非叶子 Tensor 在反向传播后填充 `.grad`；对叶子 Tensor 调用它没有额外作用。

典型用途

- 调试梯度流
- 可视化中间层梯度
- 分析梯度消失或爆炸
- 实现特定梯度算法

### 3.4.6 类型或设备转换对叶子属性的影响

对于需要梯度的 Tensor，如果通过一个被记录的操作生成新 Tensor，新 Tensor可能变成非叶子。

```python
x = torch.randn(
    3,
    requires_grad=True,
)

y = x.to(torch.float64)

print(x.is_leaf)  # True
print(y.is_leaf)  # False
```

因为类型转换生成了一个依赖 `x` 的新 Tensor。

如果优化器要直接优化某个 Tensor，通常应确保它是需要梯度的叶子 Tensor。

模型参数通过 `nn.Parameter` 和 `nn.Module` 进行管理时，正常使用

```python
model.to(device)
```

即可，不需要手动重建参数。

## 3.5 forward 与 backward 的执行流程

### 3.5.1 前向传播做了什么

前向传播不只是计算模型输出，还可能为反向传播建立必要信息。

```python
outputs = model(inputs)
loss = criterion(outputs, targets)
```

这个过程主要包括

1. 根据输入执行 Tensor 算子
2. 生成中间 Tensor
3. 记录可求导操作之间的依赖
4. 为部分操作保存反向所需的中间值
5. 得到最终输出或标量损失

可以概括为

```text
执行数值计算
      +
建立反向计算依赖
      +
保存反向所需信息
```

### 3.5.2 `backward()` 从哪里开始

标准训练通常对标量损失调用

```python
loss.backward()
```

标量可以理解为只有一个输出值

```text
loss.shape = []
```

此时，PyTorch 隐式使用

```text
∂loss/∂loss = 1
```

作为反向传播的初始梯度。

### 3.5.3 标量输出的反向传播

示例

```python
x = torch.tensor(
    2.0,
    requires_grad=True,
)

y = 3 * x + 1
loss = y ** 2

loss.backward()

print(x.grad)  # tensor(42.)
```

数学推导

```text
y = 3x + 1
loss = y²

∂loss/∂y = 2y
∂y/∂x = 3

∂loss/∂x
= ∂loss/∂y × ∂y/∂x
= 2y × 3
```

当 `x = 2` 时

```text
y = 7
∂loss/∂x = 2 × 7 × 3 = 42
```

### 3.5.4 非标量输出为什么需要传入梯度

考虑

```python
x = torch.tensor(
    [1.0, 2.0, 3.0],
    requires_grad=True,
)

y = x ** 2
```

此时

```text
y = [x₁², x₂², x₃²]
```

`y` 不是标量。严格来说，它对 `x` 的导数是 Jacobian 矩阵，而不是一个与 `x` 同形状的普通标量梯度。

因此，直接执行

```python
# y.backward()
```

通常会报错。

可以提供一个上游梯度向量

```python
gradient = torch.tensor(
    [1.0, 1.0, 1.0]
)

y.backward(gradient)

print(x.grad)
# tensor([2., 4., 6.])
```

这实际上计算的是 vector-Jacobian product，更准确地说是上游向量与 Jacobian 的乘积。PyTorch 的 `backward` 对非标量输出要求提供形状兼容的 `gradient` 或 `grad_tensors`。

日常训练通常先把逐样本损失归约为标量

```python
loss = per_sample_loss.mean()
loss.backward()
```

### 3.5.5 反向传播的关键过程

可以将 `backward()` 简化为

```text
1. 从 loss 对应节点开始
2. 设置初始上游梯度
3. 根据依赖关系反向调度节点
4. 每个节点计算局部梯度
5. 将梯度传递给其输入节点
6. 多条路径的梯度相加
7. 将结果累积到叶子 Tensor 的 .grad
8. 释放不再需要的中间缓冲
```

对于某一操作

```text
y = f(x)
```

如果收到上游梯度

```text
∂L/∂y
```

当前反向节点计算

```text
∂L/∂x
=
∂L/∂y × ∂y/∂x
```

这就是链式法则在计算图中的局部执行。

### 3.5.6 多条依赖路径为什么要相加

如果

```text
y = f(x) + g(x)
```

则

```text
∂y/∂x = ∂f/∂x + ∂g/∂x
```

因此，同一个 Tensor 在计算图中被多个后续节点使用时，来自不同路径的梯度会累加。

```python
x = torch.tensor(
    2.0,
    requires_grad=True,
)

a = x * 3
b = x * 4
loss = a + b

loss.backward()

print(x.grad)  # tensor(7.)
```

### 3.5.7 backward 后图发生什么

默认情况下，反向传播完成后，PyTorch 会释放反向计算所需的已保存中间值。

因此

```python
loss.backward()
# loss.backward()
```

第二次执行通常报错。

这并不表示所有 Tensor 对象都立刻消失，而是反向传播所需的部分中间缓冲已被释放。

如果下一轮重新执行前向

```python
loss = model(inputs)
```

会建立一张新的计算图，可以正常再次调用 `backward()`。

### 3.5.8 `backward()` 不负责更新参数

标准流程

```python
optimizer.zero_grad()

outputs = model(inputs)
loss = criterion(outputs, targets)

loss.backward()
optimizer.step()
```

其中

```text
loss.backward()
计算并累积 parameter.grad

optimizer.step()
读取 parameter.grad 并更新 parameter
```

如果只调用 `backward()`

```python
loss.backward()
```

参数值不会自动变化。

如果只调用

```python
optimizer.step()
```

但没有正确梯度，优化器也无法完成预期更新。

## 3.6 梯度累积机制

### 3.6.1 PyTorch 为什么默认累积梯度

调用 `backward()` 时，梯度会累加到已有的 `.grad`，而不是自动覆盖。

```python
x = torch.tensor(
    2.0,
    requires_grad=True,
)

y = x ** 2
y.backward()

print(x.grad)  # tensor(4.)

y = x ** 2
y.backward()

print(x.grad)  # tensor(8.)
```

第一次反向得到

```text
4
```

第二次又得到

```text
4
```

最终

```text
x.grad = 4 + 4 = 8
```

PyTorch 的 `backward()` 会将梯度累积到叶子 Tensor，因此在标准训练中需要显式清理梯度。

### 3.6.2 两种不同层面的梯度相加

需要区分两类累积。

**同一次 backward 内部的路径累积**

```text
x → 分支 A ─┐
             ├→ loss
x → 分支 B ─┘
```

来自不同分支的梯度必须相加，这是链式法则本身的要求。

**多次 backward 之间的 `.grad` 累积**

```python
loss1.backward()
loss2.backward()
```

第二次 backward 的结果会加到第一次保存的 `.grad` 上。

这属于 PyTorch 梯度缓冲区的累积行为。

### 3.6.3 为什么设计为累积而不是覆盖

梯度累积支持多种场景。

**多个损失共同优化**

```python
total_loss = classification_loss + auxiliary_loss
total_loss.backward()
```

也可以在需要时分别反向，不过必须正确处理计算图生命周期。

**模拟更大的 batch**

当 GPU 无法容纳较大 batch 时，可以使用多个 micro-batch 累积梯度。

**多个计算分支共享参数**

同一参数可能在模型中被多次使用，其梯度应等于所有路径贡献之和。

### 3.6.4 梯度累积模拟大 batch

假设目标有效 batch size 为

```text
effective_batch_size = 128
```

显存只能处理

```text
micro_batch_size = 32
```

则可以累积四步

```text
accumulation_steps = 128 / 32 = 4
```

标准代码

```python
accumulation_steps = 4

optimizer.zero_grad(set_to_none=True)

for step, (inputs, targets) in enumerate(loader):
    outputs = model(inputs)
    loss = criterion(outputs, targets)

    # 对多步梯度取平均
    loss = loss / accumulation_steps
    loss.backward()

    if (step + 1) % accumulation_steps == 0:
        optimizer.step()
        optimizer.zero_grad(set_to_none=True)
```

为什么除以 `accumulation_steps`？

因为如果每个 micro-batch 的 loss 已经是均值，直接相加会使总梯度放大约四倍。

```text
未缩放：
g₁ + g₂ + g₃ + g₄

缩放后：
(g₁ + g₂ + g₃ + g₄) / 4
```

这样更接近一个大 batch 上的平均梯度。

### 3.6.5 梯度累积并不完全等价于大 batch

它在很多情况下能够近似大 batch，但不一定严格等价，原因包括

- BatchNorm 的统计量仍按 micro-batch 计算
- Dropout 每个 micro-batch 的随机 mask 不同
- 数据增强具有随机性
- 优化器和学习率调度调用时机不同
- 最后一个累积窗口可能不足完整步数
- 分布式训练中梯度同步策略可能不同

因此，梯度累积主要解决有效 batch 和显存之间的矛盾，但不能简单认为任何模型中都与真实大 batch 完全一致。

### 3.6.6 意外梯度累积的常见表现

如果训练循环忘记清理梯度

```python
for inputs, targets in loader:
    outputs = model(inputs)
    loss = criterion(outputs, targets)

    loss.backward()
    optimizer.step()
```

则每一轮参数更新使用的是

```text
当前 batch 梯度
+
之前所有未清理的梯度
```

可能导致

- 参数更新越来越大
- loss 波动或发散
- 梯度范数异常
- 训练结果难以解释

## 3.7 `zero_grad` 的作用

### 3.7.1 为什么需要 `zero_grad`

因为梯度默认累积，所以在开始一次新的独立梯度计算前，需要清理旧梯度。

标准训练流程

```python
for inputs, targets in loader:
    optimizer.zero_grad(set_to_none=True)

    outputs = model(inputs)
    loss = criterion(outputs, targets)

    loss.backward()
    optimizer.step()
```

逻辑为

```text
清理上一步梯度
      ↓
计算当前 batch 的 loss
      ↓
计算当前 batch 梯度
      ↓
使用当前梯度更新参数
```

### 3.7.2 `zero_grad` 清理的是什么

`optimizer.zero_grad()` 清理的是优化器管理参数的 `.grad`。

它不会

- 修改参数值
- 清空模型输出
- 删除优化器状态
- 清理 CUDA 缓存
- 自动释放所有计算图

例如：

```python
print(parameter.grad)

optimizer.zero_grad()

print(parameter.grad)
```

清理前可能是一个 Tensor，清理后可能变为 `None` 或零 Tensor，取决于 `set_to_none`。

### 3.7.3 `set_to_none=True`

推荐显式写

```python
optimizer.zero_grad(set_to_none=True)
```

此时

```text
parameter.grad = None
```

而不是将已有梯度 Tensor 全部填为零。

PyTorch 文档指出，设为 `None` 通常可以降低内存操作并带来一定性能收益，但也会改变部分行为：本轮未收到梯度的参数仍保持 `.grad=None`，优化器通常会跳过该参数，而零梯度可能仍导致优化器执行一步。

### 3.7.4 `None` 和零梯度的区别

假设某个参数本轮没有参与计算。

**`.grad = None`**

表示

```text
本轮没有为该参数产生梯度
```

优化器通常跳过这个参数。

**`.grad = 0`**

表示

```text
本轮为该参数产生了数值为 0 的梯度
```

对于带有动量、权重衰减等状态的优化器，即使梯度为零，也可能发生状态或参数变化。

因此

```text
None
```

与

```text
全零 Tensor
```

语义并不完全相同。

### 3.7.5 `optimizer.zero_grad()` 与 `model.zero_grad()`

**`optimizer.zero_grad()`**

清理该优化器所管理参数的梯度

```python
optimizer.zero_grad()
```

**`model.zero_grad()`**

清理模型已注册参数的梯度

```python
model.zero_grad()
```

单模型、单优化器场景下，它们通常作用于同一批参数。

但在以下场景中可能不同

- 一个模型使用多个优化器
- 优化器只管理部分参数
- 多模型联合训练
- 部分参数没有加入优化器

工程中更常用

```python
optimizer.zero_grad(set_to_none=True)
```

因为它明确对应接下来要执行的参数更新。

### 3.7.6 `zero_grad` 应放在哪里

以下写法是标准形式

```python
for batch in loader:
    optimizer.zero_grad(set_to_none=True)

    loss = compute_loss(batch)
    loss.backward()

    optimizer.step()
```

也可以在 `optimizer.step()` 后清理，只要保证下一次 `backward()` 前旧梯度已被正确清除。

不过，放在迭代开始通常更容易理解

```text
这一轮开始
→ 清理旧梯度
→ 计算这一轮梯度
→ 更新参数
```

### 3.7.7 梯度累积时不要每个 micro-batch 都清零

错误写法

```python
for micro_batch in micro_batches:
    optimizer.zero_grad()

    loss = compute_loss(micro_batch)
    loss.backward()
```

每次反向前都清零，会导致前一个 micro-batch 的梯度丢失。

正确逻辑

```python
optimizer.zero_grad(set_to_none=True)

for micro_batch in micro_batches:
    loss = compute_loss(micro_batch)
    loss.backward()

optimizer.step()
optimizer.zero_grad(set_to_none=True)
```

## 3.8 `detach`、`no_grad`、`inference_mode` 的区别

三者都可能使某些计算不参与反向传播，但作用范围和约束不同。

|            机制            |   作用对象    |         主要作用         |
| :----------------------: | :-------: | :------------------: |
|        `detach()`        | 某个 Tensor |      从当前计算图中断开       |
|    `torch.no_grad()`     |  一个代码区域   |       暂停记录反向计算       |
| `torch.inference_mode()` |  一个代码区域   | 更严格地关闭 Autograd 相关开销 |

### 3.8.1 `detach()`

```python
x = torch.tensor(
    2.0,
    requires_grad=True,
)

y = x * 3
z = y.detach()

print(y.requires_grad)  # True
print(z.requires_grad)  # False
print(z.grad_fn)        # None
```

`detach()` 返回一个与当前计算图断开的 Tensor。结果不再需要梯度，但它与原 Tensor 共享底层存储，因此对任一方进行原地修改都可能影响另一方。

```python
x = torch.tensor(
    [1.0, 2.0],
    requires_grad=True,
)

y = x * 2
z = y.detach()

z[0] = 100

print(y)
# tensor([100., 4.], grad_fn=<MulBackward...>)
```

如果希望完全独立

```python
z = y.detach().clone()
```

### 3.8.2 `detach()` 阻断哪一部分梯度

```python
x = torch.tensor(
    2.0,
    requires_grad=True,
)

a = x * 3
b = a.detach()
y = b * 4
```

依赖关系变成

```text
x → a     b → y
      断开
```

`y` 不再通过 `b` 向 `a` 和 `x` 传播梯度。

常见场景

- GAN 中阻止某一网络接收另一阶段梯度
- 截断 RNN 隐状态的历史图
- 将中间结果作为常量使用
- 转换为 NumPy
- 日志或可视化

### 3.8.3 `torch.no_grad()`

```python
x = torch.tensor(
    2.0,
    requires_grad=True,
)

with torch.no_grad():
    y = x * 3

print(y.requires_grad)  # False
print(y.grad_fn)        # None
```

`no_grad()` 在其代码区域内暂停反向模式梯度记录，即使输入需要梯度，普通运算结果通常也不会建立 `grad_fn`。它适合验证、参数更新和不需要反向传播的辅助计算。

### 3.8.4 `no_grad()` 与 `detach()` 的作用范围

**`detach()`**

针对一个 Tensor

```python
detached = tensor.detach()
```

之后从该 Tensor 出发的计算不再连接原图。

**`no_grad()`**

针对一个代码区域

```python
with torch.no_grad():
    output1 = model1(x)
    output2 = model2(output1)
```

区域内执行的操作都不会正常记录反向图。

因此

```text
detach：
切断一条数据路径

no_grad：
关闭一段代码中的梯度记录
```

### 3.8.5 `torch.inference_mode()`

推理阶段可以使用

```python
model.eval()

with torch.inference_mode():
    outputs = model(inputs)
```

`inference_mode()` 与 `no_grad()` 类似，会关闭反向梯度记录，但它更加严格，还会关闭部分视图跟踪和版本计数相关开销，因此在纯推理场景可能更高效。代价是其内部创建的 inference tensor 不能像普通 `no_grad` 输出那样随意重新参与后续需要 Autograd 记录的计算。

### 3.8.6 inference tensor 的限制

下面的思路可能存在问题

```python
with torch.inference_mode():
    feature = model_a(inputs)

# 后续尝试把 feature 用于需要梯度的训练计算
output = model_b(feature)
loss = output.sum()
loss.backward()
```

如果 `feature` 是 inference tensor，后续需要保存它用于反向传播的操作可能报错。

需要跨出 inference mode 后参与训练时，可以创建普通副本

```python
with torch.inference_mode():
    feature = model_a(inputs)

feature = feature.clone()
output = model_b(feature)
```

如果只是暂时关闭梯度，但输出稍后还要作为普通常量参与可求导计算，`no_grad()` 通常更灵活。

### 3.8.7 `model.eval()` 与关闭梯度不是一回事

这是面试高频混淆点。

```python
model.eval()
```

只切换模块的训练状态，主要影响

- Dropout
- BatchNorm
- 其他依赖 `training` 状态的模块

它不会自动关闭 Autograd。`nn.Module.train()` 和 `eval()` 只影响特定模块的行为。

因此验证阶段通常同时使用

```python
model.eval()

with torch.no_grad():
    outputs = model(inputs)
```

或

```python
model.eval()

with torch.inference_mode():
    outputs = model(inputs)
```

对应职责

```text
model.eval()
切换模型层行为

no_grad / inference_mode
控制是否记录梯度计算
```

### 3.8.8 三者对比

|   对比项   |     `detach()`     |  `no_grad()`  | `inference_mode()` |
| :-----: | :----------------: | :-----------: | :----------------: |
|  作用范围   |     单个 Tensor      |     代码区域      |        代码区域        |
| 是否记录反向图 | 从该 Tensor 起不记录原图依赖 |    区域内不记录     |       区域内不记录       |
| 是否共享原数据 |        通常是         |      不适用      |        不适用         |
| 后续参与训练  |     可作为普通常量使用      | 输出通常可作为普通常量使用 |       有更严格限制       |
|  主要场景   |       局部阻断梯度       |    验证、辅助计算    |        纯推理         |
| 性能优化程度  |      不是全局推理模式      |      较高       |       通常更激进        |

### 3.8.9 选择原则

```text
只希望某个中间 Tensor 不再向前传播梯度：
detach()

一段计算暂时不需要梯度，但结果可能稍后参与训练：
no_grad()

确定整个区域是纯推理，结果不会重新进入 Autograd：
inference_mode()
```

## 3.9 `retain_graph` 与 `create_graph`

这两个参数名字相似，但目的不同

```text
retain_graph：
保留当前反向图，以便再次使用

create_graph：
为本次梯度计算继续建立图，以便计算高阶梯度
```

### 3.9.1 为什么默认不能对同一张图反向两次

```python
x = torch.tensor(
    2.0,
    requires_grad=True,
)

y = x ** 2

y.backward()
# y.backward()
```

第一次 backward 后，PyTorch 默认释放反向所需的已保存中间信息。

第二次继续使用同一张图时，就可能报错。

这种设计可以降低训练过程中不必要的内存占用。

### 3.9.2 `retain_graph=True`

确实需要对同一张图执行多次反向时

```python
x = torch.tensor(
    2.0,
    requires_grad=True,
)

y = x ** 2

y.backward(retain_graph=True)
print(x.grad)  # tensor(4.)

y.backward()
print(x.grad)  # tensor(8.)
```

第一次反向保留图，第二次可以继续使用。

`retain_graph=False` 时，反向所用图会被释放；设为 `True` 可保留。官方文档同时强调，绝大多数场景并不需要手动设为 `True`，通常存在更高效的替代方式。

### 3.9.3 多个 loss 的处理

假设

```python
feature = model(inputs)

loss1 = criterion1(feature, target1)
loss2 = criterion2(feature, target2)
```

更简单的方式通常是

```python
total_loss = loss1 + loss2
total_loss.backward()
```

如果必须分别 backward

```python
loss1.backward(retain_graph=True)
loss2.backward()
```

因为两个 loss 共享同一部分前向图。

不应在所有 `backward()` 中无条件使用

```python
retain_graph=True
```

否则会延长中间 Tensor 生命周期，增加显存占用，并可能掩盖代码中错误持有计算图的问题。

### 3.9.4 `create_graph=True`

默认情况下，反向计算得到的梯度通常不会继续构建一张可供再次求导的图。

如果需要二阶或更高阶导数，应使用

```python
create_graph=True
```

例如

```python
x = torch.tensor(
    2.0,
    requires_grad=True,
)

y = x ** 3

first_derivative = torch.autograd.grad(
    outputs=y,
    inputs=x,
    create_graph=True,
)[0]

second_derivative = torch.autograd.grad(
    outputs=first_derivative,
    inputs=x,
)[0]

print(first_derivative)   # tensor(12., grad_fn=...)
print(second_derivative)  # tensor(12.)
```

数学上

```text
y = x³

dy/dx = 3x²
d²y/dx² = 6x
```

当 `x = 2`

```text
dy/dx = 12
d²y/dx² = 12
```

`create_graph=True` 使梯度结果本身仍然可微，可用于高阶导数。

### 3.9.5 `retain_graph` 与 `create_graph` 的核心区别

**`retain_graph=True`**

关注的是

```text
旧图能否再次使用
```

常见目的

- 对同一前向图执行多次 backward
- 分别处理共享图的多个 loss

**`create_graph=True`**

关注的是

```text
梯度计算本身是否建立新图
```

常见目的

- 二阶导数
- Hessian 相关计算
- 梯度惩罚
- 元学习
- 物理信息神经网络

### 3.9.6 二者之间的默认关系

`backward()` 中

```python
retain_graph=None
```

时，是否保留图会根据 `create_graph` 推断。对于普通一阶反向，通常不保留；当要求创建高阶导数图时，相关图需要保留。

但从代码可读性和内存控制角度，不应依赖模糊理解，应明确知道自己是

- 需要再次使用原图
- 还是需要对梯度继续求导

### 3.9.7 常见误区

**误区一：报显存不足就设置 `retain_graph=True`**

正好相反。保留计算图通常会增加显存占用。

**误区二：每次 backward 都设置 `retain_graph=True` 更安全**

这会掩盖计算图生命周期问题，并增加内存压力。

**误区三：`retain_graph=True` 可以计算二阶梯度**

只保留旧图不等于梯度本身可导。高阶导数关键在于

```python
create_graph=True
```

**误区四：`create_graph=True` 只是保存更多中间结果**

它的本质是让梯度计算也被 Autograd 记录，从而形成新的可微计算图。

## 3.10 自定义 Autograd Function

### 3.10.1 为什么需要自定义 Function

如果前向过程完全由 PyTorch 内置操作组成，通常不需要自定义反向传播

```python
def my_function(x):
    return x ** 3 + 2 * x
```

Autograd 可以自动追踪这些操作。

自定义 `torch.autograd.Function` 主要用于

1. 包装 PyTorch 无法自动求导的外部计算
2. 接入自定义 C++ 或 CUDA 算子
3. 手动设计更节省内存的反向实现
4. 定义非标准梯度
5. 融合多个操作并控制保存的中间值

`torch.autograd.Function` 允许开发者定义自定义前向和反向公式。

### 3.10.2 自定义三次方函数

实现

```text
y = x³
```

其梯度为

```text
dy/dx = 3x²
```

代码：

```python
import torch


class CubicFunction(torch.autograd.Function):
    @staticmethod
    def forward(ctx, input_tensor):
        """
        前向计算 y = x^3。
        """
        # 保存 backward 需要使用的 Tensor
        ctx.save_for_backward(input_tensor)

        return input_tensor ** 3

    @staticmethod
    def backward(ctx, grad_output):
        """
        根据上游梯度计算 loss 对输入的梯度。
        """
        # 取回 forward 保存的 Tensor
        (input_tensor,) = ctx.saved_tensors

        # 链式法则：
        # dL/dx = dL/dy * dy/dx
        #       = grad_output * 3x^2
        grad_input = grad_output * 3 * input_tensor ** 2

        return grad_input


def cubic(input_tensor):
    return CubicFunction.apply(input_tensor)


x = torch.tensor(
    2.0,
    requires_grad=True,
)

y = cubic(x)
y.backward()

print(y)       # tensor(8., grad_fn=<CubicFunctionBackward>)
print(x.grad)  # tensor(12.)
```

### 3.10.3 为什么通过 `.apply()` 调用

不要直接实例化并调用

```python
# function = CubicFunction()
# y = function.forward(...)
```

标准入口是

```python
y = CubicFunction.apply(x)
```

`.apply()` 会负责

- 调用自定义 forward
- 建立对应 Autograd 节点
- 记录输入输出关系
- 在反向时调用自定义 backward

通常会在外部封装一层普通函数

```python
def cubic(x):
    return CubicFunction.apply(x)
```

这样上层模型不需要关心实现细节。

### 3.10.4 `ctx` 的作用

`ctx` 是 forward 和 backward 之间的上下文对象。

可以使用

```python
ctx.save_for_backward(tensor1, tensor2)
```

保存反向传播需要的 Tensor。

在 backward 中

```python
tensor1, tensor2 = ctx.saved_tensors
```

取出。

PyTorch 建议使用 `save_for_backward()` 保存反向所需 Tensor，而不是直接将 Tensor 长期挂在 `ctx` 上，这有助于正确管理生命周期、释放中间值并兼容 saved tensor hooks。

普通非 Tensor 信息可以保存为

```python
ctx.scale = scale
```

### 3.10.5 `grad_output` 是什么

假设自定义操作

```text
y = f(x)
```

模型最终损失为

```text
L = g(y)
```

backward 接收到的

```python
grad_output
```

表示

```text
∂L/∂y
```

自定义 backward 需要返回

```text
∂L/∂x
=
∂L/∂y × ∂y/∂x
```

因此，backward 不应该只返回局部导数

```text
∂y/∂x
```

而应乘以上游梯度。

对于三次方

```text
局部导数：
∂y/∂x = 3x²

完整输入梯度：
∂L/∂x = grad_output × 3x²
```

### 3.10.6 backward 返回值数量

forward 有几个输入，backward 通常就要返回几个对应梯度。

```python
class ScaledMultiply(torch.autograd.Function):
    @staticmethod
    def forward(ctx, x, y, scale):
        ctx.save_for_backward(x, y)
        ctx.scale = scale

        return scale * x * y

    @staticmethod
    def backward(ctx, grad_output):
        x, y = ctx.saved_tensors
        scale = ctx.scale

        grad_x = grad_output * scale * y
        grad_y = grad_output * scale * x

        # scale 是普通 Python 数值，不对其求梯度
        grad_scale = None

        return grad_x, grad_y, grad_scale
```

对应关系

```text
forward 输入：
x, y, scale

backward 返回：
grad_x, grad_y, grad_scale
```

不需要或不能求梯度的输入返回 `None`。

### 3.10.7 使用 `gradcheck` 验证反向实现

手写梯度公式很容易出错，应使用

```python
torch.autograd.gradcheck()
```

验证。

```python
x = torch.randn(
    4,
    dtype=torch.double,
    requires_grad=True,
)

passed = torch.autograd.gradcheck(
    CubicFunction.apply,
    (x,),
)

print(passed)  # True
```

`gradcheck` 会将自定义 backward 的解析梯度与数值近似梯度进行比较。由于数值差分对精度敏感，通常使用双精度输入。PyTorch 官方也建议使用 `gradcheck()` 验证自定义 backward 的正确性。

### 3.10.8 自定义 Function 的常见错误

**错误一：忘记乘 `grad_output`**

错误

```python
return 3 * x ** 2
```

正确

```python
return grad_output * 3 * x ** 2
```

**错误二：backward 返回数量与 forward 输入不一致**

forward 有三个输入，backward 就需要返回三个位置对应的结果，包括必要的 `None`。

**错误三：在 `ctx` 上随意保存大量 Tensor**

可能延长 Tensor 生命周期，导致内存无法及时释放。

**错误四：没有执行 `gradcheck`**

前向结果正确，不代表反向公式正确。

**错误五：已有 PyTorch 操作可以自动求导，却重复手写 backward**

如果只是组合现有 PyTorch 算子，优先写普通 Python 函数或 `nn.Module`。这样代码更简单，也更容易支持高阶导数、编译和其他自动变换。

### 3.10.9 自定义 Function 与自定义 Module 的区别

|       对比项       | `nn.Module` | `autograd.Function` |
| :-------------: | :---------: | :-----------------: |
|      主要作用       |  组织模型结构和参数  |    定义一个操作的前向与反向     |
|     是否管理参数      |     可以      |      不直接负责参数注册      |
| 是否通常手写 backward |     不需要     |         需要          |
|      调用方式       | `module(x)` | `Function.apply(x)` |
|      典型场景       |   自定义网络层    |      自定义算子或梯度       |

例如，一个自定义网络层通常写成

```python
class MyLayer(torch.nn.Module):
    def forward(self, x):
        return x ** 3
```

只有确实需要控制反向公式时，才使用

```python
torch.autograd.Function
```

## 3.11 本章小节

|         概念          |           核心含义            |
| :-----------------: | :-----------------------: |
|   `requires_grad`   |       是否需要追踪相关梯度计算        |
|       `grad`        |       反向传播后保存的累积梯度        |
|      `grad_fn`      |   生成当前非叶子 Tensor 的反向节点    |
|      叶子 Tensor      | 用户创建、通常作为梯度最终累积目标的 Tensor |
|     非叶子 Tensor      |     被记录操作产生的中间 Tensor     |
|    `backward()`     |        反向传播并累积叶子梯度        |
|    `zero_grad()`    |           清理旧梯度           |
|     `detach()`      |    将指定 Tensor 与当前计算图断开    |
|     `no_grad()`     |       在代码区域内关闭梯度记录        |
| `inference_mode()`  |         更严格的纯推理模式         |
|   `retain_graph`    |        保留原图以供再次反向         |
|   `create_graph`    |     为梯度计算建立新图以支持高阶导数      |
| `autograd.Function` |       自定义算子的前向与反向规则       |

完成本章后，应能够独立回答

1. 自动微分与数值微分有什么区别
2. 为什么神经网络适合反向模式自动微分
3. PyTorch 计算图在什么时候建立
4. `requires_grad`、`grad`、`grad_fn` 分别表示什么
5. 什么是叶子 Tensor 和非叶子 Tensor
6. 为什么非叶子 Tensor 默认没有 `.grad`
7. `loss.backward()` 内部做了什么
8. 非标量 Tensor 为什么需要传入 `gradient`
9. 为什么梯度默认累积
10. `zero_grad(set_to_none=True)` 有什么作用
11. `detach()`、`no_grad()`、`inference_mode()` 有什么区别
12. `model.eval()` 是否会关闭梯度
13. `retain_graph` 与 `create_graph` 有什么区别
14. 如何计算二阶导数
15. 如何实现并验证自定义 Autograd Function
16. 原地操作为什么可能破坏反向传播

# 4 `nn.Module` 与模型构建

`torch.nn.Module` 是 PyTorch 组织神经网络的基础抽象。

Tensor 和 Autograd 解决的是

```text
数据如何表示
    +
梯度如何计算
```

而 `nn.Module` 进一步解决

```text
模型包含哪些层
参数由谁管理
子模块如何嵌套
模型如何切换设备
状态如何保存和恢复
训练与推理行为如何切换
```

一个完整模型通常可以抽象为

```text
nn.Module
├── Parameters：需要训练的参数
├── Buffers：不参与优化但属于模型状态的 Tensor
├── Submodules：组成当前模型的子模块
├── forward：前向计算逻辑
└── training：当前处于训练还是评估模式
```

因此，`nn.Module` 不只是为了让代码“写得更像面向对象”，而是 PyTorch 模型管理机制的核心。

## 4.1 `nn.Module` 的设计作用

### 4.1.1 为什么不能只用普通 Python 函数

从数学上看，一个模型就是函数

```text
y = f(x; θ)
```

其中

- `x` 是输入
- `y` 是输出
- `θ` 是模型参数

确实可以用普通函数实现模型

```python
import torch


def linear(x, weight, bias):
    return x @ weight.T + bias
```

但参数一多，就需要手动管理

- 哪些 Tensor 是模型参数
- 哪些参数需要求梯度
- 哪些参数应该交给优化器
- 参数位于 CPU 还是 GPU
- 参数如何保存
- 多层模型如何组织
- 模型如何切换训练与推理状态

例如

```python
weight1 = torch.randn(32, 10, requires_grad=True)
bias1 = torch.zeros(32, requires_grad=True)

weight2 = torch.randn(2, 32, requires_grad=True)
bias2 = torch.zeros(2, requires_grad=True)
```

此时还要手动把所有参数传入优化器

```python
optimizer = torch.optim.Adam(
    [weight1, bias1, weight2, bias2],
    lr=1e-3,
)
```

模型层数增加后，这种管理方式容易出现

- 漏掉某个参数
- 参数没有移动到 GPU
- 保存 checkpoint 时遗漏状态
- 参数名称混乱
- 子结构难以复用

`nn.Module` 的作用就是统一解决这些问题。

### 4.1.2 `nn.Module` 是模型状态的容器

所有标准神经网络层，例如

- `nn.Linear`    
- `nn.Conv2d`
- `nn.LayerNorm`
- `nn.Embedding`
- `nn.TransformerEncoder`

本身都继承自 `nn.Module`。

自定义模型也应该继承它

```python
from torch import nn


class MyModel(nn.Module):
    def __init__(self):
        super().__init__()

    def forward(self, x):
        return x
```

一个 Module 可以包含其他 Module，并形成树形嵌套结构。子模块以普通属性形式赋值后会被自动注册，之后能够参与递归参数遍历、设备迁移和状态保存。

例如

```python
class Classifier(nn.Module):
    def __init__(self):
        super().__init__()

        self.backbone = nn.Sequential(
            nn.Linear(10, 32),
            nn.ReLU(),
        )

        self.head = nn.Linear(32, 2)

    def forward(self, x):
        features = self.backbone(x)
        return self.head(features)
```

对应模型树

```text
Classifier
├── backbone: Sequential
│   ├── 0: Linear
│   └── 1: ReLU
└── head: Linear
```

### 4.1.3 参数自动管理

当 `nn.Parameter` 或包含参数的子模块被赋值为 Module 属性时，PyTorch 会自动注册它们。

```python
model = Classifier()

for parameter in model.parameters():
    print(parameter.shape)
```

优化器可以直接获得整个模型中的参数

```python
optimizer = torch.optim.AdamW(
    model.parameters(),
    lr=1e-3,
)
```

开发者不需要手动维护参数列表。

### 4.1.4 递归设备迁移

调用

```python
model = model.to("cuda")
```

PyTorch 会递归移动模型中已注册的

- Parameter
- Buffer
- 子模块中的 Parameter 和 Buffer

普通未注册 Tensor 属性不会自动获得相同的状态管理能力。`Module.to()` 会递归处理模块的参数和 Buffer，并原地修改模块后返回自身。

因此，模型迁移设备不是简单地给对象添加一个 `device` 属性，而是实际迁移整个模块树中的模型状态。

### 4.1.5 统一状态保存

模型可以通过

```python
state = model.state_dict()
```

获取参数和持久化 Buffer。

保存

```python
torch.save(
    model.state_dict(),
    "model.pt",
)
```

恢复

```python
model.load_state_dict(
    torch.load(
        "model.pt",
        map_location="cpu",
    )
)
```

这使模型训练、断点恢复和部署具有统一的状态接口。

### 4.1.6 训练与推理状态切换

Module 具有

```python
model.train()
model.eval()
```

用于递归切换模型及其子模块的运行状态。

状态切换主要影响

- Dropout
- BatchNorm
- 其他依赖 `self.training` 的自定义模块

它并不会自动完成训练，也不会自动关闭梯度。

### 4.1.7 Hook 与调试扩展

`nn.Module` 还提供 Hook 机制，例如

- forward pre-hook
- forward hook
- backward hook
- state dict hook

可以用于

- 查看中间特征
- 统计激活值
- 调试梯度
- 性能分析
- 模型剪枝和量化工具
- 提取指定网络层输出

例如

```python
def print_output_shape(module, inputs, output):
    print(
        module.__class__.__name__,
        output.shape,
    )


handle = model.head.register_forward_hook(
    print_output_shape
)

outputs = model(torch.randn(8, 10))

handle.remove()
```

这也是为什么一般应调用

```python
model(inputs)
```

而不是直接调用

```python
model.forward(inputs)
```

Module 实例的调用入口会负责执行相关 Hook 和 Module 调用逻辑，直接调用 `forward()` 会绕过这些机制。

### 4.1.8 `nn.Module` 解决的问题总结

```text
普通 Python 对象
    ↓
继承 nn.Module
    ↓
获得统一的模型管理能力
```

具体包括

|  能力   |           对应接口           |
| :---: | :----------------------: |
| 参数注册  |      `nn.Parameter`      |
| 子模块注册 | Module 属性、`add_module()` |
| 参数遍历  |      `parameters()`      |
| 命名遍历  |   `named_parameters()`   |
| 状态保存  |      `state_dict()`      |
| 状态恢复  |   `load_state_dict()`    |
| 设备迁移  |          `to()`          |
| 模式切换  |    `train()`、`eval()`    |
| 梯度清理  |      `zero_grad()`       |
| 调试扩展  |           Hook           |

## 4.2 `__init__` 与 `forward` 的关系

### 4.2.1 `__init__` 负责定义模型结构

`__init__` 主要用于

- 创建神经网络层
- 注册参数
- 注册 Buffer
- 保存模型配置
- 组织子模块
- 创建与输入无关的固定状态

例如

```python
class MLP(nn.Module):
    def __init__(
        self,
        input_dim,
        hidden_dim,
        output_dim,
    ):
        super().__init__()

        self.linear1 = nn.Linear(
            input_dim,
            hidden_dim,
        )
        self.activation = nn.ReLU()
        self.linear2 = nn.Linear(
            hidden_dim,
            output_dim,
        )
```

模型初始化完成后，其基本结构和参数集合通常已经确定。

### 4.2.2 `forward` 负责定义数据流

`forward()` 描述输入如何经过模型得到输出

```python
class MLP(nn.Module):
    def __init__(
        self,
        input_dim,
        hidden_dim,
        output_dim,
    ):
        super().__init__()

        self.linear1 = nn.Linear(
            input_dim,
            hidden_dim,
        )
        self.activation = nn.ReLU()
        self.linear2 = nn.Linear(
            hidden_dim,
            output_dim,
        )

    def forward(self, x):
        x = self.linear1(x)
        x = self.activation(x)
        x = self.linear2(x)
        return x
```

可以概括为

```text
__init__：
模型拥有什么

forward：
模型如何计算
```

### 4.2.3 为什么必须先调用 `super().__init__()`

标准写法

```python
class MyModel(nn.Module):
    def __init__(self):
        super().__init__()

        self.layer = nn.Linear(10, 2)
```

父类初始化会建立 Module 内部用于管理

- 参数
- Buffer
- 子模块
- Hook
- 训练状态

的数据结构。

官方文档要求在给子类分配 Module 属性之前调用父类的 `__init__()`。

错误写法

```python
class WrongModel(nn.Module):
    def __init__(self):
        self.layer = nn.Linear(10, 2)
        super().__init__()
```

可能在注册子模块时直接报错。

### 4.2.4 为什么不要在 `forward` 中创建可训练层

错误示例

```python
class WrongModel(nn.Module):
    def forward(self, x):
        layer = nn.Linear(
            x.shape[-1],
            10,
        )
        return layer(x)
```

问题包括

1. 每次 forward 都创建新参数
2. 不同 batch 使用不同权重
3. 参数没有在优化器创建时注册
4. 参数不会正常出现在模型 `state_dict` 中
5. 设备可能和输入不一致
6. 模型无法真正学习稳定映射

更加隐蔽的错误

```python
class WrongModel(nn.Module):
    def forward(self, x):
        self.layer = nn.Linear(
            x.shape[-1],
            10,
        ).to(x.device)

        return self.layer(x)
```

虽然层被赋值给了属性，但每次 forward 都会覆盖旧层，优化器仍然只持有创建优化器时的旧参数引用。

正确写法

```python
class CorrectModel(nn.Module):
    def __init__(self, input_dim):
        super().__init__()

        self.layer = nn.Linear(
            input_dim,
            10,
        )

    def forward(self, x):
        return self.layer(x)
```

### 4.2.5 `forward` 中可以使用普通 Python 控制流

动态图允许在 `forward` 中使用

- `if`
- `for`
- `while`
- 多分支
- 多输入
- 返回字典、元组或自定义结构

例如

```python
class ConditionalModel(nn.Module):
    def __init__(self, dim):
        super().__init__()

        self.positive_branch = nn.Linear(dim, dim)
        self.negative_branch = nn.Linear(dim, dim)

    def forward(self, x):
        if x.mean() >= 0:
            return self.positive_branch(x)

        return self.negative_branch(x)
```

两个分支对应的层都已经在 `__init__` 中注册，`forward` 只负责决定本次执行哪条路径。

### 4.2.6 无参数操作可以直接写在 `forward` 中

并非所有操作都必须在 `__init__` 中定义。

下面这些写法完全合理

```python
def forward(self, x):
    x = torch.relu(x)
    x = x.flatten(start_dim=1)
    x = x.mean(dim=1)
    return x
```

因为

- `torch.relu()` 没有需要注册的参数
- `flatten()` 只是 Tensor 形状操作
- `mean()` 只是归约操作

通常遵循

```text
有参数或有运行状态的层：
放在 __init__

无参数、无状态的 Tensor 操作：
可以直接写在 forward
```

### 4.2.7 Module 与 Functional API

PyTorch 经常同时提供

```python
nn.ReLU()
```

和

```python
torch.nn.functional.relu(x)
```

对于无参数激活函数，两者都可以使用

```python
class ModelA(nn.Module):
    def __init__(self):
        super().__init__()
        self.activation = nn.ReLU()

    def forward(self, x):
        return self.activation(x)
```

```python
import torch.nn.functional as F


class ModelB(nn.Module):
    def forward(self, x):
        return F.relu(x)
```

区别主要在于

- Module 形式会出现在模型结构中
- Module 更便于注册 Hook
- Functional 形式更适合简单、无状态操作
- 对有参数操作，必须显式提供相应权重

例如

```python
F.linear(x, weight, bias)
```

不会自动替开发者注册 `weight` 和 `bias`。

## 4.3 `Parameter` 与普通 Tensor 的区别

### 4.3.1 `Parameter` 是特殊的 Tensor

`nn.Parameter` 是 Tensor 的子类。

```python
import torch
from torch import nn

parameter = nn.Parameter(
    torch.randn(3, 4)
)

print(isinstance(parameter, torch.Tensor))
# True

print(parameter.requires_grad)
# True
```

Parameter 的特殊之处不主要在计算能力，而在于：当 Parameter 被赋值为 Module 属性时，会自动注册为模型参数，默认梯度学习。

普通 Tensor 不会自动注册，默认梯度不学习。

### 4.3.2 Parameter 的自动注册

```python
class CustomLinear(nn.Module):
    def __init__(
        self,
        input_dim,
        output_dim,
    ):
        super().__init__()

        self.weight = nn.Parameter(
            torch.randn(
                output_dim,
                input_dim,
            )
        )

        self.bias = nn.Parameter(
            torch.zeros(output_dim)
        )

    def forward(self, x):
        return x @ self.weight.T + self.bias
```

查看参数

```python
model = CustomLinear(10, 2)

for name, parameter in model.named_parameters():
    print(name, parameter.shape)
```

输出

```text
weight [2, 10]
bias   [2]
```

它们会自动

- 出现在 `parameters()` 中
- 出现在 `state_dict()` 中
- 被 `model.to(device)` 移动
- 可以交给优化器更新

### 4.3.3 普通 Tensor 属性不会成为参数

```python
class Example(nn.Module):
    def __init__(self):
        super().__init__()

        self.parameter = nn.Parameter(
            torch.ones(3)
        )

        self.tensor = torch.ones(3)
```

检查

```python
model = Example()

print(
    list(model.named_parameters())
)
```

只会看到

```text
parameter
```

不会看到普通 Tensor 属性 `tensor`。

这样设计是必要的，因为模型中可能保存临时 Tensor，例如

- 缓存
- 最近一次隐状态
- 调试信息
- 临时统计结果

如果所有 Tensor 属性都被视为参数，优化器会错误地更新这些临时状态。

### 4.3.4 Parameter 不等于一定会更新

一个 Parameter 是否更新，还取决于

1. `requires_grad` 是否为 `True`
2. 是否参与当前 loss 的计算
3. 是否成功得到梯度
4. 是否被交给优化器
5. 是否执行了 `optimizer.step()`
6. 优化器是否跳过该参数

例如，冻结参数

```python
for parameter in model.backbone.parameters():
    parameter.requires_grad_(False)
```

这些对象仍然是 Parameter，也仍会出现在

```python
model.parameters()
model.state_dict()
```

但不会通过普通反向传播获得梯度。

### 4.3.5 只把可训练参数传给优化器

冻结模型后可以筛选

```python
trainable_parameters = [
    parameter
    for parameter in model.parameters()
    if parameter.requires_grad
]

optimizer = torch.optim.AdamW(
    trainable_parameters,
    lr=1e-3,
)
```

但需要注意：如果之后重新将某些参数解冻，优化器不会自动发现新参数。

例如

```python
parameter.requires_grad_(True)
```

只是重新启用梯度，并不会自动把它加入已经创建好的 optimizer。

此时可能需要

- 重新创建优化器
- 或使用 `optimizer.add_param_group()`

### 4.3.6 Parameter 通常是叶子 Tensor

模型参数通常由用户直接创建，并由优化器直接更新，所以一般是叶子 Tensor

```python
layer = nn.Linear(4, 2)

print(layer.weight.is_leaf)
# True

print(layer.weight.grad_fn)
# None
```

反向传播后，梯度累积到

```python
layer.weight.grad
layer.bias.grad
```

优化器读取这些 `.grad` 并更新参数。

### 4.3.7 不要使用 `.data` 绕过 Autograd

过去有时可以看到

```python
parameter.data -= learning_rate * parameter.grad
```

这种写法会绕过 Autograd 的安全检查，可能破坏版本计数和计算图一致性。

更安全的手动更新方式是

```python
with torch.no_grad():
    parameter -= learning_rate * parameter.grad
```

实际训练中，应优先使用优化器

```python
optimizer.step()
```

### 4.3.8 `nn.ParameterList` 与 `nn.ParameterDict`

如果参数数量是动态配置的，不能简单放进普通 Python list

```python
self.weights = [
    nn.Parameter(torch.randn(10, 10))
    for _ in range(4)
]
```

这些 Parameter 不会因为位于普通 list 中就自动被 Module 遍历。

应使用

```python
self.weights = nn.ParameterList([
    nn.Parameter(torch.randn(10, 10))
    for _ in range(4)
])
```

字典形式可使用

```python
self.weights = nn.ParameterDict({
    "left": nn.Parameter(
        torch.randn(10, 10)
    ),
    "right": nn.Parameter(
        torch.randn(10, 10)
    ),
})
```

`ParameterDict` 中的参数会被正确注册，并可以被 Module 的参数遍历接口发现。

### 4.3.9 `nn.Parameter` 与普通 Tensor 有什么区别？

**参考回答**

Parameter 是 Tensor 的子类，两者都能执行 Tensor 运算。

核心区别是，当 Parameter 被赋值给 `nn.Module` 属性时，它会自动注册为模型参数，出现在 `parameters()` 和 `state_dict()` 中，并能被优化器管理。普通 Tensor 属性不会自动注册为参数。

Parameter 默认通常需要梯度，但它是否真正更新还取决于是否参与计算、是否产生梯度以及是否被优化器管理。

**面试官追问**

- Parameter 一定会被优化器更新吗？
- 冻结参数后它还是 Parameter 吗？
- 为什么普通 Tensor 不自动注册为参数？
- Parameter 为什么通常是叶子 Tensor？
- 为什么不建议使用 `.data` 更新参数？

**容易答错的地方**

不要回答：Parameter 可以求梯度，Tensor 不能。

普通浮点 Tensor 同样可以设置 `requires_grad=True`。核心差异是 Module 的注册语义。

## 4.4 `parameters`、`named_parameters`、`modules`

### 4.4.1 `parameters()`

`model.parameters()` 返回模型中已注册 Parameter 的迭代器，默认递归遍历子模块。

```python
model = Classifier()

for parameter in model.parameters():
    print(parameter.shape)
```

最常见用途是创建优化器

```python
optimizer = torch.optim.AdamW(
    model.parameters(),
    lr=1e-3,
)
```

优化器需要一个参数迭代器，而 `model.parameters()` 正好提供整个模块树中的参数。

### 4.4.2 `named_parameters()`

返回

```text
参数名称 + Parameter
```

示例

```python
for name, parameter in model.named_parameters():
    print(
        name,
        parameter.shape,
        parameter.requires_grad,
    )
```

可能输出

```text
backbone.0.weight  [32, 10] True
backbone.0.bias    [32]     True
head.weight        [2, 32]  True
head.bias          [2]      True
```

参数名称体现模块层级

```text
backbone.0.weight
│        │ │
│        │ └── 参数名
│        └──── 子模块索引
└──────────── 顶层子模块名
```

常见用途

- 按名称冻结参数
- 设置不同学习率
- 打印模型参数
- 检查参数是否注册
- 构建参数分组

### 4.4.3 按名称冻结参数

```python
for name, parameter in model.named_parameters():
    if name.startswith("backbone"):
        parameter.requires_grad_(False)
```

检查

```python
for name, parameter in model.named_parameters():
    print(name, parameter.requires_grad)
```

这种方式适用于

- 冻结 backbone
- 只训练分类头
- LoRA 或 Adapter 微调
- 分层学习率

### 4.4.4 优化器参数分组

例如 backbone 使用较小学习率，head 使用较大学习率

```python
optimizer = torch.optim.AdamW(
    [
        {
            "params": model.backbone.parameters(),
            "lr": 1e-4,
        },
        {
            "params": model.head.parameters(),
            "lr": 1e-3,
        },
    ],
    weight_decay=1e-2,
)
```

也可以使用 `named_parameters()` 排除 bias 和归一化参数的 weight decay，但应根据具体模型和训练方案设计，不能机械套用。

### 4.4.5 `modules()`

`model.modules()` 递归返回

- 模型自身
- 所有层级的子模块

```python
for module in model.modules():
    print(module.__class__.__name__)
```

可能输出

```text
Classifier
Sequential
Linear
ReLU
Linear
```

常见用途

- 查找指定类型层
- 修改模块配置
- 自定义初始化
- 注册 Hook
- 模型分析

例如初始化所有 Linear

```python
for module in model.modules():
    if isinstance(module, nn.Linear):
        nn.init.xavier_uniform_(
            module.weight
        )

        if module.bias is not None:
            nn.init.zeros_(module.bias)
```

### 4.4.6 `named_modules()`

返回

```text
模块名称 + Module
```

```python
for name, module in model.named_modules():
    print(
        name,
        module.__class__.__name__,
    )
```

顶层模型的名称通常是空字符串

```text
""            Classifier
"backbone"    Sequential
"backbone.0"  Linear
"backbone.1"  ReLU
"head"        Linear
```

它适合

- 定位特定层
- 按名称替换模块
- 提取中间层
- 注册指定 Hook

### 4.4.7 `children()` 与 `modules()` 的区别

`children()` 只返回直接子模块

```python
for module in model.children():
    print(module)
```

而 `modules()` 会递归返回所有层级，并包含模型自身。

```text
model.children()
├── backbone
└── head
```

```text
model.modules()
├── model 自身
├── backbone
├── backbone.0
├── backbone.1
└── head
```

### 4.4.8 普通 list 不会注册子模块

错误写法

```python
class WrongModel(nn.Module):
    def __init__(self):
        super().__init__()

        self.layers = [
            nn.Linear(10, 10)
            for _ in range(4)
        ]

    def forward(self, x):
        for layer in self.layers:
            x = layer(x)

        return x
```

这些 Linear 可以被 Python 调用，但不会被 Module 注册机制正确管理。

结果可能包括

- `model.parameters()` 找不到这些参数
- `model.to("cuda")` 不会移动它们
- `state_dict()` 不会保存它们
- 优化器不会更新它们

正确写法

```python
class CorrectModel(nn.Module):
    def __init__(self):
        super().__init__()

        self.layers = nn.ModuleList([
            nn.Linear(10, 10)
            for _ in range(4)
        ])

    def forward(self, x):
        for layer in self.layers:
            x = layer(x)

        return x
```

`ModuleList` 可以像普通 list 一样索引和迭代，但内部 Module 会被正确注册。

### 4.4.9 `ModuleList` 与 `Sequential` 的区别

**`ModuleList`**

只负责注册和保存子模块，不定义固定前向流程

```python
self.layers = nn.ModuleList([...])

for layer in self.layers:
    x = layer(x)
```

适合

- 动态循环
- 跳跃连接
- 条件执行
- 多分支
- 需要访问中间结果

**`Sequential`**

按照保存顺序自动依次调用

```python
self.network = nn.Sequential(
    nn.Linear(10, 32),
    nn.ReLU(),
    nn.Linear(32, 2),
)
```

前向

```python
x = self.network(x)
```

`Sequential` 将模块串成固定流水线，而 `ModuleList` 只注册模块，具体如何调用仍由 `forward()` 决定。

## 4.5 `state_dict` 的作用

### 4.5.1 什么是 `state_dict`

`state_dict` 是一个从名称映射到状态 Tensor 的字典结构。

对于模型，它主要包含

- 已注册 Parameter
- 持久化 Buffer

官方教程将其描述为保存各层参数 Tensor 的 Python 字典；Module 的持久化 Buffer 也会被包含在状态字典中。

示例

```python
model = Classifier()

for name, tensor in model.state_dict().items():
    print(name, tensor.shape)
```

可能输出

```text
backbone.0.weight [32, 10]
backbone.0.bias   [32]
head.weight       [2, 32]
head.bias         [2]
```

如果模型包含 BatchNorm，还可能看到

```text
running_mean
running_var
num_batches_tracked
```

### 4.5.2 `state_dict` 不等于完整模型

`state_dict()` 通常不保存

- Python 类定义
- `forward()` 代码
- 模型构造参数
- 数据预处理逻辑
- 训练配置
- 评价指标
- 普通未注册 Tensor
- `persistent=False` 的 Buffer

因此，加载前必须先创建结构兼容的模型

```python
model = Classifier()
model.load_state_dict(state)
```

不能只依赖参数文件自动推断所有模型结构。

### 4.5.3 保存模型参数

标准方式

```python
torch.save(
    model.state_dict(),
    "classifier.pt",
)
```

加载

```python
model = Classifier()

state = torch.load(
    "classifier.pt",
    map_location="cpu",
)

model.load_state_dict(state)
```

推理前

```python
model.eval()
```

需要注意

```text
load_state_dict()
负责恢复状态

eval()
负责切换模型模式
```

加载参数不会自动调用 `eval()`。

### 4.5.4 `strict=True`

默认

```python
model.load_state_dict(
    state,
    strict=True,
)
```

要求 checkpoint 中的 key 与当前模型 `state_dict()` 的 key 匹配。

如果不匹配，可能出现

- missing keys
- unexpected keys

例如，当前模型新增了分类头

```text
missing key:
classifier.weight
classifier.bias
```

checkpoint 包含当前模型不存在的旧模块

```text
unexpected key:
old_head.weight
```

### 4.5.5 `strict=False`

迁移学习或部分加载时可以使用

```python
result = model.load_state_dict(
    state,
    strict=False,
)

print(result.missing_keys)
print(result.unexpected_keys)
```

但 `strict=False` 不表示任何不匹配都能忽略。

例如参数名称相同但 shape 不同

```text
checkpoint：
head.weight [1000, 768]

当前模型：
head.weight [10, 768]
```

通常仍然会产生尺寸不匹配错误。

因此，`strict=False` 主要放宽 key 的缺失和多余，不是自动解决所有参数兼容问题。

### 4.5.6 迁移学习中排除分类头

```python
state = torch.load(
    "pretrained.pt",
    map_location="cpu",
)

filtered_state = {
    name: tensor
    for name, tensor in state.items()
    if not name.startswith("classifier.")
}

result = model.load_state_dict(
    filtered_state,
    strict=False,
)

print("Missing:", result.missing_keys)
print("Unexpected:", result.unexpected_keys)
```

这种方式适合

- backbone 预训练
- 修改类别数
- 替换任务头
- 部分模块复用

### 4.5.7 完整训练 Checkpoint

只保存模型状态不能完整恢复训练。

标准 checkpoint 通常包含

```python
checkpoint = {
    "epoch": epoch,
    "global_step": global_step,
    "model": model.state_dict(),
    "optimizer": optimizer.state_dict(),
    "scheduler": scheduler.state_dict(),
    "best_metric": best_metric,
    "config": config,
}

torch.save(
    checkpoint,
    "checkpoint.pt",
)
```

恢复

```python
checkpoint = torch.load(
    "checkpoint.pt",
    map_location="cpu",
)

model.load_state_dict(
    checkpoint["model"]
)

optimizer.load_state_dict(
    checkpoint["optimizer"]
)

scheduler.load_state_dict(
    checkpoint["scheduler"]
)

start_epoch = checkpoint["epoch"] + 1
global_step = checkpoint["global_step"]
```

优化器状态可能包含

- Momentum
- Adam 一阶矩
- Adam 二阶矩
- 参数组配置

如果只恢复模型权重，而不恢复优化器状态，训练轨迹可能发生明显变化。

### 4.5.8 保存最佳模型时的引用问题

训练过程中可以保存到文件

```python
if validation_score > best_score:
    best_score = validation_score

    torch.save(
        model.state_dict(),
        "best_model.pt",
    )
```

如果只在内存中写

```python
best_state = model.state_dict()
```

后续训练可能继续修改底层参数对象。为了得到真正独立快照，应使用深拷贝

```python
import copy

best_state = copy.deepcopy(
    model.state_dict()
)
```

### 4.5.9 DDP 模型的 `module.` 前缀

某些封装后的模型状态名称可能出现

```text
module.backbone.weight
```

而未封装模型期望

```text
backbone.weight
```

更规范的保存方式通常是保存原始内部模型

```python
torch.save(
    ddp_model.module.state_dict(),
    "model.pt",
)
```

而不是依赖加载时批量修改 key。

### 4.5.10 `state_dict()` 保存了什么？

**参考回答**

模型的 `state_dict()` 保存已注册参数和持久化 Buffer，并以分层名称映射到对应 Tensor。

它不包含模型类定义和 `forward()` 代码，因此加载时需要先创建结构兼容的模型，再调用 `load_state_dict()`。

如果要恢复完整训练，还应额外保存优化器、学习率调度器、epoch、global step 和相关配置。

**面试官追问**

- Buffer 是否在 `state_dict` 中？
- `strict=False` 能解决 shape 不匹配吗？
- 为什么保存模型后还要保存 optimizer？
- `state_dict()` 是否包含模型结构？
- 如何加载预训练 backbone 并忽略分类头？

**容易答错的地方**

不要认为：`state_dict` 只保存可训练参数。持久化 Buffer 也属于模型状态，会被保存。

## 4.6 `register_parameter` 与 `register_buffer`

### 4.6.1 为什么需要显式注册接口

多数情况下，通过属性赋值就能自动注册 Parameter

```python
self.weight = nn.Parameter(
    torch.randn(10, 10)
)
```

但在以下场景中，可能需要显式调用

```python
self.register_parameter(
    "weight",
    parameter,
)
```

例如

- 根据配置动态注册参数
- 可选参数
- 自定义参数容器
- 框架级模型构造
- 需要程序化命名

### 4.6.2 `register_parameter`

示例

```python
class OptionalBiasLinear(nn.Module):
    def __init__(
        self,
        input_dim,
        output_dim,
        use_bias=True,
    ):
        super().__init__()

        self.weight = nn.Parameter(
            torch.empty(
                output_dim,
                input_dim,
            )
        )

        if use_bias:
            bias = nn.Parameter(
                torch.zeros(output_dim)
            )
        else:
            bias = None

        self.register_parameter(
            "bias",
            bias,
        )

        nn.init.xavier_uniform_(
            self.weight
        )

    def forward(self, x):
        return nn.functional.linear(
            x,
            self.weight,
            self.bias,
        )
```

Parameter 被注册后

- 可以通过属性访问
- 会出现在参数遍历中
- 会参与设备迁移
- 非空参数会出现在 `state_dict` 中

### 4.6.3 什么是 Buffer

Buffer 是：属于模型状态、需要跟随模型迁移，但不应被优化器视为可训练参数的 Tensor。

典型例子

- BatchNorm 的运行均值
- BatchNorm 的运行方差
- 固定位置编码
- 固定 mask
- 统计计数器
- 量化参数
- 指数滑动平均状态

注册方式

```python
self.register_buffer(
    "position_ids",
    torch.arange(max_length),
)
```

Buffer 默认是持久化的，会被保存到 `state_dict`；设置 `persistent=False` 后仍是 Buffer，但不会写入 `state_dict`。

### 4.6.4 为什么不能直接使用普通 Tensor 属性

```python
class WrongModel(nn.Module):
    def __init__(self):
        super().__init__()

        self.position_ids = torch.arange(128)
```

调用

```python
model.to("cuda")
```

普通 Tensor 属性不会自动像注册 Buffer 一样迁移。

之后可能出现

```text
输入：CUDA Tensor
position_ids：CPU Tensor
```

从而引发 device mismatch。

正确写法

```python
class CorrectModel(nn.Module):
    def __init__(self):
        super().__init__()

        self.register_buffer(
            "position_ids",
            torch.arange(128),
        )
```

此后

```python
model.to("cuda")
```

会同步移动该 Buffer。

### 4.6.5 `persistent=False`

有些状态

- 可以根据配置重新生成
- 不希望增加 checkpoint 大小
- 只在运行过程中缓存
- 每次加载都应重新构建

可以注册为非持久化 Buffer

```python
self.register_buffer(
    "causal_mask",
    mask,
    persistent=False,
)
```

它仍然会

- 跟随 `model.to(device)`
- 被识别为 Buffer
- 通过 `named_buffers()` 遍历

但不会出现在 `state_dict()` 中。

### 4.6.6 Parameter、Buffer 与普通 Tensor 对比

|     属性类型     | 优化器管理 | 随 `to()` 迁移 | 默认进入 `state_dict` |  常见用途  |
| :----------: | :---: | :---------: | :---------------: | :----: |
|  Parameter   |   是   |      是      |         是         | 可训练权重  |
|  持久化 Buffer  |   否   |      是      |         是         | 模型运行状态 |
| 非持久化 Buffer  |   否   |      是      |         否         | 可重建缓存  |
| 普通 Tensor 属性 |   否   |      否      |         否         | 临时普通数据 |

### 4.6.7 Buffer 是否可以 `requires_grad=True`

技术上，Buffer 仍然是 Tensor，可以具有 `requires_grad=True`。

但从设计语义上，如果某个 Tensor 需要被优化器作为模型权重更新，更适合将其定义为 Parameter。

否则可能出现

- 能得到梯度
- 但不在 `model.parameters()` 中
- 优化器默认找不到它

因此

```text
需要训练和优化：
Parameter

不参与优化但属于模型状态：
Buffer
```

### 4.6.8 查看 Buffer

```python
for name, buffer in model.named_buffers():
    print(
        name,
        buffer.shape,
        buffer.device,
    )
```

这对于排查以下问题非常有用

- 为什么状态没有移动到 GPU
- checkpoint 中为什么多出某些 key
- BatchNorm 状态是否正确
- 自定义 mask 是否被注册

## 4.7 `train` 与 `eval` 的区别

### 4.7.1 `train()` 做了什么

调用

```python
model.train()
```

等价于

```python
model.train(True)
```

它会递归地将模型及子模块的

```python
module.training
```

设置为 `True`。

### 4.7.2 `eval()` 做了什么

调用

```python
model.eval()
```

等价于

```python
model.train(False)
```

它会递归将

```python
module.training
```

设置为 `False`。

这些模式只会影响特定模块，例如 Dropout 和 BatchNorm，并不会改变所有层的计算。

### 4.7.3 Dropout 的行为

训练时，Dropout 以概率 `p` 随机将部分元素置零，并对保留元素进行相应缩放。

```python
dropout = nn.Dropout(p=0.5)

x = torch.ones(10)

dropout.train()
print(dropout(x))
```

输出中可能出现部分零值。

评估模式下，标准 Dropout 相当于恒等映射

```python
dropout.eval()
print(dropout(x))
```

输出与输入相同。

### 4.7.4 BatchNorm 的行为

BatchNorm 训练时通常

1. 使用当前 batch 的均值和方差归一化
2. 更新 running mean 和 running variance

评估时通常

1. 不再使用当前 batch 统计值
2. 使用训练期间积累的运行统计量

PyTorch 的 BatchNorm 默认会在训练阶段更新运行均值与方差，并在评估阶段使用这些统计量。

因此，如果验证阶段忘记调用

```python
model.eval()
```

可能导致

- 使用验证 batch 的统计值
- 继续修改运行统计量
- 验证结果依赖 batch size
- 评估结果波动

### 4.7.5 LayerNorm 的行为

LayerNorm 根据当前样本指定维度的统计量进行归一化，在训练和评估模式下都使用输入自身的统计量，因此其核心归一化行为不会像 BatchNorm 那样依赖运行均值与方差。

所以

```text
train/eval 对 BatchNorm 影响明显
train/eval 对 LayerNorm 核心归一化逻辑通常没有影响
```

但整个模型仍应正确切换模式，因为其中可能还存在 Dropout 等其他层。

### 4.7.6 `eval()` 不会关闭梯度

```python
model.eval()

outputs = model(inputs)
loss = outputs.sum()
loss.backward()
```

只要梯度模式开启、参数需要梯度，这段代码仍然可以建立计算图并执行反向传播。

因此验证阶段一般写成

```python
model.eval()

with torch.no_grad():
    for inputs, targets in validation_loader:
        outputs = model(inputs)
```

或

```python
model.eval()

with torch.inference_mode():
    for inputs, targets in validation_loader:
        outputs = model(inputs)
```

职责分工

```text
model.eval()
控制 Dropout、BatchNorm 等模块行为

no_grad / inference_mode
控制是否记录反向计算图
```

### 4.7.7 `train()` 不会开始训练

```python
model.train()
```

不会自动

- 读取训练数据
- 执行 forward
- 计算 loss
- 执行 backward
- 更新参数

它只是切换 Module 的运行状态。

真正训练需要完整流程

```python
model.train()

for inputs, targets in train_loader:
    optimizer.zero_grad(
        set_to_none=True
    )

    outputs = model(inputs)
    loss = criterion(
        outputs,
        targets,
    )

    loss.backward()
    optimizer.step()
```

### 4.7.8 自定义模块使用 `self.training`

自定义模块也可以根据模式改变行为

```python
class NoiseLayer(nn.Module):
    def __init__(self, noise_std=0.1):
        super().__init__()
        self.noise_std = noise_std

    def forward(self, x):
        if self.training:
            noise = torch.randn_like(x)
            x = x + self.noise_std * noise

        return x
```

调用

```python
layer.train()
```

会添加噪声。

调用

```python
layer.eval()
```

则不会添加噪声。

### 4.7.9 验证后要恢复训练模式

训练过程中进行验证

```python
for epoch in range(num_epochs):
    model.train()
    train_one_epoch(...)

    model.eval()

    with torch.no_grad():
        validate(...)
```

下一轮开始时需要重新调用

```python
model.train()
```

否则模型会继续保持评估状态，导致

- Dropout 被关闭
- BatchNorm 不更新统计量
- 训练行为发生变化

## 4.8 `nn.Linear`

### 4.8.1 核心作用

Linear 实现仿射变换

```text
y = xWᵀ + b
```

定义

```python
layer = nn.Linear(
    in_features=10,
    out_features=32,
    bias=True,
)
```

其参数通常为

```text
weight.shape = [out_features, in_features]
bias.shape   = [out_features]
```

`nn.Linear` 对输入最后一个维度进行变换，其他前导维度保持不变。

### 4.8.2 输入输出形状

```python
layer = nn.Linear(10, 32)
```

二维输入

```text
输入：[B, 10]
输出：[B, 32]
```

三维输入

```text
输入：[B, T, 10]
输出：[B, T, 32]
```

代码

```python
x = torch.randn(8, 20, 10)
y = layer(x)

print(y.shape)
# torch.Size([8, 20, 32])
```

Linear 不要求输入必须是二维，它只要求最后一维等于 `in_features`。

### 4.8.3 常见误区

错误

```python
x = torch.randn(8, 10, 20)
layer = nn.Linear(10, 32)

# layer(x)
```

Linear 会检查最后一维 `20`，而不是中间维度 `10`。

如果希望处理维度 `10`，需要先调整维度顺序，或者改变 Linear 配置。

## 4.9 卷积层

### 4.9.1 `nn.Conv1d`

常用于

- 一维时间序列 
- 音频
- 序列局部特征
- 生物医学信号

标准输入

```text
[N, C_in, L]
```

输出

```text
[N, C_out, L_out]
```

PyTorch `Conv1d` 按 batch、channel、length 的布局处理输入。

示例

```python
conv = nn.Conv1d(
    in_channels=16,
    out_channels=32,
    kernel_size=3,
    stride=1,
    padding=1,
)

x = torch.randn(8, 16, 100)
y = conv(x)

print(y.shape)
# [8, 32, 100]
```

### 4.9.2 `nn.Conv2d`

常用于

- 图像
- 特征图
- 二维时频图
- 空间局部建模

标准输入

```text
[N, C_in, H, W]
```

输出

```text
[N, C_out, H_out, W_out]
```

示例

```python
conv = nn.Conv2d(
    in_channels=3,
    out_channels=64,
    kernel_size=3,
    stride=2,
    padding=1,
)

x = torch.randn(8, 3, 224, 224)
y = conv(x)

print(y.shape)
# [8, 64, 112, 112]
```

卷积输出尺寸一般可按下式分析

```text
L_out
=
floor(
    (L_in + 2P - D(K - 1) - 1) / S
    + 1
)
```

其中

- `K`：kernel size
- `S`：stride
- `P`：padding
- `D`：dilation

### 4.9.3 `groups`

`groups` 控制输入和输出通道之间的连接方式。

```text
groups = 1：
普通卷积

groups > 1：
分组卷积

groups = in_channels：
深度卷积的一种常见配置
```

使用分组卷积时，必须满足输入和输出通道数与 `groups` 的整除约束。

### 4.9.4 卷积常见误区

**输入布局错误**

错误

```text
[N, L, C]
```

而 Conv1d 期望

```text
[N, C, L]
```

需要

```python
x = x.transpose(1, 2)
```

**`padding=1` 不一定保持尺寸**

只有在特定条件下，例如

```text
kernel_size = 3
stride = 1
dilation = 1
padding = 1
```

才会保持长度不变。

## 4.10 归一化层

### 4.10.1 BatchNorm

在同一个 batch 内，对每个 channel 的所有样本和空间位置计算均值方差进行归一化。

常见输入

```text
BatchNorm1d：
[N, C] 或 [N, C, L]

BatchNorm2d：
[N, C, H, W]
```

典型用法

```python
norm = nn.BatchNorm2d(64)

x = torch.randn(8, 64, 32, 32)
y = norm(x)
```

特点

- 依赖 batch 统计
- 维护运行均值和方差
- 训练和评估行为不同
- batch 很小时统计可能不稳定

### 4.10.2 LayerNorm

对单个样本中的若干特征维度计算均值方差进行归一化，不依赖 batch。

LayerNorm 对输入最后若干维度进行归一化

```python
norm = nn.LayerNorm(768)

x = torch.randn(8, 128, 768)
y = norm(x)
```

这里每个 token 的最后一维 `768` 被独立归一化。

LayerNorm 的 `normalized_shape` 指定最后若干维，统计量由当前输入计算，训练和评估阶段都使用输入自身统计。

常用于

- Transformer
- NLP
- LLM
- 不适合依赖 batch 统计的任务

### 4.10.3 GroupNorm

将 channel 划分成多个 group，在每个样本内部对每个 group 的 channel 和空间维度计算均值方差。

GroupNorm 将通道划分为若干组，在每个样本内部完成归一化

```python
norm = nn.GroupNorm(
    num_groups=8,
    num_channels=64,
)
```

常见于

- 检测和分割
- 小 batch 图像训练
- batch size 波动较大的任务

### 4.10.4 归一化方法对比

|    方法     |      主要统计范围       | 是否依赖 batch |      典型场景       |
| :-------: | :---------------: | :--------: | :-------------: |
| BatchNorm | batch 与空间维度上的每个通道 |     是      |   CNN、大 batch   |
| LayerNorm |    每个样本的最后若干维     |     否      | Transformer、LLM |
| GroupNorm |     每个样本的通道分组     |     否      |  小 batch 视觉模型   |

不能简单地认为某种归一化始终更好，应根据

- 输入布局
- batch size
- 网络结构
- 任务类型

选择。

## 4.11 Dropout

定义

```python
dropout = nn.Dropout(p=0.1)
```

训练时

```text
每个元素以概率 p 被置零
```

未被置零的元素会进行缩放，使输出期望保持稳定。

评估时

```text
Dropout(x) = x
```

标准 Dropout 训练时随机置零，评估时为恒等映射。

### 4.11.1 Dropout 的设计动机

如果神经元总是以固定组合共同工作，模型可能过度依赖特定特征组合。

Dropout 训练时随机屏蔽部分激活，相当于向网络加入结构噪声，使模型不容易依赖单一路径。

但 Dropout 并不是所有任务中都必须使用

- 概率过大会降低模型容量
- 卷积网络中使用方式可能与 MLP 不同
- 大模型中的 Dropout 比例取决于训练规模和配置
- 数据量充足时未必需要很强 Dropout

### 4.11.2 `Dropout` 与 `Dropout2d`

`nn.Dropout` 通常逐元素随机置零。

`nn.Dropout2d` 更偏向按通道随机置零，常用于卷积特征图。官方文档说明其通常接收 Conv2d 相关输入。

## 4.12 `nn.Embedding`

### 4.12.1 核心作用

Embedding 可以理解为可训练查找表

```text
embedding_table.shape
=
[num_embeddings, embedding_dim]
```

定义

```python
embedding = nn.Embedding(
    num_embeddings=10000,
    embedding_dim=256,
)
```

输入是整数索引

```python
token_ids = torch.tensor([
    [10, 25, 37],
    [5, 8, 9],
])

vectors = embedding(token_ids)

print(vectors.shape)
# [2, 3, 256]
```

输入 shape 中每个整数索引都会查找对应向量，输出在原 shape 后追加 `embedding_dim`。`nn.Embedding` 维护大小为词表数乘嵌入维度的查找表。

### 4.12.2 Embedding 等价于什么

概念上

```python
vectors = embedding.weight[token_ids]
```

但 `nn.Embedding` 进一步提供

- 参数注册
- `padding_idx`
- 稀疏梯度选项
- 最大范数约束
- 频率相关梯度缩放

### 4.12.3 `padding_idx`

```python
embedding = nn.Embedding(
    num_embeddings=10000,
    embedding_dim=256,
    padding_idx=0,
)
```

索引 `0` 对应 padding token。

默认情况下，padding 行通常初始化为零，并且该行不会按普通方式在训练中更新。

### 4.12.4 输入 dtype

Embedding 输入必须是整数索引 Tensor，通常使用

```python
torch.long
```

错误

```python
token_ids = torch.tensor([
    1.0,
    2.0,
])
```

正确

```python
token_ids = torch.tensor(
    [1, 2],
    dtype=torch.long,
)
```

### 4.12.5 Embedding 与 Linear 的区别

Linear

```text
输入：
连续特征向量

操作：
矩阵乘法
```

Embedding

```text
输入：
离散整数 ID

操作：
按行查表
```

虽然 one-hot 向量乘 Embedding 矩阵在数学上可以得到相同结果，但显式 one-hot 会浪费大量内存和计算。

## 4.13 Hook 与调试扩展

### 4.13.1 Hook 是什么

Hook 可以理解为：在模型执行到某个特定阶段时，自动调用的一段回调函数。

它允许开发者在不修改原模型 `forward()` 主体代码的情况下，观察或干预模型执行过程。

典型执行位置包括

```text
模块执行前
    ↓
Forward Pre-Hook
    ↓
module.forward()
    ↓
Forward Hook
    ↓
反向传播
    ↓
Backward Hook
```

Hook 常用于

- 打印中间 Tensor 的 shape
- 提取中间层特征
- 统计激活值分布
- 检查 NaN、Inf
- 查看梯度大小
- 分析梯度消失和梯度爆炸
- 临时修改模块输入、输出或梯度
- 实现可视化、剪枝、量化和性能分析工具

PyTorch 的 Module Hook 主要包括

|               Hook                |       调用时机       |   常见用途    |
| :-------------------------------: | :--------------: | :-------: |
|    `register_forward_pre_hook`    | `forward()` 执行之前 |  检查或修改输入  |
|      `register_forward_hook`      | `forward()` 执行之后 |  查看或修改输出  |
| `register_full_backward_pre_hook` |     模块反向计算前      | 检查或修改输出梯度 |
|   `register_full_backward_hook`   |     模块反向计算期间     | 查看输入、输出梯度 |
|      `Tensor.register_hook`       | 某个 Tensor 的梯度产生时 | 观察或修改指定梯度 |

Module Hook 的注册函数会返回一个 `RemovableHandle`，调用 `handle.remove()` 可以移除对应 Hook。

### 4.13.2 为什么需要 Hook

最直接的调试方法是在 `forward()` 中插入打印代码

```python
def forward(self, x):
    print("Input:", x.shape)

    x = self.encoder(x)

    print("Encoder output:", x.shape)

    x = self.classifier(x)

    return x
```

这种方式在小模型中可行，但在复杂模型中存在明显问题

- 需要直接修改模型源代码
- 调试代码与业务逻辑混在一起
- 难以同时观察大量模块
- 第三方预训练模型不方便修改
- 调试结束后需要手动删除代码
- 容易遗漏某些执行分支

Hook 将观察逻辑从模型实现中分离

```text
模型代码：
负责正常计算

Hook：
负责观察、记录或临时干预
```

因此，Hook 更适合临时调试、实验分析和工具化扩展。

### 4.13.3 Forward Pre-Hook

Forward Pre-Hook 在模块的 `forward()` 执行前调用。

注册方式

```python
handle = module.register_forward_pre_hook(
    hook_function
)
```

默认函数签名

```python
def hook_function(module, args):
    ...
```

其中

- `module`：当前执行的模块
- `args`：传递给该模块的位置参数，以 tuple 形式组织

如果注册时设置

```python
with_kwargs=True
```

则 Hook 还会收到关键字参数

```python
def hook_function(module, args, kwargs):
    ...
```

Forward Pre-Hook 可以只观察输入，也可以返回修改后的输入。

**示例：检查输入形状**

```python
import torch
from torch import nn


class SimpleModel(nn.Module):
    def __init__(self) -> None:
        super().__init__()

        self.linear = nn.Linear(
            in_features=10,
            out_features=4,
        )

    def forward(
        self,
        x: torch.Tensor,
    ) -> torch.Tensor:
        return self.linear(x)


model = SimpleModel()


def check_input_shape(
    module: nn.Module,
    args: tuple[torch.Tensor, ...],
) -> None:
    x = args[0]

    print(
        f"{module.__class__.__name__} "
        f"input shape: {tuple(x.shape)}"
    )

    if x.shape[-1] != 10:
        raise ValueError(
            "Expected the last input dimension "
            f"to be 10, but received {x.shape[-1]}."
        )


handle = model.linear.register_forward_pre_hook(
    check_input_shape
)

inputs = torch.randn(8, 10)
outputs = model(inputs)

handle.remove()
```

输出

```text
Linear input shape: (8, 10)
```

这种方式适合检查

- 输入维度 
- dtype
- device
- batch size
- 是否存在 NaN 或 Inf

### 4.13.4 Forward Hook

Forward Hook 在模块的 `forward()` 已经计算出输出后调用。

注册方式

```python
handle = module.register_forward_hook(
    hook_function
)
```

默认函数签名

```python
def hook_function(module, args, output):
    ...
```

如果设置

```python
with_kwargs=True
```

函数签名为

```python
def hook_function(
    module,
    args,
    kwargs,
    output,
):
    ...
```

Forward Hook 可以查看输出，也可以返回新的输出替换原输出。由于它在 `forward()` 之后调用，此时再原地修改输入不会影响已经完成的前向计算。

### 4.13.5 示例：提取中间特征

Forward Hook 常用于提取预训练模型的中间表示。

```python
features: dict[str, torch.Tensor] = {}


def save_features(
    name: str,
):
    def hook(
        module: nn.Module,
        args: tuple[torch.Tensor, ...],
        output: torch.Tensor,
    ) -> None:
        features[name] = output.detach()

    return hook
```

注册

```python
handle = model.network[0].register_forward_hook(
    save_features("hidden")
)

outputs = model(torch.randn(4, 16))

print(features["hidden"].shape)
# torch.Size([4, 32])

handle.remove()
```

这里使用

```python
output.detach()
```

是为了避免特征字典长期持有完整计算图。

如果将带有 `grad_fn` 的中间输出直接保存

```python
features[name] = output
```

那么这个 Python 容器可能继续引用其计算图和前向中间结果，导致内存或显存无法及时释放。

如果还需要一份完全独立、不会被后续原地修改影响的数据，可以使用

```python
features[name] = output.detach().clone()
```

如果要保存到 CPU

```python
features[name] = output.detach().cpu()
```

但 `.cpu()` 会产生设备间数据传输，频繁执行可能降低训练性能。

### 4.13.6 示例：检查激活值中的 NaN 和 Inf

```python
def check_activation(
    module: nn.Module,
    args: tuple[torch.Tensor, ...],
    output: torch.Tensor,
) -> None:
    if not isinstance(output, torch.Tensor):
        return

    if torch.isnan(output).any():
        raise RuntimeError(
            f"NaN detected in "
            f"{module.__class__.__name__}"
        )

    if torch.isinf(output).any():
        raise RuntimeError(
            f"Inf detected in "
            f"{module.__class__.__name__}"
        )
```

递归注册

```python
handles = []

for module in model.modules():
    if len(list(module.children())) == 0:
        handles.append(
            module.register_forward_hook(
                check_activation
            )
        )
```

这里仅对叶子模块注册 Hook，否则既会在外层容器上触发，也会在内部层上触发，日志可能大量重复。

不过，`output` 不一定总是单个 Tensor，也可能

- tuple
- list
- dict
- 自定义输出对象

更加通用的调试工具需要递归检查输出结构，而不能假设所有模块都只返回一个 Tensor。

### 4.13.7 `always_call` 的作用

Forward Hook 默认只在模块成功完成前向计算后触发。

注册时可以设置

```python
handle = module.register_forward_hook(
    hook_function,
    always_call=True,
)
```

这样即使 `forward()` 抛出异常，Hook 也会被调用。该选项可用于错误诊断和资源清理。

但如果 `forward()` 没有正常生成输出，Hook 中必须考虑

```python
output
```

可能不存在正常的 Tensor 结果。

### 4.13.8 Full Backward Hook

Module 的完整反向 Hook 使用

```python
handle = module.register_full_backward_hook(
    hook_function
)
```

函数签名

```python
def hook_function(
    module,
    grad_input,
    grad_output,
):
    ...
```

其中

- `grad_input`：损失相对于该模块输入的梯度
- `grad_output`：损失相对于该模块输出的梯度
- 二者都以 tuple 形式提供

完整反向 Hook 通常在模块输入梯度被计算时触发；如果输入都不需要梯度，则可能在输出梯度计算时触发；如果模块输出也不需要梯度，则 Hook 不会触发。反向 Hook 不允许原地修改其输入或输出梯度，但可以返回新的 `grad_input`。

### 4.13.9 Tensor Hook

除了给整个 Module 注册 Hook，还可以直接给 Tensor 注册梯度 Hook

```python
handle = tensor.register_hook(
    hook_function
)
```

函数签名

```python
def hook_function(grad):
    ...
```

它会在该 Tensor 的梯度被计算时触发。

Hook 不应原地修改传入的 `grad`，但可以返回一个新的 Tensor，作为替换后的梯度继续参与反向传播。

### 4.13.10 示例：查看指定 Tensor 的梯度

```python
x = torch.tensor(
    [1.0, 2.0, 3.0],
    requires_grad=True,
)


def print_gradient(
    gradient: torch.Tensor,
) -> None:
    print("Gradient:", gradient)


handle = x.register_hook(
    print_gradient
)

loss = (x ** 2).sum()
loss.backward()

handle.remove()
```

输出

```text
Gradient: tensor([2., 4., 6.])
```

Tensor Hook 特别适合

- 查看非参数 Tensor 的梯度
- 调试中间特征梯度
- 对比多个分支的梯度
- 临时实现梯度裁剪或梯度缩放
- 检查某个 Tensor 是否参与反向传播

### 4.13.11 Module Backward Hook 与 Tensor Hook 的区别

| 对比项  |    Module Backward Hook    |   Tensor Hook   |
| :--: | :------------------------: | :-------------: |
| 注册对象 |        `nn.Module`         |     Tensor      |
| 观察范围 |         模块输入、输出梯度          |  指定 Tensor 的梯度  |
| 参数形式 | `grad_input`、`grad_output` |     `grad`      |
| 常见用途 |          分析某一层梯度流          |   精确观察或修改某个梯度   |
| 返回值  |      可替换 `grad_input`      | 可替换该 Tensor 的梯度 |

选择原则

```text
想分析某一层整体的反向行为：
register_full_backward_hook

想观察某个具体 Tensor 的梯度：
tensor.register_hook
```

### 4.13.12 使用 Context Manager 自动移除 Hook

忘记移除 Hook 是常见问题。

可以使用上下文管理器统一管理

```python
from contextlib import contextmanager
from collections.abc import Iterator

from torch.utils.hooks import RemovableHandle


@contextmanager
def managed_hooks(
    handles: list[RemovableHandle],
) -> Iterator[None]:
    try:
        yield
    finally:
        for handle in handles:
            handle.remove()
```

使用

```python
handles = register_shape_hooks(model)

with managed_hooks(handles):
    outputs = model(inputs)
```

无论模型执行是否成功，退出代码区域后都会尝试移除 Hook。

更规范的设计是让注册逻辑本身位于 Context Manager 内部，避免 Hook 在异常发生后继续残留。

### 4.13.13 State Dict Hook

除前向和反向 Hook 外，Module 还支持围绕状态保存和加载的 Hook，例如

- `register_state_dict_pre_hook`
- `register_state_dict_post_hook`
- `register_load_state_dict_pre_hook`
- `register_load_state_dict_post_hook`

它们可以用于

- 保存前修改状态字典
- 重命名旧 checkpoint 中的 key
- 兼容模型结构升级
- 检查 missing keys 和 unexpected keys
- 为历史 checkpoint 添加迁移逻辑

例如，加载后查看不兼容 key

```python
def report_incompatible_keys(
    module: nn.Module,
    incompatible_keys,
) -> None:
    if incompatible_keys.missing_keys:
        print(
            "Missing keys:",
            incompatible_keys.missing_keys,
        )

    if incompatible_keys.unexpected_keys:
        print(
            "Unexpected keys:",
            incompatible_keys.unexpected_keys,
        )


handle = model.register_load_state_dict_post_hook(
    report_incompatible_keys
)
```

状态字典 Hook 可以修改保存结果或影响加载时的不兼容 key 检查，因此应谨慎使用，避免隐藏真实的 checkpoint 兼容性问题。

### 4.13.14 全局 Hook

PyTorch 还支持对所有 Module 注册全局 Hook，例如全局 Forward Hook。

这种 Hook 会影响程序中全部 Module，官方将其定位为调试和性能分析用途，并明确指出它会给 `nn.Module` 系统增加全局状态。

因此，普通项目代码中更推荐

```python
specific_module.register_forward_hook(...)
```

而不是使用全局 Hook。

全局 Hook 适合

- 框架级调试    
- 全模型性能追踪
- 临时分析所有模块调用
- 构建统一诊断工具

但必须特别注意

- 及时移除
- 避免影响其他模型
- 避免在并发任务中产生副作用
- 不要作为普通业务逻辑的一部分

### 4.13.15 Hook 的执行顺序

同一个模块可以注册多个 Hook。

注册时可以使用

```python
prepend=True
```

让当前 Hook 在已有同类 Hook 之前执行

```python
handle = module.register_forward_hook(
    hook_function,
    prepend=True,
)
```

默认

```python
prepend=False
```

新 Hook 通常在已有同类 Hook 之后执行。全局 Module Hook 与具体模块 Hook 之间也有固定顺序，例如全局 Forward Hook 会先于模块自身注册的 Forward Hook。

如果多个 Hook 会修改输出或梯度，执行顺序会直接影响最终结果，因此不应依赖不清晰的隐式顺序。

### 4.13.16 为什么调用 `model.forward()` 可能导致 Hook 不执行

标准调用方式

```python
outputs = model(inputs)
```

它实际进入 Module 的调用包装逻辑，再由该逻辑调用

```python
model.forward(inputs)
```

Module 调用包装负责处理

- Forward Pre-Hook
- `forward()`
- Forward Hook
- Backward Hook 相关准备
- 其他框架扩展逻辑

如果直接调用

```python
outputs = model.forward(inputs)
```

会绕过 Module 的标准调用入口，因此注册在 Module 上的 Hook 可能不会正常触发。PyTorch 官方也建议调用 Module 实例，而不是直接调用其 `forward()`。

因此应始终优先使用

```python
model(inputs)
```

### 4.13.17 Hook 的性能开销

Hook 会在模型执行路径中增加额外的 Python 回调。

如果 Hook 中执行

- `.item()`
- `.cpu()`
- `.numpy()`
- 大量打印
- 保存完整激活
- 复杂统计
- 文件写入

可能显著降低训练速度。

特别是在 GPU 训练中

```python
gradient.norm().item()
```

可能要求 CPU 获取 GPU 计算结果，从而产生同步。

因此，调试 Hook 应遵循

```text
只在必要步骤启用
只观察必要模块
控制日志频率
避免每步保存完整激活
调试完成及时移除
```

例如

```python
if global_step % 100 == 0:
    ...
```

而不是每个 batch 都执行复杂统计。

### 4.13.18 Hook 与计算图、显存泄漏

下面的写法风险较高

```python
saved_outputs = []


def save_output(module, args, output):
    saved_outputs.append(output)
```

`output` 可能包含 `grad_fn`，它会继续引用前向计算图。

随着训练不断执行

```text
第 1 个 batch 的图
第 2 个 batch 的图
第 3 个 batch 的图
...
```

都可能被 `saved_outputs` 持有，造成显存持续增长。

更安全的方式

```python
def save_output(module, args, output):
    saved_outputs.append(
        output.detach().cpu()
    )
```

如果只需要统计值，应尽量只保存聚合结果

```python
def save_statistics(module, args, output):
    statistics.append({
        "mean": output.detach().mean().item(),
        "std": output.detach().std().item(),
    })
```

不过 `.item()` 会产生 CPU-GPU 同步，仍应控制调用频率。

### 4.13.19 Hook 中修改数据的风险

技术上，部分 Hook 可以返回修改后的

- 输入
- 输出
- 梯度。

但这会让模型真实行为分散到多个位置

```text
forward() 中的计算
       +
Hook 中隐藏的修改
       =
最终模型行为
```

可能导致

- 阅读 `forward()` 无法理解模型
- 保存模型后无法自动保存 Hook 逻辑
- 训练和推理环境行为不一致
- `torch.compile`、导出和部署兼容性变差
- 调试代码意外进入正式训练
- 多个 Hook 顺序影响结果

因此建议

```text
调试 Hook：
尽量只读

长期模型逻辑：
写入 Module 或 forward

确需修改梯度：
封装为明确、可测试的训练组件
```

### 4.13.20 常见误区

**误区一：Hook 会自动永久保存到模型中**

Hook 是运行时注册的 Python 回调，通常不会作为模型 `state_dict` 的一部分保存。

重新创建或加载模型后，需要重新注册 Hook。

**误区二：注册 Hook 后不需要移除**

Hook 会持续存在于 Module 上。

如果在循环中重复注册但不移除

```python
for epoch in range(num_epochs):
    module.register_forward_hook(hook)
```

同一个 Hook 可能被重复调用越来越多次。

正确方式是保存并移除 handle

```python
handle = module.register_forward_hook(hook)

try:
    outputs = model(inputs)
finally:
    handle.remove()
```

**误区三：Backward Hook 一定会执行**

如果模块输入和输出都不需要梯度，反向 Hook 不会触发。完整反向 Hook 的具体触发位置还取决于输入是否需要梯度。

**误区四：可以随意原地修改 Hook 参数**

反向 Hook 中不允许对输入或输出梯度进行原地修改。需要改变梯度时，应返回新的梯度对象。

**误区五：直接调用 `forward()` 与 `model(x)` 完全相同**

数值计算可能相同，但直接调用 `forward()` 会绕过 Module 调用入口和相关 Hook 机制。

**误区六：保存中间输出不会影响显存**

如果保存的输出仍连接计算图，计算图和中间激活可能无法及时释放。调试时应根据需求使用

```python
output.detach()
```

或

```python
output.detach().cpu()
```

## 4.14 自定义模型的标准写法

### 4.14.1 一个规范的残差 MLP

下面给出一个可直接运行的标准示例。

```python
from __future__ import annotations

import torch
from torch import nn


class ResidualMLPBlock(nn.Module):
    """A pre-normalization residual MLP block."""

    def __init__(
        self,
        d_model: int,
        expansion_ratio: int = 4,
        dropout: float = 0.1,
    ) -> None:
        super().__init__()

        if d_model <= 0:
            raise ValueError(
                "d_model must be positive"
            )

        if expansion_ratio <= 0:
            raise ValueError(
                "expansion_ratio must be positive"
            )

        if not 0.0 <= dropout < 1.0:
            raise ValueError(
                "dropout must be in [0, 1)"
            )

        hidden_dim = (
            d_model * expansion_ratio
        )

        self.norm = nn.LayerNorm(d_model)

        self.mlp = nn.Sequential(
            nn.Linear(
                d_model,
                hidden_dim,
            ),
            nn.GELU(),
            nn.Dropout(dropout),
            nn.Linear(
                hidden_dim,
                d_model,
            ),
            nn.Dropout(dropout),
        )

    def forward(
        self,
        x: torch.Tensor,
    ) -> torch.Tensor:
        """
        Args:
            x: Tensor of shape [B, D].

        Returns:
            Tensor of shape [B, D].
        """
        residual = x
        x = self.norm(x)
        x = self.mlp(x)

        return residual + x


class MLPClassifier(nn.Module):
    """Residual MLP classifier for vector inputs."""

    def __init__(
        self,
        input_dim: int,
        d_model: int,
        num_classes: int,
        num_layers: int = 3,
        dropout: float = 0.1,
    ) -> None:
        super().__init__()

        if input_dim <= 0:
            raise ValueError(
                "input_dim must be positive"
            )

        if num_classes <= 1:
            raise ValueError(
                "num_classes must be greater than 1"
            )

        if num_layers < 0:
            raise ValueError(
                "num_layers cannot be negative"
            )

        self.input_dim = input_dim
        self.num_classes = num_classes

        self.input_projection = nn.Linear(
            input_dim,
            d_model,
        )

        self.blocks = nn.ModuleList([
            ResidualMLPBlock(
                d_model=d_model,
                expansion_ratio=4,
                dropout=dropout,
            )
            for _ in range(num_layers)
        ])

        self.output_norm = nn.LayerNorm(
            d_model
        )

        self.classifier = nn.Linear(
            d_model,
            num_classes,
        )

    def forward_features(
        self,
        x: torch.Tensor,
    ) -> torch.Tensor:
        """
        Extract hidden features.

        Args:
            x: Tensor of shape [B, input_dim].

        Returns:
            Tensor of shape [B, d_model].
        """
        if x.ndim != 2:
            raise ValueError(
                "Expected input shape [B, D], "
                f"but received {tuple(x.shape)}"
            )

        if x.shape[-1] != self.input_dim:
            raise ValueError(
                f"Expected feature dimension "
                f"{self.input_dim}, "
                f"but received {x.shape[-1]}"
            )

        x = self.input_projection(x)

        for block in self.blocks:
            x = block(x)

        return self.output_norm(x)

    def forward(
        self,
        x: torch.Tensor,
    ) -> torch.Tensor:
        """
        Args:
            x: Tensor of shape [B, input_dim].

        Returns:
            Logits of shape [B, num_classes].
        """
        features = self.forward_features(x)
        logits = self.classifier(features)

        return logits
```

### 4.14.2 逐步解释

**第一步：继承 `nn.Module`**

```python
class MLPClassifier(nn.Module):
```

使模型获得

- 参数管理
- 子模块注册
- 状态保存
- 设备迁移
- 训练模式切换

**第二步：调用父类初始化**

```python
super().__init__()
```

必须在注册子模块前执行。

**第三步：在 `__init__` 中创建所有可训练层**

```python
self.input_projection = nn.Linear(...)
self.blocks = nn.ModuleList(...)
self.output_norm = nn.LayerNorm(...)
self.classifier = nn.Linear(...)
```

这些模块会自动注册。

**第四步：使用 `ModuleList` 保存重复模块**

```python
self.blocks = nn.ModuleList([...])
```

保证每个残差块内部参数都能被

- 优化器发现
- 移动到 GPU
- 保存到 checkpoint

**第五步：将特征提取与任务输出分开**

```python
forward_features()
forward()
```

这样便于

- 提取 Embedding
- 替换分类头
- 迁移学习
- 调试中间特征
- 多任务学习

**第六步：`forward` 只描述数据流**

```python
features = self.forward_features(x)
logits = self.classifier(features)
```

没有在 `forward` 中创建新参数，也没有写死设备。

### 4.14.3 模型基本测试

```python
def main() -> None:
    device = torch.device(
        "cuda"
        if torch.cuda.is_available()
        else "cpu"
    )

    model = MLPClassifier(
        input_dim=128,
        d_model=256,
        num_classes=10,
        num_layers=4,
        dropout=0.1,
    ).to(device)

    inputs = torch.randn(
        32,
        128,
        device=device,
    )

    logits = model(inputs)

    assert logits.shape == (32, 10)

    print(model)
    print("Output shape:", logits.shape)


if __name__ == "__main__":
    main()
```

模型实现后，至少应该检查

- 输入 shape
- 输出 shape
- dtype
- device
- 参数是否注册
- forward 是否能运行
- backward 是否正常

### 4.14.4 检查参数是否注册

```python
for name, parameter in model.named_parameters():
    print(
        name,
        tuple(parameter.shape),
        parameter.requires_grad,
    )
```

统计参数量

```python
total_parameters = sum(
    parameter.numel()
    for parameter in model.parameters()
)

trainable_parameters = sum(
    parameter.numel()
    for parameter in model.parameters()
    if parameter.requires_grad
)

print("Total:", total_parameters)
print("Trainable:", trainable_parameters)
```

### 4.14.5 测试反向传播

```python
model.train()

inputs = torch.randn(
    8,
    128,
    device=device,
)

targets = torch.randint(
    low=0,
    high=10,
    size=(8,),
    device=device,
)

criterion = nn.CrossEntropyLoss()

logits = model(inputs)
loss = criterion(logits, targets)

loss.backward()
```

检查梯度

```python
for name, parameter in model.named_parameters():
    if parameter.requires_grad:
        print(
            name,
            parameter.grad is None,
        )
```

如果某个本应参与训练的参数

```text
parameter.grad is None
```

需要检查

- 是否参与 forward
- 是否被 `detach()`
- 是否位于优化器中
- 是否在无梯度区域执行
- 是否被错误冻结

### 4.14.6 不要在模型中写死设备

错误

```python
class WrongModel(nn.Module):
    def forward(self, x):
        mask = torch.ones(
            x.shape,
            device="cuda:0",
        )

        return x * mask
```

问题

- CPU 环境无法运行
- 多卡时可能落到错误 GPU
- MPS、XPU 等其他设备无法复用

更好的方式

```python
mask = torch.ones_like(x)
```

或者

```python
mask = torch.ones(
    x.shape,
    device=x.device,
    dtype=x.dtype,
)
```

固定模型状态则应注册为 Buffer。

### 4.14.7 不要覆盖 `__call__`

一般只需要实现

```python
forward()
```

不应自行覆盖

```python
__call__()
```

因为 Module 的调用入口承担

- forward Hook
- backward Hook
- 调用包装
- 框架集成逻辑

随意覆盖可能破坏 PyTorch 机制。

### 4.14.8 合理使用 `Sequential`

简单串联结构

```python
self.encoder = nn.Sequential(
    nn.Linear(128, 256),
    nn.GELU(),
    nn.Dropout(0.1),
    nn.Linear(256, 256),
)
```

适合 `Sequential`。

存在以下逻辑时，通常编写自定义 `forward()`

- 残差连接
- 多输入
- 多输出
- 多分支
- 条件路由
- 保存中间结果
- 复杂 mask
- 跨层连接

### 4.14.9 参数初始化

可以在模型构造后统一初始化

```python
def initialize_weights(
    module: nn.Module,
) -> None:
    if isinstance(module, nn.Linear):
        nn.init.xavier_uniform_(
            module.weight
        )

        if module.bias is not None:
            nn.init.zeros_(module.bias)


model.apply(initialize_weights)
```

但不应不加分析地覆盖所有层默认初始化。

标准 PyTorch 层通常已经具有合理的默认初始化。自定义初始化需要考虑

- 激活函数
- fan-in / fan-out
- 残差结构
- 网络深度
- 预训练权重
- 任务特性

### 4.14.10 一个标准模型应具备的基本特征

```text
结构层面
├── 继承 nn.Module
├── 正确调用 super().__init__()
├── 参数和子模块在 __init__ 中注册
├── 动态模块使用 ModuleList / ModuleDict
└── forward 只描述数据流

工程层面
├── 不写死 device
├── 明确输入输出 shape
├── 支持 train / eval
├── 支持 state_dict
├── 能正确移动到 CPU / GPU
└── 能完成 forward 和 backward

可维护性
├── 模块职责清晰
├── 参数命名清晰
├── 输入检查合理
├── 避免隐藏副作用
└── 复杂模型拆分为子模块
```

## 4.15 本章小节
```text
nn.Module
│
├── __init__
│   ├── 注册 Parameter
│   ├── 注册 Buffer
│   ├── 注册子模块
│   └── 保存模型配置
│
├── forward
│   └── 定义输入到输出的数据流
│
├── parameters()
│   └── 返回模型参数
│
├── modules()
│   └── 返回模块树
│
├── state_dict()
│   └── 返回参数与持久化 Buffer
│
├── to(device)
│   └── 移动 Parameter 与 Buffer
│
└── train() / eval()
    └── 切换模块运行状态
```

状态分类

```text
模型中的 Tensor
│
├── Parameter
│   ├── 交给优化器
│   ├── 随模型迁移
│   └── 保存到 state_dict
│
├── Buffer
│   ├── 不交给优化器
│   ├── 随模型迁移
│   └── 持久化时保存到 state_dict
│
└── 普通 Tensor 属性
    ├── 不自动参与优化
    ├── 不自动随模型迁移
    └── 不自动保存
```

完成本章后，应能够独立回答

1. `nn.Module` 解决了哪些模型管理问题
2. 为什么要先调用 `super().__init__()`
3. `__init__` 和 `forward` 分别负责什么
4. 为什么不能在 `forward` 中创建可训练层
5. Parameter 与普通 Tensor 有什么区别
6. Parameter 是否一定会被优化器更新
7. `parameters()` 与 `named_parameters()` 有什么区别
8. `modules()`、`children()` 和 `named_modules()` 有什么区别
9. `ModuleList` 和普通 list 有什么区别
10. `ModuleList` 和 `Sequential` 有什么区别
11. `state_dict()` 保存哪些内容
12. `strict=False` 能处理哪些不匹配
13. Parameter、Buffer 和普通 Tensor 属性有什么区别
14. `persistent=False` 有什么作用
15. `train()` 与 `eval()` 改变了什么
16. 为什么 `eval()` 不能代替 `no_grad()`
17. Linear 对输入哪一个维度进行变换
18. Conv1d 和 Conv2d 的输入布局是什么
19. BatchNorm 和 LayerNorm 的主要区别
20. Dropout 在训练和推理阶段分别做什么
21. Embedding 的输入和输出 shape 如何变化
22. 如何实现并检查一个规范的自定义模型

# 5 损失函数与优化器

模型训练可以概括为两个核心问题

```text
损失函数：模型应该优化什么？
优化器：模型参数应该如何更新？
```

完整流程如下

```text
输入数据
   ↓
模型前向传播
   ↓
得到预测结果
   ↓
损失函数衡量预测误差
   ↓
Autograd 计算参数梯度
   ↓
优化器根据梯度更新参数
```

需要明确区分

|    组件    |      主要职责      |
| :------: | :------------: |
|    模型    |    根据输入产生预测    |
|   损失函数   | 将预测质量转换为可优化的标量 |
| Autograd |  计算损失关于参数的梯度   |
|   优化器    | 根据梯度和内部状态更新参数  |
|  学习率调度器  |  动态调整优化器的学习率   |

## 5.1 损失函数的作用

### 5.1.1 损失函数是什么

损失函数用于衡量模型预测与真实目标之间的差异。

假设模型为

```text
ŷ = f(x; θ)
```

其中

- `x` 是输入
- `θ` 是模型参数
- `ŷ` 是模型预测
- `y` 是真实目标

损失函数可以表示为

```text
L(ŷ, y)
```

训练的目标是寻找一组参数，使整体损失尽可能小

```text
θ* = arg min L(f(x; θ), y)
```

损失函数把“模型预测得好不好”转换为一个数值，并为反向传播提供优化方向。

### 5.1.2 损失函数连接任务目标与参数更新

优化器本身并不知道

- 当前是分类还是回归
- 哪个类别更重要
- 哪些位置应该忽略
- 错误预测应该受到多大惩罚

这些信息主要由损失函数定义。

例如

```text
回归任务：
预测值应该接近真实连续值

多分类任务：
正确类别的预测概率应该更高

多标签任务：
每个标签都应独立判断是否存在

度量学习：
相似样本距离应该更近，不相似样本更远
```

因此，损失函数不是训练流程中的固定模板，而是对任务目标的数学表达。

### 5.1.3 为什么损失通常需要可导

基于梯度的优化器需要计算

```text
∂L / ∂θ
```

所以损失函数及模型前向过程通常需要对参数近似处处可导。

这不意味着所有位置都必须严格可导。例如 ReLU 在零点不可导，但框架可以选定一个次梯度，使训练仍能进行。

真正重要的是：整个训练目标必须能够通过 Autograd 得到有意义的参数梯度。

对于准确率等离散指标

```text
accuracy = 正确样本数 / 总样本数
```

其值依赖 `argmax` 等离散操作，通常不适合作为直接优化目标。

所以分类训练往往使用可导的交叉熵作为 loss，同时使用 accuracy、F1 等指标评价模型。

### 5.1.4 Loss 与 Metric 的区别

|     对比项     |       Loss        |     Metric      |
| :---------: | :---------------: | :-------------: |
|    主要用途     |     反向传播和参数优化     |     评价模型效果      |
|   是否需要可导    |       通常需要        |      通常不需要      |
| 是否必须与业务指标相同 |        不一定        |    应尽量贴近任务目标    |
|    常见例子     | Cross Entropy、MSE | Accuracy、F1、AUC |

例如在类别不平衡任务中

```text
训练 Loss：
加权交叉熵

评价 Metric：
Macro-F1、Recall、AUC
```

Loss 下降不一定意味着所有业务指标都同步提升。

可能出现

- loss 下降，accuracy 不变
- accuracy 上升，少数类 Recall 下降
- 训练 loss 持续下降，验证 F1 开始下降

因此，训练时应同时监控 loss 和任务相关 metric。

### 5.1.5 样本损失与批量损失

损失函数通常先为每个样本或元素计算损失，再执行归约。

PyTorch 常见 `reduction` 参数包括：

```python
reduction="none"
reduction="mean"
reduction="sum"
```

示例

```python
import torch
from torch import nn

prediction = torch.tensor([1.0, 2.0, 3.0])
target = torch.tensor([1.5, 1.5, 2.0])

criterion = nn.MSELoss(reduction="none")
loss = criterion(prediction, target)

print(loss)
# tensor([0.2500, 0.2500, 1.0000])
```

改为均值

```python
criterion = nn.MSELoss(reduction="mean")
loss = criterion(prediction, target)

print(loss)
# tensor(0.5000)
```

`MSELoss` 的未归约形式是逐元素平方误差，默认 `reduction="mean"` 会对全部相关元素取平均。

### 5.1.6 `mean` 与 `sum` 对梯度的影响

假设 batch size 为 `B`。

使用求和

```text
L_sum = Σᵢ Lᵢ
```

使用均值

```text
L_mean = (1/B) Σᵢ Lᵢ
```

两者梯度满足

```text
∇L_sum = B × ∇L_mean
```

因此，使用 `sum` 时，梯度规模通常随 batch size 增大。

使用 `mean` 时，不同 batch size 下梯度量级通常更容易保持接近。

但具体归约范围可能不仅是 batch 维，还可能包括

- 序列长度
- 图像空间维度
- 通道维度
- 有效 token 数

在分割、语言模型和变长序列任务中，应明确损失究竟对哪些元素求平均。

### 5.1.7 自定义加权损失

使用 `reduction="none"` 可以进行自定义加权

```python
criterion = nn.CrossEntropyLoss(
    reduction="none"
)

logits = torch.randn(4, 3)
targets = torch.tensor([0, 1, 2, 1])

sample_weights = torch.tensor([
    1.0,
    2.0,
    1.0,
    0.5,
])

per_sample_loss = criterion(
    logits,
    targets,
)

loss = (
    per_sample_loss * sample_weights
).sum() / sample_weights.sum()
```

这种方式适合

- 样本重要性加权
- 难例挖掘
- 只计算有效位置
- 多任务动态权重
- 序列 mask

需要注意分母应与加权逻辑一致，否则 loss 的整体尺度可能发生非预期变化。

## 5.2 常见损失函数

不同任务需要不同损失函数。首先应根据任务输出语义进行选择，而不是只看模型最后一层输出维度。

### 5.2.1 回归损失

**MSELoss**

均方误差

```text
MSE = (1/N) Σᵢ (ŷᵢ - yᵢ)²
```

PyTorch

```python
criterion = nn.MSELoss()

prediction = torch.randn(8, 1)
target = torch.randn(8, 1)

loss = criterion(
    prediction,
    target,
)
```

特点

- 对较大误差惩罚更强
- 梯度随误差线性增大
- 对异常值较敏感
- 常用于普通连续值回归

`MSELoss` 计算输入和目标之间的逐元素平方误差。

**L1Loss**

平均绝对误差

```text
MAE = (1/N) Σᵢ |ŷᵢ - yᵢ|
```

```python
criterion = nn.L1Loss()
```

特点

- 对异常值通常比 MSE 更稳健
- 梯度幅度不会随误差无限增大
- 在零点附近不光滑
- 优化过程有时不如 MSE 平稳

**SmoothL1Loss/Huber Loss**

Smooth L1 在误差较小时使用二次形式，误差较大时使用线性形式。

直观上

```text
小误差区域：
类似 MSE，曲线平滑

大误差区域：
类似 L1，降低异常值影响
```

```python
criterion = nn.SmoothL1Loss(
    beta=1.0
)
```

常见于

- 目标检测中的边界框回归
- 含异常值的回归
- 希望兼顾平滑梯度与鲁棒性的场景

### 5.2.2 二分类损失

**BCELoss**

二元交叉熵

```text
L = -[y log(p) + (1-y) log(1-p)]
```

其中

- `y ∈ {0, 1}`
- `p` 是属于正类的概率

使用 `BCELoss` 时输入必须是概率

```python
criterion = nn.BCELoss()

probability = torch.sigmoid(logits)
loss = criterion(
    probability,
    targets,
)
```

`BCELoss` 接收概率形式的输入，并计算二元交叉熵。

**BCEWithLogitsLoss**

更推荐的标准写法

```python
criterion = nn.BCEWithLogitsLoss()

logits = model(inputs)
loss = criterion(
    logits,
    targets.float(),
)
```

它内部组合了

```text
Sigmoid
   +
Binary Cross Entropy
```

但通过更加稳定的计算方式避免显式计算极端概率。PyTorch 官方说明，`BCEWithLogitsLoss` 结合 Sigmoid 和 BCE，并利用 log-sum-exp 技巧提高数值稳定性。

因此不要写成

```python
probability = torch.sigmoid(logits)

loss = nn.BCEWithLogitsLoss()(
    probability,
    targets,
)
```

这相当于重复应用 Sigmoid 的语义，输入不再是正确的 logits。

### 5.2.3 多分类损失

**CrossEntropyLoss**

用于每个样本只属于一个类别的多分类问题。

```python
criterion = nn.CrossEntropyLoss()

logits = torch.randn(8, 5)
targets = torch.randint(
    low=0,
    high=5,
    size=(8,),
)

loss = criterion(logits, targets)
```

典型形状

```text
logits：[B, C]
target：[B]
```

其中 target 是类别索引

```text
0 ≤ target[i] < C
```

`CrossEntropyLoss` 接收未归一化 logits，可用于具有 `C` 个类别的分类问题。

**NLLLoss**

NLLLoss 接收对数概率

```python
log_probability = torch.log_softmax(
    logits,
    dim=1,
)

loss = nn.NLLLoss()(
    log_probability,
    targets,
)
```

通常

```text
CrossEntropyLoss
≈
LogSoftmax + NLLLoss
```

PyTorch 文档也说明，NLLLoss 通常配合 LogSoftmax 使用，而 CrossEntropyLoss 可以避免手动增加 LogSoftmax。

### 5.2.4 多标签分类损失

多标签分类通常使用

```python
criterion = nn.BCEWithLogitsLoss()

logits = torch.randn(8, 5)

targets = torch.randint(
    low=0,
    high=2,
    size=(8, 5),
).float()

loss = criterion(logits, targets)
```

形状

```text
logits：[B, C]
target：[B, C]
```

每个类别是独立的二元判断

```text
第 0 类：存在或不存在
第 1 类：存在或不存在
……
```

因此输出端采用逐元素 Sigmoid 语义，而不是类别间互斥的 Softmax。

### 5.2.5 KLDivLoss

KL 散度用于衡量两个概率分布之间的差异，常见于

- 知识蒸馏
- 分布匹配
- 变分模型
- 软标签训练

使用时应特别注意输入语义。常见写法是

```python
student_log_probability = torch.log_softmax(
    student_logits,
    dim=-1,
)

teacher_probability = torch.softmax(
    teacher_logits,
    dim=-1,
)

loss = nn.KLDivLoss(
    reduction="batchmean"
)(
    student_log_probability,
    teacher_probability,
)
```

KL 散度不是对称距离

```text
KL(P || Q) ≠ KL(Q || P)
```

并且输入是否为 log probability 取决于具体 API 配置，不能仅凭 shape 判断。

### 5.2.6 度量学习损失

常见形式包括

- Contrastive Loss
- Triplet Margin Loss
- Cosine Embedding Loss

例如 Triplet Loss

```python
criterion = nn.TripletMarginLoss(
    margin=1.0
)

loss = criterion(
    anchor,
    positive,
    negative,
)
```

其目标是让

```text
distance(anchor, positive)
+
margin
<
distance(anchor, negative)
```

常用于

- 人脸识别
- 检索
- Embedding 学习
- 相似度匹配

### 5.2.7 Focal Loss

Focal Loss 常用于类别不平衡或易样本占比过高的分类任务。

其核心思想是

```text
降低易分类样本的损失权重
让训练更关注难分类样本
```

PyTorch 核心 `torch.nn` 中通常不直接提供通用 Focal Loss 类，可通过 BCE 或 Cross Entropy 的未归约结果自行实现，或使用具体领域库中的实现。

简化二分类实现

```python
import torch
import torch.nn.functional as F


def binary_focal_loss(
    logits: torch.Tensor,
    targets: torch.Tensor,
    alpha: float = 0.25,
    gamma: float = 2.0,
) -> torch.Tensor:
    targets = targets.to(
        dtype=logits.dtype
    )

    bce = F.binary_cross_entropy_with_logits(
        logits,
        targets,
        reduction="none",
    )

    probability = torch.sigmoid(logits)

    p_t = (
        probability * targets
        + (1.0 - probability)
        * (1.0 - targets)
    )

    alpha_t = (
        alpha * targets
        + (1.0 - alpha)
        * (1.0 - targets)
    )

    loss = (
        alpha_t
        * (1.0 - p_t).pow(gamma)
        * bce
    )

    return loss.mean()
```

实际使用时需要确认

- alpha 的定义
- 正负样本权重
- reduction
- 多分类还是多标签
- 库实现是否包含额外变体

### 5.2.8 常见损失选择表

|      任务      |    输出语义    |         常见 Loss          |
| :----------: | :--------: | :----------------------: |
|    连续值回归     |    连续数值    |       MSE、L1、Huber       |
|     二分类      |   一个二元标签   |    BCEWithLogitsLoss     |
|    单标签多分类    | 多个互斥类别选一个  |     CrossEntropyLoss     |
|    多标签分类     | 每类独立存在或不存在 |    BCEWithLogitsLoss     |
|    类别不平衡     | 少数类或难样本重要  | 加权 CE、`pos_weight`、Focal |
|     分布匹配     |  概率分布之间差异  |        KLDivLoss         |
| Embedding 学习 |   相似样本靠近   |   Triplet、Contrastive    |

## 5.3 `CrossEntropyLoss` 的内部逻辑

### 5.3.1 CrossEntropyLoss 的标准输入

对于普通多分类

```python
logits = torch.randn(8, 5)
targets = torch.tensor([
    0, 1, 2, 3,
    4, 0, 1, 2,
])

criterion = nn.CrossEntropyLoss()
loss = criterion(logits, targets)
```

要求

```text
logits.shape = [B, C]
targets.shape = [B]
targets.dtype = torch.long
```

输入是 logits，而不是概率。

### 5.3.2 Logits 是什么

logits 是模型最后一层输出的未归一化分数。

例如

```text
logits = [2.1, -0.5, 0.7]
```

它们

- 不要求位于 `[0, 1]`
- 不要求总和为 1
- 可以为负数
- 类别间相对大小决定预测结果

概率可以通过 Softmax 得到

```python
probabilities = torch.softmax(
    logits,
    dim=-1,
)
```

### 5.3.3 单样本的计算过程

假设 logits 为

```text
z = [z₀, z₁, z₂]
```

真实类别为

```text
y = 1
```

Softmax 概率

```text
pᵢ = exp(zᵢ) / Σⱼ exp(zⱼ)
```

交叉熵

```text
L = -log(pᵧ)
```

也就是

```text
L = -log(p₁)
```

如果正确类别概率越高

```text
p₁ → 1
```

则

```text
L → 0
```

如果正确类别概率很低，loss 会变大。

### 5.3.4 为什么不要先手动 Softmax

错误写法

```python
probabilities = torch.softmax(
    logits,
    dim=1,
)

loss = nn.CrossEntropyLoss()(
    probabilities,
    targets,
)
```

CrossEntropyLoss 会把传入内容当作 logits，再执行内部的 LogSoftmax。

这会造成

- 输入语义错误
- 梯度特性变化
- 数值稳定性下降
- 模型学习可能受损

正确写法

```python
loss = nn.CrossEntropyLoss()(
    logits,
    targets,
)
```

需要概率时只在评价或推理阶段计算

```python
probabilities = torch.softmax(
    logits,
    dim=1,
)
```

### 5.3.5 为什么内部使用 LogSoftmax 更稳定

直接计算

```text
log(softmax(z))
```

可能先产生非常大或非常小的指数值。

更加稳定的形式利用

```text
log softmax(zᵢ)
=
zᵢ - log Σⱼ exp(zⱼ)
```

实现中还可以减去最大 logit

```text
m = max(z)

log Σ exp(z)
=
m + log Σ exp(z - m)
```

这样避免 `exp(z)` 在数值上溢出。

因此，CrossEntropyLoss 接收 logits 并统一计算，比用户手动拼接 Softmax 和 log 更稳健。

### 5.3.6 图像分割中的 CrossEntropyLoss

CrossEntropyLoss 不只支持 `[B, C]`。

语义分割中常见

```text
logits：[B, C, H, W]
target：[B, H, W]
```

代码

```python
logits = torch.randn(
    4,
    10,
    128,
    128,
)

targets = torch.randint(
    low=0,
    high=10,
    size=(4, 128, 128),
)

loss = nn.CrossEntropyLoss()(
    logits,
    targets,
)
```

类别维仍然位于位置 `1`

```text
[B, C, ...]
```

target 不包含类别维，每个空间位置保存一个类别索引。

### 5.3.7 `weight`：类别权重

类别不平衡时

```python
class_weights = torch.tensor([
    1.0,
    2.5,
    0.8,
])

criterion = nn.CrossEntropyLoss(
    weight=class_weights
)
```

`weight` 应为长度为 `C` 的一维 Tensor，为每个类别分配权重。PyTorch 文档明确指出，该参数适合处理不平衡训练集。

注意权重应位于与 logits 兼容的设备

```python
class_weights = class_weights.to(
    logits.device
)
```

权重过大可能造成

- 梯度波动
- 少数类过拟合
- 整体校准下降

因此不能只根据类别数量机械使用极端倒数权重。

### 5.3.8 `ignore_index`

序列标注和分割任务中，部分位置不应参与 loss，例如

- Padding token
- 无效标签
- 未标注像素

```python
criterion = nn.CrossEntropyLoss(
    ignore_index=-100
)
```

target 中值为 `ignore_index` 的位置不会参与对应 loss 和梯度归约。`ignore_index` 适用于 target 为类别索引的情形。

但应区分

```text
attention_mask：
控制 Attention 是否关注某些 token

ignore_index：
控制某些位置是否参与损失
```

二者不是同一个机制。

### 5.3.9 Label Smoothing

标准 one-hot 目标类似

```text
真实类别：1
其他类别：0
```

Label Smoothing 将其与均匀分布混合，使目标不再绝对为 0 或 1。

PyTorch 支持

```python
criterion = nn.CrossEntropyLoss(
    label_smoothing=0.1
)
```

`label_smoothing` 的取值范围为 `[0,1]`，会将原目标与均匀分布混合。

可能作用

- 降低过度自信
- 提供一定正则化
- 改善部分任务的泛化和校准

但它不是所有任务都必然有效，尤其需要考虑

- 标签本身是否含噪声
- 是否依赖精确概率校准
- 类别数量
- 蒸馏或软标签目标
- 下游决策方式

### 5.3.10 软标签目标

CrossEntropyLoss 也可以接收类别概率分布形式的 target

```text
logits：[B, C]
target：[B, C]
```

例如

```python
logits = torch.randn(2, 3)

soft_targets = torch.tensor([
    [0.7, 0.2, 0.1],
    [0.1, 0.3, 0.6],
])

loss = nn.CrossEntropyLoss()(
    logits,
    soft_targets,
)
```

此时 target 通常为浮点 Tensor，并表示每个类别的目标概率

适用于

- Mixup
- Label Smoothing
- 知识蒸馏
- 概率标签

不过，框架不会在所有情况下替你检查 target 是否构成合法概率分布，开发者应自行确保其语义正确。

## 5.4 多分类与多标签分类的区别

### 5.4.1 多分类

多分类中，每个样本只能属于一个类别。

例如识别一张图片属于

```text
猫 / 狗 / 鸟
```

一个样本不能同时既是猫又是狗。

输出

```text
logits.shape = [B, C]
```

目标

```text
targets.shape = [B]
targets[i] = 类别索引
```

损失函数

```python
criterion = nn.CrossEntropyLoss()
```

概率变换

```python
probabilities = torch.softmax(
    logits,
    dim=1,
)
```

Softmax 使每个样本的类别概率之和为 1

```text
Σc p_c = 1
```

### 5.4.2 多标签分类

多标签分类中，每个样本可以同时拥有多个标签。

例如图片可能同时包含

```text
人物、汽车、道路、建筑
```

每个类别都是独立的二元判断。

输出

```text
logits.shape = [B, C]
```

目标

```text
targets.shape = [B, C]
```

例如

```text
[1, 0, 1, 1]
```

表示第 0、2、3 类存在。

损失函数

```python
criterion = nn.BCEWithLogitsLoss()
```

概率变换

```python
probabilities = torch.sigmoid(logits)
```

每个类别概率独立，不要求总和为 1。

### 5.4.3 核心区别

|     对比项      |       多分类        |        多标签        |
| :----------: | :--------------: | :---------------: |
|     类别关系     |       相互排斥       |       相互独立        |
|   每个样本标签数    |        一个        |     零个、一个或多个      |
|    输出激活语义    |     Softmax      |      Sigmoid      |
| Target shape |      `[B]`       |      `[B,C]`      |
|  Target 内容   |       类别索引       |      0/1 向量       |
|   常用 Loss    | CrossEntropyLoss | BCEWithLogitsLoss |
|     推理决策     |     `argmax`     |      每类阈值判断       |

### 5.4.4 多分类推理

```python
logits = model(inputs)

predicted_classes = logits.argmax(
    dim=1
)
```

由于 Softmax 是单调变换

```text
argmax(logits)
=
argmax(softmax(logits))
```

只需要类别预测时，不必先计算 Softmax。

如果需要概率

```python
probabilities = torch.softmax(
    logits,
    dim=1,
)
```

### 5.4.5 多标签推理

```python
logits = model(inputs)
probabilities = torch.sigmoid(logits)

predictions = (
    probabilities >= 0.5
)
```

阈值不一定固定为 `0.5`。

可以根据验证集为不同类别设置不同阈值

```python
thresholds = torch.tensor([
    0.3,
    0.5,
    0.7,
], device=logits.device)

predictions = (
    probabilities >= thresholds
)
```

阈值选择会影响

- Precision
- Recall
- F1
- 业务误报率
- 业务漏报率

### 5.4.6 `pos_weight` 的作用

在多标签或二分类不平衡任务中

```python
criterion = nn.BCEWithLogitsLoss(
    pos_weight=positive_weights
)
```

`pos_weight` 用于调整正样本项的权重。官方文档指出，可以通过 `pos_weight` 在 Precision 和 Recall 之间进行权衡。

概念上，某类正样本越稀少，可考虑提高其正样本损失权重。

但应注意

- 它影响训练目标，不直接修改推理阈值
- 增大正样本权重常常提高 Recall，但可能降低 Precision
- 最终仍需在验证集上选择阈值

## 5.5 SGD、Momentum、Adam、AdamW

### 5.5.1 优化器的作用

Autograd 计算

```text
parameter.grad
```

优化器根据这些梯度和自身状态更新参数。

PyTorch 优化器需要接收待优化参数，并维护学习率、动量、权重衰减以及算法内部状态。

标准用法

```python
optimizer = torch.optim.AdamW(
    model.parameters(),
    lr=1e-3,
)

optimizer.zero_grad(
    set_to_none=True
)

loss.backward()
optimizer.step()
```

### 5.5.2 SGD

最基础的梯度下降更新为

```text
θₜ₊₁ = θₜ - ηgₜ
```

其中

- `θₜ`：当前参数
- `gₜ`：当前梯度
- `η`：学习率

PyTorch

```python
optimizer = torch.optim.SGD(
    model.parameters(),
    lr=0.1,
)
```

虽然名称是 SGD，但在深度学习中通常使用 mini-batch 梯度

```text
gₜ
=
一个 mini-batch 上的平均梯度
```

### 5.5.3 SGD 的特点

优点

- 更新规则简单
- 优化器状态少
- 额外显存占用低
- 在部分视觉任务中泛化表现较好
- 便于分析学习率和梯度行为

局限

- 对学习率敏感
- 不同方向曲率差异大时可能震荡
- 在狭长谷底中收敛较慢
- 通常需要良好的学习率调度

### 5.5.4 Momentum

Momentum 引入速度或动量状态，累积过去梯度方向。

一种常见表达为

```text
vₜ = μvₜ₋₁ + gₜ
θₜ₊₁ = θₜ - ηvₜ
```

其中

```text
μ = momentum coefficient
```

PyTorch

```python
optimizer = torch.optim.SGD(
    model.parameters(),
    lr=0.1,
    momentum=0.9,
)
```

PyTorch 的具体 SGD 实现细节应以当前 API 算法说明为准，其支持可选 Momentum、Dampening 和 Nesterov。

### 5.5.5 Momentum 为什么有效

可以类比为带惯性的运动。

如果连续多步梯度方向相似

```text
动量不断累积
→ 沿稳定方向加速
```

如果某个方向的梯度不断正负震荡

```text
正负贡献部分抵消
→ 减少横向震荡
```

因此 Momentum 常用于

- 加快一致方向上的优化
- 平滑 mini-batch 噪声
- 减少狭长谷底中的震荡

### 5.5.6 Nesterov Momentum

PyTorch SGD 支持

```python
optimizer = torch.optim.SGD(
    model.parameters(),
    lr=0.1,
    momentum=0.9,
    nesterov=True,
)
```

Nesterov Momentum 可以理解为：先根据当前动量预测即将到达的位置，再在预测位置附近计算或组合修正方向。

它比普通 Momentum 多了一定“向前看”的效果，但实际是否更好仍取决于任务和超参数。

### 5.5.7 Adam

Adam 为每个参数维护

- 梯度的一阶矩估计
- 梯度平方的二阶矩估计

简化形式

```text
mₜ = β₁mₜ₋₁ + (1-β₁)gₜ

vₜ = β₂vₜ₋₁ + (1-β₂)gₜ²
```

进行偏差修正

```text
m̂ₜ = mₜ / (1-β₁ᵗ)

v̂ₜ = vₜ / (1-β₂ᵗ)
```

参数更新

```text
θₜ₊₁
=
θₜ
-
η × m̂ₜ / (√v̂ₜ + ε)
```

PyTorch

```python
optimizer = torch.optim.Adam(
    model.parameters(),
    lr=1e-3,
    betas=(0.9, 0.999),
    eps=1e-8,
)
```

PyTorch Adam 默认维护一阶和二阶矩状态，并支持 AMSGrad 等选项。

### 5.5.8 Adam 的设计动机

Adam 结合了两个思想

```text
Momentum：
平滑梯度方向

自适应学习率：
根据历史梯度平方调整每个参数的更新尺度
```

如果某个参数历史梯度平方较大

```text
√v̂ 较大
→ 有效步长变小
```

如果某个参数历史梯度较小

```text
√v̂ 较小
→ 相对步长变大
```

这使 Adam 在以下场景中较常见

- Transformer
- LLM
- 稀疏或尺度差异明显的梯度
- 快速实验
- 对 SGD 学习率较敏感的模型

### 5.5.9 Adam 的优化器状态开销

对于每个参数，Adam 通常需要额外维护

```text
m：一阶矩
v：二阶矩
```

因此仅从主要状态看

```text
参数本身：1 份
梯度：1 份
m：1 份
v：1 份
```

混合精度训练中还可能存在 FP32 主权重等额外状态。

所以 Adam 的显存占用通常明显高于普通无 Momentum SGD。

这也是大模型训练中需要使用

- 优化器状态分片
- ZeRO
- FSDP
- CPU Offload

的重要原因之一。

### 5.5.10 AdamW

AdamW 的核心是将 Weight Decay 与 Adam 的梯度自适应更新解耦。

简化理解

```text
Adam：
先根据梯度计算自适应更新

AdamW：
执行自适应梯度更新
+
独立执行参数衰减
```

PyTorch

```python
optimizer = torch.optim.AdamW(
    model.parameters(),
    lr=1e-3,
    weight_decay=1e-2,
)
```

PyTorch 官方将 AdamW 描述为权重衰减不累积进 Momentum 和方差状态的 Adam 变体。

### 5.5.11 Adam 与 AdamW 的区别

如果把 L2 正则项直接加入 loss

```text
L_total
=
L_data + λ/2 ||θ||²
```

梯度变成

```text
∇L_total
=
∇L_data + λθ
```

Adam 会对整个梯度，包括 `λθ`，共同进行一阶、二阶矩估计和自适应缩放。

AdamW 则将参数衰减从梯度自适应过程里拆开

```text
θ ← θ - AdamUpdate(g)
θ ← θ - ηλθ
```

也可以写成近似合并形式

```text
θ ← (1-ηλ)θ - AdamUpdate(g)
```

解耦权重衰减的原始工作强调，在 Adam 等自适应优化器中，L2 正则化与 Weight Decay 并不等价。

### 5.5.12 如何选择优化器

没有一个优化器在所有任务中绝对最佳。

经验性选择

|        场景         |         常见选择         |
| :---------------: | :------------------: |
|     CNN 图像分类      | SGD + Momentum、AdamW |
| Transformer / LLM |        AdamW         |
|      快速实验与基线      |      Adam、AdamW      |
|      显存非常敏感       |      SGD 或分片优化器      |
|       稀疏梯度        |    需选择支持稀疏梯度的优化器     |

更重要的是联合调整

- 学习率
- Weight Decay
- Batch Size
- Warmup
- 调度策略
- 梯度裁剪

### 5.5.13 参数组

不同参数可以使用不同超参数

```python
optimizer = torch.optim.AdamW(
    [
        {
            "params": model.backbone.parameters(),
            "lr": 1e-4,
        },
        {
            "params": model.classifier.parameters(),
            "lr": 1e-3,
            "weight_decay": 0.0,
        },
    ],
    weight_decay=1e-2,
)
```

常见场景

- 预训练 backbone 使用较小学习率
- 新分类头使用较大学习率
- bias 和 Norm 参数不做 Weight Decay
- 不同模块使用不同衰减比例
- LoRA 参数单独配置

优化器维护参数组及其学习率、Weight Decay 等元数据。

## 5.6 Weight Decay 与 L2 正则

### 5.6.1 L2 正则化

L2 正则化在原始损失上增加参数平方项

```text
L_total
=
L_data
+
λ/2 ||θ||²
```

其中

```text
||θ||² = Σᵢ θᵢ²
```

其梯度为

```text
∇L_total
=
∇L_data
+
λθ
```

也就是说，参数越大，正则项提供的反向拉力越大。

### 5.6.2 Weight Decay

Weight Decay 直接在参数更新时使参数向零衰减。

对于基础 SGD

```text
θₜ₊₁
=
(1 - ηλ)θₜ
-
ηgₜ
```

可以分成

```text
梯度更新：
θ ← θ - ηg

参数衰减：
θ ← (1-ηλ)θ
```

其中

- `η` 是学习率
- `λ` 是 Weight Decay 系数

### 5.6.3 什么时候二者等价

对最基础的 SGD

```text
L2 梯度：
g + λθ
```

更新

```text
θ ← θ - η(g + λθ)
```

展开

```text
θ ← (1-ηλ)θ - ηg
```

这与 Weight Decay 形式一致。

因此可以说：在普通 SGD 的特定更新形式下，L2 正则与 Weight Decay 等价。

但这个结论不能无条件推广到所有优化器。

### 5.6.4 为什么在 Adam 中不等价

Adam 会对梯度进行

- Momentum 累积
- 二阶矩统计
- 参数级自适应缩放

如果把 L2 项加入梯度

```text
g' = g + λθ
```

那么 `λθ` 也会被纳入

```text
mₜ
vₜ
自适应分母
```

不同参数的正则项会被不同程度缩放。

AdamW 将衰减项从这个过程拆出

```text
先进行 Adam 梯度更新
再独立缩小参数
```

因此对于 Adam 类自适应优化器

```text
L2 penalty
≠
decoupled weight decay
```

这正是 AdamW 的设计重点。

### 5.6.5 PyTorch 中如何使用

AdamW

```python
optimizer = torch.optim.AdamW(
    model.parameters(),
    lr=1e-3,
    weight_decay=1e-2,
)
```

显式 L2 正则

```python
l2_penalty = torch.zeros(
    (),
    device=loss.device,
)

for parameter in model.parameters():
    l2_penalty = (
        l2_penalty
        + parameter.pow(2).sum()
    )

total_loss = (
    data_loss
    + 0.5 * regularization_strength
    * l2_penalty
)
```

这两个写法在自适应优化器中并不等价。

普通训练中通常优先使用优化器自带的 Weight Decay，而不是手动遍历所有参数构造正则项。

### 5.6.6 是否所有参数都应该衰减

工程中经常不对以下参数应用 Weight Decay

- bias
- LayerNorm 的缩放和偏置
- BatchNorm 的缩放和偏置
- 某些 Embedding
- 特定标量参数

原因是这些参数通常不表示普通权重矩阵，其尺度行为和正则化需求可能不同。

但这不是不可违反的定律。是否排除应结合

- 模型结构
- 论文或官方训练配置
- 数据规模
- 验证集结果
- 参数本身的语义

### 5.6.7 Weight Decay 不是解决过拟合的万能方法

Weight Decay 可能

- 限制参数规模
- 改善部分模型泛化
- 提供一定隐式或显式正则化效果

但过大的 Weight Decay 会造成

- 欠拟合
- 表征能力下降
- 训练 loss 降不下去
- 部分参数过度收缩

它需要与学习率联合调节，因为参数衰减步长通常与

```text
learning_rate × weight_decay
```

相关。

## 5.7 学习率调度器

### 5.7.1 为什么需要调整学习率

学习率决定每次参数更新的步长。

学习率过大

- loss 剧烈震荡
- 越过局部低谷
- 出现 NaN 或发散
- 训练后期难以精细收敛

学习率过小

- 收敛速度慢
- 长时间停留在平坦区域
- 训练预算内无法充分优化
- 可能过早表现为“训练不动”

一种常见策略是

```text
训练初期：
使用较大或逐渐增大的学习率快速探索

训练后期：
降低学习率进行精细收敛
```

学习率调度器直接调整优化器各参数组中的学习率。

### 5.7.2 StepLR

每隔固定 epoch 将学习率乘以 `gamma`

```python
optimizer = torch.optim.SGD(
    model.parameters(),
    lr=0.1,
)

scheduler = torch.optim.lr_scheduler.StepLR(
    optimizer,
    step_size=30,
    gamma=0.1,
)
```

训练

```python
for epoch in range(num_epochs):
    train_one_epoch(
        model,
        train_loader,
        optimizer,
    )

    scheduler.step()
```

学习率变化

```text
epoch 0～29：0.1
epoch 30～59：0.01
epoch 60～89：0.001
```

优点是简单直观，缺点是变化不平滑，里程碑需要手动设置。

### 5.7.3 MultiStepLR

在指定 epoch 下降

```python
scheduler = (
    torch.optim.lr_scheduler.MultiStepLR(
        optimizer,
        milestones=[30, 60, 90],
        gamma=0.1,
    )
)
```

适用于已有成熟训练经验、明确知道在哪些阶段降低学习率的任务。

### 5.7.4 CosineAnnealingLR

学习率按照余弦曲线逐渐下降

```python
scheduler = (
    torch.optim.lr_scheduler.CosineAnnealingLR(
        optimizer,
        T_max=num_epochs,
        eta_min=1e-6,
    )
)
```

通常具有

```text
前期下降较慢
中间平滑变化
后期逐渐接近最小学习率
```

PyTorch `CosineAnnealingLR` 通过 `T_max` 指定调度周期长度，通过 `eta_min` 指定最小学习率。

### 5.7.5 ReduceLROnPlateau

当监控指标长时间不再改善时降低学习率

```python
scheduler = (
    torch.optim.lr_scheduler.ReduceLROnPlateau(
        optimizer,
        mode="min",
        factor=0.1,
        patience=5,
        min_lr=1e-6,
    )
)
```

训练

```python
for epoch in range(num_epochs):
    train_one_epoch(...)

    validation_loss = validate(...)

    scheduler.step(
        validation_loss
    )
```

它与普通 scheduler 的主要区别是

```text
普通 scheduler.step()
通常根据 epoch 或 step 推进

ReduceLROnPlateau.step(metric)
根据监控指标更新
```

`ReduceLROnPlateau` 会在监控指标经过指定 patience 后仍未改善时降低学习率。

### 5.7.6 Warmup

Warmup 在训练初期从较小学习率逐渐增加到目标学习率。

设计动机包括

- 初始参数和优化器状态尚未稳定
- 大 batch 训练初期更新可能过大
- Transformer 中早期梯度可能不稳定
- Adam 的矩估计处于建立阶段

可以使用 `LinearLR`

```python
warmup_scheduler = (
    torch.optim.lr_scheduler.LinearLR(
        optimizer,
        start_factor=0.1,
        end_factor=1.0,
        total_iters=1000,
    )
)
```

`LinearLR` 会在指定迭代范围内线性改变学习率乘法因子。

注意需要明确

```text
total_iters 表示 epoch
还是 optimizer step
```

这取决于你多久调用一次 `scheduler.step()`。

### 5.7.7 Warmup + Cosine

在 Transformer 和 LLM 训练中常见

```text
前若干 step：
线性 Warmup

之后：
Cosine Decay
```

PyTorch 可以通过组合调度器实现

```python
warmup_steps = 1000
total_steps = 10000

warmup = torch.optim.lr_scheduler.LinearLR(
    optimizer,
    start_factor=0.1,
    end_factor=1.0,
    total_iters=warmup_steps,
)

cosine = (
    torch.optim.lr_scheduler.CosineAnnealingLR(
        optimizer,
        T_max=(
            total_steps - warmup_steps
        ),
        eta_min=1e-6,
    )
)

scheduler = (
    torch.optim.lr_scheduler.SequentialLR(
        optimizer,
        schedulers=[
            warmup,
            cosine,
        ],
        milestones=[warmup_steps],
    )
)
```

如果 scheduler 按 step 更新，就应在每次真正的 `optimizer.step()` 后调用。

### 5.7.8 OneCycleLR

OneCycle 策略先将学习率从较小值增加到最大值，再下降到很小值。

```python
scheduler = torch.optim.lr_scheduler.OneCycleLR(
    optimizer,
    max_lr=1e-2,
    epochs=num_epochs,
    steps_per_epoch=len(train_loader),
)
```

标准调用

```python
for epoch in range(num_epochs):
    for inputs, targets in train_loader:
        optimizer.zero_grad(
            set_to_none=True
        )

        loss = compute_loss(
            model,
            inputs,
            targets,
        )

        loss.backward()
        optimizer.step()
        scheduler.step()
```

OneCycleLR 按 batch 更新，`scheduler.step()` 应在每个训练 batch 的 `optimizer.step()` 之后调用。

### 5.7.9 `optimizer.step()` 与 `scheduler.step()` 的顺序

通常应

```python
optimizer.step()
scheduler.step()
```

PyTorch 的通用调度器文档也建议在 optimizer 更新后调用 scheduler 的 `step()`。

标准训练

```python
loss.backward()

optimizer.step()
scheduler.step()

optimizer.zero_grad(
    set_to_none=True
)
```

但调度频率仍取决于调度器

|     Scheduler     |        常见调用频率         |
| :---------------: | :-------------------: |
|      StepLR       |       每个 epoch        |
|    MultiStepLR    |       每个 epoch        |
| CosineAnnealingLR | 每 epoch 或每 step，取决于设计 |
|    OneCycleLR     |   每个 optimizer step   |
| ReduceLROnPlateau |    每次验证后，传入 metric    |
|      Warmup       |  通常每个 optimizer step  |

### 5.7.10 梯度累积下的 Scheduler

假设每 4 个 micro-batch 更新一次参数。

错误逻辑

```python
loss.backward()
scheduler.step()
```

这会导致学习率按 micro-batch 推进，但参数并没有同步更新。

正确逻辑

```python
loss.backward()

if should_update:
    optimizer.step()
    scheduler.step()

    optimizer.zero_grad(
        set_to_none=True
    )
```

调度器中的 step 通常应对应真正的参数更新次数，而不是 forward 次数。

### 5.7.11 查看当前学习率

```python
current_lrs = scheduler.get_last_lr()
print(current_lrs)
```

或者

```python
for group in optimizer.param_groups:
    print(group["lr"])
```

如果有多个参数组，学习率可能不同。

`get_last_lr()` 返回每个 optimizer 参数组当前最近一次计算出的学习率。

### 5.7.12 Checkpoint 中保存 Scheduler

```python
checkpoint = {
    "model": model.state_dict(),
    "optimizer": optimizer.state_dict(),
    "scheduler": scheduler.state_dict(),
    "epoch": epoch,
    "global_step": global_step,
}
```

恢复 scheduler 状态非常重要，否则学习率可能重新从初始阶段开始。

恢复时还要注意优化器和 scheduler 的初始化与加载顺序。当前 PyTorch 调度器文档提示，恢复 checkpoint 时应避免 scheduler 初始化覆盖优化器中已加载的学习率。

## 5.8 梯度裁剪

### 5.8.1 为什么需要梯度裁剪

如果梯度范数过大

```text
参数更新量
≈
learning_rate × gradient
```

参数可能在一步内发生巨大变化，导致

- loss 剧烈波动
- 参数发散
- 激活出现 Inf
- loss 变成 NaN
- 训练不稳定

梯度裁剪通过限制梯度大小降低极端更新风险。

常见于

- RNN
- LSTM
- Transformer
- LLM
- 深层网络
- 强化学习
- 高阶梯度训练

### 5.8.2 按梯度范数裁剪

PyTorch 标准写法

```python
total_norm = (
    torch.nn.utils.clip_grad_norm_(
        model.parameters(),
        max_norm=1.0,
    )
)
```

它把所有参数梯度视为一个整体，计算总范数。如果超过 `max_norm`，就按统一比例缩小所有梯度。PyTorch `clip_grad_norm_()` 会基于所有参数梯度的整体范数执行原地裁剪，并返回裁剪前的总范数。

概念上

```text
如果 ||g|| ≤ max_norm：
保持不变

如果 ||g|| > max_norm：
g ← g × max_norm / ||g||
```

这样

- 梯度整体方向基本不变
- 只缩小梯度整体尺度

### 5.8.3 标准调用位置

```python
optimizer.zero_grad(
    set_to_none=True
)

logits = model(inputs)
loss = criterion(logits, targets)

loss.backward()

total_norm = (
    torch.nn.utils.clip_grad_norm_(
        model.parameters(),
        max_norm=1.0,
    )
)

optimizer.step()
```

顺序必须是

```text
backward
   ↓
得到梯度
   ↓
clip gradients
   ↓
optimizer.step
```

如果在 `backward()` 前裁剪，此时梯度尚未计算。

如果在 `optimizer.step()` 后裁剪，本次参数更新已经完成。

### 5.8.4 按数值裁剪

```python
torch.nn.utils.clip_grad_value_(
    model.parameters(),
    clip_value=1.0,
)
```

它将每一个梯度元素限制到

```text
[-1, 1]
```

按值裁剪会直接改变各梯度元素的相对关系，因此整体方向可能明显变化。

PyTorch 提供 `clip_grad_value_()` 用于将参数梯度元素限制在指定范围内。

### 5.8.5 Norm Clipping 与 Value Clipping

|     对比项     |   Norm Clipping   |   Value Clipping   |
| :---------: | :---------------: | :----------------: |
|    限制对象     |      整体梯度范数       |       每个梯度元素       |
|  是否保持整体方向   |       基本保持        |        可能改变        |
|    常见程度     |        更常见        |        特定需求        |
| PyTorch API | `clip_grad_norm_` | `clip_grad_value_` |

大多数深度学习训练更常使用 Norm Clipping。

### 5.8.6 `norm_type`

默认

```python
clip_grad_norm_(
    parameters,
    max_norm=1.0,
    norm_type=2.0,
)
```

表示使用 L2 范数。

也可以使用

```python
norm_type=float("inf")
```

此时总范数对应最大绝对梯度元素。

不要只记住 `max_norm=1.0`。阈值应根据

- 模型规模
- 损失归约方式
- 优化器
- 学习率
- 混合精度
- 实际梯度范数分布

进行选择。

### 5.8.7 记录梯度范数

由于 `clip_grad_norm_()` 返回裁剪前总范数，可以用于监控

```python
total_norm = (
    torch.nn.utils.clip_grad_norm_(
        model.parameters(),
        max_norm=1.0,
        error_if_nonfinite=True,
    )
)

logger.log({
    "gradient_norm": total_norm.item()
})
```

如果长期

```text
total_norm 远低于 max_norm
```

说明裁剪几乎没有触发。

如果几乎每一步

```text
total_norm 远高于 max_norm
```

说明需要进一步检查

- 学习率是否过大
- loss scale 是否异常
- 损失归约是否错误
- 数据是否存在异常
- 模型是否数值不稳定
- 阈值是否过小

梯度裁剪不应成为掩盖训练错误的工具。

### 5.8.8 AMP 下的梯度裁剪

混合精度训练中，GradScaler 会先放大 loss

```text
scaled_loss = loss × scale
```

此时参数梯度也是放大后的梯度。

因此应先恢复真实梯度，再裁剪

```python
scaler.scale(loss).backward()

scaler.unscale_(optimizer)

torch.nn.utils.clip_grad_norm_(
    model.parameters(),
    max_norm=1.0,
)

scaler.step(optimizer)
scaler.update()
```

如果在 `unscale_()` 前裁剪，裁剪的是被放大的梯度，阈值语义错误。

### 5.8.9 梯度累积下的裁剪

梯度累积时，不应每个 micro-batch 都裁剪，否则会改变累积结果。

标准逻辑

```python
optimizer.zero_grad(
    set_to_none=True
)

for step, batch in enumerate(loader):
    loss = compute_loss(batch)
    loss = loss / accumulation_steps

    loss.backward()

    should_update = (
        (step + 1)
        % accumulation_steps
        == 0
    )

    if should_update:
        torch.nn.utils.clip_grad_norm_(
            model.parameters(),
            max_norm=1.0,
        )

        optimizer.step()

        optimizer.zero_grad(
            set_to_none=True
        )
```

应在完整累积梯度形成后、真正执行 optimizer step 前裁剪。

### 5.8.10 梯度裁剪不能解决所有 NaN

梯度裁剪主要限制反向梯度大小。

如果 NaN 已经在前向阶段产生

```text
log(负数)
除以零
exp 溢出
非法归一化
输入包含 NaN
```

则裁剪可能已经来不及。

正确排查应区分

```text
前向输出是否已经 NaN
loss 是否 NaN
梯度是否 NaN
optimizer.step 后参数是否 NaN
```

可以配合

```python
if not torch.isfinite(loss):
    raise FloatingPointError(
        f"Non-finite loss: {loss.item()}"
    )
```

以及

```python
clip_grad_norm_(
    model.parameters(),
    max_norm=1.0,
    error_if_nonfinite=True,
)
```

## 5.9 本章小节

```text
模型输出 logits
       ↓
损失函数
       ↓
标量 loss
       ↓
loss.backward()
       ↓
parameter.grad
       ↓
梯度裁剪
       ↓
optimizer.step()
       ↓
参数更新
       ↓
scheduler.step()
       ↓
调整后续学习率
```

优化组件的职责

```text
Loss
定义优化目标

Autograd
计算梯度

Optimizer
决定更新规则

Weight Decay
限制或衰减参数规模

Scheduler
控制不同时期的学习率

Gradient Clipping
限制极端梯度更新
```

|       任务       |          标准方法           |
| :------------: | :---------------------: |
|     连续值回归      |      MSE、L1、Huber       |
|      二分类       |    BCEWithLogitsLoss    |
|     单标签多分类     |    CrossEntropyLoss     |
|     多标签分类      |    BCEWithLogitsLoss    |
|     类别不平衡      | 类别权重、`pos_weight`、Focal |
| Transformer 优化 | AdamW + Warmup / Cosine |
|     基础视觉模型     | SGD + Momentum 或 AdamW  |
|     梯度爆炸风险     |   `clip_grad_norm_()`   |

完成本章后，应能够独立回答

1. 损失函数和评价指标有什么区别
2. 为什么损失函数通常需要可导
3. `mean`、`sum` 和 `none` 如何影响梯度
4. MSE、L1 和 Huber 的区别
5. BCELoss 与 BCEWithLogitsLoss 的区别
6. 为什么 CrossEntropyLoss 前不能手动 Softmax
7. CrossEntropyLoss 与 LogSoftmax、NLLLoss 的关系
8. CrossEntropyLoss 的 logits 和 target shape 是什么
9. `weight`、`ignore_index` 和 `label_smoothing` 分别做什么
10. 多分类与多标签分类有什么区别
11. Softmax 和 Sigmoid 分别用于什么场景
12. SGD、Momentum、Adam 和 AdamW 有什么区别
13. Adam 为什么需要一阶矩、二阶矩和偏差修正
14. Adam 优化器为什么占用更多显存
15. Weight Decay 与 L2 正则何时等价
16. AdamW 为什么要解耦 Weight Decay
17. 哪些参数经常不使用 Weight Decay
18. Warmup、Cosine、OneCycle 和 ReduceLROnPlateau 的区别
19. `optimizer.step()` 与 `scheduler.step()` 的调用顺序
20. 梯度累积时 scheduler 应如何推进
21. Norm Clipping 与 Value Clipping 的区别
22. 梯度裁剪应放在训练流程的哪个位置
23. AMP 下为什么要先 unscale 再裁剪
24. 为什么梯度裁剪不能替代数值异常排查

# 6 Dataset 与 DataLoader

在完整的 PyTorch 训练系统中，模型计算只是其中一部分。训练数据还需要经历

```text
磁盘、数据库或内存中的原始数据
                ↓
读取单个样本
                ↓
数据解析与预处理
                ↓
采样和打乱
                ↓
组成 Batch
                ↓
复制到 GPU
                ↓
输入模型
```

PyTorch 使用两个核心抽象组织这条数据流水线

```text
Dataset：
定义如何获得一个样本

DataLoader：
定义如何采样、并行读取、组成 Batch 并交付数据
```

二者的职责需要明确分离

|       组件       |         主要职责         |
| :------------: | :------------------: |
|   `Dataset`    |   描述数据集以及单个样本如何读取    |
|   `Sampler`    |    决定读取哪些样本以及读取顺序    |
| `BatchSampler` |     将多个样本索引组成一批      |
|  `collate_fn`  | 将多个样本组合成模型可使用的 Batch |
|  `DataLoader`  |   组织采样、并行加载、预取和批处理   |
|      训练循环      | 将 Batch 移动到设备并执行模型计算 |

PyTorch 官方将 `Dataset` 和 `DataLoader` 作为两个基础数据原语：前者保存样本及标签的访问逻辑，后者围绕 Dataset 提供可迭代的数据加载能力。

## 6.1 Dataset 的作用

### 6.1.1 Dataset 是数据访问协议

`Dataset` 并不一定要求把全部数据预先加载到内存中。

它主要回答一个问题：给定一个样本索引或迭代位置，如何得到对应的数据和标签？

例如，一个图像分类数据集的单个样本可能是

```text
(image_tensor, class_index)
```

一个文本分类数据集的单个样本可能是

```text
{
    "input_ids": Tensor,
    "attention_mask": Tensor,
    "label": int
}
```

一个时间序列数据集可能返回

```text
(signal, valid_length, label, subject_id)
```

Dataset 将原始存储方式和模型训练逻辑隔离开

```text
原始数据格式
CSV / JSON / 图片 / 音频 / 数据库
                ↓
             Dataset
                ↓
统一的样本结构
                ↓
             DataLoader
                ↓
训练循环
```

这样即使数据来源改变，模型和训练循环通常也不需要大幅修改。

### 6.1.2 为什么不直接在训练循环中读取文件

可以在训练循环中直接写

```python
for path in file_paths:
    image = read_image(path)
    label = ...
```

但这种方式会把以下逻辑混在一起

- 文件读取
- 数据解析
- 数据增强
- 标签转换
- 批处理
- 模型训练
- 异常处理

最终训练代码会变得难以维护、测试和复用。

使用 Dataset 后

```python
dataset = MyDataset(...)
loader = DataLoader(dataset, ...)

for inputs, targets in loader:
    outputs = model(inputs)
```

训练循环只关心模型所需的输入，不需要知道数据来自图片、数据库还是压缩文件。

### 6.1.3 Map-style Dataset

最常见的是映射式数据集，即 Map-style Dataset。

它可以抽象为

```text
index → sample
```

例如

```text
0 → 第一个样本
1 → 第二个样本
2 → 第三个样本
```

通常继承

```python
torch.utils.data.Dataset
```

并实现

```python
__len__()
__getitem__()
```

PyTorch 的 `Dataset` 基类用于表示从 key 到样本的映射。子类必须实现 `__getitem__()`，多数 Sampler 和 DataLoader 默认配置还依赖 `__len__()`。

示例

```python
from __future__ import annotations

from pathlib import Path

import torch
from torch.utils.data import Dataset


class FeatureDataset(Dataset):
    def __init__(
        self,
        feature_paths: list[Path],
        labels: list[int],
    ) -> None:
        if len(feature_paths) != len(labels):
            raise ValueError(
                "feature_paths and labels must "
                "have the same length"
            )

        self.feature_paths = feature_paths
        self.labels = labels

    def __len__(self) -> int:
        return len(self.feature_paths)

    def __getitem__(
        self,
        index: int,
    ) -> tuple[torch.Tensor, int]:
        path = self.feature_paths[index]

        # 假设文件中保存的是 Tensor
        feature = torch.load(
            path,
            map_location="cpu",
        )

        label = self.labels[index]

        return feature, label
```

这个 Dataset 并没有在初始化时把所有特征加载到内存，而是在访问某个样本时读取对应文件。

### 6.1.4 IterableDataset

对于流式数据或无法随机索引的数据，可以使用

```python
torch.utils.data.IterableDataset
```

它的基本抽象是

```text
数据流 → 依次产生样本
```

而不是

```text
index → sample
```

典型场景包括

- 实时消息流
- 日志流
- 网络数据
- 超大规模顺序文件
- 数据库游标
- 动态生成样本
- 无法预先知道总长度的数据

示例

```python
from collections.abc import Iterator

from torch.utils.data import IterableDataset


class NumberStreamDataset(IterableDataset):
    def __init__(
        self,
        start: int,
        end: int,
    ) -> None:
        super().__init__()

        if end < start:
            raise ValueError(
                "end must not be smaller than start"
            )

        self.start = start
        self.end = end

    def __iter__(self) -> Iterator[int]:
        for value in range(
            self.start,
            self.end,
        ):
            yield value
```

`IterableDataset` 子类需要实现 `__iter__()`。当 `num_workers > 0` 时，每个 worker 会持有自己的 Dataset 副本，因此通常需要主动对数据流进行分片，否则不同 worker 可能返回重复样本。

### 6.1.5 Map-style 与 Iterable-style 对比

|   对比项    | Map-style Dataset | IterableDataset |
| :------: | :---------------: | :-------------: |
|   核心接口   |   `__getitem__`   |   `__iter__`    |
|  是否随机索引  |       通常支持        |      通常不支持      |
|  是否容易打乱  |        容易         |     需要自行设计      |
| 是否容易确定长度 |       通常容易        |     可能无法确定      |
|   典型场景   |    图片、表格、离线样本     |  数据流、超大文件、在线数据  |
| 多进程注意事项  |     索引由主进程分配      | 每个 worker 需正确分片 |

### 6.1.6 Dataset 中通常放什么

适合放入 Dataset 的逻辑

- 样本路径管理
- 文件读取
- 数据解析
- 单样本预处理
- 单样本数据增强
- 标签转换
- 样本级异常检查

不适合放入 Dataset 的逻辑

- 模型前向传播
- GPU 上的大规模模型计算
- `optimizer.step()`
- 全局训练指标更新
- 与其他 batch 强耦合的逻辑
- 每个样本都重复初始化的大型对象

例如，不应该在 `__getitem__()` 中每次重新创建 tokenizer

```python
def __getitem__(self, index):
    tokenizer = ExpensiveTokenizer(...)
```

更合理的是在 `__init__()` 中创建一次，或者从外部传入。

### 6.1.7 Dataset 是否应该返回 GPU Tensor

通常不建议普通 DataLoader 的 Dataset 直接返回 CUDA Tensor。

更标准的流程是

```text
worker 进程：
读取和处理 CPU 数据
        ↓
主进程：
接收 CPU Batch
        ↓
训练循环：
将 Batch 复制到 GPU
```

这样更容易结合

- 多进程 DataLoader
- pinned memory
- `non_blocking=True`
- 多 GPU 进程隔离

如果在 worker 中直接初始化 CUDA 上下文，可能增加多进程复杂性和显存管理风险。

## 6.2 `__len__` 与 `__getitem__`

### 6.2.1 `__len__()` 的作用

对于 Map-style Dataset

```python
def __len__(self) -> int:
    return number_of_samples
```

它通常表示数据集可以访问的样本数量。

示例

```python
class SimpleDataset(Dataset):
    def __init__(
        self,
        features: torch.Tensor,
        labels: torch.Tensor,
    ) -> None:
        if len(features) != len(labels):
            raise ValueError(
                "features and labels must "
                "have the same length"
            )

        self.features = features
        self.labels = labels

    def __len__(self) -> int:
        return self.features.shape[0]

    def __getitem__(
        self,
        index: int,
    ) -> tuple[torch.Tensor, torch.Tensor]:
        return (
            self.features[index],
            self.labels[index],
        )
```

调用

```python
dataset = SimpleDataset(
    features=torch.randn(100, 16),
    labels=torch.randint(0, 4, (100,)),
)

print(len(dataset))  # 100
```

`__len__()` 会被许多默认 Sampler、BatchSampler 和 `len(dataloader)` 的估算逻辑使用。

### 6.2.2 `__getitem__()` 的作用

`__getitem__()` 定义如何取得一个样本

```python
sample = dataset[index]
```

例如

```python
feature, label = dataset[10]
```

在默认的 Map-style DataLoader 中，一个 Batch 的读取过程近似为

```python
samples = [
    dataset[index]
    for index in batch_indices
]

batch = collate_fn(samples)
```

PyTorch 官方将自动批处理的 Map-style 数据加载过程近似描述为

```python
for indices in batch_sampler:
    yield collate_fn(
        [dataset[i] for i in indices]
    )
```

### 6.2.3 `__getitem__()` 应返回什么

PyTorch 不强制单个样本只能是 Tensor。

可以返回

**Tuple**

```python
return feature, label
```

**Dictionary**

```python
return {
    "input": feature,
    "label": label,
}
```

**多个字段**

```python
return {
    "signal": signal,
    "length": length,
    "label": label,
    "subject_id": subject_id,
}
```

默认 `collate_fn` 能够处理常见的

- Tensor
- NumPy 数组
- Python 数值
- 字典
- tuple
- list
- namedtuple

它会尽量保留原有容器结构，并在外层增加 Batch 维。

### 6.2.4 Dataset 应返回单样本还是 Batch

标准 Dataset 的 `__getitem__()` 通常返回单个样本

```text
feature.shape = [D]
label.shape   = []
```

DataLoader 组成 Batch 后

```text
features.shape = [B, D]
labels.shape   = [B]
```

不建议在普通 `__getitem__()` 中自行再创建 Batch，否则容易与 DataLoader 的自动批处理重复。

不过，在数据库批量查询、连续磁盘读取等特殊场景，也可以关闭自动 batching，或者实现批量读取接口。

### 6.2.5 可选的 `__getitems__()`

较新的 PyTorch Dataset 协议允许 Map-style Dataset 可选实现

```python
__getitems__(indices)
```

它一次接收一组索引并返回一组样本，可以减少逐样本 Python 调用或利用批量 I/O。PyTorch 官方文档和数据加载优化教程均说明，该接口可用于加速 batched sample loading

示例

```python
class BatchedFeatureDataset(Dataset):
    def __init__(
        self,
        features: torch.Tensor,
        labels: torch.Tensor,
    ) -> None:
        self.features = features
        self.labels = labels

    def __len__(self) -> int:
        return len(self.features)

    def __getitem__(
        self,
        index: int,
    ) -> tuple[torch.Tensor, torch.Tensor]:
        return (
            self.features[index],
            self.labels[index],
        )

    def __getitems__(
        self,
        indices: list[int],
    ) -> list[
        tuple[torch.Tensor, torch.Tensor]
    ]:
        batch_features = self.features[indices]
        batch_labels = self.labels[indices]

        return [
            (
                batch_features[i],
                batch_labels[i],
            )
            for i in range(len(indices))
        ]
```

它更适合

- 一次数据库查询读取多行
- 一次从大文件读取连续块
- 能向量化处理多个样本
- 单样本函数调用开销很高

普通项目中先实现可靠的 `__getitem__()` 即可，不必为了形式强行增加 `__getitems__()`。

### 6.2.6 数据增强应该放在哪里

单样本数据增强通常放在 `__getitem__()` 中

```python
class ImageDataset(Dataset):
    def __init__(
        self,
        image_paths,
        labels,
        transform=None,
    ):
        self.image_paths = image_paths
        self.labels = labels
        self.transform = transform

    def __len__(self):
        return len(self.image_paths)

    def __getitem__(self, index):
        image = read_image(
            self.image_paths[index]
        )

        if self.transform is not None:
            image = self.transform(image)

        label = self.labels[index]

        return image, label
```

这样使用多 worker 时，不同样本的数据解析和增强可以并行执行。

但需要注意

- transform 应尽量可序列化
- 不要在每次调用时初始化大对象
- 随机增强需要考虑 worker 随机种子
- CPU 增强过重可能成为瓶颈

### 6.2.7 `__getitem__()` 中的异常处理

不建议无条件吞掉异常

```python
def __getitem__(self, index):
    try:
        ...
    except Exception:
        return torch.zeros(...)
```

这样会掩盖

- 文件损坏
- 标签错位
- 数据格式错误
- 路径错误
- 解码失败

更合理的方式是抛出包含索引和路径的信息

```python
def __getitem__(self, index):
    path = self.paths[index]

    try:
        sample = load_sample(path)
    except Exception as error:
        raise RuntimeError(
            f"Failed to load index={index}, "
            f"path={path}"
        ) from error

    return sample
```

### 6.2.8 自定义非整数索引

默认 DataLoader 的索引 Sampler 通常产生整数索引。

如果 Dataset 使用字符串 key

```python
dataset["sample_001"]
```

则需要自定义 Sampler 来产生相应 key。PyTorch 文档明确指出，默认索引 Sampler 产生整数索引；对非整数 key 的 Map-style Dataset，需要提供自定义 Sampler。

## 6.3 DataLoader 的基本流程

### 6.3.1 DataLoader 解决什么问题

Dataset 只定义

```text
如何获得一个样本
```

DataLoader 进一步负责

- 选择样本索引
- 打乱样本顺序
- 将索引组成 Batch
- 调用 Dataset 读取样本
- 多进程并行读取
- 调用 `collate_fn`
- 预取后续 Batch
- 将 Batch 放入 pinned memory
- 向训练循环提供迭代接口

基本使用

```python
from torch.utils.data import DataLoader

loader = DataLoader(
    dataset,
    batch_size=32,
    shuffle=True,
    num_workers=4,
    pin_memory=True,
)
```

训练

```python
for inputs, targets in loader:
    ...
```

### 6.3.2 Map-style DataLoader 的完整链路

可以将一个 Batch 的生成过程理解为

```text
Sampler
产生样本索引
    ↓
BatchSampler
将索引组成一批
    ↓
Worker 进程
调用 dataset[index]
    ↓
得到一组样本
    ↓
collate_fn
将样本组成 Batch
    ↓
可选的 pin_memory
    ↓
主进程获得 Batch
    ↓
复制到 GPU
```

更接近伪代码的表达是

```python
for indices in batch_sampler:
    samples = [
        dataset[index]
        for index in indices
    ]

    batch = collate_fn(samples)
    yield batch
```

`batch_size` 和 `drop_last` 本质上帮助 DataLoader 从 Sampler 构建 BatchSampler；随后 `collate_fn` 将取得的一组样本合并成 Batch。

### 6.3.3 `num_workers=0` 的流程

`num_workers` 表示用于数据加载的子进程数量；设置为 `0` 时，数据在主进程加载。

```python
loader = DataLoader(
    dataset,
    batch_size=32,
    num_workers=0,
)
```

此时

```text
训练主进程
├── 调用 Dataset
├── 执行数据增强
├── 执行 collate_fn
└── 执行模型训练
```

所有数据加载工作都在主进程执行。

优点

- 调试简单
- 异常栈更直接
- 没有进程通信开销
- 适合小数据和轻量预处理

缺点

- 数据读取和模型训练串行
- CPU 处理慢时 GPU 可能等待数据

### 6.3.4 `num_workers>0` 的流程

```python
loader = DataLoader(
    dataset,
    batch_size=32,
    num_workers=4,
)
```

此时 DataLoader 会使用 worker 子进程并行读取数据。

```text
主进程
├── 生成或协调样本索引
├── 接收已完成 Batch
└── 执行 GPU 训练

Worker 0
└── 读取和处理部分 Batch

Worker 1
└── 读取和处理部分 Batch

Worker 2
└── 读取和处理部分 Batch

Worker 3
└── 读取和处理部分 Batch
```

### 6.3.5 多进程中 Dataset 的行为

当 `num_workers > 0` 时，每个 worker 进程会拥有各自的 Dataset 对象副本。对 IterableDataset，这一点尤其重要，因为如果每个副本都从相同位置迭代，就可能重复返回数据。

这还意味着

- Dataset 中的大型 Python 对象可能在多个 worker 中占用额外内存
- 每个 worker 可能各自打开文件或数据库连接
- Dataset 内部可变状态通常不与主进程实时共享
- 不能依赖 worker 修改 Dataset 后主进程能直接看到

### 6.3.6 Worker 何时启动

通常在创建 DataLoader 迭代器时

```python
iterator = iter(loader)
```

多进程 worker 才开始工作，而不是单纯执行

```python
loader = DataLoader(...)
```

时立刻完成所有数据加载。

每次新 epoch

```python
for batch in loader:
    ...
```

会创建新的迭代器。

如果

```python
persistent_workers=False
```

worker 通常会在一轮迭代结束后退出，下一轮重新创建。

如果

```python
persistent_workers=True
```

worker 会在 Dataset 被消费完后继续保留，从而减少跨 epoch 的进程创建和初始化开销。

### 6.3.7 `prefetch_factor`

当 `num_workers > 0` 时，可以设置

```python
DataLoader(
    dataset,
    num_workers=4,
    prefetch_factor=2,
)
```

它控制每个 worker 预先准备的 Batch 数量。

粗略理解

```text
预取中的 Batch 数量
≈
num_workers × prefetch_factor
```

例如

```text
4 workers × 2
≈ 最多预取 8 个 Batch
```

预取可以减少训练循环等待下一批数据的时间，但也会增加

- CPU 内存占用
- 共享内存占用
- 预处理中的样本数量
- 数据延迟和队列长度

PyTorch 的数据加载优化教程说明，每个 worker 维护由 `prefetch_factor` 控制的 Batch 队列；worker 会并行准备 Batch 并传回主进程。

### 6.3.8 `persistent_workers`

```python
loader = DataLoader(
    dataset,
    batch_size=32,
    num_workers=4,
    persistent_workers=True,
)
```

适合

- 训练 epoch 较多
- 每个 epoch 较短
- worker 初始化开销高
- Dataset 初始化或文件连接较重

但它也会导致

- worker 进程长期存在
- Dataset 副本持续占用内存
- 文件句柄和连接持续保留
- 数据集状态更新需要额外设计

### 6.3.9 `drop_last`

如果样本数不能整除 batch size

```text
样本数：103
batch_size：32
```

则 Batch 大小为

```text
32, 32, 32, 7
```

设置

```python
drop_last=True
```

后，最后一个不完整 Batch 会被丢弃。

常见原因

- BatchNorm 对极小 Batch 不稳定
- 模型要求固定 Batch shape
- 分布式训练需要更一致的步数
- 对比学习对 Batch 结构有要求

但会损失少量训练样本。

对于 IterableDataset 和多 worker 场景，`drop_last` 的含义更复杂，可能针对各 worker Dataset 副本的最后不完整批次进行处理。

### 6.3.10 `len(dataloader)`

对普通 Map-style Dataset，通常近似为

```text
drop_last=False：
ceil(len(dataset) / batch_size)

drop_last=True：
floor(len(dataset) / batch_size)
```

但真正长度主要依据 Sampler 或 BatchSampler。

对 IterableDataset，PyTorch 只能根据 Dataset 长度和 batch size 做启发式估算；在多 worker 分片和 `drop_last` 场景中，可能不完全准确。

### 6.3.11 DataLoader 生成一个 Batch 的流程是什么？

对于 Map-style Dataset，Sampler 先产生样本索引，BatchSampler 将多个索引组成一批。worker 根据这些索引调用 Dataset 的 `__getitem__()` 读取样本，然后 `collate_fn` 将样本列表合并成 Batch。

如果启用了 `pin_memory`，Batch 中的 Tensor 还会被复制到 pinned memory，最后由主进程交给训练循环，并由训练循环移动到 GPU。

DataLoader worker 通常负责 CPU 数据读取和预处理，而不是替主进程执行模型的 GPU forward。

## 6.4 `batch_size`、`shuffle`、`num_workers`

### 6.4.1 `batch_size`

`batch_size` 表示一次迭代交付给训练循环的样本数量。

```python
loader = DataLoader(
    dataset,
    batch_size=64,
)
```

如果单个样本

```text
feature.shape = [D]
```

默认 collate 后

```text
batch.shape = [64, D]
```

### 6.4.2 batch size 对训练的影响

较大的 batch size 通常意味着

- 更高的单步 GPU 并行度
- 更稳定的梯度估计
- 更高的激活显存占用
- 每个 epoch 的 optimizer step 更少
- 可能需要重新调整学习率
- 可能影响泛化行为

较小 batch size 通常意味着

- 显存占用较低
- 梯度噪声更大
- optimizer step 更多
- GPU 可能无法充分利用
- BatchNorm 统计可能不稳定

因此不能仅根据 DataLoader 吞吐选择 batch size，还要考虑模型优化行为。PyTorch 数据加载优化教程也强调，batch size 与显存、梯度稳定性和学习率调节相互关联。

### 6.4.3 batch size 与有效 batch size

使用梯度累积时

```text
effective_batch_size
=
micro_batch_size
×
accumulation_steps
×
world_size
```

例如

```text
每卡 micro-batch = 8
梯度累积 = 4
GPU 数量 = 4

有效 batch size
= 8 × 4 × 4
= 128
```

但 DataLoader 的 `batch_size` 通常只表示当前进程一次读取的 micro-batch 大小。

### 6.4.4 `shuffle`

训练集通常设置

```python
shuffle=True
```

它会使样本每轮以随机顺序读取。

主要作用

- 减少固定样本顺序带来的偏差
- 避免相同类别长期连续出现
- 改善 mini-batch 的随机性
- 防止模型利用数据原始排序

验证集通常

```python
shuffle=False
```

因为

- 不需要通过随机顺序优化参数
- 固定顺序更便于复现和对应样本
- 某些评估需要保持原始次序

### 6.4.5 `shuffle` 与 Sampler

对于 Map-style Dataset，DataLoader 会根据 `shuffle` 自动选择或构建采样策略。

如果显式传入

```python
sampler=custom_sampler
```

就不应再同时设置

```python
shuffle=True
```

PyTorch DataLoader 文档规定，显式指定 `sampler` 时不能同时指定 `shuffle`。

错误

```python
loader = DataLoader(
    dataset,
    shuffle=True,
    sampler=custom_sampler,
)
```

正确

```python
loader = DataLoader(
    dataset,
    sampler=custom_sampler,
    shuffle=False,
)
```

或者省略 `shuffle`。

### 6.4.6 `num_workers`

```python
num_workers=0
```

表示主进程加载数据。

```python
num_workers=4
```

表示使用四个 worker 子进程。

增加 worker 主要用于并行化

- 磁盘读取
- 图片解码
- 音频解码
- 数据增强
- 文本解析
- NumPy CPU 计算

但 `num_workers` 不是越大越好。

### 6.4.7 worker 过少

可能表现为

- GPU 利用率周期性下降
- 每个训练 step 前等待数据
- DataLoader 时间接近或超过模型计算时间
- CPU 只有一个核心繁忙
- 数据增强串行执行

### 6.4.8 worker 过多

可能导致

- 进程切换开销增加
- CPU 竞争
- 内存占用增加
- `/dev/shm` 耗尽
- 文件句柄过多
- 存储随机读取压力增加
- 反而降低吞吐

PyTorch 官方优化教程建议根据具体任务逐步增加 worker，直到吞吐趋于平台；过多 worker 会增加 Dataset 副本、预取 Batch 和共享内存压力。

### 6.4.9 如何选择 num_workers

没有适用于所有机器的固定答案。

建议实验

```text
num_workers:
0 → 2 → 4 → 8 → 12
```

记录

- 每秒样本数
- 每个 epoch 时间
- GPU 利用率
- CPU 利用率
- 内存占用
- 数据等待时间

停止增加的信号

- 吞吐不再提升
- CPU 已接近饱和
- 内存明显增大
- I/O 已经达到上限
- 共享内存开始报错

## 6.5 `collate_fn` 的作用

### 6.5.1 为什么需要 collate

Dataset 返回的是单个样本

```python
sample = dataset[index]
```

DataLoader 需要把多个样本组合成 Batch

```text
sample 0
sample 1
sample 2
...
sample B-1
    ↓
collate_fn
    ↓
一个 Batch
```

`collate_fn` 的输入通常是样本列表

```python
[
    sample_0,
    sample_1,
    ...
]
```

输出是训练循环接收的 Batch。

PyTorch DataLoader 文档将 `collate_fn` 定义为将一组样本合并为 mini-batch 的函数。

### 6.5.2 默认 collate_fn 的行为

假设 Dataset 返回

```python
return image, label
```

其中

```text
image.shape = [C, H, W]
label       = int
```

默认 collate 后

```text
images.shape = [B, C, H, W]
labels.shape = [B]
```

默认行为主要包括

- 在最前面增加 Batch 维
- 将 NumPy 数组转换为 Tensor
- 将 Python 数值转换为 Tensor
- 保留字典、tuple、list 等容器结构
- 分别对相同字段进行批处理

### 6.5.3 默认 collate 的限制

默认 collate 通常要求同一字段形状一致。

例如变长序列

```text
sample 0 length = 10
sample 1 length = 15
sample 2 length = 8
```

无法直接执行

```python
torch.stack([
    sequence_0,
    sequence_1,
    sequence_2,
])
```

因为 shape 不同。

此时需要自定义 `collate_fn`

- Padding
- 截断
- Packed Sequence
- 返回 list
- 按长度排序
- 构造 attention mask

### 6.5.4 变长序列 Padding

```python
from __future__ import annotations

import torch
from torch.nn.utils.rnn import pad_sequence


def pad_collate_fn(
    batch: list[
        tuple[torch.Tensor, int]
    ],
) -> tuple[
    torch.Tensor,
    torch.Tensor,
    torch.Tensor,
]:
    sequences, labels = zip(*batch)

    lengths = torch.tensor(
        [
            sequence.shape[0]
            for sequence in sequences
        ],
        dtype=torch.long,
    )

    padded_sequences = pad_sequence(
        sequences,
        batch_first=True,
        padding_value=0.0,
    )

    label_tensor = torch.tensor(
        labels,
        dtype=torch.long,
    )

    return (
        padded_sequences,
        lengths,
        label_tensor,
    )
```

假设单个序列 shape 为

```text
[T, D]
```

则输出

```text
padded_sequences：
[B, T_max, D]

lengths：
[B]

labels：
[B]
```

DataLoader

```python
loader = DataLoader(
    dataset,
    batch_size=32,
    collate_fn=pad_collate_fn,
)
```

### 6.5.5 构造 Attention Mask

文本或序列模型中，可以在 collate 阶段构造 mask

```python
def sequence_collate_fn(batch):
    sequences, labels = zip(*batch)

    lengths = torch.tensor(
        [len(sequence) for sequence in sequences],
        dtype=torch.long,
    )

    padded = pad_sequence(
        sequences,
        batch_first=True,
        padding_value=0,
    )

    positions = torch.arange(
        padded.shape[1]
    ).unsqueeze(0)

    attention_mask = (
        positions < lengths.unsqueeze(1)
    )

    labels = torch.tensor(
        labels,
        dtype=torch.long,
    )

    return {
        "input_ids": padded,
        "attention_mask": attention_mask,
        "labels": labels,
    }
```

输出

```text
input_ids：
[B, T_max]

attention_mask：
[B, T_max]

labels：
[B]
```

### 6.5.6 为什么动态 Padding 更高效

如果整个数据集统一 Padding 到全局最大长度

```text
全局最大长度 = 2048
```

即使某个 Batch 内最长序列只有 200，也要处理大量 Padding。

动态 Padding 只 Padding 到当前 Batch 最大长度

```text
当前 Batch 最大长度 = 200
```

这样可以减少

- Batch 内存
- Attention 计算量
- GPU 显存
- 无效 token 计算

但不同 Batch 的 shape 会变化，可能影响

- 编译图复用
- Kernel 选择
- 显存碎片
- 吞吐稳定性

工程中常结合 length bucketing，使相近长度样本进入同一 Batch。

### 6.5.7 collate_fn 在哪个进程执行

当

```python
num_workers=0
```

时，collate 通常在主进程执行。

当

```python
num_workers>0
```

时，样本读取和 collate 通常在 worker 中执行，再把已组成的 Batch 发送给主进程。

因此，过重的 collate 逻辑可以通过多 worker 并行，但也可能增加

- 进程通信数据量
- 临时内存
- worker 计算负载

### 6.5.8 collate_fn 不应做什么

不建议在 `collate_fn` 中

- 执行模型 forward
- 将 Batch 固定移动到某张 GPU
- 每次创建大型 tokenizer
- 修改全局训练状态
- 依赖不可 pickle 的闭包
- 执行跨 Batch 的指标累计

collate 的核心职责仍是

```text
样本列表 → 合法 Batch
```

### 6.5.9 自定义 Batch 类型与 pin_memory

默认 pinning 逻辑能够识别 Tensor 以及由 Tensor 构成的常见映射和可迭代容器。

如果 `collate_fn` 返回自定义 Batch 类型，DataLoader 默认可能无法自动 pin 其中的 Tensor。此时可以为自定义类型实现 `pin_memory()`。

示例

```python
class CustomBatch:
    def __init__(
        self,
        samples,
    ) -> None:
        inputs, targets = zip(*samples)

        self.inputs = torch.stack(inputs)
        self.targets = torch.tensor(
            targets,
            dtype=torch.long,
        )

    def pin_memory(self):
        self.inputs = self.inputs.pin_memory()
        self.targets = self.targets.pin_memory()

        return self


def custom_collate_fn(samples):
    return CustomBatch(samples)
```

## 6.6 Sampler 与 DistributedSampler

### 6.6.1 Sampler 的作用

Sampler 决定

```text
本轮读取哪些样本
以及以什么顺序读取
```

它一般不负责读取样本内容，只产生索引。

```text
Sampler
输出：
3, 7, 1, 9, 0, ...

Dataset
根据索引读取：
dataset[3]
dataset[7]
dataset[1]
...
```

PyTorch 的 Sampler 基类要求子类实现 `__iter__()`，用于产生索引或一组索引，并可选实现 `__len__()`。

### 6.6.2 常见 Sampler

**SequentialSampler**

按顺序采样

```text
0, 1, 2, 3, ...
```

通常对应

```python
shuffle=False
```

**RandomSampler**

随机采样

```text
7, 1, 9, 0, ...
```

通常对应

```python
shuffle=True
```

**SubsetRandomSampler**

从给定索引子集中随机采样

```python
from torch.utils.data import (
    DataLoader,
    SubsetRandomSampler,
)

train_indices = [
    0, 2, 3, 8, 10, 12,
]

sampler = SubsetRandomSampler(
    train_indices
)

loader = DataLoader(
    dataset,
    batch_size=4,
    sampler=sampler,
)
```

### 6.6.3 WeightedRandomSampler

类别不平衡时，可以提高少数类样本被采到的概率

```python
from torch.utils.data import (
    WeightedRandomSampler,
)

sample_weights = torch.tensor([
    0.1,
    0.1,
    0.8,
    0.8,
])

sampler = WeightedRandomSampler(
    weights=sample_weights,
    num_samples=len(sample_weights),
    replacement=True,
)
```

PyTorch 的 `WeightedRandomSampler` 根据给定权重从样本索引中采样，权重不必预先归一化。

需要注意，传入的是

```text
每个样本的权重
```

而不直接是

```text
每个类别的权重
```

类别权重通常需要先映射成样本权重

```python
class_weights = torch.tensor([
    0.2,
    0.8,
])

sample_weights = class_weights[
    labels
]
```

### 6.6.4 重采样与加权 Loss 的区别

处理类别不平衡有两种常见策略。

**Weighted Sampler**

改变样本进入 Batch 的概率

```text
少数类样本被更多次采样
```

**Weighted Loss**

不改变数据分布，但改变样本或类别对 loss 的贡献

```text
少数类错误受到更大惩罚
```

二者作用位置不同

```text
Weighted Sampler：
数据层

Weighted Loss：
优化目标层
```

可以组合，但可能导致少数类被过度放大，需要通过验证集判断。

### 6.6.5 BatchSampler

普通 Sampler 每次产生一个索引

```text
3
7
1
9
```

BatchSampler 每次产生一组索引

```text
[3, 7, 1, 9]
[0, 2, 8, 5]
```

示例

```python
from torch.utils.data import (
    BatchSampler,
    RandomSampler,
)

sampler = RandomSampler(dataset)

batch_sampler = BatchSampler(
    sampler=sampler,
    batch_size=32,
    drop_last=False,
)

loader = DataLoader(
    dataset,
    batch_sampler=batch_sampler,
)
```

指定 `batch_sampler` 后，不能再同时指定

- `batch_size`
- `shuffle`
- `sampler`
- `drop_last`

PyTorch DataLoader 文档明确规定这些参数互斥。

### 6.6.6 自定义 Sampler

例如按序列长度升序采样

```python
from collections.abc import Iterator

import torch
from torch.utils.data import Sampler


class LengthSortedSampler(
    Sampler[int]
):
    def __init__(
        self,
        lengths: list[int],
    ) -> None:
        self.lengths = torch.tensor(
            lengths,
            dtype=torch.long,
        )

    def __iter__(self) -> Iterator[int]:
        sorted_indices = torch.argsort(
            self.lengths
        )

        yield from sorted_indices.tolist()

    def __len__(self) -> int:
        return len(self.lengths)
```

使用

```python
sampler = LengthSortedSampler(
    sequence_lengths
)

loader = DataLoader(
    dataset,
    batch_size=32,
    sampler=sampler,
)
```

但严格排序会降低训练样本随机性。实际中常使用

- 长度分桶
- 桶内随机
- 桶间随机
- 动态 Batch Size

### 6.6.7 DistributedSampler 的作用

在 DDP 中，每个进程通常对应一张 GPU，并各自创建一个 DataLoader。

如果每个进程都直接遍历完整 Dataset

```text
Rank 0：读取全部数据
Rank 1：读取全部数据
Rank 2：读取全部数据
Rank 3：读取全部数据
```

就会造成重复训练。

DistributedSampler 将 Dataset 划分给不同进程

```text
Rank 0：子集 0
Rank 1：子集 1
Rank 2：子集 2
Rank 3：子集 3
```

PyTorch 的 `DistributedSampler` 会限制每个进程只加载 Dataset 的一个子集，常与 DDP 配合使用。DDP 本身不会自动切分输入数据，因此数据分片需要由用户侧负责。

### 6.6.8 DistributedSampler 标准写法

```python
from torch.utils.data import (
    DataLoader,
    DistributedSampler,
)

sampler = DistributedSampler(
    dataset,
    num_replicas=world_size,
    rank=rank,
    shuffle=True,
    drop_last=False,
)

loader = DataLoader(
    dataset,
    batch_size=32,
    sampler=sampler,
    num_workers=4,
    pin_memory=True,
)
```

注意不再设置

```python
shuffle=True
```

因为打乱逻辑已经由 DistributedSampler 管理。

### 6.6.9 为什么每个 epoch 要调用 `set_epoch`

训练循环

```python
for epoch in range(num_epochs):
    sampler.set_epoch(epoch)

    for inputs, targets in loader:
        ...
```

`set_epoch(epoch)` 会让 DistributedSampler 在不同 epoch 使用不同随机排列。

如果不调用，多个 epoch 可能使用相同的打乱顺序。

PyTorch 官方文档明确要求，在分布式模式中，每个 epoch 创建 DataLoader iterator 前调用 `set_epoch()`，否则跨 epoch 的排序可能保持一致。

### 6.6.10 Dataset 数量不能整除进程数

假设

```text
Dataset 样本数：10
world_size：4
```

无法平均分成四份。

DistributedSampler 可以根据 `drop_last`

**`drop_last=True`**

丢弃尾部样本，使各 rank 样本数一致。

**`drop_last=False`**

添加额外索引，使各 rank 样本数一致。

这意味着在 `drop_last=False` 时，一轮中可能存在少量重复样本。

### 6.6.11 分布式中的 batch size

在 DDP 中

```python
DataLoader(
    batch_size=32
)
```

通常表示每个 rank 的本地 batch size。

如果

```text
world_size = 4
```

则不考虑梯度累积时

```text
global_batch_size
=
32 × 4
=
128
```

这会影响

- 学习率
- scheduler 总步数
- 每个 epoch 的 optimizer step
- 梯度统计

## 6.7 `pin_memory` 与数据拷贝优化

### 6.7.1 CPU 内存与 GPU 显存

模型训练通常需要把 Batch 从 CPU 复制到 GPU

```python
inputs = inputs.to("cuda")
```

数据路径为

```text
CPU 内存
   ↓
Host-to-Device 拷贝
   ↓
GPU 显存
```

普通 CPU 内存通常是 pageable memory，即操作系统可以将其页面换出或重新映射。

Pinned memory，也称 page-locked memory，是被锁定在物理内存中的区域。

### 6.7.2 为什么 pinned memory 有利于 H2D

GPU 执行 DMA 传输时，需要稳定的物理内存来源。

如果数据位于普通 pageable memory，运行时可能需要额外经过内部 staging 或固定内存步骤。

Pinned memory 能够更直接地用于 CPU 到 GPU 的数据传输，通常具有更高传输效率。PyTorch 官方说明，从 pinned memory 发起 Host-to-GPU 拷贝通常更快；DataLoader 设置 `pin_memory=True` 后，会自动把获取到的 Tensor 放到 pinned memory。

### 6.7.3 DataLoader 中使用 pin_memory

```python
loader = DataLoader(
    dataset,
    batch_size=64,
    num_workers=4,
    pin_memory=True,
)
```

训练

```python
for inputs, targets in loader:
    inputs = inputs.to(
        device,
        non_blocking=True,
    )

    targets = targets.to(
        device,
        non_blocking=True,
    )
```

推荐模式

```text
DataLoader：
pin_memory=True

Tensor 迁移：
to(device, non_blocking=True)
```

PyTorch 官方数据加载优化教程推荐由 DataLoader 完成 pinning，再在迁移到 GPU 时使用 `non_blocking=True`。

### 6.7.4 `non_blocking=True` 是什么

```python
inputs = inputs.to(
    "cuda",
    non_blocking=True,
)
```

表示在条件允许时，CPU 线程不必等待这次复制完全结束再返回。

但它不是无条件保证

```text
复制一定与计算完全重叠
```

要实现有效重叠，还取决于

- 源 Tensor 是否位于 pinned memory
- CUDA Stream
- 后续是否立即同步
- GPU 是否支持并发复制与计算
- 数据大小是否足够
- 当前 Stream 依赖关系

PyTorch 的 CUDA 语义文档说明，`non_blocking=True` 会在可能的情况下尝试相对于 Host 异步执行拷贝。

### 6.7.5 为什么不建议在主线程手动 pin

不推荐每个 Batch 都这样写

```python
inputs = inputs.pin_memory()
inputs = inputs.to(
    "cuda",
    non_blocking=True,
)
```

手动执行 `pin_memory()` 本身需要复制和锁页，且在 Python 主线程中通常是阻塞操作，可能抵消异步传输的收益。

PyTorch 官方教程指出，直接在主线程使用 `tensor.pin_memory().to(..., non_blocking=True)` 可能比普通迁移更慢；通常应让 DataLoader 管理 pinned memory。

### 6.7.6 pin_memory 是否一定加速

不一定

如果瓶颈主要在

- 图片解码
- CPU 数据增强
- 网络存储
- 小文件随机读取
- 模型计算
- 多卡通信

那么 H2D 传输占比可能很低，`pin_memory=True` 的提升不明显。

PyTorch 官方优化示例中也展示了 pin memory 收益可能较小，因为实际瓶颈可能由 CPU transform 主导。

### 6.7.7 pinned memory 也有成本

Pinned memory 不能被操作系统随意换出，使用过多可能影响

- 系统可用内存
- 其他进程
- 内存分配开销
- 操作系统整体响应

因此不应把整个超大 Dataset 全部长期 pin 住。

DataLoader 通常只对已加载和预取的 Batch 进行 pinning，更符合流水线使用方式。

### 6.7.8 自定义容器的 pin_memory

如果 `collate_fn` 返回

```python
{
    "input": tensor,
    "label": tensor,
}
```

默认 pinning 通常可以递归识别。

如果返回自定义对象

```python
CustomBatch(...)
```

则应实现

```python
def pin_memory(self):
    ...
    return self
```

否则 DataLoader 默认可能不会 pin 其中的 Tensor。

### 6.7.9 GPU Prefetch

普通流程

```text
等待 Batch
   ↓
H2D 拷贝
   ↓
GPU 计算
   ↓
等待下一 Batch
```

更高级的优化是

```text
当前 Batch 在 GPU 计算
        +
下一 Batch 通过另一个 Stream 传输
```

这样可以尝试重叠

```text
H2D Transfer
与
GPU Compute
```

但这需要正确处理

- CUDA Stream
- `wait_stream()`
- Tensor 生命周期
- `record_stream()`
- 异步错误
- 额外预取显存

PyTorch 官方数据加载优化教程给出了基于 CUDA Stream 的 DataPrefetcher 示例，同时说明只有当传输时间能够与计算有效重叠时，这种方式才有明显收益。

## 6.8 数据加载性能瓶颈分析

### 6.8.1 数据流水线的潜在瓶颈

一次 Batch 可能经过

```text
存储系统
   ↓
文件读取
   ↓
解码与解析
   ↓
数据增强
   ↓
样本合并
   ↓
进程间传输
   ↓
Pinned Memory
   ↓
CPU 到 GPU
   ↓
模型计算
```

任何阶段都可能成为瓶颈。

性能分析不能只修改

```python
num_workers
```

而应定位 GPU 为什么在等待。

### 6.8.2 常见瓶颈类型

**存储 I/O 瓶颈**

表现

- CPU 使用率不高
- 磁盘使用率很高
- worker 大量处于 I/O wait
- 更换更快存储后吞吐明显提高

常见原因

- HDD 随机读取
- 网络文件系统
- 大量小文件
- 数据压缩
- 远程对象存储
- 多 worker 同时争抢同一磁盘

优化方向

- 使用 SSD 或本地缓存
- 合并小文件
- 使用顺序读取格式
- 降低随机访问
- 提前建立索引
- 缓存常用元数据
- 适当减少 worker，避免磁盘抖动

**CPU 解码和增强瓶颈**

表现

- CPU 接近满载
- GPU 利用率周期性下降
- 增加 worker 后吞吐提高
- 关闭数据增强后速度明显提升

常见原因

- 图片 JPEG 解码
- 音频重采样
- 大量 Python 循环
- 复杂随机增强
- 文本分词
- NumPy 小操作频繁调用

优化方向

- 增加适量 worker
- 使用向量化操作
- 减少 Python 循环
- 使用更高效解码库
- 将可缓存的预处理离线完成
- 对 Batch 执行向量化增强
- 使用 `__getitems__()` 批量读取

**collate 瓶颈**

表现

- 单样本读取快
- 组成 Batch 时耗时长
- 变长数据 Padding 开销大
- 大量 Python 容器和小 Tensor

优化方向

- 减少过深的嵌套结构
- 避免大量小 Tensor
- 对序列进行长度分桶
- 使用向量化 Padding
- 把固定处理移出 collate
- 预分配或批量构造 Tensor

**进程间通信瓶颈**

表现

- 增加 worker 不再提升
- 大 Batch 或大量小 Tensor 时吞吐下降
- `/dev/shm` 报错
- 系统文件描述符不足
- worker 偶发退出

多进程 DataLoader 需要在 worker 与主进程之间传输 Batch，Tensor 通常涉及共享内存机制。PyTorch 多进程文档说明，Tensor 通过 multiprocessing queue 时需要转移到共享内存。

优化方向

- 减少 Batch 中的小对象数量
- 合并小 Tensor
- 降低 `num_workers`
- 降低 `prefetch_factor`
- 减小 batch size
- 增加容器共享内存
- 检查文件描述符限制

**H2D 拷贝瓶颈**

表现

- CPU Batch 已经准备好
- GPU 计算前仍有明显传输等待
- 输入 Tensor 较大
- 模型本身计算较轻

优化方向

- `pin_memory=True`
- `.to(device, non_blocking=True)`
- 减少不必要字段
- 降低输入精度或尺寸
- 重叠数据传输与计算
- 避免训练循环中频繁 `.cpu()`

**模型计算瓶颈**

如果 DataLoader 已经能提前准备多个 Batch，而 GPU 持续高利用率，则瓶颈可能不在数据加载。

此时继续增加 worker 通常没有收益，只会增加 CPU 和内存开销。

### 6.8.3 如何判断 GPU 是否在等待数据

一种简单方法是分别测量

```text
data_time：
获得下一个 Batch 的等待时间

compute_time：
模型前向、反向和更新耗时
```

示例

```python
import time

import torch


def train_with_timing(
    model,
    loader,
    criterion,
    optimizer,
    device,
):
    model.train()

    end_time = time.perf_counter()

    for step, (inputs, targets) in enumerate(
        loader
    ):
        data_time = (
            time.perf_counter()
            - end_time
        )

        inputs = inputs.to(
            device,
            non_blocking=True,
        )
        targets = targets.to(
            device,
            non_blocking=True,
        )

        if device.type == "cuda":
            torch.cuda.synchronize()

        compute_start = time.perf_counter()

        optimizer.zero_grad(
            set_to_none=True
        )

        outputs = model(inputs)
        loss = criterion(
            outputs,
            targets,
        )

        loss.backward()
        optimizer.step()

        if device.type == "cuda":
            torch.cuda.synchronize()

        compute_time = (
            time.perf_counter()
            - compute_start
        )

        if step % 100 == 0:
            print(
                f"step={step}, "
                f"data_time={data_time:.4f}s, "
                f"compute_time={compute_time:.4f}s"
            )

        end_time = time.perf_counter()
```

这里使用同步是为了测量准确，但同步本身会影响流水线，因此只适合诊断，不建议长期保留在高性能训练循环中。

### 6.8.4 单独测试 DataLoader 吞吐

可以不运行模型，先测试数据加载

```python
import time


def benchmark_loader(
    loader,
    max_batches: int = 200,
) -> float:
    start = time.perf_counter()
    sample_count = 0

    for batch_index, batch in enumerate(loader):
        if isinstance(batch, (tuple, list)):
            inputs = batch[0]
        elif isinstance(batch, dict):
            inputs = batch["inputs"]
        else:
            inputs = batch

        sample_count += len(inputs)

        if batch_index + 1 >= max_batches:
            break

    elapsed = time.perf_counter() - start

    if elapsed <= 0:
        raise RuntimeError(
            "Invalid benchmark duration"
        )

    return sample_count / elapsed
```

测试配置

```text
num_workers = 0
num_workers = 2
num_workers = 4
num_workers = 8
```

然后比较

```text
samples / second
```

不要只测一个 Batch，因为第一批可能包含

- worker 启动
- 文件缓存冷启动
- 首次解码
- Python 模块导入
- pinned memory 线程初始化

### 6.8.5 `/dev/shm` 不足

Linux 容器中常见错误

```text
Unexpected bus error encountered in worker
```

或

```text
unable to open shared memory object
```

可能是共享内存不足。

多 worker 会在共享内存中传递 Batch，压力大致受以下因素影响

```text
num_workers
×
prefetch_factor
×
batch_size
×
单样本大小
```

优化方式

- 增大容器 `/dev/shm`
- 减少 `num_workers`
- 减少 `prefetch_factor`
- 减小 batch size
- 减少 Batch 中无用字段
- 减少大量小 Tensor

PyTorch 官方优化教程也将共享内存不足列为多进程数据加载中的常见问题，并建议从 worker 数、预取深度和 batch size 等方面降低压力。

### 6.8.6 Worker 随机性

每个 worker 需要具有适当的随机种子，否则可能产生重复增强。

PyTorch 为 worker 提供基础 seed，并可以通过

```python
torch.utils.data.get_worker_info()
```

获得

- worker id
- worker 数量
- worker seed
- 当前 worker 的 Dataset 副本

`get_worker_info()` 可用于 Dataset 分片或在 `worker_init_fn` 中初始化其他随机库。

示例

```python
import random

import numpy as np
import torch


def seed_worker(worker_id: int) -> None:
    worker_seed = (
        torch.initial_seed() % 2**32
    )

    np.random.seed(worker_seed)
    random.seed(worker_seed)


generator = torch.Generator()
generator.manual_seed(42)

loader = DataLoader(
    dataset,
    batch_size=32,
    shuffle=True,
    num_workers=4,
    worker_init_fn=seed_worker,
    generator=generator,
)
```

这仍不意味着整个训练一定完全确定性，因为模型算子、CUDA 和第三方库也可能存在非确定行为。

### 6.8.7 Worker 卡死或超时

可能原因

- 文件系统阻塞
- 数据库连接不可多进程共享
- Dataset 内部死锁
- 第三方解码库线程不安全
- worker 中无限循环
- 样本读取异常未正确处理
- 共享内存不足

调试建议

1. 先设置 `num_workers=0`
2. 确认单样本读取稳定
3. 逐步增加 worker
4. 给 DataLoader 设置合理 `timeout`
5. 检查 worker 错误日志
6. 避免在 fork 前初始化复杂线程池
7. 检查容器共享内存

### 6.8.8 数据顺序与长尾 Batch

如果不同样本处理时间差异很大，例如

```text
短文本：1 ms
长文本：50 ms
```

默认保持顺序返回时，某个慢 Batch 可能造成队头阻塞。

当前 DataLoader 提供 `in_order` 配置；关闭顺序约束可能降低部分慢 worker 导致的等待，但会影响复现性，并可能在类别不平衡时改变短期数据分布。PyTorch 文档对 `in_order=False` 给出了相应警告。

多数常规训练保持默认

```python
in_order=True
```

即可。

### 6.8.9 一个较完整的 DataLoader 配置

```python
import os
import random

import numpy as np
import torch
from torch.utils.data import DataLoader


def seed_worker(worker_id: int) -> None:
    worker_seed = (
        torch.initial_seed() % 2**32
    )

    np.random.seed(worker_seed)
    random.seed(worker_seed)


def build_train_loader(
    dataset,
    batch_size: int,
    use_cuda: bool,
) -> DataLoader:
    cpu_count = os.cpu_count() or 1

    # 这里只是保守初始值，
    # 最终仍需要通过 benchmark 调整。
    num_workers = min(
        4,
        cpu_count,
    )

    generator = torch.Generator()
    generator.manual_seed(42)

    return DataLoader(
        dataset,
        batch_size=batch_size,
        shuffle=True,
        num_workers=num_workers,
        pin_memory=use_cuda,
        drop_last=True,
        persistent_workers=(
            num_workers > 0
        ),
        prefetch_factor=(
            2
            if num_workers > 0
            else None
        ),
        worker_init_fn=seed_worker,
        generator=generator,
    )
```

注意

- `prefetch_factor` 通常只在 `num_workers > 0` 时有意义
- `drop_last=True` 是否合理取决于任务
- worker 数量必须实际测试
- 验证集通常不需要 shuffle
- DDP 下应使用 DistributedSampler，而不是 `shuffle=True`

### 6.8.10 分布式 DataLoader 标准配置

```python
from torch.utils.data import (
    DataLoader,
    DistributedSampler,
)


def build_distributed_loader(
    dataset,
    batch_size: int,
    rank: int,
    world_size: int,
    num_workers: int,
) -> tuple[
    DataLoader,
    DistributedSampler,
]:
    sampler = DistributedSampler(
        dataset,
        num_replicas=world_size,
        rank=rank,
        shuffle=True,
        drop_last=False,
    )

    loader = DataLoader(
        dataset,
        batch_size=batch_size,
        sampler=sampler,
        num_workers=num_workers,
        pin_memory=True,
        persistent_workers=(
            num_workers > 0
        ),
        drop_last=True,
    )

    return loader, sampler
```

训练

```python
for epoch in range(num_epochs):
    sampler.set_epoch(epoch)

    for inputs, targets in loader:
        inputs = inputs.to(
            device,
            non_blocking=True,
        )

        targets = targets.to(
            device,
            non_blocking=True,
        )

        ...
```

## 6.9 本章小节

```text
Dataset
定义单个样本如何读取
        ↓
Sampler
决定样本索引和顺序
        ↓
BatchSampler
将索引组成 Batch
        ↓
Worker
读取样本并执行预处理
        ↓
collate_fn
将样本列表合并为 Batch
        ↓
pin_memory
将 CPU Tensor 放入锁页内存
        ↓
主进程
获得 Batch
        ↓
to(device, non_blocking=True)
复制到 GPU
        ↓
模型训练
```

|         组件         |           输入            |     输出     |    主要职责     |
| :----------------: | :---------------------: | :--------: | :---------: |
|      Dataset       |         索引或迭代状态         |    单个样本    |    样本读取     |
|      Sampler       |       Dataset 信息        |    单个索引    |    样本顺序     |
|    BatchSampler    |       Sampler 索引        |    一组索引    |    索引分批     |
|     collate_fn     |          样本列表           |   Batch    | 组合与 Padding |
|     DataLoader     |       Dataset 和配置       | Batch 迭代器  |   调度完整流水线   |
| DistributedSampler | Dataset、rank、world size | 当前 rank 索引 |   分布式数据分片   |

|          参数          |         核心作用         |        常见风险        |
| :------------------: | :------------------: | :----------------: |
|     `batch_size`     |        每批样本数         |     OOM、优化行为变化     |
|      `shuffle`       |        随机样本顺序        |    与 sampler 冲突    |
|    `num_workers`     |      并行数据加载进程数       |   CPU、内存和 IPC 开销   |
|     `collate_fn`     |     自定义 Batch 组合     | Python 开销、shape 错误 |
|     `pin_memory`     | 使用 pinned CPU memory |       占用锁页内存       |
|  `prefetch_factor`   | 每 worker 预取 Batch 数  |    RAM 和共享内存增加     |
| `persistent_workers` |  跨 epoch 保留 worker   |     长期占用进程和内存      |
|     `drop_last`      |     丢弃不完整 Batch      |       损失部分样本       |
|      `sampler`       |       自定义采样顺序        |  不能与 shuffle 同时指定  |

完成本章后，应能够独立回答

1. Dataset 和 DataLoader 的职责有什么区别
2. Map-style Dataset 和 IterableDataset 有什么区别
3. `__len__()` 和 `__getitem__()` 分别做什么
4. Dataset 是否必须把数据全部加载到内存
5. `__getitems__()` 可以解决什么问题
6. DataLoader 生成一个 Batch 的完整流程是什么
7. `num_workers=0` 和大于 0 有什么区别
8. 多进程 DataLoader 中 Dataset 是否共享同一个对象
9. `batch_size` 如何影响显存、吞吐和梯度
10. `shuffle` 与 Sampler 为什么不能同时设置
11. `num_workers` 为什么不是越多越好
12. `collate_fn` 的输入和输出分别是什么
13. 如何处理变长序列
14. 动态 Padding 有什么优点和代价
15. Sampler 与 BatchSampler 有什么区别
16. WeightedRandomSampler 接收样本权重还是类别权重
17. DistributedSampler 为什么必要
18. DDP 是否会自动切分 Dataset
19. 为什么每个 epoch 要调用 `sampler.set_epoch(epoch)`
20. `pin_memory=True` 做了什么
21. `non_blocking=True` 是否一定实现异步传输
22. 为什么不建议每个 Batch 手动调用 `pin_memory()`
23. `prefetch_factor` 与内存占用有什么关系
24. `persistent_workers` 适合什么场景
25. 如何判断数据加载是否成为 GPU 瓶颈
26. `/dev/shm` 不足时如何排查
27. 如何为不同 worker 设置随机种子
28. 如何系统调优 DataLoader 吞吐

# 7 PyTorch 标准训练流程

一个完整的 PyTorch 训练任务，不只是执行

```python
loss.backward()
optimizer.step()
```

还需要正确组织

- 数据读取与设备迁移
- 模型训练和验证状态
- 前向传播与反向传播
- 损失和指标统计
- 学习率调度
- Checkpoint 保存与恢复
- 日志记录
- 随机种子与实验复现
- 异常检查和最佳模型管理

可以将标准训练系统概括为

```text
准备配置与随机种子
        ↓
构建 Dataset / DataLoader
        ↓
构建模型、Loss、Optimizer、Scheduler
        ↓
可选：恢复 Checkpoint
        ↓
循环执行多个 Epoch
    ├── 训练一个 Epoch
    ├── 验证一个 Epoch
    ├── 记录日志
    ├── 保存最新 Checkpoint
    └── 更新最佳模型
        ↓
加载最佳模型并测试
```

本章重点不是简单记住一个训练代码模板，而是理解每个步骤的职责、状态和调用顺序。

## 7.1 单机单卡训练流程

### 7.1.1 什么是单机单卡训练

单机单卡指

```text
一台计算机
    +
一张 GPU
    +
一个主要训练进程
```

模型参数、梯度、优化器状态和当前 Batch 通常都位于同一张 GPU 上。

这是最基础的训练形式，也是学习以下高级内容的前提

- AMP 混合精度
- 梯度累积
- 单机多卡 DDP
- FSDP
- 大模型训练
- 推理优化

如果单卡训练流程本身不正确，直接加入多卡或 AMP 只会让问题更加难以排查。

### 7.1.2 训练前的准备工作

标准训练开始前通常需要完成以下工作。

**第一步：准备配置**

例如

```python
config = {
    "seed": 42,
    "num_epochs": 20,
    "batch_size": 64,
    "learning_rate": 1e-3,
    "weight_decay": 1e-2,
    "num_classes": 10,
}
```

配置至少应记录

- 数据集和划分方式
- 模型结构
- Batch Size
- 优化器
- 学习率
- Weight Decay
- Scheduler
- Epoch 数量
- 随机种子
- 数据增强
- 损失函数

如果这些信息没有被记录，即使保留了模型权重，也很难准确复现实验。

**第二步：设置随机种子**

```python
set_seed(config["seed"])
```

随机种子影响

- 参数初始化
- 数据打乱
- Dropout
- 随机数据增强
- 数据集划分
- 采样器
- 部分 CUDA 算法

不过，固定种子不等于绝对复现，具体原因会在 7.7 节展开。

**第三步：选择设备**

```python
device = torch.device(
    "cuda"
    if torch.cuda.is_available()
    else "cpu"
)
```

然后将模型放到设备

```python
model = model.to(device)
```

DataLoader 通常仍返回 CPU Tensor，由训练循环逐 Batch 移动到 GPU

```python
inputs = inputs.to(
    device,
    non_blocking=True,
)
```

**第四步：构建数据加载器**

```python
train_loader = DataLoader(
    train_dataset,
    batch_size=config["batch_size"],
    shuffle=True,
    num_workers=4,
    pin_memory=(
        device.type == "cuda"
    ),
)

validation_loader = DataLoader(
    validation_dataset,
    batch_size=config["batch_size"],
    shuffle=False,
    num_workers=4,
    pin_memory=(
        device.type == "cuda"
    ),
)
```

常见约定

```text
训练集：
shuffle=True
可能包含随机数据增强

验证集：
shuffle=False
通常只执行确定性预处理
```

**第五步：构建训练组件**

```python
model = Classifier(...).to(device)

criterion = nn.CrossEntropyLoss()

optimizer = torch.optim.AdamW(
    model.parameters(),
    lr=config["learning_rate"],
    weight_decay=config["weight_decay"],
)

scheduler = (
    torch.optim.lr_scheduler.CosineAnnealingLR(
        optimizer,
        T_max=config["num_epochs"],
    )
)
```

各组件职责

|    组件     |    作用    |
| :-------: | :------: |
|   Model   | 根据输入产生预测 |
| Criterion |  计算优化目标  |
| Optimizer | 根据梯度更新参数 |
| Scheduler |  调整学习率   |

### 7.1.3 Epoch 与 Iteration

**Epoch**

一个 Epoch 通常表示训练集被完整遍历一次。

```text
训练集样本数：10 000
batch_size：100

一个 Epoch 约包含：
10 000 / 100 = 100 个 Iteration
```

**Iteration / Step**

一次 Iteration 通常处理一个 Batch，并完成一次

```text
forward
→ loss
→ backward
→ optimizer.step
```

如果使用梯度累积，则多个 micro-batch 才对应一次真正的 optimizer step。

因此还应区分

```text
batch step：
处理了多少个 Batch

optimizer step：
真正更新了多少次参数

global step：
日志系统中定义的全局计数
```

不要默认这三个计数始终相同。

### 7.1.4 标准 Epoch 循环

```python
best_validation_accuracy = 0.0

for epoch in range(num_epochs):
    train_metrics = train_one_epoch(
        model=model,
        loader=train_loader,
        criterion=criterion,
        optimizer=optimizer,
        device=device,
    )

    validation_metrics = evaluate(
        model=model,
        loader=validation_loader,
        criterion=criterion,
        device=device,
    )

    scheduler.step()

    if (
        validation_metrics["accuracy"]
        > best_validation_accuracy
    ):
        best_validation_accuracy = (
            validation_metrics["accuracy"]
        )

        save_checkpoint(...)
```

每个 Epoch 的职责可以概括为

```text
model.train()
执行训练，更新参数
        ↓
model.eval()
执行验证，不更新参数
        ↓
记录指标
        ↓
调整学习率
        ↓
保存 Checkpoint
```

### 7.1.5 一个最小单卡训练循环

```python
for epoch in range(num_epochs):
    model.train()

    for inputs, targets in train_loader:
        inputs = inputs.to(device)
        targets = targets.to(device)

        optimizer.zero_grad(
            set_to_none=True
        )

        logits = model(inputs)
        loss = criterion(
            logits,
            targets,
        )

        loss.backward()
        optimizer.step()

    model.eval()

    with torch.no_grad():
        for inputs, targets in validation_loader:
            inputs = inputs.to(device)
            targets = targets.to(device)

            logits = model(inputs)
            validation_loss = criterion(
                logits,
                targets,
            )
```

这段代码包含训练的基本骨架，但完整工程还需要

- 准确统计指标
- 检查 NaN
- 日志记录
- Scheduler
- Checkpoint
- 随机种子
- 异常恢复

### 7.1.6 请描述一个标准的 PyTorch 单卡训练流程。

**参考回答**

首先设置配置和随机种子，构建 Dataset、DataLoader、模型、损失函数、优化器和学习率调度器，并将模型移动到 GPU。

每个 Epoch 先调用 `model.train()`，逐 Batch 执行设备迁移、梯度清理、前向传播、损失计算、反向传播和参数更新。随后调用 `model.eval()`，在 `torch.no_grad()` 或 `torch.inference_mode()` 下执行验证。

每轮结束后统计指标、更新 Scheduler，并保存包含模型、优化器、调度器和训练进度的 Checkpoint。

**面试官追问**

- Epoch 和 optimizer step 有什么区别？
- Scheduler 应该在什么时候调用？
- 为什么验证集通常不 shuffle？
- 模型和数据为什么都要移动到 GPU？
- 单卡训练恢复时需要保存哪些状态？

**容易答错的地方**

标准训练不只是

```python
loss.backward()
optimizer.step()
```

还包括训练状态、验证、日志、Checkpoint 和复现管理。

## 7.2 前向传播、Loss 计算、反向传播、参数更新

一个训练 Iteration 的标准顺序是

```text
1. 清理旧梯度
2. 前向传播
3. 计算 Loss
4. 反向传播
5. 可选：处理梯度
6. 更新参数
7. 可选：更新 Scheduler
```

代码

```python
optimizer.zero_grad(set_to_none=True)

logits = model(inputs)
loss = criterion(logits, targets)

loss.backward()

torch.nn.utils.clip_grad_norm_(
    model.parameters(),
    max_norm=1.0,
)

optimizer.step()
```

### 7.2.1 清理旧梯度

```python
optimizer.zero_grad(
    set_to_none=True
)
```

PyTorch 梯度默认累积。如果不清理

```text
当前参数梯度
=
之前的梯度
+
当前 Batch 梯度
```

在普通独立 Batch 训练中，这通常是不正确的。

`set_to_none=True` 将参数梯度设为 `None`，避免把已有梯度 Tensor 全部写零，通常具有更清晰的“本轮尚未产生梯度”语义。

### 7.2.2 前向传播

```python
logits = model(inputs)
```

这个过程会

1. 调用 Module 的 `forward()`
2. 执行 Tensor 运算
3. 得到模型输出
4. 在梯度模式开启时建立 Autograd 计算图
5. 保存反向传播所需的部分中间结果

分类模型通常输出

```text
logits.shape = [B, C]
```

而不是直接输出最终类别索引。

不要在模型训练 forward 中直接使用

```python
predictions = logits.argmax(dim=1)
```

作为损失输入，因为 `argmax` 是离散操作，无法提供正常的分类梯度。

### 7.2.3 Loss 计算

```python
loss = criterion(
    logits,
    targets,
)
```

Loss 通常应为标量

```python
print(loss.shape)
# torch.Size([])
```

如果使用

```python
reduction="none"
```

得到逐样本或逐元素损失，就需要自行执行合理归约

```python
per_sample_loss = criterion(
    logits,
    targets,
)

loss = per_sample_loss.mean()
```

Loss 的尺度会影响

- 梯度量级
- 学习率适用范围
- 梯度裁剪触发频率
- 多任务损失权重
- 梯度累积逻辑

### 7.2.4 数值有效性检查

训练中应尽早检查

```python
if not torch.isfinite(loss):
    raise FloatingPointError(
        f"Non-finite loss detected: "
        f"{loss.detach().item()}"
    )
```

非有限 Loss 可能来自

- 输入包含 NaN 或 Inf
- 学习率过大
- 除零
- `log(0)`
- 指数溢出
- FP16 数值范围不足
- 标签或 Loss 使用错误
- 参数已经发散

不要简单跳过所有 NaN Batch，否则可能掩盖系统性错误。

### 7.2.5 反向传播

```python
loss.backward()
```

它会

- 从 Loss 节点开始反向遍历计算图
- 根据链式法则计算梯度
- 将梯度累积到叶子参数的 `.grad`
- 默认释放不再需要的反向中间状态

它不会

- 自动更新参数
- 自动清理旧梯度
- 自动调整学习率
- 自动保存模型

### 7.2.6 梯度检查与梯度裁剪

反向后可以检查梯度

```python
for name, parameter in model.named_parameters():
    if (
        parameter.requires_grad
        and parameter.grad is None
    ):
        print(
            f"No gradient: {name}"
        )
```

但 `.grad is None` 不一定总是错误。可能原因包括

- 参数未参与当前计算路径
- 条件分支没有执行该模块
- 参数被冻结
- 参数被有意排除
- 计算图被 `detach()` 截断

梯度裁剪应放在

```text
backward 之后
optimizer.step 之前
```

```python
gradient_norm = (
    torch.nn.utils.clip_grad_norm_(
        model.parameters(),
        max_norm=1.0,
        error_if_nonfinite=True,
    )
)
```

### 7.2.7 参数更新

```python
optimizer.step()
```

优化器会

1. 遍历其管理的参数
2. 读取 `parameter.grad`
3. 更新内部状态
4. 按优化算法修改参数值

例如 AdamW 还会维护

- 一阶矩
- 二阶矩
- step 计数
- 参数组超参数

优化器只会处理初始化时交给它的 Parameter。后来新注册或新解冻的参数，不一定自动加入已有优化器。

### 7.2.8 Scheduler 更新

Scheduler 的调用频率取决于策略。

**按 Epoch 调整**

```python
for epoch in range(num_epochs):
    train_one_epoch(...)
    validate(...)

    scheduler.step()
```

**按 Optimizer Step 调整**

```python
for batch in train_loader:
    loss.backward()
    optimizer.step()
    scheduler.step()
```

**根据验证指标调整**

```python
validation_loss = evaluate(...)

scheduler.step(
    validation_loss
)
```

典型代表是 `ReduceLROnPlateau`。

不能把所有 Scheduler 都机械地放在同一个位置。

### 7.2.9 为什么通常先 `optimizer.step()` 再 `scheduler.step()`

通常应写为

```python
optimizer.step()
scheduler.step()
```

其语义是

```text
使用当前学习率完成本次参数更新
        ↓
计算下一次参数更新应使用的学习率
```

如果顺序不正确，可能跳过调度器设计的第一个学习率值。

### 7.2.10 一个标准训练 Batch

```python
def train_step(
    model: nn.Module,
    inputs: torch.Tensor,
    targets: torch.Tensor,
    criterion: nn.Module,
    optimizer: torch.optim.Optimizer,
) -> dict[str, float]:
    optimizer.zero_grad(
        set_to_none=True
    )

    logits = model(inputs)

    loss = criterion(
        logits,
        targets,
    )

    if not torch.isfinite(loss):
        raise FloatingPointError(
            "The training loss is not finite."
        )

    loss.backward()

    gradient_norm = (
        torch.nn.utils.clip_grad_norm_(
            model.parameters(),
            max_norm=1.0,
            error_if_nonfinite=True,
        )
    )

    optimizer.step()

    predictions = logits.argmax(dim=1)

    correct = (
        predictions == targets
    ).sum().item()

    return {
        "loss": loss.detach().item(),
        "correct": float(correct),
        "gradient_norm": (
            gradient_norm.detach().item()
        ),
    }
```

注意指标计算应使用

```python
loss.detach()
logits.detach()
```

或放在无梯度环境中，避免日志代码意外保留计算图。

### 7.2.11 常见调用顺序错误

**错误一：忘记清梯度**

```python
loss.backward()
optimizer.step()
```

旧梯度会继续累积。

**错误二：先 step，再 backward**

```python
optimizer.step()
loss.backward()
```

当前参数更新时没有使用本轮梯度。

**错误三：先裁剪，再 backward**

```python
clip_grad_norm_(...)
loss.backward()
```

裁剪时本轮梯度尚未计算。

**错误四：CrossEntropyLoss 前手动 Softmax**

```python
probabilities = torch.softmax(
    logits,
    dim=1,
)

loss = criterion(
    probabilities,
    targets,
)
```

CrossEntropyLoss 应直接接收 logits。

**错误五：为了日志保存整个 Loss Tensor**

```python
loss_history.append(loss)
```

这样可能长期持有计算图。

应保存普通数值

```python
loss_history.append(
    loss.detach().item()
)
```

## 7.3 训练阶段与验证阶段的区别

训练和验证虽然都会执行前向传播，但目的和状态不同。

|      对比项      |       训练阶段       |      验证阶段      |
| :-----------: | :--------------: | :------------: |
|     模型模式      | `model.train()`  | `model.eval()` |
|    是否建立反向图    |        是         |      通常否       |
| 是否调用 backward |        是         |       否        |
|    是否更新参数     |        是         |       否        |
|    Dropout    |        开启        |       关闭       |
|   BatchNorm   | 使用 Batch 统计并更新状态 |     使用运行统计     |
|     数据增强      |      可随机增强       |    通常确定性预处理    |
|     数据打乱      |       通常开启       |      通常关闭      |
|     主要目的      |       优化参数       |     估计泛化能力     |

### 7.3.1 训练阶段

训练阶段需要

```python
model.train()
```

并开启梯度计算。

标准逻辑

```python
for inputs, targets in train_loader:
    optimizer.zero_grad(
        set_to_none=True
    )

    logits = model(inputs)
    loss = criterion(
        logits,
        targets,
    )

    loss.backward()
    optimizer.step()
```

训练数据可以包含随机增强，例如

- 随机裁剪
- 随机翻转
- 颜色扰动
- 随机遮挡
- 时间序列抖动
- 随机噪声

这些增强属于训练策略的一部分。

### 7.3.2 验证阶段

验证阶段用于估计模型在未参与参数更新的数据上的表现。

```python
model.eval()

with torch.no_grad():
    for inputs, targets in validation_loader:
        logits = model(inputs)
        loss = criterion(
            logits,
            targets,
        )
```

验证阶段不能执行

```python
optimizer.step()
```

也通常不执行

```python
loss.backward()
```

否则验证集会间接影响参数，导致评估结果失去意义。

### 7.3.3 为什么验证阶段仍然需要计算 Loss

验证 Loss 可以反映

- 模型对正确类别的置信程度
- 是否开始过拟合
- 学习率是否需要降低
- Early Stopping 条件
- 最佳 Checkpoint 选择

可能出现

```text
验证 Accuracy 不变
但验证 Loss 上升
```

这可能意味着预测类别尚未改变，但模型对错误样本变得更加自信。

因此只看 Accuracy 可能遗漏模型校准和过拟合趋势。

### 7.3.4 验证阶段的数据预处理

训练和验证的基础数据处理必须一致，例如

- 输入尺寸
- 通道顺序
- 标准化均值和方差
- Tokenizer
- 特征定义
- 标签映射

但随机增强通常不同。

例如图像任务

```text
训练：
RandomResizedCrop
RandomHorizontalFlip
Normalize

验证：
Resize
CenterCrop
Normalize
```

如果验证阶段仍包含随机增强，指标会产生额外波动，也更难进行实验比较。

### 7.3.5 验证集不能参与模型选择之外的反复调参吗

验证集本来就用于

- 选择模型
- 调整超参数
- Early Stopping
- 选择阈值

但如果围绕同一个验证集进行大量反复试验，研发过程可能逐渐对验证集过拟合。

所以严格实验通常还需要独立测试集

```text
训练集：
更新模型参数

验证集：
选择模型和超参数

测试集：
最终一次或少量次数报告性能
```

### 7.3.6 正确统计 Epoch Loss

假设不同 Batch 大小可能不同。

错误写法

```python
epoch_loss += loss.item()

epoch_loss /= len(loader)
```

如果最后一个 Batch 较小，而 Loss 是 Batch 均值，这种方法会让每个 Batch 权重相同，而不是每个样本权重相同。

更严格的样本均值

```python
total_loss = 0.0
total_samples = 0

for inputs, targets in loader:
    loss = criterion(
        model(inputs),
        targets,
    )

    batch_size = targets.shape[0]

    total_loss += (
        loss.item() * batch_size
    )
    total_samples += batch_size

epoch_loss = (
    total_loss / total_samples
)
```

但这要求 Loss 的 reduction 确实是按样本均值。如果 Loss 还对 token、像素等维度归约，则需根据任务重新定义统计分母。

### 7.3.7 训练指标与验证指标的关系

**训练 Loss 和验证 Loss 都下降**

模型仍在正常学习。

**训练 Loss 下降，验证 Loss 上升**

可能开始过拟合。

**训练和验证 Loss 都不下降**

可能是

- 学习率不合适
- 数据或标签错误
- 梯度未更新
- Loss 使用错误
- 模型容量不足
- 输入预处理错误

**训练指标很好，验证指标极差**

可能是

- 数据泄漏检查方向相反
- 训练和验证分布不一致
- 过拟合
- `eval()` 使用错误
- 验证预处理错误
- 指标实现错误

## 7.4 Checkpoint 保存与恢复训练

### 7.4.1 为什么需要 Checkpoint

Checkpoint 主要用于

- 训练中断后继续
- 保存当前最新状态
- 保存验证集最优状态
- 回退到历史版本
- 模型部署
- 实验比较
- 故障恢复

只保存最终模型权重，不足以支持完整恢复训练。

### 7.4.2 只保存模型权重

用于推理或模型发布时

```python
torch.save(
    model.state_dict(),
    "model_weights.pt",
)
```

加载

```python
model = Model(...)

state_dict = torch.load(
    "model_weights.pt",
    map_location="cpu",
    weights_only=True,
)

model.load_state_dict(
    state_dict
)

model.eval()
```

PyTorch 推荐保存模型的 `state_dict`，因为其更灵活，并避免将使用方式与某个序列化后的 Python 模型对象紧密绑定。

### 7.4.3 完整训练 Checkpoint 应保存什么

为了恢复训练，通常保存

```python
checkpoint = {
    "epoch": epoch,
    "global_step": global_step,
    "model_state_dict": (
        model.state_dict()
    ),
    "optimizer_state_dict": (
        optimizer.state_dict()
    ),
    "scheduler_state_dict": (
        scheduler.state_dict()
        if scheduler is not None
        else None
    ),
    "best_metric": best_metric,
    "config": config,
}
```

还可能包括

- AMP GradScaler
- Early Stopping 状态
- 随机数生成器状态
- 当前数据采样进度
- 指标历史
- 代码版本
- 数据版本

PyTorch 官方教程明确指出，恢复训练时不能只保存模型权重，还应保存优化器状态及其他恢复所需信息。

### 7.4.4 为什么要保存优化器状态

Momentum SGD 需要保存

```text
Momentum Buffer
```

Adam/AdamW 需要保存

- 一阶矩
- 二阶矩
- step
- 参数组配置

如果只恢复模型参数，却重新创建空优化器

```text
参数位置恢复了
但优化器“速度”和历史统计丢失
```

训练轨迹会发生变化，甚至可能在恢复点附近产生明显震荡。

### 7.4.5 为什么要保存 Scheduler 状态

Scheduler 可能保存

- 当前调度步数
- 上一次 Epoch
- 当前阶段
- Warmup 进度
- Cosine 进度
- Plateau 最佳指标
- patience 计数

如果不恢复 Scheduler，学习率可能错误地重新从起点开始。

例如模型已经训练到 Cosine 后期，但恢复后 Scheduler 又使用初始阶段学习率，就可能破坏已有参数。

### 7.4.6 保存 Checkpoint

```python
from pathlib import Path
from typing import Any

import torch


def save_checkpoint(
    path: str | Path,
    *,
    model: torch.nn.Module,
    optimizer: torch.optim.Optimizer,
    scheduler,
    epoch: int,
    global_step: int,
    best_metric: float,
    config: dict[str, Any],
) -> None:
    path = Path(path)
    path.parent.mkdir(
        parents=True,
        exist_ok=True,
    )

    checkpoint = {
        "epoch": epoch,
        "global_step": global_step,
        "model_state_dict": (
            model.state_dict()
        ),
        "optimizer_state_dict": (
            optimizer.state_dict()
        ),
        "scheduler_state_dict": (
            scheduler.state_dict()
            if scheduler is not None
            else None
        ),
        "best_metric": best_metric,
        "config": config,
    }

    temporary_path = path.with_suffix(
        path.suffix + ".tmp"
    )

    torch.save(
        checkpoint,
        temporary_path,
    )

    temporary_path.replace(path)
```

先写临时文件再替换，可以降低程序在保存中途异常时留下半个 Checkpoint 的风险。

### 7.4.7 加载 Checkpoint

```python
def load_checkpoint(
    path: str | Path,
    *,
    model: torch.nn.Module,
    optimizer: (
        torch.optim.Optimizer | None
    ) = None,
    scheduler=None,
    device: torch.device,
) -> dict:
    checkpoint = torch.load(
        path,
        map_location=device,
        weights_only=True,
    )

    model.load_state_dict(
        checkpoint["model_state_dict"]
    )

    if (
        scheduler is not None
        and checkpoint[
            "scheduler_state_dict"
        ] is not None
    ):
        scheduler.load_state_dict(
            checkpoint[
                "scheduler_state_dict"
            ]
        )

    if optimizer is not None:
        optimizer.load_state_dict(
            checkpoint[
                "optimizer_state_dict"
            ]
        )

    return checkpoint
```

当前 PyTorch 文档提醒：恢复 Scheduler 和 Optimizer 时，需要注意初始化及加载顺序，避免优化器加载的学习率被 Scheduler 初始化覆盖；官方 Optimizer 文档示例采用先初始化并加载 Scheduler，再加载 Optimizer 状态。

### 7.4.8 恢复训练进度

```python
checkpoint = load_checkpoint(...)

start_epoch = (
    checkpoint["epoch"] + 1
)

global_step = checkpoint[
    "global_step"
]

best_metric = checkpoint[
    "best_metric"
]
```

训练

```python
for epoch in range(
    start_epoch,
    num_epochs,
):
    ...
```

保存的 Epoch 语义必须统一。

例如约定

```text
checkpoint["epoch"]
=
已经完整完成的最后一个 Epoch
```

则恢复时从

```python
epoch + 1
```

开始。

### 7.4.9 恢复优化器状态后的设备问题

通常模型和优化器按正常顺序创建并加载后，优化器状态会与加载内容关联。

但在跨设备、复杂参数重建或特殊加载方式下，应检查优化器状态 Tensor 的设备。

可以编写

```python
def move_optimizer_state(
    optimizer: torch.optim.Optimizer,
    device: torch.device,
) -> None:
    for state in optimizer.state.values():
        for key, value in state.items():
            if isinstance(
                value,
                torch.Tensor,
            ):
                state[key] = value.to(
                    device
                )
```

不过不要无条件调用并掩盖模型参数和优化器创建顺序错误。

### 7.4.10 最新模型与最佳模型

建议分别保存

```text
last.pt：
最近一次完成的训练状态
用于故障恢复

best.pt：
验证指标最优的模型状态
用于最终测试或部署
```

示例

```python
save_checkpoint(
    "checkpoints/last.pt",
    ...
)

if validation_accuracy > best_accuracy:
    best_accuracy = validation_accuracy

    save_checkpoint(
        "checkpoints/best.pt",
        ...
    )
```

二者不能混为一谈。

训练最后一轮的模型不一定是验证集最优模型。

### 7.4.11 按 Loss 还是按 Metric 保存最佳模型

取决于任务目标。

例如

```text
分类任务：
可能按 Macro-F1 保存

回归任务：
可能按 MAE 保存

概率建模：
可能按验证 NLL 保存
```

不应只因为训练中使用 CrossEntropyLoss，就默认最佳模型必须按验证 Cross Entropy 选择。

最佳 Checkpoint 应与最终关注指标保持一致。

### 7.4.12 RNG 状态与精确恢复

如果希望恢复后尽量延续相同随机序列，还可以保存

```python
import random

import numpy as np
import torch


def capture_rng_state() -> dict:
    state = {
        "python": random.getstate(),
        "numpy": np.random.get_state(),
        "torch_cpu": torch.get_rng_state(),
    }

    if torch.cuda.is_available():
        state["torch_cuda"] = (
            torch.cuda.get_rng_state_all()
        )

    return state
```

恢复

```python
def restore_rng_state(
    state: dict,
) -> None:
    random.setstate(
        state["python"]
    )

    np.random.set_state(
        state["numpy"]
    )

    torch.set_rng_state(
        state["torch_cpu"]
    )

    if (
        torch.cuda.is_available()
        and "torch_cuda" in state
    ):
        torch.cuda.set_rng_state_all(
            state["torch_cuda"]
        )
```

PyTorch 提供 CPU RNG 状态以及全部 CUDA 设备 RNG 状态的读取和恢复接口。

但即使保存 RNG 状态，也可能因为

- DataLoader 预取
- worker 进度
- 断点发生在 Epoch 中间
- 非确定性算子
- 外部随机库

而无法做到完全一致的中间 Batch 恢复。

### 7.4.13 `torch.load` 的安全问题

`torch.load()` 底层涉及反序列化。不要加载来源不可信的 Checkpoint。

现代 PyTorch 默认倾向使用

```python
weights_only=True
```

限制反序列化范围，更适合由普通 Tensor、基础类型和字典组成的 state dict。自 PyTorch 2.6 起，在未显式传入自定义 `pickle_module` 时，`torch.load` 默认使用 `weights_only=True`。

如果 Checkpoint 中保存了自定义 Python 类对象，受限加载可能失败。

更稳健的做法是将配置保存为

```python
{
    "model_name": "mlp",
    "hidden_dim": 256,
}
```

而不是直接保存复杂自定义类实例。

## 7.5 日志记录与实验管理

### 7.5.1 为什么需要日志

如果训练只打印

```text
Epoch 10 completed
```

很难回答

- Loss 是否稳定下降
- 学习率何时变化
- 梯度是否爆炸
- 是否开始过拟合
- 哪个配置产生了最佳结果
- 两次实验到底差在哪里

日志系统的目标是记录

```text
发生了什么
在什么时候发生
对应什么配置
得到什么结果
```

### 7.5.2 应记录哪些训练指标

每个 Epoch 常见记录

- Train Loss
- Validation Loss
- Train Metric
- Validation Metric
- Learning Rate
- Epoch 耗时
- 全局 Step
- 最佳指标
- GPU 显存
- 梯度范数

按 Step 可选记录

- 当前 Loss
- Learning Rate
- Gradient Norm
- 吞吐
- Data Time
- Compute Time
- Tokens/s 或 Samples/s

不应记录过于频繁的重型内容，例如每一步都保存

- 全部参数直方图
- 全部梯度
- 大量图像
- 完整预测结果

这会显著影响训练性能和磁盘占用。

### 7.5.3 Step 日志与 Epoch 日志

**Step 日志**

适合观察

- Loss 突变
- 学习率 Warmup
- 梯度爆炸
- 性能波动

但噪声较大。

**Epoch 日志**

适合比较

- 训练趋势
- 验证性能
- 过拟合
- 最佳模型

较合理的策略是

```text
训练 Step：
每 50 或 100 Step 记录一次

验证：
每个 Epoch 记录一次
```

### 7.5.4 正确计算平均 Loss

一个通用的累加器

```python
class AverageMeter:
    def __init__(self) -> None:
        self.reset()

    def reset(self) -> None:
        self.total = 0.0
        self.count = 0

    def update(
        self,
        value: float,
        weight: int = 1,
    ) -> None:
        self.total += value * weight
        self.count += weight

    @property
    def average(self) -> float:
        if self.count == 0:
            return 0.0

        return self.total / self.count
```

使用

```python
loss_meter = AverageMeter()

for inputs, targets in loader:
    loss = ...

    loss_meter.update(
        loss.detach().item(),
        weight=targets.shape[0],
    )
```

需要确认 `weight` 的含义。

对于语言模型，更合理的权重可能是

```text
有效 token 数
```

而不是样本数。

### 7.5.5 Python Logging

相比大量 `print()`，工程中可以使用

```python
import logging


def build_logger() -> logging.Logger:
    logger = logging.getLogger(
        "training"
    )

    logger.setLevel(logging.INFO)

    if not logger.handlers:
        formatter = logging.Formatter(
            "%(asctime)s | "
            "%(levelname)s | "
            "%(message)s"
        )

        stream_handler = (
            logging.StreamHandler()
        )
        stream_handler.setFormatter(
            formatter
        )

        logger.addHandler(
            stream_handler
        )

    return logger
```

使用

```python
logger.info(
    "epoch=%d train_loss=%.4f "
    "val_loss=%.4f val_acc=%.4f",
    epoch,
    train_loss,
    validation_loss,
    validation_accuracy,
)
```

优点包括

- 日志级别
- 时间戳
- 文件和终端输出
- 统一格式
- 更便于检索

### 7.5.6 TensorBoard

PyTorch 提供

```python
from torch.utils.tensorboard import (
    SummaryWriter,
)
```

用于写入 TensorBoard 事件日志。`SummaryWriter` 可以记录标量、图像、直方图、模型图等信息。

示例

```python
writer = SummaryWriter(
    log_dir="runs/experiment_001"
)

writer.add_scalar(
    "loss/train",
    train_loss,
    epoch,
)

writer.add_scalar(
    "loss/validation",
    validation_loss,
    epoch,
)

writer.add_scalar(
    "metric/validation_accuracy",
    validation_accuracy,
    epoch,
)

writer.add_scalar(
    "optimization/learning_rate",
    optimizer.param_groups[0]["lr"],
    epoch,
)

writer.close()
```

合理的 Tag 层级

```text
loss/train
loss/validation

accuracy/train
accuracy/validation

optimization/learning_rate
optimization/gradient_norm
```

TensorBoard 可以根据层级对图表进行组织。

### 7.5.7 第三方实验管理工具

实际项目还可能使用

- Weights & Biases
- MLflow
- ClearML
- 自建实验平台

其核心能力通常包括

- 参数记录
- 指标曲线
- Artifact 管理
- Checkpoint 上传
- 运行对比
- 团队共享
- 超参数搜索

无论使用什么工具，底层实验信息仍应保持明确和可导出，避免项目完全依赖某个平台。

### 7.5.8 实验目录设计

推荐每次运行使用独立目录

```text
experiments/
└── 20260803_182400_resnet18_seed42/
    ├── config.json
    ├── train.log
    ├── metrics.jsonl
    ├── checkpoints/
    │   ├── last.pt
    │   └── best.pt
    ├── tensorboard/
    └── environment.txt
```

应避免所有实验都覆盖

```text
model.pt
train.log
```

### 7.5.9 配置快照

训练开始时保存

```python
import json
from pathlib import Path


def save_config(
    config: dict,
    path: str | Path,
) -> None:
    with open(
        path,
        "w",
        encoding="utf-8",
    ) as file:
        json.dump(
            config,
            file,
            indent=2,
            ensure_ascii=False,
        )
```

不要只记录“最终配置”。

还应明确

- 默认值
- 命令行覆盖值
- 环境变量
- 自动推导值
- 实际使用的 Batch Size

### 7.5.10 环境信息

为了复现，应记录

```python
environment = {
    "torch_version": torch.__version__,
    "cuda_version": torch.version.cuda,
    "device": str(device),
    "device_name": (
        torch.cuda.get_device_name(0)
        if torch.cuda.is_available()
        else "cpu"
    ),
}
```

还可以记录

- Python 版本
- 操作系统
- CUDA Driver
- cuDNN
- Git Commit
- 是否存在未提交修改
- 数据版本
- 依赖包版本

### 7.5.11 JSON Lines 指标记录

除可视化平台外，建议保留简单、可解析的指标文件

```python
import json


def append_metrics(
    path,
    metrics: dict,
) -> None:
    with open(
        path,
        "a",
        encoding="utf-8",
    ) as file:
        file.write(
            json.dumps(
                metrics,
                ensure_ascii=False,
            )
            + "\n"
        )
```

每行一条记录

```json
{"epoch": 1, "train_loss": 0.82, "val_accuracy": 0.76}
{"epoch": 2, "train_loss": 0.61, "val_accuracy": 0.81}
```

这种格式

- 易于追加
- 单行损坏不影响全部文件
- 易于用 Python、Shell 和数据工具读取
- 不依赖特定实验平台

### 7.5.12 日志代码不要破坏训练图

错误

```python
logged_losses.append(loss)
```

这会保存仍连接计算图的 Tensor。

正确

```python
logged_losses.append(
    loss.detach().item()
)
```

记录预测结果时同样应考虑

```python
predictions = (
    logits.detach()
    .cpu()
    .numpy()
)
```

不要在每个 Step 无条件将大型输出复制到 CPU，否则可能引入频繁同步和性能开销。
### 7.5.13 一个训练实验应该记录哪些信息？

**参考回答**

至少应记录完整配置、训练和验证 Loss、核心评价指标、学习率、Epoch 与 global step、最佳指标、Checkpoint 路径和运行环境。

为了准确比较实验，还应保存随机种子、数据版本、代码版本、依赖环境和实际使用的模型配置。日志中的 Tensor 应先 detach，避免长期保留计算图。

**面试官追问**

- 为什么不能只记录最终 Accuracy？
- 为什么不能把 Loss Tensor 直接放进列表？
- Step 日志与 Epoch 日志分别适合观察什么？
- 如何避免多次实验互相覆盖？
- 为什么需要记录 Git Commit 和数据版本？

**容易答错的地方**

实验管理不只是画 Loss 曲线。配置、代码、数据、环境和 Checkpoint 必须形成可追踪关系。

## 7.6 随机种子与实验复现

### 7.6.1 什么是实验复现

复现可以分成不同层级。

**结果级复现**

多次训练得到相近指标

```text
Run 1：84.2%
Run 2：84.5%
Run 3：84.1%
```

**轨迹级复现**

每个 Epoch 的 Loss 和指标基本一致。

**逐步复现**

每个 Batch、每次参数更新和最终权重完全一致。

越严格的复现，代价越高，也越依赖软硬件环境。

PyTorch 官方指出，即使使用相同种子，也不能保证跨 PyTorch 版本、不同平台或 CPU/GPU 环境完全复现；确定性配置通常还可能降低性能。

### 7.6.2 随机性的来源

常见随机来源包括

- Python `random`
- NumPy
- PyTorch CPU RNG
- PyTorch CUDA RNG
- 参数初始化
- Dropout
- DataLoader Shuffle
- Worker 随机增强
- 数据集划分
- Sampler
- 某些 CUDA 算法
- 第三方库
- 多线程执行顺序

因此，只调用

```python
torch.manual_seed(42)
```

通常还不够。

### 7.6.3 基础随机种子函数

```python
from __future__ import annotations

import os
import random

import numpy as np
import torch


def set_seed(
    seed: int,
    *,
    deterministic: bool = False,
) -> None:
    if seed < 0:
        raise ValueError(
            "seed must be non-negative"
        )

    os.environ["PYTHONHASHSEED"] = str(
        seed
    )

    random.seed(seed)
    np.random.seed(seed)

    torch.manual_seed(seed)

    if torch.cuda.is_available():
        torch.cuda.manual_seed_all(seed)

    if deterministic:
        torch.backends.cudnn.benchmark = (
            False
        )

        torch.use_deterministic_algorithms(
            True
        )
```

`torch.manual_seed()` 会为 PyTorch RNG 设置种子；对于多 GPU，也常显式调用 `torch.cuda.manual_seed_all()` 以表达意图。

### 7.6.4 `PYTHONHASHSEED`

Python 中部分哈希相关容器顺序可能受哈希随机化影响。

设置

```python
os.environ["PYTHONHASHSEED"] = str(seed)
```

通常应在 Python 进程启动前通过环境变量设置，运行时再设置不一定能追溯性地改变已经初始化的哈希行为。

例如命令行

```bash
PYTHONHASHSEED=42 python train.py
```

不要认为在程序运行中途修改该变量能重新初始化整个解释器状态。

### 7.6.5 cuDNN Benchmark

```python
torch.backends.cudnn.benchmark = True
```

允许 cuDNN 根据输入 shape 测试并选择更快的卷积算法。

适合

- 输入 shape 基本固定
- 更重视性能
- 不要求严格复现

但基准噪声可能导致不同运行选择不同算法。

严格复现时通常设置

```python
torch.backends.cudnn.benchmark = False
```

PyTorch 官方说明，关闭 benchmark 可以避免由于基准测试噪声造成的算法选择变化，但可能降低性能。

### 7.6.6 确定性算法

```python
torch.use_deterministic_algorithms(
    True
)
```

该设置要求相关操作使用确定性算法；如果某个操作没有确定性实现，可能直接报错。

它的价值是

- 尽早发现非确定性算子
- 提高调试时重复运行的一致性
- 避免静默使用已知非确定性实现

代价可能包括

- 运行速度降低
- 额外内存
- 某些算子不可用
- 算法选择受限

### 7.6.7 `cudnn.deterministic`

还可以看到

```python
torch.backends.cudnn.deterministic = True
```

它主要影响 cuDNN 算法选择。

相比之下

```python
torch.use_deterministic_algorithms(
    True
)
```

覆盖范围更广，并可在遇到不支持确定性的操作时抛出错误。

`cudnn.benchmark=False` 与 `cudnn.deterministic=True` 也不是同一个设置：前者控制算法搜索和选择稳定性，后者约束所选算法本身的确定性。

### 7.6.8 DataLoader Worker 的随机种子

多进程 DataLoader 中，每个 worker 需要获得不同但可复现的随机状态。

标准写法

```python
import random

import numpy as np
import torch
from torch.utils.data import DataLoader


def seed_worker(
    worker_id: int,
) -> None:
    worker_seed = (
        torch.initial_seed() % 2**32
    )

    np.random.seed(worker_seed)
    random.seed(worker_seed)


generator = torch.Generator()
generator.manual_seed(42)

train_loader = DataLoader(
    train_dataset,
    batch_size=64,
    shuffle=True,
    num_workers=4,
    worker_init_fn=seed_worker,
    generator=generator,
)
```

PyTorch 官方复现指南推荐使用 `worker_init_fn` 和显式 `Generator`，为 DataLoader worker 中的 NumPy 和 Python 随机源设置可复现种子。

### 7.6.9 数据集划分也要固定

错误

```python
train_dataset, validation_dataset = (
    random_split(
        dataset,
        [8000, 2000],
    )
)
```

如果未控制 Generator，每次划分可能不同。

正确

```python
split_generator = torch.Generator()
split_generator.manual_seed(42)

train_dataset, validation_dataset = (
    torch.utils.data.random_split(
        dataset,
        [8000, 2000],
        generator=split_generator,
    )
)
```

更严格的工程中，应直接保存

- Train IDs
- Validation IDs
- Test IDs

这样即使后续代码或随机过程改变，数据划分仍保持不变。

### 7.6.10 为什么相同种子仍可能得到不同结果

可能原因包括

1. 使用了非确定性 CUDA 算子
2. PyTorch、CUDA 或 cuDNN 版本不同
3. GPU 型号不同
4. 多线程归约顺序不同
5. 浮点加法顺序不同
6. DataLoader Worker 数变化
7. 随机调用次数变化
8. 数据文件顺序变化
9. 第三方库没有设置种子
10. 恢复训练时没有保存 RNG 状态

浮点运算不满足严格结合律

```text
(a + b) + c
不一定逐位等于
a + (b + c)
```

并行归约顺序发生变化时，细微误差可能逐渐积累。

### 7.6.11 改变 num_workers 会影响结果吗

可能会。

虽然 DataLoader 可以为 worker 设置种子，但改变

```text
num_workers
```

可能改变

- 样本预取顺序
- 随机增强在哪个 worker 执行
- 随机数消费顺序
- IterableDataset 分片方式
- Batch 到达时间

因此，严格比较实验时，应保持 DataLoader 配置一致。

### 7.6.12 随机种子不是越固定越好

在模型研究中，不能只报告一个“幸运种子”的结果。

更可靠的评估方式是运行多个种子

```text
seed = 1
seed = 2
seed = 3
seed = 4
seed = 5
```

然后报告

```text
mean ± standard deviation
```

固定种子的主要价值是

- 调试
- 公平消融
- 定位代码变化
- 重复具体实验

而不是证明模型对随机性稳定。

### 7.6.13 性能模式与复现模式

可以将配置明确分成两种。

**调试与复现模式**

```python
torch.backends.cudnn.benchmark = False

torch.use_deterministic_algorithms(
    True
)
```

特点

- 更容易重复
- 更容易定位差异
- 可能更慢
- 某些操作可能报错

**性能模式**

```python
torch.backends.cudnn.benchmark = True
```

并不强制所有算法确定。

特点

- 可能获得更高性能
- 逐位复现能力下降
- 更适合正式大规模训练

不能一边要求绝对确定性，一边默认所有最快算法和异步优化都不受限制。

### 7.6.14 完整复现需要记录什么

```text
代码
├── Git Commit
└── 未提交修改

配置
├── 模型
├── 优化器
├── Scheduler
├── Batch Size
└── 随机种子

数据
├── 数据版本
├── 划分 ID
└── 预处理版本

环境
├── Python
├── PyTorch
├── CUDA
├── cuDNN
├── GPU
└── 依赖版本

运行状态
├── Checkpoint
├── Optimizer State
├── Scheduler State
├── RNG State
└── Global Step
```

只保存一个 seed 数字远远不够。

## 7.7 完整单机单卡训练模板

下面给出一个结构相对完整、可复用的分类任务训练模板。

```python
from __future__ import annotations

import json
import logging
import os
import random
import time
from pathlib import Path
from typing import Any

import numpy as np
import torch
from torch import nn
from torch.utils.data import DataLoader


class AverageMeter:
    def __init__(self) -> None:
        self.reset()

    def reset(self) -> None:
        self.total = 0.0
        self.count = 0

    def update(
        self,
        value: float,
        weight: int,
    ) -> None:
        self.total += value * weight
        self.count += weight

    @property
    def average(self) -> float:
        if self.count == 0:
            return 0.0

        return self.total / self.count


def set_seed(
    seed: int,
    *,
    deterministic: bool,
) -> None:
    random.seed(seed)
    np.random.seed(seed)
    torch.manual_seed(seed)

    if torch.cuda.is_available():
        torch.cuda.manual_seed_all(seed)

    if deterministic:
        torch.backends.cudnn.benchmark = (
            False
        )

        torch.use_deterministic_algorithms(
            True
        )


def seed_worker(
    worker_id: int,
) -> None:
    worker_seed = (
        torch.initial_seed() % 2**32
    )

    np.random.seed(worker_seed)
    random.seed(worker_seed)


def train_one_epoch(
    *,
    model: nn.Module,
    loader: DataLoader,
    criterion: nn.Module,
    optimizer: torch.optim.Optimizer,
    device: torch.device,
    max_grad_norm: float | None = None,
) -> dict[str, float]:
    model.train()

    loss_meter = AverageMeter()
    total_correct = 0
    total_samples = 0

    for inputs, targets in loader:
        inputs = inputs.to(
            device,
            non_blocking=True,
        )

        targets = targets.to(
            device,
            non_blocking=True,
        )

        optimizer.zero_grad(
            set_to_none=True
        )

        logits = model(inputs)

        loss = criterion(
            logits,
            targets,
        )

        if not torch.isfinite(loss):
            raise FloatingPointError(
                "Non-finite training loss: "
                f"{loss.detach().item()}"
            )

        loss.backward()

        if max_grad_norm is not None:
            torch.nn.utils.clip_grad_norm_(
                model.parameters(),
                max_norm=max_grad_norm,
                error_if_nonfinite=True,
            )

        optimizer.step()

        batch_size = targets.shape[0]

        loss_meter.update(
            loss.detach().item(),
            weight=batch_size,
        )

        predictions = logits.argmax(
            dim=1
        )

        total_correct += (
            predictions == targets
        ).sum().item()

        total_samples += batch_size

    if total_samples == 0:
        raise RuntimeError(
            "Training DataLoader is empty."
        )

    return {
        "loss": loss_meter.average,
        "accuracy": (
            total_correct / total_samples
        ),
    }


@torch.inference_mode()
def evaluate(
    *,
    model: nn.Module,
    loader: DataLoader,
    criterion: nn.Module,
    device: torch.device,
) -> dict[str, float]:
    model.eval()

    loss_meter = AverageMeter()
    total_correct = 0
    total_samples = 0

    for inputs, targets in loader:
        inputs = inputs.to(
            device,
            non_blocking=True,
        )

        targets = targets.to(
            device,
            non_blocking=True,
        )

        logits = model(inputs)

        loss = criterion(
            logits,
            targets,
        )

        batch_size = targets.shape[0]

        loss_meter.update(
            loss.item(),
            weight=batch_size,
        )

        predictions = logits.argmax(
            dim=1
        )

        total_correct += (
            predictions == targets
        ).sum().item()

        total_samples += batch_size

    if total_samples == 0:
        raise RuntimeError(
            "Validation DataLoader is empty."
        )

    return {
        "loss": loss_meter.average,
        "accuracy": (
            total_correct / total_samples
        ),
    }


def save_checkpoint(
    path: str | Path,
    *,
    model: nn.Module,
    optimizer: torch.optim.Optimizer,
    scheduler,
    epoch: int,
    global_step: int,
    best_accuracy: float,
    config: dict[str, Any],
) -> None:
    path = Path(path)
    path.parent.mkdir(
        parents=True,
        exist_ok=True,
    )

    checkpoint = {
        "epoch": epoch,
        "global_step": global_step,
        "model_state_dict": (
            model.state_dict()
        ),
        "optimizer_state_dict": (
            optimizer.state_dict()
        ),
        "scheduler_state_dict": (
            scheduler.state_dict()
            if scheduler is not None
            else None
        ),
        "best_accuracy": best_accuracy,
        "config": config,
    }

    temporary_path = path.with_suffix(
        path.suffix + ".tmp"
    )

    torch.save(
        checkpoint,
        temporary_path,
    )

    temporary_path.replace(path)


def train(
    *,
    model: nn.Module,
    train_loader: DataLoader,
    validation_loader: DataLoader,
    criterion: nn.Module,
    optimizer: torch.optim.Optimizer,
    scheduler,
    device: torch.device,
    config: dict[str, Any],
    output_dir: str | Path,
) -> None:
    output_dir = Path(output_dir)

    checkpoint_dir = (
        output_dir / "checkpoints"
    )
    checkpoint_dir.mkdir(
        parents=True,
        exist_ok=True,
    )

    best_accuracy = float("-inf")
    global_step = 0

    for epoch in range(
        config["num_epochs"]
    ):
        epoch_start = time.perf_counter()

        train_metrics = train_one_epoch(
            model=model,
            loader=train_loader,
            criterion=criterion,
            optimizer=optimizer,
            device=device,
            max_grad_norm=(
                config.get(
                    "max_grad_norm"
                )
            ),
        )

        validation_metrics = evaluate(
            model=model,
            loader=validation_loader,
            criterion=criterion,
            device=device,
        )

        if scheduler is not None:
            scheduler.step()

        global_step += len(train_loader)

        epoch_time = (
            time.perf_counter()
            - epoch_start
        )

        current_lr = (
            optimizer.param_groups[0]["lr"]
        )

        metrics = {
            "epoch": epoch,
            "global_step": global_step,
            "train_loss": (
                train_metrics["loss"]
            ),
            "train_accuracy": (
                train_metrics["accuracy"]
            ),
            "validation_loss": (
                validation_metrics["loss"]
            ),
            "validation_accuracy": (
                validation_metrics[
                    "accuracy"
                ]
            ),
            "learning_rate": current_lr,
            "epoch_time_seconds": (
                epoch_time
            ),
        }

        logging.info(
            json.dumps(metrics)
        )

        save_checkpoint(
            checkpoint_dir / "last.pt",
            model=model,
            optimizer=optimizer,
            scheduler=scheduler,
            epoch=epoch,
            global_step=global_step,
            best_accuracy=best_accuracy,
            config=config,
        )

        if (
            validation_metrics["accuracy"]
            > best_accuracy
        ):
            best_accuracy = (
                validation_metrics[
                    "accuracy"
                ]
            )

            save_checkpoint(
                checkpoint_dir / "best.pt",
                model=model,
                optimizer=optimizer,
                scheduler=scheduler,
                epoch=epoch,
                global_step=global_step,
                best_accuracy=best_accuracy,
                config=config,
            )
```

这份模板仍需要根据任务调整

- Loss 的正确加权方式
- 多标签或回归指标
- AMP
- 梯度累积
- 按 step 的 Scheduler
- Early Stopping
- 分布式训练
- 中断信号处理
- RNG 状态保存

## 7.8 本章小节

```text
训练开始
│
├── 固定配置和数据划分
├── 设置随机种子
├── 构建 DataLoader
├── 构建 Model
├── 构建 Loss
├── 构建 Optimizer
└── 构建 Scheduler
        ↓
每个 Epoch
│
├── model.train()
│   ├── zero_grad
│   ├── forward
│   ├── loss
│   ├── backward
│   ├── gradient processing
│   └── optimizer.step
│
├── model.eval()
│   └── no_grad / inference_mode
│       ├── forward
│       ├── validation loss
│       └── metrics
│
├── scheduler.step
├── logging
├── save last checkpoint
└── save best checkpoint
```

|         操作          |   修改参数   | 建立计算图 | 改变模块模式 |
| :-----------------: | :------: | :---: | :----: |
|   `model.train()`   |    否     |   否   |   是    |
|   `model.eval()`    |    否     |   否   |   是    |
|     普通 forward      |    否     |  通常是  |   否    |
|  `torch.no_grad()`  |    否     |  关闭   |   否    |
|  `loss.backward()`  |    否     | 执行反向  |   否    |
| `optimizer.step()`  |    是     |   否   |   否    |
| `scheduler.step()`  | 间接影响后续更新 |   否   |   否    |
| `load_state_dict()` |   恢复状态   |   否   |   否    |

完成本章后，应能够独立回答

1. 单机单卡训练的完整流程是什么
2. Epoch、Batch Step 和 Optimizer Step 有什么区别
3. 一个训练 Step 的标准调用顺序是什么
4. `backward()` 和 `optimizer.step()` 分别做什么
5. 为什么需要 `zero_grad()`
6. 为什么日志中不能长期保存 Loss Tensor
7. 训练阶段和验证阶段有什么区别
8. 为什么验证阶段仍然需要计算 Loss
9. 如何正确统计整个 Epoch 的平均 Loss
10. 为什么不能简单平均不同 Batch 的 Accuracy
11. `model.train()` 和 `model.eval()` 改变了什么
12. `model.eval()` 和 `torch.no_grad()` 有什么区别
13. 验证阶段为什么通常同时需要二者
14. Checkpoint 与模型权重文件有什么区别
15. 恢复训练时应保存哪些状态
16. 为什么必须保存 Optimizer 和 Scheduler 状态
17. last Checkpoint 和 best Checkpoint 有什么区别
18. `map_location` 和 `weights_only` 有什么作用
19. 如何尽量恢复训练的随机状态
20. 一个实验应该记录哪些日志
21. 为什么要保存配置、代码版本和数据版本
22. 只设置 `torch.manual_seed()` 为什么不够
23. cuDNN Benchmark 与确定性有什么关系
24. `torch.use_deterministic_algorithms()` 有什么作用
25. 如何设置 DataLoader Worker 的随机种子
26. 为什么相同种子仍可能产生不同结果
27. 为什么严格复现与最高性能之间通常存在取舍

# 8 GPU、CUDA 与显存管理

本章的核心关系可以概括为

```text
CPU 主机端
├── Python 代码
├── DataLoader
├── CUDA API 调度
└── CPU 内存
        │
        │ Host-to-Device Copy
        ▼
GPU 设备端
├── 模型参数
├── 梯度
├── 激活值
├── 优化器状态
├── 临时工作空间
└── CUDA Kernel 执行
```

## 8.1 PyTorch 如何使用 GPU

### 8.1.1 CPU 与 GPU 的基本分工

CPU 更适合

- 操作系统和程序控制
- 复杂条件分支
- 文件读取
- 数据解析
- 网络通信
- 任务调度

GPU 更适合

- 大规模矩阵乘法
- 卷积
- Attention
- 向量化逐元素计算
- 高并行数值计算

在典型 PyTorch 训练程序中

```text
CPU：
执行 Python 训练循环
准备数据
向 GPU 提交计算任务

GPU：
执行 Tensor 算子
完成 forward 和 backward 中的大规模计算
```

因此，PyTorch 的 GPU 训练不是“整个 Python 程序都运行在 GPU 上”，而是 CPU 负责控制，GPU 负责执行被提交的 CUDA 算子。

### 8.1.2 CUDA 是什么

CUDA 是 NVIDIA 提供的 GPU 并行计算平台和编程接口。

PyTorch 通过

```python
torch.cuda
```

提供 CUDA 设备管理、Stream、Event、显存统计和同步等能力。

在 PyTorch 中创建 CUDA Tensor 后，对该 Tensor 执行的算子通常会调度到相应 CUDA 设备。不同 GPU 上的 Tensor 默认不能直接参与普通跨设备计算，设备间复制通常需要通过 `to()`、`copy_()` 等接口显式完成。

### 8.1.3 检查 CUDA 是否可用

```python
import torch

print(torch.cuda.is_available())
```

标准设备选择

```python
device = torch.device(
    "cuda:0"
    if torch.cuda.is_available()
    else "cpu"
)
```

查看 GPU 数量

```python
print(torch.cuda.device_count())
```

查看当前设备

```python
print(torch.cuda.current_device())
```

查看设备名称

```python
if torch.cuda.is_available():
    print(
        torch.cuda.get_device_name(0)
    )
```

需要注意

```python
torch.cuda.is_available()
```

为 `False` 时，可能原因包括

- 当前 PyTorch 安装包不包含 CUDA 支持
- NVIDIA Driver 不可用
- GPU 对当前进程不可见
- 容器没有正确挂载 GPU
- CUDA 运行环境与驱动不兼容

### 8.1.4 将模型和数据移动到 GPU

```python
model = model.to(device)

inputs = inputs.to(device)
targets = targets.to(device)

outputs = model(inputs)
```

必须满足

```text
模型参数所在设备
=
输入 Tensor 所在设备
```

否则常见报错类似

```text
Expected all tensors to be on the same device
```

模型的

```python
model.to(device)
```

会递归移动已经注册的

- Parameter
- Buffer

但不会自动移动

- DataLoader 返回的数据
- 普通未注册 Tensor 属性
- 函数内部写死在 CPU 上创建的 Tensor

### 8.1.5 在模型中创建临时 Tensor

错误写法

```python
class WrongModel(torch.nn.Module):
    def forward(self, x):
        mask = torch.ones(x.shape)
        return x * mask
```

如果 `x` 位于 GPU，而 `mask` 默认创建在 CPU，就会出现设备不一致。

更合理的写法

```python
mask = torch.ones_like(x)
```

或者

```python
mask = torch.ones(
    x.shape,
    device=x.device,
    dtype=x.dtype,
)
```

需要长期保存、随模型迁移的 Tensor，应注册为 Buffer

```python
self.register_buffer(
    "mask",
    mask,
)
```

### 8.1.6 `.cuda()` 与 `.to()`

下面两种写法都能将 Tensor 移动到 CUDA

```python
x = x.cuda()
```

```python
x = x.to("cuda")
```

工程代码通常更推荐

```python
x = x.to(device)
```

因为 `.to()` 可以统一处理

- CPU
- CUDA
- dtype
- 多种加速设备
- `non_blocking`
- memory format

例如

```python
x = x.to(
    device=device,
    dtype=torch.float16,
    non_blocking=True,
)
```

### 8.1.7 `.to()` 通常不是原地操作

错误

```python
x.to(device)

print(x.device)
# 仍可能是 cpu
```

正确

```python
x = x.to(device)
```

模型的

```python
model.to(device)
```

会原地修改模块中的参数和 Buffer，同时返回模块自身，因此也常写成

```python
model = model.to(device)
```

### 8.1.8 指定 GPU

显式指定

```python
device = torch.device("cuda:1")
```

也可以在程序外控制可见设备

```bash
CUDA_VISIBLE_DEVICES=1 python train.py
```

此时进程内部看到的第一张 GPU 通常会重新编号为

```text
cuda:0
```

所以需要区分

```text
物理 GPU 编号
与
当前进程中的逻辑 GPU 编号
```

### 8.1.9 GPU 不一定始终更快

对于以下情况，GPU 可能没有明显优势

- Tensor 很小
- 模型计算量很低
- 大量 Python 控制流
- CPU 与 GPU 之间频繁复制数据
- 每个算子规模很小
- 每一步都强制同步
- 数据加载速度过慢

GPU 加速需要足够大的并行计算量，才能抵消

- Kernel 启动开销
- 数据传输开销
- 调度开销
- 同步开销

## 8.2 CPU 与 GPU 的数据传输

### 8.2.1 CPU 和 GPU 使用不同的内存空间

CPU Tensor 通常位于主机内存

```text
Host Memory / RAM
```

CUDA Tensor 位于 GPU 显存

```text
Device Memory / VRAM
```

数据迁移过程包括

```text
CPU → GPU：
Host-to-Device，H2D

GPU → CPU：
Device-to-Host，D2H
```

代码

```python
x_cpu = torch.randn(1024, 1024)

x_gpu = x_cpu.to("cuda")
x_back = x_gpu.to("cpu")
```

这些操作通常需要真实复制数据，而不是简单修改 Tensor 的设备标记。

### 8.2.2 数据传输为什么可能成为瓶颈

GPU 内部显存带宽通常很高，但 CPU 与 GPU 之间的数据需要通过 PCIe、NVLink 或其他互联通道传输。

如果训练循环频繁执行

```python
x = x.cpu()
x = x.cuda()
```

即使模型计算很快，也可能被传输和同步拖慢。

应尽量保持

```text
数据进入 GPU 后
在 GPU 上完成尽可能多的连续计算
```

避免在单个训练 Step 内反复往返。

### 8.2.3 DataLoader 到 GPU 的标准流程

```python
loader = DataLoader(
    dataset,
    batch_size=64,
    num_workers=4,
    pin_memory=True,
)

for inputs, targets in loader:
    inputs = inputs.to(
        device,
        non_blocking=True,
    )

    targets = targets.to(
        device,
        non_blocking=True,
    )
```

其中

```text
pin_memory=True：
让 DataLoader 将 CPU Tensor 放到锁页内存

non_blocking=True：
在条件满足时，减少 Host 线程等待
```

对于 pageable CPU 内存，CUDA 在执行 H2D 传输前通常需要先准备一份 page-locked 副本；Pinned Memory 可以减少这部分额外处理。

### 8.2.4 Pinned Memory

普通 CPU 内存通常是 Pageable Memory，操作系统可以对其页面进行调度和换出。

Pinned Memory，也称 Page-locked Memory，被锁定在物理内存中，更适合 DMA 数据传输。

```python
x = torch.randn(
    1024,
    1024,
    pin_memory=True,
)

print(x.is_pinned())
```

但 pinned memory 也有成本

- 分配成本更高
- 不能被操作系统自由换出
- 使用过多会影响系统内存
- 不适合把整个超大数据集全部长期锁页

一般应让 DataLoader 自动管理 Batch 的 pinning，而不是手动将全部数据集放入 pinned memory。

### 8.2.5 `non_blocking=True`

```python
x_gpu = x_cpu.to(
    "cuda",
    non_blocking=True,
)
```

`non_blocking=True` 的主要含义是：在条件允许时，Host 线程不必等待数据复制完全结束后才继续执行。

PyTorch 的数据传输教程说明，普通 `.to("cuda")` 底层同样会发起异步 CUDA 拷贝，但默认会在每次复制后执行同步；`non_blocking=True` 则避免这次 Host 侧同步。

这并不意味着

```text
数据复制一定与 GPU 计算完全重叠
```

要使 H2D 和 Kernel 真正重叠，通常还需要

- 源数据位于 pinned memory
- 使用独立的非默认 CUDA Stream
- GPU 具有可用的 DMA Engine
- 后续代码没有立即同步
- 复制和计算之间不存在直接依赖

### 8.2.6 从 GPU 取出 Python 数值

```python
loss_value = loss.item()
```

`loss` 位于 GPU 时，CPU 必须等待对应 GPU 计算完成，才能获取其数值。

因此 `.item()` 可能造成同步。

不建议每一步都执行大量

```python
tensor.item()
tensor.cpu()
tensor.numpy()
print(tensor)
```

更合理的是按一定频率记录

```python
if step % 100 == 0:
    loss_value = loss.item()
```

### 8.2.7 GPU Tensor 转 NumPy

NumPy 使用 CPU 内存，不能直接读取普通 CUDA Tensor。

标准写法

```python
array = (
    tensor
    .detach()
    .cpu()
    .numpy()
)
```

其中

```text
detach：
切断计算图关系

cpu：
执行 GPU 到 CPU 数据复制

numpy：
构造 NumPy 数组
```

如果在训练主循环中频繁进行该转换，可能引入大量 D2H 传输和同步。

### 8.2.8 传输优化原则

```text
第一原则：
尽量减少 CPU 与 GPU 往返次数

第二原则：
使用批量传输，而不是大量小 Tensor 分别传输

第三原则：
GPU 训练时测试 pin_memory + non_blocking

第四原则：
不要为了日志频繁将大 Tensor 移回 CPU

第五原则：
只有 profiler 证明传输是瓶颈后，
才引入复杂的多 Stream Prefetch
```

## 8.3 CUDA 异步执行机制

### 8.3.1 什么是异步执行

执行

```python
outputs = model(inputs)
```

时，CPU 通常不是等待 GPU 完成整个 forward 后才执行下一行。

更接近实际的过程是

```text
CPU：
向 CUDA Stream 提交 Kernel A
向 CUDA Stream 提交 Kernel B
向 CUDA Stream 提交 Kernel C
继续执行后续 Python 代码

GPU：
按照 Stream 中的依赖顺序执行 A、B、C
```

CUDA 操作通常相对于 Host 异步执行。PyTorch 官方也指出，GPU 错误可能在实际执行后才被报告，因此报错时显示的 Python 调用栈不一定正好对应真正出错的算子。

### 8.3.2 CUDA Stream

CUDA Stream 可以理解为：GPU 操作的有序任务队列。

在同一个 Stream 中

```text
Kernel A
   ↓
Kernel B
   ↓
Kernel C
```

通常按照提交顺序执行。

不同 Stream 之间可能并发或重叠，但必须正确处理依赖关系。

默认情况下，PyTorch 会在当前 CUDA Stream 上提交大多数操作。

```python
stream = torch.cuda.current_stream()
print(stream)
```

### 8.3.3 异步执行的优势

异步提交可以让 CPU

- 提前准备下一批任务
- 连续提交多个 Kernel
- 进行日志和数据处理
- 与 GPU 计算形成流水线

如果每个 Kernel 都要求 CPU 等待 GPU 完成

```text
CPU 提交一次
等待 GPU
再提交一次
再等待 GPU
```

会造成严重的调度开销和设备空闲。

### 8.3.4 哪些操作可能触发同步

常见同步点包括

- `torch.cuda.synchronize()`
- 从 CUDA Tensor 获取 `.item()`
- 将 CUDA Tensor 复制到 CPU
- 某些需要 CPU 读取 GPU 结果的控制逻辑
- 某些打印和调试操作
- 使用默认阻塞方式进行数据复制

例如

```python
condition = x_gpu.sum().item() > 0
```

CPU 必须先获得 GPU 上的求和结果，才能判断分支。

### 8.3.5 GPU 计时为什么容易错误

错误方式

```python
import time

start = time.perf_counter()

outputs = model(inputs)

elapsed = (
    time.perf_counter() - start
)

print(elapsed)
```

由于 CUDA 异步执行，测到的时间可能主要是

```text
CPU 提交任务的时间
```

而不是

```text
GPU 完成计算的时间
```

### 8.3.6 使用同步进行简单计时

```python
torch.cuda.synchronize()
start = time.perf_counter()

outputs = model(inputs)

torch.cuda.synchronize()
elapsed = (
    time.perf_counter() - start
)
```

同步保证计时区间内的 GPU 工作已经完成。

但同步会破坏原有异步流水线，所以这种方式适合

- 调试
- 小规模基准
- 性能定位

不应无条件放在生产训练循环的每一步中。

### 8.3.7 使用 CUDA Event 计时

```python
start_event = torch.cuda.Event(
    enable_timing=True
)

end_event = torch.cuda.Event(
    enable_timing=True
)

start_event.record()

outputs = model(inputs)

end_event.record()

torch.cuda.synchronize()

elapsed_ms = start_event.elapsed_time(
    end_event
)

print(elapsed_ms)
```

CUDA Event 在 GPU 时间线上记录事件，比单纯使用 CPU 时钟更适合测量 GPU Kernel 执行时间。

### 8.3.8 Warmup

GPU 性能测试通常要先 Warmup

```python
with torch.inference_mode():
    for _ in range(20):
        _ = model(inputs)

torch.cuda.synchronize()
```

首次执行可能包含

- CUDA Context 初始化
- Kernel 加载
- cuDNN 或其他后端算法选择
- 显存池扩张
- 编译或图捕获
- Cache 建立

因此只测第一次 forward 通常不能代表稳定性能。

### 8.3.9 异步错误定位

由于 CUDA 异步执行，真正错误可能在较早的某个 Kernel 中发生，但在后续同步点才被报告。

调试时可以设置

```bash
CUDA_LAUNCH_BLOCKING=1 python train.py
```

这会强制 CUDA 调用更接近同步执行，使错误堆栈更容易对应实际出错代码。PyTorch 官方将其作为定位异步 CUDA 错误的调试手段。

但该环境变量会明显降低性能，不应作为正常训练配置。

## 8.4 显存主要由哪些部分组成

训练过程中，`nvidia-smi` 看到的显存并不只包括模型参数。

可以将显存占用分为以下几类

```text
显存占用
├── 模型参数与 Buffer
├── 参数梯度
├── 优化器状态
├── 前向激活值
├── Autograd 保存的中间结果
├── 临时 Tensor
├── 算子工作空间
├── CUDA Context 与库状态
├── PyTorch 缓存分配器保留内存
└── 其他 CUDA 库或通信库分配
```

PyTorch 的内存统计主要关注由 PyTorch CUDA Allocator 管理的内存；直接通过其他 CUDA API 或 NCCL 等第三方组件分配的显存，未必能出现在 PyTorch 的 Memory Snapshot 中。

### 8.4.1 模型参数

例如一个 Linear 层

```python
layer = torch.nn.Linear(
    in_features=1024,
    out_features=4096,
    bias=True,
)
```

参数数量为

```text
weight：
4096 × 1024

bias：
4096
```

参数显存近似

```text
参数数量 × 每个元素字节数
```

常见 dtype

| dtype | 每元素字节数  |
| :---: | :-----: |
| FP32  | 4 Bytes |
| FP16  | 2 Bytes |
| BF16  | 2 Bytes |
| FP64  | 8 Bytes |
| INT8  | 1 Byte  |

### 8.4.2 Buffer

Buffer 也会占用显存，例如

- BatchNorm running mean
- BatchNorm running variance
- 固定 mask
- 位置编码
- 量化参数
- 模型缓存

虽然 Buffer 不一定参与优化，但调用

```python
model.to("cuda")
```

后，它们同样会移动到 GPU。

### 8.4.3 梯度

当参数参与反向传播后，通常会产生与参数形状相同的梯度

```text
parameter.shape
=
parameter.grad.shape
```

如果参数是 FP32，普通训练中梯度通常也按 FP32 存储

```text
参数：4 Bytes / element
梯度：4 Bytes / element
```

调用

```python
optimizer.zero_grad(
    set_to_none=True
)
```

后，梯度引用可以被设为 `None`，下次 backward 时重新分配或复用相应内存。

### 8.4.4 激活值

激活值是 forward 中各层产生的中间 Tensor，例如

```text
输入 Embedding
Attention 输出
MLP 中间特征
卷积特征图
残差分支结果
归一化中间量
```

训练时，Autograd 需要保存部分中间结果用于 backward。

因此，激活值显存通常与以下因素相关

```text
Batch Size
×
序列长度或图像分辨率
×
隐藏维度或通道数
×
网络深度
×
数据类型
```

激活值不一定与模型参数量成正比。

例如

- 小参数模型处理超长序列，也可能占用大量显存
- 高分辨率视觉模型的特征图可能远大于参数
- 增加 Batch Size 常常主要增加激活显存

### 8.4.5 优化器状态

不同优化器的状态量不同。

**无 Momentum 的 SGD**

主要保存

- 参数
- 梯度

**Momentum SGD**

额外为每个参数保存一个 Momentum Buffer。

**Adam / AdamW**

通常为每个参数保存

- 一阶矩 `exp_avg`
- 二阶矩 `exp_avg_sq`
- step 等少量元数据

Adam/AdamW 的状态显存通常明显大于无 Momentum 的 SGD。

### 8.4.6 临时 Tensor

算子执行过程中可能产生临时 Tensor，例如

- Softmax 中间结果
- Attention Score
- 广播后的计算结果
- Loss 中间结果
- 拼接和排序缓冲
- 梯度裁剪统计量
- 参数更新临时结果

这些 Tensor 生命周期可能很短，但仍会影响峰值显存。

### 8.4.7 算子工作空间

cuDNN、cuBLAS 或其他后端算子可能为了获得更高性能申请工作空间。

例如某个卷积算法可能

```text
计算速度更快
但需要更大的临时 Workspace
```

所以两个数学结果相同的算法，其峰值显存可能不同。

### 8.4.8 CUDA Context 与库状态

进程第一次使用 CUDA 时，会初始化

- CUDA Context
- 库 Handle
- Kernel Module
- 内部 Cache
- Runtime 状态

因此，即使只创建一个很小的 CUDA Tensor，`nvidia-smi` 也可能显示一定的基础显存占用。

`memory_allocated()` 通常小于 `nvidia-smi` 显示值，其中一个原因就是缓存分配器保留内存和 CUDA Context 等额外占用。

### 8.4.9 PyTorch 缓存分配器

频繁执行

```text
申请显存
释放显存
重新申请显存
```

如果每次都直接调用底层 CUDA 分配和释放接口，会产生同步和性能开销。

因此 PyTorch 使用 Caching Allocator

```text
Tensor 被删除
    ↓
内存块不一定立即归还给 CUDA Driver
    ↓
暂时留在 PyTorch 缓存池
    ↓
后续 Tensor 可以快速复用
```

PyTorch 官方说明，缓存分配器可以减少显存分配和释放引起的设备同步，但缓存中的未使用内存仍可能在 `nvidia-smi` 中显示为已使用。

### 8.4.10 推理显存

普通推理不需要

- 参数梯度
- 反向计算图
- 优化器状态
- 大量 backward 中间值

所以通常

```text
推理显存
<
训练显存
```

推理显存主要包括

- 模型参数
- Buffer
- 当前激活值
- 临时工作空间
- Batch 输入
- 特定模型缓存

对于自回归 LLM，还会存在 KV Cache，其大小与

- Batch Size
- 序列长度
- 层数
- KV Head 数
- Head Dimension
- dtype

相关。

## 8.5 参数、梯度、激活值、优化器状态的显存占用

### 8.5.1 参数显存计算

假设模型参数数量为

```text
N
```

参数 dtype 每个元素占用

```text
S_parameter Bytes
```

则参数显存近似为

```text
M_parameter
=
N × S_parameter
```

例如 1 亿参数 FP32 模型

```text
100 000 000 × 4 Bytes
=
400 000 000 Bytes
≈
381 MiB
```

如果只做粗略十进制估算，也常说约 400 MB。

### 8.5.2 梯度显存计算

如果每个参数都需要梯度，并且梯度 dtype 与参数一致

```text
M_gradient
≈
M_parameter
```

例如 1 亿 FP32 参数

```text
参数约 400 MB
梯度约 400 MB
```

但实际中可能有

- 参数被冻结
- 某些参数不参与当前路径
- 梯度是 `None`
- 混合精度策略改变梯度 dtype
- 优化器或分布式框架进行梯度分片

因此这只是基础估算。

### 8.5.3 SGD 显存估算

**SGD，无 Momentum**

每个 FP32 参数主要需要

```text
参数：4 Bytes
梯度：4 Bytes
```

合计约

```text
8 Bytes / parameter
```

不包含激活、临时 Tensor、Buffer 和分配器开销。

**SGD + Momentum**

额外保存

```text
Momentum：4 Bytes
```

合计约：

```text
12 Bytes / parameter
```

### 8.5.4 Adam / AdamW 显存估算

典型 FP32 AdamW

```text
参数：4 Bytes
梯度：4 Bytes
一阶矩：4 Bytes
二阶矩：4 Bytes
```

总计约

```text
16 Bytes / parameter
```

仍然不包括

- 激活
- Buffer
- 临时更新 Tensor
- 算子 Workspace
- Allocator Overhead
- 分布式通信 Buffer

### 8.5.5 混合精度训练的显存

标准 AMP 常见做法是

```text
模型参数仍保留为 FP32
部分前向和反向算子使用 FP16/BF16
部分激活使用低精度
```

在这种情况下，AMP 的主要显存收益通常来自

- 激活值
- 临时 Tensor
- 部分算子 Workspace

而参数、FP32 梯度和 Adam 状态不一定减半。

因此不能简单地认为

```text
开启 AMP
→ 整个训练显存减半
```

如果显存主要被 Adam 状态和 FP32 参数占用，AMP 的总体节省比例可能有限。

某些训练系统会将模型权重存为 FP16/BF16，并额外维护 FP32 Master Weight；此时每参数占用取决于具体实现，不能仅根据模型表面 dtype 推断。

### 8.5.6 激活显存为什么难以简单估算

激活显存取决于计算图中哪些 Tensor 必须保存到 backward。

例如

```text
Linear：
可能需要保存输入或相关信息

ReLU：
可能保存 mask 或输出

Normalization：
可能保存统计量

Attention：
可能保存 Q、K、V 或其他中间结果
```

而且现代算子可能采用

- 算子融合
- 重计算
- Memory-efficient Attention
- In-place 优化
- 自定义 backward

因此准确估算通常需要

- 实际运行
- Peak Memory 统计
- Profiler
- Memory Snapshot

### 8.5.7 激活显存与 Batch Size

很多模型中，激活显存近似随 Batch Size 线性增加

```text
Batch Size × 2
→
部分激活显存约 ×2
```

但以下部分通常不随 Batch Size 线性变化

- 模型参数
- 优化器状态
- 固定 Buffer
- CUDA Context

因此减小 Batch Size 对参数量巨大的模型未必足够，但对激活主导的模型通常很有效。

### 8.5.8 序列长度和图像分辨率

Transformer 中的中间量通常随序列长度增长，某些传统 Attention 中间矩阵还可能随序列长度平方增长

视觉模型中，特征图显存与

```text
Batch
× Channel
× Height
× Width
```

相关。

所以

```text
图像边长从 224 增加到 448
```

空间像素数量约变成四倍，而不是两倍。

OOM 排查时不能只关注 Batch Size，还应检查

- Sequence Length
- Image Resolution
- Crop Size
- Token 数量
- 特征图尺寸

### 8.5.9 Gradient Checkpointing

Gradient Checkpointing 的基本思想是

```text
前向阶段：
不保存全部中间激活

反向阶段：
重新计算部分 forward
以换取较低显存
```

本质是

```text
用额外计算时间
交换更低激活显存
```

它主要降低激活值，而不会直接消除

- 参数
- 梯度
- Adam 状态

### 8.5.10 测量不同阶段显存

```python
def gib(value: int) -> float:
    return value / 1024**3


def print_cuda_memory(
    stage: str,
) -> None:
    torch.cuda.synchronize()

    allocated = (
        torch.cuda.memory_allocated()
    )
    reserved = (
        torch.cuda.memory_reserved()
    )

    max_allocated = (
        torch.cuda.max_memory_allocated()
    )
    max_reserved = (
        torch.cuda.max_memory_reserved()
    )

    print(
        f"[{stage}] "
        f"allocated={gib(allocated):.3f} GiB, "
        f"reserved={gib(reserved):.3f} GiB, "
        f"max_allocated={gib(max_allocated):.3f} GiB, "
        f"max_reserved={gib(max_reserved):.3f} GiB"
    )
```

使用

```python
torch.cuda.reset_peak_memory_stats()

print_cuda_memory("before forward")

outputs = model(inputs)
loss = criterion(outputs, targets)

print_cuda_memory("after forward")

loss.backward()

print_cuda_memory("after backward")

optimizer.step()

print_cuda_memory("after optimizer step")
```

注意同步会影响性能，因此这类统计适合诊断，不适合每一步长期执行。

## 8.6 `memory_allocated` 与 `memory_reserved`

### 8.6.1 为什么需要缓存分配器

如果每个 Tensor 创建和释放都直接执行

```text
cudaMalloc
cudaFree
```

会产生较高开销，并可能引起设备同步。

PyTorch 会向 CUDA 申请较大的内存段，再将其切分成较小内存块供 Tensor 使用。

```text
CUDA Driver
    ↓ 申请 Segment
PyTorch Caching Allocator
    ↓ 切分 Block
Tensor A / Tensor B / Tensor C
```

Tensor 被释放后，对应 Block 可以暂时保留在缓存池中，等待后续复用。

### 8.6.2 `memory_allocated()`

```python
allocated = (
    torch.cuda.memory_allocated()
)
```

表示：当前由存活 CUDA Tensor 占用的 PyTorch 显存字节数。

官方定义是当前 Tensor 占用的 GPU Memory。

示例

```python
x = torch.randn(
    1024,
    1024,
    device="cuda",
)

print(
    torch.cuda.memory_allocated()
)
```

删除引用

```python
del x
```

如果没有其他引用，`memory_allocated()` 通常会下降。

### 8.6.3 `memory_reserved()`

```python
reserved = (
    torch.cuda.memory_reserved()
)
```

表示：当前由 PyTorch Caching Allocator 管理的显存总量。

其中包括

```text
正在被 Tensor 使用的内存
+
已经空闲但仍被 PyTorch 缓存的内存
```

官方定义是当前由 Caching Allocator 管理的 GPU Memory。

通常满足

```text
memory_allocated
≤
memory_reserved
```

### 8.6.4 两者的关系

可以近似理解为

```text
reserved
=
allocated
+
cached but currently unused
+
allocator 内部开销或碎片
```

例如

```text
allocated = 6 GB
reserved  = 10 GB
```

说明

```text
约 6 GB 被存活 Tensor 使用
约 4 GB 由分配器管理但当前未必直接被 Tensor 使用
```

这 4 GB 不一定是一整块可连续使用的空间，还可能存在碎片和不同大小的 Block。

### 8.6.5 为什么 `nvidia-smi` 更高

`nvidia-smi` 通常显示整个 CUDA 进程占用的显存，包括

- PyTorch Allocator Reserved Memory
- CUDA Context
- 库状态
- NCCL
- 非 PyTorch CUDA 分配
- 其他 Runtime 内存

因此常见关系是

```text
memory_allocated
<
memory_reserved
≤ 或接近
nvidia-smi 进程显存
```

但它们不是严格可直接相减的统一口径。

PyTorch 官方明确说明，`memory_allocated()` 往往低于 `nvidia-smi`，因为未使用缓存和 CUDA Context 等也会占用显存。

### 8.6.6 峰值统计

```python
torch.cuda.reset_peak_memory_stats()

run_one_iteration()

peak_allocated = (
    torch.cuda.max_memory_allocated()
)

peak_reserved = (
    torch.cuda.max_memory_reserved()
)
```

其中

- `max_memory_allocated()`：存活 Tensor 占用量的历史峰值
- `max_memory_reserved()`：Allocator 管理内存的历史峰值

峰值通常比迭代结束时的当前值更能反映 OOM 风险。

### 8.6.7 `memory_summary()`

```python
print(
    torch.cuda.memory_summary(
        abbreviated=False
    )
)
```

它会输出可读的 Allocator 统计，适合

- 定期检查
- OOM 异常处理
- 分析 Active 和 Inactive Block
- 判断是否可能存在碎片

PyTorch 官方将 `memory_summary()` 定位为训练中或 OOM 处理时可使用的可读内存统计。

### 8.6.8 Memory Snapshot

复杂问题可以使用 Memory Snapshot

```python
torch.cuda.memory._record_memory_history()

run_training_code()

torch.cuda.memory._dump_snapshot(
    "memory_snapshot.pickle"
)
```

Snapshot 可以记录

- 分配事件
- 释放事件
- OOM 事件
- Segment
- Block
- 分配调用栈
- 内存时间线

官方 Memory Visualizer 可以查看每个 Segment 如何被划分为 Active Block、Inactive Block 和空闲空间，从而分析为什么 Reserved Memory 存在但新分配仍然失败。

需要注意，Snapshot 默认只可见由 PyTorch Allocator 管理的内存，NCCL 或直接 CUDA API 分配的显存可能不可见。

## 8.7 `empty_cache` 的作用与误区

### 8.7.1 `empty_cache()` 做了什么

```python
torch.cuda.empty_cache()
```

它会释放：当前没有被存活 Tensor 使用、但仍由 PyTorch Caching Allocator 保留的空闲缓存。

这些内存返回 CUDA 后

- 其他 GPU 进程可以使用
- `nvidia-smi` 中可能看到占用下降

PyTorch 官方明确说明，`empty_cache()` 释放的是未占用的缓存内存。

### 8.7.2 `empty_cache()` 不会释放存活 Tensor

```python
x = torch.randn(
    10000,
    10000,
    device="cuda",
)

torch.cuda.empty_cache()
```

只要 `x` 仍然存活，它占用的显存就不能被释放。

需要先删除所有引用

```python
del x
torch.cuda.empty_cache()
```

但如果其他变量、列表、闭包或计算图仍然持有它，内存仍不会释放。

### 8.7.3 `empty_cache()` 不会增加 PyTorch 可用显存

这是最重要的结论。

PyTorch 本来就能够重用自己缓存中的空闲内存。

所以

```python
torch.cuda.empty_cache()
```

通常不会让当前 PyTorch 进程突然获得更多可用于 Tensor 的内存。

官方文档明确指出，它不会增加 PyTorch 可用 GPU Memory，但在某些情况下可能帮助缓解碎片。

### 8.7.4 为什么调用后 `nvidia-smi` 下降

调用前

```text
PyTorch 已经不使用某些 Block
但它们仍在 Allocator 缓存中
```

调用后

```text
Allocator 将完全空闲的 Segment
归还给 CUDA Driver
```

所以其他进程和 `nvidia-smi` 能看到这些空间重新可用。

但对于当前进程而言，调用前这些缓存本来就可以被复用。

### 8.7.5 为什么不应每个 Iteration 调用

错误

```python
for batch in loader:
    train_step(batch)
    torch.cuda.empty_cache()
```

这样会破坏缓存分配器的复用机制，导致后续重新执行较慢的底层显存申请。

可能造成

- 训练变慢
- 更多分配和释放
- 更多同步
- 吞吐波动

Caching Allocator 的存在正是为了避免频繁底层分配，因此正常训练中不应每一步清空缓存。

### 8.7.6 什么时候可以使用

相对合理的场景包括

**场景一：一个进程依次加载多个大模型**

```python
del model_a
del optimizer_a

torch.cuda.empty_cache()

model_b = load_model_b()
```

**场景二：Notebook 中删除旧实验对象**

Jupyter 中前一次模型仍可能占用显存，确认删除引用后可以清理缓存。

**场景三：将空闲缓存让给其他 GPU 进程**

当前进程暂时不需要这些缓存时，可以主动归还。

**场景四：特定碎片问题**

某些边界情况下，清空完全空闲 Segment 可能缓解碎片，但它不是通用 OOM 解决方案。

### 8.7.7 `del`、垃圾回收与 `empty_cache`

完整过程可能是

```python
import gc

del large_tensor
del model
del optimizer

gc.collect()
torch.cuda.empty_cache()
```

各自职责

```text
del：
删除当前变量引用

gc.collect：
处理可能存在的 Python 引用环

empty_cache：
将分配器中已经空闲的缓存归还 CUDA
```

如果 Tensor 仍被其他对象引用，后两者也无法释放其有效显存。

## 8.8 OOM 问题排查

CUDA OOM 通常表示

```text
程序需要申请一块新的 GPU 内存
但当前无法完成该分配
```

但 OOM 的根因并不只有“模型太大”。

可以将常见原因分为

```text
OOM
├── 真实峰值超过显存容量
├── 计算图或 Tensor 被意外长期持有
├── 动态 Shape 导致峰值突然增加
├── 缓存分配器碎片
├── 其他进程占用显存
├── 非 PyTorch 组件占用显存
└── 多卡进程或设备映射错误
```

### 8.8.1 第一步：确认其他进程占用

首先检查

```bash
nvidia-smi
```

确认

- 是否有其他训练任务
- 是否有遗留 Python 进程
- 当前程序是否启动了多个进程
- 是否错误地让多个 Rank 使用同一张 GPU

PyTorch 内部也可以查看

```python
print(
    torch.cuda.list_gpu_processes()
)
```

如果一张卡已经被其他进程占用大部分显存，单纯修改当前模型代码无法解决。

### 8.8.2 第二步：确认 OOM 发生在哪个阶段

需要定位

```text
加载模型时 OOM
forward 时 OOM
loss 计算时 OOM
backward 时 OOM
optimizer.step 时 OOM
validation 时 OOM
保存或评估时 OOM
```

不同阶段对应不同原因。

**加载模型时 OOM**

主要检查

- 参数本身过大
- 加载了多个模型副本
- 旧模型没有删除
- 模型被错误复制到多张卡
- Checkpoint 加载过程产生临时副本

**Forward 时 OOM**

主要检查

- Batch Size
- Sequence Length
- Image Resolution
- 激活值
- 临时 Workspace
- 动态输入
- Attention 中间量

**Backward 时 OOM**

主要检查

- Autograd Saved Tensor
- 参数梯度
- 梯度 Hook
- `retain_graph=True`
- 高阶梯度
- 激活值峰值

**Optimizer Step 时 OOM**

主要检查

- Adam 状态首次创建
- 优化器临时 Buffer
- Master Weight
- 参数分组
- 模型参数重复加入优化器

### 8.8.3 第三步：记录显存阶段变化

```python
def log_memory(
    stage: str,
) -> None:
    torch.cuda.synchronize()

    print(
        stage,
        "allocated=",
        torch.cuda.memory_allocated()
        / 1024**3,
        "reserved=",
        torch.cuda.memory_reserved()
        / 1024**3,
        "peak=",
        torch.cuda.max_memory_allocated()
        / 1024**3,
    )
```

使用

```python
torch.cuda.reset_peak_memory_stats()

log_memory("start")

outputs = model(inputs)
log_memory("after forward")

loss = criterion(outputs, targets)
log_memory("after loss")

loss.backward()
log_memory("after backward")

optimizer.step()
log_memory("after step")
```

这样可以初步判断是哪一阶段显存突然增长。

### 8.8.4 判断是否存在显存泄漏

如果每个 Iteration 后

```text
memory_allocated
持续增加
且不回落
```

通常说明有存活 Tensor 或计算图被长期持有。

常见错误一

```python
loss_history.append(loss)
```

`loss` 连接完整计算图。

正确

```python
loss_history.append(
    loss.detach().item()
)
```

### 8.8.5 `running_loss += loss` 问题

错误

```python
running_loss = 0

for batch in loader:
    loss = compute_loss(batch)
    running_loss += loss
```

第一次后 `running_loss` 变成 Tensor，并可能连接后续多个计算图。

正确

```python
running_loss += (
    loss.detach().item()
)
```

或者

```python
running_loss += float(
    loss.detach()
)
```

### 8.8.6 保存模型输出导致计算图累积

错误

```python
all_outputs = []

for batch in loader:
    outputs = model(batch)
    all_outputs.append(outputs)
```

训练模式下，`outputs` 仍连接计算图。

如果只是保存预测

```python
all_outputs.append(
    outputs.detach().cpu()
)
```

如果数据量很大，还应考虑直接写入磁盘，避免 CPU 内存继续增长。

### 8.8.7 `retain_graph=True`

错误习惯

```python
loss.backward(
    retain_graph=True
)
```

如果并不需要重复使用同一张计算图，会导致反向所需中间结果长期保留，显存可能持续增长。

标准训练通常应使用

```python
loss.backward()
```

只有确实需要多次对共享图反向时才保留。

### 8.8.8 Hook 引用

Forward Hook 或 Backward Hook 中如果保存 Tensor

```python
features.append(output)
```

也可能长期持有计算图。

更安全

```python
features.append(
    output.detach().cpu()
)
```

Hook 使用结束后还应移除

```python
handle.remove()
```

### 8.8.9 验证阶段忘记关闭梯度

错误

```python
model.eval()

for inputs, targets in validation_loader:
    outputs = model(inputs)
```

`eval()` 不会关闭 Autograd。

正确

```python
model.eval()

with torch.inference_mode():
    for inputs, targets in validation_loader:
        outputs = model(inputs)
```

如果验证中还保存每个 Batch 输出，忘记关闭梯度尤其容易造成 OOM。

### 8.8.10 真实显存不足时的优化顺序

**方法一：减小 Batch Size**

```python
batch_size = batch_size // 2
```

这是最直接的激活显存优化。

**方法二：使用梯度累积**

```text
小 Micro-batch
×
多个累积 Step
≈
较大有效 Batch
```

注意梯度累积不能降低参数和优化器状态显存。

**方法三：降低输入规模**

例如

- 减少序列长度
- 减小图像分辨率
- 减少视频帧数
- 减少候选数量
- 限制动态 Padding 上限

**方法四：AMP**

使用 FP16 或 BF16 降低激活和部分临时 Tensor 显存。

**方法五：Gradient Checkpointing**

用重计算降低激活保存量。

**方法六：冻结部分参数**

冻结参数可以减少

- 对应梯度
- 对应优化器状态

但 forward 激活是否减少，取决于计算图和哪些输入仍然需要梯度。

**方法七：更换优化器**

Adam 状态较大，内存受限时可以考虑

- SGD
- 低精度优化器
- 8-bit Optimizer
- Optimizer State Offload
- ZeRO/FSDP 分片

**方法八：模型分片或 Offload**

模型本身过大时需要

- DDP 以外的参数分片
- FSDP
- ZeRO
- Tensor Parallel
- CPU Offload
- NVMe Offload

### 8.8.11 动态 Shape 导致的 OOM

训练前几步正常，某一步突然 OOM，可能是该 Batch 更大

```text
更长序列
更大图像
更多检测目标
更多有效 Token
```

需要记录每个 OOM Batch 的

```python
print(inputs.shape)
print(attention_mask.sum())
print(sequence_lengths.max())
```

变长序列任务中，仅固定样本数量意义有限

```text
batch_size=8
```

但 8 个 512-token 样本和 8 个 8192-token 样本的显存完全不同。

可以采用

- Length Bucketing
- Token-based Batch
- 最大序列长度限制
- 动态减小 Batch
- Padding 到有限档位

### 8.8.12 Allocated 与 Reserved 的诊断

**情况一**

```text
allocated 接近显存上限
reserved 也接近上限
```

大概率是存活 Tensor 和真实峰值过大。

优先

- 减少 Batch
- AMP
- Checkpointing
- 缩短序列
- 减小模型或分片

**情况二**

```text
allocated 持续逐步增长
```

大概率有 Tensor 或计算图被长期持有。

优先检查

- list
- dict
- Hook
- `retain_graph`
- 日志
- Cache
- 闭包
- 全局变量

**情况三**

```text
reserved 远大于 allocated
并且 OOM 信息显示大量保留但未分配内存
```

可能存在碎片或动态分配模式问题。

优先使用

```python
torch.cuda.memory_summary()
```

或 Memory Snapshot 分析 Block 和 Segment。

### 8.8.13 内存碎片

假设缓存池有

```text
多个零散的 100 MB 空闲块
```

程序需要

```text
一个连续的 500 MB Block
```

总空闲量可能足够，但没有合适大小的连续 Block，仍可能分配失败。

动态 Batch Size、频繁变化的 Sequence Length 和大量不同尺寸临时 Tensor，可能增加碎片风险。

### 8.8.14 `PYTORCH_ALLOC_CONF`

PyTorch 允许通过

```bash
PYTORCH_ALLOC_CONF=...
```

调整缓存分配器行为。旧别名

```bash
PYTORCH_CUDA_ALLOC_CONF=...
```

仍可用于兼容。当前官方文档将 `PYTORCH_ALLOC_CONF` 作为主要名称。

例如

```bash
PYTORCH_ALLOC_CONF=expandable_segments:True
```

`expandable_segments` 针对频繁变化的分配大小提供一种实验性策略，可能减少动态 Batch 场景中的尾部碎片。

例如

```bash
PYTORCH_ALLOC_CONF=max_split_size_mb:256
```

可能减少大型 Block 被过度切分。

但官方将 `max_split_size_mb` 定位为：OOM 且存在大量 Inactive Split Block 时的最后手段。

它可能明显影响性能，不应在没有 `memory_summary()` 或 Snapshot 证据时随意设置。

### 8.8.15 OOM 后的处理

可以捕获

```python
try:
    outputs = model(inputs)
except torch.cuda.OutOfMemoryError:
    ...
```

但 OOM 恢复并不只是调用

```python
torch.cuda.empty_cache()
```

还需要确保失败计算产生的引用已经释放。

示例

```python
import gc

try:
    outputs = model(inputs)
    loss = criterion(outputs, targets)
    loss.backward()

except torch.cuda.OutOfMemoryError:
    optimizer.zero_grad(
        set_to_none=True
    )

    if "outputs" in locals():
        del outputs

    if "loss" in locals():
        del loss

    gc.collect()
    torch.cuda.empty_cache()

    raise
```

训练任务中通常应保留 OOM 上下文并终止或重新配置，而不是静默跳过所有 OOM Batch。

### 8.8.16 OOM 排查标准流程

```text
第一步：
检查 nvidia-smi 和其他进程

第二步：
确认 OOM 发生阶段

第三步：
记录 allocated、reserved、peak

第四步：
观察是否随 Iteration 持续增长

第五步：
检查计算图、list、Hook、retain_graph

第六步：
检查 Batch、Sequence Length、Resolution

第七步：
使用 memory_summary 或 Snapshot

第八步：
针对真实峰值、泄漏或碎片分别优化

第九步：
最后再考虑 Allocator 环境变量
```

## 8.9 本章小节

```text
PyTorch CUDA 执行
│
├── CPU
│   ├── Python 控制逻辑
│   ├── DataLoader
│   └── 提交 CUDA 操作
│
├── GPU
│   ├── CUDA Stream
│   ├── Kernel
│   └── Device Memory
│
└── 异步执行
    ├── 提高流水线效率
    ├── 计时需要 Event 或同步
    └── 错误可能延迟报告
```

显存关系

```text
nvidia-smi 进程显存
│
├── PyTorch Reserved Memory
│   ├── Allocated：存活 Tensor 使用
│   └── Cached：空闲但可复用
│
├── CUDA Context
├── CUDA 库状态
├── NCCL 等外部分配
└── 其他运行时显存
```

训练显存

```text
训练显存
=
参数
+
梯度
+
优化器状态
+
激活值
+
Autograd 中间结果
+
临时 Tensor
+
Workspace
+
Allocator 与 Runtime 开销
```

|             API             |          作用           |
| :-------------------------: | :-------------------: |
| `torch.cuda.is_available()` |     检查 CUDA 是否可用      |
|     `tensor.to(device)`     |       移动 Tensor       |
|     `model.to(device)`      | 移动 Parameter 和 Buffer |
| `torch.cuda.synchronize()`  |     等待 GPU 当前任务完成     |
|     `torch.cuda.Event`      |       GPU 时间线计时       |
|    `memory_allocated()`     |    当前存活 Tensor 占用     |
|     `memory_reserved()`     |   Allocator 管理的总显存    |
|  `max_memory_allocated()`   |      Tensor 占用峰值      |
|   `max_memory_reserved()`   |      Reserved 峰值      |
| `reset_peak_memory_stats()` |        重置峰值统计         |
|     `memory_summary()`      |        输出分配器统计        |
|       `empty_cache()`       |        归还空闲缓存         |
|     `memory_snapshot()`     |       获取分配器状态快照       |

完成本章后，应能够独立回答

1. PyTorch 的 CPU 和 GPU 分别负责什么
2. `model.to(device)` 会移动哪些对象
3. CPU Tensor 与 CUDA Tensor 为什么不能直接计算
4. `.to()` 是否是原地操作
5. CPU 到 GPU 传输为什么可能成为瓶颈
6. `pin_memory` 与 `non_blocking` 分别做什么
7. CUDA 为什么采用异步执行
8. 什么是 CUDA Stream
9. 为什么普通 Python 计时不能准确测量 GPU 时间
10. 如何使用 CUDA Event 进行计时
11. `CUDA_LAUNCH_BLOCKING=1` 有什么作用
12. 训练显存由哪些部分组成
13. 为什么训练显存高于推理显存
14. 参数、梯度和优化器状态如何估算
15. FP32 Adam 为什么约为 16 Bytes/参数
16. AMP 为什么不一定让总显存减半
17. 激活显存受哪些因素影响
18. `memory_allocated` 与 `memory_reserved` 有什么区别
19. 为什么 `nvidia-smi` 显存通常更高
20. 如何统计显存峰值
21. `empty_cache()` 释放什么
22. 为什么不应每个 Iteration 调用 `empty_cache()`
23. 如何判断是否存在计算图泄漏
24. 保存 Loss 或 Output 为什么可能导致 OOM
25. 如何区分真实容量不足、泄漏和碎片
26. 动态 Sequence Length 为什么会突然 OOM
27. `memory_summary()` 和 Memory Snapshot 有什么作用
28. Allocator 环境变量为什么应作为最后手段
29. OOM 的标准排查顺序是什么

# 9 混合精度训练 AMP

AMP（Automatic Mixed Precision，自动混合精度）解决的核心问题是：在尽量保持模型训练数值稳定性的前提下，让适合低精度的计算使用 FP16/BF16，让数值敏感的计算继续使用 FP32。

因此，AMP 并不是

```text
把整个模型强制转换成 FP16
```

而是

```text
不同算子
根据数值特性
选择不同计算精度
```

典型训练流程

```text
FP32 模型参数
      ↓
autocast
      ↓
部分算子使用 FP16 / BF16
部分算子保持 FP32
      ↓
计算 Loss
      ↓
FP16 时可使用 GradScaler
      ↓
Backward
      ↓
Optimizer Step
```

PyTorch 官方将 AMP 的主要组成分为 Autocast 和 Gradient Scaling：前者自动为不同操作选择合适的低精度或 FP32，后者主要用于缓解低精度反向传播中的梯度下溢。

## 9.1 为什么需要混合精度训练

### 9.1.1 FP32 训练的问题

传统神经网络训练大量使用

```python
torch.float32
```

FP32 的优势是

- 数值范围较大
- 精度较高
- 训练稳定性好
- 算子支持成熟

但代价也明显

```text
每个元素占 4 Bytes
```

在大规模神经网络中，会带来

- 较大的激活显存
- 较高的显存带宽需求
- 更大的临时 Tensor
- 无法充分利用 GPU 的低精度高吞吐计算单元

现代 GPU 上，大型矩阵乘法、卷积等算子通常可以从 FP16/BF16 和 Tensor Core 中获得明显性能收益。PyTorch 官方 AMP 教程也指出，Linear、Convolution 等算子适合低精度，而 Reduction 等操作通常更需要 FP32 的动态范围。

### 9.1.2 为什么不全部使用 FP16

最直接的方法似乎是

```python
model = model.half()
inputs = inputs.half()
```

这样整个计算都使用 FP16。

但 FP16 存在明显的数值范围限制

```text
数值太大
→ Overflow
→ Inf

数值太小
→ Underflow
→ 0
```

尤其是

- Softmax
- Log
- Reduction
- Loss
- 梯度
- 归一化
- 大范围累加

对数值精度和动态范围更加敏感。

NVIDIA 的数值精度文档指出，FP16 只有 5 位指数，最大有限值约为 65504，因此相比 BF16 和 FP32 更容易发生溢出。

所以

```text
全部 FP32：
稳定，但可能慢且占显存

全部 FP16：
快，但数值稳定性风险较高

混合精度：
在二者之间取得平衡
```

### 9.1.3 AMP 的核心思想

假设模型包含

```text
Linear
MatMul
Softmax
LayerNorm
Loss
Reduction
```

AMP 不会简单统一使用一个 dtype，而可能形成

```text
Linear / MatMul
        ↓
FP16 / BF16

部分 Reduction / Loss
        ↓
FP32
```

具体某个算子采用什么精度，由 PyTorch 的 Autocast 策略和算子实现决定。`autocast` 会根据算子的数值特性自动选择执行 dtype。

因此 AMP 中的“Automatic”主要就是：不要求开发者手动为网络中的每个算子决定 FP16、BF16 或 FP32。

### 9.1.4 AMP 为什么能够加速

主要有三个来源。

**第一：低精度计算吞吐更高**

在支持 Tensor Core 的 GPU 上，矩阵乘法、卷积等低精度运算可以获得更高吞吐。PyTorch 官方教程指出，AMP 的收益在能够充分利用 Tensor Core 且计算规模足够大的模型中尤其明显。

**第二：降低显存带宽压力**

FP32

```text
4 Bytes / element
```

FP16 / BF16

```text
2 Bytes / element
```

对于采用低精度保存的激活和临时 Tensor

```text
需要读取和写入的数据量减少
```

因此内存带宽压力可能下降。

**第三：降低部分显存占用**

低精度激活

```text
FP32 activation：
4 Bytes

FP16 / BF16 activation：
2 Bytes
```

因此可以

- 增大 Batch Size
- 增大 Sequence Length
- 使用更大的模型
- 降低训练峰值显存

但需要特别注意：AMP 并不意味着整个训练显存减半。

模型参数、Optimizer State 等部分仍可能保持 FP32。

### 9.1.5 AMP 主要优化哪些显存

普通 AMP 中，收益通常主要来自

```text
激活值
临时 Tensor
部分算子中间结果
```

而

```text
模型 Parameter
Adam 一阶矩
Adam 二阶矩
```

通常不会因为开启 autocast 就全部变成半精度。

因此

```text
激活显存占比很大的模型
→ AMP 显存收益往往明显

Optimizer State 占比很大的模型
→ AMP 整体节省比例可能没有想象中大
```

### 9.1.6 AMP 不只是“省显存”

AMP 的目标一般有两个

```text
Performance
+
Memory
```

有时候开启 AMP 后

- 显存明显下降
- 训练明显加速

有时候

- 显存下降明显
- 速度提升有限

也可能

- 网络太小
- GPU 没有充分利用
- CPU/DataLoader 是瓶颈
- 大量小 CUDA Kernel
- 频繁 `.item()` 同步

导致 AMP 加速效果很小。PyTorch 官方 AMP 教程也将 GPU 未饱和、CPU Bound、过多小算子和 CPU-GPU 同步列为 AMP 加速不明显的常见原因。

## 9.2 FP32、FP16、BF16 的区别

浮点数通常可以抽象为

```text
符号位
+
指数位
+
尾数位
```

其中

```text
指数位：
主要决定数值范围

尾数位：
主要决定数值精度
```

### 9.2.1 FP32

FP32 使用 32 bit

```text
1 bit 符号
8 bit 指数
23 bit 尾数
```

特点

- 动态范围大
- 精度高
- 每元素 4 Bytes
- 深度学习训练中的经典默认精度

可以粗略理解为

```text
Range：大
Precision：高
Memory：高
```

### 9.2.2 FP16

FP16 使用 16 bit

```text
1 bit 符号
5 bit 指数
10 bit 尾数
```

因此相比 FP32

```text
指数位：
8 → 5

尾数位：
23 → 10
```

这意味着

- 数值范围明显减小
- 表示精度降低
- 存储量减半

最大有限数约

```text
65504
```

因此较容易发生

```text
Overflow
Underflow
```

### 9.2.3 BF16

BF16 同样占 16 bit

```text
1 bit 符号
8 bit 指数
7 bit 尾数
```

其最大特点是

```text
指数位与 FP32 相同
```

所以它保留了接近 FP32 的动态范围，但降低了尾数精度。NVIDIA 官方文档将 BF16 描述为 8 位指数、7 位尾数，因此相比 FP16 动态范围更大，但有效精度更低。

可以理解为

```text
FP16：
牺牲较多 Range
保留更多 Precision

BF16：
保留较大 Range
牺牲更多 Precision
```

### 9.2.4 三者比较

|  类型  | 总位数 | 指数位 | 尾数位 | 每元素 |  动态范围   |   精度    |
| :--: | :-: | :-: | :-: | :-: | :-----: | :-----: |
| FP32 | 32  |  8  | 23  | 4 B |    大    |    高    |
| FP16 | 16  |  5  | 10  | 2 B |   较小    |    中    |
| BF16 | 16  |  8  |  7  | 2 B | 接近 FP32 | 低于 FP16 |

BF16 的宽指数范围使其在深度学习中更不容易因大数或小数超出表示范围。

### 9.2.5 FP16 与 BF16 谁更稳定

一般来说

```text
FP16：
有效数字更多
但动态范围窄

BF16：
有效数字更少
但动态范围大
```

深度学习中，很多数值问题来自

```text
梯度或激活数量级过大 / 过小
```

因此 BF16 较大的动态范围通常使训练更加容易。

这也是为什么在原生支持 BF16 的现代 AI 加速硬件上，BF16 是非常重要的训练精度。

### 9.2.6 BF16 为什么通常不需要 Loss Scaling

GradScaler 主要解决

```text
FP16 梯度过小
→ 低于可表示范围
→ 下溢为 0
```

PyTorch 官方将 Gradient Scaling 的主要目标定义为防止小梯度在混合精度训练中 underflow。

而 BF16 拥有与 FP32 相同宽度的指数位，动态范围远大于 FP16。

由此可知，在常规 BF16 混合精度训练中，通常没有 FP16 那样强烈的 Loss Scaling 需求，因此工程实践中经常直接使用

```python
with torch.autocast(
    device_type="cuda",
    dtype=torch.bfloat16,
):
    ...
```

而不启用 GradScaler。

但这并不意味着 BF16 不可能出现 NaN/Inf。模型本身的数值问题仍然可能导致溢出或非法计算。

### 9.2.7 FP16 与 BF16 的选择

可以形成以下基本判断

```text
硬件原生支持 BF16
+
模型训练任务
        ↓
通常优先考虑 BF16

BF16 性能支持不好
但 FP16 Tensor Core 很强
        ↓
FP16 + GradScaler
```

实际选择还需要 benchmark

- 吞吐
- 显存
- 收敛
- GPU 型号
- 算子支持

## 9.3 `autocast` 的作用

### 9.3.1 autocast 是什么

现代 PyTorch 可以使用

```python
with torch.autocast(
    device_type="cuda",
    dtype=torch.float16,
):
    outputs = model(inputs)
    loss = criterion(
        outputs,
        targets,
    )
```

`autocast` 是一个上下文管理器。

它的作用是：在该区域内，根据算子的 Autocast 规则自动选择更合适的计算精度。

PyTorch 官方示例中，Linear 可以在 autocast 下得到 FP16 输出，而 MSE Loss 会以 FP32 计算，说明不同操作并不强制使用同一 dtype。

### 9.3.2 autocast 不等于把输入全部转换

使用 AMP 时通常不需要

```python
inputs = inputs.half()
model = model.half()
```

官方 AMP 示例明确说明，启用 autocast 时无需手动修改输入 dtype。

推荐

```python
inputs = inputs.to(device)

with torch.autocast(
    device_type="cuda",
    dtype=torch.float16,
):
    logits = model(inputs)
```

参数仍可正常保持 FP32，由 autocast 决定各算子的执行精度。

### 9.3.3 autocast 应包住哪些代码

标准范围

```python
with torch.autocast(
    device_type="cuda",
    dtype=torch.float16,
):
    logits = model(inputs)
    loss = criterion(
        logits,
        targets,
    )
```

即

```text
Forward
+
Loss
```

### 9.3.4 backward 不应该放进 autocast

不推荐

```python
with torch.autocast(
    device_type="cuda",
    dtype=torch.float16,
):
    logits = model(inputs)
    loss = criterion(logits, targets)

    loss.backward()
```

推荐

```python
with torch.autocast(
    device_type="cuda",
    dtype=torch.float16,
):
    logits = model(inputs)
    loss = criterion(logits, targets)

loss.backward()
```

PyTorch 官方明确建议在 autocast 区域退出后再执行 backward；反向算子会使用相应前向算子所选择的 dtype。

### 9.3.5 autocast 自动选择精度

不要认为

```text
autocast(dtype=float16)
```

意味着区域内所有东西都必须是 FP16。

更准确的是

```text
允许 Autocast 根据算子策略
选择 FP16 / BF16 / FP32
```

例如某些

```text
MatMul
Linear
Conv
```

适合低精度。

而一些数值敏感操作可能提升到

```text
FP32
```

这也是 AMP 比简单 `.half()` 更可靠的重要原因。

### 9.3.6 不要依赖某个中间 Tensor 一定是什么 dtype

例如

```python
with torch.autocast(
    device_type="cuda",
    dtype=torch.float16,
):
    x = layer1(x)
    x = layer2(x)
```

不应假定

```text
所有 x 都必然是 float16
```

因为最终 dtype 取决于算子 Autocast 规则。

除非确实需要，否则不要在 Autocast 区域中大量手动执行

```python
.float()
.half()
.bfloat16()
```

这会削弱 Autocast 自动管理精度的意义。

### 9.3.7 局部关闭 autocast

如果发现某一段计算数值不稳定，可以局部禁用

```python
with torch.autocast(
    device_type="cuda",
    dtype=torch.float16,
):
    features = model(inputs)

    with torch.autocast(
        device_type="cuda",
        enabled=False,
    ):
        stable_features = (
            features.float()
        )

        result = sensitive_operation(
            stable_features
        )
```

这是解决 AMP 局部数值问题的重要手段。PyTorch 官方 AMP 故障排查也建议，在怀疑某段复杂计算发生 overflow 时，可以局部关闭 autocast 并以 FP32 运行该区域。

## 9.4 `GradScaler` 的作用

### 9.4.1 FP16 梯度下溢问题

考虑一个非常小的梯度

```text
1 × 10^-8
```

FP32 可以表示非常小的数值。

FP16 动态范围更窄，某些极小梯度可能变为

```text
0
```

即

```text
Gradient Underflow
```

如果大量梯度变成 0

```text
参数没有有效更新
→ 模型难以训练
```

PyTorch 官方将 Gradient Scaling 的目的描述为：放大 Loss，使反向传播中的小梯度具有更大的数值，从而降低它们 flush to zero 的风险。

### 9.4.2 Loss Scaling 的核心思想

原始 Loss

```text
L
```

乘一个较大的 Scale

```text
L_scaled = S × L
```

例如

```text
S = 65536
```

则

```text
∂L_scaled / ∂θ
=
S × ∂L / ∂θ
```

因此原本很小的梯度被放大

```text
1e-8
×
65536
≈
6.55e-4
```

这样更不容易在 FP16 中下溢。

### 9.4.3 更新参数前必须恢复梯度尺度

如果直接使用

```text
S × gradient
```

更新

```text
parameter
```

就相当于学习率被额外放大了 `S` 倍。

因此更新前需要

```text
gradient_scaled
        ↓
÷ S
        ↓
真实 gradient
```

PyTorch 的 GradScaler 会自动管理这一过程。

注

1. 梯度计算时激活值（FP16）的梯度传递给FP32的参数的梯度，然后再恢复梯度

### 9.4.4 GradScaler 的基本写法

当前 PyTorch AMP API 可以写

```python
scaler = torch.amp.GradScaler(
    "cuda"
)
```

训练

```python
with torch.autocast(
    device_type="cuda",
    dtype=torch.float16,
):
    logits = model(inputs)
    loss = criterion(
        logits,
        targets,
    )

scaler.scale(loss).backward()

scaler.step(optimizer)

scaler.update()
```

这是 PyTorch 官方 AMP 示例使用的标准流程。

### 9.4.5 `scaler.scale(loss)`

```python
scaled_loss = scaler.scale(loss)

scaled_loss.backward()
```

等价概念

```text
loss
×
current_scale
        ↓
backward
```

得到的 `.grad` 也是经过 Scale 的。

因此此时直接查看

```python
parameter.grad
```

看到的还不是真实梯度尺度。

### 9.4.6 `scaler.step(optimizer)`

不能直接

```python
optimizer.step()
```

FP16 AMP 标准写法是

```python
scaler.step(optimizer)
```

GradScaler 会在更新参数前处理梯度 Scale，并检查 Inf/NaN。如果梯度存在非有限值，本次 `optimizer.step()` 会被跳过，防止错误梯度污染模型参数。

### 9.4.7 `scaler.update()`

```python
scaler.update()
```

根据最近的梯度数值情况调整 Scale。

可以理解为

```text
长期稳定：
Scale 可以逐渐增大

发生 Overflow：
Scale 减小
```

这就是

```text
Dynamic Loss Scaling
```

它比固定写死一个 Loss Scale 更方便。

### 9.4.8 为什么 Scale 不能无限大

Scale 越大

```text
越不容易 Underflow
```

但同时

```text
越容易 Overflow
```

例如

```text
原梯度 = 100
Scale = 65536

Scaled Gradient
≈ 6.5 × 10^6
```

FP16 无法表示时，就可能产生

```text
Inf
```

所以需要动态寻找

```text
不容易 Underflow
同时不容易 Overflow
```

的 Scale。

### 9.4.9 梯度裁剪必须先 `unscale_`

错误

```python
scaler.scale(loss).backward()

torch.nn.utils.clip_grad_norm_(
    model.parameters(),
    1.0,
)

scaler.step(optimizer)
```

因为裁剪的是

```text
放大后的梯度
```

正确

```python
scaler.scale(loss).backward()

scaler.unscale_(optimizer)

torch.nn.utils.clip_grad_norm_(
    model.parameters(),
    max_norm=1.0,
)

scaler.step(optimizer)
scaler.update()
```

PyTorch 官方明确要求，如果需要在 `scaler.step()` 前检查或修改 `.grad`，应先调用 `scaler.unscale_(optimizer)`；梯度裁剪就是典型场景。

## 9.5 AMP 标准训练写法

### 9.5.1 FP16 AMP 标准流程

现代 PyTorch 推荐围绕

```python
torch.autocast(...)
torch.amp.GradScaler(...)
```

组织 AMP。官方示例使用 `torch.autocast` 执行混合精度前向，并使用 `torch.amp.GradScaler("cuda")` 完成梯度缩放。

完整代码

```python
import torch


use_amp = True

scaler = torch.amp.GradScaler(
    "cuda",
    enabled=use_amp,
)

for inputs, targets in train_loader:
    inputs = inputs.to(
        device,
        non_blocking=True,
    )

    targets = targets.to(
        device,
        non_blocking=True,
    )

    optimizer.zero_grad(
        set_to_none=True
    )

    # 只包围 forward 和 loss
    with torch.autocast(
        device_type="cuda",
        dtype=torch.float16,
        enabled=use_amp,
    ):
        logits = model(inputs)

        loss = criterion(
            logits,
            targets,
        )

    # 对放大后的 loss 进行 backward
    scaler.scale(loss).backward()

    # 如果需要梯度裁剪，
    # 必须先恢复真实梯度尺度
    scaler.unscale_(optimizer)

    torch.nn.utils.clip_grad_norm_(
        model.parameters(),
        max_norm=1.0,
    )

    # 检查 Inf / NaN 后更新参数
    scaler.step(optimizer)

    # 动态更新 loss scale
    scaler.update()
```

其核心顺序

```text
zero_grad
    ↓
autocast
    ├── forward
    └── loss
    ↓
scaler.scale(loss).backward()
    ↓
scaler.unscale_(optimizer)
    ↓
gradient clipping
    ↓
scaler.step(optimizer)
    ↓
scaler.update()
```

梯度裁剪的顺序与 PyTorch 官方 AMP 示例一致。

### 9.5.2 不使用梯度裁剪

如果没有

```text
Gradient Clipping
Gradient Logging
Gradient Penalty
```

可以简化为

```python
optimizer.zero_grad(
    set_to_none=True
)

with torch.autocast(
    device_type="cuda",
    dtype=torch.float16,
):
    logits = model(inputs)
    loss = criterion(
        logits,
        targets,
    )

scaler.scale(loss).backward()

scaler.step(optimizer)
scaler.update()
```

`scaler.step()` 会自行处理更新前的梯度反缩放和非有限值检查。

### 9.5.3 BF16 AMP

BF16 具有更大的动态范围，因此通常可以

```python
optimizer.zero_grad(
    set_to_none=True
)

with torch.autocast(
    device_type="cuda",
    dtype=torch.bfloat16,
):
    logits = model(inputs)
    loss = criterion(
        logits,
        targets,
    )

loss.backward()

torch.nn.utils.clip_grad_norm_(
    model.parameters(),
    max_norm=1.0,
)

optimizer.step()
```

这里没有 GradScaler。

这是基于 BF16 动态范围接近 FP32，而 GradScaler 主要解决 FP16 类低精度的小梯度下溢问题这一数值特性的工程选择。

### 9.5.4 同时支持 FP32、FP16 和 BF16

工程代码可以写成

```python
from __future__ import annotations

import contextlib

import torch


def train_one_epoch(
    model,
    loader,
    criterion,
    optimizer,
    device,
    amp_dtype=None,
    max_grad_norm=None,
):
    model.train()

    use_amp = (
        device.type == "cuda"
        and amp_dtype is not None
    )

    use_scaler = (
        use_amp
        and amp_dtype == torch.float16
    )

    scaler = torch.amp.GradScaler(
        "cuda",
        enabled=use_scaler,
    )

    for inputs, targets in loader:
        inputs = inputs.to(
            device,
            non_blocking=True,
        )

        targets = targets.to(
            device,
            non_blocking=True,
        )

        optimizer.zero_grad(
            set_to_none=True
        )

        if use_amp:
            autocast_context = (
                torch.autocast(
                    device_type="cuda",
                    dtype=amp_dtype,
                )
            )
        else:
            autocast_context = (
                contextlib.nullcontext()
            )

        with autocast_context:
            logits = model(inputs)

            loss = criterion(
                logits,
                targets,
            )

        if use_scaler:
            scaler.scale(loss).backward()

            if max_grad_norm is not None:
                scaler.unscale_(
                    optimizer
                )

                torch.nn.utils.clip_grad_norm_(
                    model.parameters(),
                    max_grad_norm,
                )

            scaler.step(optimizer)
            scaler.update()

        else:
            loss.backward()

            if max_grad_norm is not None:
                torch.nn.utils.clip_grad_norm_(
                    model.parameters(),
                    max_grad_norm,
                )

            optimizer.step()
```

这样

```text
amp_dtype=None
→ FP32

amp_dtype=torch.float16
→ FP16 AMP + GradScaler

amp_dtype=torch.bfloat16
→ BF16 AMP
```

### 9.5.5 Scheduler 的位置

对于按 optimizer step 调度的 Scheduler

```python
scaler.step(optimizer)
scaler.update()

scheduler.step()
```

但要注意一个细节

如果 GradScaler 因检测到 Inf/NaN 而跳过

```text
optimizer.step()
```

是否也应该推进 Scheduler，需要根据训练框架设计处理。

对于简单任务可以保持普通调用；对严格的大规模训练，应确保 scheduler 的步数语义与真正 optimizer update 一致。

### 9.5.6 梯度累积 + AMP

假设

```python
accumulation_steps = 4
```

标准思路

```python
optimizer.zero_grad(
    set_to_none=True
)

for step, (inputs, targets) in enumerate(
    train_loader
):
    with torch.autocast(
        device_type="cuda",
        dtype=torch.float16,
    ):
        logits = model(inputs)

        loss = criterion(
            logits,
            targets,
        )

        loss = (
            loss / accumulation_steps
        )

    scaler.scale(loss).backward()

    should_update = (
        (step + 1)
        % accumulation_steps
        == 0
    )

    if should_update:
        scaler.unscale_(optimizer)

        torch.nn.utils.clip_grad_norm_(
            model.parameters(),
            max_norm=1.0,
        )

        scaler.step(optimizer)
        scaler.update()

        optimizer.zero_grad(
            set_to_none=True
        )
```

重点是

```text
整个梯度累积窗口
使用一致的 Scale

完成累积后
再 unscale / clip / step / update
```

### 9.5.7 AMP Checkpoint

如果使用 FP16 GradScaler，完整 Checkpoint 应额外保存

```python
checkpoint = {
    "model": model.state_dict(),
    "optimizer": optimizer.state_dict(),
    "scheduler": scheduler.state_dict(),
    "scaler": scaler.state_dict(),
    "epoch": epoch,
}
```

恢复

```python
model.load_state_dict(
    checkpoint["model"]
)

optimizer.load_state_dict(
    checkpoint["optimizer"]
)

scheduler.load_state_dict(
    checkpoint["scheduler"]
)

scaler.load_state_dict(
    checkpoint["scaler"]
)
```

PyTorch 官方建议，为准确恢复 AMP 训练，应将 GradScaler 的 `state_dict()` 与 Model、Optimizer 状态一同保存。

### 9.5.8 验证阶段

验证阶段可以

```python
model.eval()

with torch.inference_mode():
    with torch.autocast(
        device_type="cuda",
        dtype=torch.float16,
    ):
        logits = model(inputs)
```

推理和验证只需要 Autocast，不需要 GradScaler，因为不存在反向梯度下溢问题。PyTorch 官方 AMP 教程也明确说明，推理/验证可单独使用 autocast，无需 GradScaler。

## 9.6 AMP 的适用场景

### 9.6.1 大量矩阵乘法的模型

AMP 特别适合

- Transformer
- LLM
- Vision Transformer
- CNN
- 大型 MLP
- 多模态模型

因为这些模型包含大量

```text
GEMM
MatMul
Linear
Convolution
```

这类操作通常能够充分利用 GPU 低精度计算能力。PyTorch 官方教程指出，Linear 和 Convolution 是 AMP 低精度收益明显的典型操作。

### 9.6.2 GPU Tensor Core 计算

AMP 的性能提升依赖硬件。

如果 GPU 对

```text
FP16
BF16
```

拥有高吞吐硬件支持，AMP 的收益更明显。

如果硬件较旧

```text
低精度吞吐优势较小
```

则可能主要获得显存收益，而没有明显速度提升。PyTorch 官方 AMP 教程也指出，具有 Tensor Core 的 GPU 更容易获得明显 AMP 加速。

### 9.6.3 显存受限训练

如果 FP32 出现

```text
CUDA Out Of Memory
```

但主要显存来自激活和临时 Tensor，可以尝试 AMP。

例如

```text
原 Batch Size = 16
FP32 OOM
```

开启 AMP 后可能可以

```text
Batch Size = 16
正常运行
```

甚至增大 Batch。

但如果主要显存来自

```text
巨大的模型参数
+
Adam Optimizer State
```

AMP 不一定足够。

### 9.6.4 长序列训练

Transformer 中，长序列通常带来大量

- Q/K/V
- Attention 中间量
- MLP 激活
- Residual Tensor

这些激活采用低精度后，显存收益可能比较明显。

因此 AMP 经常与

```text
AMP
+
Flash Attention / SDPA
+
Gradient Checkpointing
```

组合使用。

### 9.6.5 大模型训练

对于 LLM 训练，低精度已经成为核心工程技术之一。

常见思路

```text
Parameter / Master State：
根据训练框架保留所需精度

MatMul / Attention：
BF16 / FP16

部分数值敏感操作：
FP32
```

大模型训练关注的不只是单个 Tensor dtype，而是

- Parameter Precision
- Compute Precision
- Gradient Precision
- Optimizer State Precision
- Communication Precision

AMP 只是整个混合精度体系中的基础入口。

### 9.6.6 推理同样可以使用 autocast

AMP 不只用于训练。

推理

```python
model.eval()

with torch.inference_mode():
    with torch.autocast(
        device_type="cuda",
        dtype=torch.float16,
    ):
        outputs = model(inputs)
```

此时

```text
不需要 Gradient Scaling
```

因为没有 backward。

不过实际生产推理通常还会进一步采用

- 显式 FP16/BF16 权重
- TensorRT
- Quantization
- FP8
- INT8
- INT4

而不仅仅依赖 autocast。

### 9.6.7 AMP 不一定适合的场景

**模型太小**

GPU 没有被充分利用，AMP 转换和调度收益可能很小。

**大量 CPU 操作**

如果瓶颈在

```text
DataLoader
Python
CPU preprocessing
```

AMP 无法解决。

**算子低精度支持差**

部分自定义算子或特殊算子可能

- 不支持半精度
- 自动回退 FP32
- 低精度性能反而较差

**高数值敏感任务**

某些

- 科学计算
- 高精度回归
- 特殊概率模型
- 数值优化

可能对低精度误差更加敏感。

是否使用 AMP 最终应通过实际

```text
吞吐
显存
训练曲线
最终指标
```

综合判断。

## 9.7 AMP 的数值稳定性问题

AMP 最核心的风险不是 API 使用，而是

```text
有限浮点精度
导致数值行为变化
```

主要问题包括

```text
Underflow
Overflow
Rounding Error
Loss of Precision
Inf
NaN
```

### 9.7.1 Underflow

数值过小

```text
|x| < 当前格式可表示范围
```

可能逐渐变成

```text
0
```

FP16 梯度尤其容易出现这个问题。

GradScaler 的主要目标就是缓解这一问题。

### 9.7.2 Overflow

数值过大

```text
x > 最大可表示值
```

会变成

```text
Inf
```

后续例如：

```text
Inf - Inf
0 × Inf
Inf / Inf
```

可能进一步产生

```text
NaN
```

FP16 最大有限值约为 65504，因此比 BF16/FP32 更容易发生大数溢出。

### 9.7.3 精度损失

浮点数尾数有限。

例如两个很接近的大数

```text
10000.0
10000.1
```

在较低精度中可能无法区分。

这会影响

- Reduction
- 方差
- 差值
- Softmax
- 归一化
- 概率计算

BF16 虽然动态范围大，但只有 7 位尾数，因此比 FP16 的 10 位尾数具有更低的有效精度。

### 9.7.4 Softmax 为什么敏感

Softmax

```text
exp(x_i)
────────────
Σ exp(x_j)
```

指数函数会放大输入差异。

如果 logits 很大

```text
exp(x)
```

还容易发生数值溢出。

现代框架通常会使用稳定实现，例如

```text
x_i - max(x)
```

再计算指数。

这也是为什么不要自行写低精度、未经数值稳定处理的 Softmax。

NVIDIA 的精度文档也将 Softmax 和 Sigmoid 列为对精度变化较敏感的计算。

### 9.7.5 Loss 突然变成 NaN 如何排查

第一步

```text
关闭 AMP
```

运行 FP32。

如果 FP32 仍然 NaN

```text
大概率不是 AMP 本身导致
```

可能是

- 学习率过大
- 输入存在 NaN
- 除零
- Log 非法输入
- Loss 实现错误
- 梯度爆炸

PyTorch 官方 AMP 故障排查也建议分别关闭 autocast 或 GradScaler，以判断 Inf/NaN 是否由混合精度路径引起。

### 9.7.6 分阶段检查

可以检查

```python
if not torch.isfinite(inputs).all():
    raise RuntimeError(
        "Non-finite input"
    )

with torch.autocast(
    device_type="cuda",
    dtype=torch.float16,
):
    logits = model(inputs)

if not torch.isfinite(logits).all():
    raise RuntimeError(
        "Non-finite logits"
    )

loss = criterion(
    logits,
    targets,
)

if not torch.isfinite(loss):
    raise RuntimeError(
        "Non-finite loss"
    )
```

对于大型模型，不建议长期对每个 Tensor 执行完整

```python
torch.isfinite(...)
```

因为会增加

- Kernel
- 同步
- 性能成本

它更适合 Debug。

### 9.7.7 局部切回 FP32

如果确定某段计算容易溢出

```python
with torch.autocast(
    device_type="cuda",
    dtype=torch.float16,
):
    features = encoder(inputs)

    with torch.autocast(
        device_type="cuda",
        enabled=False,
    ):
        stable_features = (
            features.float()
        )

        loss = sensitive_loss(
            stable_features
        )
```

这体现 AMP 的真正思想

```text
不是追求所有计算低精度

而是：
尽可能低精度
必要时保留高精度
```

PyTorch 官方同样建议，在怀疑某个 forward 子区域发生低精度 overflow 时，可以局部禁用 autocast 并强制 FP32。

### 9.7.8 检查 Gradient Norm

```python
scaler.unscale_(optimizer)

gradient_norm = (
    torch.nn.utils.clip_grad_norm_(
        model.parameters(),
        max_norm=1.0,
        error_if_nonfinite=True,
    )
)
```

记录

```python
print(
    gradient_norm.item()
)
```

如果训练过程中从

```text
1
2
3
```

突然变成

```text
100000
Inf
NaN
```

需要进一步检查

- 学习率
- Loss
- 输入异常
- 模型结构
- AMP 精度
- 梯度爆炸

### 9.7.9 GradScaler Scale 持续下降

如果

```text
Scale
不断下降
```

通常说明 GradScaler 频繁检测到

```text
Inf / NaN Gradient
```

这可能表示

- FP16 不适合当前数值范围
- 某个算子容易溢出
- 学习率过高
- 模型本身已经不稳定

此时不应该简单地认为

```text
GradScaler 坏了
```

而应检查训练数值。

### 9.7.10 BF16 也不是绝对安全

BF16 动态范围较大，但

```text
尾数只有 7 bit
```

所以可能出现较明显的

```text
Rounding Error
Precision Loss
```

而且任何 dtype 中，如果数学计算本身出现

```text
除零
非法 log
极端指数
```

都仍可能产生 NaN。

因此

```text
BF16 不需要频繁 Loss Scaling
≠
BF16 不存在数值稳定问题
```

### 9.7.11 不要用 AMP 掩盖模型问题

如果 FP32 已经出现

```text
Loss = NaN
Gradient = Inf
```

切换成 AMP 通常不会真正解决问题。

应该先保证

```text
FP32 Baseline
能够正常训练
```

再开启 AMP 比较

- 收敛速度
- 最终指标
- Loss 曲线
- 显存
- Throughput

这样才能确认问题来自

```text
模型本身
还是
低精度计算
```

## 9.8 本章小节

|             API              |        作用         |
| :--------------------------: | :---------------: |
|    `torch.autocast(...)`     | 自动选择 Forward 算子精度 |
| `torch.amp.GradScaler(...)`  | 动态管理 Loss Scaling |
|     `scaler.scale(loss)`     |      放大 Loss      |
| `scaler.unscale_(optimizer)` |     恢复真实梯度尺度      |
|   `scaler.step(optimizer)`   |    检查梯度并执行参数更新    |
|      `scaler.update()`       |     更新 Scale      |
|    `scaler.state_dict()`     | 保存 GradScaler 状态  |

完成本章后，应能够独立回答

1. 为什么需要混合精度训练
2. AMP 与纯 FP16 训练有什么区别
3. AMP 为什么可能提高训练速度
4. AMP 为什么可以降低部分显存
5. AMP 是否会让训练总显存减半
6. FP32、FP16、BF16 各自占多少字节
7. FP16 和 BF16 的指数位、尾数位有什么区别
8. FP16 为什么更容易 Overflow 和 Underflow
9. BF16 为什么通常比 FP16 更容易训练
10. BF16 为什么通常不需要 GradScaler
11. `autocast` 的作用是什么
12. autocast 是否会把所有算子变成 FP16
13. 为什么不需要手动执行 `model.half()`
14. backward 为什么应放在 autocast 区域外
15. GradScaler 解决什么问题
16. Loss Scaling 的数学原理是什么
17. `scaler.scale()`、`unscale_()`、`step()` 和 `update()` 分别做什么
18. 为什么梯度裁剪必须发生在 `unscale_()` 之后
19. FP16 AMP 的标准训练顺序是什么
20. BF16 AMP 的标准训练顺序是什么
21. 梯度累积和 AMP 如何组合
22. Checkpoint 为什么需要保存 Scaler
23. 验证和推理为什么不需要 GradScaler
24. 哪些模型更容易从 AMP 中获益
25. AMP 加速不明显可能有哪些原因
26. FP16 训练 NaN 应如何排查
27. 如何让数值敏感区域强制使用 FP32
28. 为什么 BF16 也不能保证绝对数值稳定
29. 为什么应该先建立稳定的 FP32 Baseline，再验证 AMP

# 10 PyTorch 性能优化

PyTorch 性能优化最重要的原则不是“看到某个技巧就加上”，而是：先定位瓶颈，再针对瓶颈优化。

一个 GPU 训练 Step 可以粗略拆成

```text
数据读取 / 预处理
        ↓
CPU → GPU 数据传输
        ↓
Python / PyTorch 算子调度
        ↓
GPU Forward
        ↓
Loss
        ↓
GPU Backward
        ↓
Optimizer Step
```

训练慢可能发生在任何一层

```text
训练性能瓶颈
├── DataLoader / I/O
├── CPU 数据预处理
├── H2D 数据传输
├── CPU-GPU 同步
├── Python 调度
├── 大量小 CUDA Kernel
├── GPU 算力利用不足
├── 显存不足导致 Batch 太小
└── 模型本身计算量过大
```

因此，性能优化应遵循

```text
Measure
   ↓
Locate Bottleneck
   ↓
Optimize
   ↓
Measure Again
```

而不是

```text
num_workers 拉满
+ batch_size 拉满
+ AMP
+ checkpoint
+ compile
```

然后期待一定变快。

## 10.1 PyTorch 训练慢的常见原因

### 10.1.1 首先区分 GPU 是否真的“忙”

训练速度慢时，第一步通常观察

- GPU Utilization
- GPU Memory
- CPU Utilization
- DataLoader 等待时间
- 每 Step 时间
- H2D Copy
- CUDA Kernel 数量与执行时间

可以粗略划分为两种情况。

### 10.1.2 情况一：GPU 利用率长期较低

例如

```text
GPU Utilization：
20% → 5% → 60% → 10% → ...
```

通常说明 GPU 经常没有足够工作可做。

可能原因

```text
DataLoader 太慢
CPU 数据增强太重
CPU-GPU 数据传输慢
频繁同步
Batch 太小
大量 Python 逻辑
大量小 CUDA Kernel
```

PyTorch 官方性能指南明确指出，DataLoader 的同步加载、CPU-GPU 同步以及过多独立 Pointwise Kernel 都可能降低 GPU 利用率；多 worker 数据加载和算子融合是相应优化方向。

### 10.1.3 情况二：GPU 利用率长期接近满载

例如

```text
GPU Utilization：
95%～100%
```

同时数据可以及时供应，那么主要瓶颈很可能已经位于 GPU 计算本身。

此时继续

```text
num_workers:
8 → 16 → 32
```

通常不会有明显帮助。

优化重点应转向

- AMP
- 更高效算子
- 算子融合
- `torch.compile`
- Flash/SDPA 类高效 Attention
- 减少不必要计算
- 更合理的数据布局
- 更高效模型结构

### 10.1.4 常见性能瓶颈分类

|       瓶颈       |     常见表现      |         主要优化方向          |
| :------------: | :-----------: | :---------------------: |
|  Data Loading  | GPU 等待 Batch  |     workers、缓存、I/O      |
| CPU Preprocess |    CPU 满载     |      向量化、并行、离线预处理       |
|    H2D Copy    |   Copy 占比高    | pin memory、non-blocking |
|  CPU-GPU Sync  | Timeline 大量空隙 |  减少 `.item()`、`.cpu()`  |
|      小算子       | Kernel 很多但很短  |      算子融合、compile       |
|    Batch 太小    |   GPU 利用不足    |        增大 Batch         |
|      显存不足      |  无法增大 Batch   |    AMP、Checkpointing    |
|      模型计算      |   GPU 持续满载    |     高效 Kernel、模型优化      |

### 10.1.5 不要只看单次 Step 时间

训练开始阶段可能包含

- CUDA Context 初始化
- 内存池扩张
- Kernel 加载
- 后端算法选择
- `torch.compile` 编译
- Cache 建立

因此性能测试通常需要

```text
Warmup
   ↓
稳定执行若干 Step
   ↓
正式统计
```

此外 CUDA 默认异步执行，因此简单用 CPU 时钟包住一个 CUDA 操作可能只测到任务提交时间；准确 GPU 计时需要同步或 CUDA Event。

### 10.1.6 优化优先级

对于普通 GPU 训练，可以先按照

```text
1. DataLoader 是否喂得上 GPU
2. 是否存在频繁 CPU-GPU 同步
3. Batch Size 是否太小
4. 是否正确使用 AMP
5. 是否存在大量小 Kernel
6. Profiler 定位最耗时算子
7. 再考虑 compile / fused kernel / Triton
```

这个顺序比一开始就写自定义 CUDA Kernel 更合理。

## 10.2 数据加载瓶颈

数据加载完整路径通常是

```text
Storage
   ↓
Dataset.__getitem__()
   ↓
Decode / Transform
   ↓
collate_fn
   ↓
DataLoader Worker
   ↓
Pinned Memory
   ↓
CPU → GPU
```

任何一段过慢，都可能导致

```text
GPU：
计算完当前 Batch
        ↓
等待下一 Batch
        ↓
GPU Idle
```

PyTorch 的 DataLoader 在 `num_workers=0` 时由主进程同步加载；设置多个 worker 后，可以让数据准备与训练形成一定程度的重叠。

### 10.2.1 `num_workers`

最基础的优化

```python
train_loader = DataLoader(
    dataset,
    batch_size=64,
    num_workers=4,
)
```

`num_workers=0`

```text
主进程：
读取数据
   ↓
训练 GPU
   ↓
再读取数据
```

多个 worker

```text
Worker：
准备 Batch N+1
准备 Batch N+2

同时

GPU：
训练 Batch N
```

但：`num_workers` 不是越大越好。

PyTorch 官方建议根据 CPU、GPU、数据位置和预处理复杂度实际调节；过多 worker 会带来 CPU 竞争、内存和进程通信开销。

可以实际测试

```text
0
2
4
8
16
```

观察

```text
samples / second
GPU utilization
CPU utilization
RAM
```

找到吞吐趋于平台的位置。

### 10.2.2 `pin_memory`

GPU 训练可以测试

```python
DataLoader(
    dataset,
    pin_memory=True,
)
```

然后

```python
inputs = inputs.to(
    device,
    non_blocking=True,
)
```

Pinned Host Memory 更适合 CPU → GPU 的数据传输，PyTorch DataLoader 可以自动将取出的 Tensor 放入 pinned memory。

但它不是万能优化。如果瓶颈在

```text
JPEG 解码
网络存储
Tokenizer
复杂数据增强
```

即使 H2D Copy 加快，对总训练时间影响也可能很小。

### 10.2.3 `persistent_workers`

多 Epoch 训练可以使用

```python
DataLoader(
    dataset,
    num_workers=4,
    persistent_workers=True,
)
```

减少每轮重新启动 worker 的开销，尤其适合 worker 初始化成本较高的场景。PyTorch 当前数据加载优化教程将其作为跨 Epoch 减少 worker restart overhead 的常见手段。

### 10.2.4 优化 Dataset

如果 Dataset 内部存在

```python
def __getitem__(self, index):
    tokenizer = HugeTokenizer(...)
```

每个样本都重新初始化对象，就会严重拖慢。

应尽量避免

- 每样本初始化大型对象
- 大量 Python 小循环
- 重复打开文件
- 重复计算固定特征
- 大量小文件随机读取

可以考虑

```text
离线预处理
缓存
批量读取
向量化
合并小文件
长度分桶
```

### 10.2.5 如何确认是 DataLoader 瓶颈

可以分别记录

```text
data_time
compute_time
```

例如

```python
import time

end = time.perf_counter()

for inputs, targets in train_loader:
    data_time = (
        time.perf_counter() - end
    )

    inputs = inputs.to(
        device,
        non_blocking=True,
    )
    targets = targets.to(
        device,
        non_blocking=True,
    )

    torch.cuda.synchronize()
    start = time.perf_counter()

    train_step(
        inputs,
        targets,
    )

    torch.cuda.synchronize()

    compute_time = (
        time.perf_counter() - start
    )

    print(
        f"data={data_time:.3f}, "
        f"compute={compute_time:.3f}"
    )

    end = time.perf_counter()
```

这里的 `synchronize()` 只是为了诊断计时准确性，不应长期放在高性能训练主循环中，因为它会破坏 CUDA 的异步流水线。

## 10.3 CPU-GPU 同步瓶颈

CUDA 的一个核心特点是

```text
CPU 提交 GPU 工作
不需要每次都等待 GPU 完成
```

PyTorch CUDA 操作默认相对于 Host 异步提交，同一 Stream 中依次执行；这种机制让 CPU 可以提前提交后续 GPU 工作。

如果频繁强制同步

```text
CPU：
提交 Kernel
   ↓
等待 GPU
   ↓
提交 Kernel
   ↓
等待 GPU
```

GPU Pipeline 和 CPU 调度就无法充分重叠。

### 10.3.1 常见同步操作

PyTorch 官方性能指南特别列出了应避免频繁出现的同步行为，例如

```python
print(cuda_tensor)

value = cuda_tensor.item()

cpu_tensor = cuda_tensor.cpu()
```

以及 CPU/GPU 之间的数据复制。

### 10.3.2 `.item()` 为什么会同步

假设

```python
loss = criterion(
    model(inputs),
    targets,
)
```

`loss` 位于 GPU。

执行

```python
loss_value = loss.item()
```

CPU 必须知道这个具体数值。

因此流程变成

```text
CPU：
“给我 loss 数值”
        ↓
等待 GPU 完成产生 loss 的计算
        ↓
GPU → CPU
        ↓
得到 Python float
```

如果每个 Step 都多次调用 `.item()`

```python
loss.item()
grad_norm.item()
accuracy.item()
...
```

会不断制造同步点。

### 10.3.3 日志不要过于频繁

可以

```python
if step % 100 == 0:
    logger.info(
        "loss=%.4f",
        loss.item(),
    )
```

而不是

```python
print(loss.item())
```

每一步都执行。

### 10.3.4 Python 条件判断也可能同步

例如

```python
if loss.item() < 0.1:
    ...
```

CPU 必须等待 GPU 得到 Loss。

更加隐蔽的例子

```python
if tensor.sum().item() > 0:
    ...
```

这种数据依赖型 Python 控制流会导致 CPU 读取 GPU 结果。

### 10.3.5 设备之间来回传输

性能较差

```python
x = model(x_gpu)

x = x.cpu()
x = cpu_process(x)
x = x.cuda()

x = next_layer(x)
```

应尽量让连续计算留在同一设备

```text
CPU → GPU
     ↓
尽可能多地完成计算
     ↓
最后再 GPU → CPU
```

### 10.3.6 `torch.cuda.synchronize()`

```python
torch.cuda.synchronize()
```

会等待相关 CUDA 工作完成。

它适合

- Debug
- 准确基准
- 测量 GPU 时间
- 特定同步需求

不适合无意义地放在每个训练 Step 中。CUDA 官方语义文档明确指出，异步执行使无同步计时不准确，因此需要时可用 `synchronize()` 或 CUDA Event，但这类同步本身会改变原有执行流水线。

## 10.4 小算子与 Python 调度开销

### 10.4.1 GPU Kernel 并不是零成本启动

假设

```python
x = x + 1
x = x * 2
x = torch.sigmoid(x)
x = x - 0.5
```

在 PyTorch Eager Mode 中，这可能对应多个独立 GPU Kernel

```text
Python
 ↓
Kernel 1: add
 ↓
Kernel 2: multiply
 ↓
Kernel 3: sigmoid
 ↓
Kernel 4: subtract
```

每个 Kernel 都存在

- Python/Dispatcher 调度
- CUDA Kernel Launch
- Tensor 元数据处理
- Global Memory 读写

PyTorch 官方性能指南指出，多个逐元素操作通常会生成多个独立 Kernel；融合后可以减少 Kernel Launch，并避免中间结果被反复写回和读取显存。

### 10.4.2 为什么大矩阵乘法反而效率高

例如一个 GEMM

```python
y = x @ weight
```

虽然计算量巨大，但一次 Kernel 可以让大量 GPU 线程工作较长时间。

而

```text
1000 个极小 Kernel
```

可能每个只执行几微秒，GPU 还没充分利用，就已经结束。

于是整体时间主要消耗在

```text
Launch
Dispatch
Memory Access
```

而不是数学计算。

### 10.4.3 Python `for` 循环

例如

```python
outputs = []

for i in range(x.shape[1]):
    outputs.append(
        torch.relu(
            self.linear(x[:, i])
        )
    )

output = torch.stack(
    outputs,
    dim=1,
)
```

可能产生大量

```text
slice
linear
relu
stack
```

调度。

如果数学语义允许，可以直接向量化

```python
output = torch.relu(
    self.linear(x)
)
```

一次对 `[B,T,D]` 的最后一个维度执行 Linear。

### 10.4.4 算子融合

原始

```text
Kernel 1:
读取 x
计算 A
写回 A

Kernel 2:
读取 A
计算 B
写回 B

Kernel 3:
读取 B
计算 C
写回 C
```

融合后

```text
Fused Kernel:
读取 x
计算 A → B → C
写回结果
```

减少

- Kernel Launch
- Global Memory 读写
- 中间 Tensor
- Python 调度

PyTorch 官方性能指南将算子融合作为减少逐元素操作 Kernel Launch 和 Memory Access 的主要优化，并指出 `torch.compile`/TorchInductor 能对符合条件的算子进行自动融合。

### 10.4.5 不要为了减少“代码行”而做错误优化

例如

```python
x.add_(y)
```

虽然 In-place 可能减少某些分配，但它可能

- 破坏 Autograd
- 增加代码复杂度
- 阻碍部分编译优化
- 实际性能收益很小

性能优化必须根据 Profiler 判断。

### 10.4.6 小算子问题的典型 Profiler 特征

你可能看到

```text
Op                         Calls
aten::add                  5000
aten::mul                  4000
aten::slice                3000
aten::copy_                2500
```

但每个 CUDA Kernel

```text
5 μs
10 μs
8 μs
```

此时值得检查

- 是否可以向量化
- 是否可以减少 Python 循环
- 是否有重复 reshape/copy
- 是否能算子融合
- 是否适合 `torch.compile`

## 10.5 Batch Size 对吞吐和显存的影响

Batch Size 是训练性能中非常重要的参数。

它同时影响

```text
GPU 并行度
显存
吞吐
梯度统计
Optimizer Step 数
训练收敛行为
```

### 10.5.1 Batch 太小

假设 GPU 可以并行处理大量矩阵运算，但输入只有

```text
Batch Size = 1
```

可能导致

```text
GPU SM 利用不足
Tensor Core 利用不足
Kernel 规模不足
固定调度开销占比高
```

这时增大 Batch Size

```text
1 → 8 → 32 → 64
```

通常能提高吞吐，直到硬件逐渐接近饱和。

### 10.5.2 Batch 增大后吞吐为什么会提升

例如

```text
Batch 16：
100 samples/s

Batch 32：
170 samples/s

Batch 64：
260 samples/s
```

不是因为单个样本计算量减少，而是：GPU 可以一次处理更多数据，摊薄 Kernel Launch 等固定开销，同时提高计算单元利用率。

### 10.5.3 吞吐不会无限线性增加

最终可能

```text
Batch 64：
260 samples/s

Batch 128：
300 samples/s

Batch 256：
305 samples/s
```

此时 GPU 已经比较饱和。

继续增大

```text
Batch 512
```

可能只增加显存，不明显增加吞吐。

### 10.5.4 Batch Size 与显存

参数显存基本与 Batch Size 无关

```text
Parameter
Optimizer State
```

而激活值通常明显受 Batch Size 影响

```text
Activation Memory
≈
Batch Size × 单样本激活规模
```

因此

```text
Batch Size ↑
       ↓
Activation Memory ↑
       ↓
Peak VRAM ↑
```

### 10.5.5 Batch Size 还影响训练算法

不能只根据“哪个吞吐最高”选择 Batch Size。

更大的 Batch 会改变

- 梯度噪声
- 一个 Epoch 中的 optimizer step 数
- 学习率适用范围
- BatchNorm 统计
- 优化路径
- 最终泛化

因此 Batch Size 是

```text
System Hyperparameter
+
Optimization Hyperparameter
```

而不只是 GPU 性能参数。

### 10.5.6 一个常见调优方式

固定

- 模型
- dtype
- 数据
- 训练逻辑

分别测试

```text
B = 8
B = 16
B = 32
B = 64
B = 128
```

记录

```text
samples/s
step time
peak memory
GPU utilization
最终训练指标
```

找到：吞吐已经较高，但仍保留一定显存余量，并且优化行为合理的 Batch Size。

## 10.6 Gradient Accumulation

Gradient Accumulation，即梯度累积，主要解决：单次显存无法容纳目标 Batch Size 时，用多个较小 Micro-batch 累积梯度，再执行一次参数更新。

### 10.6.1 基本原理

假设希望

```text
Batch Size = 128
```

但显存只能容纳

```text
Micro Batch = 32
```

则

```text
Accumulation Steps
=
128 / 32
=
4
```

执行

```text
Micro Batch 1
forward + backward
保留梯度

Micro Batch 2
forward + backward
继续累积梯度

Micro Batch 3
...

Micro Batch 4
...

最后：
optimizer.step()
```

### 10.6.2 标准代码

```python
accumulation_steps = 4

optimizer.zero_grad(
    set_to_none=True
)

for step, (inputs, targets) in enumerate(
    train_loader
):
    logits = model(inputs)

    loss = criterion(
        logits,
        targets,
    )

    loss = (
        loss / accumulation_steps
    )

    loss.backward()

    should_update = (
        (step + 1)
        % accumulation_steps
        == 0
    )

    if should_update:
        optimizer.step()

        optimizer.zero_grad(
            set_to_none=True
        )
```

### 10.6.3 为什么 Loss 通常要除以累积步数

假设每个 Micro-batch loss 使用

```text
reduction="mean"
```

各自梯度

```text
g1, g2, g3, g4
```

如果直接累积

```text
g_total
=
g1 + g2 + g3 + g4
```

相较于大 Batch 的平均梯度，会放大约 4 倍。

因此

```python
loss = (
    loss / accumulation_steps
)
```

最终

```text
g_total
=
(g1 + g2 + g3 + g4) / 4
```

更接近一个大 Batch 的平均梯度。

### 10.6.4 Gradient Accumulation 节省什么显存

它通过减小

```text
Micro Batch Size
```

减少单次

```text
Activation Memory
```

但不会明显减少

```text
模型 Parameter
Optimizer State
```

而参数 `.grad` 在累积期间还必须保留。

所以

```text
Gradient Accumulation
≠
所有显存减半
```

### 10.6.5 Gradient Accumulation 会加速吗

通常它的主要目的不是加速，而是

```text
省显存
+
实现较大的 Effective Batch
```

例如真实 Batch 128 一次 forward

```text
1 次大 Batch Forward/Backward
```

梯度累积

```text
4 次 Micro-batch Forward/Backward
```

Python 调度和 Kernel Launch 次数更多，所以吞吐可能下降。

### 10.6.6 Effective Batch Size

单卡

```text
Effective Batch
=
Micro Batch
×
Accumulation Steps
```

DDP

```text
Global Effective Batch
=
Micro Batch
×
Accumulation Steps
×
World Size
```

例如

```text
Micro Batch = 8
Accumulation = 4
GPU = 8

Global Batch
=
8 × 4 × 8
=
256
```

### 10.6.7 梯度累积并不总是严格等价于大 Batch

原因可能包括

- BatchNorm 每个 Micro-batch 独立统计
- Dropout 使用不同随机 mask
- 随机数据增强
- Scheduler Step 语义
- 最后不足完整累积窗口
- 多卡梯度同步方式

因此更准确的说法是：在常见 loss averaging 条件下，梯度累积可以使梯度尺度接近较大 Batch，但整个训练行为不一定逐位等价。

## 10.7 Gradient Checkpointing

更准确的名称通常是：Activation Checkpointing。

它解决的问题与 Gradient Accumulation 不同。

```text
Gradient Accumulation：
减小一次处理的 Batch

Gradient Checkpointing：
减少 Forward 时保存的激活
```

### 10.7.1 为什么训练需要保存激活

普通训练

```text
Forward
│
├── Activation 1 ─────────┐
├── Activation 2 ───────┐ │
├── Activation 3 ─────┐ │ │
└── Output           │ │ │
                     ↓ ↓ ↓
                  Backward
```

Backward 需要很多 Forward 中间结果。

所以这些 Tensor 在 Forward 结束后不能立即释放。

### 10.7.2 Checkpointing 的基本思想

普通方式

```text
Forward：
保存 A、B、C、D

Backward：
直接使用 A、B、C、D
```

Checkpoint

```text
Forward：
只保留部分关键输入/Checkpoint

Backward：
重新执行部分 Forward
重新得到 B、C
再计算梯度
```

即

```text
Memory ↓
Compute ↑
```

PyTorch 官方将 Activation Checkpointing 定义为一种“以计算换显存”的技术：Forward 时不保留 checkpoint 区域内全部中间激活，Backward 时按需重新执行这段函数。

### 10.7.3 基本使用

```python
from torch.utils.checkpoint import (
    checkpoint,
)


class Model(nn.Module):
    def __init__(self):
        super().__init__()

        self.block1 = Block()
        self.block2 = Block()
        self.block3 = Block()

    def forward(self, x):
        x = self.block1(x)

        x = checkpoint(
            self.block2,
            x,
            use_reentrant=False,
        )

        x = self.block3(x)

        return x
```

当前 PyTorch 文档推荐显式使用 `use_reentrant=False` 的实现。

### 10.7.4 Forward 与 Backward 流程

假设

```text
Block 2：
A → B → C → D
```

普通训练

```text
Forward：
A → B → C → D
    保存 B、C

Backward：
直接使用 B、C
```

Checkpoint

```text
Forward：
A → B → C → D
只保存需要的边界状态

Backward：
重新 A → B → C → D
得到需要的中间量
再计算梯度
```

因此一次完整训练中

```text
Block 2 Forward
```

可能被执行不止一次。

### 10.7.5 Checkpointing 节省哪部分显存

主要降低

```text
Activation Memory
Autograd Saved Tensor
```

不会直接降低

```text
Parameter
Gradient
Optimizer State
```

所以如果一个超大模型的显存主要是

```text
Adam State
+
Parameters
```

Checkpointing 可能仍无法解决问题。

### 10.7.6 哪些模型收益明显

通常

```text
深层 Transformer
LLM
ViT
非常深的 CNN
长序列网络
```

收益明显。

原因是这些模型

```text
层数多
×
每层 Activation 大
```

训练激活显存占比较高。

### 10.7.7 Checkpointing 的代价

核心代价

```text
Backward 需要重新做 Forward
```

所以

```text
训练时间 ↑
峰值激活显存 ↓
```

不能把 Gradient Checkpointing 当成性能优化。

它更准确地属于：Memory Optimization / Compute-Memory Tradeoff。

### 10.7.8 不是什么都应该 Checkpoint

如果对所有极小操作都做 Checkpoint

```text
重计算和调度成本可能过高
```

通常选择

```text
Transformer Block
Residual Block
较大的 Sequential Segment
```

作为粒度。

### 10.7.9 随机算子问题

Checkpoint 区域中可能存在

```text
Dropout
Random Noise
```

如果 Forward 和重计算阶段随机状态不同，重计算结果就可能不同。

PyTorch 的 checkpoint 实现默认会处理 RNG 状态保存/恢复以维持这类语义，但这本身会增加一定运行成本；同时官方也提醒，如果重计算时函数行为因全局状态等原因与原 Forward 不一致，可能产生错误甚至不正确梯度。

### 10.7.10 Gradient Accumulation 与 Checkpointing 对比

|  对比项   | Gradient Accumulation | Gradient Checkpointing |
| :----: | :-------------------: | :--------------------: |
|  核心目标  |       模拟大 Batch       |         减少激活显存         |
|   手段   |    多个 Micro-batch     |  Backward 时重算 Forward  |
|  主要减少  |      单次 Batch 激活      |   Saved Activations    |
| 是否增加计算 |       通常增加调度次数        |        明确增加重计算         |
| 参数状态显存 |         基本不变          |          基本不变          |
|  优化行为  |     改变有效 Batch 设计     |       理论目标应保持一致        |
|  典型场景  |       Batch 放不下       |       模型/序列激活放不下       |

两者可以同时使用

```text
AMP
+
Micro Batch
+
Gradient Accumulation
+
Activation Checkpointing
```

这是大模型训练中常见的显存控制组合思路。

## 10.8 `torch.profiler` 基础使用

性能优化最重要的工具之一就是

```python
torch.profiler
```

其核心问题不是：“我的训练慢吗？”

而是：时间具体花在哪里？

PyTorch Profiler 可以记录 CPU Operator、CUDA Kernel、输入 Shape 和 Tensor 内存分配等信息。

### 10.8.1 最简单的 Profiler

```python
import torch
from torch.profiler import (
    profile,
    ProfilerActivity,
)


with profile(
    activities=[
        ProfilerActivity.CPU,
        ProfilerActivity.CUDA,
    ],
) as prof:
    outputs = model(inputs)

print(
    prof.key_averages().table(
        sort_by="self_cuda_time_total",
        row_limit=20,
    )
)
```

Profiler 的 `activities` 可以指定 CPU 和 CUDA；`key_averages()` 可以聚合操作统计，再按照 CPU/CUDA 时间排序。

### 10.8.2 `Self` 和 `Total` 时间

例如

```text
CPU total
Self CPU
CUDA total
Self CUDA
```

可以理解

```text
Total：
包括它调用的子操作

Self：
只统计这个操作自身
```

假设

```text
Module A
├── MatMul
└── ReLU
```

那么

```text
A Total
=
自身 + MatMul + ReLU
```

而

```text
A Self
```

不包含子操作耗时。

PyTorch Profiler 官方示例也区分了 Self Time 和 Total Time，并支持按相应字段排序。

### 10.8.3 记录输入 Shape

```python
with profile(
    activities=[
        ProfilerActivity.CPU,
        ProfilerActivity.CUDA,
    ],
    record_shapes=True,
) as prof:
    model(inputs)
```

然后

```python
print(
    prof.key_averages(
        group_by_input_shape=True
    ).table(
        sort_by="self_cuda_time_total",
        row_limit=20,
    )
)
```

Shape 很重要。

同一个

```text
aten::matmul
```

可能出现

```text
[4, 16] × [16, 16]

和

[4096, 4096] × [4096, 4096]
```

性能含义完全不同。

Profiler 的 `record_shapes=True` 会记录 Operator 输入 Shape，`group_by_input_shape=True` 可以进一步按 Shape 聚合。

### 10.8.4 内存分析

```python
with profile(
    activities=[
        ProfilerActivity.CPU,
        ProfilerActivity.CUDA,
    ],
    record_shapes=True,
    profile_memory=True,
) as prof:
    outputs = model(inputs)
```

可以观察

```text
CPU Memory
CUDA Memory
Self CUDA Memory
```

`profile_memory=True` 会跟踪 Tensor 的内存分配和释放。

### 10.8.5 `record_function`

可以主动标记代码段

```python
from torch.profiler import record_function


with profile(
    activities=[
        ProfilerActivity.CPU,
        ProfilerActivity.CUDA,
    ],
) as prof:

    with record_function(
        "data_transfer"
    ):
        inputs = inputs.to(
            device
        )

    with record_function(
        "forward"
    ):
        outputs = model(inputs)

    with record_function(
        "loss"
    ):
        loss = criterion(
            outputs,
            targets,
        )

    with record_function(
        "backward"
    ):
        loss.backward()
```

这样 Timeline 中会看到

```text
data_transfer
forward
loss
backward
```

比单独面对大量

```text
aten::...
cudaLaunchKernel
...
```

更容易理解。`record_function()` 正是 PyTorch Profiler 提供的用户自定义区间标记机制。

### 10.8.6 不要 Profile 整个训练过程

例如完整训练

```text
100 Epoch
×
10000 Step
```

如果全部记录

- 开销很大
- Trace 文件很大
- 很难分析

PyTorch Profiler 为长时间任务提供了 `schedule`、`on_trace_ready` 和 `prof.step()`，可以只采集少数代表性 Step。

### 10.8.7 Profiler Schedule

例如

```python
from torch.profiler import (
    profile,
    schedule,
    ProfilerActivity,
)


profiler_schedule = schedule(
    wait=5,
    warmup=2,
    active=3,
    repeat=1,
)
```

含义

```text
前 5 Step：
WAIT
不记录

接下来 2 Step：
WARMUP
开始 Profiler，但丢弃结果

接下来 3 Step：
ACTIVE
正式记录
```

官方 Profiler Recipe 使用同样的 Wait → Warmup → Active 思路，以避免训练开始阶段的额外开销污染正式采样。

### 10.8.8 完整训练 Profiler 示例

```python
import torch
from torch.profiler import (
    profile,
    schedule,
    ProfilerActivity,
    tensorboard_trace_handler,
)


profiler_schedule = schedule(
    wait=5,
    warmup=2,
    active=3,
    repeat=1,
)


with profile(
    activities=[
        ProfilerActivity.CPU,
        ProfilerActivity.CUDA,
    ],
    schedule=profiler_schedule,
    # 保存记录结果
    on_trace_ready=(
        tensorboard_trace_handler(
            "./profiler_logs"
        )
    ),
    record_shapes=True,
    profile_memory=True,
    with_stack=True,
) as prof:

    for step, (
        inputs,
        targets,
    ) in enumerate(train_loader):

        inputs = inputs.to(
            device,
            non_blocking=True,
        )

        targets = targets.to(
            device,
            non_blocking=True,
        )

        optimizer.zero_grad(
            set_to_none=True
        )

        outputs = model(inputs)

        loss = criterion(
            outputs,
            targets,
        )

        loss.backward()
        optimizer.step()

        prof.step()

        if step >= 20:
            break
```

其中

```python
prof.step()
```

非常重要，它告诉 Profiler

```text
当前训练 Step 已结束
进入下一个 Step
```

使用非默认 `schedule` 时，Profiler 根据 Step 计数决定何时 Wait、Warmup、Active 和保存 Trace。

### 10.8.9 Profiler 重要参数

|        参数         |       作用       |
| :---------------: | :------------: |
|   `activities`    | 采集 CPU / CUDA  |
|  `record_shapes`  |   记录输入 Shape   |
| `profile_memory`  | 记录 Tensor 内存分配 |
|   `with_stack`    |    记录代码调用位置    |
|    `schedule`     |     控制采样窗口     |
| `on_trace_ready`  |  Trace 完成后的处理  |
| `record_function` |   自定义代码区域标签    |

这些能力均由当前 `torch.profiler.profile` API 提供。

### 10.8.10 Profiler 本身有开销

不要认为

```text
开启 Profiler 后
训练速度仍代表真实训练速度
```

尤其开启

```python
record_shapes=True
profile_memory=True
with_stack=True
```

会带来额外记录成本。当前官方文档特别提醒，Shape 和 Stack Tracking 会增加额外开销，`record_shapes=True` 还会临时持有 Tensor 引用。

因此正确做法

```text
普通训练：
关闭 Profiler

性能诊断：
短时间开启 Profiler
```

### 10.8.11 Profiler 应重点看什么

**第一：CPU 是否存在明显空洞**

如果

```text
GPU Kernel 之间存在大量空隙
```

可能是

- CPU 没及时提交
- DataLoader
- Python
- 同步
- Kernel Launch

**第二：GPU 最耗时 Kernel**

找到

```text
CUDA Time Top N
```

确认主要计算在哪里。

**第三：Kernel 数量**

如果大量

```text
5 μs
10 μs
20 μs
```

的小 Kernel，可能存在融合空间。

**第四：Memcpy**

大量

```text
HtoD
DtoH
```

说明设备传输可能过多。

**第五：Shape**

检查异常的大 Shape

```text
Sequence Length 突然增大
错误 Broadcast
意外生成 [B,B] Tensor
```

**第六：Memory**

检查

```text
哪个 Operator
产生了显著 CUDA Memory Allocation
```

## 10.9 本章小节

可以把单卡训练的性能问题按照以下流程分析

```text
训练慢
 │
 ├── GPU 利用率低
 │    │
 │    ├── 等数据？
 │    │    ├── I/O
 │    │    ├── Dataset
 │    │    ├── Transform
 │    │    └── DataLoader
 │    │
 │    ├── 等 CPU？
 │    │    ├── Python Loop
 │    │    ├── 小算子
 │    │    └── Kernel Launch
 │    │
 │    └── 等同步？
 │         ├── .item()
 │         ├── .cpu()
 │         └── device copy
 │
 └── GPU 利用率高
      │
      ├── Kernel 本身慢
      ├── MatMul / Conv / Attention
      ├── Memory Bandwidth
      ├── Batch Size
      └── 算法 / Kernel 优化
```

优化原则

```text
数据喂不上
→ 优化 DataLoader

同步太多
→ 减少 Host-Device Dependency

小 Kernel 太多
→ Vectorize / Fuse / Compile

GPU 没吃满
→ 增大合适 Batch

显存不够
→ AMP / Accumulation / Checkpointing

不知道哪里慢
→ torch.profiler
```

完成本章后，应能够独立回答

1. PyTorch 训练慢通常有哪些原因
2. 为什么性能优化必须先 Profile
3. GPU 利用率低通常说明什么
4. GPU 利用率高但训练仍慢应该优化什么
5. 如何判断 DataLoader 是否成为瓶颈
6. `num_workers` 为什么不是越大越好
7. `pin_memory` 对性能有什么作用
8. 为什么 `.item()` 会产生 CPU-GPU 同步
9. 为什么频繁 `.cpu()` 会影响训练吞吐
10. CUDA 异步执行为什么有利于性能
11. 为什么大量小算子会降低 GPU 利用率
12. Kernel Launch Overhead 是什么
13. 算子融合为什么能够加速
14. Batch Size 为什么会影响 GPU 吞吐
15. Batch Size 为什么不能无限增加
16. Batch Size 为什么同时也是优化超参数
17. Gradient Accumulation 的原理是什么
18. 为什么累积时通常要除以 `accumulation_steps`
19. Gradient Accumulation 主要降低哪部分显存
20. Gradient Accumulation 是否是加速技术
21. Activation Checkpointing 的原理是什么
22. Checkpointing 为什么能够减少显存
23. Checkpointing 为什么会增加计算量
24. Gradient Accumulation 和 Checkpointing 有什么区别
25. `torch.profiler` 能采集哪些信息
26. `record_shapes`、`profile_memory`、`with_stack` 分别有什么作用
27. `record_function` 有什么作用
28. Profiler 为什么需要 Wait / Warmup / Active
29. 使用 Schedule 时为什么需要调用 `prof.step()`
30. 如何从 Profiler 中识别 Data、同步和小 Kernel 瓶颈

# 11 分布式训练基础

单机单卡训练中

```text
1 个训练进程
    ↓
1 张 GPU
    ↓
Forward → Backward → Optimizer Step
```

当模型计算量、数据量或训练时间继续增大时，可以让多张 GPU 同时处理不同数据，再同步梯度

```text
GPU 0：Batch 0 → Forward → Backward ┐
GPU 1：Batch 1 → Forward → Backward ├→ 梯度同步 → 各自更新参数
GPU 2：Batch 2 → Forward → Backward ┤
GPU 3：Batch 3 → Forward → Backward ┘
```

PyTorch 中最基础、最重要的数据并行方案是

```python
torch.nn.parallel.DistributedDataParallel
```

简称 DDP。PyTorch 官方明确建议单机多 GPU 数据并行优先使用 DDP；它通常也明显快于 `torch.nn.DataParallel`。

## 11.1 为什么需要分布式训练

### 11.1.1 单卡训练的主要限制

单张 GPU 主要受到两个约束

```text
算力有限
+
显存有限
```

例如单卡训练一个模型，一个 Step 需要

```text
500 ms
```

如果理想情况下使用 8 张 GPU，将一个 Global Batch 分散到 8 个 GPU

```text
GPU 0：一部分数据
GPU 1：一部分数据
...
GPU 7：一部分数据
```

就可以并行完成大量计算。

分布式训练的主要目标之一就是：通过多设备并行计算，提高训练吞吐，缩短训练时间。

### 11.1.2 数据并行的基本思想

假设有 4 张 GPU。

每张 GPU 保存完整模型

```text
GPU 0：Model Replica 0
GPU 1：Model Replica 1
GPU 2：Model Replica 2
GPU 3：Model Replica 3
```

但处理不同的数据

```text
GPU 0：Batch A
GPU 1：Batch B
GPU 2：Batch C
GPU 3：Batch D
```

分别计算本地梯度

```text
GPU 0：g0
GPU 1：g1
GPU 2：g2
GPU 3：g3
```

然后同步

```text
g = Average(g0, g1, g2, g3)
```

最后所有 GPU 都使用相同的 `g` 更新参数。

这就是最基本的：Synchronous Data Parallelism，同步数据并行。

DDP 正是这种思路：每个进程拥有一个完整模型副本，并通过梯度同步保证各副本保持一致。

### 11.1.3 DDP 能解决显存不足吗

这是一个高频误区。

DDP 中每个 GPU 通常仍然需要完整保存

```text
完整模型参数
+
完整梯度
+
完整优化器状态
```

因此：DDP 主要解决计算吞吐问题，而不是单卡放不下模型的问题。

PyTorch 官方对 FSDP 的说明也明确区分了两者：DDP 中每个 Rank 拥有完整模型副本和优化器状态，而 FSDP 会进一步对参数、梯度和优化器状态进行分片。

例如

```text
模型单卡训练需要 30 GB
GPU 显存只有 24 GB
```

增加到

```text
4 × 24 GB GPU
```

并直接使用普通 DDP，模型仍然可能单卡 OOM。

这种问题通常要考虑

- FSDP
- ZeRO
- Tensor Parallel
- Pipeline Parallel
- CPU Offload
- Gradient Checkpointing
- Mixed Precision

### 11.1.4 分布式训练并不一定线性加速

理想状态

```text
1 GPU：100 samples/s
2 GPU：200 samples/s
4 GPU：400 samples/s
8 GPU：800 samples/s
```

实际上通常达不到，因为增加了

```text
梯度通信
进程同步
DataLoader 压力
CPU 开销
通信等待
负载不均衡
```

因此实际训练时间可以粗略理解为

```text
Step Time
=
Compute Time
+
Communication Time
+
Synchronization / Other Overhead
```

DDP 会通过梯度 Bucket 和异步 All-Reduce，尽量让通信与 Backward 计算重叠，从而减少通信暴露在关键路径上的时间。

## 11.2 DataParallel 与 DistributedDataParallel

PyTorch 中容易混淆

```python
nn.DataParallel
```

简称

```text
DP
```

以及

```python
nn.parallel.DistributedDataParallel
```

简称

```text
DDP
```

官方目前更推荐 DDP。

### 11.2.1 DataParallel

使用方式非常简单

```python
model = MyModel().cuda()

model = torch.nn.DataParallel(
    model,
    device_ids=[0, 1, 2, 3],
)
```

DP 的核心模式可以理解为

```text
单个 Python 进程
        ↓
主 GPU
        ↓
切分输入 Batch
        ↓
复制模型到多个 GPU
        ↓
并行 Forward
        ↓
收集结果
        ↓
Backward
        ↓
梯度汇总
```

`DataParallel` 会自动切分输入并将任务发送到多个 GPU，最终再收集各设备的结果。

### 11.2.2 DataParallel 的问题

DP 的重要限制之一是

```text
一个 Python 进程
+
多个执行线程
```

这会产生

- Python 主线程调度压力
- 单主设备数据 Scatter / Gather
- 每轮 Module Replication
- 扩展性较差

因此在多 GPU 训练中，DDP 通常具有更好的性能和扩展性，PyTorch 官方也明确指出单机多 GPU DDP 显著快于 `DataParallel`。

### 11.2.3 DistributedDataParallel

DDP 通常采用

```text
一张 GPU
对应
一个 Python 进程
```

例如 4 GPU

```text
Process 0 → GPU 0 → Model Replica 0
Process 1 → GPU 1 → Model Replica 1
Process 2 → GPU 2 → Model Replica 2
Process 3 → GPU 3 → Model Replica 3
```

每个进程独立执行

```text
DataLoader
Forward
Loss
Backward
Optimizer Step
```

只有需要同步的地方进行通信。

DDP 要求每个进程创建自己的 DDP 实例；它既支持单机多 GPU，也支持多机分布式训练。

### 11.2.4 DP 与 DDP 对比

|    对比项     |  DataParallel  |  DistributedDataParallel  |
| :--------: | :------------: | :-----------------------: |
|     简称     |       DP       |            DDP            |
|    进程模型    |    单进程，多线程     |            多进程            |
| 常见 GPU 映射  |  一个进程控制多 GPU   |         一进程一 GPU          |
|    输入切分    | DP 自动 Scatter  | 通常由 DistributedSampler 分片 |
|    模型副本    | Forward 时复制到设备 |       每个进程长期持有一个副本        |
|    梯度同步    |     汇总到主副本     |   Collective All-Reduce   |
|   单机多卡性能   |      通常较差      |           通常更好            |
|     多机     |      不适合       |            支持             |
| PyTorch 推荐 |      不优先       |            推荐             |

## 11.3 DDP 的基本原理

DDP 最核心的思想可以概括为

```text
模型复制
+
数据切分
+
本地计算
+
梯度同步
```

### 11.3.1 初始化阶段

假设

```text
world_size = 4
```

启动四个进程

```text
Rank 0
Rank 1
Rank 2
Rank 3
```

每个 Rank

```text
创建相同模型
        ↓
绑定自己的 GPU
        ↓
包装 DistributedDataParallel
```

最终

```text
Rank 0 / GPU0：θ0
Rank 1 / GPU1：θ1
Rank 2 / GPU2：θ2
Rank 3 / GPU3：θ3
```

DDP 初始化过程中会确保模型副本状态一致，之后通过同步梯度而不是每一步重新广播所有参数来维持一致性。

### 11.3.2 Forward 阶段

各个 Rank 使用不同数据

```text
Rank 0：
x0 → model → loss0

Rank 1：
x1 → model → loss1

Rank 2：
x2 → model → loss2

Rank 3：
x3 → model → loss3
```

Forward 基本是独立执行的

```text
GPU 0 ── Forward ──┐
GPU 1 ── Forward ──┤ 并行
GPU 2 ── Forward ──┤
GPU 3 ── Forward ──┘
```

DDP 不会替你自动把整个 Dataset 切给各 GPU，这通常由 `DistributedSampler` 完成。

### 11.3.3 Backward 阶段

各 Rank 首先计算自己的局部梯度

```text
Rank 0：
g0 = ∂loss0 / ∂θ

Rank 1：
g1 = ∂loss1 / ∂θ

Rank 2：
g2 = ∂loss2 / ∂θ

Rank 3：
g3 = ∂loss3 / ∂θ
```

然后 DDP 自动同步梯度

```text
g
=
(g0 + g1 + g2 + g3) / 4
```

最终

```text
Rank 0：parameter.grad = g
Rank 1：parameter.grad = g
Rank 2：parameter.grad = g
Rank 3：parameter.grad = g
```

DDP Reducer 会在梯度 Bucket 准备好时启动异步 All-Reduce，并在 backward 完成时保证各 Rank 对应参数的梯度一致。

### 11.3.4 Optimizer 为什么不用再次同步参数

梯度同步完成后

```text
Rank 0 参数：θ
梯度：g

Rank 1 参数：θ
梯度：g
...
```

每个进程分别调用

```python
optimizer.step()
```

因为

```text
初始参数相同
+
梯度相同
+
Optimizer 状态相同
+
更新规则相同
```

所以

```text
θ_new
```

仍然相同。

因此标准 DDP 不需要每个 Step 再把更新后的模型从 Rank 0 广播给所有 GPU。

### 11.3.5 DDP 同步的是梯度，不是 Loss

这是非常容易混淆的地方。

执行

```python
loss.backward()
```

DDP 自动同步

```text
Gradient
```

但

```python
loss.item()
```

仍然只是：当前 Rank 本地 Batch 的 Loss。

如果要获得所有 GPU 的平均 Loss

```python
dist.all_reduce(...)
```

需要自己做指标聚合。

例如

```python
loss_for_log = loss.detach()

dist.all_reduce(
    loss_for_log,
    op=dist.ReduceOp.SUM,
)

loss_for_log /= world_size
```

也就是说

```text
训练所需梯度：
DDP 自动同步

日志指标：
通常需要自己同步
```

### 11.3.6 Bucket 与计算通信重叠

如果等所有参数梯度都计算完，再一次性通信

```text
完整 Backward
      ↓
完整 All-Reduce
```

GPU 可能出现明显通信等待。

DDP 更高效的方式是将梯度组织成 Bucket

```text
Backward

Layer N Gradient Ready
Layer N-1 Gradient Ready
        ↓
Bucket 1 Ready
        ↓
立即启动 Async All-Reduce

同时继续：

Layer N-2 Backward
Layer N-3 Backward
```

即

```text
Backward Compute
      ↘
       Communication
```

形成：Compute-Communication Overlap。

PyTorch DDP Reducer 正是按 Bucket 管理梯度，当一个 Bucket 的梯度全部 Ready 时启动异步 All-Reduce。

## 11.4 `rank`、`world_size`、`process_group`

这三个概念是理解 `torch.distributed` 的基础。

### 11.4.1 world_size

`world_size` 表示：当前 Process Group 中参与分布式计算的进程总数。

例如

```text
4 个训练进程
```

则

```python
world_size = 4
```

注意

```text
world_size
≠ 必然等于机器数量
≠ 必然等于物理 GPU 总数
```

常见 DDP 一进程一 GPU 时

```text
world_size
=
参与训练的 GPU 数量
```

### 11.4.2 rank

`rank` 表示：当前进程在 Process Group 中的全局编号。

如果

```text
world_size = 4
```

则

```text
rank ∈ {0, 1, 2, 3}
```

例如

```text
Rank 0 → GPU 0
Rank 1 → GPU 1
Rank 2 → GPU 2
Rank 3 → GPU 3
```

在单机训练中这种对应关系很直观。

### 11.4.3 local_rank

多机训练还需要

```text
LOCAL_RANK
```

它表示：当前进程在本机上的编号。

例如

```text
2 台机器
每台 4 张 GPU
world_size = 8
```

机器 A

```text
Global Rank     Local Rank

0               0
1               1
2               2
3               3
```

机器 B

```text
Global Rank     Local Rank

4               0
5               1
6               2
7               3
```

因此

```text
RANK：
全局身份

LOCAL_RANK：
当前机器内部的设备身份
```

`torchrun` 会向 worker 提供 `LOCAL_RANK`、`RANK` 和 `WORLD_SIZE` 等分布式环境变量。

### 11.4.4 Process Group

Process Group 可以理解为：一组可以共同执行 Collective Communication 的进程。

初始化

```python
import torch.distributed as dist

dist.init_process_group(
    backend="nccl",
)
```

之后形成默认 Process Group

```text
Rank 0
Rank 1
Rank 2
Rank 3
  │
  └──── Default Process Group
```

可以执行

```python
dist.all_reduce(...)
dist.broadcast(...)
dist.all_gather(...)
dist.barrier()
```

`torch.distributed` 提供这些 Collective Communication Primitive，而 DDP 就构建在这一通信能力之上。

### 11.4.5 Rank 0 的特殊用途

工程中通常约定

```text
Rank 0
```

负责

- 打印全局日志
- 保存 Checkpoint
- 写 TensorBoard
- 创建结果文件

例如

```python
if rank == 0:
    torch.save(
        model.module.state_dict(),
        "model.pt",
    )
```

原因不是：Rank 0 是 DDP 的中央参数服务器。

实际上梯度 All-Reduce 是 Collective 操作，没有一个 Rank 负责集中计算所有梯度。

Rank 0 主要只是工程上的约定。

## 11.5 DistributedSampler 的作用

DDP 本身

```text
只负责模型梯度同步
```

不会自动完成

```text
Dataset 数据分片
```

PyTorch 官方明确说明 DDP 不会自动切分输入数据，通常需要 `DistributedSampler` 负责每个进程的数据子集。

### 11.5.1 不使用 DistributedSampler 会怎样

假设有

```text
Dataset = 10000 samples
4 GPU
```

如果四个 Rank 都创建普通 DataLoader

```text
Rank 0：10000 samples
Rank 1：10000 samples
Rank 2：10000 samples
Rank 3：10000 samples
```

那么

```text
同一份数据
被重复训练 4 次
```

不仅浪费计算，而且实际 Global Batch 语义完全错误。

### 11.5.2 使用 DistributedSampler

```python
from torch.utils.data import (
    DataLoader,
    DistributedSampler,
)

sampler = DistributedSampler(
    train_dataset,
    num_replicas=world_size,
    rank=rank,
    shuffle=True,
)

train_loader = DataLoader(
    train_dataset,
    batch_size=batch_size,
    sampler=sampler,
    num_workers=4,
    pin_memory=True,
)
```

逻辑类似

```text
完整 Dataset
      ↓
DistributedSampler
      ↓

Rank 0 → Subset 0
Rank 1 → Subset 1
Rank 2 → Subset 2
Rank 3 → Subset 3
```

每个进程只读取自己的数据子集。

### 11.5.3 不要再设置 `shuffle=True`

错误

```python
DataLoader(
    dataset,
    sampler=sampler,
    shuffle=True,
)
```

Sampler 已经负责采样顺序。

应该

```python
DataLoader(
    dataset,
    sampler=sampler,
)
```

训练数据是否打乱由

```python
DistributedSampler(
    ...,
    shuffle=True,
)
```

决定。

### 11.5.4 为什么必须 `set_epoch`

训练

```python
for epoch in range(num_epochs):

    train_sampler.set_epoch(epoch)

    for batch in train_loader:
        ...
```

如果不调用

```python
train_sampler.set_epoch(epoch)
```

不同 Epoch 可能持续产生相同的伪随机排列。

PyTorch 官方明确要求，在使用 `shuffle=True` 的 `DistributedSampler` 时，每个 Epoch 创建 DataLoader Iterator 前调用 `set_epoch()`，从而让不同 Epoch 使用不同排序。

### 11.5.5 Global Batch Size

假设

```text
每 GPU Batch Size = 32
world_size = 4
```

那么

```text
Global Batch Size
=
32 × 4
=
128
```

再加梯度累积

```text
Micro Batch = 8
Accumulation Steps = 4
World Size = 8
```

则

```text
Global Effective Batch
=
8 × 4 × 8
=
256
```

这是调整学习率和 Scheduler 时必须明确的概念。

## 11.6 梯度同步与 All-Reduce

All-Reduce 是 DDP 最重要的通信操作之一。

### 11.6.1 什么是 Reduce

假设

```text
Rank 0：Tensor = 1
Rank 1：Tensor = 2
Rank 2：Tensor = 3
Rank 3：Tensor = 4
```

Reduce-Sum 到 Rank 0

```text
Rank 0：
1 + 2 + 3 + 4 = 10
```

其他 Rank 不一定得到结果。

### 11.6.2 什么是 All-Reduce

All-Reduce 则是

```text
所有 Rank 提供数据
       ↓
执行 Reduce
       ↓
所有 Rank 都得到结果
```

例如 SUM

```text
输入：

Rank 0：1
Rank 1：2
Rank 2：3
Rank 3：4

All-Reduce SUM

输出：

Rank 0：10
Rank 1：10
Rank 2：10
Rank 3：10
```

PyTorch DDP 就利用 All-Reduce 同步不同模型副本产生的梯度。

### 11.6.3 DDP 梯度同步

对于参数

```text
W
```

四个 GPU 的本地梯度

```text
g0
g1
g2
g3
```

DDP 需要得到等价于

```text
g =
(g0 + g1 + g2 + g3) / world_size
```

的同步梯度。

完成后

```text
Rank 0 → g
Rank 1 → g
Rank 2 → g
Rank 3 → g
```

官方 DDP Reducer 文档描述的行为正是：梯度 Bucket Ready 后触发异步 All-Reduce，最终平均后的梯度写回对应的 `param.grad`。

### 11.6.4 梯度同步发生在哪里

非常重要

```python
loss.backward()
```

执行过程中，就已经发生 DDP 通信。

并不是

```python
loss.backward()

# backward 结束
dist.all_reduce(...)
```

才开始同步。

DDP 为参数梯度注册 Autograd Hook

```text
Backward
   ↓
某些 Parameter Gradient Ready
   ↓
DDP Reducer 得到通知
   ↓
Bucket Ready
   ↓
启动 Async All-Reduce
```

因此

```text
Backward Computation
和
Gradient Communication
```

可以发生重叠。

### 11.6.5 为什么不用参数服务器

一种简单的分布式设计可以是

```text
GPU 1 ─┐
GPU 2 ─┼→ Rank 0 → 汇总梯度 → 再发送
GPU 3 ─┘
```

此时 Rank 0 容易成为

```text
Bandwidth Bottleneck
```

Collective All-Reduce 则让各 Rank 共同参与通信，不依赖一个中心进程完成全部聚合。

因此 DDP 的 Rank 0

```text
不是中央梯度服务器
```

### 11.6.6 Ring All-Reduce

一个经典 All-Reduce 实现思路是 Ring All-Reduce

```text
GPU0 → GPU1 → GPU2 → GPU3
 ↑                 ↓
 └─────────────────┘
```

数据被切成多个 Chunk，并通过多个阶段完成

```text
Reduce-Scatter
+
All-Gather
```

从而避免把全部通信压力集中到一张 GPU。

PyTorch 的 DDP 教学材料用 Ring All-Reduce 来说明多设备同步梯度的基本通信思想；具体算法和路径最终由所使用的通信后端及硬件环境决定。

### 11.6.7 Gradient Accumulation 与 DDP

假设

```text
Accumulation Steps = 4
```

如果普通写

```python
loss.backward()
```

DDP 每个 Micro-batch 都会同步梯度。

这会产生不必要通信。

DDP 提供

```python
model.no_sync()
```

可以暂时关闭梯度 All-Reduce。PyTorch 性能指南明确推荐在 DDP 梯度累积场景使用 `no_sync()` 避免不必要通信。

例如

```python
for step, batch in enumerate(loader):

    should_update = (
        (step + 1) % accumulation_steps == 0
    )

    if should_update:

        loss = forward_loss(batch)
        loss.backward()

        optimizer.step()
        optimizer.zero_grad(
            set_to_none=True
        )

    else:

        with model.no_sync():
            loss = forward_loss(batch)
            loss.backward()
```

这样

```text
Micro-batch 1：不同步
Micro-batch 2：不同步
Micro-batch 3：不同步
Micro-batch 4：同步
```

## 11.7 NCCL 与 Gloo

`torch.distributed` 本身提供统一接口

```python
dist.all_reduce()
dist.broadcast()
dist.barrier()
```

真正执行底层通信的则是：Communication Backend。

PyTorch 内置后端包括 NCCL 和 Gloo 等。当前官方分布式文档将 NCCL 作为 CUDA GPU 训练的主要后端，将 Gloo 作为 CPU 通信的重要后端。

### 11.7.1 NCCL

NCCL

```text
NVIDIA Collective Communications Library
```

主要面向 NVIDIA GPU 的 Collective Communication。

例如

- All-Reduce
- Broadcast
- All-Gather
- Reduce-Scatter

PyTorch 官方分布式教程指出，NCCL 为 CUDA Tensor 提供高度优化的 Collective Communication；如果主要进行 CUDA Tensor 通信，应优先考虑 NCCL。

典型 DDP

```python
dist.init_process_group(
    backend="nccl"
)
```

### 11.7.2 NCCL 能利用什么硬件

在不同机器配置中，GPU 间通信可能通过

```text
PCIe
NVLink
NVSwitch
InfiniBand / RDMA
高速网络
```

NCCL 会负责大量底层拓扑与 Collective Communication 实现细节。

因此对于 NVIDIA GPU DDP

```text
NCCL
```

通常是首选。

### 11.7.3 Gloo

Gloo 是另一个 Distributed Communication Backend。

常见定位

```text
CPU Distributed Training
CPU Tensor Collective
某些调试和兼容场景
```

PyTorch 当前分布式集成文档将 Gloo 作为 CPU 场景的内置后端。

例如

```python
dist.init_process_group(
    backend="gloo"
)
```

### 11.7.4 NCCL 与 Gloo 对比

|        对比项        |     NCCL      |   Gloo    |
| :---------------: | :-----------: | :-------: |
|       主要目标        | NVIDIA GPU 通信 |  CPU 通信   |
|     CUDA DDP      |      推荐       |   通常非首选   |
|  CPU Distributed  |      不适合      |    常用     |
| GPU Collective 性能 |       高       | 通常不如 NCCL |
|   NVIDIA GPU 训练   |      首选       |  调试/兼容场景  |

### 11.7.5 通信 Backend 不等于训练框架

需要区分

```text
DDP：
分布式训练策略

NCCL：
通信实现
```

关系是

```text
DistributedDataParallel
        ↓
torch.distributed
        ↓
Process Group
        ↓
NCCL / Gloo
        ↓
GPU / Network Hardware
```

因此：DDP 不等于 NCCL，NCCL 也不只服务于 DDP。

## 11.8 DDP 训练的标准流程

下面以最常见的

```text
单机
4 张 NVIDIA GPU
1 GPU / Process
NCCL
torchrun
```

为例。

### 11.8.1 整体流程

```text
torchrun
   ↓
启动 N 个 Python Process
   ↓
每个进程读取：
RANK
LOCAL_RANK
WORLD_SIZE
   ↓
设置当前 CUDA Device
   ↓
init_process_group(NCCL)
   ↓
创建 Dataset
   ↓
DistributedSampler
   ↓
创建 Model
   ↓
移动到 Local GPU
   ↓
包装 DDP
   ↓
每个 Rank 独立训练
   ↓
Backward 自动 All-Reduce Gradient
   ↓
各 Rank Optimizer Step
   ↓
Rank 0 保存 Checkpoint
   ↓
destroy_process_group()
```

### 11.8.2 标准代码

```python
from __future__ import annotations

import os

import torch
import torch.distributed as dist
from torch import nn
from torch.nn.parallel import (
    DistributedDataParallel as DDP,
)
from torch.utils.data import (
    DataLoader,
    DistributedSampler,
)


def setup_distributed():
    local_rank = int(
        os.environ["LOCAL_RANK"]
    )
    rank = int(
        os.environ["RANK"]
    )
    world_size = int(
        os.environ["WORLD_SIZE"]
    )

    torch.cuda.set_device(
        local_rank
    )

    dist.init_process_group(
        backend="nccl"
    )

    device = torch.device(
        "cuda",
        local_rank,
    )

    return (
        rank,
        local_rank,
        world_size,
        device,
    )
```

`torchrun` 会为 Worker 提供 `LOCAL_RANK`、`RANK` 和 `WORLD_SIZE`，NCCL 是官方推荐的 NVIDIA GPU DDP Backend。

**构建 DataLoader**

```python
def build_loader(
    dataset,
    *,
    batch_size,
    rank,
    world_size,
):
    sampler = DistributedSampler(
        dataset,
        num_replicas=world_size,
        rank=rank,
        shuffle=True,
    )

    loader = DataLoader(
        dataset,
        batch_size=batch_size,
        sampler=sampler,
        num_workers=4,
        pin_memory=True,
        persistent_workers=True,
    )

    return loader, sampler
```

**构建模型**

```python
def build_model(
    device,
    local_rank,
):
    model = MyModel()

    model = model.to(device)

    model = DDP(
        model,
        device_ids=[
            local_rank
        ],
        output_device=local_rank,
    )

    return model
```

在一进程一 GPU 的标准 CUDA DDP 中，应先让当前进程绑定自己的 GPU，再让 DDP 使用该设备。

**训练**

```python
def train_one_epoch(
    model,
    loader,
    optimizer,
    criterion,
    device,
):
    model.train()

    for inputs, targets in loader:

        inputs = inputs.to(
            device,
            non_blocking=True,
        )

        targets = targets.to(
            device,
            non_blocking=True,
        )

        optimizer.zero_grad(
            set_to_none=True
        )

        outputs = model(inputs)

        loss = criterion(
            outputs,
            targets,
        )

        # DDP 的梯度通信主要就在
        # backward 过程中自动发生。
        loss.backward()

        optimizer.step()
```

DDP Reducer 会在 backward 期间负责梯度 Bucket 的 All-Reduce。

**主训练循环**

```python
def main():

    (
        rank,
        local_rank,
        world_size,
        device,
    ) = setup_distributed()

    train_dataset = MyDataset(...)

    train_loader, train_sampler = (
        build_loader(
            train_dataset,
            batch_size=32,
            rank=rank,
            world_size=world_size,
        )
    )

    model = build_model(
        device,
        local_rank,
    )

    criterion = nn.CrossEntropyLoss()

    optimizer = torch.optim.AdamW(
        model.parameters(),
        lr=1e-3,
    )

    num_epochs = 20

    for epoch in range(
        num_epochs
    ):

        # 非常重要：
        # 让每个 epoch 的 shuffle 顺序变化。
        train_sampler.set_epoch(
            epoch
        )

        train_one_epoch(
            model=model,
            loader=train_loader,
            optimizer=optimizer,
            criterion=criterion,
            device=device,
        )

        if rank == 0:
            print(
                f"epoch={epoch} finished"
            )

    if rank == 0:
        torch.save(
            model.module.state_dict(),
            "model.pt",
        )

    dist.destroy_process_group()


if __name__ == "__main__":
    main()
```

`DistributedSampler.set_epoch()` 应在每个 Epoch 开始迭代前调用；完成分布式工作后可以显式销毁 Process Group。

**torchrun 启动**

4 张 GPU

```bash
torchrun \
    --standalone \
    --nproc-per-node=4 \
    train.py
```

含义

```text
--standalone
单机 Rendezvous 配置

--nproc-per-node=4
当前机器启动 4 个 Worker Process
```

在典型 CUDA DDP 中

```text
Process 0 → GPU 0
Process 1 → GPU 1
Process 2 → GPU 2
Process 3 → GPU 3
```

`torchrun` 是 PyTorch 当前官方提供的 Elastic/Distributed Worker 启动工具。

### 11.8.3 为什么使用 `model.module`

DDP 包装后

```python
model
```

实际上类似

```text
DistributedDataParallel
└── module
    └── 原始 MyModel
```

所以保存裸模型权重常用

```python
model.module.state_dict()
```

而不是将 DDP Wrapper 本身作为模型结构保存。

### 11.8.4 Validation 怎么做

有两种常见思路。

**方案一：只在 Rank 0 验证**

简单

```text
Rank 0：
完整 Validation

其他 Rank：
等待
```

适合验证集较小。

但其他 GPU 会闲置。

**方案二：分布式验证**

每个 Rank 验证一部分

```text
Rank 0：Validation Subset 0
Rank 1：Validation Subset 1
...
```

然后通过

```python
dist.all_reduce(...)
```

汇总

- correct
- sample count
- loss sum
- confusion matrix 等

对于大型验证集通常更高效。

### 11.8.5 指标同步示例

```python
correct = torch.tensor(
    local_correct,
    device=device,
    dtype=torch.float64,
)

total = torch.tensor(
    local_total,
    device=device,
    dtype=torch.float64,
)

dist.all_reduce(
    correct,
    op=dist.ReduceOp.SUM,
)

dist.all_reduce(
    total,
    op=dist.ReduceOp.SUM,
)

accuracy = (
    correct / total
)

if rank == 0:
    print(
        "accuracy=",
        accuracy.item(),
    )
```

再次强调

```text
Gradient
→ DDP 自动同步

Metric
→ 通常开发者自己同步
```

### 11.8.6 Checkpoint 只由 Rank 0 保存

错误

```python
# 四个 Rank 同时写同一个文件
torch.save(
    model.module.state_dict(),
    "model.pt",
)
```

可能造成多个进程同时写文件。

正确

```python
if rank == 0:
    torch.save(
        {
            "model":
                model.module.state_dict(),

            "optimizer":
                optimizer.state_dict(),

            "epoch":
                epoch,
        },
        "checkpoint.pt",
    )
```

如果其他 Rank 必须等待文件完成，可以根据流程加入

```python
dist.barrier()
```

`barrier()` 是 Collective Synchronization，只有 Process Group 中相关 Rank 都到达后才能继续。

## 11.9 本章小节

```text
                    Dataset
                       │
             DistributedSampler
                       │
        ┌──────────────┼──────────────┐
        ↓              ↓              ↓
      Rank 0         Rank 1         Rank 2
      GPU 0          GPU 1          GPU 2
        │              │              │
     Batch 0         Batch 1        Batch 2
        │              │              │
     Forward         Forward        Forward
        │              │              │
      Loss 0          Loss 1         Loss 2
        │              │              │
     Backward        Backward       Backward
        │              │              │
       g0             g1             g2
        └──────────────┼──────────────┘
                       │
                   All-Reduce
                       │
           g = Average(g0,g1,g2)
                       │
        ┌──────────────┼──────────────┐
        ↓              ↓              ↓
      Rank 0         Rank 1         Rank 2
     grad = g        grad = g        grad = g
        │              │              │
 optimizer.step() optimizer.step() optimizer.step()
        │              │              │
       θ'             θ'             θ'
```

|           问题            |            正确理解            |
| :---------------------: | :------------------------: |
|   DDP 是否自动切 Dataset？    | 否，通常用 `DistributedSampler` |
|  DDP 是否自动同步 Gradient？   |             是              |
|    DDP 是否自动同步 Loss？     |             否              |
|     DDP 是否每步同步完整参数？     |        不需要，核心是同步梯度         |
|     每张 GPU 是否有完整模型？     |             是              |
| 每张 GPU 是否有独立 Optimizer？ |            通常是             |
| 各 Optimizer 更新后参数是否一致？  |           正常情况下是           |
|    Rank 0 是否是参数服务器？     |             不是             |
|  DDP 是否能让单卡放不下的模型直接运行？  |            通常不能            |
|  CUDA DDP 推荐 Backend？   |            NCCL            |

为了避免后续概念混乱，可以先建立

```text
DDP
│
├── 每卡完整模型
├── 不同数据
└── 同步梯度
        ↓
主要解决：
训练吞吐
```

```text
FSDP / ZeRO
│
├── 分片 Parameter
├── 分片 Gradient
└── 分片 Optimizer State
        ↓
主要解决：
模型状态显存
```

```text
Tensor / Pipeline Parallel
│
└── 模型计算本身跨设备切分
        ↓
主要解决：
模型或单层本身无法放入一张 GPU
```

PyTorch 对 FSDP 的官方介绍也正是以 DDP 的完整模型复制为对比，说明 FSDP 通过状态分片降低单 Rank 显存。

完成本章后，应能够独立回答

1. 为什么需要分布式训练
2. 数据并行的基本原理是什么
3. DDP 是否能够直接解决单卡模型 OOM
4. DataParallel 和 DDP 有什么区别
5. 为什么 PyTorch 更推荐 DDP
6. 为什么 DDP 通常采用一进程一 GPU
7. DDP 的 Forward 和 Backward 分别发生什么
8. DDP 为什么只同步梯度就能保证参数一致
9. DDP 是否自动同步 Loss
10. Gradient Bucket 有什么作用
11. 为什么梯度通信可以与 Backward 计算重叠
12. `rank` 是什么
13. `local_rank` 是什么
14. `world_size` 是什么
15. `process_group` 是什么
16. Rank 0 是否是中央参数服务器
17. DistributedSampler 解决什么问题
18. 为什么不能让所有 Rank 都遍历完整 Dataset
19. 为什么每个 Epoch 要调用 `sampler.set_epoch(epoch)`
20. DDP 的 Global Batch Size 如何计算
21. All-Reduce 是什么
22. Reduce 和 All-Reduce 有什么区别
23. DDP 的 All-Reduce 在什么时候发生
24. Ring All-Reduce 的基本思想是什么
25. 梯度累积为什么可以配合 `no_sync()`
26. NCCL 和 Gloo 有什么区别
27. CUDA DDP 为什么通常选择 NCCL
28. `torchrun` 的作用是什么
29. DDP 为什么通常只让 Rank 0 写日志和 Checkpoint
30. 为什么分布式 Metric 通常需要额外 All-Reduce
31. DDP、FSDP 和模型并行分别主要解决什么问题

# 12 PyTorch 编译与图优化

前面介绍的 PyTorch 默认执行方式属于 Eager Execution

```text
执行一行 Python
    ↓
立即调度对应 PyTorch Operator
    ↓
执行 CUDA Kernel / CPU Kernel
    ↓
继续执行下一行
```

这种模式灵活、易调试，也是 PyTorch 长期以来最重要的开发体验。但从性能角度看，它也意味着很多操作只能逐个执行，编译器很难提前观察更大的计算范围并进行全局优化。

`torch.compile` 的目标就是：尽量保持 PyTorch 原有 Python 编程方式，同时把可以捕获的计算转换成计算图，再交给编译器进行融合、代码生成和运行时优化。

当前 PyTorch 的 `torch.compile` 主要建立在 TorchDynamo、AOTAutograd 和 TorchInductor 三个核心组件之上；TorchInductor 是默认后端，在 GPU 上会大量利用 Triton 进行代码生成。

可以先建立整体认识

```text
普通 PyTorch

Python
  ↓
Eager Operator
  ↓
CUDA Kernel
```

编译模式：

```text
Python / nn.Module
        ↓
   TorchDynamo
   捕获计算图
        ↓
   AOTAutograd
捕获/生成反向图
        ↓
   TorchInductor
融合 + 代码生成
        ↓
Triton / C++ / Backend Kernel
        ↓
        GPU / CPU
```

## 12.1 Eager Execution 的优点与缺点

### 12.1.1 什么是 Eager Execution

Eager 可以理解为：代码执行到哪里，PyTorch 就立即执行到哪里。

例如

```python
def forward(x, weight):
    x = torch.matmul(x, weight)
    x = torch.relu(x)
    x = x + 1
    return x
```

在 Eager Mode 中，可以粗略理解为

```text
Python 调用 torch.matmul
        ↓
执行 MatMul

Python 调用 torch.relu
        ↓
执行 ReLU

Python 调用 add
        ↓
执行 Add
```

PyTorch 并不要求用户事先构造完整静态计算图，这也是其动态图编程体验的基础。

### 12.1.2 Eager 的最大优点：灵活

例如

```python
def forward(self, x):
    if x.mean() > 0:
        x = self.block_a(x)
    else:
        x = self.block_b(x)

    return x
```

模型行为可以直接由

```text
Python 控制流
+
Tensor 数据
```

决定。

还可以使用

- `if`
- `for`
- `while`
- Python 函数
- 动态模块调用
- 不同 Shape
- 自定义数据结构

这种能力让模型代码非常接近普通 Python，而不是要求用户先学习一套独立的静态图 DSL。

### 12.1.3 Eager 非常适合 Debug

例如

```python
x = self.layer1(x)

print(x.shape)

x = self.layer2(x)

breakpoint()

x = self.layer3(x)
```

可以直接

- 打印 Tensor
- 使用 Python Debugger
- 插入断点
- 修改控制流
- 检查中间变量

这也是 Eager 对

```text
模型研究
算法开发
快速实验
Bug 定位
```

非常友好的原因。

### 12.1.4 Eager 的问题：Python 调度开销

假设

```python
x = x + 1
x = x * 2
x = torch.relu(x)
x = x.square()
```

Eager 可能产生

```text
Python
 ↓
Add Kernel
 ↓
Python
 ↓
Multiply Kernel
 ↓
Python
 ↓
ReLU Kernel
 ↓
Python
 ↓
Square Kernel
```

如果每个 Kernel 的计算量很大

```text
大型 MatMul
大型 Convolution
```

Python 开销相对很小。

但如果模型包含大量

```text
小 Tensor Operator
小 Pointwise Operator
短 Kernel
```

CPU/Python 的调度开销可能占据明显比例。

PyTorch 官方 `torch.compile` 教程指出，编译后的性能收益主要来源之一就是减少 Python 开销；实际收益与模型架构和 Batch Size 有关。

### 12.1.5 Eager 的问题：难以跨算子优化

考虑

```python
y = x + 1
z = torch.relu(y)
out = z * 2
```

Eager 视角更接近

```text
Add
↓
写入中间 Tensor y

ReLU
↓
读取 y
↓
写入 z

Multiply
↓
读取 z
↓
写入 out
```

如果编译器能够看到整个计算

```text
x
↓
Add
↓
ReLU
↓
Multiply
↓
out
```

就可能把部分操作融合成一个 Kernel

```text
读取 x
    ↓
+1 → ReLU → ×2
    ↓
写出 out
```

从而减少

- Kernel Launch
- 中间 Tensor
- GPU Global Memory 读写

PyTorch 官方教程将减少 Python Overhead 和 GPU Read/Write 视为 `torch.compile` 加速的重要来源。

### 12.1.6 Eager 与 Compile 的本质差异

可以概括为

```text
Eager：

一次看到一个操作
立即执行
```

而

```text
Compile：

先尽可能观察一段计算
形成 Graph
再进行整体优化
```

## 12.2 为什么需要 `torch.compile`

### 12.2.1 性能优化需要更大的计算视野

假设模型

```python
def forward(x):
    x = x + 1
    x = torch.relu(x)
    x = x * 2
    return x
```

Eager

```text
Add
↓
ReLU
↓
Multiply
```

编译器希望得到

```text
Graph
{
    Add
    ReLU
    Multiply
}
```

然后才能进一步进行

```text
Graph Optimization
      ↓
Operator Fusion
      ↓
Backend Code Generation
      ↓
Optimized Kernel
```

这也是计算图优化的基本逻辑。

### 12.2.2 `torch.compile` 的设计目标

历史上的编译系统经常要求用户

```text
修改模型代码
限制 Python 特性
使用新的静态图语言
```

而 `torch.compile` 希望

```python
model = MyModel()

model = torch.compile(model)
```

即可尝试优化原有 PyTorch 程序。

官方将 `torch.compile` 描述为通过追踪 Python 代码中的 PyTorch 操作并 JIT 编译为优化 Kernel，以较少代码改动提升性能。

### 12.2.3 `torch.compile` 主要解决什么问题

主要包括

**Python Overhead**

减少

```text
Python
→ Dispatcher
→ Kernel Launch
```

频繁往返。

**Kernel Fusion**

将多个可融合操作

```text
Add
ReLU
Mul
```

合成更少的 Kernel。

**减少中间显存读写**

例如

```text
Kernel A
写 Tensor A

Kernel B
读 Tensor A
写 Tensor B
```

可能变成

```text
Fused Kernel
内部完成 A → B
```

**Backend 优化**

针对

- CPU
- NVIDIA GPU
- AMD GPU
- Intel GPU

生成更加合适的代码。

TorchInductor 是默认 `torch.compile` Backend；PyTorch 当前文档说明，它在 NVIDIA、AMD 和 Intel GPU 上以 Triton 作为关键代码生成组件。

### 12.2.4 `torch.compile` 不是模型导出

必须区分

```text
torch.compile
```

和

```text
torch.export
```

`torch.compile` 的主要目标是

```text
在当前 PyTorch Runtime 中
优化程序执行性能
```

而 `torch.export` 更强调

```text
捕获可序列化的单一计算图
用于后续部署或其他 Python-less Runtime
```

官方教程明确指出，若目标是序列化 Graph 或运行在不同的、无 Python 的环境，应考虑 `torch.export`；`torch.export` 不允许普通 `torch.compile` 那样通过 Graph Break 分段继续执行。

## 12.3 `torch.compile` 的基本用法

### 12.3.1 编译模型

最常见写法

```python
model = MyModel().cuda()

model = torch.compile(model)
```

之后训练代码基本不变

```python
optimizer = torch.optim.AdamW(
    model.parameters(),
    lr=1e-3,
)

for inputs, targets in train_loader:
    inputs = inputs.cuda()
    targets = targets.cuda()

    optimizer.zero_grad(
        set_to_none=True
    )

    outputs = model(inputs)

    loss = criterion(
        outputs,
        targets,
    )

    loss.backward()

    optimizer.step()
```

`torch.compile` 支持 `nn.Module` 和普通 Python Callable；`nn.Module` 当前也提供 `.compile()` 方法，可原地编译其调用路径。

### 12.3.2 编译普通函数

```python
def function(x, y):
    x = torch.sin(x)
    y = torch.cos(y)

    return x + y


compiled_function = torch.compile(
    function
)
```

也可以作为 Decorator

```python
@torch.compile
def function(x):
    return torch.relu(x + 1)
```

官方教程同时支持这两种基本形式。

### 12.3.3 默认 Backend

默认

```python
torch.compile(
    model,
    backend="inductor",
)
```

即

```text
TorchInductor
```

官方将 Inductor 定位为性能和编译开销之间较均衡的默认 Backend。

### 12.3.4 `mode`

当前主要模式包括

```python
torch.compile(
    model,
    mode="default",
)
```

以及

```python
mode="reduce-overhead"
```

```python
mode="max-autotune"
```

```python
mode="max-autotune-no-cudagraphs"
```

当前官方定义大致是

|             Mode             |                     定位                     |
| :--------------------------: | :----------------------------------------: |
|          `default`           |                 性能与编译开销平衡                  |
|      `reduce-overhead`       | 尝试借助 CUDA Graphs 降低 Python/Launch Overhead |
|        `max-autotune`        |     更积极尝试 MatMul/Conv 等实现与 Autotuning      |
| `max-autotune-no-cudagraphs` |      类似 max-autotune，但不使用 CUDA Graphs      |

其中 `reduce-overhead` 可能使用更多内存保存 CUDA Graph Workspace，也不是所有计算图都适用；`max-autotune` 通常会增加编译/调优成本。

入门阶段通常先

```python
model = torch.compile(model)
```

而不是一开始就

```python
mode="max-autotune"
```

### 12.3.5 `fullgraph`

默认

```python
torch.compile(
    model,
    fullgraph=False,
)
```

意味着：能编译的区域尽量编译，遇到无法捕获的区域允许发生 Graph Break。

如果

```python
torch.compile(
    model,
    fullgraph=True,
)
```

则要求目标函数能够形成一个完整 Graph；如果发生 Graph Break，会直接报错。官方也推荐 `fullgraph=True` 作为定位和消除 Graph Break 的重要手段。

### 12.3.6 Dynamic Shape

现实任务可能

```text
Batch Size：
32 → 27

Sequence Length：
128 → 256 → 512
```

Shape 不固定。

`torch.compile` 需要在

```text
Static Specialization
```

和

```text
Dynamic Shape
```

之间权衡。

当前 `dynamic` 参数

```python
torch.compile(
    model,
    dynamic=None,
)
```

默认会在发生 Shape 变化和重新编译后尝试更动态的策略；`dynamic=False` 强制静态特化，`dynamic=True` 会更积极尝试生成可处理变化 Shape 的 Kernel，但部分操作或优化仍可能迫使其特化。

### 12.3.7 第一次执行通常更慢

例如

```python
compiled_model = torch.compile(
    model
)

output = compiled_model(inputs)
```

第一次或前几次调用可能明显更慢，因为需要

```text
Dynamo Trace
    ↓
Graph Capture
    ↓
AOTAutograd
    ↓
Inductor Compile
    ↓
Triton / C++ Code Generation
```

之后满足相同 Guard 的输入可以复用缓存后的编译结果。

官方教程明确指出，初次 `torch.compile` 执行包含编译开销，只有重复执行后才能公平观察稳态收益。

因此 Benchmark

```text
Eager
vs
Compile
```

必须包含 Warmup。

## 12.4 TorchDynamo、AOTAutograd、TorchInductor

这是本章最重要的体系结构。

可以概括为

```text
Python Program
       ↓
   TorchDynamo
       ↓
Forward FX Graph
       ↓
   AOTAutograd
   ↙         ↘
Forward Graph Backward Graph
       ↓
   TorchInductor
       ↓
Optimized Kernel
```

官方当前也正是用这三个组件解释 `torch.compile` 的主要编译链路。

### 12.4.1 TorchDynamo：捕获 Python 程序中的计算图

TorchDynamo 是编译链的前端。

其主要工作可以理解为：观察 Python Frame 的执行，把可以表示成 PyTorch Graph 的部分捕获出来。

官方说明 Dynamo 利用 CPython 的 Frame Evaluation API 捕获 PyTorch Graph。

例如

```python
def forward(x, weight):
    x = torch.matmul(x, weight)
    x = torch.relu(x)
    x = x + 1

    return x
```

Dynamo 希望捕获

```text
x ─────────┐
           ↓
        MatMul
           ↓
          ReLU
           ↓
          Add
           ↓
         output
```

形成类似

```text
FX Graph
```

的中间表示。

### 12.4.2 Dynamo 不负责最终高性能 Kernel

需要注意

```text
TorchDynamo
≠
最终 GPU Compiler
```

它更像

```text
Python / PyTorch Graph Capture Frontend
```

主要负责

- 捕获 Graph
- 处理 Python Frame
- 建立 Guards
- 在需要时 Recompile
- 处理 Graph Break

捕获出来的 Graph 再交给 Backend。

### 12.4.3 Guard

假设第一次运行

```python
x.shape == [32, 768]
x.dtype == float16
device == cuda
```

Dynamo 编译出的 Graph 可能依赖这些条件。

于是建立类似

```text
Guard:

shape 满足条件？
dtype 满足条件？
device 满足条件？
某些 Python 值是否仍一致？
```

如果下一次调用满足

```text
Guard PASS
```

就直接复用已编译结果。

如果

```text
Guard FAIL
```

就可能重新捕获和重新编译。

当前 `torch.compile` 官方 API 明确说明，单个 Python Frame 可能因 Guard Failure 被多次编译，并将结果缓存到对应 Code Object。

### 12.4.4 Recompilation

例如第一次

```text
Batch = 32
```

第二次

```text
Batch = 16
```

如果 Graph 被特化为

```text
Batch == 32
```

则可能

```text
Guard Failure
     ↓
Recompile
```

频繁 Shape 变化

```text
128
256
384
512
...
```

可能导致

```text
Compile
Compile
Compile
Compile
```

从而吞掉原本想获得的性能收益。

当前 PyTorch 的默认策略会在发现 Shape 动态变化后尝试更动态的 Kernel，但并不保证任何动态图都只需编译一次。

### 12.4.5 AOTAutograd：捕获反向计算

如果只有 Forward Graph

```text
Forward
→ Compile
```

训练中的

```text
Backward
```

仍有很大优化空间。

AOTAutograd 的作用是：根据捕获的前向计算，在 Ahead-of-Time 阶段生成可编译的反向图。

官方文档说明，`torch.compile` 训练路径使用 AOTAutograd 为 Dynamo 捕获到的 Forward Segment 生成 Backward Graph，再让后端编译前向和反向图。

概念上

```text
Forward Graph
        ↓
   AOTAutograd
    ↙       ↘
Forward    Backward
Graph       Graph
```

### 12.4.6 为什么 Backward 也需要编译

Eager Autograd

```text
Backward Operator 1
Backward Operator 2
Backward Operator 3
...
```

同样可能存在

- Python/Engine 调度
- 小算子
- 中间内存访问
- 可融合运算

如果将 Backward 也转成较大的 Graph

```text
Backward Graph
     ↓
Compiler
```

就可以进一步优化训练。

### 12.4.7 TorchInductor：真正的默认编译后端

TorchInductor 位于

```text
Graph Capture
      ↓
Graph Transformation
      ↓
Code Generation
```

的后端位置。

它会对 Graph 进行优化，然后针对硬件生成执行代码。TorchInductor 是当前 `torch.compile` 的默认 Backend。

GPU

```text
TorchInductor
     ↓
Triton / GPU Code Generation
     ↓
GPU Kernel
```

CPU 则使用相应 CPU 代码生成路径。

### 12.4.8 Triton 在哪里

需要区分

```text
torch.compile
TorchDynamo
AOTAutograd
TorchInductor
Triton
```

它们不是同一层。

可以理解

```text
torch.compile
     ↓
统一用户入口

TorchDynamo
     ↓
Python Graph Capture

AOTAutograd
     ↓
Autograd Graph

TorchInductor
     ↓
Compiler Backend

Triton
     ↓
GPU Kernel Code Generation 的重要基础
```

当前 PyTorch 官方说明 TorchInductor 在 NVIDIA、AMD 和 Intel GPU Backend 上利用 Triton 作为关键构建模块。

### 12.4.9 一句话记忆

```text
Dynamo：
抓图

AOTAutograd：
抓/生成反向图

Inductor：
优化图并生成高性能代码
```

## 12.5 Graph Break 是什么

Graph Break 是 `torch.compile` 中最重要的概念之一。

### 12.5.1 为什么会发生 Graph Break

假设 Dynamo 正在捕获

```text
Operator A
Operator B
Operator C
Operator D
```

如果在 B 和 C 之间遇到当前无法继续安全捕获的 Python 行为

```text
Graph 1
A → B
    ↓
Graph Break
    ↓
Eager Python
    ↓
Graph 2
C → D
```

这就是：Graph Break：一个原本希望连续捕获的计算区域被切成多个 Graph。

默认 `fullgraph=False` 时，Dynamo 可以让未捕获部分以 Eager 方式执行，并在后续继续寻找可编译区域；这使 `torch.compile` 能兼容更多 Python 代码，但也意味着优化范围被缩小。

### 12.5.2 Graph Break 不一定是错误

这一点很重要。

例如

```text
Graph 1
   ↓
Graph Break
   ↓
Eager
   ↓
Graph 2
```

程序仍可能

```text
正常运行
结果正确
```

只是

```text
Optimization Opportunity ↓
```

官方教程明确把 Graph Break 描述为“失去优化机会”，而不是默认直接产生错误。

### 12.5.3 为什么 Graph Break 会降低性能

例如原本

```text
A → B → C → D → E
```

可以整体优化

```text
Fused Graph
```

发生 Graph Break 后

```text
A → B

Eager C

D → E
```

编译器只能分别优化两个较小区域。

于是可能失去

- 跨 Graph Fusion
- 全局 Memory Planning
- 更大的 Kernel Fusion
- Python Overhead 消除机会

Graph Break 越频繁，编译收益通常越容易下降。

### 12.5.4 常见 Graph Break 来源

常见原因包括

**不支持或难以追踪的 Python 行为**

例如某些

- C Extension
- 特殊对象操作
- 动态 Python Side Effect
- 不受支持的第三方代码

**Printing / Logging**

例如

```python
print(x)
```

当前官方 Common Graph Breaks 文档将 Printing、Logging 和 Warning 归为会触发 Graph Break 的常见行为。

**Tensor Data → Python 控制**

例如

```python
value = x.sum().item()

if value > 0:
    ...
```

这要求从 Tensor 世界回到 Python Scalar/Control Flow，往往需要额外处理，并可能限制图捕获。

**显式 Graph Break**

调试时甚至可以

```python
torch._dynamo.graph_break()
```

主动制造分界。

### 12.5.5 `fullgraph=True`

调试 Graph Break 时

```python
compiled_model = torch.compile(
    model,
    fullgraph=True,
)
```

如果存在无法捕获的代码

```text
直接 Error
```

而不是悄悄

```text
Graph 1
→ Eager
→ Graph 2
```

因此 `fullgraph=True` 很适合回答：“我的这个函数到底能不能被完整捕获？”

官方明确推荐用它来识别并消除 Graph Break。

### 12.5.6 Debug Backend

如果 Compile 出错，可以逐层定位。

**第一步**

```python
torch.compile(
    model,
    backend="eager",
)
```

它主要测试

```text
TorchDynamo Graph Capture
```

而捕获到的 Graph 仍用 Eager 执行。

如果这里失败

```text
问题大概率在 Dynamo / Graph Capture
```

**第二步**

```python
torch.compile(
    model,
    backend="aot_eager",
)
```

加入

```text
AOTAutograd
```

但不使用 Inductor Codegen。

如果

```text
eager 正常
aot_eager 失败
```

重点检查 AOTAutograd 路径。

**第三步**

```python
torch.compile(
    model,
    backend="inductor",
)
```

完整

```text
Dynamo
+
AOTAutograd
+
Inductor
```

这是 PyTorch 官方 FAQ 推荐的 Compile 故障分层定位方法。

### 12.5.7 Graph Break 与 Recompile 不一样

这两个概念需要严格区分。

**Graph Break**

```text
这一段代码
当前无法继续形成同一个 Graph
```

因此

```text
Graph 被切断
```

**Recompile**

已经存在

```text
Compiled Graph A
```

但新输入

```text
Guard 不满足
```

于是

```text
重新生成 Graph B
```

例如

```text
Graph Break：
程序结构导致图被切开

Recompile：
运行条件变化导致旧图不能复用
```

这两个问题都可能降低性能，但原因不同。

## 12.6 `torch.compile` 的适用场景

### 12.6.1 大量重复执行的模型

由于 Compile 存在 Cold Start

```text
第一次：
Trace + Compile + Execute

后续：
Reuse Compiled Code
```

因此特别适合

```text
同一个模型
+
大量 Step
```

例如

- 长时间训练
- 在线服务持续推理
- Benchmark
- 大规模 Batch 推理

如果只执行

```text
1～2 次 Forward
```

编译成本可能比节省的时间还大。

官方教程明确指出首次执行需要额外编译时间，只有后续能够复用编译结果时才能获得收益。

### 12.6.2 大量 Pointwise / 小算子

例如

```python
x = x + bias
x = torch.sigmoid(x)
x = x * gate
x = x + residual
```

如果模型拥有大量类似组合

```text
许多小 Kernel
+
大量 Global Memory Read/Write
```

Inductor 的 Fusion 更可能带来收益。

官方教程指出 `torch.compile` 的性能收益很大一部分来自降低 Python Overhead 和 GPU Memory Read/Write，因此这类 Operator-heavy 代码是重要适用方向。

### 12.6.3 Transformer 与深度学习模型训练

现代模型通常含有

```text
MatMul
Attention
LayerNorm
Activation
Residual
Pointwise Ops
```

其中部分算子组合具有 Fusion 和 Codegen 优化空间。

`torch.compile` 当前支持训练，并通过 AOTAutograd 捕获 Backward Graph；官方也明确说明支持 DDP。

### 12.6.4 推理

推理时

```python
model.eval()

model = torch.compile(model)
```

也可以使用 Compile。

因为没有

```text
Backward
Optimizer
```

整个执行路径通常更加简单。

对于小 Batch、Python/Kernel Launch Overhead 明显的 CUDA 工作负载，还可以测试

```python
mode="reduce-overhead"
```

该模式会在满足条件时利用 CUDA Graphs 降低 Python Overhead。

### 12.6.5 Shape 相对稳定

假设

```text
Batch = 32
Sequence Length = 512
Hidden Size = 768
```

长期稳定，编译器通常更容易

```text
Specialize
Optimize
Reuse
```

如果每个 Batch

```text
Sequence Length
100
287
493
1021
...
```

则可能增加动态 Shape 管理和 Recompilation 的复杂性。

当前 PyTorch 支持动态 Shape 编译，但动态维度、数据依赖输出和部分特殊操作仍会提高编译复杂度，因此 Shape 越稳定，通常越容易获得可预测的编译收益。

### 12.6.6 如何判断该不该用

不要问

```text
torch.compile 好不好？
```

应该问

```text
这个具体模型
+
这个 GPU
+
这个 Shape
+
这个 Batch Size
```

能不能得到收益。

建议比较

```text
Eager Throughput
vs
Compile Throughput
```

以及

```text
Compile Time
Peak Memory
Latency
Recompile Count
Graph Break
Accuracy
```

## 12.7 `torch.compile` 的局限性

`torch.compile` 很重要，但不能理解成

```text
model = torch.compile(model)

→ 自动把所有模型变快
```

### 12.7.1 编译需要时间

Compile 本身包含

```text
Trace
AOTAutograd
Graph Optimization
Code Generation
Triton Compilation
可能的 Autotuning
```

因此

```text
Cold Start Latency ↑
```

官方教程展示的一个核心现象就是：首次 Compile 执行明显更慢，而后续复用编译结果后才可能得到加速。

所以

```text
模型执行次数很少
```

可能不适合编译。

### 12.7.2 Graph Break 会限制优化效果

如果代码

```text
Graph
↓
Break
↓
Graph
↓
Break
↓
Graph
```

最终就变成很多小图。

这会减少

- Fusion
- 跨算子优化
- Python Overhead 消除机会

Graph Break 是 `torch.compile` 性能分析中的核心概念。

### 12.7.3 Recompilation

假设不断出现

```text
Shape A
Shape B
Shape C
Shape D
```

如果旧 Graph 的 Guard 不满足

```text
Recompile
```

过多重编译会带来大量编译时间。

当前 `torch.compile` 会为一个 Code Object 缓存多个编译结果，并存在 Recompile Limit；超过限制后可以回退 Eager。

因此性能问题可能不是

```text
Compiled Kernel 太慢
```

而是

```text
不停重新编译
```

### 12.7.4 Dynamic Shape 更复杂

动态图是 PyTorch 的优势之一，但也是 Compiler 的难点之一。

例如

```text
Batch
Sequence Length
Image Size
Detection Count
Sparse Shape
```

不断变化时，需要 Symbolic Shape 和 Guard 等机制。

PyTorch 当前 Dynamic Shape 系统能够让部分变化维度在同一编译 Artifact 中运行，但并不是所有算子和优化都可以完全动态化，而且 Dynamic Rank 仍不属于支持目标。

### 12.7.5 并非所有 Python 代码都适合捕获

典型风险

- 第三方 C Extension
- 特殊 Python Side Effect
- Logging
- 文件操作
- 特殊自定义 Operator
- 数据依赖 Python 控制

此时可能

```text
Graph Break
```

或编译失败。

PyTorch 提供 Fine-grained Tracing API 以及 Custom Operator/Backend 等机制处理复杂场景，但这会提高工程复杂度。

### 12.7.6 Debug 更复杂

Eager

```text
Python Stack Trace
↓
出错行
```

Compile

```text
Python
↓
Dynamo
↓
AOTAutograd
↓
Inductor
↓
Triton / C++
↓
GPU Kernel
```

错误可能发生在不同层。

因此官方建议使用

```text
backend="eager"
      ↓
backend="aot_eager"
      ↓
backend="inductor"
```

逐层定位。

### 12.7.7 数值可能存在细微差异

编译器可能改变

- 算子融合
- 运算顺序
- Reduction 顺序
- Backend Kernel

浮点运算又不具有严格结合律

```text
(a + b) + c
```

和

```text
a + (b + c)
```

不一定逐位相同。

PyTorch 官方 Compile FAQ 也专门提供 Accuracy Debugging 路径，因为后端 Codegen 的数值可能与 Eager 存在细微差异，并在某些训练任务中影响稳定性。

因此启用 Compile 后需要比较

```text
Output
Loss
Training Curve
Final Metric
```

而不仅看速度。

### 12.7.8 显存不一定下降

编译可能通过融合减少部分中间 Tensor，但某些模式也可能

```text
显存增加
```

例如 `reduce-overhead` 为了 CUDA Graph 重放可能缓存 Workspace；官方明确说明该模式可能以更多内存换取更低 Python Overhead。

所以

```text
torch.compile
≠
显存优化工具
```

显存必须实测

```python
torch.cuda.max_memory_allocated()
```

### 12.7.9 不一定加速大型 Compute-Bound Kernel

假设模型 95% 时间都在

```text
一个已经高度优化的大型 GEMM
```

那么

```text
Python Overhead
Kernel Launch Overhead
Pointwise Fusion
```

本来就只占很小比例。

此时 Compile 提升可能有限。

官方教程同样指出，如果工作负载主要受 GPU Compute 限制，`torch.compile` 的相对收益可能不如 Python/Memory Overhead 较明显的模型。

## 12.8 本章小节

可以把整个过程记成

```text
Python / nn.Module
        │
        ▼
┌─────────────────┐
│   TorchDynamo   │
│                 │
│ Python Frame    │
│ ↓               │
│ Graph Capture   │
│ ↓               │
│ Guards          │
└────────┬────────┘
         │
         ▼
      FX Graph
         │
         ▼
┌─────────────────┐
│  AOTAutograd    │
│                 │
│ Forward Graph   │
│      +          │
│ Backward Graph  │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ TorchInductor   │
│                 │
│ Fusion          │
│ Optimization    │
│ Code Generation │
└────────┬────────┘
         │
         ▼
    Triton / C++
         │
         ▼
      GPU / CPU
```

Eager 与 Compile 对比

|       对比项       |  Eager   |   `torch.compile`    |
| :-------------: | :------: | :------------------: |
|      执行方式       | 逐算子立即执行  |    捕获 Graph 后优化执行    |
|   Python 灵活性    |   非常高    | 高，但部分代码会 Graph Break |
|      Debug      |    简单    |         更复杂          |
|      初始启动       |    快     |  有 Compile Overhead  |
|      小算子调度      |  开销可能明显  |         可降低          |
| Operator Fusion |   较有限    |          更强          |
|    动态 Shape     |   天然支持   | 需要动态 Shape/Guard 机制  |
|    Recompile    |   无此概念   |         可能发生         |
|      稳态性能       | Baseline |         可能更高         |
|     是否保证加速      |    —     |         不保证          |

三个核心组件必须记住

```text
TorchDynamo
=
Python / PyTorch Graph Capture
+
Guard
+
Graph Break / Recompile 管理
```

```text
AOTAutograd
=
为捕获到的 Forward Segment
生成可编译的 Backward Graph
```

```text
TorchInductor
=
默认 Compiler Backend
+
Graph Optimization
+
Kernel Code Generation
```

实际项目中，不建议

```text
写完模型
↓
直接 compile
↓
看到快了
↓
结束
```

更合理的是

```text
第一步
建立正确的 Eager Baseline
        ↓
第二步
Profiler 确认性能问题
        ↓
第三步
model = torch.compile(model)
        ↓
第四步
Warmup
        ↓
第五步
比较稳态 Throughput / Latency
        ↓
第六步
检查 Accuracy
        ↓
第七步
检查 Graph Break / Recompile
        ↓
第八步
再尝试 mode / fullgraph / Dynamic Shape 调优
```

如果 Compile 出错，则按照官方推荐的分层路径

```text
backend="eager"
      ↓
测试 Dynamo

backend="aot_eager"
      ↓
测试 AOTAutograd

backend="inductor"
      ↓
测试完整 Compiler
```

进行定位。

完成本章后，应能够独立回答

1. 什么是 PyTorch Eager Execution
2. Eager Execution 为什么开发体验很好
3. Eager 为什么会存在 Python 调度开销
4. 为什么大量小算子在 Eager 下可能比较慢
5. 为什么计算图有利于性能优化
6. `torch.compile` 的核心目标是什么
7. `torch.compile` 为什么可能降低 Python Overhead
8. `torch.compile` 为什么可能减少显存读写
9. `torch.compile` 和 `torch.export` 有什么区别
10. `torch.compile(model)` 的基本写法是什么
11. 默认 Backend 是什么
12. `default`、`reduce-overhead` 和 `max-autotune` 的大致定位是什么
13. 为什么第一次调用 compiled model 通常比较慢
14. Benchmark Compile 为什么需要 Warmup
15. TorchDynamo 的作用是什么
16. AOTAutograd 的作用是什么
17. TorchInductor 的作用是什么
18. Triton 在 `torch.compile` 技术栈中的位置是什么
19. Guard 是什么
20. Guard Failure 为什么可能导致 Recompile
21. Graph Break 是什么
22. Graph Break 是否一定导致程序报错
23. Graph Break 为什么会降低编译优化空间
24. `fullgraph=True` 有什么作用
25. Graph Break 和 Recompile 有什么区别
26. 动态 Shape 为什么给 Compiler 带来困难
27. 哪些模型比较适合使用 `torch.compile`
28. 为什么执行次数很少时可能不适合 Compile
29. 为什么 Compute-Bound 模型的 Compile 收益可能有限
30. `torch.compile` 是否一定省显存
31. `torch.compile` 是否一定加速
32. Compile 出错时如何使用 `eager → aot_eager → inductor` 分层排查
33. 为什么启用 Compile 后还需要检查训练精度和最终指标

# 13 模型导出与推理部署

训练完成并不意味着模型已经可以直接上线。

训练阶段关注的是

```text
如何得到更好的参数
```

而推理部署关注的是

```text
如何稳定、快速、低成本地
用这些参数处理真实请求
```

一个较完整的部署链路通常是

```text
训练完成
   ↓
选择最佳 Checkpoint
   ↓
恢复模型参数
   ↓
切换推理状态
   ↓
数值正确性验证
   ↓
选择部署形式
   ├── PyTorch Eager / torch.compile
   ├── torch.export / AOTInductor
   └── ONNX / 其他推理 Runtime
   ↓
性能 Benchmark
   ↓
服务封装
   ↓
监控延迟、吞吐、显存与错误
```

需要特别注意当前 PyTorch 的技术演进

```text
TorchScript
→ 历史上重要的部署方案
→ 当前已经 deprecated

torch.export
→ PyTorch 2.x 新一代模型导出基础

ONNX Exporter
→ 当前推荐使用基于 torch.export 的 dynamo 路径
```

PyTorch 当前官方文档已经明确标记 TorchScript 为 deprecated，并建议使用 `torch.export`；当前 ONNX Exporter 也推荐基于 `torch.export` 的 `dynamo=True` 路径。

## 13.1 训练与推理的区别

训练和推理虽然都会执行

```python
outputs = model(inputs)
```

但目标、状态和资源需求明显不同。

### 13.1.1 训练阶段

训练阶段的核心目标：根据 Loss 计算梯度，并不断更新模型参数。

完整过程

```text
Input
  ↓
Forward
  ↓
Prediction
  ↓
Loss
  ↓
Backward
  ↓
Gradient
  ↓
Optimizer Step
  ↓
更新 Parameter
```

因此训练需要维护

- 模型参数
- Forward 激活
- Autograd Graph
- 参数梯度
- Optimizer State
- AMP Scaler（FP16 时）
- 反向传播临时 Tensor

### 13.1.2 推理阶段

推理阶段

```text
Input
  ↓
Forward
  ↓
Prediction
```

通常不需要

```text
Backward
Gradient
Optimizer
训练 Autograd Graph
```

因此推理通常具有

```text
显存更低
计算量更少
延迟更低
可使用更多编译和部署优化
```

### 13.1.3 推理代码

训练

```python
model.train()

for inputs, targets in train_loader:
    optimizer.zero_grad(
        set_to_none=True
    )

    logits = model(inputs)

    loss = criterion(
        logits,
        targets,
    )

    loss.backward()

    optimizer.step()
```

推理

```python
model.eval()

with torch.inference_mode():
    logits = model(inputs)
```

`inference_mode()` 是比 `no_grad()` 更进一步的无梯度执行模式，会关闭更多 Autograd bookkeeping，从而为纯推理提供额外优化机会。

### 13.1.4 训练与推理的核心区别

|       对比       |  训练  |    推理    |
| :------------: | :--: | :------: |
|    Forward     |  是   |    是     |
|      Loss      |  是   |   通常否    |
|    Backward    |  是   |    否     |
|    Gradient    |  需要  |   不需要    |
|   Optimizer    |  需要  |   不需要    |
|    Dropout     |  开启  |    关闭    |
|   BatchNorm    | 更新统计 |  使用保存统计  |
| Autograd Graph |  需要  |  通常不需要   |
|       显存       |  高   |    较低    |
|       目标       |  收敛  | 延迟/吞吐/成本 |

### 13.1.5 推理阶段仍然可能很复杂

例如 LLM 推理

```text
Prompt
  ↓
Tokenizer
  ↓
Prefill
  ↓
KV Cache
  ↓
逐 Token Decode
  ↓
Sampling
  ↓
Detokenization
```

真正服务系统中还需要

- 请求调度
- Dynamic Batching
- Cache
- 多 GPU
- Streaming
- 超时控制
- 并发控制

所以：模型推理代码只是推理服务的一部分。

## 13.2 `model.eval` 与 `inference_mode`

这是推理代码中最容易混淆的地方之一。

需要记住

```text
model.eval()
和
torch.inference_mode()
解决的不是同一个问题
```

### 13.2.1 `model.eval()`

```python
model.eval()
```

本质上等价于

```python
model.train(False)
```

它修改

```python
module.training
```

从而改变依赖训练状态的 Module 行为。

典型包括

```text
Dropout
BatchNorm
自定义 self.training 分支
```

### 13.2.2 Dropout

训练

```python
model.train()
```

Dropout

```text
随机将部分元素置 0
```

推理

```python
model.eval()
```

Dropout 不再随机丢弃激活。

### 13.2.3 BatchNorm

训练模式下

```text
当前 Batch Statistics
+
更新 Running Statistics
```

推理模式下

```text
使用已经保存的 Running Mean
和
Running Variance
```

因此如果推理时忘记

```python
model.eval()
```

可能直接导致模型输出发生变化。

### 13.2.4 `eval()` 不会关闭梯度

下面代码

```python
model.eval()

outputs = model(inputs)
loss = outputs.sum()

loss.backward()
```

仍然可能正常构建 Autograd Graph 并执行 backward。

因此

```text
model.eval()
≠
关闭梯度
```

### 13.2.5 `torch.inference_mode()`

推理通常写

```python
model.eval()

with torch.inference_mode():
    outputs = model(inputs)
```

`inference_mode()` 面向明确不会进行 Autograd 的代码路径，比普通 no-grad 模式进一步关闭部分 Autograd 相关跟踪。官方也强调，Inference Mode 并不会自动调用 `model.eval()`，二者仍然是不同维度的控制。

### 13.2.6 与 `torch.no_grad()` 的区别

```python
with torch.no_grad():
    ...
```

主要作用

```text
不记录普通反向计算图
```

而

```python
with torch.inference_mode():
    ...
```

更加严格，适合

```text
明确的纯推理路径
```

可以建立

```text
model.eval()
→ Module 状态

no_grad / inference_mode
→ Autograd 状态
```

### 13.2.7 标准推理模板

```python
from __future__ import annotations

import torch
from torch import nn


@torch.inference_mode()
def predict(
    model: nn.Module,
    inputs: torch.Tensor,
    device: torch.device,
) -> torch.Tensor:
    model.eval()

    inputs = inputs.to(
        device,
        non_blocking=True,
    )

    logits = model(inputs)

    return logits
```

### 13.2.8 推理代码不要频繁切换状态

例如

```python
for request in requests:
    model.eval()

    with torch.inference_mode():
        ...
```

`eval()` 本身很轻，但更合理的服务结构是模型加载完成后

```python
model.eval()
```

一次性设置推理状态，然后请求处理函数只执行

```python
with torch.inference_mode():
    ...
```

## 13.3 TorchScript 基础

这一节需要先说明当前定位：TorchScript 仍然值得理解，因为大量旧项目和旧面试资料会涉及，但它已经不是当前新项目首选的 PyTorch 部署方案。

所以学习重点应是

```text
知道它是什么
知道 trace / script 区别
能看懂旧项目
知道当前为什么转向 torch.export
```

而不是投入大量时间深挖 TorchScript。

### 13.3.1 TorchScript 原本解决什么问题

普通 PyTorch 模型

```python
class Model(nn.Module):
    ...
```

严重依赖

```text
Python Runtime
```

TorchScript 希望

```text
Python nn.Module
      ↓
TorchScript IR
      ↓
序列化模型
      ↓
Python / C++ Runtime
```

也就是把 Python 定义的模型转换为一种可以序列化和在非普通 Python Eager 路径运行的表示。

### 13.3.2 TorchScript 两种核心方式

历史上主要有

```text
Tracing
+
Scripting
```

### 13.3.3 `torch.jit.trace`

例如

```python
model.eval()

example_input = torch.randn(
    1,
    3,
    224,
    224,
)

traced_model = torch.jit.trace(
    model,
    example_input,
)

torch.jit.save(
    traced_model,
    "model.pt",
)
```

加载

```python
model = torch.jit.load(
    "model.pt"
)

model.eval()

with torch.inference_mode():
    outputs = model(inputs)
```

### 13.3.4 Trace 的基本思想

Trace 可以理解为

```text
给模型一个 Example Input
        ↓
实际执行一次 Forward
        ↓
记录执行到的 Tensor Operator
        ↓
生成 Graph
```

优点

```text
简单
对纯 Tensor 数据流比较友好
```

问题是：它主要记录“这次 Example Input 实际走过的路径”。

例如

```python
def forward(self, x):
    if x.sum() > 0:
        return self.block_a(x)

    return self.block_b(x)
```

如果 Example Input 只进入

```text
block_a
```

Tracing 并不能天然像完整 Python 解释器一样理解所有数据依赖控制流。

### 13.3.5 `torch.jit.script`

历史上另一种方式

```python
scripted_model = torch.jit.script(
    model
)
```

Scripting 会分析支持的 Python 代码子集，因此比简单 Trace 更适合处理

- `if`
- 循环
- 部分数据相关控制流

PyTorch 当前的 `torch.compile` 教程也用 TorchScript 做历史对比：Scripting 能处理数据依赖控制流，但要求代码符合 TorchScript 支持的 Python 子集，并可能要求较多代码修改。

### 13.3.6 Trace 与 Script

|    对比     |      Trace      | Script |
| :-------: | :-------------: | :----: |
|   基本方式    | 跑 Example Input |  分析代码  |
|   使用简单度   |        高        |   较低   |
|  数据相关控制流  |       较弱        |   更强   |
| Python 限制 |      相对较少       |   较多   |
|   当前定位    |     Legacy      | Legacy |

### 13.3.7 为什么 TorchScript 被新体系替代

PyTorch 2.x 的新体系逐渐转向

```text
torch.compile
+
torch.export
+
AOTInductor
```

其中

```text
torch.compile
→ 优化 PyTorch Runtime 执行

torch.export
→ 得到标准化 Single Graph

AOTInductor
→ 将 ExportedProgram Ahead-of-Time 编译成部署 Artifact
```

PyTorch 官方当前对 AOTInductor 的定位就是：以 `torch.export` 得到的模型为输入，生成可用于非 Python 环境部署的共享库/部署产物。

## 13.4 ONNX 基础

ONNX

```text
Open Neural Network Exchange
```

可以理解为：一种跨框架、跨 Runtime 的机器学习模型图表示格式。

PyTorch 官方将 ONNX 描述为开放的模型表示标准，导出的模型可以由支持 ONNX 的 Runtime 消费，例如 ONNX Runtime。

### 13.4.1 ONNX 在部署链路中的位置

```text
PyTorch nn.Module
      ↓
ONNX Export
      ↓
ONNX Graph
      ↓
Inference Runtime
      ↓
CPU / GPU / Accelerator
```

重点

```text
ONNX 是模型表示格式
```

而不是

```text
ONNX 本身就是 GPU 推理引擎
```

真正执行 Graph 的是 Runtime。

### 13.4.2 为什么需要 ONNX

主要价值在

**跨框架**

训练

```text
PyTorch
```

部署

```text
其他支持 ONNX 的 Runtime
```

**跨语言**

模型服务不一定使用 Python。

**跨硬件**

不同 Runtime 可以针对

- CPU
- GPU
- Edge Accelerator

提供对应执行后端。

### 13.4.3 当前 PyTorch 推荐的 ONNX 导出方式

当前推荐

```python
torch.onnx.export(
    model,
    example_inputs,
    dynamo=True,
)
```

这个路径会基于

```text
torch.export
```

捕获 `ExportedProgram`，然后转换为 ONNX Graph。当前 PyTorch 官方将 `dynamo=True` 的 Exporter 作为推荐路径，而旧的 TorchScript-based ONNX Exporter 已不再推荐。

### 13.4.4 ONNX 导出示例

```python
import torch


model = MyModel()

model.load_state_dict(
    torch.load(
        "model_weights.pt",
        map_location="cpu",
        weights_only=True,
    )
)

model.eval()

example_inputs = (
    torch.randn(
        1,
        3,
        224,
        224,
    ),
)

onnx_program = torch.onnx.export(
    model,
    example_inputs,
    dynamo=True,
)

onnx_program.save(
    "model.onnx"
)
```

当前 ONNX Exporter 返回 `ONNXProgram`，可以通过 `.save()` 将模型保存为 `.onnx` 文件。

### 13.4.5 为什么导出前要 `eval()`

如果模型仍然处于

```python
model.train()
```

那么

- Dropout
- BatchNorm
- 依赖训练状态的逻辑

可能与真正线上推理不同。

PyTorch 官方 ONNX 教程也明确建议导出推理模型前调用 `model.eval()`。

### 13.4.6 Example Input 的作用

```python
example_inputs = (
    torch.randn(
        1,
        3,
        224,
        224,
    ),
)
```

Export 需要根据输入

- dtype
- Shape
- 代码执行路径
- Dynamic Shape 约束

构造 Graph。

因此 Example Input 必须符合实际输入语义。

### 13.4.7 Dynamic Shape

真实部署中可能

```text
Batch：
1～32

Sequence Length：
1～2048
```

不能简单把所有 Shape 固定死。

当前基于 `torch.export` 的 ONNX Exporter支持通过动态 Shape 描述来表达变化维度；在现代 `dynamo=True` 路径中，应优先使用 `dynamic_shapes` 思路，而不是依赖旧 TorchScript Exporter 的设计。

概念例如

```python
batch = torch.export.Dim(
    "batch",
    min=1,
    max=32,
)
```

让 Batch 维具有一定动态范围。

### 13.4.8 Export 成功不等于部署正确

这是非常重要的工程原则。

导出

```text
成功
```

只说明

```text
Graph 转换成功
```

不代表

```text
结果完全一致
性能一定更快
动态 Shape 都正常
目标 Runtime 全部支持
```

必须进行

```text
PyTorch Output
vs
Exported Runtime Output
```

数值对比。

PyTorch 官方 ONNX 教程也将导出后的 Runtime 结果与原始 PyTorch 输出进行数值比较作为完整流程的一部分。

### 13.4.9 ONNX 的限制

ONNX 并不支持所有 Python 语义和所有 PyTorch 数据结构。

PyTorch 官方教程明确指出，ONNX 标准并不具有与 PyTorch 完全相同的数据结构和类型表达能力，因此复杂模型在输入、控制流、自定义 Operator 等方面可能需要额外适配。

常见问题包括

- Unsupported Operator
- 自定义算子
- Dynamic Shape
- Python Control Flow
- 特殊数据结构
- Runtime Operator 支持差异

## 13.5 `torch.export` 基础

`torch.export` 是 PyTorch 2.x 部署体系中非常重要的基础设施。

它的目标可以概括为：把一个 PyTorch Module 捕获为标准化的单一计算图 `ExportedProgram`，供后续编译、转换和非普通 Python Eager 环境使用。

PyTorch 官方将 `torch.export` 定位为 PyTorch 2.x 的模型导出方式，可以从模型中提取单图表示 `ExportedProgram`。

### 13.5.1 基本流程

```text
nn.Module
   ↓
torch.export.export
   ↓
ExportedProgram
   ↓
保存 / 转换 / 编译
   ├── AOTInductor
   ├── ONNX
   └── 其他 Backend
```

### 13.5.2 基本使用

```python
import torch


class Model(torch.nn.Module):
    def __init__(self):
        super().__init__()

        self.linear = torch.nn.Linear(
            128,
            64,
        )

    def forward(
        self,
        x,
    ):
        return torch.relu(
            self.linear(x)
        )


model = Model()
model.eval()

example_inputs = (
    torch.randn(
        8,
        128,
    ),
)

exported_program = (
    torch.export.export(
        model,
        example_inputs,
    )
)
```

返回

```text
torch.export.ExportedProgram
```

当前官方教程将 `torch.export.export()` 作为核心入口，输出一个可序列化并可被后续工具消费的 `ExportedProgram`。

### 13.5.3 运行 ExportedProgram

通常

```python
exported_module = (
    exported_program.module()
)

outputs = exported_module(
    *example_inputs
)
```

官方 `torch.export` 教程也使用 `.module()` 将 `ExportedProgram` 变成可调用 Module。

### 13.5.4 保存

```python
torch.export.save(
    exported_program,
    "model.pt2",
)
```

加载

```python
loaded_program = (
    torch.export.load(
        "model.pt2"
    )
)

loaded_model = (
    loaded_program.module()
)
```

PyTorch 当前提供 `torch.export.save()` / `torch.export.load()` 用于序列化 `ExportedProgram`，官方示例使用 `.pt2` 扩展名。

### 13.5.5 `torch.export` 和 `torch.compile` 区别

这是高频面试问题。

**`torch.compile`**

```text
目标：
当前 PyTorch Runtime 内加速执行
```

允许

```text
Graph Break
+
回到 Python Eager
+
继续捕获
```

**`torch.export`**

```text
目标：
得到一个可导出的完整 Graph 表示
```

因此要求更严格

```text
不能依赖运行时回退 Python
```

可以理解

```text
torch.compile：
“能编多少编多少，主要为了跑得快”

torch.export：
“我要得到一个完整、可携带的图”
```

PyTorch 官方对两者也做了明确区分：一般加速 Python PyTorch 程序优先 `torch.compile`；当需要完整导出图、序列化或面向不同运行环境时，则使用 `torch.export`。

### 13.5.6 Dynamic Shape

可以声明

```python
batch = torch.export.Dim(
    "batch",
    min=1,
    max=32,
)

dynamic_shapes = {
    "x": {
        0: batch,
    }
}
```

然后

```python
exported_program = (
    torch.export.export(
        model,
        example_inputs,
        dynamic_shapes=dynamic_shapes,
    )
)
```

`torch.export` 允许通过 `dynamic_shapes` 显式描述输入维度约束，而不是只能把 Example Input 的全部 Shape 固定下来。

### 13.5.7 `torch.export` 不是最终高性能 Runtime

非常重要

```text
torch.export
```

主要产生

```text
ExportedProgram
```

它本身不是

```text
TensorRT
GPU Kernel Compiler
完整模型服务器
```

它通常是部署流程中的中间表示。

例如

```text
PyTorch Model
      ↓
torch.export
      ↓
ExportedProgram
      ↓
AOTInductor
      ↓
Compiled Artifact
```

AOTInductor 当前就是面向 `torch.export` 模型执行 Ahead-of-Time 编译、生成部署 Artifact 的 PyTorch 组件。

### 13.5.8 AOTInductor

可以建立基本定位

```text
torch.compile：
JIT-style Runtime Compile

AOTInductor：
Ahead-of-Time Compile
```

部署前

```text
Export
   ↓
Compile
   ↓
生成 Artifact
```

部署机器

```text
Load Artifact
   ↓
Inference
```

从而避免在线请求到达后再做完整编译。

当前官方 AOTInductor 文档说明，它以 `torch.export` 模型为输入，可以产生面向非 Python 环境部署的共享库/Artifact。

## 13.6 PyTorch 模型部署的一般流程

部署不能简单理解为

```text
torch.save()
↓
服务器 load()
```

完整工程通常应该分阶段进行。

### 13.6.1 第一步：确定模型版本

训练阶段可能产生

```text
epoch_1.pt
epoch_2.pt
...
best.pt
last.pt
```

部署通常选择

```text
best.pt
```

但具体依据应该是

- Accuracy
- F1
- Recall
- Validation Loss
- 业务指标

### 13.6.2 第二步：重建模型并加载权重

推荐

```python
model = MyModel(
    hidden_dim=256,
    num_classes=10,
)

state_dict = torch.load(
    "best.pt",
    map_location="cpu",
    weights_only=True,
)

model.load_state_dict(
    state_dict
)
```

PyTorch 官方仍推荐以 `state_dict` 作为普通 Python PyTorch 模型保存/加载的重要方式，而不是依赖 pickle 一个完整 Module 对象。

### 13.6.3 第三步：切换推理状态

```python
model.eval()
```

然后

```python
with torch.inference_mode():
    ...
```

### 13.6.4 第四步：建立 Eager Baseline

在任何 Export 或 Compile 前，先得到正确基线

```python
model = model.to(device)
model.eval()

with torch.inference_mode():
    reference_output = model(
        example_input
    )
```

这个结果是后续部署结果对比的

```text
Reference
```

### 13.6.5 第五步：选择部署路径

可以粗略分成三类。

**路径 A：继续使用 PyTorch Runtime**

```text
state_dict
+
nn.Module
+
PyTorch Python
```

可以进一步

```python
model = torch.compile(model)
```

优点

- 简单
- 兼容 Python
- 调试方便
- 改造成本低

适合

- Python Backend
- 内部服务
- GPU Server
- 快速上线

**路径 B：`torch.export` + AOTInductor**

```text
nn.Module
   ↓
torch.export
   ↓
ExportedProgram
   ↓
AOTInductor
   ↓
Compiled Artifact
```

适合

- 需要 Ahead-of-Time 编译
- 希望减少 Python Runtime 依赖
- 更接近原生部署

当前 AOTInductor 官方目标正是把 `torch.export` 模型提前编译为可部署 Artifact。

**路径 C：ONNX**

```text
PyTorch
   ↓
ONNX
   ↓
ONNX-compatible Runtime
```

适合

- 跨框架
- 跨 Runtime
- 目标环境已经有成熟 ONNX 推理栈

当前 PyTorch 推荐使用

```python
torch.onnx.export(
    model,
    example_inputs,
    dynamo=True,
)
```

### 13.6.6 第六步：数值正确性验证

应该准备一批代表性输入

```text
Normal Input
Boundary Input
Min Shape
Max Shape
Dynamic Shape
Extreme Value
```

然后比较

```text
Eager PyTorch
vs
Exported / Compiled Runtime
```

例如

```python
torch.testing.assert_close(
    eager_output,
    deployed_output,
    rtol=1e-4,
    atol=1e-5,
)
```

不同 dtype 和不同 Backend 可能需要调整误差容限；PyTorch 官方 ONNX 流程也明确要求将 Runtime 输出与原始 PyTorch 输出进行数值验证。

### 13.6.7 第七步：Warmup

推理 Benchmark 前

```python
with torch.inference_mode():
    for _ in range(20):
        _ = model(inputs)
```

GPU 第一次执行可能涉及

- CUDA Context
- Allocator
- Kernel 初始化
- `torch.compile` 编译
- Autotuning

因此

```text
第一次延迟
≠
稳定服务延迟
```

### 13.6.8 第八步：性能 Benchmark

至少记录

```text
Latency
Throughput
Peak Memory
GPU Utilization
CPU Utilization
```

服务场景还要记录

```text
P50
P95
P99 Latency
```

平均延迟不能代表尾延迟。

### 13.6.9 第九步：服务化

真实推理服务通常还需要

```text
HTTP / RPC
        ↓
Request Queue
        ↓
Preprocess
        ↓
Batching
        ↓
Model Runtime
        ↓
Postprocess
        ↓
Response
```

因此最终服务延迟不是只有

```text
model.forward()
```

而是

```text
End-to-End Latency
=
Queue
+
Preprocess
+
H2D
+
Model
+
D2H
+
Postprocess
+
Serialization
```

## 13.7 推理延迟、吞吐与 Batch 的关系

这是推理系统最重要的基础概念之一。

### 13.7.1 Latency

Latency：完成一次请求需要多长时间。

例如

```text
Request
→
20 ms
→
Response
```

则

```text
Latency = 20 ms
```

### 13.7.2 Throughput

Throughput：单位时间能够处理多少工作。

例如

```text
1000 samples / second
```

记作

```text
1000 samples/s
```

LLM 中更常见

```text
tokens/s
requests/s
```

### 13.7.3 Batch Size = 1

```text
Request 1
    ↓
GPU
    ↓
Response
```

优点

```text
无需等待其他请求
单请求排队较少
```

缺点

```text
GPU 并行度可能不足
吞吐较低
```

### 13.7.4 增大 Batch

例如

```text
Request 1 ┐
Request 2 │
Request 3 ├→ Batch → GPU
Request 4 ┘
```

GPU 一次处理更多样本。

因此通常

```text
Batch ↑
   ↓
GPU Utilization ↑
   ↓
Throughput ↑
```

直到 GPU 饱和。

### 13.7.5 Batch 与 Latency 并不是简单反比

例如

```text
Batch 1：
5 ms
→ 200 samples/s

Batch 8：
12 ms
→ 667 samples/s
```

Batch 8

```text
单次 Batch 延迟更高
```

但

```text
整体吞吐更高
平均每样本计算成本更低
```

因此：Latency 和 Throughput 往往存在 Trade-off。

### 13.7.6 在线服务还存在 Batch Waiting

真实服务为了组成 Batch

```text
Request A 到达
        ↓
等待 B/C/D
        ↓
组成 Batch
        ↓
Inference
```

所以服务端延迟

```text
Request Latency
=
Queue Waiting
+
Batch Formation
+
Inference
+
Postprocess
```

Batch 越大，并不一定越好。

### 13.7.7 Static Batching

例如

```text
必须凑满 32 个请求
才执行
```

低流量时可能

```text
等待很久
```

### 13.7.8 Dynamic Batching

更合理的思想

```text
最多等待 2 ms
或者
最多凑 16 个请求

哪个条件先满足就执行
```

这样

```text
高流量：
自动形成较大 Batch
→ 高吞吐

低流量：
超时立即执行
→ 控制延迟
```

### 13.7.9 Offline 与 Online 推理不同

**Offline Inference**

例如

```text
对 1000 万张图片批量预测
```

主要追求

```text
Maximum Throughput
```

因此通常可以采用较大 Batch。

**Online Inference**

例如

```text
用户实时调用 API
```

主要关注

```text
P95/P99 Latency
+
Throughput
```

Batch 不能无限增大。

### 13.7.10 LLM 中更加复杂

LLM 推理需要区分

```text
Prefill
和
Decode
```

Prefill

```text
大量 Prompt Token
矩阵计算密集
```

Decode

```text
每轮生成少量新 Token
反复读取 KV Cache
```

因此 LLM Serving 中的 Batch 不能简单套用传统 CNN

```text
固定 Batch Size
```

而需要考虑

- 请求长度
- 已生成长度
- KV Cache
- 连续批处理
- Decode 调度

## 13.8 推理性能优化方法

推理优化不能只依赖某一个技术。

可以按

```text
模型执行
数据传输
计算精度
计算图
Batching
内存
服务调度
```

多个层次进行优化。

PyTorch 官方性能指南也将异步数据加载、Pinned Memory、避免同步、禁用不需要的 Autograd、算子融合和编译等作为通用性能优化方向。

### 13.8.1 第一层：关闭训练相关行为

最基本

```python
model.eval()

with torch.inference_mode():
    outputs = model(inputs)
```

这是任何推理 Benchmark 的 Baseline。

### 13.8.2 第二层：AMP / 低精度

GPU

```python
with torch.inference_mode():
    with torch.autocast(
        device_type="cuda",
        dtype=torch.bfloat16,
    ):
        outputs = model(inputs)
```

或者

```python
dtype=torch.float16
```

推理不需要

```text
GradScaler
```

因为不存在 backward。

PyTorch 的 AMP API允许 autocast 单独用于前向计算，不需要与 GradScaler 绑定使用。

### 13.8.3 第三层：增大合适 Batch

测试

```text
B = 1
2
4
8
16
32
64
```

记录

```text
Latency
samples/s
Peak Memory
```

不要只看

```text
最大 Batch
```

而应找到 SLA 下的最佳点。

### 13.8.4 第四层：`torch.compile`

例如

```python
model = torch.compile(
    model
)
```

或者对小 Batch、Python Overhead 较明显的场景测试

```python
model = torch.compile(
    model,
    mode="reduce-overhead",
)
```

`torch.compile` 通过图捕获、算子融合和代码生成来减少 Python/Kernel Launch 等开销；实际收益取决于模型、Shape 和硬件，需要 Warmup 后实测。

### 13.8.5 第五层：避免 CPU-GPU 同步

不要在推理关键路径频繁

```python
value = tensor.item()

array = tensor.cpu().numpy()

print(tensor)
```

因为

```text
CPU 需要得到 GPU 结果
        ↓
同步
```

GPU CUDA 操作通常相对于 Host 异步执行，频繁取回结果会破坏流水线。

### 13.8.6 第六层：优化 H2D

DataLoader 场景

```python
loader = DataLoader(
    dataset,
    pin_memory=True,
    ...
)
```

迁移

```python
inputs = inputs.to(
    device,
    non_blocking=True,
)
```

Pinned Memory 和异步数据加载是 PyTorch 官方性能调优指南中的常见 GPU 数据流水线优化。

### 13.8.7 第七层：减少 Python 小算子

例如

```python
for i in range(T):
    x[:, i] = operation(
        x[:, i]
    )
```

尽量改为

```python
x = vectorized_operation(x)
```

减少

```text
Python Loop
Kernel Launch
小 Tensor
```

如果模型存在很多 Pointwise Operation，`torch.compile` 也可能通过融合减少这部分开销。

### 13.8.8 第八层：Quantization

推理可以考虑

```text
FP32
 ↓
FP16 / BF16
 ↓
INT8
 ↓
更低精度
```

低精度可以降低

- Model Size
- Memory Bandwidth
- 部分硬件计算成本

PyTorch 当前的新一代量化方向建立在 PyTorch 2 Export/PT2E 流程之上，包括 Post-Training Quantization 和 Quantization-Aware Training。

但量化需要重新验证

```text
Accuracy
Latency
Throughput
Operator Coverage
```

不是 dtype 越低越好。

### 13.8.9 第九层：合理控制 Dynamic Shape

例如 NLP

```text
Sequence Length：
101
137
203
414
```

如果每个请求 Shape 完全不同，可能

- 降低 Batch 效率
- 增加 Padding 波动
- 增加 Compiler Dynamic Shape 复杂度

可以做

```text
Length Bucketing
```

例如

```text
1～128
129～256
257～512
513～1024
```

让相近长度请求组成 Batch。

### 13.8.10 第十层：Ahead-of-Time Compilation

如果不希望线上第一次请求触发

```text
Compile
```

可以考虑

```text
torch.export
+
AOTInductor
```

提前

```text
Export
↓
Compile
↓
Package
```

部署时直接加载编译后的 Artifact。

AOTInductor 当前正是 PyTorch 提供的 Ahead-of-Time 部署路径，可以基于 `torch.export` 生成部署 Artifact。

### 13.8.11 第十一层：Profiler

不要凭感觉优化。

使用

```python
torch.profiler
```

查看

- CUDA Kernel
- CPU Operator
- Memcpy
- Input Shape
- GPU Idle
- Kernel Launch
- Memory

PyTorch Profiler 可以同时收集训练和推理阶段的 CPU/GPU Operator 与 Kernel 性能信息。

### 13.8.12 正确 Benchmark

错误

```python
start = time.time()

outputs = model(inputs)

end = time.time()
```

CUDA 是异步的。

简单 Benchmark

```python
import time
import torch


model.eval()

# Warmup
with torch.inference_mode():
    for _ in range(50):
        _ = model(inputs)

torch.cuda.synchronize()

start = time.perf_counter()

with torch.inference_mode():
    for _ in range(100):
        _ = model(inputs)

torch.cuda.synchronize()

elapsed = (
    time.perf_counter()
    - start
)

latency_ms = (
    elapsed / 100 * 1000
)

print(
    f"{latency_ms:.3f} ms"
)
```

CUDA 异步执行意味着 Host 时钟计时需要适当同步，或者使用 CUDA Event。

### 13.8.13 优化优先级

一个比较合理的顺序

```text
① 正确的 Eager Baseline
   model.eval()
   inference_mode()

② Profile

③ 合理 Batch Size

④ FP16 / BF16

⑤ 减少 CPU-GPU 同步

⑥ 优化数据传输

⑦ torch.compile

⑧ Shape Bucketing / Dynamic Batching

⑨ Quantization

⑩ torch.export / ONNX / AOTInductor
```

这里没有绝对顺序，例如部署环境已经要求 ONNX，则导出格式自然更早确定。

但核心原则不变：先找到瓶颈，再优化。

## 13.9 本章小节

```text
state_dict
│
├── 保存 Parameter / Buffer
├── 仍需要 Python 模型代码
└── 适合 PyTorch Runtime
```

```text
TorchScript
│
├── 历史图表示
├── trace / script
├── 可序列化
└── 当前已 deprecated
```

```text
torch.export
│
├── PyTorch 2.x 新导出体系
├── ExportedProgram
├── Single Graph
├── 支持 Dynamic Shape Constraint
└── 可作为 AOTInductor / ONNX 等下游输入
```

```text
ONNX
│
├── 开放模型交换格式
├── 跨 Runtime
├── 需要目标 Runtime
└── 当前 PyTorch 推荐 dynamo Exporter
```

|       技术        |       主要目标       |      输出/执行方式      |   是否主要用于部署   |
| :-------------: | :--------------: | :---------------: | :----------: |
| `torch.compile` |  加速 PyTorch 程序   |   编译后的 Callable   | 可以，但首先是运行时性能 |
| `torch.export`  |   获得完整模型 Graph   | `ExportedProgram` |    是，下游基础    |
|      ONNX       |     跨框架模型表示      | `.onnx` + Runtime |      是       |
|   TorchScript   | 历史 PyTorch 部署 IR |   ScriptModule    |    Legacy    |

当前 PyTorch 中，`torch.compile` 是编译优化入口，而 `torch.export` 面向完整 Graph 的导出和下游部署；TorchScript 已进入 deprecated 状态。

PyTorch 模型部署流程总结

```text
训练模型
   ↓
Best Checkpoint
   ↓
load_state_dict
   ↓
model.eval()
   ↓
inference_mode()
   ↓
Eager Correctness Baseline
   ↓
选择部署方式
   │
   ├── Python PyTorch
   │     └── torch.compile
   │
   ├── torch.export
   │     └── AOTInductor
   │
   └── ONNX
         └── ONNX-compatible Runtime
   ↓
数值一致性验证
   ↓
Warmup
   ↓
Benchmark
   ├── Latency
   ├── Throughput
   ├── P95 / P99
   └── Memory
   ↓
Batch / Precision / Compiler 优化
   ↓
服务化
```

完成本章后，应能够独立回答

1. 训练和推理的核心区别是什么
2. 为什么推理显存通常低于训练
3. `model.eval()` 做了什么
4. `model.eval()` 是否关闭梯度
5. `inference_mode()` 是什么
6. `inference_mode()` 和 `no_grad()` 有什么区别
7. 为什么推理通常同时使用 `eval()` 和 `inference_mode()`
8. TorchScript 是什么
9. `torch.jit.trace` 的原理是什么
10. Trace 为什么难以处理数据相关控制流
11. `torch.jit.script` 与 Trace 有什么区别
12. TorchScript 当前是什么定位
13. ONNX 是模型格式还是 Runtime
14. 当前 PyTorch 推荐的 ONNX Exporter 是什么
15. `dynamo=True` 与 `torch.export` 有什么关系
16. 为什么 ONNX 导出成功后仍要做数值验证
17. `torch.export` 的作用是什么
18. `ExportedProgram` 是什么
19. `torch.export` 和 `torch.compile` 有什么区别
20. Dynamic Shape 在模型导出中为什么重要
21. AOTInductor 是什么
22. AOT 与 JIT 的基本区别是什么
23. PyTorch 模型的一般部署流程是什么
24. 为什么部署前应该先建立 Eager Baseline
25. Latency 和 Throughput 分别是什么
26. Batch Size 为什么能够提高吞吐
27. Batch Size 为什么也可能增加请求延迟
28. Offline 和 Online 推理对 Batch 的要求有什么不同
29. Dynamic Batching 的基本思想是什么
30. 推理为什么可以使用 FP16/BF16
31. 推理为什么不需要 GradScaler
32. `torch.compile` 如何用于推理
33. 为什么应减少 `.item()`、`.cpu()` 等同步操作
34. Pinned Memory 和 `non_blocking` 如何帮助推理数据传输
35. Quantization 主要解决什么问题
36. 为什么 Dynamic Shape 可能影响推理性能
37. 为什么 Benchmark 需要 Warmup
38. CUDA 异步执行为什么会导致普通 Python 计时不准确
39. 如何使用 CUDA Event 测量 GPU 推理延迟
40. 为什么模型部署优化最终必须围绕 Latency、Throughput、Memory、Accuracy 和成本 一起评估

