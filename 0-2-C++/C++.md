<font size = 6>C++</font>

[toc]

# 面向对象三大特性

## 封装

利用抽象数据类型将数据和基于数据的操作封装在一起，使其构成一个不可分割的独立实体。数据被保护在抽象数据类型的内部，尽可能地隐藏内部的细节，只保留一些对外的接口使其与外部发生联系。用户无需关心对象内部的细节，但可以通过对象对外提供的接口来访问该对象。

优点

- 减少耦合：可以独立地开发、测试、优化、使用、理解和修改
- 减轻维护的负担：可以更容易被理解，并且在调试的时候可以不影响其他模块
- 有效地调节性能：可以通过剖析来确定哪些模块影响了系统的性能
- 提高软件的可重用性
- 降低了构建大型系统的风险：即使整个系统不可用，但是这些独立的模块却有可能是可用的

以下 Person 类封装 name、gender、age 等属性，外界只能通过 get() 方法获取一个 Person 对象的 name 属性和 gender 属性，而无法获取 age 属性，但是 age 属性可以供 work() 方法使用。

注意到 gender 属性使用 int 数据类型进行存储，封装使得用户注意不到这种实现细节。并且在需要修改 gender 属性使用的数据类型时，也可以在不影响客户端代码的情况下进行。

```c++
class Person {
private:
  	string name;
    int gender;
    int age;

public:
    Person(string name, int gender, int age) : name(name), gender(gender), age(age) {}

    // 常量成员函数（不修改成员变量）
    string getName() const {
        return name;
    }

    string getGender() const {
        return gender == 0 ? "man" : "woman";
    }

    void work() const {
        if (18 <= age && age <= 50) {
            cout << name << " is working very hard!" << endl;
        } else {
            cout << name << " can't work any more!" << endl;
        }
    }
};
```

## 继承

继承实现了 **IS-A** 关系，例如 Cat 和 Animal 就是一种 IS-A 关系，因此 Cat 可以继承自 Animal，从而获得 Animal 非 private 的属性和方法。

继承应该遵循里氏替换原则，子类对象必须能够替换掉所有父类对象。

Cat 可以当做 Animal 来使用，也就是说可以使用 Animal 引用 Cat 对象。父类引用指向子类对象称为**向上转型** 。

```c++
// 基类 Animal
class Animal {
public:
    virtual void speak() const = 0; 	// 纯虚函数，确保 Animal 是抽象类
    virtual ~Animal() {} 							// 虚析构函数，确保派生类对象的正确清理
};

// 派生类 Cat
class Cat : public Animal {
public:
    void speak() const override { 		// 覆盖基类的虚函数
        std::cout << "Meow!" << std::endl;
    }
};

int main() {
    // 使用 new 动态创建一个 Cat 对象，并将其地址赋给 Animal 类型的引用
    Animal* animal = new Cat();
    animal->speak(); 			// 调用 Cat 的 speak 方法
    delete animal; 				// 释放动态分配的内存
    return 0;
}
```

## 多态

多态分为编译时多态和运行时多态

- 编译时多态：主要指方法的重载
- 运行时多态：指程序中定义的对象引用所指向的具体类型在运行期间才确定

运行时多态有三个条件

- 继承
- 覆盖（重写）
- 向上转型

下面的代码中，乐器类（Instrument）有两个子类：Wind 和 Percussion，它们都覆盖了父类的 play() 方法，并且在 main() 方法中使用父类 Instrument 来引用 Wind 和 Percussion 对象。在 Instrument 引用调用 play() 方法时，会执行实际引用对象所在类的 play() 方法，而不是 Instrument 类的方法。

```java
class Instrument {
public:
    virtual void play() {
        std::cout << "Instrument is playing..." << std::endl;
    }
    virtual ~Instrument() = default;
};

class Wind : public Instrument {
public:
    void play() override {
        std::cout << "Wind is playing..." << std::endl;
    }
};

class Percussion : public Instrument {
public:
    void play() override {
        std::cout << "Percussion is playing..." << std::endl;
    }
};

int main() {
    std::vector<std::unique_ptr<Instrument>> instruments;
    instruments.push_back(std::make_unique<Wind>());
    instruments.push_back(std::make_unique<Percussion>());

    for (const auto& instrument : instruments) {
        instrument->play();
    }

    return 0;
}

运行结果：
Wind is playing...
Percussion is playing...
```

# 设计原则

## S.O.L.I.D

