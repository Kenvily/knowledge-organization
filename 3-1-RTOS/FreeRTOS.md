<font size = 6>FreeRTOS</font>

[toc]

# 裸机与RTOS

## 裸机

又称为前后台系统，前台系统指的中断服务函数，后台系统指的大循环，即应用程序。

<img src=".\img\image-20250208212430742.png" alt="image-20250208212430742" style="zoom: 50%;" />

特点

- 实时性非常差，(应用程序) 轮流执行
- 空等待，CPU不执行其他代码
- 结构臃肿，实现功能都放在无限循环

## RTOS

Real Time OS，实时操作系统，强调实时性，按时间片并发执行任务，充分利用CPU资源。

<img src=".\img\image-20250208212607405.png" alt="image-20250208212607405" style="zoom: 60%;" />

特点

- 分而治之：实现功能划分为多个任务
- 延时函数，非空等待，让出CPU使用权给较低优先级的任务，充分利用CPU资源
- 抢占式：高优先级任务可抢占低优先级任务
- 任务堆栈：每个任务都有自己的栈空间，用于保存局部变量信息以及任务的上下文信息

注

1. 中断可以打断任意任务
2. 任务可以同等优先级
3. 中断是一瞬间完成的，不能处理太多。如果在裸机中，中断中用标志位来控制，又会导致原本中断中处理的程序不能得到及时处理

# FreeRTOS

一个免费的嵌入式实时操作系统。

特点

- 免费开源
- 简单
- 可裁剪：核心代码9000+行，包含在3个.c文件中
- 优先级不限：任务优先级分配没有限制，多任务可同一优先级
- 任务不限：可创建的实时任务数量没有软件限制，但受限于硬件内存
- 抢占/协程/时间片：支持抢占式，协程式、时间片流转任务调度

注

1. 对于任务而言，数字越大优先级越高
2. 对于中断而言，数字越小优先级越高
3. 软件实现优先级没有限制，但硬件实现优先级通常受限于MCU的架构，如STM32最大只有32个优先级
4. 对于每一个任务，都要分配一个任务堆栈

# FreeRTOS移植

## FreeRTOS源码

- include：内包含了FreeRTOS的头文件
- protable：内包含了FreeRTOS的移植文件，根据需求进行选择
- croutine.c：协程相关文件
- event_groups.c：事件相关文件
- list.c：列表相关文件
- queue.c：队列相关文件
- stream_buffer.c：流式缓冲区相关文件
- tasks.c：任务相关文件
- timers.c ：软件定时器相关文件

## 移植步骤

1. 添加FreeRTOS源码：将FreeRTOS源码添加至基础工程、头文件路径等
2. FreeRTOSConfig.h：添加FreeRTOSConfig.h 配置文件
3. 修改SYSTEM文件：修改SYSTEM文件中的sys.c、delay.c、usart.c
4. 修改中断相关文件：修改Systick中断、SVC中断、PendSV中断
5. 添加应用程序：验证移植是否成功

## FreeRTOSConfig.h

FreeRTOSConfig.h配置文件是对FreeRTOS的功能进行配置和裁剪，以及API函数的使能等 。

相关宏大致可分为三类

- INCLUDE：配置FreeRTOS中可选的API函数
- config：完成FreeRTOS的功能配置和裁剪
- 其他配置项：PendSV宏定义、SVC宏定义

# 任务调度

调度器就是使用相关的调度算法来决定当前需要执行的哪个任务。

FreeRTOS 一共支持三种任务调度方式

- 抢占式调度：主要是针对优先级不同的任务，每个任务都有一个优先级，优先级高的任务可以抢占优先级低的任务
- 时间片调度：主要针对优先级相同的任务，当多个任务的优先级相同时， 任务调度器会在每一次系统时钟节拍到的时候切换任务
- 协程式调度：当前执行任务将会一直运行，同时高优先级的任务不会抢占低优先级任务

注

1. 任务切换是在中断中执行的

## 抢占式调度

特点

- 高优先级任务优先执行
- 高优先级任务不停止，低优先级任务无法执行
- 被抢占的任务将会进入就绪态

<img src=".\img\image-20250208221630734.png" alt="image-20250208221630734" style="zoom:50%;" />

运行过程如下

1. 首先Task1在运行中，在这个过程中Task2就绪了，在抢占式调度器的作用下Task2会抢占Task1的运行
2. Task2运行过程中，Task3就绪了，在抢占式调度器的作用下Task3会抢占Task2的运行
3. Task3运行过程中，Task3阻塞了（系统延时或等待信号量等），此时就绪态中，优先级最高的任务Task2执行
4. Task3阻塞解除了（延时到了或者接收到信号量），此时Task3恢复到就绪态中，抢占TasK2的运行

## 时间片调度

### 介绍

同等优先级任务轮流地享有相同的 CPU 时间（可设置），叫（一个）时间片，在FreeRTOS中一个时间片就等于SysTick中断周期。

特点

- 同等优先级任务，轮流执行，时间片流转
- 一个时间片大小，取决于滴答定时器中断周期
- 任务中途被打断或阻塞，没有用完的时间片不会再使用，下次该任务得到执行 还是按照一个时间片的时钟节拍运行

<img src=".\img\image-20250208222052588.png" alt="image-20250208222052588" style="zoom:60%;" />

运行过程如下

1. 首先Task1运行完一个时间片后，切换至Task2运行
2. Task2运行完一个时间片后，切换至Task3运行
3. Task3运行过程中（还不到一个时间片），Task3阻塞了（系统延时或等待信号量等），此时丢掉剩余时间片，直接切换到下一个任务Task1
4. Task1运行完一个时间片后，切换至Task2运行

注

1. 使用时间片调度需把宏 configUSE_TIME_SLICING 和 configUSE_PREEMPTION 置1
2. 通过configTICK_RATE_HZ可以定义系统时钟节拍频率

### 实例

<img src=".\img\image-20250213055006850.png" alt="image-20250213055006850" style="zoom:40%;" />

```c
/*freertos_demo.c*/

/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define START_TASK_PRIO         1
#define START_TASK_STACK_SIZE   128
TaskHandle_t    start_task_handler;
void start_task( void * pvParameters );

/* TASK1 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK1_PRIO         2
#define TASK1_STACK_SIZE   128
TaskHandle_t    task1_handler;
void task1( void * pvParameters );


/* TASK2 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK2_PRIO         2
#define TASK2_STACK_SIZE   128
TaskHandle_t    task2_handler;
void task2( void * pvParameters );

/**
 * @brief       FreeRTOS例程入口函数
 * @param       无
 * @retval      无
 */
void freertos_demo(void)
{    
    xTaskCreate((TaskFunction_t         )   start_task,
                (char *                 )   "start_task",
                (configSTACK_DEPTH_TYPE )   START_TASK_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   START_TASK_PRIO,
                (TaskHandle_t *         )   &start_task_handler );
    vTaskStartScheduler();
}


void start_task( void * pvParameters )
{
    taskENTER_CRITICAL();               /* 进入临界区 */
    xTaskCreate((TaskFunction_t         )   task1,
                (char *                 )   "task1",
                (configSTACK_DEPTH_TYPE )   TASK1_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK1_PRIO,
                (TaskHandle_t *         )   &task1_handler );
                
    xTaskCreate((TaskFunction_t         )   task2,
                (char *                 )   "task2",
                (configSTACK_DEPTH_TYPE )   TASK2_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK2_PRIO,
                (TaskHandle_t *         )   &task2_handler );
    vTaskDelete(NULL);
    taskEXIT_CRITICAL();                /* 退出临界区 */
}

/* 任务一，实现LED0每500ms翻转一次 */
void task1( void * pvParameters )
{
    uint32_t task1_num = 0;
    while(1)
    {
        taskENTER_CRITICAL();               /* 进入临界区 */
        printf("task1运行次数：%d\r\n",++task1_num);
        taskEXIT_CRITICAL();                /* 退出临界区 */
        delay_ms(10);						//因为要测试时间片，所以不能为vTaskDelay(10);
    }
}


/* 任务二，列表项的插入和删除实验 */
void task2( void * pvParameters )
{
    uint32_t task2_num = 0;
    while(1)
    {
        taskENTER_CRITICAL();               /* 进入临界区 */
        printf("task2运行次数：%d\r\n",++task2_num);
        taskEXIT_CRITICAL();                /* 退出临界区 */
        delay_ms(10);
    }
}

/*现象：
- 同等优先级的情况下，两任务循环每次各打印约4次输出
- 不同等优先级的情况下，只执行优先级更高的任务
*/
```

## 任务状态

### 概念

FreeRTOS中任务共存在4种状态

- 运行态：正在执行的任务，该任务就处于运行态，注意在STM32中，同一时间仅一个任务处于运行态
- 就绪态：如果该任务已经能够被执行，但当前还未被执行，那么该任务处于就绪态
- 阻塞态：如果一个任务因延时或等待外部事件发生，那么这个任务就处于阻塞态
- 挂起态：类似暂停，调用函数 vTaskSuspend() 进入挂起态，需要调用解挂函数vTaskResume()才可以进入就绪态

### 任务状态转换图

<img src=".\img\image-20250208222551992.png" alt="image-20250208222551992" style="zoom:50%;" />

注

1. 仅就绪态可转变成运行态
2. 其他状态的任务想运行，必须先转变成就绪态

### 任务状态表

这四种状态中，除了运行态，其他三种任务状态的任务都有其对应的任务状态列表。

<img src=".\img\image-20250208222824455.png" alt="image-20250208222824455" style="zoom:50%;" />

32位的变量，当某个位置1时，代表所对应的优先级就绪列表有任务存在。

**举例**

假设，创建三个任务分别是：task1、task2、task3，优先级分别是：1、2、3。

<img src=".\img\image-20250208223115168.png" alt="image-20250208223115168" style="zoom:40%;" />

注

1. 调度器总是在所有处于就绪列表的任务中，选择具有最高优先级的任务来执行

## 任务调度器

### 开启任务调度器

`vTaskStartScheduler() `用于启动任务调度器，任务调度器启动后， FreeRTOS 便会开始进行任务调度。

内部实现

1. 创建空闲任务
2. 如果使能软件定时器，则创建定时器任务
3. 关闭中断，防止调度器开启之前或过程中，受中断干扰，会在运行第一个任务时打开中断
4. 初始化全局变量，并将任务调度器的运行标志设置为已运行
5. 初始化任务运行时间统计功能的时基定时器
6. 调用函数 xPortStartScheduler() 

### xPortStartScheduler()

该函数用于完成启动任务调度器中与硬件架构相关的配置部分，以及启动第一个任务。

内部实现

1. 检测用户在 FreeRTOSConfig.h 文件中对中断的相关配置是否有误 
2. 配置 PendSV 和 SysTick 的中断优先级为最低优先级
3. 调用函数 vPortSetupTimerInterrupt()配置 SysTick
4. 初始化临界区嵌套计数器为 0 
5. 调用函数 prvEnableVFP()使能 FPU
6. 调用函数 prvStartFirstTask()启动第一个任务

## 启动任务

假设我们要启动的第一个任务是任务A，那么就需要将任务A的寄存器值恢复到CPU寄存器。

程序在运行过程中需要一定的栈空间来保存局部变量等一些信息，ARM Cortex-M 内核提供了两个栈空间

- 主堆栈指针（MSP）：它由 OS 内核、异常服务例程以及所有需要特权访问的应用程序代码来使用
- 进程堆栈指针（PSP）：用于常规的应用程序代码（不处于异常服务例程中时）

注

1. 在FreeRTOS中，中断使用MSP（主堆栈），中断以外使用PSP（进程堆栈）
2. SVC中断只在启动第一次任务时会调用一次，以后均不调用 
3. 压栈（保存现场），从上往下（高地址往低地址），将CPU寄存器的值赋值到内存中
4. 出栈（恢复现场），从下往上（低地址往高地址），将内存的值赋值到CPU寄存器中

<img src=".\img\image-20250213022807372.png" alt="image-20250213022807372" style="zoom:70%;" />

## 任务切换

### 介绍

任务切换的过程在PendSV中断服务函数里边完成，其本质是CPU寄存器的切换。

假设当由任务A切换到任务B时，主要分为两步

1. 需暂停任务A的执行，并将此时任务A的寄存器保存到任务堆栈，这个过程叫做保存现场
2. 将任务B的各个寄存器值（被存于任务堆栈中）恢复到CPU寄存器中，这个过程叫做恢复现场

对任务A保存现场，对任务B恢复现场，这个整体的过程称之为上下文切换。

<img src=".\img\image-20250213021322776.png" alt="image-20250213021322776" style="zoom:40%;" />

### PendSV中断是如何触发

- 滴答定时器中断调用（1ms中断一次）
- 执行FreeRTOS提供的相关API函数：portYIELD() 

本质都是通过向中断控制和状态寄存器 ICSR 的bit28 写入 1 挂起 PendSV 来启动 PendSV 中断。

<img src=".\img\image-20250213021611165.png" alt="image-20250213021611165" style="zoom:80%;" />

### 切换过程

1. 当前的psp是正在运行的任务的栈指针，读取当前psp进程指针，存入r0
2. 压栈（保存现场）
3. 获取当前最高优先级任务的任务控制块
4. 出栈（恢复现场）
5. 更新切换后的任务的的栈指针给PSP
6. bx r14 执行新任务函数

### 最高优先级任务

通过函数`vTaskSwitchContext（）`查找最高优先级任务。

- 利用函数`taskSELECT_HIGHEST_PRIORITY_TASK()`，前导置零指令找到最高优先级
- 利用函数`listGET_OWNER_OF_NEXT_ENTRY()`，获取最高优先级的任务句柄

所谓的前导置零指令，可以简单理解为计算一个 32位数，头部 0 的个数。

<img src=".\img\image-20250213021835607.png" alt="image-20250213021835607" style="zoom:50%;" />

# 任务创建和删除

任务的创建和删除本质就是调用FreeRTOS的API函数。

## API函数

|       API函数       |       描述       |
| :-----------------: | :--------------: |
|    xTaskCreate()    | 动态方式创建任务 |
| xTaskCreateStatic() | 静态方式创建任务 |
|    vTaskDelete()    |     删除任务     |

- 动态创建任务：任务的任务控制块以及任务的栈空间所需的内存，均由 FreeRTOS 从 FreeRTOS 管理的堆中分配 
- 静态创建任务：任务的任务控制块以及任务的栈空间所需的内存，需用户分配提供

注

1. 在实际的应用中，动态方式创建任务方便，静态创建任务对内存管理更有效
2. 静态创建可将任务堆栈放置在特定的内存位置，并且无需关心对内存分配失败的处理

## 动态创建任务函数

### 代码

```C
/*
* 返回值：
- pdPASS表示任务创建成功
- errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY表示任务创建失败
*/
BaseType_t xTaskCreate
(
    TaskFunction_t 					pxTaskCode,		/* 指向任务函数的指针 */	
    const char * const 				pcName, 		/* 任务名字，最大长度configMAX_TASK_NAME_LEN */
	const 	configSTACK_DEPTH_TYPE 	usStackDepth, 	/* 任务堆栈大小，注意字为单位 */
	void * const 					pvParameters,	/* 传递给任务函数的参数 */
	UBaseType_t 					uxPriority,		/* 任务优先级，范围：0 ~ configMAX_PRIORITIES - 1 */
	TaskHandle_t * const 			pxCreatedTask 	/* 任务句柄，就是任务的任务控制块 */
)
```

### 实现流程

1. 将宏 configSUPPORT_DYNAMIC_ALLOCATION 配置为 1
2. 定义函数入口参数
3. 编写任务函数

### 内部实现

此函数创建的任务会立刻进入就绪态，由任务调度器调度运行。

1. 申请堆栈内存（大小手动）&任务控制块内存（大小固定）
2. TCB结构体成员赋值
3. 添加新任务到就绪列表中

## 静态创建任务函数

### 代码

```C
/*
* 返回值：
- NULL表示用户没有提供相应的内存，任务创建失败
- 其他值表示任务句柄，任务创建成功
*/
TaskHandle_t xTaskCreateStatic
(
    	TaskFunction_t			pxTaskCode,				/* 指向任务函数的指针 */
    	const char * const		pcName,					/* 任务函数名 */
    	const uint32_t			ulStackDepth, 			/* 任务堆栈大小注意字为单位 */
    	void * const			pvParameters, 			/* 传递的任务函数参数 */
    	UBaseType_t				uxPriority, 			/* 任务优先级 */
    	StackType_t * const		puxStackBuffer, 		/* 任务堆栈，一般为数组，由用户分配 */
    	StaticTask_t * const	pxTaskBuffer			/* 任务控制块指针，由用户分配 */
); 		
```

### 实现流程

1. 需将宏configSUPPORT_STATIC_ALLOCATION 配置为 1 
2. 定义空闲任务&定时器任务的任务堆栈及TCB
3. 实现两个接口函数`vApplicationGetIdleTaskMemory()`和`vApplicationGetTimerTaskMemory()`
4. 定义函数入口参数
5. 编写任务函数

### 内部实现

此函数创建的任务会立刻进入就绪态，由任务调度器调度运行。

1. TCB结构体成员赋值
2. 添加新任务到就绪列表中

## TCB介绍

每个任务都有属于自己的任务控制块，类似身份证。

```C
typedef struct tskTaskControlBlock       
{
    volatile StackType_t 		* pxTopOfStack; 				/* 任务栈栈顶，必须为TCB的第一个成员 */
    ListItem_t 			xStateListItem;         				/* 任务状态列表项 */      
	ListItem_t 			xEventListItem;							/* 任务事件列表项 */
    UBaseType_t 		uxPriority;                				/* 任务优先级，数值越大，优先级越大 */
    StackType_t 		* pxStack;								/* 任务栈起始地址 */
    char 				pcTaskName[ configMAX_TASK_NAME_LEN ]; 	/* 任务名字 */		
	…
	省略很多条件编译的成员
} tskTCB;
```

注

1. 任务栈栈顶，在任务切换时的任务上下文保存、任务恢复息息相关

## 任务删除函数

### 介绍

用于删除已被创建的任务，被删除的任务将从就绪态任务列表、阻塞态任务列表、挂起态任务列表和事件列表中移除。

```C
/*
* 形参：
- xTaskToDelete 待删除任务的任务句柄
*/
void vTaskDelete(TaskHandle_t xTaskToDelete);
```

注

1. 当传入的参数为NULL，则代表删除任务自身（当前正在运行的任务）
2. 空闲任务会负责释放被删除任务中由系统分配的内存，但是由用户在任务删除前申请的内存， 则需要由用户在任务被删除前提前释放，否则将导致内存泄露 
3. 动态创建任务宏打开时空闲任务自动创建；静态创建任务宏只要打开须手动配置空闲任务

### 实现流程

1. 使用删除任务函数，需将宏INCLUDE_vTaskDelete 配置为 1 
2. 入口参数输入需要删除的任务句柄（NULL代表删除本身）

### 内部实现

1. 获取所要删除任务的控制块：通过传入的任务句柄，判断所需要删除哪个任务，NULL代表删除自身
2. 将被删除任务，移除所在列表：将该任务在所在列表中移除，包括：就绪、阻塞、挂起、事件等列表
3. 判断所需要删除的任务：删除任务自身，需先添加到等待删除列表，内存释放将在空闲任务执行 or 删除其他任务，释放内存，任务数量--
4. 更新下个任务的阻塞时间：当被删除的任务就是下一个阻塞超时的任务时，需要更新阻塞时间

## 临界区

临界区用来保护那些不想被打断的程序。

由于任务调度是通过中断实现的，而进入临界区作用是关闭中断，所以进入临界区可以禁止任务调度，直至退出临界区。

## 计算机中的数据单位

- 比特（bit）：计算机中最小的数据单位，表示一个二进制位，可以是0或1，通常用小写的“b”表示

- 字节（byte）：计算机中常用的数据单位，由8个比特组成，通常用大写的“B”表示

- 字（word）：其大小取决于计算机的架构，如32位系统中 1 字=32 比特=4 字节，64位系统中 1 字=64 比特=8 字节，通常用“word”表示

## 实例

<img src=".\img\image-20250209214342460.png" alt="image-20250209214342460" style="zoom:40%;" />

### 动态创建任务

```C
/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define START_TASK_PRIO         1
#define START_TASK_STACK_SIZE   128
TaskHandle_t    start_task_handler;
void start_task( void * pvParameters );

/* TASK1 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK1_PRIO         2
#define TASK1_STACK_SIZE   128
TaskHandle_t    task1_handler;
void task1( void * pvParameters );

/* TASK2 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK2_PRIO         3
#define TASK2_STACK_SIZE   128
TaskHandle_t    task2_handler;
void task2( void * pvParameters );

/* TASK3 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK3_PRIO         4
#define TASK3_STACK_SIZE   128
TaskHandle_t    task3_handler;
void task3( void * pvParameters );

/**
 * @brief       FreeRTOS例程入口函数
 * @param       无
 * @retval      无
 */
void freertos_demo(void)
{    
    xTaskCreate((TaskFunction_t         )   start_task,		// 指向函数地址 
                (char *                 )   "start_task",
                (configSTACK_DEPTH_TYPE )   START_TASK_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   START_TASK_PRIO,
                (TaskHandle_t *         )   &start_task_handler );
    vTaskStartScheduler();				// 任务调度器
}


void start_task( void * pvParameters )
{
    taskENTER_CRITICAL();               /* 进入临界区 */
    xTaskCreate((TaskFunction_t         )   task1,
                (char *                 )   "task1",
                (configSTACK_DEPTH_TYPE )   TASK1_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK1_PRIO,
                (TaskHandle_t *         )   &task1_handler );
                
    xTaskCreate((TaskFunction_t         )   task2,
                (char *                 )   "task2",
                (configSTACK_DEPTH_TYPE )   TASK2_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK2_PRIO,
                (TaskHandle_t *         )   &task2_handler );
                
    xTaskCreate((TaskFunction_t         )   task3,
                (char *                 )   "task3",
                (configSTACK_DEPTH_TYPE )   TASK3_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK3_PRIO,
                (TaskHandle_t *         )   &task3_handler );
    vTaskDelete(NULL);
    taskEXIT_CRITICAL();                /* 退出临界区 */
}

/* 任务一，实现LED0每500ms翻转一次 */
void task1( void * pvParameters )
{
    while(1)
    {
        printf("task1正在运行！！！\r\n");		//串口中打印信息
        LED0_TOGGLE();
        vTaskDelay(500);
    }
}

/* 任务二，实现LED1每500ms翻转一次 */
void task2( void * pvParameters )
{
    while(1)
    {
        printf("task2正在运行！！！\r\n");
        LED1_TOGGLE();
        vTaskDelay(500);
    }
}

/* 任务三，判断按键KEY0，按下KEY0删除task1 */
void task3( void * pvParameters )
{
    uint8_t key = 0;
    while(1)
    {
        printf("task3正在运行！！！\r\n");
        key = key_scan(0);
        if(key == KEY0_PRES)
        {
            if(task1_handler != NULL)		// 防止重复删除task1所在存储空间
            {
                printf("删除task1任务\r\n");
                vTaskDelete(task1_handler);
                task1_handler = NULL;
            }
        }
        vTaskDelay(10);
    }
}
```

### 静态创建任务

```C
/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define START_TASK_PRIO         1
#define START_TASK_STACK_SIZE   128
TaskHandle_t    start_task_handler;
StackType_t     start_task_stack[START_TASK_STACK_SIZE];
StaticTask_t    start_task_tcb;
void start_task( void * pvParameters );

/* TASK1 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK1_PRIO         2
#define TASK1_STACK_SIZE   128
TaskHandle_t    task1_handler;
StackType_t     task1_stack[TASK1_STACK_SIZE];
StaticTask_t    task1_tcb;
void task1( void * pvParameters );

/* TASK2 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK2_PRIO         3
#define TASK2_STACK_SIZE   128
TaskHandle_t    task2_handler;
StackType_t     task2_stack[TASK2_STACK_SIZE];
StaticTask_t    task2_tcb;
void task2( void * pvParameters );

/* TASK3 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK3_PRIO         4
#define TASK3_STACK_SIZE   128
TaskHandle_t    task3_handler;
StackType_t     task3_stack[TASK3_STACK_SIZE];
StaticTask_t    task3_tcb;
void task3( void * pvParameters );

/* 空闲任务配置 */
StaticTask_t idle_task_tcb;
StackType_t  idle_task_stack[configMINIMAL_STACK_SIZE];

/* 软件定时器任务配置 */
StaticTask_t timer_task_tcb;
StackType_t  timer_task_stack[configTIMER_TASK_STACK_DEPTH];

/* 空闲任务内存分配 */
void vApplicationGetIdleTaskMemory( StaticTask_t ** ppxIdleTaskTCBBuffer,
                                    StackType_t ** ppxIdleTaskStackBuffer,
                                    uint32_t * pulIdleTaskStackSize )
{
    * ppxIdleTaskTCBBuffer = &idle_task_tcb;
    * ppxIdleTaskStackBuffer = idle_task_stack;
    * pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}


/* 软件定时器内存分配 */
void vApplicationGetTimerTaskMemory( StaticTask_t ** ppxTimerTaskTCBBuffer,
                                     StackType_t ** ppxTimerTaskStackBuffer,
                                     uint32_t * pulTimerTaskStackSize )
{
    * ppxTimerTaskTCBBuffer = &timer_task_tcb;
    * ppxTimerTaskStackBuffer = timer_task_stack;
    * pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
    
/**
 * @brief       FreeRTOS例程入口函数
 * @param       无
 * @retval      无
 */
void freertos_demo(void)
{    
    start_task_handler = xTaskCreateStatic( (TaskFunction_t )   start_task,
                                            (char *         )   "start_task", 
                                            (uint32_t       )   START_TASK_STACK_SIZE,
                                            (void *         )   NULL,
                                            (UBaseType_t    )   START_TASK_PRIO,
                                            (StackType_t *  )   start_task_stack,
                                            (StaticTask_t * )   &start_task_tcb );
    vTaskStartScheduler();
}


void start_task( void * pvParameters )
{
    taskENTER_CRITICAL();               /* 进入临界区 */
    
    task1_handler = xTaskCreateStatic(  (TaskFunction_t )   task1,
                                        (char *         )   "task1", 
                                        (uint32_t       )   TASK1_STACK_SIZE,
                                        (void *         )   NULL,
                                        (UBaseType_t    )   TASK1_PRIO,
                                        (StackType_t *  )   task1_stack,
                                        (StaticTask_t * )   &task1_tcb );
                        
    task2_handler = xTaskCreateStatic(  (TaskFunction_t )   task2,
                                        (char *         )   "task2", 
                                        (uint32_t       )   TASK2_STACK_SIZE,
                                        (void *         )   NULL,
                                        (UBaseType_t    )   TASK2_PRIO,
                                        (StackType_t *  )   task2_stack,
                                        (StaticTask_t * )   &task2_tcb );
                        
    task3_handler = xTaskCreateStatic(  (TaskFunction_t )   task3,
                                        (char *         )   "task3", 
                                        (uint32_t       )   TASK3_STACK_SIZE,
                                        (void *         )   NULL,
                                        (UBaseType_t    )   TASK3_PRIO,
                                        (StackType_t *  )   task3_stack,
                                        (StaticTask_t * )   &task3_tcb );
    
    vTaskDelete(start_task_handler);
    taskEXIT_CRITICAL();                /* 退出临界区 */
}

/* 任务一，实现LED0每500ms翻转一次 */
void task1( void * pvParameters )
{
    while(1)
    {
        printf("task1正在运行！！！\r\n");
        LED0_TOGGLE();
        vTaskDelay(500);
    }
}

/* 任务二，实现LED1每500ms翻转一次 */
void task2( void * pvParameters )
{
    while(1)
    {
        printf("task2正在运行！！！\r\n");
        LED1_TOGGLE();
        vTaskDelay(500);
    }
}

/* 任务三，判断按键KEY0，按下KEY0删除task1 */
void task3( void * pvParameters )
{
    uint8_t key = 0;
    while(1)
    {
        printf("task3正在运行！！！\r\n");
        key = key_scan(0);
        if(key == KEY0_PRES)
        {
            if(task1_handler != NULL)
            {
                printf("删除task1任务\r\n");
                vTaskDelete(task1_handler);
                task1_handler = NULL;
            }
        }
        vTaskDelay(10);
    }
}
```

# 任务的挂起与恢复

## API函数

|       API函数        |           描述           |
| :------------------: | :----------------------: |
|    vTaskSuspend()    |         挂起任务         |
|    vTaskResume()     |     恢复被挂起的任务     |
| xTaskResumeFromISR() | 在中断中恢复被挂起的任务 |

- 挂起：挂起任务类似暂停，可恢复； 删除任务，无法恢复
- 恢复：恢复被挂起的任务
- FromISR：带FromISR后缀是在中断函数中专用的API函数
- 空闲任务无法被阻塞和挂起

## 任务挂起函数

### 介绍

```C
/*
* 形参：
- xTaskToSuspend 待挂起任务的任务句柄
*/
void vTaskSuspend(TaskHandle_t xTaskToSuspend) 
```

注

1. 此函数用于挂起任务，使用时需将宏 INCLUDE_vTaskSuspend 配置为 1
2. 无论优先级如何，被挂起的任务都将不再被执行，直到任务被恢复
3. 当传入的参数为NULL，则代表挂起任务自身（当前正在运行的任务）

### 内部实现

1. 获取所要挂起任务的控制块：通过传入的任务句柄，判断所需要挂起哪个任务，NULL代表挂起自身
2. 移除所在列表：将要挂起的任务从相应的状态列表和事件列表中移除（就绪或阻塞列表）
3. 插入挂起任务列表：将待挂起任务的任务状态列表向插入到挂起态任务列表末尾
4. 判断任务调度器是否运行：在运行，更新下一次阻塞时间，防止被挂起任务为下一次阻塞超时任务
5. 判断待挂起任务是否为当前任务：如果挂起的是任务自身，且调度器正在运行，需要进行一次任务切换；调度器没有运行，判断挂起任务数是否等于任务总数，是，当前控制块赋值为NULL；否，寻找下一个最高优先级任务

## 任务中恢复任务函数

### 介绍

```c
/*
* 形参：
- xTaskToResume 待恢复任务的任务句柄
*/
void vTaskResume(TaskHandle_t xTaskToResume) 
```

注

1. 使用该函数注意宏 INCLUDE_vTaskSuspend 必须定义为 1
2. 任务无论被 vTaskSuspend() 挂起多少次，只需在任务中调用 vTakResume() 恢复一次，就可以继续运行。且被恢复的任务会进入就绪态

### 内部实现

1. 恢复任务不能是正在运行任务
2. 判断任务是否在挂起列表中：是，就会将该任务在挂起列表中移除， 将该任务添加到就绪列表中
3. 判断恢复任务优先级：判断恢复的任务优先级是否大于当前正在运行的 是的话执行任务切换

## 中断中恢复任务函数

### 介绍

```c
/*
* 返回值：
- pdTRUE  任务恢复后需要进行任务切换
- pdFALSE 任务恢复后不需要进行任务切换
* 形参：
- xTaskToResume 待恢复任务的任务句柄
*/
BaseType_t xTaskResumeFromISR(TaskHandle_t xTaskToResume) 
```

注

1. 使用该函数注意宏 INCLUDE_vTaskSuspend 和 INCLUDE_xTaskResumeFromISR 必须定义为 1
2. 该函数专用于中断服务函数中，用于解挂被挂起任务
3. 中断服务程序中要调用 freeRTOS 的 API 函数，则中断优先级不能高于FreeRTOS所管理的最高优先级（5~15）
4. FreeTROS 不推荐设置子优先级位，这意味着中断可以打断抢占优先级较低的中断，但同一抢占优先级下的中断不会被排序

### 内部实现

1. 关闭freertos可管理中断，防止被其他的中断打断，并返回关闭前basepri寄存器的值
2. 判断是否有挂起任务
3. 将前面保存的basepri的值，恢复回来
4. 返回 xYieldRequired 的值，用于决定是否需要进行任务切换

## 实例

<img src=".\img\image-20250210165859258.png" alt="image-20250210165859258" style="zoom:40%;" />

```c
/*freertos_demo.c*/

/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define START_TASK_PRIO         1
#define START_TASK_STACK_SIZE   128
TaskHandle_t    start_task_handler;
void start_task( void * pvParameters );

/* TASK1 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK1_PRIO         2
#define TASK1_STACK_SIZE   128
TaskHandle_t    task1_handler;
void task1( void * pvParameters );

/* TASK2 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK2_PRIO         3
#define TASK2_STACK_SIZE   128
TaskHandle_t    task2_handler;
void task2( void * pvParameters );

/* TASK3 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK3_PRIO         4
#define TASK3_STACK_SIZE   128
TaskHandle_t    task3_handler;
void task3( void * pvParameters );

/**
 * @brief       FreeRTOS例程入口函数
 * @param       无
 * @retval      无
 */
void freertos_demo(void)
{    
    xTaskCreate((TaskFunction_t         )   start_task,
                (char *                 )   "start_task",
                (configSTACK_DEPTH_TYPE )   START_TASK_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   START_TASK_PRIO,
                (TaskHandle_t *         )   &start_task_handler );
    vTaskStartScheduler();
}

void start_task( void * pvParameters )
{
    taskENTER_CRITICAL();               /* 进入临界区 */
    xTaskCreate((TaskFunction_t         )   task1,
                (char *                 )   "task1",
                (configSTACK_DEPTH_TYPE )   TASK1_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK1_PRIO,
                (TaskHandle_t *         )   &task1_handler );
                
    xTaskCreate((TaskFunction_t         )   task2,
                (char *                 )   "task2",
                (configSTACK_DEPTH_TYPE )   TASK2_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK2_PRIO,
                (TaskHandle_t *         )   &task2_handler );
                
    xTaskCreate((TaskFunction_t         )   task3,
                (char *                 )   "task3",
                (configSTACK_DEPTH_TYPE )   TASK3_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK3_PRIO,
                (TaskHandle_t *         )   &task3_handler );
    vTaskDelete(NULL);
    taskEXIT_CRITICAL();                /* 退出临界区 */
}

/* 任务一，实现LED0每500ms翻转一次 */
void task1( void * pvParameters )
{
    uint32_t task1_num = 0;
    while(1)
    {
        printf("task1_num:%d\r\n",++task1_num);
        LED0_TOGGLE();
        vTaskDelay(500);
    }
}

/* 任务二，实现LED1每500ms翻转一次 */
void task2( void * pvParameters )
{
    uint32_t task2_num = 0;
    while(1)
    {
        printf("task2_num:%d\r\n",++task2_num);
        LED1_TOGGLE();
        vTaskDelay(500);
    }
}

/* 任务三，判断按键KEY0，按下KEY0删除task1 */
void task3( void * pvParameters )
{
    uint8_t key = 0;
    while(1)
    {
        key = key_scan(0);
        if(key == KEY0_PRES)
        {
            printf("挂起task1\r\n");
            vTaskSuspend(task1_handler);
        }else if(key == KEY1_PRES)
        {
            printf("在任务中恢复task1\r\n");
            vTaskResume(task1_handler);
        }
        vTaskDelay(10);
    }
}
```

```c
/*exit.c*/
/**
 * @brief       KEY2 外部中断服务程序
 * @param       无
 * @retval      无
 */
void KEY2_INT_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(KEY2_INT_GPIO_PIN);        /* 调用中断处理公用函数，清除KEY2所在中断线的中断标志位，中断下半部在HAL_GPIO_EXTI_Callback执行 */
    __HAL_GPIO_EXTI_CLEAR_IT(KEY2_INT_GPIO_PIN);        /* HAL库默认先清中断再处理回调，退出时再清一次中断，避免按键抖动误触发 */
}

/**
 * @brief       中断服务程序中需要做的事情
                在HAL库中所有的外部中断服务函数都会调用此函数
 * @param       GPIO_Pin:中断引脚号
 * @retval      无
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    delay_ms(20);      /* 消抖 */
    switch(GPIO_Pin)
    {
        BaseType_t xYieldRequired;					//返回值
        case KEY2_INT_GPIO_PIN:
            if (KEY2 == 0)
            {
                xYieldRequired = xTaskResumeFromISR(task1_handler);
                printf("在中断中恢复task1\r\n");
            }
            if(xYieldRequired == pdTRUE)
            {
                portYIELD_FROM_ISR(xYieldRequired);	//进行任务切换
            }
            break;
        default : break;
    }
}

/**
 * @brief       外部中断初始化程序
 * @param       无
 * @retval      无
 */
void extix_init(void)
{
    GPIO_InitTypeDef gpio_init_struct;
    
    key_init();    
    gpio_init_struct.Pin = KEY2_INT_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_IT_FALLING;            /* 下降沿触发 */
    gpio_init_struct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(KEY2_INT_GPIO_PORT, &gpio_init_struct);    /* KEY2配置为下降沿触发中断 */
    
    HAL_NVIC_SetPriority(KEY2_INT_IRQn, 5, 0);               /* 抢占5，子优先级0 */
    HAL_NVIC_EnableIRQ(KEY2_INT_IRQn);                       /* 使能中断线2 */
}
```

# 任务相关API

## 概要

这些API函数主要用于程序调试阶段，查看任务运行状态，以及统计任务时间占比（空闲任务占比越大，代表应用程序压力越小）。

|             函数              |              描述              |
| :---------------------------: | :----------------------------: |
|      uxTaskPriorityGet()      |         获取任务优先级         |
|      vTaskPrioritySet()       |         设置任务优先级         |
|   uxTaskGetNumberOfTasks()    |      获取系统中任务的数量      |
|    uxTaskGetSystemState()     |      获取所有任务状态信息      |
|        vTaskGetInfo()         |     获取指定单个的任务信息     |
|  xTaskGetCurrentTaskHandle()  |     获取当前任务的任务句柄     |
|       xTaskGetHandle()        | 根据任务名获取该任务的任务句柄 |
| uxTaskGetStackHighWaterMark() | 获取任务的任务栈历史剩余最小值 |
|        eTaskGetState()        |          获取任务状态          |
|          vTaskList()          | 以“表格”形式获取所有任务的信息 |
|    vTaskGetRunTimeStats()     |       获取任务的运行时间       |

## uxTaskPriorityGet

此函数用于获取指定任务的任务优先级，使用该函数需将宏 INCLUDE_uxTaskPriorityGet 置 1。

```c
/*
* 形参：
- xTask 要查找的任务句柄，NULL代表任务自身
* 返回值：
- 整数 任务优先级数值
*/
UBaseType_t  uxTaskPriorityGet( const TaskHandle_t xTask );
```

## vTaskPrioritySet

此函数用于改变某个任务的任务优先级，使用该函数需将宏 INCLUDE_vTaskPrioritySet 为 1 。

```c
/*
* 形参：
- xTask 要查找的任务句柄，NULL代表任务自身
- uxNewPriority 需要设置的任务优先级
*/
void vTaskPrioritySet( TaskHandle_t xTask, UBaseType_t uxNewPriority );
```

## uxTaskGetNumberOfTasks

此函数用于获取系统中任务的任务数量。

```c
/*
* 返回值：
- 整数 系统中任务的数量          
*/
UBaseType_t uxTaskGetNumberOfTasks(void);
```

注

1. vTaskStartScheduler()函数中自动启动空闲任务与软件定时器任务

## uxTaskGetSystemState

此函数用于获取系统中所有任务的任务状态信息，需申请内存用来存放任务状态信息，使用该函数需将宏 configUSE_TRACE_FACILITY 置 1。

```c
/*
* 形参：
- xTaskStatusArray 指向TaskStatus_t结构体数组首地址
- uxArraySize 接收信息的数组大小
- pulTotalRunTime 系统总运行时间，为NULL 则省略总运行时间值
* 返回值：
- 整数 获取信息的任务数量
*/
UBaseType_t uxTaskGetSystemState(  TaskStatus_t* const pxTaskStatusArray,
								   const UBaseType_t uxArraySize,
                                   configRUN_TIME_COUNTER_TYPE * const pulTotalRunTime);
```

TaskStatus_t 结构体

```c
typedef struct xTASK_STATUS
{
    TaskHandle_t 					xHandle;                /* 任务句柄 */ 
    const char *					pcTaskName;          	/* 任务名 */ 
    UBaseType_t						xTaskNumber;           	/* 任务编号 创建顺序 */ 
    eTaskState e					CurrentState;          	/* 任务状态 */ 
    UBaseType_t 					uxCurrentPriority;      /* 任务优先级 */ 
    UBaseType_t 					uxBasePriority;        	/* 任务原始优先级*/ 
    configRUN_TIME_COUNTER_TYPE		ulRunTimeCounter; 		/* 任务运行时间*/
    StackType_t * 					pxStackBase;       		/* 任务栈基地址 */ 
    configSTACK_DEPTH_TYPE 			usStackHighWaterMark;   /* 任务栈历史剩余最小值 */ 
} TaskStatus_t;
```

## vTaskGetInfo

此函数用于获取指定的单个任务的状态信息，使用该函数需将宏 configUSE_TRACE_FACILITY 置 1 。

```c
/*
* 形参：
- xTask	指定获取信息的任务的句柄
- pxTaskStatus 接收任务信息的变量
- xGetFreeStackSpace 任务栈历史剩余最小值，当为“pdFALSE” 则跳过这个步骤，为“pdTRUE”则检查历史剩余最小堆栈
- eState  任务状态，可直接赋值，如想获取代入“eInvalid”
*/
void vTaskGetInfo( 	TaskHandle_t 	xTask,
                  	TaskStatus_t * 	pxTaskStatus,
                  	BaseType_t 		xGetFreeStackSpace,		   
                  	eTaskState 		eState);    
```

eTaskState 结构体

```c
typedef enum
{
	eRunning = 0,	/* 运行态 */ 
	eReady			/* 就绪态 */ 
	eBlocked, 		/* 阻塞态 */ 
	eSuspended, 	/* 挂起态 */ 
	eDeleted, 		/* 任务被删除 */ 
	eInvalid		/* 无效 */ 
} eTaskState;
```

## xTaskGetCurrentTaskHandle

此函数用于获取当前任务的任务句柄， 使用该函数需将宏 INCLUDE_xTaskGetCurrentTaskHandle 置 1。

```c
/*
* 返回值：
- TaskHandle_t 当前任务的任务句柄
*/
TaskHandle_t xTaskGetCurrentTaskHandle( void ); 
```

## xTaskGetHandle

此函数用于通过任务名获取任务句柄 ， 使用该函数需将宏 INCLUDE_xTaskGetHandle 置 1。

```c
/*
* 形参：
- pcNameToQuery 任务名
* 返回值：
- TaskHandle 任务句柄
*/
TaskHandle_t xTaskGetHandle(const char * pcNameToQuery); 
```

## uxTaskGetStackHighWaterMark

此函数用于获取指定任务的任务栈历史最小剩余堆栈，返回值为0时会内存溢出，使用该函数需将宏 INCLUDE_uxTaskGetStackHighWaterMark 置1。

```c
/*
* 形参：
- xTask 任务句柄
* 返回值：
- UBaseType_t 任务栈的历史剩余最小值 单位为字节
*/
UBaseType_t uxTaskGetStackHighWaterMark( TaskHandle_t  xTask );
```

## eTaskGetState

此函数用于查询某个任务的运行状态，使用此函数需将宏 INCLUDE_eTaskGetState 置1。

```c
/*
* 形参：
- xTask 待获取状态任务的任务句柄
* 返回值：
- eTaskState 任务状态
*/
eTaskState eTaskGetState(TaskHandle_t xTask);
```

## vTaskList

此函数用于以“表格”的形式获取系统中任务的信息 ，使用此函数需将宏 configUSE_TRACE_FACILITY 和configUSE_STATS_FORMATTING_FUNCTIONS 置1 。

```c
/*
* 形参：
- pcWriteBuffer 接收任务信息的缓存指针
*/
void vTaskList(char * pcWriteBuffer);
```

例如

<img src=".\img\image-20250216213118798.png" alt="image-20250216213118798" style="zoom:70%;" />

- Name：创建任务的时候给任务分配的名字
- State：任务的壮态信息，X 是运行态，B 是阻塞态，R 是就绪态，S 是挂起态，D 是删除态
- Priority：任务优先级
- Stack：堆栈历史最小剩余大小 单位字节
- Num：任务编号，唯一的，当多个任务使用同一个任务名的时候可以通过此编号来做区分

## vTaskGetRunTimeStats