| 简写 |                全拼                 |   中文翻译   |
| :--: | :---------------------------------: | :----------: |
| SRP  | The Single Responsibility Principle | 单一责任原则 |
| OCP  |      The Open Closed Principle      | 开放封闭原则 |
| LSP  |  The Liskov Substitution Principle  | 里氏替换原则 |
| ISP  | The Interface Segregation Principle | 接口分离原则 |
| DIP  | The Dependency Inversion Principle  | 依赖倒置原则 |

### 单一责任原则

> 修改一个类的原因应该只有一个。

换句话说就是让一个类只负责一件事，当这个类需要做过多事情的时候，就需要分解这个类。

如果一个类承担的职责过多，就等于把这些职责耦合在了一起，一个职责的变化可能会削弱这个类完成其它职责的能力。

### 开放封闭原则

> 类应该对扩展开放，对修改关闭。

扩展就是添加新功能的意思，因此该原则要求在添加新功能时不需要修改代码。

符合开闭原则最典型的设计模式是装饰者模式，它可以动态地将责任附加到对象上，而不用去修改类的代码。

### 里氏替换原则

> 子类对象必须能够替换掉所有父类对象。

继承是一种 IS-A 关系，子类需要能够当成父类来使用，并且需要比父类更特殊。

如果不满足这个原则，那么各个子类的行为上就会有很大差异，增加继承体系的复杂度。

### 接口分离原则

> 不应该强迫客户依赖于它们不用的方法。

因此使用多个专门的接口比使用单一的总接口要好。

### 依赖倒置原则

> 高层模块不应该依赖于低层模块，二者都应该依赖于抽象；
> 抽象不应该依赖于细节，细节应该依赖于抽象。

高层模块包含一个应用程序中重要的策略选择和业务模块，如果高层模块依赖于低层模块，那么低层模块的改动就会直接影响到高层模块，从而迫使高层模块也需要改动。

依赖于抽象意味着

- 任何变量都不应该持有一个指向具体类的指针或者引用；
- 任何类都不应该从具体类派生；
- 任何方法都不应该覆写它的任何基类中的已经实现的方法。

## 其他常见原则

除了上述的经典原则，在实际开发中还有下面这些常见的设计原则。

| 简写 |               全拼                |   中文翻译   |
| :--: | :-------------------------------: | :----------: |
| LOD  |        The Law of Demeter         |  迪米特法则  |
| CRP  |   The Composite Reuse Principle   | 合成复用原则 |
| CCP  |   The Common Closure Principle    | 共同封闭原则 |
| SAP  | The Stable Abstractions Principle | 稳定抽象原则 |
| SDP  | The Stable Dependencies Principle | 稳定依赖原则 |

### 迪米特法则

迪米特法则又叫作最少知识原则（Least Knowledge Principle，简写 LKP），就是说一个对象应当对其他对象有尽可能少的了解，不和陌生人说话。

### 合成复用原则

尽量使用对象组合，而不是通过继承来达到复用的目的。

### 共同封闭原则

一起修改的类，应该组合在一起（同一个包里）。如果必须修改应用程序里的代码，我们希望所有的修改都发生在一个包里（修改关闭），而不是遍布在很多包里。

### 稳定抽象原则

最稳定的包应该是最抽象的包，不稳定的包应该是具体的包，即包的抽象程度跟它的稳定性成正比。

### 稳定依赖原则

包之间的依赖关系都应该是稳定方向依赖的，包要依赖的包要比自己更具有稳定性。

# 基础

* 一般数据成员设为私有
* 定义函数前需声明
* 虚函数：派生类可以重写这个虚函数，实现多态性
* C/C++在windows系统下返回上两级目录

```c++
 QIcon con("..\\..\\back.svg");
//Windows中 "\（反斜杠）"表示路径，而C语言中\为转义字符，故需\\表示\ 
```

1. **`void ClientHandler::working()`**：
   - 这是一个成员函数的声明，它属于`ClientHandler`类。这个函数是`ClientHandler`类的一部分，因此它可以访问类的私有成员和保护成员。
2. **`void working()`**：
   - 这是一个全局函数的声明，它不属于任何类。是全局作用域的一部分，它不能直接访问任何类的私有成员，除非这些成员被明确地作为参数传递给它，或者通过类的实例。

# 线程池

在 C/C++ 中，线程池（Thread Pool）是一种多线程编程技术，它用于管理和优化线程的使用。线程池的主要目的是减少线程创建和销毁的开销，通过重用已经创建的线程来执行多个任务，从而提高程序的执行效率和响应速度。