此函数用于统计任务的运行时间信息，使用此函数需将宏 configGENERATE_RUN_TIME_STATS、configUSE_STATS_FORMATTING_FUNCTIONS 置1 。

当将此宏 configGENERATE_RUN_TIME_STAT 置1之后，还需要实现2个宏定义

- portCONFIGURE_TIMER_FOR_RUNTIME_STATE() ：用于初始化用于配置任务运行时间统计的时基定时器
- portGET_RUN_TIME_COUNTER_VALUE()：用于获取该功能时基硬件定时器计数的计数值 

注

1. 这个时基定时器的计时精度需高于系统时钟节拍精度的10至100倍

```c
/*
* 形参：
- pcWriteBuffer  接收任务运行时间信息的缓存指针
*/
Void vTaskGetRunTimeStats( char * pcWriteBuffer ) 
```

例如

<img src=".\img\image-20250216213446695.png" alt="image-20250216213446695" style="zoom:70%;" />

- Task：任务名称
- Abs Time：任务实际运行的绝对时间，时基定时器中断时间*Abs Time为实际运行时间（s）
- % Time：占总处理时间的百分比

## 实例

### 任务状态查询

<img src=".\img\image-20250216214224799.png" alt="image-20250216214224799" style="zoom:40%;" />

```c
/*freertos_demo.c*/

/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define START_TASK_PRIO         1
#define START_TASK_STACK_SIZE   128
TaskHandle_t    start_task_handler;
void start_task( void * pvParameters );

/* TASK1 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK1_PRIO         2
#define TASK1_STACK_SIZE   128
TaskHandle_t    task1_handler;
void task1( void * pvParameters );

/* TASK2 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK2_PRIO         3
#define TASK2_STACK_SIZE   128
TaskHandle_t    task2_handler;
void task2( void * pvParameters );
/**
 * @brief       FreeRTOS例程入口函数
 * @param       无
 * @retval      无
 */
void freertos_demo(void)
{    
    xTaskCreate((TaskFunction_t         )   start_task,
                (char *                 )   "start_task",
                (configSTACK_DEPTH_TYPE )   START_TASK_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   START_TASK_PRIO,
                (TaskHandle_t *         )   &start_task_handler );
    vTaskStartScheduler();
}

void start_task( void * pvParameters )
{
    taskENTER_CRITICAL();               /* 进入临界区 */
    xTaskCreate((TaskFunction_t         )   task1,
                (char *                 )   "task1",
                (configSTACK_DEPTH_TYPE )   TASK1_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK1_PRIO,
                (TaskHandle_t *         )   &task1_handler );
                
    xTaskCreate((TaskFunction_t         )   task2,
                (char *                 )   "task2",
                (configSTACK_DEPTH_TYPE )   TASK2_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK2_PRIO,
                (TaskHandle_t *         )   &task2_handler );
    vTaskDelete(NULL);
    taskEXIT_CRITICAL();                /* 退出临界区 */
}

/* 任务一，实现LED0每500ms翻转一次 */
void task1( void * pvParameters )
{
    while(1)
    {
        LED0_TOGGLE();
        vTaskDelay(500);
    }
}

char task_buff[500];
/* 任务二，实现任务状态查询API函数使用 */
void task2( void * pvParameters )
{
    UBaseType_t priority_num = 0;
    UBaseType_t task_num = 0;
    UBaseType_t task_num2 = 0;
    TaskStatus_t * status_array = 0;
    TaskStatus_t * status_array2 = 0;
    TaskHandle_t task_handle = 0;
    UBaseType_t task_stack_min = 0;
    eTaskState state = 0;
    
    uint8_t i = 0;
    
    vTaskPrioritySet( task2_handler,4 );
    priority_num = uxTaskPriorityGet( NULL );
    printf("task2任务优先级为%ld\r\n",priority_num);
    
    task_num = uxTaskGetNumberOfTasks();
    printf("任务数量：%ld\r\n",task_num);
    
    status_array = mymalloc(SRAMIN,(sizeof(TaskStatus_t) * task_num));
    task_num2 = uxTaskGetSystemState( status_array,task_num,NULL);
    printf("任务名\t\t任务优先级\t任务编号\r\n");
    for(i = 0; i < task_num2; i++)
    {
        printf("%s\t\t%ld\t%ld\r\n",
                status_array[i].pcTaskName,
                status_array[i].uxCurrentPriority,
                status_array[i].xTaskNumber);
    }
    
    status_array2 = mymalloc(SRAMIN,sizeof(TaskStatus_t));
    vTaskGetInfo( task2_handler,status_array2,pdTRUE,eInvalid);
    printf("任务名：%s\r\n",status_array2->pcTaskName);
    printf("任务优先级：%ld\r\n",status_array2->uxCurrentPriority);
    printf("任务编号：%ld\r\n",status_array2->xTaskNumber);
    printf("任务状态：%d\r\n",status_array2->eCurrentState);
    
    task_handle = xTaskGetHandle( "task1" );
    printf("任务句柄：%#x\r\n",(int)task_handle);
    printf("task1的任务句柄：%#x\r\n",(int)task1_handler);
    
    state = eTaskGetState( task2_handler );
    printf("当前task2的任务状态为：%d\r\n",state);
    
    vTaskList( task_buff );
    printf("%s\r\n",task_buff);
    while(1)
    {
//        task_stack_min = uxTaskGetStackHighWaterMark( task2_handler );
//        printf("task2历史剩余最小堆栈为%ld\r\n",task_stack_min);
        vTaskDelay(1000);
    }
}
```

### 任务时间统计

<img src=".\img\image-20250216214448348.png" alt="image-20250216214448348" style="zoom:50%;" />

```c
/*FreeRTOSConfig.h*/

/* 运行时间和任务状态统计相关定义 */
#define configGENERATE_RUN_TIME_STATS                   1			                       /* 1: 使能任务运行时间统计功能, 默认: 0 */
#if configGENERATE_RUN_TIME_STATS
#include "timer.h" 
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS(void)    ConfigureTimeForRunTimeStats(void)	//配置时基定时器
extern uint32_t FreeRTOSRunTimeTicks;
#define portGET_RUN_TIME_COUNTER_VALUE()                FreeRTOSRunTimeTicks
#endif
#define configUSE_TRACE_FACILITY                        1              				         /* 1: 使能可视化跟踪调试, 默认: 0 */
#define configUSE_STATS_FORMATTING_FUNCTIONS            1                      				 /* 1: configUSE_TRACE_FACILITY为1时，会编译vTaskList()和vTaskGetRunTimeStats()函数, 默认: 0 */
```

```c
/*timer.c*/

uint32_t FreeRTOSRunTimeTicks;

TIM_HandleTypeDef TIM3_Handler;      //定时器句柄 

//时基定时器配置
void ConfigureTimeForRunTimeStats(void)
{
	TIM3_Init(10-1,90-1);
	FreeRTOSRunTimeTicks = 0;
}

//通用定时器3中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!
void TIM3_Init(u16 arr,u16 psc)
{  
    TIM3_Handler.Instance = TIM3;                       	   	//通用定时器3
    TIM3_Handler.Init.Prescaler = psc;                    	 	//分频系数
    TIM3_Handler.Init.CounterMode=TIM_COUNTERMODE_UP;    		//向上计数器
    TIM3_Handler.Init.Period=arr;                        		//自动装载值
    TIM3_Handler.Init.ClockDivision=TIM_CLOCKDIVISION_DIV1;//时钟分频因子
    HAL_TIM_Base_Init(&TIM3_Handler);
    
    HAL_TIM_Base_Start_IT(&TIM3_Handler); //使能定时器3和定时器3更新中断：TIM_IT_UPDATE   
}

//定时器底册驱动，开启时钟，设置中断优先级
//此函数会被HAL_TIM_Base_Init()函数调用
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if(htim->Instance==TIM3)
	{
		__HAL_RCC_TIM3_CLK_ENABLE();            //使能TIM3时钟
		HAL_NVIC_SetPriority(TIM3_IRQn,6,0);    //设置中断优先级，抢占优先级6，子优先级0
		HAL_NVIC_EnableIRQ(TIM3_IRQn);          //开启ITM3中断   
	}
}

//定时器3中断服务函数
void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TIM3_Handler);
}

//回调函数，定时器中断服务函数调用
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim==(&TIM3_Handler))
    {
			FreeRTOSRunTimeTicks++;
      //  LED1=!LED1;        //LED1反转
    }
}
```

```c
/*freertos_demo.c*/

/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define START_TASK_PRIO         1
#define START_TASK_STACK_SIZE   128
TaskHandle_t    start_task_handler;
void start_task( void * pvParameters );

/* TASK1 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK1_PRIO         2
#define TASK1_STACK_SIZE   128
TaskHandle_t    task1_handler;
void task1( void * pvParameters );

/* TASK2 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK2_PRIO         3
#define TASK2_STACK_SIZE   128
TaskHandle_t    task2_handler;
void task2( void * pvParameters );

/**
 * @brief       FreeRTOS例程入口函数
 * @param       无
 * @retval      无
 */
void freertos_demo(void)
{    
    xTaskCreate((TaskFunction_t         )   start_task,
                (char *                 )   "start_task",
                (configSTACK_DEPTH_TYPE )   START_TASK_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   START_TASK_PRIO,
                (TaskHandle_t *         )   &start_task_handler );
    vTaskStartScheduler();
}

void start_task( void * pvParameters )
{
    taskENTER_CRITICAL();               /* 进入临界区 */
    xTaskCreate((TaskFunction_t         )   task1,
                (char *                 )   "task1",
                (configSTACK_DEPTH_TYPE )   TASK1_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK1_PRIO,
                (TaskHandle_t *         )   &task1_handler );
                
    xTaskCreate((TaskFunction_t         )   task2,
                (char *                 )   "task2",
                (configSTACK_DEPTH_TYPE )   TASK2_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK2_PRIO,
                (TaskHandle_t *         )   &task2_handler );
    vTaskDelete(NULL);
    taskEXIT_CRITICAL();                /* 退出临界区 */
}

/* 任务一，实现LED0每500ms翻转一次 */
void task1( void * pvParameters )
{
    while(1)
    {
        LED0_TOGGLE();
        vTaskDelay(500);
    }
}

char task_buff[500];
/* 任务二，实现任务运行时间统计API函数的使用 */
void task2( void * pvParameters )
{
    uint8_t key = 0;
    while(1)
    {
        key = key_scan(0);
        if(key == KEY0_PRES)
        {
            vTaskGetRunTimeStats(task_buff);		//耗时，仅用于调试阶段
            printf("%s\r\n",task_buff);
        }
        vTaskDelay(10);
    }
}
```

#  中断管理

## 中断

让CPU打断正常运行的程序，转而去处理紧急的事件（程序），就叫中断。

中断执行机制，可简单概括为三步

1. 中断请求：外设产生中断请求（GPIO外部中断、定时器中断等）
2. 响应中断：CPU停止执行当前程序，转而去执行中断处理程序（ISR）
3. 退出中断：执行完毕，返回被打断的程序处，继续往下执行

## 中断优先级分组设置

### 介绍

ARM Cortex-M 使用了 8 位宽的寄存器来配置中断的优先等级，这个寄存器就是中断优先级配置寄存器。但STM32只用了中断优先级配置寄存器的高4位，所以STM32提供了最大16级的中断优先等级。

<img src=".\img\image-20250212154357997.png" alt="image-20250212154357997" style="zoom:50%;" />

STM32 的中断优先级可以分为抢占优先级和子优先级 

- 抢占优先级：抢占优先级高的中断可以打断正在执行但抢占优先级低的中断
- 子优先级：当**同时**发生具有相同抢占优先级的两个中断时，子优先级数值小的优先执行，但不可抢占

### 分组方式

一共有 5 种分配方式，对应着中断优先级分组的 5 个组 

<img src=".\img\image-20250211010843500.png" alt="image-20250211010843500" style="zoom:60%;" />

通过调用函数`HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4）`即可完成设置。

注

1. 中断优先级数值越小越优先

### FreeRTOS中应用

建议

- 低于 configMAX_SYSCALL_INTERRUPT_PRIORITY 优先级的中断里才允许调用 FreeRTOS 的API函数
- 建议将所有优先级位指定为抢占优先级位，方便FreeRTOS管理，即调用函数`HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4)`
- 中断优先级数值越小越优先，任务优先级数值越大越优先

<img src=".\img\image-20250211011334653.png" alt="image-20250211011334653" style="zoom:50%;" />

## 中断相关寄存器

### 系统中断优先级配置寄存器

三个系统中断优先级配置寄存器，分别为 SHPR1、 SHPR2、 SHPR3 

- SHPR1寄存器地址：0xE000ED18
- SHPR2寄存器地址：0xE000ED1C
- SHPR3寄存器地址：0xE000ED20

<img src=".\img\image-20250212161509912.png" alt="image-20250212161509912" style="zoom:60%;" />

**FreeRTOS配置PendSV和Systick中断优先级**

通过SHPR3将PendSV和SysTick的中断优先级设置为最低优先级（15），保证系统任务切换 不会阻塞系统其他中断的响应。

- sysTick 主要用于提供时间基准和触发任务调度，是操作系统时间管理的核心
- PendSV 主要用于处理任务切换和上下文管理，确保任务切换的高效性和灵活性

### 中断屏蔽寄存器

三个中断屏蔽寄存器，分别为 PRIMASK、 FAULTMASK 和 BASEPRI 。

<img src=".\img\image-20250212155037749.png" alt="image-20250212155037749" style="zoom:60%;" />

FreeRTOS所使用的中断管理就是利用的 BASEPRI 这个寄存器。

- BASEPRI：屏蔽优先级低于某一个阈值的中断，如BASEPRI设置为0x50，代表中断优先级在5\~15内的均被屏蔽，0~4的中断优先级正常执行；当设置为0时，则不关闭任何中断

<img src=".\img\image-20250212155355113.png" alt="image-20250212155355113" style="zoom:60%;" />

在中断服务函数中调度FreeRTOS的API函数需注意

- 中断服务函数的优先级需在FreeRTOS所管理的范围内
- 在中断服务函数里边需调用FreeRTOS的API函数，必须使用带“FromISR”后缀的函数
- 关中函数：`vPortRaiseBASEPRI`
- 开中断函数：`vPortSetBASEPRI`

## 实例

<img src=".\img\image-20250212155559120.png" alt="image-20250212155559120" style="zoom:50%;" />

```C
/*timer.c*/

TIM_HandleTypeDef g_timx_handle;         /* 定时器参数句柄 */
TIM_HandleTypeDef g_tim7_handle;         /* 定时器参数句柄 */
/**
 * @brief       基本定时器TIMX定时中断初始化函数
 * @note
 *              基本定时器的时钟来自APB1,当PPRE1 ≥ 2分频的时候
 *              基本定时器的时钟为APB1时钟的2倍, 而APB1为45M, 所以定时器时钟 = 90Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr : 自动重装值。
 * @param       psc : 时钟预分频数
 * @retval      无
 */
void btim_timx_int_init(uint16_t arr, uint16_t psc)
{
    g_timx_handle.Instance = BTIM_TIMX_INT;                      /* 定时器x */
    g_timx_handle.Init.Prescaler = psc;                          /* 分频 */
    g_timx_handle.Init.CounterMode = TIM_COUNTERMODE_UP;         /* 递增计数模式 */
    g_timx_handle.Init.Period = arr;                             /* 自动装载值 */
    HAL_TIM_Base_Init(&g_timx_handle);
    
    HAL_TIM_Base_Start_IT(&g_timx_handle);                       /* 使能定时器x和定时器更新中断 */
}

/* TIM7初始化函数 */
void btim_tim7_int_init(uint16_t arr, uint16_t psc)
{
    g_tim7_handle.Instance = BTIM_TIM7_INT;                      /* 定时器x */
    g_tim7_handle.Init.Prescaler = psc;                          /* 分频 */
    g_tim7_handle.Init.CounterMode = TIM_COUNTERMODE_UP;         /* 递增计数模式 */
    g_tim7_handle.Init.Period = arr;                             /* 自动装载值 */
    HAL_TIM_Base_Init(&g_tim7_handle);
    
    HAL_TIM_Base_Start_IT(&g_tim7_handle);                       /* 使能定时器x和定时器更新中断 */
}
/**
 * @brief       定时器底层驱动，开启时钟，设置中断优先级
                此函数会被HAL_TIM_Base_Init()函数调用
 * @param       无
 * @retval      无
 */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == BTIM_TIMX_INT)
    {
        BTIM_TIMX_INT_CLK_ENABLE();                     /* 使能TIMx时钟 */
        HAL_NVIC_SetPriority(BTIM_TIMX_INT_IRQn, 6, 0); /* 抢占6，子优先级0 */
        HAL_NVIC_EnableIRQ(BTIM_TIMX_INT_IRQn);         /* 开启ITMx中断 */
    }
    if(htim->Instance == BTIM_TIM7_INT)
    {
        BTIM_TIM7_INT_CLK_ENABLE();                     /* 使能TIM7时钟 */
        HAL_NVIC_SetPriority(BTIM_TIM7_INT_IRQn, 4, 0); /* 抢占4，子优先级0 */
        HAL_NVIC_EnableIRQ(BTIM_TIM7_INT_IRQn);         /* 开启ITM7中断 */
    }
}

/**
 * @brief       基本定时器TIMX中断服务函数
 * @param       无
 * @retval      无
 */
void BTIM_TIMX_INT_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_timx_handle);  /* 定时器回调函数 */
}

/* TIM7中断服务函数 */
void BTIM_TIM7_INT_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_tim7_handle);  /* 定时器回调函数 */
}

/**
 * @brief       回调函数，定时器中断服务函数调用
 * @param       无
 * @retval      无
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == BTIM_TIMX_INT)
    {
        printf("TIM6优先级为6的正在运行\r\n");	//一般中断中不建议使用printf 比较占用时间
    }else if(htim->Instance == BTIM_TIM7_INT)
    {
        printf("TIM7优先级为4的正在运行\r\n");
    }
}
```

```c
/*freertos_demo.c*/

/* START_TASK 任务 配置
* 包括: 任务句柄 任务优先级 堆栈大小 创建任务
*/
#define START_TASK_PRIO         1
#define START_TASK_STACK_SIZE   128
TaskHandle_t    start_task_handler;
void start_task( void * pvParameters );

/* TASK1 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK1_PRIO         2
#define TASK1_STACK_SIZE   128
TaskHandle_t    task1_handler;
void task1( void * pvParameters );

/**
 * @brief       FreeRTOS例程入口函数
 * @param       无
 * @retval      无
 */
void freertos_demo(void)
{    
    xTaskCreate((TaskFunction_t         )   start_task,
                (char *                 )   "start_task",
                (configSTACK_DEPTH_TYPE )   START_TASK_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   START_TASK_PRIO,
                (TaskHandle_t *         )   &start_task_handler );
    vTaskStartScheduler();
}


void start_task( void * pvParameters )
{
    taskENTER_CRITICAL();               /* 进入临界区 */
    xTaskCreate((TaskFunction_t         )   task1,
                (char *                 )   "task1",
                (configSTACK_DEPTH_TYPE )   TASK1_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK1_PRIO,
                (TaskHandle_t *         )   &task1_handler );
    vTaskDelete(NULL);
    taskEXIT_CRITICAL();                /* 退出临界区 */
}

/* 任务一，实现LED0每500ms翻转一次 */
void task1( void * pvParameters )
{
    uint8_t task1_num = 0;
    while(1)
    {
        if(++task1_num == 5)
        {
            task1_num = 0;
            printf("关中断！！\r\n");
            portDISABLE_INTERRUPTS();		//关中断 此时低于优先级5的中断被关闭，而高于优先级5的中断不受影响
            delay_ms(5000);					//须为死延时
            printf("开中断！！！\r\n");		
            portENABLE_INTERRUPTS();		//开中断
        }
        vTaskDelay(1000);					//若调用系统延时又会自动执行portENABLE_INTERRUPTS()开中断				
    }
}
```

```c
int main(void)
{
    HAL_Init();                                 /* 初始化HAL库 */
    sys_stm32_clock_init(360, 25, 2, 8);        /* 设置时钟,180Mhz */
    delay_init(180);                            /* 延时初始化 */
    usart_init(115200);                         /* 串口初始化为115200 */
    led_init();                                 /* 初始化LED */
    key_init();                                 /* 初始化按键 */
    sdram_init();                               /* SRAM初始化 */
    lcd_init();                                 /* 初始化LCD */
    btim_timx_int_init(10000 - 1 ,9000 - 1);    /* 1s中断一次 根据系统选择*/
    btim_tim7_int_init(10000 - 1 ,9000 - 1);    /* 1s中断一次 */
    
    my_mem_init(SRAMIN);                        /* 初始化内部内存池 */
    my_mem_init(SRAMEX);                        /* 初始化外部内存池 */
    my_mem_init(SRAMCCM);                       /* 初始化CCM内存池 */
    
    freertos_demo();
}
```

# 临界段代码保护

## 临界区

临界段代码也叫做临界区，是指那些必须完整运行，不能被打断的代码段。

适用场合

- 外设：需严格按照时序初始化的外设：IIC、SPI等等
- 系统：系统自身需求
- 用户：用户需求

什么可以打断当前程序的运行

- 中断
- 任务调度

## API函数

 FreeRTOS 在进入临界段代码的时候需要关闭中断，当处理完临界段代码以后再打开中断。

|            API函数            |       描述       |
| :---------------------------: | :--------------: |
|     taskENTER_CRITICAL()      | 任务级进入临界段 |
|      taskEXIT_CRITICAL()      | 任务级退出临界段 |
| taskENTER_CRITICAL_FROM_ISR() | 中断级进入临界段 |
| taskEXIT_CRITICAL_FROM_ISR()  | 中断级退出临界段 |

<img src=".\img\image-20250212175349834.png" alt="image-20250212175349834" style="zoom:60%;" />

特点

- 成对使用
- 支持嵌套
- 尽量保持临界段耗时短
- 临界区是直接屏蔽了中断，系统任务调度靠中断，ISR也靠中断

# 调度器挂起与恢复

挂起任务调度器， 只单纯挂起调度器使任务无法调度，但中断正常。

|     **函数**      |    **描述**    |
| :---------------: | :------------: |
| vTaskSuspendAll() | 挂起任务调度器 |
| xTaskResumeAll()  | 恢复任务调度器 |

<img src=".\img\image-20250212175519583.png" alt="image-20250212175519583" style="zoom:60%;" />

注

1. 与临界区不一样的是，挂起任务调度器，未关闭中断
2. 它仅仅是防止了任务之间的资源争夺，中断照样可以直接响应
3. 挂起调度器的方式，适用于临界区位于任务与任务之间，既不用去延时中断，又可以做到临界区的安全

# 列表和列表项

## 介绍

列表是 FreeRTOS 中的一个数据结构，概念上和链表有点类似，列表被用来跟踪 FreeRTOS 中的任务。 

列表项就是存放在列表中的项目，代表着FreeRTOS 中的某一任务。

<img src=".\img\image-20250212225950712.png" alt="image-20250212225950712" style="zoom:60%;" />

列表相当于链表，列表项相当于节点，FreeRTOS 中的列表是一个双向环形链表 。

- 列表的特点：列表项间的地址非连续的，是人为的连接到一起的。列表项的数目是由后期添加的个数决定的，随时可以改变
- 数组的特点：数组成员地址是连续的，数组在最初确定了成员数量后期无法改变

注

1. 在OS中任务的数量是不确定的，并且任务状态是会发生改变的，所以非常适用列表(链表)这种数据结构
2. 有关于列表的东西均在文件 list.c 和 list.h 中

**列表和列表项的关系**

列表初始状态，以及即将插入的两个列表项如下

<img src=".\img\image-20250212231228119.png" alt="image-20250212231228119" style="zoom:50%;" />

## 列表

FreeRTOS中列表一般分为就绪、阻塞、挂起三种列表。

```c
typedef struct xLIST
{
    listFIRST_LIST_INTEGRITY_CHECK_VALUE		/* 校验值 */
    volatile UBaseType_t uxNumberOfItems;		/* 列表中的列表项数量 */
   	ListItem_t * configLIST_VOLATILE pxIndex	/* 用于遍历列表项的指针 */
    MiniListItem_t xListEnd						/* 末尾列表项 */
   	listSECOND_LIST_INTEGRITY_CHECK_VALUE		/* 校验值 */
} List_t;
```

<img src=".\img\image-20250212230500557.png" alt="image-20250212230500557" style="zoom:50%;" />

注

1. 在该结构体中， 包含了两个宏，这两个宏是确定的已知常量， FreeRTOS通过检查这两个常量的值，来判断列表的数据在程序运行过程中，是否遭到破坏 ，该功能一般用于调试， 默认是不开启的 
2. 成员uxNumberOfItems，用于记录列表中列表项的个数（不包含 xListEnd）
3. 成员 pxIndex 用于指向列表中的某个列表项，一般用于遍历列表中的所有列表项 
4. 成员变量 xListEnd 是一个迷你列表项，排在最末尾

## 列表项

列表项是列表中用于存放数据的地方，在 list.h 文件中。

```c
struct xLIST_ITEM
{
    listFIRST_LIST_ITEM_INTEGRITY_CHECK_VALUE			/* 用于检测列表项的数据完整性 */
    configLIST_VOLATILE TickType_t xItemValue			/* 列表项的值 */
    struct xLIST_ITEM * configLIST_VOLATILE pxNext		/* 下一个列表项 */
  	struct xLIST_ITEM * configLIST_VOLATILE pxPrevious	/* 上一个列表项 */
    void * pvOwner										/* 列表项的拥有者 */
    struct xLIST * configLIST_VOLATILE pxContainer; 	/* 列表项所在列表 */
   	listSECOND_LIST_ITEM_INTEGRITY_CHECK_VALUE			/* 用于检测列表项的数据完整性 */
};
typedef struct xLIST_ITEM ListItem_t; 	
```

<img src=".\img\image-20250212230705526.png" alt="image-20250212230705526" style="zoom:50%;" />

注

1. 成员变量 xItemValue 为列表项的值，这个值多用于按升序对列表中的列表项进行排序 
2. 成员变量 pxNext 和 pxPrevious 分别用于指向列表中列表项的下一个列表项和上一个列表项 
3. 成员变量 pxOwner 用于指向包含列表项的对象（通常是任务控制块） 
4. 成员变量 pxContainer 用于指向列表项所在列表

## 迷你列表项

迷你列表项也是列表项，但迷你列表项仅用于标记列表的末尾和挂载其他插入列表中的列表项。

```c
struct xMINI_LIST_ITEM
{
    listFIRST_LIST_ITEM_INTEGRITY_CHECK_VALUE 			/* 用于检测数据完整性 */
	configLIST_VOLATILE TickType_t xItemValue;			/* 列表项的值 */
   	struct xLIST_ITEM * configLIST_VOLATILE pxNext;		/* 上一个列表项 */
   	struct xLIST_ITEM * configLIST_VOLATILE pxPrevious; /* 下一个列表项 */
};
typedef struct xMINI_LIST_ITEM MiniListItem_t;
```

<img src=".\img\image-20250212230935765.png" alt="image-20250212230935765" style="zoom:50%;" />

注

1. 成员变量 xItemValue 为列表项的值，这个值多用于按升序对列表中的列表项进行排序 
2. 成员变量 pxNext 和 pxPrevious 分别用于指向列表中列表项的下一个列表项和上一个列表项 
3. 迷你列表项只用于标记列表的末尾和挂载其他插入列表中的列表项，因此不需要成员变量 pxOwner 和 pxContainer，以节省内存开销

## API函数

|         函数          |        描述        |
| :-------------------: | :----------------: |
|   vListInitialise()   |     初始化列表     |
| vListInitialiseItem() |    初始化列表项    |
|   vListInsertEnd()    | 列表末尾插入列表项 |
|     vListInsert()     |   列表插入列表项   |
|    uxListRemove()     |   列表移除列表项   |

### vListInitialise()

```c
/*
* * 形参：
- pxList 待初始化列表
*/
void vListInitialise(List_t * const pxList);
```

### vListInitialiseItem()

```c
/*
* * 形参：
- pxItem 待初始化列表项
*/
void vListInitialiseItem( ListItem_t * const pxItem);
```

### vListInsert() 

此函数用于将待插入列表的列表项按照列表项值升序进行排序，有序地插入到列表中。 

```c
/*
* * 形参：
- pxList 列表
- pxNewListItem 待插入列表项
*/
void vListInsert(List_t * const pxList, ListItem_t * const pxNewListItem);
```

注

1. 实现原理参考双向循环链表的插入，下同

### vListInsertEnd() 

此函数用于将待插入列表的列表项插入到列表 pxIndex 指针指向的列表项前面，是一种无序的插入方法 。

```c
/*
* * 形参：
- pxList 列表
- pxNewListItem 待插入列表项
*/
void vListInsertEnd(List_t * const pxList,ListItem_t * const pxNewListItem);
```

### uxListRemove() 

此函数用于将列表项从列表项所在列表中移除。

```c
/*
* * 形参：
- pxItemToRemove 待移除的列表项
* * 返回值：
- 整数 表示待移除列表项移除后，所在列表剩余列表项的数量
*/
UBaseType_t  uxListRemove (ListItem_t * const pxItemToRemove );
```
注

1. 并不释放该列表项所在空间，只是从该列表中移除

## 实例

<img src=".\img\image-20250212233146079.png" alt="image-20250212233146079" style="zoom:40%;" />

```c
/* freertos_demo.c */

/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define START_TASK_PRIO         1
#define START_TASK_STACK_SIZE   128
TaskHandle_t    start_task_handler;
void start_task( void * pvParameters );

/* TASK1 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK1_PRIO         2
#define TASK1_STACK_SIZE   128
TaskHandle_t    task1_handler;
void task1( void * pvParameters );

/* TASK2 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK2_PRIO         3
#define TASK2_STACK_SIZE   128
TaskHandle_t    task2_handler;
void task2( void * pvParameters );

List_t          TestList;           /* 定义测试列表 */
ListItem_t      ListItem1;          /* 定义测试列表项1 */
ListItem_t      ListItem2;          /* 定义测试列表项2 */
ListItem_t      ListItem3;          /* 定义测试列表项3 */
/**
 * @brief       FreeRTOS例程入口函数
 * @param       无
 * @retval      无
 */
void freertos_demo(void)
{    
    xTaskCreate((TaskFunction_t         )   start_task,
                (char *                 )   "start_task",
                (configSTACK_DEPTH_TYPE )   START_TASK_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   START_TASK_PRIO,
                (TaskHandle_t *         )   &start_task_handler );
    vTaskStartScheduler();
}

void start_task( void * pvParameters )
{
    taskENTER_CRITICAL();               /* 进入临界区 */
    xTaskCreate((TaskFunction_t         )   task1,
                (char *                 )   "task1",
                (configSTACK_DEPTH_TYPE )   TASK1_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK1_PRIO,
                (TaskHandle_t *         )   &task1_handler );
                
    xTaskCreate((TaskFunction_t         )   task2,
                (char *                 )   "task2",
                (configSTACK_DEPTH_TYPE )   TASK2_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK2_PRIO,
                (TaskHandle_t *         )   &task2_handler );
    vTaskDelete(NULL);
    taskEXIT_CRITICAL();                /* 退出临界区 */
}

/* 任务一，实现LED0每500ms翻转一次 */
void task1( void * pvParameters )
{
    while(1)
    {
        LED0_TOGGLE();
        vTaskDelay(500);
    }
}

/* 任务二，列表项的插入和删除实验 */
void task2( void * pvParameters )
{
    vListInitialise(&TestList);         /* 初始化列表 */
    vListInitialiseItem(&ListItem1);    /* 初始化列表项1 */
    vListInitialiseItem(&ListItem2);    /* 初始化列表项2 */
    vListInitialiseItem(&ListItem3);    /* 初始化列表项3 */
    ListItem1.xItemValue = 40;
    ListItem2.xItemValue = 60;
    ListItem3.xItemValue = 50;

    /* 第二步：打印列表和其他列表项的地址 */
    printf("/**************第二步：打印列表和列表项的地址**************/\r\n");
    printf("项目\t\t\t地址\r\n");
    printf("TestList\t\t0x%p\t\r\n", &TestList);
    printf("TestList->pxIndex\t0x%p\t\r\n", TestList.pxIndex);
    printf("TestList->xListEnd\t0x%p\t\r\n", (&TestList.xListEnd));
    printf("ListItem1\t\t0x%p\t\r\n", &ListItem1);
    printf("ListItem2\t\t0x%p\t\r\n", &ListItem2);
    printf("ListItem3\t\t0x%p\t\r\n", &ListItem3);
    printf("/*************************结束**************************/\r\n");
    
    printf("\r\n/****************第三步：列表项1插入列表***************/\r\n");
    vListInsert((List_t*    )&TestList,         /* 列表 */
                (ListItem_t*)&ListItem1);       /* 列表项 */
    printf("项目\t\t\t\t地址\r\n");
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (ListItem1.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n",(TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (ListItem1.pxPrevious));
    printf("/**************************结束**************************/\r\n");
    
    /* 第四步：列表项2插入列表 */
    printf("\r\n/*****************第四步：列表项2插入列表***************/\r\n");
    vListInsert((List_t*    )&TestList,         /* 列表 */
                (ListItem_t*)&ListItem2);       /* 列表项 */
    printf("项目\t\t\t\t地址\r\n");
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (ListItem1.pxNext));
    printf("ListItem2->pxNext\t\t0x%p\r\n", (ListItem2.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (ListItem1.pxPrevious));
    printf("ListItem2->pxPrevious\t\t0x%p\r\n", (ListItem2.pxPrevious));
    printf("/************************结束***************************/\r\n");
    
    /* 第五步：列表项3插入列表 */
    printf("\r\n/**************第五步：列表项3插入列表******************/\r\n");
    vListInsert((List_t*    )&TestList,         /* 列表 */
                (ListItem_t*)&ListItem3);       /* 列表项 */
    printf("项目\t\t\t\t地址\r\n");
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (ListItem1.pxNext));
    printf("ListItem2->pxNext\t\t0x%p\r\n", (ListItem2.pxNext));
    printf("ListItem3->pxNext\t\t0x%p\r\n", (ListItem3.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (ListItem1.pxPrevious));
    printf("ListItem2->pxPrevious\t\t0x%p\r\n", (ListItem2.pxPrevious));
    printf("ListItem3->pxPrevious\t\t0x%p\r\n", (ListItem3.pxPrevious));
    printf("/*************************结束*************************/\r\n");
    
    /* 第六步：移除列表项2 */
    printf("\r\n/*****************第六步：移除列表项2*****************/\r\n");
    uxListRemove((ListItem_t*   )&ListItem2);   /* 移除列表项 */
    printf("项目\t\t\t\t地址\r\n");
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (ListItem1.pxNext));
    printf("ListItem3->pxNext\t\t0x%p\r\n", (ListItem3.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (ListItem1.pxPrevious));
    printf("ListItem3->pxPrevious\t\t0x%p\r\n", (ListItem3.pxPrevious));
    printf("/*************************结束***************************/\r\n");
    
    /* 第七步：列表末尾添加列表项2 */
    printf("\r\n/**************第七步：列表末尾添加列表项2***************/\r\n");
    TestList.pxIndex = &ListItem1;
    vListInsertEnd((List_t*     )&TestList,     /* 列表 */
                   (ListItem_t* )&ListItem2);   /* 列表项 */
    printf("项目\t\t\t\t地址\r\n");
    printf("TestList->pxIndex\t\t0x%p\r\n", TestList.pxIndex);
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (ListItem1.pxNext));
    printf("ListItem2->pxNext\t\t0x%p\r\n", (ListItem2.pxNext));
    printf("ListItem3->pxNext\t\t0x%p\r\n", (ListItem3.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (ListItem1.pxPrevious));
    printf("ListItem2->pxPrevious\t\t0x%p\r\n", (ListItem2.pxPrevious));
    printf("ListItem3->pxPrevious\t\t0x%p\r\n", (ListItem3.pxPrevious));
    printf("/***********************实验结束*************************/\r\n");
    while(1)
    {
        vTaskDelay(1000);
    }
}
```

# 时间管理

## 延时函数

|       函数        |   描述   |
| :---------------: | :------: |
|   vTaskDelay()    | 相对延时 |
| xTaskDelayUntil() | 绝对延时 |

- 相对延时：指每次延时都是从执行函数vTaskDelay()开始，直到延时指定的时间结束
- 绝对延时：指从任务一开始执行时就进行延时，适用于需要按照一定频率运行的任务

<img src=".\img\image-20250217141050429.png" alt="image-20250217141050429" style="zoom:80%;" />

- (1)为任务主体，也就是任务真正要做的工作
- (2)是任务函数中调用vTaskDelayUntil()对任务进行延时
- (3)为其他任务在运行

**流程**

1. 正在运行的任务调用延时函数，此时将该任务移除就绪列表，并添加到阻塞列表中
2. 滴答中断里边进行计时判断阻塞时间是否到达，如果到达将从阻塞列表移除，添加到就绪列表

## 内部实现

1. 判断延时时间是否大于0，大于0才有效
2. 挂起调度器
3. 将当前正在运行的任务从就绪列表移除，添加到阻塞列表 prvAddCurrentTaskToDelayedList( )
4. 恢复任务调度器
5. 进行一次任务切换

## 实例

<img src=".\img\image-20250217141230997.png" alt="image-20250217141230997" style="zoom:40%;" />

```c
/* freertos_demo.c */

/*FreeRTOS配置*/

/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define START_TASK_PRIO         1
#define START_TASK_STACK_SIZE   128
TaskHandle_t    start_task_handler;
void start_task( void * pvParameters );

/* TASK1 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK1_PRIO         5
#define TASK1_STACK_SIZE   128
TaskHandle_t    task1_handler;
void task1( void * pvParameters );

/* TASK2 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK2_PRIO         3
#define TASK2_STACK_SIZE   128
TaskHandle_t    task2_handler;
void task2( void * pvParameters );

/**
 * @brief       FreeRTOS例程入口函数
 * @param       无
 * @retval      无
 */
void freertos_demo(void)
{    
    xTaskCreate((TaskFunction_t         )   start_task,
                (char *                 )   "start_task",
                (configSTACK_DEPTH_TYPE )   START_TASK_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   START_TASK_PRIO,
                (TaskHandle_t *         )   &start_task_handler );
    vTaskStartScheduler();
}

void start_task( void * pvParameters )
{
    taskENTER_CRITICAL();               /* 进入临界区 */
    xTaskCreate((TaskFunction_t         )   task1,
                (char *                 )   "task1",
                (configSTACK_DEPTH_TYPE )   TASK1_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK1_PRIO,
                (TaskHandle_t *         )   &task1_handler );
                
    xTaskCreate((TaskFunction_t         )   task2,
                (char *                 )   "task2",
                (configSTACK_DEPTH_TYPE )   TASK2_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK2_PRIO,
                (TaskHandle_t *         )   &task2_handler );
    vTaskDelete(NULL);
    taskEXIT_CRITICAL();                /* 退出临界区 */
}

/*task1优先级较高则task2实际时间有时略小于500ms
* task2优先级较高则task1实际时间有时略大于500ms
*/

/* 任务一，演示相对延时函数 520ms */
void task1( void * pvParameters )
{
    while(1)
    {
        LED0_TOGGLE();      /* PB1 */
        delay_ms(20);
        vTaskDelay(500);
    }
}

/* 任务二，演示绝对延时函数 500ms */
void task2( void * pvParameters )
{
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    while(1)
    {
        LED1_TOGGLE();      /* PB0 */
        delay_ms(20);
        vTaskDelayUntil(&xLastWakeTime,500);
    }
}
```

# 队列

## 引入

FreeRTOS基于队列，实现了多种功能，其中包括队列集、互斥信号量、计数型信号量、二值信号量、 递归互斥信号量，因此很有必要深入了解 FreeRTOS 的队列 。

队列是任务到任务、任务到中断、中断到任务数据交流的一种机制（消息传递） 。

类似全局变量，假设有一个全局变量 a = 0，现有两个任务都在写这个变量a

<img src=".\img\image-20250217151052907.png" alt="image-20250217151052907" style="zoom:50%;" />

全局变量的弊端：数据无保护，导致数据不安全，当多个任务同时对该变量操作时，数据易受损。

**使用队列的情况**

<img src=".\img\image-20250217151416501.png" alt="image-20250217151416501" style="zoom:40%;" />

读写队列做好了保护，防止多任务同时访问冲突，我们只需要直接调用API函数即可，简单易用。

<img src=".\img\image-20250217151503560.png" alt="image-20250217151503560" style="zoom:50%;" />

## 简介

在队列中可以存储数量有限、大小固定的数据。队列中的每一个数据叫做“队列项目”，队列能够存储“队列项目”的最大数量称为队列的长度。

在创建队列时，就要指定队列长度以及队列项目的大小。

例如

<img src=".\img\image-20250217151729200.png" alt="image-20250217151729200" style="zoom:50%;" />

队列长度为5个，队列项目大小为10字节。

**FreeRTOS队列特点**

- 数据入队出队方式：队列通常采用“先进先出”(FIFO)的数据存储缓冲机制，即先入队的数据会先从队列中被读取，FreeRTOS中也可以配置为“后进先出”LIFO方式
- 数据传递方式：FreeRTOS中队列采用实际值传递，即将数据拷贝到队列中进行传递， FreeRTOS采用拷贝数据传递，也可以传递指针，所以在传递较大的数据的时候采用指针传递
- 多任务访问：队列不属于某个任务，任何任务和中断都可以向队列发送/读取消息
- 出队、入队阻塞：当任务向一个队列发送消息时，可以指定一个阻塞时间，假设此时当队列已满无法入队
  - 若阻塞时间为0 ：直接返回不会等待
  - 若阻塞时间为0~port_MAX_DELAY ：等待设定的阻塞时间，若在该时间内还无法入队，超时后直接返回不再等待
  - 若阻塞时间为port_MAX_DELAY ：死等，一直等到可以入队为止。出队阻塞与入队阻塞类似

**入队/出队阻塞**

入队阻塞

<img src=".\img\image-20250217152202054.png" alt="image-20250217152202054" style="zoom:40%;" />

队列满了，此时写不进去数据。

- 将该任务的状态列表项挂载在阻塞列表pxDelayedTaskList
- 将该任务的事件列表项挂载在等待发送列表xTasksWaitingToSend

出队阻塞

<img src=".\img\image-20250217152307396.png" alt="image-20250217152307396" style="zoom:40%;" />

队列为空，此时读取不了数据。

- 将该任务的状态列表项挂载在阻塞列表pxDelayedTaskList
- 将该任务的事件列表项挂载在等待接收列表xTasksWaitingToReceive

注

1. 当多个任务因入队/出队阻塞而进入阻塞状态，当队列阻塞状态解除，优先级最高的任务或如果大家的优先级相同，那等待时间最久的任务会进入就绪态

## 基本操作

<img src=".\img\image-20250217152706097.png" alt="image-20250217152706097" style="zoom:50%;" />

<img src=".\img\image-20250217152717035.png" alt="image-20250217152717035" style="zoom:45%;" />

## 队列结构体

```c
typedef struct QueueDefinition 
{
    int8_t * pcHead								/* 存储区域的起始地址 */
    int8_t * pcWriteTo;        					/* 下一个写入的位置 */
    union
    {
    	QueuePointers_t     xQueue; 
		SemaphoreData_t  xSemaphore; 
    } u ;
    List_t xTasksWaitingToSend; 				/* 等待发送列表 */
    List_t xTasksWaitingToReceive;				/* 等待接收列表 */
    volatile UBaseType_t uxMessagesWaiting; 	/* 非空闲队列项目的数量 实现信号量*/
    UBaseType_t uxLength；						/* 队列长度 */
    UBaseType_t uxItemSize;                		/* 队列项目的大小 字节 */
    volatile int8_t cRxLock; 					/* 读取上锁计数器 */
    volatile int8_t cTxLock；					/* 写入上锁计数器 */
   /* 其他的一些条件编译 */
} xQUEUE;
```

当用于队列使用时

```c
typedef struct QueuePointers
{
    int8_t * pcTail; 				/* 存储区的结束地址 */
    int8_t * pcReadFrom;			/* 最后一个读取队列的地址 */
} QueuePointers_t;
```

当用于互斥信号量和递归互斥信号量时 

```c
typedef struct SemaphoreData
{
    TaskHandle_t xMutexHolder;			/* 互斥信号量持有者 */
    UBaseType_t uxRecursiveCallCount;	/* 递归互斥信号量的获取计数器 */
} SemaphoreData_t;
```