## 线程池的主要特点

1. **线程复用**：线程池中的线程会在完成任务后被保留，而不是销毁，以便可以被重新用于执行后续的任务。

2. **任务队列**：线程池通常包含一个任务队列，用于存储待执行的任务。

3. **线程管理**：线程池负责创建和管理线程的生命周期，包括线程的创建、执行和销毁。

4. **并发控制**：线程池可以控制同时运行的线程数量，避免过多的线程竞争导致的资源冲突和性能问题。

5. **负载均衡**：线程池可以动态分配任务给线程，实现负载均衡，提高资源利用率。

## 线程池的工作流程

1. **任务提交**：当有新任务需要执行时，任务会被提交到线程池的任务队列中。

2. **任务分配**：线程池中的线程会从任务队列中取出任务并执行。

3. **任务执行**：线程执行分配到的任务。

4. **线程等待**：如果任务队列为空，线程会等待新任务的到来。

5. **线程销毁**：在某些情况下，如果线程长时间没有任务执行，线程池可能会销毁一些线程以节省资源。

## 线程池的应用场景

1. **服务器程序**：在服务器程序中，线程池可以处理多个客户端的请求，提高响应速度和处理能力。

2. **并行计算**：在需要并行执行多个计算任务的场景中，线程池可以有效地管理线程资源。

3. **图形界面程序**：在图形界面程序中，线程池可以用于后台任务的处理，避免界面冻结。

4. **批量处理**：在需要批量处理大量数据的场景中，线程池可以提高处理效率。

## C/C++ 中实现线程池

在 C/C++ 中实现线程池，通常需要以下组件：

- **线程类**：用于创建和管理线程。
- **同步机制**：如互斥锁（mutex）、条件变量（condition variables）等，用于线程间的同步和通信。
- **任务队列**：用于存储待执行的任务。
- **工作线程**：线程池中的线程，负责从任务队列中取出任务并执行。

在 C++11 及更高版本中，可以使用标准库中的线程（`<thread>`）、互斥锁（`<mutex>`）和条件变量（`<condition_variable>`）等来实现线程池。此外，也有一些第三方库，如 Boost.Asio，提供了线程池的实现。

线程池是提高多线程程序性能和资源利用率的重要工具，它通过减少线程创建和销毁的开销，以及提供线程和任务管理的机制，使得多线程编程更加高效和易于控制。

# 智能指针

智能指针是 C++11 引入的一种管理动态分配内存的模板类，它们提供了自动的内存管理功能，以帮助防止内存泄漏和其他内存管理错误。智能指针的主要目的是确保当对象不再被使用时，能够自动释放它们所占用的内存。

C++ 标准库提供了几种智能指针类型，主要包括：

1. **`std::unique_ptr`**：
   - 用于管理单个对象的内存，不允许复制（但可以移动），确保同一时间只有一个智能指针管理该对象。
   - 当 `std::unique_ptr` 被销毁时（例如离开作用域），它会自动删除它所管理的对象。

2. **`std::shared_ptr`**：
   - 用于管理对象的内存，允许多个智能指针共同管理同一个对象。
   - 使用引用计数机制来跟踪有多少个 `std::shared_ptr` 指向同一个对象。当最后一个引用被销毁时，所管理的对象会被删除。

3. **`std::weak_ptr`**：
   - 是 `std::shared_ptr` 的一种补充，它持有一个弱引用，不增加对象的引用计数。
   - 可以用于解决 `std::shared_ptr` 相互引用时可能产生的循环引用问题。

智能指针的使用示例：

```cpp
#include <memory>
#include <iostream>

class MyClass {
public:
    void doSomething() {
        std::cout << "Doing something" << std::endl;
    }
};

int main() {
    // 使用 std::unique_ptr
    std::unique_ptr<MyClass> uniquePtr(new MyClass());
    uniquePtr->doSomething(); // 使用 MyClass 的方法

    // 使用 std::shared_ptr
    std::shared_ptr<MyClass> sharedPtr1(new MyClass());
    std::shared_ptr<MyClass> sharedPtr2 = sharedPtr1; // 共享同一个对象
    sharedPtr1->doSomething(); // 使用 MyClass 的方法

    // sharedPtr1 和 sharedPtr2 都会在离开作用域时自动释放对象
    return 0;
}
```

智能指针的优点包括：