队列结构体整体示意图

<img src=".\img\image-20250217153230207.png" alt="image-20250217153230207" style="zoom:50%;" />

## API函数

使用队列的主要流程：创建队列 -> 写队列 -> 读队列。

### 创建队列相关API函数

|         函数         |       描述       |
| :------------------: | :--------------: |
|    xQueueCreate()    | 动态方式创建队列 |
| xQueueCreateStatic() | 静态方式创建队列 |

区别：队列所需的内存空间由 FreeRTOS 从 FreeRTOS 管理的堆中分配，而静态创建需要用户自行分配内存。

**xQueueCreate**

用于使用动态方式创建队列，队列所需内存空间由 FreeRTOS 从 FreeRTOS 管理的堆中分配 。

```c
/*
* 形参：
- uxQueueLength 队列长度
- uxItemSize 队列项目的大小
* 返回值：
- NULL 队列创建失败
- 其他值 队列创建成功，返回队列句柄
*/
#define xQueueCreate( uxQueueLength,uxItemSize) xQueueGenericCreate((uxQueueLength ), ( uxItemSize ), (queueQUEUE_TYPE_BASE ))
```

FreeRTOS 基于队列实现了多种功能，每一种功能对应一种队列类型，queue.h 文件中有定义。

```c
#define queueQUEUE_TYPE_BASE               ( ( uint8_t ) 0U )	/* 队列 */
#define queueQUEUE_TYPE_SET                ( ( uint8_t ) 0U )	/* 队列集 */
#define queueQUEUE_TYPE_MUTEX              ( ( uint8_t ) 1U )	/* 互斥信号量 */
#define queueQUEUE_TYPE_COUNTING_SEMAPHORE ( ( uint8_t ) 2U )   /* 计数型信号量 */
#define queueQUEUE_TYPE_BINARY_SEMAPHORE   ( ( uint8_t ) 3U )	/* 二值信号量 */
#define queueQUEUE_TYPE_RECURSIVE_MUTEX    ( ( uint8_t ) 4U )	/* 递归互斥信号量 */
```

### 往队列写入消息API函数

|            函数            |                        描述                        |
| :------------------------: | :------------------------------------------------: |
|        xQueueSend()        |             往队列的尾部写入消息  FIFO             |
|     xQueueSendToBack()     |               同 xQueueSend()  FIFO                |
|    xQueueSendToFront()     |             往队列的头部写入消息  LIFO             |
|     xQueueOverwrite()      |     覆写队列消息（只用于队列长度为 1  的情况）     |
|    xQueueSendFromISR()     |            在中断中往队列的尾部写入消息            |
| xQueueSendToBackFromISR()  |               同 xQueueSendFromISR()               |
| xQueueSendToFrontFromISR() |            在中断中往队列的头部写入消息            |
|  xQueueOverwriteFromISR()  | 在中断中覆写队列消息（只用于队列长度为 1  的情况） |

- 任务级中，均调用xQueueGenericSend( )，只是写入位置不同

- 中断中，均调用xQueueGenericSendFromISR( )，只是写入位置不同

队列一共有 3 种写入位置

```c
#define queueSEND_TO_BACK        ( ( BaseType_t ) 0 )		/* 写入队列尾部 */
#define queueSEND_TO_FRONT       ( ( BaseType_t ) 1 )		/* 写入队列头部 */
#define queueOVERWRITE           ( ( BaseType_t ) 2 )		/* 覆写队列*/
```

注

1. 覆写方式写入队列，只有在队列的队列长度为 1 时，才能够使用，且阻塞时间为0

**xQueueGenericSend**

```c
/*
* 形参：
- xQueue 待写入的队列
- pvItemToQueue 待写入消息
- xTicksToWait 阻塞超时时间
- xCopyPosition 写入的位置
* 返回值：
- pdTRUE 队列写入成功
- errQUEUE_FULL 队列写入失败
*/
BaseType_t xQueueGenericSend(QueueHandle_t xQueue,					        							const void * const 	pvItemToQueue,
                          	TickType_t xTicksToWait,					        						 const BaseType_t xCopyPosition);
```

### 从队列读取消息API函数

|          函数          |                  描述                  |
| :--------------------: | :------------------------------------: |
|    xQueueReceive()     |     从队列头部读取消息，并删除消息     |
|      xQueuePeek()      |           从队列头部读取消息           |
| xQueueReceiveFromISR() | 在中断中从队列头部读取消息，并删除消息 |
|  xQueuePeekFromISR()   |       在中断中从队列头部读取消息       |

**xQueueReceive**

此函数用于在任务中，从队列中读取消息，并且消息读取成功后，会将消息从队列中移除。

```c
/*
* 形参：
- xQueue 待读取的队列
- pvBuffer 信息读取缓冲区
- xTicksToWait 阻塞超时时间
* 返回值：
- pdTRUE 读取成功
- pdFALSE 读取失败
*/
BaseType_t xQueueReceive( QueueHandle_t xQueue,void * const pvBuffer,TickType_t xTicksToWait );
```

**xQueuePeek**

此函数用于在任务中，从队列中读取消息， 但与函数 xQueueReceive()不同，此函数在成功读取消息后，并不会移除已读取的消息。

```c
/*
* 形参：
- xQueue 待读取的队列
- pvBuffer 信息读取缓冲区
- xTicksToWait 阻塞超时时间
* 返回值：
- pdTRUE 读取成功
- pdFALSE 读取失败
*/
BaseType_t xQueuePeek( QueueHandle_t xQueue,void * const pvBuffer,   TickType_t xTicksToWait);
```

## 实例

<img src=".\img\image-20250217155252246.png" alt="image-20250217155252246" style="zoom:40%;" />

```c
/* freertos_demo.c */

/*FreeRTOS配置*/

/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define START_TASK_PRIO         1
#define START_TASK_STACK_SIZE   128
TaskHandle_t    start_task_handler;
void start_task( void * pvParameters );

/* TASK1 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK1_PRIO         2
#define TASK1_STACK_SIZE   128
TaskHandle_t    task1_handler;
void task1( void * pvParameters );


/* TASK2 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK2_PRIO         3
#define TASK2_STACK_SIZE   128
TaskHandle_t    task2_handler;
void task2( void * pvParameters );

/* TASK3 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK3_PRIO         4
#define TASK3_STACK_SIZE   128
TaskHandle_t    task3_handler;
void task3( void * pvParameters );

QueueHandle_t key_queue;        /* 小数据句柄 */
QueueHandle_t big_date_queue;   /* 大数据句柄 */
char buff[100] = {"我是一个大数组，大大的数组 124214 uhsidhaksjhdklsadhsaklj"};
/**
 * @brief       FreeRTOS例程入口函数
 * @param       无
 * @retval      无
 */
void freertos_demo(void)
{    
    /* 队列的创建 */
    key_queue = xQueueCreate( 2, sizeof(uint8_t) );
    if(key_queue != NULL)
    {
        printf("key_queue队列创建成功！！\r\n");
    }else printf("key_queue队列创建失败！！\r\n");
    
    big_date_queue = xQueueCreate( 1, sizeof(char *) );
    if(big_date_queue != NULL)
    {
        printf("big_date_queue队列创建成功！！\r\n");
    }else printf("big_date_queue队列创建失败！！\r\n");
    
    xTaskCreate((TaskFunction_t         )   start_task,
                (char *                 )   "start_task",
                (configSTACK_DEPTH_TYPE )   START_TASK_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   START_TASK_PRIO,
                (TaskHandle_t *         )   &start_task_handler );
    vTaskStartScheduler();
}

void start_task( void * pvParameters )
{
    taskENTER_CRITICAL();               /* 进入临界区 */
    xTaskCreate((TaskFunction_t         )   task1,
                (char *                 )   "task1",
                (configSTACK_DEPTH_TYPE )   TASK1_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK1_PRIO,
                (TaskHandle_t *         )   &task1_handler );
                
    xTaskCreate((TaskFunction_t         )   task2,
                (char *                 )   "task2",
                (configSTACK_DEPTH_TYPE )   TASK2_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK2_PRIO,
                (TaskHandle_t *         )   &task2_handler );
               
    xTaskCreate((TaskFunction_t         )   task3,
                (char *                 )   "task3",
                (configSTACK_DEPTH_TYPE )   TASK3_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK3_PRIO,
                (TaskHandle_t *         )   &task3_handler );                
    vTaskDelete(NULL);
    taskEXIT_CRITICAL();                /* 退出临界区 */
}

/* 任务一，实现入队 */
void task1( void * pvParameters )
{
    uint8_t key = 0;
    char * buf;
    BaseType_t   err = 0;
    buf = &buff[0]; /* buf = &buff[0] */
    while(1) 
    {
        key = key_scan(0);
        if(key == KEY0_PRES || key == KEY1_PRES)
        {
            err = xQueueSend( key_queue, &key, portMAX_DELAY );
            if(err != pdTRUE)
            {
                printf("key_queue队列发送失败\r\n");
            }
        }else if(key == WKUP_PRES)
        {
            err = xQueueSend( big_date_queue, &buf, portMAX_DELAY );
            if(err != pdTRUE)
            {
                printf("key_queue队列发送失败\r\n");
            }
        }
        vTaskDelay(10);
    }
}

/* 任务二，小数据出队 */
void task2( void * pvParameters )
{
    uint8_t key = 0;
    BaseType_t err = 0;
    while(1)
    {
        err = xQueueReceive( key_queue,&key,portMAX_DELAY);
        if(err != pdTRUE)
        {
            printf("key_queue队列读取失败\r\n");
        }else 
        {
            printf("key_queue读取队列成功，数据：%d\r\n",key);
        }
    }
}

/* 任务三，大数据出队 */
void task3( void * pvParameters )
{
    char * buf;
    BaseType_t err = 0;
    while(1)
    {
        err = xQueueReceive( big_date_queue,&buf,portMAX_DELAY);
        if(err != pdTRUE)
        {
            printf("big_date_queue队列读取失败\r\n");
        }else 
        {
            printf("数据：%s\r\n",buf);
        }
    }
}
```

# 信号量

## 简介

信号量是一种解决同步问题的机制，可以实现对共享资源的有序访问，用于传递状态。

**特点**

- 当计数值大于0，代表有信号量资源
- 当释放信号量，信号量计数值（资源数）加一
- 当获取信号量，信号量计数值（资源数）减一
- 信号量的计数值都有限制：限定最大值。如果最大值被限定为1，那么它就是二值信号量；如果最大值不是1，它就是计数型信号量

**队列与信号量的对比**

|                             队列                             |                            信号量                            |
| :----------------------------------------------------------: | :----------------------------------------------------------: |
| 可以容纳多个数据；  创建队列有两部分内存：队列结构体+队列项存储空间 | 仅存放计数值，无法存放其他数据；创建信号量，只需分配信号量结构体 |
|                 写入队列：当队列满时，可阻塞                 | 释放信号量：不可阻塞，计数值++，  当计数值为最大值时，返回失败 |
|                读取队列：当队列为空时，可阻塞                |         获取信号量：计数值--，  当没有资源时，可阻塞         |

## 二值信号量

### 简介

二值信号量的本质是一个队列长度为 1 的队列 ，该队列就只有空和满两种情况，这就是二值。 

二值信号量通常用于互斥访问或任务同步， 与互斥信号量比较类似，但是二值信号量有可能会导致优先级翻转的问题 ，所以二值信号量更适合用于同步。

<img src=".\img\image-20250218000404579.png" alt="image-20250218000404579" style="zoom:50%;" />

### API函数

使用二值信号量的过程：创建二值信号量 -> 释放二值信号量 -> 获取二值信号量。

|              函数              |            描述            |
| :----------------------------: | :------------------------: |
|    xSemaphoreCreateBinary()    | 使用动态方式创建二值信号量 |
| xSemaphoreCreateBinaryStatic() | 使用静态方式创建二值信号量 |
|        xSemaphoreGive()        |         释放信号量         |
|        xSemaphoreTake()        |         获取信号量         |
|    xSemaphoreGiveFromISR()     |     在中断中释放信号量     |
|    xSemaphoreTakeFromISR()     |     在中断中获取信号量     |

**xSemaphoreCreateBinary**

```c
/*
* 返回值：
- NULL 创建失败
- 其他值 创建成功返回二值信号量的句柄
*/
#define xSemaphoreCreateBinary() xQueueGenericCreate(1,semSEMAPHORE_QUEUE_ITEM_LENGTH,queueQUEUE_TYPE_BINARY_SEMAPHORE)
```

**xSemaphoreGive**

不支持阻塞。

```c
/*
* 形参：
- xSemaphore 要释放的信号量句柄
* 返回值：
- pdPASS 释放信号量成功
- errQUEUE_FULL 释放信号量失败
*/
#define xSemaphoreGive (xSemaphore) xQueueGenericSend((QueueHandle_t)( xSemaphore),NULL,semGIVE_BLOCK_TIME,queueSEND_TO_BACK)
```

**xSemaphoreTake**

支持阻塞。

```c
/*
* 形参：
- xSemaphore 要获取的信号量句柄
- xBlockTime 阻塞时间
* 返回值：
- pdTRUE 获取信号量成功
- pdFALSE 超时，获取信号量失败
*/
BaseType_t xSemaphoreTake( xSemaphore,xBlockTime);
```

## 计数型信号量

### 简介

计数型信号量相当于队列长度大于1 的队列，因此计数型信号量能够容纳多个资源，这在计数型信号量被创建的时候确定的。

适用场合

- 事件计数：当每次事件发生后，在事件处理函数中释放计数型信号量（计数值+1），其他任务会获取计数型信号量（计数值-1） ，这种场合一般在创建时将初始计数值设置为 0 
- 资源管理：信号量表示有效的资源数目。任务必须先获取信号量（信号量计数值-1 ）才能获取资源控制权。当计数值减为零时表示没有的资源。当任务使用完资源后，必须释放信号量（信号量计数值+1）。信号量创建时计数值应等于最大资源数目

### API函数

使用计数型信号量的过程：创建计数型信号量 -> 释放信号量 -> 获取信号量。

计数型信号量的释放和获取与二值信号量相同 。

|               函数               |             描述             |
| :------------------------------: | :--------------------------: |
|    xSemaphoreCreateCounting()    | 使用动态方法创建计数型信号量 |
| xSemaphoreCreateCountingStatic() | 使用静态方法创建计数型信号量 |
|      uxSemaphoreGetCount()       |      获取信号量的计数值      |

**xSemaphoreCreateCounting**

此函数用于创建一个计数型信号量 。

```c
/*
* 形参：
- uxMaxCount 计数值的最大值限定
- uxInitialCount 计数值的初始值
* 返回值：
- NULL 创建失败
- 其他值 创建成功返回计数型信号量的句柄
*/
#define xSemaphoreCreateCounting(uxMaxCount,  uxInitialCount) xQueueCreateCountingSemaphore((uxMaxCount),(uxInitialCount)) 
```

**uxSemaphoreGetCount**

此函数用于获取信号量当前计数值大小。

```c
/*
* 形参：
- xSemaphore 信号量句柄
* 返回值：
- 整数 当前信号量的计数值大小
*/
#define uxSemaphoreGetCount(xSemaphore) uxQueueMessagesWaiting((QueueHandle_t)(xSemaphore))
```

## 优先级翻转

优先级翻转：高优先级的任务反而慢执行，低优先级的任务反而优先执行。

优先级翻转在抢占式内核中是非常常见的，但是在实时操作系统中是不允许出现优先级翻转的，因为优先级翻转会破坏任务的预期顺序，可能会导致未知的严重后果。

在使用二值信号量的时候，经常会遇到优先级翻转的问题。

例如

<img src=".\img\image-20250218002624744.png" alt="image-20250218002624744" style="zoom:40%;" />

高优先级任务被低优先级任务阻塞，导致高优先级任务迟迟得不到调度。但其他中等优先级的任务却能抢到CPU资源。从现象上看，就像是中优先级的任务比高优先级任务具有更高的优先权（即优先级翻转）。

## 互斥信号量

### 简介

互斥信号量其实就是一个拥有优先级继承的二值信号量，在同步的应用中二值信号量最适合。互斥信号量适合用于那些需要互斥访问的应用中。

优先级继承：当一个互斥信号量正在被一个低优先级的任务持有时， 如果此时有个高优先级的任务也尝试获取这个互斥信号量，那么这个高优先级的任务就会被阻塞。不过这个高优先级的任务会将低优先级任务的优先级提升到与自己相同的优先级。

例如

<img src=".\img\image-20250218003059198.png" alt="image-20250218003059198" style="zoom:40%;" />

此时任务H的阻塞时间仅仅是任务L 的执行时间，将优先级翻转的危害降到了最低。

**补充**

优先级继承并不能完全的消除优先级翻转的问题，它只是尽可能的降低优先级翻转带来的影响。

互斥信号量不能用于中断服务函数中，原因如下

- 互斥信号量有任务优先级继承的机制， 但是中断不是任务，没有任务优先级， 所以互斥信号量只能用与任务中，不能用于中断服务函数
- 中断服务函数中不能因为要等待互斥信号量而设置阻塞时间进入阻塞态

### API函数

使用互斥信号量：首先将宏configUSE_MUTEXES置1。

使用流程：创建互斥信号量 -> （take）获取信号量 ->（give）释放信号量。

互斥信号量的释放和获取函数与二值信号量相同，只不过互斥信号量不支持中断中调用。

**创建互斥信号量函数**

|             函数              |            描述            |
| :---------------------------: | :------------------------: |
|    xSemaphoreCreateMutex()    | 使用动态方法创建互斥信号量 |
| xSemaphoreCreateMutexStatic() | 使用静态方法创建互斥信号量 |

注

1. 创建互斥信号量时，会主动释放一次信号量

**xSemaphoreCreateMutex**

```c
/*
* 返回值：
- NULL 创建失败
- 其他值 创建成功返回计数型信号量的句柄
*/
#define xSemaphoreCreateMutex() xQueueCreateMutex(queueQUEUE_TYPE_MUTEX)
```

## 实例

### 二值信号量

<img src=".\img\image-20250218001249890.png" alt="image-20250218001249890" style="zoom:40%;" />

```c
/* freertos_demo.c */

/*FreeRTOS配置*/

/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define START_TASK_PRIO         1
#define START_TASK_STACK_SIZE   128
TaskHandle_t    start_task_handler;
void start_task( void * pvParameters );

/* TASK1 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK1_PRIO         2
#define TASK1_STACK_SIZE   128
TaskHandle_t    task1_handler;
void task1( void * pvParameters );

/* TASK2 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK2_PRIO         3
#define TASK2_STACK_SIZE   128
TaskHandle_t    task2_handler;
void task2( void * pvParameters );

QueueHandle_t semphore_handle;
/**
 * @brief       FreeRTOS例程入口函数
 * @param       无
 * @retval      无
 */
void freertos_demo(void)
{    
    semphore_handle = xSemaphoreCreateBinary();
    if(semphore_handle != NULL)
    {
        printf("二值信号量创建成功！！！\r\n");
    }
    xTaskCreate((TaskFunction_t         )   start_task,
                (char *                 )   "start_task",
                (configSTACK_DEPTH_TYPE )   START_TASK_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   START_TASK_PRIO,
                (TaskHandle_t *         )   &start_task_handler );
    vTaskStartScheduler();
}

void start_task( void * pvParameters )
{
    taskENTER_CRITICAL();               /* 进入临界区 */
    xTaskCreate((TaskFunction_t         )   task1,
                (char *                 )   "task1",
                (configSTACK_DEPTH_TYPE )   TASK1_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK1_PRIO,
                (TaskHandle_t *         )   &task1_handler );
                
    xTaskCreate((TaskFunction_t         )   task2,
                (char *                 )   "task2",
                (configSTACK_DEPTH_TYPE )   TASK2_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK2_PRIO,
                (TaskHandle_t *         )   &task2_handler );
                             
    vTaskDelete(NULL);
    taskEXIT_CRITICAL();                /* 退出临界区 */
}

/* 任务一，释放二值信号量 */
void task1( void * pvParameters )
{
    uint8_t key = 0;
    BaseType_t err;
    while(1) 
    {
        key = key_scan(0);
        if(key == KEY0_PRES)
        {
            if(semphore_handle != NULL)
            {
                err = xSemaphoreGive(semphore_handle);
                if(err == pdPASS)
                {
                    printf("信号量释放成功！！\r\n");
                }else printf("信号量释放失败！！\r\n");
            }       
        }
        vTaskDelay(10);
    }
}

/* 任务二，获取二值信号量 */
void task2( void * pvParameters )
{
    uint32_t i = 0;
    BaseType_t err;
    while(1)
    {
        err = xSemaphoreTake(semphore_handle,portMAX_DELAY); /* 获取信号量并死等 */
        if(err == pdTRUE)
        {
            printf("获取信号量成功\r\n");
        }else printf("已超时%d\r\n",++i);
    }
}
```

### 计数型信号量

<img src=".\img\image-20250218002150842.png" alt="image-20250218002150842" style="zoom:40%;" />

```c
/* freertos_demo.c */

/*FreeRTOS配置*/

/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define START_TASK_PRIO         1
#define START_TASK_STACK_SIZE   128
TaskHandle_t    start_task_handler;
void start_task( void * pvParameters );

/* TASK1 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK1_PRIO         2
#define TASK1_STACK_SIZE   128
TaskHandle_t    task1_handler;
void task1( void * pvParameters );

/* TASK2 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK2_PRIO         3
#define TASK2_STACK_SIZE   128
TaskHandle_t    task2_handler;
void task2( void * pvParameters );

QueueHandle_t count_semphore_handle;
/**
 * @brief       FreeRTOS例程入口函数
 * @param       无
 * @retval      无
 */
void freertos_demo(void)
{    
    count_semphore_handle = xSemaphoreCreateCounting(100 , 0);  /* 创建计数型信号量 */
    if(count_semphore_handle != NULL)
    {
        printf("计数型信号量创建成功！！！\r\n");
    }
    xTaskCreate((TaskFunction_t         )   start_task,
                (char *                 )   "start_task",
                (configSTACK_DEPTH_TYPE )   START_TASK_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   START_TASK_PRIO,
                (TaskHandle_t *         )   &start_task_handler );
    vTaskStartScheduler();
}

void start_task( void * pvParameters )
{
    taskENTER_CRITICAL();               /* 进入临界区 */
    xTaskCreate((TaskFunction_t         )   task1,
                (char *                 )   "task1",
                (configSTACK_DEPTH_TYPE )   TASK1_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK1_PRIO,
                (TaskHandle_t *         )   &task1_handler );
                
    xTaskCreate((TaskFunction_t         )   task2,
                (char *                 )   "task2",
                (configSTACK_DEPTH_TYPE )   TASK2_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK2_PRIO,
                (TaskHandle_t *         )   &task2_handler );
                             
    vTaskDelete(NULL);
    taskEXIT_CRITICAL();                /* 退出临界区 */
}

/* 任务一，释放计数型信号量 */
void task1( void * pvParameters )
{
    uint8_t key = 0;
    while(1) 
    {
        key = key_scan(0);
        if(key == KEY0_PRES)
        {
            if(count_semphore_handle != NULL)
            {
                xSemaphoreGive(count_semphore_handle);      /* 释放信号量 */
            }
        }
        vTaskDelay(10);
    }
}

/* 任务二，获取计数型信号量 */
void task2( void * pvParameters )
{
    BaseType_t err = 0;
    while(1)
    {
        err = xSemaphoreTake(count_semphore_handle,portMAX_DELAY); /* 获取信号量并死等 */
        if(err == pdTRUE)
        {
            printf("信号量的计数值为：%d\r\n",(int)uxSemaphoreGetCount(count_semphore_handle));
        }
        vTaskDelay(1000);
    }
}
```

### 优先级翻转

<img src=".\img\image-20250218002722047.png" alt="image-20250218002722047" style="zoom:40%;" />

```c
/* freertos_demo.c */

/*FreeRTOS配置*/

/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define START_TASK_PRIO         1
#define START_TASK_STACK_SIZE   128
TaskHandle_t    start_task_handler;
void start_task( void * pvParameters );

/* TASK1 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK1_PRIO         2
#define TASK1_STACK_SIZE   128
TaskHandle_t    low_task_handler;
void low_task( void * pvParameters );


/* TASK2 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK2_PRIO         3
#define TASK2_STACK_SIZE   128
TaskHandle_t    middle_task_handler;
void middle_task( void * pvParameters );


/* TASK3 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK3_PRIO         4
#define TASK3_STACK_SIZE   128
TaskHandle_t    high_task_handler;
void high_task( void * pvParameters );

QueueHandle_t semphore_handle;
/**
 * @brief       FreeRTOS例程入口函数
 * @param       无
 * @retval      无
 */
void freertos_demo(void)
{    
    semphore_handle = xSemaphoreCreateBinary();
    if(semphore_handle != NULL)
    {
        printf("二值信号量创建成功！！！\r\n");
    }
    xSemaphoreGive(semphore_handle);        /* 释放一次信号量 */
    
    xTaskCreate((TaskFunction_t         )   start_task,
                (char *                 )   "start_task",
                (configSTACK_DEPTH_TYPE )   START_TASK_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   START_TASK_PRIO,
                (TaskHandle_t *         )   &start_task_handler );
    vTaskStartScheduler();
}

void start_task( void * pvParameters )
{
    taskENTER_CRITICAL();               /* 进入临界区 */
    xTaskCreate((TaskFunction_t         )   low_task,
                (char *                 )   "low_task",
                (configSTACK_DEPTH_TYPE )   TASK1_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK1_PRIO,
                (TaskHandle_t *         )   &low_task_handler );
                
    xTaskCreate((TaskFunction_t         )   middle_task,
                (char *                 )   "middle_task",
                (configSTACK_DEPTH_TYPE )   TASK2_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK2_PRIO,
                (TaskHandle_t *         )   &middle_task_handler );
                
    xTaskCreate((TaskFunction_t         )   high_task,
                (char *                 )   "high_task",
                (configSTACK_DEPTH_TYPE )   TASK3_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK3_PRIO,
                (TaskHandle_t *         )   &high_task_handler );
                             
    vTaskDelete(NULL);
    taskEXIT_CRITICAL();                /* 退出临界区 */
}

/* 任务一，低优先级任务 */
void low_task( void * pvParameters )
{
    while(1) 
    {
        printf("low_task获取信号量\r\n");
        xSemaphoreTake(semphore_handle,portMAX_DELAY);
        printf("low_task正在运行！！！\r\n");
        delay_ms(3000);
        printf("low_task释放信号量\r\n");
        xSemaphoreGive(semphore_handle); 
        vTaskDelay(1000);
    }
}

/* 任务二，中优先级任务 */
void middle_task( void * pvParameters )
{
    while(1)
    {
        printf("middle_task正在运行！！！\r\n");
        vTaskDelay(1000);
    }
}

/* 任务三，高优先级任务 */
void high_task( void * pvParameters )
{
    while(1)
    {
        printf("high_task获取信号量\r\n");
        xSemaphoreTake(semphore_handle,portMAX_DELAY);
        printf("high_task正在运行！！！\r\n");
        delay_ms(1000);
        printf("high_task释放信号量\r\n");
        xSemaphoreGive(semphore_handle); 
        vTaskDelay(1000);
    }
}
```

### 互斥信号量

<img src=".\img\image-20250218003818164.png" alt="image-20250218003818164" style="zoom:40%;" />

```c
/* freertos_demo.c */

/*FreeRTOS配置*/

/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define START_TASK_PRIO         1
#define START_TASK_STACK_SIZE   128
TaskHandle_t    start_task_handler;
void start_task( void * pvParameters );

/* TASK1 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK1_PRIO         2
#define TASK1_STACK_SIZE   128
TaskHandle_t    low_task_handler;
void low_task( void * pvParameters );

/* TASK2 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK2_PRIO         3
#define TASK2_STACK_SIZE   128
TaskHandle_t    middle_task_handler;
void middle_task( void * pvParameters );


/* TASK3 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK3_PRIO         4
#define TASK3_STACK_SIZE   128
TaskHandle_t    high_task_handler;
void high_task( void * pvParameters );

QueueHandle_t mutex_semphore_handle;
/**
 * @brief       FreeRTOS例程入口函数
 * @param       无
 * @retval      无
 */
void freertos_demo(void)
{    
    mutex_semphore_handle = xSemaphoreCreateMutex();    /* 创建互斥信号量，并且主动释放一次信号量 */
    if(mutex_semphore_handle != NULL)
    {
        printf("互斥信号量创建成功！！！\r\n");
    }
    
    xTaskCreate((TaskFunction_t         )   start_task,
                (char *                 )   "start_task",
                (configSTACK_DEPTH_TYPE )   START_TASK_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   START_TASK_PRIO,
                (TaskHandle_t *         )   &start_task_handler );
    vTaskStartScheduler();
}

void start_task( void * pvParameters )
{
    taskENTER_CRITICAL();               /* 进入临界区 */
    xTaskCreate((TaskFunction_t         )   low_task,
                (char *                 )   "low_task",
                (configSTACK_DEPTH_TYPE )   TASK1_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK1_PRIO,
                (TaskHandle_t *         )   &low_task_handler );
                
    xTaskCreate((TaskFunction_t         )   middle_task,
                (char *                 )   "middle_task",
                (configSTACK_DEPTH_TYPE )   TASK2_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK2_PRIO,
                (TaskHandle_t *         )   &middle_task_handler );
                
    xTaskCreate((TaskFunction_t         )   high_task,
                (char *                 )   "high_task",
                (configSTACK_DEPTH_TYPE )   TASK3_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK3_PRIO,
                (TaskHandle_t *         )   &high_task_handler );
                             
    vTaskDelete(NULL);
    taskEXIT_CRITICAL();                /* 退出临界区 */
}

/* 任务一，低优先级任务 */
void low_task( void * pvParameters )
{
    while(1) 
    {
        printf("low_task获取信号量\r\n");
        xSemaphoreTake(mutex_semphore_handle,portMAX_DELAY);
        printf("low_task正在运行！！！\r\n");
        delay_ms(3000);
        printf("low_task释放信号量\r\n");
        xSemaphoreGive(mutex_semphore_handle); 
        vTaskDelay(1000);
    }
}

/* 任务二，中优先级任务 */
void middle_task( void * pvParameters )
{
    while(1)
    {
        printf("middle_task正在运行！！！\r\n");
        vTaskDelay(1000);
    }
}

/* 任务三，高优先级任务 */
void high_task( void * pvParameters )
{
    while(1)
    {
        printf("high_task获取信号量\r\n");
        xSemaphoreTake(mutex_semphore_handle,portMAX_DELAY);
        printf("high_task正在运行！！！\r\n");
        delay_ms(1000);
        printf("high_task释放信号量\r\n");
        xSemaphoreGive(mutex_semphore_handle); 
        vTaskDelay(1000);
    }
}
```

# 队列集

## 简介

一个队列只允许任务间传递的消息为同一种数据类型，如果需要在任务间传递不同数据类型的消息时，那么就可以使用队列集 。

作用：用于对多个队列或信号量进行“监听”，其中不管哪一个消息到来，都可让任务退出阻塞状态。

<img src=".\img\image-20250218115454066.png" alt="image-20250218115454066" style="zoom:40%;" />

**使用流程**

1. 启用队列集功能需要将宏 configUSE_QUEUE_SETS 配置为 1
2. 创建队列集
3. 创建队列或信号量
4. 往队列集中添加队列或信号量
5. 往队列发送信息或释放信号量
6. 获取队列集的消息

## API函数

|             函数             |                 描述                 |
| :--------------------------: | :----------------------------------: |
|      xQueueCreateSet()       |              创建队列集              |
|       xQueueAddToSet()       |          队列添加到队列集中          |
|    xQueueRemoveFromSet()     |          从队列集中移除队列          |
|    xQueueSelectFromSet()     |     获取队列集中有有效消息的队列     |
| xQueueSelectFromSetFromISR() | 在中断中获取队列集中有有效消息的队列 |

### xQueueCreateSet

此函数用于创建队列集。

```c
/*
* 形参：
- uxEventQueueLength 队列集可容纳的队列数量
* 返回值：
- NULL 队列集创建失败
- 其他值 队列集创建成功，返回队列集句柄
*/
QueueSetHandle_t xQueueCreateSet(const UBaseType_t uxEventQueueLength); 
```

### xQueueAddToSet

此函数用于往队列集中添加队列，要注意的时，队列在被添加到队列集之前，队列中不能有有效的消息。

```c
/*
* 形参：
- xQueueOrSemaphore 待添加的队列句柄
- xQueueSet 队列集
* 返回值：
- pdPASS 队列集添加队列成功
- pdFAIL 队列集添加队列失败
*/
BaseType_t xQueueAddToSet( QueueSetMemberHandle_t QueueOrSemaphore ,
					   	   QueueSetHandle_t xQueueSet); 
```

### xQueueRemoveFromSet

此函数用于从队列集中移除队列， 要注意的是，队列在从队列集移除之前，必须没有有效的消息。

```c
/*
* 形参：
- xQueueOrSemaphore 待添加的队列句柄
- xQueueSet 队列集
* 返回值：
- pdPASS 队列集移除队列成功
- pdFAIL 队列集移除队列失败
*/
BaseType_t xQueueRemoveFromSet(QueueSetMemberHandle_t xQueueOrSemaphore ,						           QueueSetHandle_t xQueueSet ); 
```

### xQueueSelectFromSet

此函数用于在任务中获取队列集中有有效消息的队列。

```c
/*
* 形参：
- xQueueSet 队列集
- xTicksToWait 阻塞超时时间
* 返回值：
- NULL 获取消息失败
- 其他值 获取到消息的队列句柄
*/
QueueSetMemberHandle_t xQueueSelectFromSet( QueueSetHandle_t xQueueSet,
     										TickType_t const xTicksToWait);
```

## 实例

<img src=".\img\image-20250218114815104.png" alt="image-20250218114815104" style="zoom:40%;" />

```c
/* freertos_demo.c */

/*FreeRTOS配置*/

/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define START_TASK_PRIO         1
#define START_TASK_STACK_SIZE   128
TaskHandle_t    start_task_handler;
void start_task( void * pvParameters );

/* TASK1 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK1_PRIO         2
#define TASK1_STACK_SIZE   128
TaskHandle_t    task1_handler;
void task1( void * pvParameters );

/* TASK2 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK2_PRIO         3
#define TASK2_STACK_SIZE   128
TaskHandle_t    task2_handler;
void task2( void * pvParameters );

QueueSetHandle_t queueset_handle;
QueueHandle_t    queue_handle;
QueueHandle_t    semphr_handle;
/**
 * @brief       FreeRTOS例程入口函数
 * @param       无
 * @retval      无
 */
void freertos_demo(void)
{    
    xTaskCreate((TaskFunction_t         )   start_task,
                (char *                 )   "start_task",
                (configSTACK_DEPTH_TYPE )   START_TASK_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   START_TASK_PRIO,
                (TaskHandle_t *         )   &start_task_handler );
    vTaskStartScheduler();
}

void start_task( void * pvParameters )
{
    taskENTER_CRITICAL();                               /* 进入临界区 */
    queueset_handle = xQueueCreateSet( 2 );             /* 创建队列集，可以存放2个队列 */
    if(queueset_handle != NULL)
    {
        printf("队列集创建成功！！\r\n");
    }
    
    queue_handle = xQueueCreate( 1, sizeof(uint8_t) );  /* 创建队列 */ 
    semphr_handle = xSemaphoreCreateBinary();           /* 创建二值信号量 */
    
    xQueueAddToSet( queue_handle,queueset_handle);		// 添加到队列集
    xQueueAddToSet( semphr_handle,queueset_handle);
    
    xTaskCreate((TaskFunction_t         )   task1,
                (char *                 )   "task1",
                (configSTACK_DEPTH_TYPE )   TASK1_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK1_PRIO,
                (TaskHandle_t *         )   &task1_handler );
                
    xTaskCreate((TaskFunction_t         )   task2,
                (char *                 )   "task2",
                (configSTACK_DEPTH_TYPE )   TASK2_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK2_PRIO,
                (TaskHandle_t *         )   &task2_handler );
                             
    vTaskDelete(NULL);
    taskEXIT_CRITICAL();                /* 退出临界区 */
}

/* 任务一，实现队列发送以及信号量释放 */
void task1( void * pvParameters )
{
    uint8_t key = 0;
    BaseType_t err = 0;
    while(1) 
    {
        key = key_scan(0);
        if(key == KEY0_PRES)
        {
            err = xQueueSend( queue_handle, &key, portMAX_DELAY );
            if(err == pdPASS)
            {
                printf("往队列queue_handle写入数据成功！！\r\n");
            }
        }else if(key == KEY1_PRES)
        {
            err = xSemaphoreGive(semphr_handle);
            if(err == pdPASS)
            {
                printf("释放信号量成功！！\r\n");
            }
        }
        vTaskDelay(10);
    }
}

/* 任务二，获取队列集的消息 */
void task2( void * pvParameters )
{
    QueueSetMemberHandle_t member_handle;
    uint8_t key;
    while(1)
    {
        member_handle = xQueueSelectFromSet( queueset_handle,portMAX_DELAY);
        if(member_handle == queue_handle)
        {
            xQueueReceive( member_handle,&key,portMAX_DELAY);
            printf("获取到的队列数据为：%d\r\n",key);
        }else if(member_handle == semphr_handle)
        {
            xSemaphoreTake( member_handle, portMAX_DELAY );
            printf("获取信号量成功！！\r\n");
        }
    }
}
```

# 事件标志组

## 简介

事件标志位是用一个位，来表示事件是否发生。

事件标志组是一组事件标志位的集合， 可以简单的理解事件标志组，就是一个整数。

**特点**

- 它的每一个位表示一个事件（高8位不算）
- 每一位事件的含义，由用户自己决定
- 任意任务或中断都可以读写这些位
- 可以等待某一位成立，或者等待多位同时成立

**结构**

一个事件组就包含了一个 EventBits_t 数据类型的变量，变量类型 EventBits_t 的定义如下

```c
typedef TickType_t EventBits_t;
#if(configUSE_16_BIT_TICKS == 1 )
	typedef uint16_t TickType_t;
#else
	typedef uint32_t TickType_t;
#endif
#define configUSE_16_BIT_TICKS 0 
```

EventBits_t 实际上是一个 16 位或 32 位无符号的数据类型 。

虽然使用了 32 位无符号的数据类型变量来存储事件标志， 但其中的高8位用作存储事件标志组的控制信息，低24位用作存储事件标志 ，所以说一个事件组最多可以存储 24 个事件标志。

<img src=".\img\image-20250218133318576.png" alt="image-20250218133318576" style="zoom:60%;" />

 **事件标志组与队列、信号量的区别**

|     功能     |                           唤醒对象                           |                           事件清除                           |
| :----------: | :----------------------------------------------------------: | :----------------------------------------------------------: |
| 队列、信号量 |                 事件发生时，只会唤醒一个任务                 | 是消耗型的资源，队列的数据被读走就没了；信号量被获取后就减少了 |
|  事件标志组  | 事件发生时，会唤醒所有符合条件的任务，可以理解为“广播”的作用 |  被唤醒的任务有两个选择，可以让事件保留不动，也可以清除事件  |

## API函数

|             函数              |               描述               |
| :---------------------------: | :------------------------------: |
|      xEventGroupCreate()      |    使用动态方式创建事件标志组    |
|   xEventGroupCreateStatic()   |    使用静态方式创建事件标志组    |
|    xEventGroupClearBits()     |          清零事件标志位          |
| xEventGroupClearBitsFromISR() |      在中断中清零事件标志位      |
|     xEventGroupSetBits()      |          设置事件标志位          |
|  xEventGroupSetBitsFromISR()  |      在中断中设置事件标志位      |
|     xEventGroupWaitBits()     |          等待事件标志位          |
|       xEventGroupSync()       | 设置事件标志位，并等待事件标志位 |

### xEventGroupCreate

动态方式创建事件标志组。

```c
/*
* 返回值：
- NULL 事件标志组创建失败
- 其他值 事件标志组创建成功，返回其句柄
*/
EventGroupHandle_t    xEventGroupCreate ( void ); 
```

### xEventGroupClearBits

清除事件标志位。

```c
/*
* 形参:
- xEventGroup 待操作的事件标志组句柄
- uxBitsToSet 待清零的事件标志位
* 返回值：
- 整数 清零事件标志位之前事件组中事件标志位的值
*/
EventBits_t  xEventGroupClearBits( EventGroupHandle_t xEventGroup,	
                                   const EventBits_t uxBitsToClear)
```

### xEventGroupSetBits

设置事件标志位。

```c
/*
* 形参:
- xEventGroup 待操作的事件标志组句柄
- uxBitsToSet 待设置的事件标志位
* 返回值：
- 整数 函数返回事件组中的事件标志位值
*/
EventBits_t   xEventGroupSetBits(  EventGroupHandle_t 	xEventGroup,
                                   const EventBits_t 	uxBitsToSet); 
```

### xEventGroupWaitBits

等待事件标志位。

- 可以等待某一位、也可以等待多位
- 等到期望的事件后，还可以清除某些位

```c
/*
* 形参:
- xEvenrGroup 等待的事件标志组句柄
- uxBitsToWaitFor 等待的事件标志位，可以用逻辑或等待多个事件标志位
- xClearOnExit 成功等待到事件标志位后，清除事件组中对应的事件标志位，pdTRUE：清除uxBitsToWaitFor指定位；pdFALSE：不清除
- xWaitForAllBits 等待 uxBitsToWaitFor 中的所有事件标志位（逻辑与），pdTRUE：等待的位，全部为1；pdFALSE：等待的位，某个为1
- xTicksToWait 等待的阻塞时间
* 返回值：
- 等待的事件标志位值 等待事件标志位成功，返回等待到的事件标志位
- 其他值  等待事件标志位失败，返回事件组中的事件标志位
*/
EventBits_t xEventGroupWaitBits(EventGroupHandle_t 	xEventGroup, 
                                const EventBits_t 	uxBitsToWaitFor,
                                const BaseType_t 	xClearOnExit,
                                const BaseType_t 	xWaitForAllBits,
                                TickType_t 			xTicksToWait);
```

### xEventGroupSync

同步函数。

```c
/*
* 形参:
- xEventGroup 等待事件标志所在事件组
- uxBitsToSet 达到同步点后，要设置的事件标志
- uxBitsToWaitFor 等待的事件标志
- xTicksToWait 等待的阻塞时间
* 返回值：
- 等待的事件标志位值 等待事件标志位成功，返回等待到的事件标志位
- 其他值  等待事件标志位失败，返回事件组中的事件标志位
*/
EventBits_t    xEventGroupSync( EventGroupHandle_t 	xEventGroup,
                               	const EventBits_t 	uxBitsToSet,
                              	const EventBits_t 	uxBitsToWaitFor,
                               	TickType_t 			xTicksToWait);
```

## 实例

<img src=".\img\image-20250218135446587.png" alt="image-20250218135446587" style="zoom:40%;" />