- **自动内存管理**：自动释放内存，减少内存泄漏。
- **异常安全**：即使在抛出异常的情况下，也能确保资源被释放。
- **表达所有权语义**：代码更清晰，易于理解谁拥有资源。
- **兼容性**：智能指针可以与原始指针一起使用，也可以与其他智能指针一起使用。

智能指针的缺点包括：

- **性能开销**：相比于原始指针，智能指针有一定的性能开销，尤其是在 `std::shared_ptr` 的情况下，因为需要维护引用计数。
- **不可拷贝性**：`std::unique_ptr` 不能被拷贝，只能被移动，这限制了其在某些场景下的使用。

总的来说，智能指针是现代 C++ 编程中管理动态内存的重要工具，它们提供了一种安全且高效的方式来处理资源的生命周期。

# 虚函数

C++多态(polymorphism)是通过虚函数来实现的，虚函数允许子类重新定义成员函数，而子类重新定义父类的做法称为覆盖(override)，或者称为重写。

最常见的用法就是声明基类的指针，利用该指针指向任意一个子类对象，调用相应的虚函数，动态绑定。由于编写代码的时候并不能确定被调用的是基类的函数还是哪个派生类的函数，所以被成为“虚”函数。如果没有使用虚函数的话，即没有利用C++多态性，则利用基类指针调用相应的函数的时候，将总被限制在基类函数本身，而无法调用到子类中被重写过的函数。
```c++
#include<iostream>  
using namespace std;  
  
class A  
{  
public:  
    void foo()  
    {  
        printf("1\n");  
    }  
    virtual void fun()  
    {  
        printf("2\n");  
    }  
};  
class B : public A  
{  
public:  
    void foo()  //隐藏：派生类的函数屏蔽了与其同名的基类函数
    {  
        printf("3\n");  
    }  
    void fun()  //多态、覆盖
    {  
        printf("4\n");  
    }  
};  
int main(void)  
{  
    A a;  
    B b;  
    A *p = &a;  
    p->foo();  //输出1
    p->fun();  //输出2
    p = &b;  
    p->foo();  //取决于指针类型，输出1
    p->fun();  //取决于对象类型，输出4，体现了多态
    return 0;  
} 
```

虚函数的作用是允许在派生类中重新定义与基类同名的函数，并且可以通过**基类**指针或引用来访问基类和派生类中的同名函数。

虚函数是C++中用于实现多态的机制。核心理念就是通过基类访问派生类定义的函数。如果父类或者祖先类中函数func()为虚函数，则子类及后代类中，函数func()是否加[virtual](https://so.csdn.net/so/search?q=virtual&spm=1001.2101.3001.7020)关键字，都将是虚函数。为了提高程序的可读性，建议后代中虚函数都加上virtual关键字。

## C++纯虚函数及虚函数

纯虚函数是在基类中声明的一种虚函数，它在基类中没有定义，但要求任何派生类都要定义自己的实现方法。在基类中实现纯虚函数的方法是在函数原型后加“*=0*” 。

包含纯虚函数的类称为**抽象类**。由于抽象类包含了没有定义的纯虚函数，所以不能定义抽象类的对象。抽象类的主要作用是将有关的操作作为结果接口组织在一个继承层次结构中，由它来为派生类提供一个公共的根，派生类将具体实现在其基类中作为接口的操作。

## override

override 仅能用于虚函数，他属于C++新特性，是重写覆盖的意思，他的存在仅仅是为了提高代码的可阅读性

```c++
// 基类 Animal
class Animal {
public:
    virtual void speak() const = 0; 	// 纯虚函数，确保 Animal 是抽象类
    virtual ~Animal() {} 							// 虚析构函数，确保派生类对象的正确清理
};

// 派生类 Cat
class Cat : public Animal {
public:
    void speak() const override { 		// 覆盖基类的虚函数
        std::cout << "Meow!" << std::endl;
    }
};

int main() {
    // 使用 new 动态创建一个 Cat 对象，并将其地址赋给 Animal 类型的引用
    Animal* animal = new Cat();
    animal->speak(); 			// 调用 Cat 的 speak 方法
    delete animal; 				// 释放动态分配的内存
    return 0;
}
```

作用：

1. 提示程序的阅读者，这个函数是重写父类的功能。
2. 防止程序员在重写父类的函数时，把函数名写错。

**注意：**在方法声明后加 override 修饰便可，不需在方法实现的后方增加。