```c
/* freertos_demo.c */

/*FreeRTOS配置*/

/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define START_TASK_PRIO         1
#define START_TASK_STACK_SIZE   128
TaskHandle_t    start_task_handler;
void start_task( void * pvParameters );

/* TASK1 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK1_PRIO         2
#define TASK1_STACK_SIZE   128
TaskHandle_t    task1_handler;
void task1( void * pvParameters );

/* TASK2 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK2_PRIO         3
#define TASK2_STACK_SIZE   128
TaskHandle_t    task2_handler;
void task2( void * pvParameters );

EventGroupHandle_t  eventgroup_handle;
#define EVENTBIT_0  (1 << 0)
#define EVENTBIT_1  (1 << 1)

/**
 * @brief       FreeRTOS例程入口函数
 * @param       无
 * @retval      无
 */
void freertos_demo(void)
{    
    xTaskCreate((TaskFunction_t         )   start_task,
                (char *                 )   "start_task",
                (configSTACK_DEPTH_TYPE )   START_TASK_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   START_TASK_PRIO,
                (TaskHandle_t *         )   &start_task_handler );
    vTaskStartScheduler();
}

void start_task( void * pvParameters )
{
    taskENTER_CRITICAL();               /* 进入临界区 */
    eventgroup_handle = xEventGroupCreate();
    if(eventgroup_handle != NULL)
    {
        printf("事件标志组创建成功！！\r\n");
    }
    
    xTaskCreate((TaskFunction_t         )   task1,
                (char *                 )   "task1",
                (configSTACK_DEPTH_TYPE )   TASK1_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK1_PRIO,
                (TaskHandle_t *         )   &task1_handler );
                
    xTaskCreate((TaskFunction_t         )   task2,
                (char *                 )   "task2",
                (configSTACK_DEPTH_TYPE )   TASK2_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK2_PRIO,
                (TaskHandle_t *         )   &task2_handler );
                             
    vTaskDelete(NULL);
    taskEXIT_CRITICAL();                /* 退出临界区 */
}

/* 任务一，设置事件标志位 */
void task1( void * pvParameters )
{
    uint8_t key = 0;
    while(1) 
    {
        key = key_scan(0);
        if(key == KEY0_PRES)
        {
            xEventGroupSetBits( eventgroup_handle, EVENTBIT_0); /* 将事件标志组的bit0位置1 */
        }else if(key == KEY1_PRES)
        {
            xEventGroupSetBits( eventgroup_handle, EVENTBIT_1); /* 将事件标志组的bit1位置1 */
        }
        vTaskDelay(10);
    }
}

/* 任务二，获取事件标志位 */
void task2( void * pvParameters )
{
    EventBits_t event_bit = 0;
    while(1)
    {
        event_bit = xEventGroupWaitBits( eventgroup_handle,         /* 事件标志组句柄 */
                                         EVENTBIT_0 | EVENTBIT_1,   /* 等待事件标志组的bit0和bit1位 */
                                         pdTRUE,                    /* 成功等待到事件标志位后，清除事件标志组中的bit0和bit1位 */
                                         pdTRUE,                    /* 等待事件标志组的bit0和bit1位都置1,就成立 */
                                         portMAX_DELAY );           /* 死等 */
        printf("等待到的事件标志位值为：%#x\r\n",event_bit);
    }
}
```

# 任务通知

## 简介

任务通知是用来通知任务的，任务控制块中的结构体成员变量 ulNotifiedValue 就是这个通知值，常用于任务之间一对一的通信。

- 使用队列、信号量、事件标志组时都需另外创建一个结构体，通过中间的结构体进行间接通信

<img src=".\img\image-20250218152126947.png" alt="image-20250218152126947" style="zoom:70%;" />

- 使用任务通知时，任务结构体TCB中就包含了内部对象，可以直接接收别人发过来的"通知“

<img src=".\img\image-20250218152142197.png" alt="image-20250218152142197" style="zoom:70%;" />

**任务通知值的更新方式**

- 不覆盖接受任务的通知值
- 覆盖接受任务的通知值
- 更新接受任务通知值的一个或多个bit
- 增加接受任务的通知值

只要合理，灵活的利用任务通知的特点，可以在一些场合中替代队列、信号量、事件标志组。

**任务通知的优势及劣势**

优势

- 效率更高：使用任务通知向任务发送事件或数据比使用队列、事件标志组或信号量快得多
- 使用内存更小：使用其他方法时都要先创建对应的结构体，使用任务通知时无需额外创建结构体

劣势

- 无法发送数据给ISR：ISR没有任务结构体，所以无法给ISR发送数据。但是ISR可以使用任务通知的功能，发数据给任务
- 无法广播给多个任务：任务通知只能是被指定的一个任务接收并处理 
- 无法缓存多个数据：任务通知是通过更新任务通知值来发送数据的，任务结构体中只有一个任务通知值，只能保持一个数据
- 发送受阻不支持阻塞：发送方无法进入阻塞状态等待

## 结构

任务都有一个结构体：任务控制块TCB，它里边有两个结构体成员变量

```c
typedef struct tskTaskControlBlock 
{
	… …
    #if ( configUSE_TASK_NOTIFICATIONS  ==  1 )
    volatile uint32_t ulNotifiedValue [configTASK_NOTIFICATION_ARRAY_ENTRIES ];
    volatile uint8_t  ucNotifyState[configTASK_NOTIFICATION_ARRAY_ENTRIES ];
    #endif
    … …
} tskTCB;
#define configTASK_NOTIFICATION_ARRAY_ENTRIES 1  /* 定义任务通知数组的大小, 默认: 1 */
```

- 一个是 uint32_t 类型，用来表示通知值
- 一个是 uint8_t 类型，用来表示通知状态

**任务通知值**

任务通知值的更新方式有多种类型

- 计数值（数值累加，类似信号量）
- 相应位置一（类似事件标志组）
- 任意数值（支持覆写和不覆写，类似队列）

**任务通知状态**

任务通知状态共有3种取值

```c
#define	taskNOT_WAITING_NOTIFICATION  ((uint8_t)0)		 /* 任务未等待通知 */
#define taskWAITING_NOTIFICATION	  ((uint8_t)1)		 /* 任务在等待通知 */
#define taskNOTIFICATION_RECEIVED     ((uint8_t )2)		 /* 任务在等待接收 */
```

- 任务未等待通知 ：任务通知默认的初始化状态
- 等待通知：接收方已经准备好了（调用了接收任务通知函数），等待发送方给个通知
- 等待接收：发送方已经发送出去（调用了发送任务通知函数），等待接收方接收

## API函数

任务通知API函数主要有两类：发送通知和接收通知。

发送通知API函数可以用于任务和中断服务函数中，接收通知API函数只能用在任务中。

### 发送通知

本质都是调用xTaskGenericNotify函数。

|             函数             |                      描述                      |
| :--------------------------: | :--------------------------------------------: |
|        xTaskNotify()         |              发送通知，带有通知值              |
|    xTaskNotifyAndQuery()     | 发送通知，带有通知值并且保留接收任务的原通知值 |
|      xTaskNotifyGive()       |              发送通知，不带通知值              |
|     xTaskNotifyFromISR()     |           在中断中发送任务通知 下同            |
| xTaskNotifyAndQueryFromISR() |                                                |
|   xTaskNotifyGiveFromISR()   |                                                |

**xTaskGenericNotify**

```c
/*
* 形参:
- xTaskToNotify 接收任务通知的任务句柄
- uxIndexToNotify 任务的指定通知（任务通知相关数组成员）0
- ulValue 任务通知值
- eAction 通知方式（通知值更新方式）
- pulPreviousNotificationValue 用于保存更新前的任务通知值（为NULL则不保存）
*/
BaseType_t xTaskGenericNotify( TaskHandle_t 	xTaskToNotify,
                               UBaseType_t 	uxIndexToNotify,
                               uint32_t 		ulValue,
                               eNotifyAction 	eAction,
                               uint32_t * 		pulPreviousNotificationValue);
```

任务通知方式共有以下几种

```c
typedef  enum
{    
	eNoAction = 0, 				/* 无操作 */
	eSetBits					/* 更新指定bit */
	eIncrement					/* 通知值加一 */
 	eSetValueWithOverwrite		/* 覆写的方式更新通知值 */
	eSetValueWithoutOverwrite	/* 不覆写通知值 */
} eNotifyAction;
```

### 接收通知

- 阻塞条件：任务通知值为 0，并且指定了非零超时时间或无限超时
- 解除阻塞条件：任务通知值增加（通过任务或中断发送通知），或者超时

|        函数        |                             描述                             |
| :----------------: | :----------------------------------------------------------: |
| ulTaskNotifyTake() | 获取任务通知，可以设置在退出此函数的时候将任务通知值清零或者减一。  当任务通知用作二值信号量或者计数信号量的时候，使用此函数来获取信号量 |
| xTaskNotifyWait()  | 获取任务通知，比 ulTaskNotifyTak()更为复杂，可获取通知值和清除通知值的指定位 |

- 当任务通知用作于信号量时，使用函数获取信号量：ulTaskNotifyTake()
- 当任务通知用作于事件标志组或队列时，使用此函数来获取：xTaskNotifyWait()

**ulTaskNotifyTake**

此函数用于接收任务通知值，可以设置在退出此函数的时候将任务通知值清零或者减一。

```c
/*
* 形参:
- uxIndexToWaitOn 任务的指定通知（任务通知相关数组成员） 0
- xClearCountOnExit 指定在成功接收通知后，将通知值清零或减 1，pdTRUE：把通知值清零；pdFALSE：把通知值减一
- xTicksToWait 阻塞等待任务通知值的最大时间
* 返回值：
- 0 接收失败
- 非0 接收成功，返回任务通知的通知值
*/
#define ulTaskNotifyTake(xClearCountOnExit,xTicksToWait)
ulTaskGenericNotifyTake((tskDEFAULT_INDEX_TO_NOTIFY),
                        (xClearCountOnExit ),
                        (xTicksToWait)) 
```

**xTaskNotifyWait**

此函数用于获取通知值和清除通知值的指定位值，适用于模拟队列和事件标志组，使用该函数来获取任务通知 。

```c
#define xTaskNotifyWait(ulBitsToClearOnEntry, 
						ulBitsToClearOnExit, pulNotificationValue,
						xTicksToWait) 

xTaskGenericNotifyWait(tskDEFAULT_INDEX_TO_NOTIFY,
                       (ulBitsToClearOnEntry ),
                       ( ulBitsToClearOnExit ),
                       ( pulNotificationValue ),
                       ( xTicksToWait )) 
```

```c
/*
* 形参:
- uxIndexToWaitOn 任务的指定通知（任务通知相关数组成员） 0
- ulBitesToClearOnEntry 等待前清零指定任务通知值的比特位（旧值对应bit清0）
- ulBitesToClearOnExit 成功等待后清零指定的任务通知值比特位（新值对应bit清0）
- pulNotificationValue 用来取出通知值（如果不需要取出，可设为NULL）
- xTicksToWait 阻塞等待任务通知值的最大时间
* 返回值：
- pdTRUE 等待任务通知成功
- pdFALSE 等待任务通知失败
*/
BaseType_t  xTaskGenericNotifyWait( UBaseType_t uxIndexToWaitOn,
                                    uint32_t 		ulBitsToClearOnEntry,
                                    uint32_t 		ulBitsToClearOnExit,
                                    uint32_t * 		pulNotificationValue,
                                    TickType_t 		xTicksToWait);
```

## 实例

<img src=".\img\image-20250218155705763.png" alt="image-20250218155705763" style="zoom:40%;" />

### 模拟信号量

```c
/* freertos_demo.c */

/*FreeRTOS配置*/

/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define START_TASK_PRIO         1
#define START_TASK_STACK_SIZE   128
TaskHandle_t    start_task_handler;
void start_task( void * pvParameters );

/* TASK1 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK1_PRIO         2
#define TASK1_STACK_SIZE   128
TaskHandle_t    task1_handler;
void task1( void * pvParameters );

/* TASK2 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK2_PRIO         3
#define TASK2_STACK_SIZE   128
TaskHandle_t    task2_handler;
void task2( void * pvParameters );

/**
 * @brief       FreeRTOS例程入口函数
 * @param       无
 * @retval      无
 */
void freertos_demo(void)
{    
    xTaskCreate((TaskFunction_t         )   start_task,
                (char *                 )   "start_task",
                (configSTACK_DEPTH_TYPE )   START_TASK_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   START_TASK_PRIO,
                (TaskHandle_t *         )   &start_task_handler );
    vTaskStartScheduler();
}

void start_task( void * pvParameters )
{
    taskENTER_CRITICAL();               /* 进入临界区 */
    
    xTaskCreate((TaskFunction_t         )   task1,
                (char *                 )   "task1",
                (configSTACK_DEPTH_TYPE )   TASK1_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK1_PRIO,
                (TaskHandle_t *         )   &task1_handler );
                
    xTaskCreate((TaskFunction_t         )   task2,
                (char *                 )   "task2",
                (configSTACK_DEPTH_TYPE )   TASK2_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK2_PRIO,
                (TaskHandle_t *         )   &task2_handler );
                             
    vTaskDelete(NULL);
    taskEXIT_CRITICAL();                /* 退出临界区 */
}

/* 任务一，发送任务通知值 */
void task1( void * pvParameters )
{
    uint8_t key = 0;
    
    while(1) 
    {
        key = key_scan(0);
        if(key == KEY0_PRES)
        {
            printf("任务通知模拟信号量释放！\r\n");
            xTaskNotifyGive(task2_handler);
        }
        vTaskDelay(10);
    }
}

/* 任务二，接收任务通知值 */
void task2( void * pvParameters )
{
    uint32_t rev = 0;
    while(1)
    {
        rev = ulTaskNotifyTake(pdTRUE , portMAX_DELAY);
        /*pdTRUE：二值型信号量 pdFALSE：计数型信号量*/
        if(rev != 0)
        {
            printf("rev: %d\r\n",rev);
        }
        vTaskDelay(1000);
    }
}
```

### 模拟消息邮箱

```c
/* freertos_demo.c */

/*FreeRTOS配置*/

/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define START_TASK_PRIO         1
#define START_TASK_STACK_SIZE   128
TaskHandle_t    start_task_handler;
void start_task( void * pvParameters );

/* TASK1 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK1_PRIO         2
#define TASK1_STACK_SIZE   128
TaskHandle_t    task1_handler;
void task1( void * pvParameters );

/* TASK2 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK2_PRIO         3
#define TASK2_STACK_SIZE   128
TaskHandle_t    task2_handler;
void task2( void * pvParameters );

/**
 * @brief       FreeRTOS例程入口函数
 * @param       无
 * @retval      无
 */
void freertos_demo(void)
{    
    xTaskCreate((TaskFunction_t         )   start_task,
                (char *                 )   "start_task",
                (configSTACK_DEPTH_TYPE )   START_TASK_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   START_TASK_PRIO,
                (TaskHandle_t *         )   &start_task_handler );
    vTaskStartScheduler();
}

void start_task( void * pvParameters )
{
    taskENTER_CRITICAL();               /* 进入临界区 */
    
    xTaskCreate((TaskFunction_t         )   task1,
                (char *                 )   "task1",
                (configSTACK_DEPTH_TYPE )   TASK1_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK1_PRIO,
                (TaskHandle_t *         )   &task1_handler );
                
    xTaskCreate((TaskFunction_t         )   task2,
                (char *                 )   "task2",
                (configSTACK_DEPTH_TYPE )   TASK2_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK2_PRIO,
                (TaskHandle_t *         )   &task2_handler );
                             
    vTaskDelete(NULL);
    taskEXIT_CRITICAL();                /* 退出临界区 */
}

/* 任务一，发送任务通知值 */
void task1( void * pvParameters )
{
    uint8_t key = 0;
    
    while(1) 
    {
        key = key_scan(0);
        if((key != 0) && (task2_handler != NULL))
        {
            printf("任务通知模拟消息邮箱发送，发送的键值为：%d\r\n",key);
            xTaskNotify( task2_handler, key, eSetValueWithOverwrite );
        }
        vTaskDelay(10);
    }
}

/* 任务二，接收任务通知值 */
void task2( void * pvParameters )
{
    uint32_t notify_val = 0;
    while(1)
    {
        xTaskNotifyWait( 0, 0xFFFFFFFF, &notify_val, portMAX_DELAY );
        switch(notify_val)
        {
            case KEY0_PRES:
            {
                printf("接收到的通知值为：%d\r\n",notify_val);
                LED0_TOGGLE();
                break;
            }
            case KEY1_PRES:
            {
                printf("接收到的通知值为：%d\r\n",notify_val);
                LED1_TOGGLE();
                break;
            }
            default : break;
        }
    }
}
```

### 模拟事件标志组

```c
/* freertos_demo.c */

/*FreeRTOS配置*/

/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define START_TASK_PRIO         1
#define START_TASK_STACK_SIZE   128
TaskHandle_t    start_task_handler;
void start_task( void * pvParameters );

/* TASK1 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK1_PRIO         2
#define TASK1_STACK_SIZE   128
TaskHandle_t    task1_handler;
void task1( void * pvParameters );

/* TASK2 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK2_PRIO         3
#define TASK2_STACK_SIZE   128
TaskHandle_t    task2_handler;
void task2( void * pvParameters );

#define EVENTBIT_0  (1 << 0)
#define EVENTBIT_1  (1 << 1)
/**
 * @brief       FreeRTOS例程入口函数
 * @param       无
 * @retval      无
 */
void freertos_demo(void)
{    
    xTaskCreate((TaskFunction_t         )   start_task,
                (char *                 )   "start_task",
                (configSTACK_DEPTH_TYPE )   START_TASK_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   START_TASK_PRIO,
                (TaskHandle_t *         )   &start_task_handler );
    vTaskStartScheduler();
}

void start_task( void * pvParameters )
{
    taskENTER_CRITICAL();               /* 进入临界区 */
    xTaskCreate((TaskFunction_t         )   task1,
                (char *                 )   "task1",
                (configSTACK_DEPTH_TYPE )   TASK1_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK1_PRIO,
                (TaskHandle_t *         )   &task1_handler );
                
    xTaskCreate((TaskFunction_t         )   task2,
                (char *                 )   "task2",
                (configSTACK_DEPTH_TYPE )   TASK2_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK2_PRIO,
                (TaskHandle_t *         )   &task2_handler );
                             
    vTaskDelete(NULL);
    taskEXIT_CRITICAL();                /* 退出临界区 */
}

/* 任务一，发送任务通知值*/
void task1( void * pvParameters )
{
    uint8_t key = 0;
    while(1) 
    {
        key = key_scan(0);
        if(key == KEY0_PRES)
        {
            printf("将bit0位置1\r\n");
            xTaskNotify( task2_handler, EVENTBIT_0, eSetBits );
        }else if(key == KEY1_PRES)
        {
            printf("将bit1位置1\r\n");
            xTaskNotify( task2_handler, EVENTBIT_1, eSetBits );
        }
        vTaskDelay(10);
    }
}

/* 任务二，接收任务通知值 */
void task2( void * pvParameters )
{
    uint32_t notify_val = 0,event_bit = 0;
    while(1)
    {
        xTaskNotifyWait( 0, 0xFFFFFFFF, &notify_val, portMAX_DELAY );
        if(notify_val & EVENTBIT_0)
        {
            event_bit |= EVENTBIT_0;
        }
        if(notify_val & EVENTBIT_1)
        {
            event_bit |= EVENTBIT_1;
        }
        if(event_bit == (EVENTBIT_0|EVENTBIT_1))
        {
            printf("任务通知模拟事件标志组接收成功！！\r\n");
            event_bit = 0;
        }
    }
}
```

# 软件定时器

## 简介

定时器是指从指定的时刻开始，经过一个指定时间，然后触发一个超时事件，用户可自定义定时器的周期。

- 硬件定时器：芯片本身自带的定时器模块，硬件定时器的精度一般很高，每次在定时时间到达之后就会自动触发一个中断，用户在中断服务函数中处理信息，还具有PWM等功能
- 软件定时器：是指具有定时功能的软件，可设置定时周期，当指定时间到达后要调用回调函数（也称超时函数），用户在回调函数中处理信息

**软件定时器优缺点**

优点

- 硬件定时器数量有限，而软件定时器理论上只需有足够内存，就可以创建多个
- 使用简单、成本低

缺点

- 软件定时器相对硬件定时器来说，精度没有那么高（因为它以系统时钟为基准，系统时钟中断优先级又是最低，容易被打断）。 对于需要高精度要求的场合，不建议使用软件定时器

## FreeRTOS软件定时器

**特点**

- 可裁剪：软件定时器是可裁剪可配置的功能， 如果要使能软件定时器，需将configUSE_TIMERS 配置项配置成 1 
- 单次和周期：软件定时器支持设置成单次定时器或周期定时器

注

1. 软件定时器的超时回调函数是由软件定时器服务任务调用的，软件定时器的超时回调函数本身不是任务，因此不能在该回调函数中使用可能会导致任务阻塞的 API 函数
2. 软件定时器服务任务：在调用函数 vTaskStartScheduler()开启任务调度器的时候，会创建一个用于管理软件定时器的任务，这个任务就叫做软件定时器服务任务

**软件定时器服务任务作用**

- 负责软件定时器超时的逻辑判断
- 调用超时软件定时器的超时回调函数
- 处理软件定时器命令队列

注

1. 当发送命令队列或某个定时器超时了，会唤醒软件定时器任务

**软件定时器的命令队列**

FreeRTOS 提供了许多软件定时器相关的 API 函数，这些 API 函数大多都是往定时器的队列中写入消息（发送命令），这个队列叫做软件定时器命令队列，是提供给 FreeRTOS 中的软件定时器使用的，用户是不能直接访问的。 

<img src=".\img\image-20250219201403037.png" alt="image-20250219201403037" style="zoom:50%;" />

**软件定时器的相关配置**

- 当FreeRTOS 的配置项 configUSE_TIMERS 设置为1，在启动任务调度器时，会自动创建软件定时器的服务/守护任务prvTimerTask( ) 
- 软件定时器服务任务的优先级为 configTIMER_TASK_PRIORITY = 31
- 定时器的命令队列长度为 configTIMER_QUEUE_LENGTH = 5

注

1. 软件定时器的超时回调函数是在软件定时器服务任务中被调用的，服务任务不是专为某个定时器服务的，它还要处理其他定时器，所以，定时器的回调函数不要影响其他“人”
2. 回调函数要尽快实行，不能进入阻塞状态，即不能调用那些会阻塞任务的 API 函数，如vTaskDelay() 
3. 访问队列或者信号量的非零阻塞时间的 API 函数也不能调用

**软件定时器的状态**

- 休眠态：软件定时器可以通过其句柄被引用，但因为没有运行，所以其定时超时回调函数不会被执行
- 运行态：运行态的定时器，当指定时间到达之后，它的超时回调函数会被调用

注

1. 新创建的软件定时器处于休眠状态 ，也就是未运行的
2. 通过发送命令队列来使得软件定时器从休眠态转变为运行态

**单次定时器和周期定时器**

FreeRTOS 提供了两种软件定时器

- 单次定时器：单次定时器的一旦定时超时，只会执行一次其软件定时器超时回调函数，不会自动重新开启定时，不过可以被手动重新开启
- 周期定时器：周期定时器的一旦启动以后就会在执行完回调函数以后自动的重新启动 ，从而周期地执行其软件定时器回调函数

<img src=".\img\image-20250219202334798.png" alt="image-20250219202334798" style="zoom:50%;" />

单次定时器状态转换图

<img src=".\img\image-20250219202409875.png" alt="image-20250219202409875" style="zoom:50%;" />

周期定时器状态转换图

<img src=".\img\image-20250219202524736.png" alt="image-20250219202524736" style="zoom:50%;" />

## 结构

```c
typedef struct
{
    const char * 				pcTimerName				/* 软件定时器名字 */
    ListItem_t 					xTimerListItem			/* 软件定时器列表项 */
    TickType_t 					xTimerPeriodInTicks;   	/* 软件定时器的周期 */   
    void * 						pvTimerID				/* 软件定时器的ID */
    TimerCallbackFunction_t	 	pxCallbackFunction; 	/* 软件定时器的回调函数 */
#if ( configUSE_TRACE_FACILITY == 1 )
    UBaseType_t 				uxTimerNumber			/*  软件定时器的编号，调试用  */
#endif
  uint8_t 						ucStatus;          		/*  软件定时器的状态  */
} xTIMER;
```

## API函数

|            函数             |             描述             |
| :-------------------------: | :--------------------------: |
|       xTimerCreate()        |    动态方式创建软件定时器    |
|    xTimerCreateStatic()     |    静态方式创建软件定时器    |
|        xTimerStart()        |      开启软件定时器定时      |
|    xTimerStartFromISR()     |  在中断中开启软件定时器定时  |
|        xTimerStop()         |      停止软件定时器定时      |
|     xTimerStopFromISR()     |  在中断中停止软件定时器定时  |
|        xTimerReset()        |      复位软件定时器定时      |
|    xTimerResetFromISR()     |  在中断中复位软件定时器定时  |
|    xTimerChangePeriod()     | 更改软件定时器的定时超时时间 |
| xTimerChangePeriodFromISR() |   在中断中更改定时超时时间   |

### xTimerCreate

创建软件定时器。

```c
/*
* 形参:
- pcTimerName 软件定时器名
- xTimerPeriodInTicks 定时超时时间，单位：系统时钟节拍
- uxAutoReload 定时器模式， pdTRUE：周期定时器， pdFALSE：单次定时器
- pvTimerID 软件定时器 ID，用于多个软件定时器公用一个超时回调函数
- pxCallbackFunction 软件定时器超时回调函数
* 返回值
- NULL 软件定时器创建失败
- 其他值 软件定时器创建成功，返回其句柄
*/
TimerHandle_t   xTimerCreate(const char * const 	    pcTimerName,
                             const TickType_t 		    xTimerPeriodInTicks,
                             const UBaseType_t 	    	uxAutoReload,
                             void * const 			    pvTimerID,
                             TimerCallbackFunction_t    pxCallbackFunction ); 
```

### xTimerStart

开启软件定时器。

```c
/*
* 形参:
- xTimer 待开启的软件定时器的句柄
- xTickToWait 发送命令到软件定时器命令队列的最大等待时间
* 返回值
- pdPASS 软件定时器开启成功
- pdFAIL 软件定时器开启失败
*/
BaseType_t xTimerStart(TimerHandle_t 		xTimer,
                       const TickType_t 	xTicksToWait ); 
```

### xTimerStop

停止软件定时器。

```c
/*
* 形参:
- xTimer 待停止的软件定时器的句柄
- xTickToWait 发送命令到软件定时器命令队列的最大等待时间
* 返回值
- pdPASS 软件定时器停止成功
- pdFAIL 软件定时器停止失败
*/
BaseType_t   xTimerStop(TimerHandle_t 		xTimer,
                        const TickType_t 	xTicksToWait); 
```

### xTimerReset

该功能将使软件定时器的重新开启定时，复位后的软件定时器以复位时的时刻作为开启时刻重新定时。

<img src=".\img\image-20250219230951726.png" alt="image-20250219230951726" style="zoom:60%;" />

```c
/*
* 形参:
- xTimer 待复位的软件定时器的句柄
- xTickToWait 发送命令到软件定时器命令队列的最大等待时间
* 返回值
- pdPASS 软件定时器复位成功
- pdFAIL 软件定时器复位失败
*/
BaseType_t  xTimerReset(TimerHandle_t 		xTimer,
                        const TickType_t 	xTicksToWait); 
```

### xTimerChangePeriod

更改软件定时器超时时间。

```c
/*
* 形参:
- xTimer 待更新的软件定时器的句柄
- xNewPeriod 新的定时超时时间，单位：系统时钟节拍
- xTickToWait 发送命令到软件定时器命令队列的最大等待时间
* 返回值
- pdPASS 软件定时器定时超时时间更改成功
- pdFAIL 软件定时器定时超时时间更改失败
*/
BaseType_t  xTimerChangePeriod(TimerHandle_t 		xTimer,
                               const TickType_t 	xNewPeriod,
                               const TickType_t 	xTicksToWait); 
```

## 实例

<img src=".\img\image-20250219231208170.png" alt="image-20250219231208170" style="zoom:40%;" />

```c
/* freertos_demo.c */

/*FreeRTOS配置*/

/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define START_TASK_PRIO         1
#define START_TASK_STACK_SIZE   128
TaskHandle_t    start_task_handler;
void start_task( void * pvParameters );

/* TASK1 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK1_PRIO         2
#define TASK1_STACK_SIZE   128
TaskHandle_t    task1_handler;
void task1( void * pvParameters );

void timer1_callback( TimerHandle_t pxTimer );
void timer2_callback( TimerHandle_t pxTimer );

/**
 * @brief       FreeRTOS例程入口函数
 * @param       无
 * @retval      无
 */
void freertos_demo(void)
{    
    xTaskCreate((TaskFunction_t         )   start_task,
                (char *                 )   "start_task",
                (configSTACK_DEPTH_TYPE )   START_TASK_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   START_TASK_PRIO,
                (TaskHandle_t *         )   &start_task_handler );
    vTaskStartScheduler();
}

TimerHandle_t timer1_handle = 0;    /* 单次定时器 */
TimerHandle_t timer2_handle = 0;    /* 周期定时器 */

void start_task( void * pvParameters )
{
    taskENTER_CRITICAL();               /* 进入临界区 */
    
    /* 单次定时器 */
    timer1_handle = xTimerCreate( "timer1", 
                                    500,
                                    pdFALSE,
                                    (void *)1,
                                    timer1_callback );
                            
    /* 周期定时器 */
    timer2_handle = xTimerCreate( "timer2", 
                                    2000,
                                    pdTRUE,
                                    (void *)2,
                                    timer2_callback );
                                    
    xTaskCreate((TaskFunction_t         )   task1,
                (char *                 )   "task1",
                (configSTACK_DEPTH_TYPE )   TASK1_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK1_PRIO,
                (TaskHandle_t *         )   &task1_handler );
                                             
    vTaskDelete(NULL);
    taskEXIT_CRITICAL();                /* 退出临界区 */
}

/* 任务一，按键扫描并控制软件定时器 */
void task1( void * pvParameters )
{
    uint8_t key = 0;
    while(1) 
    {
        key = key_scan(0);
        if(key == KEY0_PRES)
        {
            xTimerStart(timer1_handle,portMAX_DELAY);
            xTimerStart(timer2_handle,portMAX_DELAY);
        }else if(key == KEY1_PRES)
        {
            xTimerStop(timer1_handle,portMAX_DELAY);
            xTimerStop(timer2_handle,portMAX_DELAY);
        }
        vTaskDelay(10);
    }
}

/* timer1的超时回调函数 */
void timer1_callback( TimerHandle_t pxTimer )
{
    static uint32_t timer = 0;
    printf("timer1的运行次数：%d\r\n",++timer);
}

/* timer2的超时回调函数 */
void timer2_callback( TimerHandle_t pxTimer )
{
    static uint32_t timer = 0;
    printf("timer2的运行次数：%d\r\n",++timer);
}
```

# Tickless低功耗模式

## 简介

很多应用场合对于功耗的要求很严格，比如可穿戴低功耗产品、物联网低功耗产品等，一般MCU都有相应的低功耗模式，裸机开发时可以使用MCU的低功耗模式。FreeRTOS也提供了一个叫Tickless的低功耗模式，方便带FreeRTOS操作系统的应用开发。

**STM32低功耗模式**

- 睡眠模式
- 停止模式
- 待机模式

<img src=".\img\image-20250220150927472.png" alt="image-20250220150927472" style="zoom:60%;" />

1. 进入睡眠模式
  - WFI 指令：__WFI
  - WFE 指令：__WFE 

2. 退出睡眠模式：任何中断或事件都可以唤醒睡眠模式


**Tickless模式**

Tickless低功耗模式的本质是通过调用指令 WFI 实现睡眠模式。

为了可以降低功耗，又不影响系统运行，可以在本该空闲任务执行的期间，让MCU 进入相应的低功耗模式，当其他任务准备运行的时候，唤醒MCU退出低功耗模式。

注

1. 将滴答定时器的中断周期修改为低功耗运行时间
2. 退出低功耗后，需补上系统时钟节拍数

**Tickless模式相关配置项**

- configUSE_TICKLESS_IDLE 此宏用于使能低功耗 Tickless 模式 默认为1
- configEXPECTED_IDLE_TIME_BEFORE_SLEEP 此宏用于定义系统进入相应低功耗模式的最短时长
- configPRE_SLEEP_PROCESSING(x) 此宏用于定义需要在系统进入低功耗模式前执行的事务，如进入低功耗前关闭外设时钟、降低系统主频，以达到降低功耗的目的
- configPOST_SLEEP_PROCESSING(x) 此宏用于定义需要在系统退出低功耗模式后执行的事务，如退出低功耗后开启之前关闭的外设时钟、恢复系统时钟主频，以使系统能够正常运行

**使用Tickless须满足条件**

- 在 FreeRTOSConfig.h 文件中配置宏定义 configUSE_TICKLESS_IDLE 为 1
- 满足当前空闲任务正在运行，所有其他任务处在挂起状态或阻塞状态
- 当系统可运行于低功耗模式的时钟节拍数大于等于 configEXPECTED_IDLE_TIME_BEFORE_SLEEP（该宏默认为2个系统时钟节拍）

## 实例

<img src=".\img\image-20250220151442113.png" alt="image-20250220151442113" style="zoom:40%;" />

```c
/* freertos_demo.c */

/*FreeRTOS配置*/

/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define START_TASK_PRIO         1
#define START_TASK_STACK_SIZE   128
TaskHandle_t    start_task_handler;
void start_task( void * pvParameters );

/* TASK1 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK1_PRIO         2
#define TASK1_STACK_SIZE   128
TaskHandle_t    task1_handler;
void task1( void * pvParameters );

/* TASK2 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK2_PRIO         3
#define TASK2_STACK_SIZE   128
TaskHandle_t    task2_handler;
void task2( void * pvParameters );

QueueHandle_t semphore_handle;
/* 进入低功耗前所需要执行的操作 */
void PRE_SLEEP_PROCESSING(void)
{
    __HAL_RCC_GPIOA_CLK_DISABLE();
    __HAL_RCC_GPIOB_CLK_DISABLE();
    __HAL_RCC_GPIOC_CLK_DISABLE();
    __HAL_RCC_GPIOD_CLK_DISABLE();
    __HAL_RCC_GPIOE_CLK_DISABLE();
    __HAL_RCC_GPIOF_CLK_DISABLE();
    __HAL_RCC_GPIOG_CLK_DISABLE();
}
/* 退出低功耗后所需要执行的操作 */
void POST_SLEEP_PROCESSING(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
}
/**
 * @brief       FreeRTOS例程入口函数
 * @param       无
 * @retval      无
 */
void freertos_demo(void)
{    
    semphore_handle = xSemaphoreCreateBinary();
    if(semphore_handle != NULL)
    {
        printf("二值信号量创建成功！！！\r\n");
    }
    xTaskCreate((TaskFunction_t         )   start_task,
                (char *                 )   "start_task",
                (configSTACK_DEPTH_TYPE )   START_TASK_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   START_TASK_PRIO,
                (TaskHandle_t *         )   &start_task_handler );
    vTaskStartScheduler();
}

void start_task( void * pvParameters )
{
    taskENTER_CRITICAL();               /* 进入临界区 */
    xTaskCreate((TaskFunction_t         )   task1,
                (char *                 )   "task1",
                (configSTACK_DEPTH_TYPE )   TASK1_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK1_PRIO,
                (TaskHandle_t *         )   &task1_handler );
                
    xTaskCreate((TaskFunction_t         )   task2,
                (char *                 )   "task2",
                (configSTACK_DEPTH_TYPE )   TASK2_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK2_PRIO,
                (TaskHandle_t *         )   &task2_handler );
                             
    vTaskDelete(NULL);
    taskEXIT_CRITICAL();                /* 退出临界区 */
}

/* 任务一，释放二值信号量 */
void task1( void * pvParameters )
{
    uint8_t key = 0;
    BaseType_t err;
    while(1) 
    {
        key = key_scan(0);
        if(key == KEY0_PRES)
        {
            if(semphore_handle != NULL)
            {
                err = xSemaphoreGive(semphore_handle);
                if(err == pdPASS)
                {
                    printf("信号量释放成功！！\r\n");
                }else printf("信号量释放失败！！\r\n");
            }
            
        }
        vTaskDelay(10);
    }
}

/* 任务二，获取二值信号量 */
void task2( void * pvParameters )
{
    uint32_t i = 0;
    BaseType_t err;
    while(1)
    {
        err = xSemaphoreTake(semphore_handle,portMAX_DELAY); /* 获取信号量并死等 */
        if(err == pdTRUE)
        {
            printf("获取信号量成功\r\n");
        }else printf("已超时%d\r\n",++i);
        
    }
}
```

# 内存管理

## 简介

在使用 FreeRTOS 创建任务、队列、信号量等对象的时，一般都提供了两种方法

- 动态方法创建：自动地从 FreeRTOS 管理的内存堆中申请创建对象所需的内存，并且在对象删除后，可将这块内存释放回FreeRTOS管理的内存堆
- 静态方法创建：需用户提供各种内存空间，并且使用静态方式占用的内存空间一般固定下来了，即使任务、队列等被删除后，这些被占用的内存空间一般没有其他用途

动态方式管理内存相比与静态方式，更加灵活。 

**标准 C 库内存管理**

除了 FreeRTOS 提供的动态内存管理方法，标准的 C 库也提供了函数 malloc()和函数 free()来实现动态地申请和释放内存 。

标准 C 库的动态内存管理方法有如下几个缺点

- 占用大量的代码空间，不适合用在资源紧缺的嵌入式系统中
- 没有线程安全的相关机制
- 运行有不确定性，每次调用这些函数时花费的时间可能都不相同
- 内存碎片化

因此，FreeRTOS 提供了多种动态内存管理的算法，可针对不同的嵌入式系统。

## 内存管理算法

FreeRTOS提供了5种动态内存管理算法，分别为： heap_1、heap_2、heap_3、heap_4、heap_5 

|  算法  |                  优点                   |                     缺点                     |
| :----: | :-------------------------------------: | :------------------------------------------: |
| heap_1 |           分配简单，时间确定            |        只允许申请内存，不允许释放内存        |
| heap_2 |           允许申请和释放内存            | 不能合并相邻的空闲内存块会产生碎片、时间不定 |
| heap_3 | 直接调用C库函数malloc()和 free() ，简单 |               速度慢、时间不定               |
| heap_4 | 相邻空闲内存可合并，减少内存碎片的产生  |                   时间不定                   |
| heap_5 |   能够管理多个非连续内存区域的 heap_4   |                   时间不定                   |

### heap_1

heap_1只实现了pvPortMalloc，没有实现 pvPortFree。也就是说，它只能申请内存，无法释放内存。

heap_1的实现最为简单，管理的内存堆是一个数组，在申请内存的时候， heap_1 内存管理算法只是简单地从数组中分出合适大小的内存，内存堆数组的定义如下所示 

```c
/* 定义一个大数组作为 FreeRTOS 管理的内存堆 */
static uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];
```

分配过程

<img src=".\img\image-20250220155746423.png" alt="image-20250220155746423" style="zoom:40%;" />

### heap_2

相比于 heap_1 内存管理算法， heap_2 内存管理算法使用最适应算法，并且支持释放内存。

heap_2 内存管理算法并不能将相邻的空闲内存块合并成一个大的空闲内存块，因此 heap_2 内存管理算法不可避免地会产生内存碎片。

内存碎片是由于多次申请和释放内存，但释放的内存无法与相邻的空闲内存合并而产生的。

**最适应算法**

假设heap有3块空闲内存（按内存块大小由小到大排序）：5字节、25字节、50字节，现在新创建一个任务需要申请20字节的内存

1. 找出最小的、能满足pvPortMalloc的内存：25字节
2. 把它划分为20字节、5字节；返回这20字节的地址，剩下的5字节仍然是空闲状态，留给后续的pvPortMalloc使用

分配过程

<img src=".\img\image-20250220160110999.png" alt="image-20250220160110999" style="zoom:40%;" />

适用场景：频繁的创建和删除任务，且所创建的任务堆栈都相同，这类场景下Heap_2没有碎片化的问题。

### heap_3

调用C库函数malloc()和free()，Heap_3中先暂停FreeRTOS的调度器，再去调用这些函数，使用这种方法实现了线程安全。

### heap_4

heap_4 内存管理算法使用了首次适应算法，也支持内存的申请与释放，并且能够将空闲且相邻的内存进行合并，从而减少内存碎片的现象。

heap_4内存管理算法会把相邻的空闲内存合并为一个更大的空闲内存，这有助于减少内存的碎片问题。

**首次适应算法**

假设heap有3块空闲内存（按内存块地址由低到高排序）：5字节、50字节、25字节，现在新创建一个任务需要申请20字节的内存

1. 找出第一个能满足pvPortMalloc的内存：50字节
2. 把它划分为20字节、30字节，返回这20字节的地址，剩下30字节仍然是空闲状态，留给后续的pvPortMalloc使用

分配过程

<img src=".\img\image-20250220160349306.png" alt="image-20250220160349306" style="zoom:40%;" />

适用于这种场景：频繁地分配、释放不同大小的内存。

### heap_5

heap_5 内存管理算法是在 heap_4 内存管理算法的基础上实现的，在 heap_4 内存管理算法的基础上实现了管理多个非连续内存区域的能力 。

heap_5 内存管理算法默认并没有定义内存堆 ， 需要用户手动指定内存区域的信息，对其进行初始化。

使用如下结构体指定一块内存

```c
typedef struct HeapRegion{
    uint8_t * 	pucStartAddress; 	/* 内存区域的起始地址 */
    size_t 		xSizeInBytes; 		/* 内存区域的大小，单位：字节 */
} HeapRegion_t; 
```

指定多块且不连续的内存

```c
Const  HeapRegion_t  xHeapRegions[] ={
    { (uint8_t *)0x80000000, 0x10000 }, 	/* 内存区域 1 */
    { (uint8_t *)0x90000000, 0xA0000 }, 	/* 内存区域 2 */
    { NULL, 0 } 							/* 数组终止标志 */
};vPortDefineHeapRegions(xHeapRegions); 
```

适用场景：在嵌入式系统中，那些内存的地址并不连续的场景。

## API函数

|                     函数                     |          描述          |
| :------------------------------------------: | :--------------------: |
| void * pvPortMalloc( size_t   xWantedSize ); |        申请内存        |
|        void   vPortFree( void * pv );        |        释放内存        |
|    size_t   xPortGetFreeHeapSize( void );    | 获取当前空闲内存的大小 |

注

1. 在一段内存没有被释放之前绝对不能再调用一次函数pvPortMalloc()为其再次分配内存，否则会导致内存泄露

**pvPortMalloc**

由于字节对齐和存储结构体等因素，该函数申请的内存大小实际略大。

```c
/*
* 形参：
- xWantedSize 申请的内存大小，以字节为单位 
* 返回值：
- 一个指针 指向已分配大小的内存
- NULL 申请内存失败
*/
void * pvPortMalloc( size_t  xWantedSize );
```

**vPortFree**

```c
/*
* 形参：
- * pv 指针指向一个要释放内存的内存块
*/
void  vPortFree( void * pv );
```

**xPortGetFreeHeapSize**

```c
/*
* 返回值：
- 返回当前剩余的空闲内存大小
*/
size_t  xPortGetFreeHeapSize( void );
```

## 实例

<img src=".\img\image-20250220161125947.png" alt="image-20250220161125947" style="zoom:40%;" />

```c
/* freertos_demo.c */

/*FreeRTOS配置*/

/* START_TASK 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define START_TASK_PRIO         1
#define START_TASK_STACK_SIZE   128
TaskHandle_t    start_task_handler;
void start_task( void * pvParameters );

/* TASK1 任务 配置
 * 包括: 任务句柄 任务优先级 堆栈大小 创建任务
 */
#define TASK1_PRIO         2
#define TASK1_STACK_SIZE   128
TaskHandle_t    task1_handler;
void task1( void * pvParameters );

/**
 * @brief       FreeRTOS例程入口函数
 * @param       无
 * @retval      无
 */
void freertos_demo(void)
{    
    xTaskCreate((TaskFunction_t         )   start_task,
                (char *                 )   "start_task",
                (configSTACK_DEPTH_TYPE )   START_TASK_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   START_TASK_PRIO,
                (TaskHandle_t *         )   &start_task_handler );
    vTaskStartScheduler();
}


void start_task( void * pvParameters )
{
    taskENTER_CRITICAL();               /* 进入临界区 */
    xTaskCreate((TaskFunction_t         )   task1,
                (char *                 )   "task1",
                (configSTACK_DEPTH_TYPE )   TASK1_STACK_SIZE,
                (void *                 )   NULL,
                (UBaseType_t            )   TASK1_PRIO,
                (TaskHandle_t *         )   &task1_handler );

    vTaskDelete(NULL);
    taskEXIT_CRITICAL();                /* 退出临界区 */
}

/* 任务一，申请内存以及释放内存，并显示空闲内存大小 */
void task1( void * pvParameters )
{
    uint8_t key = 0, t = 0;
    uint8_t * buf = NULL;
    while(1) 
    {
        key = key_scan(0);
        if(key == KEY0_PRES)
        {
            buf = pvPortMalloc(30);                 /* 申请内存 */
            if(buf != NULL)
            {
                printf("申请内存成功！\r\n");
            }else printf("申请内存失败\r\n");
        }else if(key == KEY1_PRES)
        {
            if(buf != NULL)
            {
                vPortFree(buf);                     /* 释放内存 */
                printf("释放内存！！\r\n");
            }                
        }
        if(t++ > 50)
        {
            t = 0;
            printf("剩余的空闲内存大小为：%d\r\n",xPortGetFreeHeapSize());
        }
        vTaskDelay(10);
    }
}
```

