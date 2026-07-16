<font size = 6>Rust</font>

[toc]

# 简介

**主流编程语言缺点**

- C/C++性能非常好，但类型系统和内存都不太安全
- java/c#拥有GC，能保证内存安全，但性能不行

**Rust介绍**

Rust是一种预编译静态类型语言，更适用于如下场景

- 需要运行时的速度
- 需要内存安全
- 更好地利用多处理器

**擅长领域**

- 高性能 Web Service
- WebAssembly
- 命令行工具
- 网络编程
- 嵌入式设备
- 系统编程

**优点**

- 内存安全，不牺牲性能
- 易于维护、调试，代码安全高效
- 无畏并发

注

1. 推荐使用msvc而不是gnu

**rustup、rustc、cargo 等工具之间的关系**

- rustup 是 rust 语言的安装器以及版本管理工具，使用 rustup 进行 rust 工具链的版本管理
- rustc 是 rust 的编译器，负责将 rust 代码编译为可执行文件或库
- cargo 是 rust 的包管理器以及构建工具，能够帮助用户管理项目依赖，构建以及运行测试，文档生成，发布库
- rustdoc 是从 rust 源码生成文档的工具

还有很多其他工具，如 rust-std、rustfmt、clippy(静态代码分析工具)、cargo-clippy

# Rustc

- 程序文件后缀名：.rs
- 编译：rustc main.rs

|  语言  |  默认编译体积  |     优化后体积     |         备注         |
| :----: | :------------: | :----------------: | :------------------: |
|   C    |     16 KB      |       14 KB        |    汇编优化潜力大    |
|  C++   |    17–20 KB    |      15–18 KB      |    依赖标准库较大    |
|  Rust  | 3–5 MB (Debug) |     100–600 KB     | Release 模式优化显著 |
|  Java  | 400 B (.class) |   20–50 MB (JAR)   |   依赖 JVM 运行时    |
| Python |   20 B (.py)   | 5–10 MB (打包 exe) |     需嵌入解释器     |

注

1. rustc只适合简单的rust程序

# Cargo

Cargo是Rust的构建系统和包管理工具，用来构建代码、下载依赖的库、构建这些库。

## 创建项目

```rust
cargo new hello_cargo              // 新建名为 hello_cargo 的目录和项目

cargo new hello_cargo --lib        // 创建一个库crate

cargo new hello_cargo --vcs=git    // 不使用git（无git相关文件）
```

目录结构

> - hello_cargo（目录）
>   - src（目录）：存放源代码
>     - main.rs
>   - Cargo.toml：配置文件
>   - .gitignore：初始化一个新的Git仓库
>   - README、许可信息、配置文件等与程序源码无关的文件

**Cargo.toml**

Cargo的配置文件，内容如下

```rust
[package]					// 区域标题，表示下方内容用来配置包
name = "hello_cargo"		// 项目名
version = "0.1.0"			// 项目版本
edition = "2024"			// 使用Rust版本

[dependencies]				// 另一个区域的开始，会列出项目的依赖项
rand = "0.8.5"				// 例如需要添加外部库 表示至少是0.8.5但小于0.9.0的版本
```

**将项目转为使用cargo**

1. 源代码文件移动到src下
2. 创建cargo.toml并填写相应的配置

## 构建运行项目

**构建项目**

```rust
cargo build					// 需要在Cargo.toml文件同级目录下
```

- Cargo 会将构建的可执行文件结果放到 target/debug 目录下


- 第一次运行会在项目目录生成Cargo.lock文件，该文件负责追踪项目依赖的精确版本，无须修改

**构建并运行项目**

```rust
cargo run					// 需要在Cargo.toml文件同级目录下
```

- 若之前编译成功，且源码没有改变，则直接运行之前编译好的二进制文件

**仅构建不产生可执行文件**

``````rust
cargo check					// 需要在Cargo.toml文件同级目录下
``````

- 检查代码是否能编译通过，不产生任何可执行文件

- 比 cargo build 快得多，在编写程序中常用，确保代码正确

**为发布构建**

```rust
cargo build --release		// 优化编译项目
```

- 会在target/release 生成可执行文件
- 代码运行更快，但编译时间会更长

## 其他命令

**添加外部库**

```rust
cargo add rand@0.8.5		// 以命令行的方式添加外部库
```

**更新库包版本**

```rust
cargo update				// 忽略cargo.lock中内容，重新寻找新版本包并写入cargo.lock
```

**安装二进制文件**

`cargo install` 命令用于在本地安装和使用二进制 crate，只有拥有二进制目标文件的包能够被安装。

所有来自 `cargo install` 的二进制文件都安装到 Rust 安装根目录的 bin 文件夹中。

如果是使用 rustup.rs 来安装 Rust 且没有自定义任何配置，这个目录将是 *$HOME/.cargo/bin*。确保将这个目录添加到 `$PATH` 环境变量中就能够运行通过 `cargo install` 安装的程序了。

例如安装 `ripgrep` 

```rust
$ cargo install ripgrep
    Updating crates.io index
  Downloaded ripgrep v14.1.1
  Downloaded 1 crate (213.6 KB) in 0.40s
  Installing ripgrep v14.1.1
--snip--
   Compiling grep v0.3.2
    Finished `release` profile [optimized + debuginfo] target(s) in 6.73s
  Installing ~/.cargo/bin/rg
   Installed package `ripgrep v14.1.1` (executable `rg`)
```

倒数第二行输出展示了安装的二进制文件的位置和名称，在这里 `ripgrep` 被命名为 `rg`。只要将安装目录加入 `$PATH`，就可以运行 `rg --help` 了。

## 库包

libary crate，即库包，为其它程序所用，本身不可独立运行。

- 默认情况下，Rust 设定了若干个会自动导入到每个程序作用域中的标准库内容，这组内容被称为预导入（prelude）内容
- cargo.toml设置好后，第一次cargo build构建项目时，cargo.lock会找到符合依赖项的所有的版本，同时写到cargo.lock，此后程序会默认使用其中版本的依赖项

注

1. 由于 Cargo.lock 文件对于可重复构建非常重要，因此它通常会和项目中的其余代码一样纳入到版本控制系统中

## 发布配置自定义

在 Rust 中发布配置（release profiles）文件是预定义和可定制的。

Cargo 有两个主要的配置：运行 `cargo build` 时采用的 `dev` 配置和运行 `cargo build --release` 的 `release` 配置。

当项目的 Cargo.toml 文件中没有显式增加任何 `[profile.*]` 部分的时候，Cargo 会对每一个配置都采用默认设置。

`Cargo.toml`

```rust
[profile.dev]
opt-level = 0		// 控制 Rust 会对代码进行何种程度的优化 默认为0
					// 值从0到3，越高的优化级别需要更多的时间编译

[profile.release]
opt-level = 3		// 默认值为3
```

## 文档注释

Rust 有特定的用于文档的注释类型，通常被称为文档注释。它们会生成 HTML 文档，这些 HTML 展示公有 API 文档注释的内容。

文档注释使用三斜杠 `///`来格式化文本，文档注释位于需要文档的项的之前。

如在`src/lib.rs`文件中，一个函数的文档注释

```rust
/// Adds one to the number given.
///
/// # Examples
///
/// ```
/// let arg = 5;
/// let answer = my_crate::add_one(arg);
///
/// assert_eq!(6, answer);
/// ```
pub fn add_one(x: i32) -> i32 {
    x + 1
}
```

运行 `cargo doc` 可以生成这个文档注释的 HTML 文档，生成的 HTML 文档放入 `target/doc` 目录。

运行 `cargo doc --open` 会构建当前 crate 文档（同时还有所有 crate 依赖的文档）的 HTML 并在浏览器中打开。如图

<img src=".\img\image-20250717204450745.png" alt="image-20250717204450745" style="zoom:50%;" />

**其他常用文档注释部分**

- Panics：这个函数可能会 `panic!` 的场景。并不希望程序崩溃的函数调用者应该确保他们不会在这些情况下调用此函数
- Errors：如果这个函数返回 `Result`，此部分描述可能会出现何种错误以及什么情况会造成这些错误，这有助于调用者编写代码来采用不同的方式处理不同的错误
- Safety：如果这个函数使用 `unsafe` 代码，这一部分应该会涉及到期望函数调用者支持的确保 `unsafe` 块中代码正常工作的不变条件

注

1. `cargo test` 也会像测试那样运行文档注释中的示例代码

**注释包含项的结构**

`//!` 通常用于 crate 根文件（通常是 src/lib.rs）或模块的根文件为 crate 或模块整体提供文档。

例如在`src/lib.rs`中，`//!`最后一行之后没有任何代码，这些注释描述了整个 crate。

```rust
//! # My Crate
//!
//! `my_crate` is a collection of utilities to make performing certain
//! calculations more convenient.

/// Adds one to the number given.
// --snip--
```

如果运行 `cargo doc --open`，将会发现这些注释显示在 `my_crate` 文档的首页，位于 crate 中公有项列表之上

<img src=".\img\image-20250717205255293.png" alt="image-20250717205255293" style="zoom:50%;" />

**使用 `pub use` 导出便捷的公有 API**

可以选择使用 `pub use` 重导出（re-export）项来使公有结构不同于私有结构。

例如，假设创建了一个描述艺术概念的库 `art`。这个库中包含了一个有两个枚举 `PrimaryColor` 和 `SecondaryColor` 的模块 `kinds`，以及一个包含函数 `mix` 的模块 `utils`

`src/lib.rs`

```rust
//! # Art
//!
//! A library for modeling artistic concepts.

pub mod kinds {
    /// The primary colors according to the RYB color model.
    pub enum PrimaryColor {
        Red,
        Yellow,
        Blue,
    }

    /// The secondary colors according to the RYB color model.
    pub enum SecondaryColor {
        Orange,
        Green,
        Purple,
    }
}

pub mod utils {
    use crate::kinds::*;

    /// Combines two primary colors in equal amounts to create
    /// a secondary color.
    pub fn mix(c1: PrimaryColor, c2: PrimaryColor) -> SecondaryColor {
        // --snip--
    }
}
```

`cargo doc` 所生成的 crate 文档首页如图，注意 `PrimaryColor` 和 `SecondaryColor` 类型、以及 `mix` 函数都没有在首页中列出。必须点击 `kinds` 或 `utils` 才能看到它们。

<img src=".\img\image-20250717205800909.png" alt="image-20250717205800909" style="zoom:50%;" />

另一个依赖这个库的 crate 需要 `use` 语句来导入 `art` 中的项，这包含指定其当前定义的模块结构。

```rust
use art::kinds::PrimaryColor;			// 使用者不得不搞清楚其中模块结构
use art::utils::mix;

fn main() {
    let red = PrimaryColor::Red;
    let yellow = PrimaryColor::Yellow;
    mix(red, yellow);
}
```

为了从公有 API 中去掉 crate 的内部组织，可以使用 `pub use` 来重导出项到顶层结构。

`src/lib.rs`

```rust
//! # Art
//!
//! A library for modeling artistic concepts.

pub use self::kinds::PrimaryColor;
pub use self::kinds::SecondaryColor;
pub use self::utils::mix;

pub mod kinds {
    // --snip--
}

pub mod utils {
    // --snip--
}
```

现在此 crate 由 `cargo doc` 生成的 API 文档会在首页列出重导出的项以及其链接，这使得 `PrimaryColor` 和 `SecondaryColor` 类型和 `mix` 函数更易于查找。

<img src=".\img\image-20250717210158693.png" alt="image-20250717210158693" style="zoom:50%;" />

用户仍然可以看到并使用上图中的内部结构，或者可以使用更为方便的结构

```rust
use art::PrimaryColor;
use art::mix;

fn main() {
    // --snip--
}
```

对于有很多嵌套模块的情况，使用 `pub use` 将类型重导出到顶级结构很方便。

## 发布到Crates.io

[crates.io](https://crates.io/) 上的 crate 注册表会分发自定义包的源代码，因此它主要托管开源代码。

在 [crates.io](https://crates.io/) 上注册账号，查看位于 https://crates.io/me/ 的账户设置页面并获取 API token，然后运行 `cargo login` 命令

```shell
$ cargo login
abcdefghijklmnopqrstuvwxyz012345
```

这个命令会通知 Cargo 你的 API token 并将其储存在本地的 ~/.cargo/credentials 文件中。

如果已经有一个希望发布的 crate。在发布之前，需要在 crate 的 Cargo.toml 文件的 `[package]` 部分增加一些本 crate 的元数据。

```rust
[package]
name = "guessing_game"	// crates.io 上的 crate 名称遵守先到先得的分配原则,一旦某个 crate 名称被使用，其他人就不能再发布这个名称的 crate 了
version = "0.1.0"
edition = "2024"
description = "A fun game where you guess what number the computer has chosen."
license = "MIT"			// 用户可能在何种条款下使用该 crate 的 license

[dependencies]
```

运行 `cargo publish` 命令发布到 crate ，发布时请多加小心，因为发布是永久性的，对应版本不可能被覆盖，其代码也不可能被删除。

虽然你不能删除 crate 的历史版本，但是可以阻止任何将来的项目将它们加入到依赖中。

撤回某个版本会阻止新项目依赖此版本，不过所有现存此依赖的项目仍然能够下载和依赖这个版本。从本质上说，撤回意味着所有带有 Cargo.lock 的项目的依赖不会被破坏，同时任何新生成的 Cargo.lock 将不能使用被撤回的版本。

为了撤回一个版本的 crate，在之前发布 crate 的目录运行 `cargo yank` 并指定希望撤回的版本。

```shell
$ cargo yank --vers 1.0.1
    Updating crates.io index
        Yank guessing_game@1.0.1
```

也可以撤销撤回操作，并允许项目可以再次开始依赖某个版本，通过在命令上增加 `--undo`

```shell
$ cargo yank --vers 1.0.1 --undo
    Updating crates.io index
      Unyank guessing_game@1.0.1
```

## Cargo工作空间

Cargo 提供了一个叫工作空间（workspaces）的功能，它可以帮助管理多个相关的协同开发的包。

随着项目增长，考虑使用工作空间，将 crate 保持在工作空间中更易于协调 crate 的改变。

**创建工作空间**

工作空间是一系列共享同样的 Cargo.lock 和输出目录的包。

假如工作空间有一个二进制 crate 和一个库 crate。二进制 crate 会提供主要功能，并会依赖另一个库 crate，库 crate会提供 `add_one` 方法，这两个 crate 将会是相同工作空间的一部分。

在 add 目录中，创建 Cargo.toml 文件，用于配置整个整个工作空间。它不包含 `[package]` 部分，它以 `[workspace]` 部分作为开始，允许向工作区添加成员。

`Cargo.toml`

```rust
[workspace]
resolver = "2"
```

接下来，在add目录新建 `adder` 二进制 crate。

在工作空间中运行 `cargo new` 也会自动将新建包加入到工作空间 `Cargo.toml` 的 `[workspace]` 定义的 `members` 键中。

`Cargo.toml`

```rust
[workspace]
resolver = "3"
members = ["adder"]
```

到此为止，可以运行 `cargo build` 来构建工作空间。

**添加其他 crate**

同理创建另一个成员包 `add_one`。

目录结构

```shell
├── Cargo.lock
├── Cargo.toml
├── add_one
│   ├── Cargo.toml
│   └── src
│       └── lib.rs
├── adder
│   ├── Cargo.toml
│   └── src
│       └── main.rs
└── target
```

在 add_one/src/lib.rs 文件中，增加一个 `add_one` 函数。

`add_one/src/lib.rs`

```rust
pub fn add_one(x: i32) -> i32 {
    x + 1
}
```

为了二进制 `adder` 包依赖库 crate `add_one` 包，首先需要在 adder/Cargo.toml 文件中增加 `add_one` 作为路径依赖。

`adder/Cargo.toml`

```rust
[dependencies]
add_one = { path = "../add_one" }
```

接下来，在 `adder` crate 中使用（ `add_one` crate 中的）函数 `add_one`。

`adder/src/main.rs`

```rust
use add_one;

fn main() {
    let num = 10;
    println!("Hello, world! {num} plus one is {}!", add_one::add_one(num));
}
```

此时在顶层 add 目录中可以运行 `cargo build` 来构建工作空间。

为了在顶层 add 目录运行二进制 crate，可以通过 `-p` 参数和包名称来运行指定二进制 crate

```shell
$ cargo run -p adder
    Finished `dev` profile [unoptimized + debuginfo] target(s) in 0.00s
     Running `target/debug/adder`
Hello, world! 10 plus one is 11!
```

**在工作空间中依赖外部包**

工作空间只在根目录有一个 Cargo.lock，而不是在每一个 crate 目录都有 Cargo.lock，这使得工作空间中的所有 crate 都使用相同的依赖，意味着其中的 crate 都是相互兼容的。

即使某个外部包被用于工作空间的某处，也不能在其他 crate 中使用它，除非也在它们的 Cargo.toml 中加入这个外部包。

如果外部包是兼容的，cargo 只会使用一个，如果外部包版本不兼容，则会使用两个不同的版本。

**为工作空间增加测试**

`add_one/src/lib.rs`

```rust
pub fn add_one(x: i32) -> i32 {
    x + 1
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn it_works() {
        assert_eq!(3, add_one(2));
    }
}
```

在顶级 add 目录运行 `cargo test`会运行工作空间中所有 crate 的测试。

也可以选择运行工作空间中特定 crate 的测试，通过在根目录使用 `-p` 参数并指定希望测试的 crate 名称。

如果选择向 [crates.io](https://crates.io/) 发布工作空间中的 crate，则需要分别发布每个 crate，可以通过 `-p` 参数来指定期望发布的 crate。

# 猜数游戏

- 生成一个 1-100 间的随机数，提示玩家输入一个猜测，程序提示偏差

```rust
use std::io;                // 使用std库中的io库
use std::cmp::Ordering;     // 枚举库
use rand::Rng;              // 随机数库
// rust默认会将prelude导入每个程序作用域中

// 声明main函数
fn main() {

    println!("--- Guessing Number ---");
    /* ------------------------------ */
    // 在rust中所有的变量默认都是不可变的
    // 使用关键词mut，将变量声明为可变变量
    // let mut foo = 1;                     // 声明可变变量foo
    // let bar = foo;                       // 声明不可变变量bar并绑定foo

    /* ------------------------------ */
    // thread_rng() 使用rand库生成随机数
    // gen_range() Rng接口中的函数，生成指定范围内的随机数 1-100
    // 返回值类型 i32 or u32等类型，隐式转换，与下文有关
    let secret_number = rand::thread_rng().gen_range(1..=100);
    println!("----- The secret number is {} -----",secret_number);

    // 循环
    loop {
        println!("guess a number:");
        /* ------------------------------ */
        // String由标准库提供，UTF-8编码，可以动态扩展
        // ::new是String类型的关联函数，创建一个空字符串
        let mut guess = String::new();

        /* ------------------------------ */
        // read_line() 获取用户的输入并存储在字符串（guess）中
        // 字符串需可变，因此使用&mut guess，获取可变引用
        // 引用在rust默认也是不可变的
        // 返回值 io::Result，即 ok: Ok(n), error: Err(e)
        // expect() 获取io::Result的Ok值，如果为Err则中断程序
        io::stdin().read_line(&mut guess).expect("Failed to read line");

        println!("You guessed: {}", guess);     		// {} 占位符

        /* ------------------------------ */
        // rust允许使用同名的新变量来隐藏原来同名的旧变量 常用于类型转换
        // let guess:u32显式声明变量类型为u32
        // trim()：去掉两端空白、回车
        // parse()：字符串解析为数值类型，返回值为Result
        let guess:u32 = match guess.trim().parse() {
            Ok(num) => num,                       		// 输入数字
            Err(_) => {                                 // 通配符 不关心错误信息
                println!("please guess a number!");
                continue;							    // 重回循环
            }
        };

        /* ------------------------------ */
        // match：类似于switch
        // cmp：比较，返回类型为Ordering
        match guess.cmp(&secret_number){
            Ordering::Less => println!("Too small!"),   // 小于
            Ordering::Greater => println!("Too big"),   // 大于
            Ordering::Equal => {                        // 等于
                println!("You win!");
                break;                   				// 跳出循环
            }
        }
    }
}
```

# 基本概念

## 语言类型

Rust是静态强类型编译语言。

<img src=".\img\image-20250702140940481.png" alt="image-20250702140940481" style="zoom:50%;" />

- 静态语言：所有数据类型在编译时已知
- 动态语言：在运行时才知道数据的类型

<img src=".\img\image-20250702141212262.png" alt="image-20250702141212262" style="zoom:50%;" />

- 强类型语言：对数据类型的赋值进行规则约束
- 弱类型语言：对数据类型的赋值进行约束少/无约束

<img src=".\img\image-20250702141453107.png" alt="image-20250702141453107" style="zoom:50%;" />

- 编译（compile）/解释（interpret）语言

<img src=".\img\image-20250702142219893.png" alt="image-20250702142219893" style="zoom:50%;" />

## 堆栈区别

- 栈内存：后进先出，访问速度快，但所有数据大小须固定已知
- 堆内存：访问较慢，但允许存储未知大小的数据

<img src=".\img\image-20250702143215532.png" alt="image-20250702143215532" style="zoom:50%;" />

## 变量可变性

Rust 编译器声明一个变量时，默认为不可变的。

- 声明变量使用`let`关键字
- 默认情况下，变量是不可变的
- 变量前加`mut`关键字，使得变量是可变的

```rust
fn main() {
    // let x = 5;							// 变量x为不可变变量
    let mut x = 5;							// 声明为可变变量
    println!("The value of x is: {x}");
    x = 6;
    println!("The value of x is: {x}");
}
```

## 常量

类似于不可变变量，常量 (constants) 是绑定到一个名称的不允许改变的值。

- 不可使用 `mut` 
- 声明常量使用 `const` 关键字，且类型必须被标注
- 常量可以在任何作用域内进行声明，包括全局作用域
- 常量只可以绑定到常量表达式
- 命名规范：Rust中常量使用全大写，单词间下划线分开

```rust
const MAC_POINTS:u32 = 100_000;
```

## 遮蔽

Rust中可以定义一个与之前变量同名的新变量，称之为第一个变量被第二个遮蔽。此时任何使用该变量名的行为中都会视为是在使用第二个变量，直到第二个变量也被遮蔽或第二个变量的作用域结束。

好处：降低了冗余的变量，如size_num、size_str

```rust
fn main() {
    let x = 5;
    let x = x + 1;
    {
        let x = x * 2;
        println!("The value of x in the inner scope is: {x}");
    }
    println!("The value of x is: {x}");
}
```

输出

```shell
The value of x in the inner scope is: 12
The value of x is: 6
```

**遮蔽与mut的区别**

- 遮蔽需要使用 `let` 关键字
- 遮蔽实际上创建了一个新变量，可以改变值的类型，并且复用这个名字

```rust
let spaces = "   ";
let spaces = spaces.len();		// ✅ 遮蔽 
```

```rust
let mut spaces = "   ";
spaces = spaces.len();			// ❌
```

## 数据类型

在 Rust 中有两类数据类型：标量类型（scalar）和复合类型（compound）。

Rust 是 静态类型（statically typed）语言，也就是说在编译时就必须知道所有变量的类型。

- 编译器通常能够根据值及其使用方式推断出它的具体类型
- 针对可能类型比较多的情况，就必须添加类型的标注，否则编译就会报错

```rust
let guess:u32 = "42".parse().expect("Not a number");
```

### 标量类型

标量（scalar）类型代表一个单独的值。Rust 有四种基本的标量类型

- 整型
- 浮点型
- 布尔类型
- 字符类型

**整型**

- 无符号整数类型u开头
- 有符号整数类型i开头
- isize和usize大小为当前计算机架构的字，64 位架构上它们是 64 位的，32 位架构上它们是 32 位的

|   长度   | 有符号  | 无符号  |
| :------: | :-----: | :-----: |
|  8-bit   |  `i8`   |  `u8`   |
|  16-bit  |  `i16`  |  `u16`  |
|  32-bit  |  `i32`  |  `u32`  |
|  64-bit  |  `i64`  |  `u64`  |
| 128-bit  | `i128`  | `u128`  |
| 架构相关 | `isize` | `usize` |

可以使用以下任何一种形式编写数字字面值，同时也允许使用 `_` 做为分隔符以方便读数，例如`1_000`，它的值与 `1000` 相同。

|          数字字面值           |     例子      |
| :---------------------------: | :-----------: |
|       Decimal (十进制)        |   `98_222`    |
|        Hex (十六进制)         |    `0xff`     |
|        Octal (八进制)         |    `0o77`     |
|        Binary (二进制)        | `0b1111_0000` |
| Byte (单字节字符)(仅限于`u8`) |    `b'A'`     |

- 除了byte类型外，所有的数值字面值都允许使用类型后缀，如57u8
- 整数默认类型为i32

整数溢出

- 调式模式下，Rust会检查整数溢出
- 发布模式下，Rust不会检查

- 如果发生溢出，Rust会执行环绕操作，如u8的256变为0

**浮点型**

均为有符号的。

- f32，单精度
- f64，双精度，默认类型

数值运算

Rust 中的所有数字类型都支持基本数学运算：加法、减法、乘法、除法和取余。

```rust
fn main() {
    // addition
    let sum = 5 + 10;

    // subtraction
    let difference = 95.5 - 4.3;

    // multiplication
    let product = 4 * 30;

    // division
    let quotient = 56.7 / 32.2;
    let truncated = -5 / 3; // 结果为 -1

    // remainder
    let remainder = 43 % 5;
}
```

注

1. 数除法会向零舍入到最接近的整数

**布尔类型**

Rust 中的布尔类型有两个可能的值：`true` 和 `false`，Rust 中的布尔类型使用 `bool` 表示，0、1不等同于true和false。

```rust
fn main() {
    let t = true;

    let f: bool = false; 			// with explicit type annotation
}
```

**字符类型**

用单引号声明 `char` 字面值，而声明字符串字面值使用双引号。

- char类型大小占用4字节，并代表了一个unicode标量值，可以表示中日韩文字、emoji表情等

```rust
fn main() {
    let c = 'z';
    let z: char = 'ℤ';				 // with explicit type annotation
    let heart_eyed_cat = '😻';
}
```

### 复合类型

复合类型可以将多个值放在一个类型里，Rust 有两个原生的复合类型：元组（tuple）和数组（array）。

**元组**

元组是一个将多个不同类型的值组合进一个复合类型。元组长度固定，一旦声明，其长度不变。

元组中的每一个位置都有一个类型，而且这些不同值的类型也不必是相同的。

声明

```rust
let tup:(i32, f64, u8) = (500, 6.4, 1);
```

通过模式匹配来获取元素值

```rust
let tup = (500, 6.4, 1);
let (x, y, z) = tup;
println!("The value of y is: {y}");
```

使用点标记法访问元素

```rust
let x: (i32, f64, u8) = (500, 6.4, 1);
let five_hundred = x.0;					// 元组的第一个索引值是 0
let six_point_four = x.1;
let one = x.2;
```

不带任何值的元组叫做 单元（unit）元组，写作 `()`，表示空值或空的返回类型。如果表达式不返回任何其他值，则会隐式返回单元值。

**数组**

数组中的每个元素的类型必须相同且长度固定。

声明

```rust
let a = [1, 2, 3, 4, 5];				// 隐式声明

let a: [i32; 5] = [1, 2, 3, 4, 5];		// 显式声明数组类型和长度

let a = [3; 5];							// 等价于let a = [3, 3, 3, 3, 3]; 
```

访问数组元素

```rust
let a = [1, 2, 3, 4, 5];

let first = a[0];
let second = a[1];
```

注

1. 如果访问的索引超出了数组的范围，那么编译不一定通过，但运行时若超出则panic，导致程序退出。通过立即退出而不是允许内存访问并继续执行，避免了程序访问无效的内存，提高了安全性

## 函数 

- 声明函数使用`fn`关键字
- 命名规范：所有字母都是小写的，单词之间下划线分开
- 不关心函数声明顺序，只要在作用域内可任意调用

### 参数

- 在函数声明里，必须声明每个参数的类型

```rust
fn main() {
    print_labeled_measurement(5, 'h');
}

fn print_labeled_measurement(value: i32, unit_label: char) {
    println!("The measurement is: {value}{unit_label}");
}
```

### 语句和表达式

函数体由一系列语句和一个可选的表达式构成。

- 语句是执行一些操作但不返回值的指令，结尾有分号
- 表达式会计算产生一个值，表达式的结尾没有分号

函数定义也是语句，但调用函数并不是语句。

 Rust中不允许`x = y = 6;`这样的写法。

```rust
fn main(){				// ✅
    let x = 5;

    let y = {			// 块为一个表达式
        let x = 1;		// 语句
        x + 3			// 表达式		
    };

    println!("The value of y is {}",y); 
}

// y = 4
```

```rust
fn main(){				// ❌
    let x = 5;

    let y = {			// 相当于 let y = ();
        let x = 1;		// 语句
        x + 3;			// 语句
    };

    println!("The value of y is {}",y); 
}

// 无法打印
```

### 函数的返回值

- 在`->`符号后边声明函数返回值的类型，不可为返回值命名
- Rust中函数隐式的返回最后的表达式，若无显式返回值则返回()
- 使用 `return` 关键字，可从函数中提前返回

```rust
fn my_function(x:i32) -> i32{				// ✅
    x + 5 									// 注意为表达式，无需 ;
// or return x + 5;
// or return x + 5
}

fn main(){
    let x = my_function(6);

    println!("The value of x is {}",x);		
}

// x = 11
```

```rust
fn main() {									
    let x = plus_one(5);

    println!("The value of x is: {x}");
}

fn plus_one(x: i32) -> i32 {
    x + 1;									//  ❌ 语句 此函数返回值为()
}
```

## 条件语句

条件必须是bool类型。

- if
- else
- else if

```rust
fn main() {
    let number = 3;

    if number {								// ❌ 非 bool 类型
        println!("number was three");
    }
}
```

```rust
fn main(){									// ✅
    let number = 6;

    if number % 4 == 0 {					// 判断必须为 bool 类型
        println!("the number is divisible by 4");
    } else if number % 3 == 0 {
         println!("the number is divisible by 3");
    } else if number % 2 == 0 {
         println!("the number is divisible by 2");
    } else{
         println!("the number is not  divisible by 4, 3, or 2");
    } 
}
```

注

1. 如果使用了多于一个`else if`，最好使用`match`重构代码

因为`if`是一个表达式，所以可以用`let`修饰。

```rust
fn main(){
    let condition = true;
    
    let number = if condition { 5 } else { 6 };			//{ "6" };  则报错
    
    println!("the value of number is {}",number);
}
```

注

1. 代码块的值是其最后一个表达式的值，而数字本身就是一个表达式
2. `if` 和 `else` 分支的值类型必须相同

## 循环语句

### loop

loop会反复执行一块代码。可以在循环里面使用`break`关键字来停止循环。

- `break`：停止循环
- `continue`：跳过本次迭代

```rust
fn main(){
    let mut conter = 0;

    let result = loop{
        conter += 1;

        if conter != 10{
            continue;
            println!("The result is {}", conter);			// 无法执行
        } else {
            break conter * 2
      //    break conter * 2;
        }
    };

    println!("The result is {}", result);					// result = 20
}
```

### while

- 每次执行循环体之前都要判断一次条件

```rust
fn main() {
    let mut number = 3;

    while number != 0 {
        println!("{number}!");

        number -= 1;
    }

    println!("LIFTOFF!!!");
}
```

### for

- 更简洁紧凑，可以针对集合中每个元素执行一些代码

```rust
fn main() {
    let a = [10, 20, 30, 40, 50];

    for element in a {
        println!("the value is: {element}");		// 顺序打印
    }
    
    for idx in 2..=4 {
        println!("the value is: {}" , a[idx]);		// 打印特定
    }
}
```

**Range**

- 标准库提供，用来生成从一个数字开始到另一个数字之前结束的所有数字的序列
- rev 方法可以反转 Range

```rust
fn main() {
    for number in (1..4).rev() {		// 左闭右开
        println!("{number}!");
    }
    println!("LIFTOFF!!!");
}
```

输出

```shell
3!
2!
1!
LIFTOFF!!!
```

## 注释

参考C语言。

# 所有权

- 安全：程序中没有未定义的行为
- 未定义行为：结果不可预测且未被编程语言指定的情况

所有权是 Rust 用于如何管理内存的一组规则。使得Rust无需GC（垃圾回收）、无需手动就可以保证内存安全。

通过所有权系统管理内存，编译器在编译时会根据一系列的规则进行检查。如果违反了任何这些规则，程序都不能编译。在运行时，所有权系统的任何功能都不会减慢程序的运行。

**所有权规则**

1. Rust 中的每一个值都有一个所有者（owner）
2. 值在任一时刻有且只有一个所有者
3. 当所有者离开作用域，这个值将被丢弃

**变量作用域**

内存在拥有它的变量离开作用域后就被自动释放。

拥有数据所有者在离开作用域后自动清除其数据的功能意味着你无须额外编写和调试相关的控制代码。

当变量离开作用域，Rust 为我们自动调用一个特殊的函数drop。

```rust
fn main(){
    // s 不可用
    let s = "hello"						// s可用
  	// 可对 s 进行操作
}
// s 作用于到此结束 不再可用 
```

**权限**

变量对其数据有三种权限

- 读（R）：数据可以被复制到另一个位置
- 写（W）：数据可以被修改
- 拥有（O）：数据可以被移动或释放

这些权限仅在编译器内部存在，运行时并不存在。

默认情况下变量对数据拥有（RO）权限，mut关键字还可以具有（W），而引用可以临时移除这些权限。

## 内存与分配

**栈数据交互方式：拷贝（copy）**

```rust
fn main(){
    let x = 5;
    let y = x;

    println!("{} {}",x, y);				// 正常使用
}
```

- 如果一个类型实现了 `Copy trait`，那么一个旧的变量在将其赋值给其他变量后仍然有效
- 任何一组简单标量值的组合都可以实现 `Copy`，如整数类型、字符类型、均为标量的元组等
- 任何需要分配内存或某种资源的都不是`Copy`

**堆数据交互方式：移动（move）**

```rust
let s1 = String::from("hello");
let s2 = s1;
```

<img src=".\img\image-20250227142402379.png" alt="image-20250227142402379" style="zoom:50%;" />

- 在栈上复制了一份，但并没有复制指针指向的堆上的数据
- 由于变量离开作用域时，Rust自动调用drop函数释放变量值所在的heap内存，为了避免二次释放，保证内存安全，Rust会让s1失效

```rust
fn main(){
    let s1 = String::from("hello");
	let s2 = s1;
    
    println!("{} {}",s1, s2);				// ❌ 无法使用 s1   
}
```

**堆数据交互方式：克隆（clone）**

在栈和堆上的数据都被复制。

```rust
fn main(){
    let s1 = String::from("hello");
    let s2 = s1.clone();

    println!("{} {}",s1, s2);				// ✅ 能正确使用s1
}
```

<img src=".\img\image-20250227143019588.png" alt="image-20250227143019588" style="zoom:50%;" />

## 所有权与函数

向函数传递值可能会移动或者复制，就像赋值语句一样。

```rust
fn main() {
    let s = String::from("hello");  	 // s 进入作用域

    takes_ownership(s);           	  	 // s 的值移动到函数里
                                  	  	 // s不再有效

    let x = 5;                     	 	 // x 进入作用域

    makes_copy(x);                	 	 // x 是 Copy 的
                                  	     // 后面可继续使用 x
}

fn takes_ownership(some_string: String) { // some_string 进入作用域
    println!("{some_string}");
} // some_string 移出作用域并调用 `drop` 方法
  // 占用的内存被释放

fn makes_copy(some_integer: i32) { // some_integer 进入作用域
    println!("{some_integer}");
} // some_integer 移出作用域
```

**返回值与作用域**

- 函数返回值的过程中同样也会发生所有权的转移
- 把一个值赋给其他变量时就会发生移动
- 当一个包含堆数据的变量离开作用域时，它的值会被drop函数清理，除非数据的所有权转移到了另一个变量

```rust
fn main() {
    let s1 = gives_ownership();         // 将返回值转移给 s1

    let s2 = String::from("hello");    
    							
    let s3 = takes_and_gives_back(s2);  // s2 被移动 不再可用
                                        // takes_and_gives_back将返回值移给 s3
} 

fn gives_ownership() -> String {             
    let some_string = String::from("yours"); 
    some_string                              
}

fn takes_and_gives_back(a_string: String) -> String {                                 
    a_string  
}
```

让函数使用值，但不获的所有权

```rust
fn main(){

    let s1 = String::from("hello");

    let (s2, len) = calculate_length(s1);

    println!("The length of {} is {}",s2,len);                 // s1失效
}

fn calculate_length(s: String) ->(String, usize){              // usize:无符号整数类型 （字）
    let length = s.len();
    (s,length)												   // 返回一个元组
}
```

## 引用与借用

允许引用某些值而不取得其所有权，`&`符号表示。

```rust
fn main(){
    let s1 = String::from("hello");
    let len = calculate_length(&s1);            		// 传递s1的引用

    println!("The length of {} is {}",s1,len);          // s1可用
}

fn calculate_length(s: &String) -> usize{               // usize:无符号整数类型 （字）
    let length = s.len();
    length
}	// 这里，s 离开了作用域。但因为它并不拥有引用值的所有权，
  	// 所以什么也不会发生
```

<img src=".\img\image-20250227161337703.png" alt="image-20250227161337703" style="zoom:60%;" />

将创建一个引用的行为称为借用（borrowing）。

引用实现了Copy，因此无法被移动

```rust
fn main() {
    let s = String::from("hello");
    let r1 = &s;
    let r2 = r1; 						// r1 被复制，不是移动

    println!("{:?}", r1); 				// ✅ 仍然可以使用 r1
    println!("{:?}", r2);
}
```

不能移动通过引用访问的值

```rust
fn main() {
    let s = String::from("hello");
    let r = &s;
    let s2 = *r; 						// ❌ 错误：不能移动`*r`，因为`r`不拥有`s`
}
```

可以定义引用的引用

```rust
fn main() {
    let s = String::from("hello");
    let r1 = &s;
    let r2 = &r1; 						// ✅ r2 类型为 &&String

    println!("{:?}", r1); 				
    println!("{:?}", r2);
}
```

**引用默认不可变**

```rust
fn main() {
    let s = String::from("hello");

    change(&s);							// ❌ 引用不可变
}

fn change(some_string: &String) {
    some_string.push_str(", world");
}
```

正如变量默认是不可变的，引用也一样，（默认）不允许修改引用的值。

**可变引用**

使用`mut`关键字修饰引用，所引用的变量也必须为可变的。

```rust
fn main(){
    let mut s1 = String::from("hello");   			 // ✅ 可变变量
    let len = calculate_length(&mut s1);             // 传递s1的可变引用

    println!("The length of {} is {}",s1,len);       
}

fn calculate_length(s: &mut String) -> usize{        // usize:无符号整数类型 （字）
    s.push_str(". world");
    s.len()
}
```

- 在特定作用域内，对某一块数据，只能有一个可变引用
- 不可以同时拥有一个可变引用和一个不可变引用
- 多个不可变引用是允许的
- 引用必须有效

注

1. 无法通过引用来获取所有权
2. 不可变引用的生命周期内无法修改

```rust
fn main(){
    let mut s = String::from("hello");    // 可变变量
    let s1 = &mut s;						
    let s2 = &mut s;					  // ❌ 只能有一个可变引用

    println!("{} {}",s1,s2);       
}
```

```rust
fn main(){
    let mut s = String::from("hello");    // 可变变量
    let s1 = &s;				
	let s2 = &s;	
    let s3 = &mut s;					  // ❌ 
    println!("{} {} {}",s1, s2, s3);
}
```

一个引用的作用域从声明的地方开始一直持续到最后一次使用为止

```rust
fn main() {
    let mut s = String::from("hello");

    let r1 = &s; // ✅ 没问题
    let r2 = &s; // 没问题
    println!("{r1} and {r2}");
    // 此位置之后 r1 和 r2 不再使用

    let r3 = &mut s; // 没问题
    println!("{r3}");
}
```

如果要使用多个可变引用，可以在不同作用域中

```rust
fn main(){
    let mut s = String::from("hello");    // ✅ 可变变量
    {
        let s1 = &mut s;
    }
    let s2 = &mut s;
}
```

- 好处是在编译时防止数据竞争
- 以下三种行为下会发生数据竞争
  - 两个或更多指针同时访问同一数据
  - 至少有一个指针被用来写入数据
  - 没有同步数据访问的机制

**引用必须有效**

悬空指针：一个指针引用了内存的某个地址，而这块内存可能已被释放或其他人使用。

在 Rust 中编译器确保引用永远也不会变成悬垂状态：当拥有一些数据的引用，编译器会确保数据不会在其引用之前离开作用域。

```rust
fn main(){
    let r = my_function();              // ❌ 引用无效
}

fn my_function() -> &String{
    let s = String::from("hello");
    &s
}										// s被释放，理应无法访问
```

## 切片

切片（slice）允许引用集合中一段连续的元素序列，而不用引用整个集合。slice 是一种引用，所以它不拥有所有权。

### 字符串 slice

字符串 切片（string slice）是 `String` 中一部分值的引用。

```rust
fn main(){
    let  s = String::from("hello world");

    // 左闭右开
    let hello = &s[0..5];	// 等价于 &s[..5]
    let world = &s[6..11];	// 等价于 &s[6..] &s[6..s.len()]

    let whole = &s[..];		// 不可变引用
    println!("{}",whole);
}
```

<img src=".\img\image-20250306110254765.png" alt="image-20250306110254765" style="zoom:50%;" />

- 字符串切片就是指向字符串中一部分内容的(不可变)引用，因此不可变
- 字符串切片的范围索引必须在有效的UTF-8字符边界内，如果尝试从一个多字节的字符中创建字符串切片，程序会报错

切片前：`word` 的索引与 `s` 中的数据不再同步问题

```rust
fn main() {
    let mut s = String::from("hello world");
    let word = first_word(&s); // word 的值为 5
    s.clear(); 										// 这清空了字符串，使其等于 ""
    // word 在此处的值仍然是 5，
    // 但是没有更多的字符串让我们可以有效地应用数值 5。word 的值现在完全无效！
}

fn first_word(s: &String) -> usize {
    let bytes = s.as_bytes();						// 转化为字节数组
    for (i, &item) in bytes.iter().enumerate() {		
        // iter 方法返回集合中的每一个元素
        // enumerate 包装了 iter 的结果(元组) 第一个元素是索引，第二个元素是集合中元素的引用
        if item == b' ' {
            return i;
        }
    }
    s.len()
}
```

切片后：程序报错，不可变引用的生命周期内无法修改

```rust
fn main() {
    let mut s = String::from("hello world");

    let word = first_word(&s);							// 不可变引用

    s.clear(); 											// ❌ 无法修改不可变引用&s

    println!("the first word is: {word}");
}

fn first_word(s: &String) -> &str {						// 返回切片
    let bytes = s.as_bytes();

    for (i, &item) in bytes.iter().enumerate() {
        if item == b' ' {
            return &s[0..i];				
        }
    }
    &s[..]
}
```

**字符串字面值**

```rust
let s = "Hello, world!";   		// s 的类型是 &str 是一个不可变引用
```

**字符串 slice 作为参数**

实际开发时会采用`&str`作为参数类型，因为能够同时接收String 和 &str 两种类型的参数

```rust
fn main(){
    let my_string = String::from("hello world");
    // first_word`适用于 String（的 slice），部分或全部 ✅
    let word = first_word(&my_string[0..6]);
    let word = first_word(&my_string[..]);
    let wordindex = first_world(&my_string);               // 字符串

    let my_string_literal = "hello world";
    // first_word 适用于字符串字面值，部分或全部
    let word = first_word(&my_string_literal[0..6]);
    let word = first_word(&my_string_literal[..]);
    let wordindex = first_world(my_string_literal);        // 字符串切片
}

fn first_world(s: &str) -> &str {
    let bytes = s.as_bytes();    					       // 字节数组

    // iter() 创建迭代器 一次返回数组中的每个元素
    // enumerate() 将 iter() 结果作为元组进行包装
    // i：索引值
    // &itme：对应数值
    for(i, &item) in bytes.iter().enumerate(){
        if item == b' '{
            return &s[..i];
        }
    }
    &s[..]
}
```

### 其他类型切片

引用数组的一部分

```rust
fn main(){
    let a = [1, 2, 3, 4, 5];
    let slice = &a[1..3];    // 类型为 &[i32]
}
```

# 结构体

不同于元组，结构体需要命名各部分数据以便能清楚的表明其值的意义。结构体不需要依赖顺序来指定或访问实例中的值。

- 命名规范：首字母大写

## 定义

- 使用`struct`关键字
- 实例化时需为每个字段指定具体值，但无需按顺序进行指定
- 访问时可以使用点号
- Rust 并不允许只将某个字段标记为可变，一旦struct的实例是可变的，那么实例中的所有字段都是可变的

```rust
struct User{										// 声明结构体
    username: String,
    email: String,
    sign_in_count: u64,
    active: bool,									// 注意逗号
}

fn main(){
    let mut user1 = User{							// 实例化结构体 
        email: String::from("123@163.com"),			// 可不按顺序，但需都进行赋值
        username: String::from("Nikky"),
        active: true,
        sign_in_count: 556,
    };
    
    user1.email = String::from("anotheremail@example.com");
}
```

结构体可作为函数的返回值

```rust
fn build_user(eamil: String, username: String) -> User {
    User{
        email: email,			
        username: username,
        active: true,
        sign_in_count: 1,
    }  
/* 参数名与字段名都完全相同可以使用字段初始化简写语法 */
    User {
        active: true,
        username,
        email,
        sign_in_count: 1,
    }
}
```

可以基于某个结构体实例来创建一个新实例

```rust
fn main() {
    
/* 简化前 */
    
    let user2 = User {
        active: user1.active,
        username: user1.username,
        email: String::from("another@example.com"),
        sign_in_count: user1.sign_in_count,
    };
}

/* 简化后 */

let user2 = User{								
    email: String::from("123@163.com"),			
    ..user1										// 其余默认值与user1实例相同
};												// 发生移动，user1不可用

let user2 = User{								
    email: String::from("123@163.com"),			
    username: String::from("Nikky"),
    ..user1										// 其余默认值与user1实例相同
};												// 未发生移动，user1可用

```

注

1. 如果更新中含有未实现copy的字段，那么结构体实例发生了移动

**元组结构体**

可以定义类似元组的结构体，称为元组结构体（Tuple struct）。

元组结构体有着结构体名称提供的含义，但没有具体的字段名，只有字段的类型。

```rust
fn main() {
    struct Color(i32, i32, i32);
    struct Point(i32, i32, i32);

    let black = Color(0,0,0);
    let origin = Point(0,0,0);		// black和origin是不同类型，无法进行赋值

    let Point(x, y, z) = origin;	// 将 origin 的值解构到名为 x、y 和 z 的变量中
    println!("{}",black.0); 		// 也可以通过.进行访问
}
```

**类单元结构体**

可以定义没有任何字段的结构体，称为类单元结构体（Unit-like-struct）。

```rust
struct AlwaysEqual;					// 类单元结构体

fn main() {
    let subject = AlwaysEqual;
}
```

**所有权**

结构体默认是移动

```rust
struct MyStruct {
    value: i32,
}

fn main() {
    let s1 = MyStruct { value: 42 };
    let s2 = s1; 						// s1 被移动到 s2

    // println!("{:?}", s1); 			// ❌ 编译错误：s1 已经被移动
    println!("{:?}", s2.value); 		// ✅ 可以使用 s2
}
```

只有所有字段都实现了 `Copy` 的结构体才能整体实现 `Copy`

```rust
#[derive(Copy, Clone)]

struct MyStruct {
    value: i32,
}

fn main() {
    let s1 = MyStruct { value: 42 };
    let s2 = s1; 				// s1 被复制到 s2，s1 仍然有效

    println!("{:?}", s1.value); // ✅ 仍然可以使用 s1
    println!("{:?}", s2.value); // ✅ 也可以使用 s2
}
```

不能为包含非 `Copy` 字段的结构体实现 `Copy`，但可以通过实现 `Clone` trait 来手动复制它

```rust
#[derive(Copy, Clone)] 			// ❌ 编译错误
struct MyStruct {
    name: String,
}

#[derive(Clone)]
struct MyStruct {
    name: String,
}

fn main() {
    let s1 = MyStruct {
        name: String::from("Alice"),
    };
    let s2 = s1.clone(); 		// 显式克隆

    println!("{}", s1.name); 	// ✅ 仍然可以使用 s1
    println!("{}", s2.name); 	// ✅ 也可以使用 s2
}
```

结构体里面也可以存放引用，但需要生命周期。

```rust
struct User {					// ❌ 编译错误
    active: bool,
    username: &str,
    email: &str,
    sign_in_count: u64,
}

fn main() {
    let user1 = User {
        active: true,
        username: "someusername123",
        email: "someone@example.com",
        sign_in_count: 1,
    };
}
```

## 示例

计算长方形面积。

**普通版**

```rust
fn main(){
    let w = 30;
    let l = 50;
    println!("{}",area(w,l));
}

fn area(width: i32, length: i32) -> i32{
    width * length
}
```

**struct版本**

```rust
#[derive(Debug)]							// 增加属性

struct Rectangle{
    width: u32,
    length: u32,
}

fn main(){
    let rect = Rectangle{
        width: 30,
        length: 50,
    };

    println!("{}",area(&rect));
    //println!("{}",rect);					// 为实现display
    println!("{:?}",rect);         			// 显示输出 
    println!("{:#?}",rect);					// 结构更加清晰 
    dbg!(&rect1);							// 打印到标准错误控制台流
}
fn area(rectangle: &Rectangle) -> u32{
    rectangle.width * rectangle.length
}
```

注

1. `#[derive(Debug)]` 是一个属性宏，用于自动生成 Debug 特性的实现。允许通过 `{:?}` 或 `{:#?}` 格式化占位符打印自定义类型的实例，从而方便调试程序

## 方法

- 方法和函数类似
- 不同的是方法是在struct（或者是枚举或 trait 对象）的上下文定义的，且第一个参数总是self

```rust
#[derive(Debug)]

struct Rectangle{
    width: u32,
    length: u32,
}

impl Rectangle{							// struct上下文
    fn area(&self) -> u32{
        self.width * self.length
    }
}
fn main(){
    let rect = Rectangle{
        width: 30,
        length: 50,
    };

    println!("{}",rect.area());
}

```

注

1. 每个结构体都允许拥有多个 `impl` 块
2. 方法的第一个参数必须有一个名为 `self` 的`Self` 类型的参数
3. 方法可以选择获得 `self` 的所有权，通过使用引用或不使用引用来实现
4. 当获得`self`所有权时，注意是否发生移动

**自动引用和解引用**

Rust 并没有一个与 `->` 等效的运算符；相反，Rust 有一个叫自动引用和解引用的功能，即根据情况自动添加`&`、`&mut`或`*`。

方法调用时自动引用和解引用——Rust 可以明确地计算出方法是仅仅读取（`&self`），做出修改（`&mut self`）或者是获取所有权（`self`）

**带多个参数的方法**

```rust
#[derive(Debug)]

struct Rectangle{
    width: u32,
    length: u32,
}
impl Rectangle{
    fn area(&self) -> u32{
        self.width * self.length
    }

    fn can_hold(&self, other: &Rectangle) -> bool{
        self.width > other.width && self.length > other.length
    }
}
fn main(){
    let rect1 = Rectangle{
        width: 30,
        length: 50,
    };

    let rect2 = Rectangle{
        width: 20,
        length: 40,
    };

    let rect3 = Rectangle{
        width: 40,
        length: 60,
    };

    println!("{}",rect1.can_hold(&rect2));
    println!("{}",rect1.can_hold(&rect3));
}
```

## 关联函数

在`impl`块里定义不把self作为第一个参数，称为关联函数。不是方法的关联函数经常被用作返回一个结构体新实例的构造函数。

- 通过`::`符号进行调用

```rust
#[derive(Debug)]

struct Rectangle{
    width: u32,
    length: u32,
}

impl Rectangle{
    fn square(size: u32) -> Self{	// 关键字Self代指Rectangle
        Self{
            width: size,
            length: size,
        }
    }
}

fn main(){
    let s = Rectangle::square(20);
    println!("{:#?}",s);
}
```

# 枚举

## 定义

枚举类型可以枚举出所有可能的值，去声明某个值是一个集合中的一员。

```rust
enum IpAddrKind {
    V4,					// 枚举的变体
    V6,					// 枚举的变体
}
```

注

1. 枚举值所需空间等于储存其最大变体的空间大小

**枚举实例化**

枚举的成员位于其标识符的命名空间中，并使用两个冒号分开

```rust
let four = IpAddrKind::V4;
let six = IpAddrKind::V6;
```

每一个定义的枚举成员的名字可以变成一个构建枚举的实例的函数。

```rust
/* 简化前 */

enum IpAddrKind {
    V4,
    V6,
}

struct IpAddr {
    kind: IpAddrKind,
    address: String,
}

let home = IpAddr {
    kind: IpAddrKind::V4,					// 枚举值
    address: String::from("127.0.0.1"),
};

let loopback = IpAddr {
    kind: IpAddrKind::V6,					// 枚举值
    address: String::from("::1"),
};

/* 简化后 */

enum IpAddr {
    V4(u8, u8, u8, u8),					// 直接将数据附加到枚举的每个成员上
    V6(String),							// 这样就不需要一个额外的结构体了
}

fn main(){
    let home = IpAddr::V4(127,0,0,1);
    let loopback = IpAddr::V6(String::from("::1"));
    println!("{:?}",home);
}
```

**枚举与结构体**

枚举的每个变体可以处理不同类型和数量的数据。例如字符串、数字类型或者结构体，甚至可以包含另一个枚举。

```rust
enum Message {
    Quit,									// 没有关联任何数据
    Move { x: i32, y: i32 },				// 类似结构体包含命名字段
    Write(String),							// 包含单独一个 String
    ChangeColor(i32, i32, i32),				// 包含三个 i32
}

// 等同于如下

struct QuitMessage; 						// 类单元结构体
struct MoveMessage {						// 结构体
    x: i32,
    y: i32,
}
struct WriteMessage(String); 				// 元组结构体
struct ChangeColorMessage(i32, i32, i32); 	// 元组结构体
```

枚举中也可以使用 `impl` 来为枚举定义方法。

```rust
enum Message {
    Quit,												// 没有关联任何数据
    Move { x: i32, y: i32 },							// 类似结构体包含命名字段
    Write(String),										// 包含单独一个 String
    ChangeColor(i32, i32, i32),							// 包含三个 i32
}

impl Message {
    fn call(&self) {
        // 在这里定义方法体
        println!("Method");
    }
}

fn main(){
    let m = Message::Write(String::from("hello"));		// 实例化枚举
    m.call();                                          	// 调用方法
}
```

## Option

Rust并没有空值，不过有一个可以表示存在或不存在概念的枚举，这个枚举就是 `Option<T>`。

```rust
enum Option<T> {
    None,				// 为空
    Some(T),			// 可能存在
}
```

- 包含在了 prelude 之中，无需将其显式引入作用域
- 可以不需要 `Option::` 前缀来直接使用 `Some` 和 `None`

```rust
let some_number = Some(5);					// 类型为Option<i32>
let some_char = Some('e');					// 类型为Option<char>

let absent_number: Option<i32> = None;		// 需指定Option整体类型
```

`Option<T>` 和 `T`是不同的类型，编译器不允许像一个肯定有效的值那样使用 `Option<T>`

```rust
let x: i8 = 5;
let y: Option<i8> = Some(5);

let sum = x + y;							// 报错 类型不同
```

好处

1. 消除了错误地假设一个非空值的风险
2. 只要一个值不是 `Option<T>` 类型，就可以安全的认定它的值不为空

注

1. 在对 `Option<T>` 进行运算之前必须将其转换为 `T`

# 模式匹配

## match

`match` 是一个极为强大的控制流运算符，允许将一个值与一系列的模式相比较，并根据相匹配的模式执行相应代码。

模式可由字面值、变量、通配符和许多其他内容构成。

- `if`表达式必须返回一个布尔值，而`match`可以是任何类型的
- 顺序比较执行
- 各分支相关联的代码是一个表达式，而表达式的结果值将作为整个 `match` 表达式的返回值
- 分支必须覆盖了所有的可能性

```rust
#[derive(Debug)]

enum Coin {
    Penny,
    Nickel,
    Dime,
    Quarter,
}

fn value_in_cents(coin: Coin) -> u8 {
    match coin {								// 可以是任何类型的
        Coin::Penny => 1,						// 每一个分支之间使用逗号分隔
        Coin::Nickel => 5,
        Coin::Dime => 10,
        Coin::Quarter => 25,
    }
}

fn main(){
    let c = Coin::Penny;
    println!("{}",value_in_cents(c));	
}
```

输出

```shell
1
```

如果分支代码较短的话通常不使用大括号。如果想要在分支中运行多行代码，可以使用大括号，而分支后的逗号是可选的

```rust
fn value_in_cents(coin: Coin) -> u8 {
    match coin {
        Coin::Penny => {
            println!("Lucky penny!");
            1
        },
        Coin::Nickel => 5,
        Coin::Dime => 10,
        Coin::Quarter => 25,
    }
}
```

**绑定值的模式**

匹配分支的另一个有用的功能是可以绑定匹配的模式的部分值，这也就是如何从枚举成员中提取值的。

```rust
#[derive(Debug)] // 这样可以立刻看到州的名称

enum UsState {
    Alabama,
    Alaska,
}

enum Coin {
    Penny,
    Nickel,
    Dime,
    Quarter(UsState),
}
fn value_in_cents(coin: Coin) -> u8 {
    match coin {
        Coin::Penny => 1,
        Coin::Nickel => 5,
        Coin::Dime => 10,
        Coin::Quarter(state) => {
            println!("State quarter from {state:?}!");
            25
        }
    }
}
fn main(){
    let c = Coin::Quarter(UsState::Alabama);
    println!("{:?}",value_in_cents(c));
}

```

输出

```shell
State quarter from Alabama!
25
```

**匹配Option**

```rust
fn plus_one(x: Option<i32>) -> Option<i32> {
    match x {
        None => None,							// 缺省则报错 必须覆盖全部分支
        Some(i) => Some(i + 1),
    }
}
fn main(){
    let five = Some(5);
    let six = plus_one(five);
    let none = plus_one(None);
}
```

**通配模式和 `_` 占位符**

由于模式是按顺序匹配的，因此必须将通配分支放在最后。

- other：其他未匹配到的分支绑定

- \_：可以匹配任意值而不绑定到该值

```rust
let dice_roll = 9;
match dice_roll {
    3 => add_fancy_hat(),
    7 => remove_fancy_hat(),
    other => move_player(other),
}

fn add_fancy_hat() {}
fn remove_fancy_hat() {}
fn move_player(num_spaces: u8) {}
```

```rust
let dice_roll = 9;
match dice_roll {
    3 => add_fancy_hat(),
    7 => remove_fancy_hat(),
    _ => (),						// 不获取怕匹配到的值
}

fn add_fancy_hat() {}
fn remove_fancy_hat() {}
fn reroll() {}
```

**所有权**

模式匹配时注意是否发生了移动。

```rust
fn main() {
    let opt: Option<String> = Some(String::from("hello"));

    match opt {
        Some(s) => println!("{s}"),     // 发生了移动
        None => println!("None"),
    }

    println!("{:?}",opt);				// ❌ 失效
}
```

```rust
fn main() {
    let opt: Option<String> = Some(String::from("hello"));

    match &opt {						 // 使用引用进行模式匹配
        Some(s) => println!("{s}"),    
        None => println!("None"),
    }

    println!("{:?}",opt);				// ✅ 仍可用
}
```

## if let与let else

**if let**

`if let` 语法用来处理只匹配一个模式的值而忽略其他模式的情况。

- match：必须穷举所有情况

```rust
let config_max = Some(3u8);
match config_max {
    Some(max) => println!("The maximum is configured to be {max}"),
    _ => (),
}
```

- if let：无需穷举所有，通过等号分隔的一个模式和一个表达式，不匹配时 `if let` 块中的代码不会执行

```rust
let config_max = Some(3u8);
if let Some(max) = config_max {
    println!("The maximum is configured to be {max}");
}
```

可以在 `if let` 中包含一个 `else`，`else` 块中的代码与 `match` 表达式中的 `_` 分支块中的代码相同

```rust
let mut count = 0;
match coin {
    Coin::Quarter(state) => println!("State quarter from {state:?}!"),
    _ => count += 1,
}

/* 二者等价 */

let mut count = 0;
if let Coin::Quarter(state) = coin {
    println!("State quarter from {state:?}!");
} else {
    count += 1;
}
```

**let else**

非常类似于 `if let`，不过没有 `if` 分支，只有 `else` 分支。

```rust
fn describe_state_quarter(coin: Coin) -> Option<String> {
    let Coin::Quarter(state) = coin else {
        return None;
    };

    if state.existed_in(1900) {
        Some(format!("{state:?} is pretty old, for America!"))
    } else {
        Some(format!("{state:?} is relatively new."))
    }
}
```

# 模块系统

## Crate

crate 是 Rust 在编译时最小的代码单位。

crate 有两种形式

- 二进制 crate：可执行的，必须有一个 `main` 函数
- 库 crate：没有 `main` 函数，无法执行，定义了可供多个项目复用的功能模块

**crate root**

编译crate的入口点。

- binary crate：src/main.rs
- library crate：src/lib.rs

## 包

包由一个或多个crate构成，包含argo.toml 文件，阐述如何去构建这些 crate。

- 最多包含一个库 crate
- 可由多个二进制 crate
- 至少有一个 crate

## 模块

将代码组织成更小、更易管理的单元的方法。

- 使用 mod 声明模块
- 可有子模块
- 路径
- 私有/公有
- use 引用

**举例**

文件结构

```shell
backyard
├── Cargo.lock
├── Cargo.toml
└── src
    ├── garden
    │   └── enums.rs
    	└── structs.rs
    ├── functions.rs
    ├── garden.rs
    └── lib.rs
    └── main.rs
```

`lib.rs`

```rust
// 编译器会在下列路径中寻找模块
// 1.内联，mod 后跟大括号
// 2.在同名同级文件 src/garden.rs
// 3.在同名文件夹中 src/garden/mod.rs

//pub mod garden{}		// 1
pub mod garden;			// 2、3
pub mod functions;		// src/functions.rs
```

`garden.rs`

```rust
pub mod enums;
// 创建文件 src/garden/enums.rs

pub mod structs;
// 创建文件 src/garden/structs.rs
```

`functions.rs`

```rust
pub fn function1() {
    println!("funtcion1");
}
```

`enums.rs`

```rust
pub enum YesNo {						// 无需声明变体为公有
    Yes,
    No,
}
```

`structs.rs`

```rust
use crate::garden::enums::YesNo;		// use+路径：进行引用

pub struct HousePrice {					// 结构体需要所有字段均为pub才能变为公有
    pub price: u32,
    pub area: String,
    pub bed_rooms: u32,
    pub main_road: YesNo,
}
```

`main.rs`

```rust
use package_name::garden::structs::HousePrice;			// 引用结构体
use package_name::functions;

fn main() {
    let y = package_name::garden::enums::YesNoe::Yes;
    let house_price = HousePrice {
        price: 10_000,
        area:String::from("Center"),
        bed_room: 3,
        main_road: YesNo::Yes,
    }
    
    functions::function1();
}
```

**路径**

- 绝对路径（absolute path）：以 crate root 开头的完整路径
- 相对路径（relative path）：从当前模块开始，可用使用 `self`、`super`

```rust
fn main() {
    crate::m1::m2::method1();                   // 绝对路径
}

mod m1 {                                        // 与main同级，可用直接被调用
    pub mod m2 {                                // 子模块默认为私有
        pub fn method1() {
            println!("Method 1");
        }
    }
}

mod x1 {
    fn method3() {
        x2::method2();                          // 相对路径
        self::x2::method2();                    // 相对路径
    }

    mod x2 {
        pub fn method2() {
            super::super::m1::m2::method1();    // 相对路径
        }
    }
}
```

**可见性**

- 父模块不能使用子模块的私有项
- 子模块中的项可以使用它们父模块中的项
- 使用关键字 pub 可以让其变为公有的

**结构体与枚举**

- 结构体：需要为结构体本身和各字段单独设置为pub
- 枚举：只需枚举本身是pub，所有变体就都是pub

**use**

引用路径。

- function：引用到父模块
- struct、enum：引用本身完整路径

同父模块可同时引用

```rust
use csv::{Writer, ReaderBuilder};
use csv::*;											// 引用全部
```

**as**

给同名的引用起别名。

```rust
use std::fmt::Result;
use std::io::Result as IoResult;
```

**pub use**

`lib.rs`

```rust
mod front_of_house {
    pub mod hosting {
        pub fn add_to_waitlist(){}
    }
}

pub use crate::front_of_house::hosting;								// 修改了级别 

pub fn eat_at_restaurant() {
    hosting::add_to_waitlist();
}
```

`main.rs`

```rust
fn main () {
    package_name::front_of_house::hosting::add_to_waitlist();		// ❌
    package_name::hosting::add_to_waitlist();						// ✅
}
```

# 常见集合

存储在堆上。

## 向量

Vector 允许储存多个类型相同的值，它们在内存中彼此相邻排列。

**定义**

```rust
let v: Vec<i32> = Vec::new();		// 须表明类型
let v = vec![1, 2, 3];				// 自动推断储存值的类型
```

**添加和移除元素**

```rust
let mut v = vec![1, 2, 3];
v.push(4); 					// 添加元素
v.pop(); 					// 移除最后一个元素
v.insert(1, 10); 			// 在索引 1 处插入元素
v.remove(2);				// 移除索引 2 的元素
```

**读取**

可以通过索引或使用 `get` 方法读取值。

 ```rust
 let v = vec![1, 2, 3, 4, 5];
 
 let third: &i32 = &v[2];
 println!("The third element is {third}");
 
 let third: Option<&i32> = v.get(2);	// 类型为 Option<&i32>
 match third {
     Some(third) => println!("The third element is {third}"),
     None => println!("There is no third element."),
 }
 ```

```rust
let v = vec![1, 2, 3, 4, 5];

let does_not_exist = &v[100];		// panic
let does_not_exist = v.get(100);	// 不会报错 返回类型为 Option<&V>
```

```rust
let mut v = vec![1, 2, 3, 4, 5];			

let first = &v[0];

v.push(6);									//  ❌ 无法编译

println!("The first element is: {first}");
```

**遍历**

```rust
let v = vec![100, 32, 57];
for i in &v {
    println!("{i}");
}

let mut v = vec![100, 32, 57];
for i in &mut v {
    *i += 50;								// 须解引用
}
```

注

1. `for` 循环中获取的 vector 引用阻止同时对整个 vector 进行修改

**内存管理**

Vec 的容量会根据需要动态扩展，但频繁扩容可能影响性能。可以使用以下方法优化

```rust
let mut v = Vec::with_capacity(10); 	// 提前分配容量
v.reserve(20); 							// 预留至少 20 个元素的空间
v.shrink_to_fit(); 						// 缩小容量到当前长度
```

**排序**

```rust
let mut v = vec![5, 3, 8, 1];
v.sort(); 								// 稳定排序（升序）
v.sort_by(|a, b| b.cmp(a)); 			// 自定义降序排序
v.sort_unstable(); 						// 非稳定排序，性能更高
```

**其他方法**

```rust
let mut v = vec![1, 2, 3];
v.clear(); 						 		// 清空 Vector
v.extend([4, 5, 6]); 			 		// 扩展元素
let slice = &v[1..3]; 					// 获取切片
v.retain(|&x| x % 2 == 0);		 		// 按条件保留元素
```

**使用枚举存储不同类型**

```rust
enum SpreadsheetCell {					// 定义一个枚举以便能在 vector 中存放不同类型的数据
    Int(i32),
    Float(f64),
    Text(String),
}

let row = vec![
    SpreadsheetCell::Int(3),
    SpreadsheetCell::Text(String::from("blue")),
    SpreadsheetCell::Float(10.12),
];

```

**离开作用域**

类似于任何其他的 `struct`，vector 在其离开作用域时会被释放。

```rust
{
    let v = vec![1, 2, 3, 4];

    // 使用 v
} // <- 在这里 v 离开作用域并被释放

```

## 字符串

是一种可增长、可变、可拥有、UTF-8 编码的字符串类型。

**定义**

```rust
let mut s = String::new();

let data = "initial contents";				// &str类型转为String类型
let s = data.to_string();
// 该方法也可直接用于字符串字面值
let s = "initial contents".to_string();

let s = String::from("initial contents");	// 使用函数
```

**附加与拼接**

`String` 的大小可以增加，其内容也可以改变。如可以使用 `+` 运算符或 `format!` 宏来拼接 `String` 值。

- `push_str`：附加字符串

```rust
let mut s1 = String::from("foo");
let s2 = "bar";
s1.push_str(s2);
println!("s2 is {s2}");					// s2 仍能使用
```

- `push`：附加字符

```rust
let mut s = String::from("lo");
s.push('l');
```

- `+`与`format!`：拼接字符串

```rust
let s1 = String::from("Hello, ");
let s2 = String::from("world!");
let s3 = s1 + &s2; 						// 注意 s1 被移动了，不能继续使用，s2被强转为 &str
										// add 只能将 String 和 &str 相加

/* 级联多个字符串 */
let s1 = String::from("tic");
let s2 = String::from("tac");
let s3 = String::from("toe");

let s = s1 + "-" + &s2 + "-" + &s3;

// 使用format! 宏更方便
let s1 = String::from("tic");
let s2 = String::from("tac");
let s3 = String::from("toe");

let s = format!("{s1}-{s2}-{s3}");
```

**索引**

Rust 的字符串不支持索引。

报错

```rust
let s1 = String::from("hi");
let h = s1[0];
```

原因

1. 有效的 Unicode 标量值可能会由不止一个字节组成
2. Rust必须从头遍历内容到指定索引以确定有多少个有效字符，所以无法满足时间复杂度为O（1）

相比使用 `[]` 和单个值的索引，可以使用 `[]` 和一个 range 来创建含特定字节的字符串 slice

```rust
let hello = "Здравствуйте";				// 这里的每个 Unicode 标量值需要两个字节存储

let s = &hello[0..4];					// 正确 s = Зд 类型为：&str
let s = &hello[0..1]					// panic
```

也可以使用`get`方法

```rust
let s = String::from("hello");

if let Some(slice) = s.get(0..3) {
    println!("{}", slice); 				// 输出 "hel"
}
```

**遍历**

操作字符串每一部分的最好的方法是明确表示需要字符还是字节。

- `chars()`：字符

```rust
for c in "Зд".chars() {
    println!("{c}");
}
```

输出

```shell
З
д
```

- `bytes`：字节

```rust
for b in "Зд".bytes() {
    println!("{b}");
}
```

输出

```shell
208
151
208
180
```

**查找**

- `contains()`：检查字符串是否包含指定的子字符串
- `find()`：返回子字符串第一次出现的位置（如果存在）

```rust
let s = String::from("hello world");
if s.contains("world") {
    println!("Found 'world' in the string!");
}

if let Some(index) = s.find("world") {
    println!("'world' found at index {}", index);
}
```

**分割**

- `split()`：按指定分隔符分割字符串，返回一个迭代器
- `lines()`：按行分割字符串，返回一个迭代器

```rust
let s = String::from("hello,world,foo,bar");
for part in s.split(',') {
    println!("{}", part);
}

let multiline = String::from("hello\nworld");
for line in multiline.lines() {
    println!("{}", line);
}
```

**替换**

- `replace()`：将字符串中的某些内容替换为其他内容

```rust
let s = String::from("hello world");
let new_s = s.replace("world", "Rust");
println!("{}", new_s); 						// 输出 "hello Rust"
```

**转换**

- `to_uppercase()`：将字符串转换为大写
- `to_lowercase()`：将字符串转换为小写

```rust
let s = String::from("Hello World");
println!("{}", s.to_uppercase()); 		// 输出 "HELLO WORLD"
println!("{}", s.to_lowercase()); 		// 输出 "hello world"
```

**比较**

- `eq()`：比较两个字符串是否相等
- `partial_cmp()`：比较两个字符串的大小

```rust
let s1 = String::from("hello");
let s2 = String::from("hello");
if s1.eq(&s2) {
    println!("s1 and s2 are equal");
}

if let Some(order) = s1.partial_cmp(&s2) {
    println!("Comparison result: {:?}", order);
}
```

**清理**

- `trim()`：移除字符串首尾的空白字符
- `trim_start()`：移除字符串开头的空白字符
- `trim_end()`：移除字符串末尾的空白字符

```rust
let s = String::from("   hello world   ");
println!("{}", s.trim()); 						// 输出 "hello world"
println!("{}", s.trim_start()); 				// 输出 "hello world   "
println!("{}", s.trim_end()); 					// 输出 "   hello world"
```

##  哈希

哈希类型储存了一个键类型 `K` 对应一个值类型 `V` 的映射。它通过一个哈希函数（hashing function）来实现映射，决定如何将键和值放入内存中。

**定义**

哈希所有的键必须是相同类型，值也必须都是相同类型

```rust
use std::collections::HashMap;

fn main() {
    let mut scores = HashMap::new();									// 新建哈希

    scores.insert(String::from("Blue"), 10);							// 插入数据
    scores.insert(String::from("Yellow"), 50);

    let vec = vec![("key1","value1"),("key2","value2")];
    let mut map : HashMap<_, _> = vec.into_iter().collect();			// 类型转换
    // into_iter()函数返回可迭代的数据结构
    // collect()函数将迭代器的结果收集到一个目标集合
}
```

**访问**

- `get` ：通过键来获取哈希 map 中的值

```rust
use std::collections::HashMap;

let mut scores = HashMap::new();

scores.insert(String::from("Blue"), 10);
scores.insert(String::from("Yellow"), 50);

let team_name = String::from("Blue");
let score = scores.get(&team_name).copied().unwrap_or(0);
// get 方法返回 Option<&V> 调用 copied 方法来获取一个 Option<v>
// 调用 unwrap_or 当值为 `None` 或 `Err` 时返回0，否则取出其中的值
```

**迭代**

- `iter()`：按键值对迭代
- `keys()`：迭代所有键
- `values()`：迭代所有值
- `into_iter()`：消耗 `HashMap` 并迭代键值对

```rust
use std::collections::HashMap;

fn main() {
    let mut map = HashMap::new();
    map.insert("key1", 10);
    map.insert("key2", 20);

    // 按键值对迭代
    for (key, value) in map.iter() {
//  for (key, value) in &scores { 		//同理
        println!("{}: {}", key, value);
    }

    // 迭代所有键
    for key in map.keys() {
        println!("Key: {}", key);
    }

    // 迭代所有值
    for value in map.values() {
        println!("Value: {}", value);
    }

    // 消耗 HashMap 并迭代
    for (key, value) in map.into_iter() {
        println!("{}: {}", key, value);
    }
}

// 打印时 k-v 出现顺序不一定
```

**查询**

- `contains_key()`：检查是否包含某个键
- `len()`：返回 `HashMap` 中的键值对数量
- `is_empty()`：检查 `HashMap` 是否为空

```rust
use std::collections::HashMap;

fn main() {
    let mut map = HashMap::new();
    map.insert("key1", 10);

    if map.contains_key("key1") {
        println!("'key1' exists in the map");
    }

    println!("Number of entries: {}", map.len()); 			// 输出 1
    println!("Is the map empty? {}", map.is_empty()); 		// 输出 false
}
```

**所有权**

- 对于实现Copy trait的值，直接复制到map中
- 对于具有所有权类型的值，移动到map中

```rust
use std::collections::HashMap;

let field_name = String::from("Favorite color");
let field_value = String::from("Blue");

let mut map = HashMap::new();
map.insert(field_name, field_value);
// 这里 field_name 和 field_value 不再有效
// 如果将值的引用插入哈希 map，这些值本身将不会被移动进哈希 map
```

**更新**

每个唯一的键只能同时关联一个值。

- `insert`：覆盖一个值

```rust
use std::collections::HashMap;

let mut scores = HashMap::new();

scores.insert(String::from("Blue"), 10);
scores.insert(String::from("Blue"), 25);

println!("{scores:?}");
```

输出

```shell
{"Blue": 25}
```

- `entry`：只在键尚不存在时插入键值对

```rust
use std::collections::HashMap;

let mut scores = HashMap::new();
scores.insert(String::from("Blue"), 10);

scores.entry(String::from("Yellow")).or_insert(50);		// 返回类型为 &mut T
scores.entry(String::from("Blue")).or_insert(50);

println!("{scores:?}");
```

输出

```shell
{"Yellow": 50, "Blue": 10}
```

根据旧值更新一个值

```rust
use std::collections::HashMap;

let text = "hello world wonderful world";

let mut map = HashMap::new();							// 需声明为可变类型

for word in text.split_whitespace() {
    let count = map.entry(word).or_insert(0);
    *count += 1;
}

println!("{map:?}");
```

输出

```shell
{"world": 2, "hello": 1, "wonderful": 1}
```

**删除**

- `remove()`：通过键删除键值对
- `clear()`：清空整个 HashMap

```rust
use std::collections::HashMap;

fn main() {
    let mut map = HashMap::new();
    map.insert("key1", 10);
    map.insert("key2", 20);

    if let Some(value) = map.remove("key1") {
        println!("Removed value: {}", value); 		// 输出 10
    }

    map.clear(); // 清空 HashMap
    println!("Map is empty: {}", map.is_empty()); 	// 输出 true
}
```

**合并**

- `extend()`：将另一个 HashMap 或迭代器的键值对插入当前 HashMap
- `append()`：将另一个 HashMap 的内容追加到当前 HashMap 中

```rust
use std::collections::HashMap;

fn main() {
    let mut map1 = HashMap::new();
    map1.insert("key1", 10);
    map1.insert("key2", 20);

    let mut map2 = HashMap::new();
    map2.insert("key3", 30);
    map2.insert("key4", 40);

    map1.extend(map2); 			// 将 map2 的内容合并到 map1
    println!("{:?}", map1); 	// 输出 {"key1": 10, "key2": 20, "key3": 30, "key4": 40}
}
```

**其他方法**

- `drain()`：移除并返回指定范围内的键值对

```rust
use std::collections::HashMap;

fn main() {
    let mut map = HashMap::new();
    map.insert("key1", 10);
    map.insert("key2", 20);
    map.insert("key3", 30);

    // 使用 drain
    let drained: HashMap<_, _> = map.drain().collect();
    
    println!("{:?}", drained);  // 输出 {"key1": 10, "key2": 20, "key3": 30}
    println!("{:?}", map);      // 输出 {}
}
```

# 错误处理

Rust 没有异常，Rust 将错误分为两大类

- 不可恢复的：用 `panic!` 宏，在程序遇到不可恢复的错误时停止执行
- 可恢复的：用`Result<T, E>` 数据类型，用于处理可恢复的错误

**不可恢复的错误场景（panic!）**

- 程序进入不可预期的状态
- 安全问题或代码无法继续执行
- 违反函数契约或关键假设

> 原型代码和示例
>
> 测试代码
>
> 安全性关键的输入验证
>
> 调用外部不可控代码时的异常

**可恢复的错误场景（Result）**

- 提供恢复选项
- 预期可能发生的错误
- 希望调用者决定如何处理错误

> 处理可预期的错误
>
> HTTP请求失败
>
> 解析错误
>
> 用户输入验证

## panic！

处理不可恢复的错误，默认情况下 panic 会打印出一个错误信息，并清理栈数据，然后退出。

在实践中有两种方法造成 panic

1. 执行会造成代码 panic 的操作
2. 显式调用 `panic!` 宏

**展开与终止**

当出现 panic 时，程序默认会开始展开。如果需要项目的最终二进制文件越小越好，通过在 Cargo.toml 的 `[profile]` 部分增加 `panic = 'abort'`，可以由展开切换为终止。

```rust
[package]
name = "gussing_game"
version = "0.1.0"
edition = "2024"

[dependencies]

[profile.release]
panic = "abort";
```

- 展开：回溯栈并清理它遇到的每一个函数的数据
- 终止：不清理数据就退出程序，程序所使用的内存需要由操作系统来清理

调用`panic!`

```rust
fn main() {
    panic!("crash and burn");
}
```

**backtrace**

backtrace 是一个执行到目前位置所有被调用的函数的列表。

- 将 `RUST_BACKTRACE` 环境变量设置为任何不是 `0` 的值就可以获取 backtrace

```shell
SET RUST_BACKTRACE = 1
```

- 必须Debug模式
- 从提到的第一个自己编写的文件的那一行开始调查

**举例**

当需要特定值时，自定义数据类型，而不是到处重复检查是个不错的选择。

```rust
pub struct Guess {
    value: i32,																// 私有的
}

impl Guess {
    pub fn new(value: i32) -> Guess {										// 关联函数
        if value < 1 || value > 100 {
            panic!("Guess value must be between 1 and 100, got {value}.");	// 报错
        }
        Guess { value }
    }

    pub fn value(&self) -> i32 {											// 方法
        self.value
    }
}
// 确保了不会存在一个 value 没有通过 Guess::new 函数的条件检查的 Guess
```

## Result

处理可恢复的错误，来告诉代码调用者需要处理潜在的成功或失败。

- 被导入到了 prelude 中
- 可处理也可不处理，程序不会报错

**数据结构**

```rust
enum Result<T, E> {
    Ok(T),
    Err(E),
}
```

**举例**

```rust
use std::fs::File;

fn main() {
    let greeting_file_result = File::open("hello.txt");

    let greeting_file = match greeting_file_result {
        Ok(file) => file,												// 成功时返回文件句柄的实例，之后获取文件句柄
        Err(error) => panic!("Problem opening the file: {error:?}"),	// 失败时返回错误信息的实例，之后调用 panic! 宏
    };
}
```

**匹配不同错误**

```rust
use std::fs::File;
use std::io::ErrorKind;

fn main() {
    let greeting_file_result = File::open("hello.txt");

    let greeting_file = match greeting_file_result {
        Ok(file) => file,
        Err(error) => match error.kind() {
            ErrorKind::NotFound => match File::create("hello.txt") {
                Ok(fc) => fc,
                Err(e) => panic!("Problem creating the file: {e:?}"),
            },
            _ => {														// 其余错误类型
                panic!("Problem opening the file: {error:?}");
            },
        },
    };
}
```

**unwrap和expect**

如果 `Result` 值是变体 `Ok`，`unwrap` 会返回 `Ok` 中的值。如果 `Result` 是变体 `Err`，`unwrap` 会为我们调用 `panic!`来终止。

```rust
use std::fs::File;

fn main() {
    let greeting_file = File::open("hello.txt").unwrap();
}
```

`expect` 方法允许自定义 `panic!` 的错误信息，可以表明意图并更易于追踪 panic 的根源。

```rust
use std::fs::File;

fn main() {
    let greeting_file = File::open("hello.txt")
        .expect("hello.txt should be included in this project");
}
```

**传播错误**

当函数的实现中调用了可能会失败的操作时，除了在这个函数中处理错误外，还可以选择让调用者知道这个错误并决定该如何处理。这被称为传播（propagating）错误。

将所有的成功或失败信息向上传播，让调用者选择合适的处理方法

```rust
use std::fs::File;
use std::io::{self, Read};

// 调用该函数最终会得到一个包含用户名的 Ok 值，或者一个包含 io::Error 的 Err 值

fn read_username_from_file() -> Result<String, io::Error> {		// 错误类型范围应尽量小
    let username_file_result = File::open("hello.txt");

    let mut username_file = match username_file_result {
        Ok(file) => file,
        Err(e) => return Err(e),								// 提前返回
    };

    let mut username = String::new();

    match username_file.read_to_string(&mut username) {
        Ok(_) => Ok(username),
        Err(e) => Err(e),
    }
}
```

**?简化传播**

一旦错误，`Err` 将作为整个函数的返回值，就好像使用了 `return` 关键字一样。

```rust
use std::fs::File;
use std::io::{self, Read};

fn read_username_from_file() -> Result<String, io::Error> {		// 函数的返回值为Result
    let mut username_file = File::open("hello.txt")?;			// 返回值Result 兼容
    let mut username = String::new();
    username_file.read_to_string(&mut username)?;
    Ok(username)
}

/* 链式方法调用 */

fn read_username_from_file() -> Result<String, io::Error> {
    let mut username = String::new();

    File::open("hello.txt")?.read_to_string(&mut username)?;

    Ok(username)
}
```

`?` 运算符只能被用于返回值与 `?` 作用的值相兼容的函数。

错误

```rust
use std::fs::File;

fn main() {
    let greeting_file = File::open("hello.txt")?;
	// 返回值为 Result 与 main 返回值()不兼容    
}
```

正确

```rust
use std::error::Error;
use std::fs::File;

fn main() -> Result<(), Box<dyn Error>> {				// Box<dyn Error> 为 任何类型的错误
    let greeting_file = File::open("hello.txt")?;

    Ok(())												// 程序返回0
	// 返回值为 Result 与 main 返回值 Result 兼容    
}
```

`?` 也可用于 `Option<T>` 值。如同对 `Result` 使用 `?` 一样，只能在返回 `Option` 的函数中对 `Option` 使用 `?`。如果值是 `None`，此时 `None` 会从函数中提前返回。如果值是 `Some`，`Some` 中的值作为表达式的返回值同时函数继续。

```rust
/* 从给定文本中返回第一行最后一个字符 */
fn last_char_of_first_line(text: &str) -> Option<char> {
    text.lines().next()?.chars().last()
}
```

注

1. 不可以混合搭配，`?` 运算符不会自动将 `Result` 转化为 `Option`，反之亦然
2. 若要混合，可以使用类似 `Result` 的 `ok` 方法或者 `Option` 的 `ok_or` 方法来显式转换
3. 还可以用于实现了`FromResidual`的数据类型的函数内

**返回自定义错误**

```rust
use std::{fs,io};
use std::fs::File;
use std::io::{Error, Read};
use std::num::ParseIntError;

pub enum MyError {
    Io(io::Error),
    ParseInt(ParseIntError),
    Other(String),
}

impl From<io::Error> for MyError {
    fn from(value: Error) -> Self {
        MyError::Io(value)
    }
}

impl From<ParseIntError> for MyError {
    fn from(value: ParseIntError) -> Self {
        MyError::ParseInt(value)
    }
}

fn read_username_from_file() -> Result<String, MyError> {
    let mut name = String::new();
    let file = File::open("lala.txt")?.read_to_string(&mut name)?;
    let num:i32 = "55".parse()?;
    Ok(name)
}
```

# 泛型

泛型允许使用一个占位符来替换特定类型，以此来减少代码冗余。

## 函数中使用泛型

在函数签名中指定参数和返回值的类型的地方，改用泛型来表示，习惯上一般用`T`作为占位符。

未使用泛型

```rust
/* 未使用泛型 */

fn largest_i32(list: &[i32]) -> &i32 {
    let mut largest = &list[0];

    for item in list {
        if item > largest {
            largest = item;
        }
    }
    
    largest
}

fn largest_char(list: &[char]) -> &char {
    let mut largest = &list[0];

    for item in list {
        if item > largest {
            largest = item;
        }
    }

    largest
}

fn main() {
    let number_list = vec![34, 50, 25, 100, 65];

    let result = largest_i32(&number_list);
    println!("The largest number is {result}");

    let char_list = vec!['y', 'm', 'a', 'q'];

    let result = largest_char(&char_list);
    println!("The largest char is {result}");
}
```

使用泛型

```rust
/* 使用泛型 */

fn largest<T>(list: &[T]) -> &T {
    // 函数 largest 有泛型类型 T
    // 有个参数 list，其类型是元素为 T 的 slice
    // largest 函数会返回一个与 T 相同类型的引用
//fn largest<T:PartialOrd>(list: &[T]) -> &T {
    let mut largest = &list[0];

    for item in list {
        if item > largest {
            largest = item;
        }
    }

    largest
}

fn main() {
    let number_list = vec![34, 50, 25, 100, 65];

    let result = largest(&number_list);
    println!("The largest number is {result}");

    let char_list = vec!['y', 'm', 'a', 'q'];

    let result = largest(&char_list);
    println!("The largest char is {result}");
}
```

## 结构体中使用泛型

同样也可以用 `<>` 语法来定义结构体，它包含一个或多个泛型参数类型字段。

```rust
struct Point<T> {		// 结构体字段类型相同
    x: T,
    y: T,
}

fn main() {
    let integer = Point { x: 5, y: 10 };
    let float = Point { x: 1.0, y: 4.0 };
}
```

定义拥有不同字段类型的泛型结构体

```rust
struct Point<T, U> {	// 结构体字段类型不相同
    x: T,
    y: U,
}

fn main() {
    let both_integer = Point { x: 5, y: 10 };
    let both_float = Point { x: 1.0, y: 4.0 };
    let integer_and_float = Point { x: 5, y: 4.0 };
}
```

注

1. 当发现代码中需要很多泛型时，这可能表明代码需要重构分解成更小的结构

## 枚举中使用泛型

和结构体类似，枚举也可以在变体中存放泛型数据类型。

```rust
enum Option<T> {
    Some(T),
    None,
}
```

枚举也可以拥有多个泛型类型。

```rust
enum Result<T, E> {
    Ok(T),
    Err(E),
}
```

## 方法中使用泛型

在为结构体和枚举实现方法时，一样也可以用泛型。

```rust
struct Point<T> {
    x: T,
    y: T,
}

impl<T> Point<T> {						// 必须在 impl 后面声明 T
    fn x(&self) -> &T {
        &self.x
    }
}

fn main() {
    let p = Point { x: 5, y: 10 };

    println!("p.x = {}", p.x());
}
```

定义方法时，可以选择为 `Point<f32>` 实例实现方法，而不是为泛型 `Point` 实例

```rust
impl Point<f32> {
    fn distance_from_origin(&self) -> f32 {
        (self.x.powi(2) + self.y.powi(2)).sqrt()
    }
}
```

当方法与结构体类型不同时

```rust
struct Point<X1, Y1> {
    x: X1,
    y: Y1,
}

impl<X1, Y1> Point<X1, Y1> {
    fn mixup<X2, Y2>(self, other: Point<X2, Y2>) -> Point<X1, Y2> {
        Point {
            x: self.x,
            y: other.y,
        }
    }
}

fn main() {
    let p1 = Point { x: 5, y: 10.4 };
    let p2 = Point { x: "Hello", y: 'c' };

    let p3 = p1.mixup(p2);

    println!("p3.x = {}, p3.y = {}", p3.x, p3.y);		// 注意 p1、p2 发生了移动
    println!("p3.x = {}, p3.y = {}", p1.x, p2.y);		// p1、p2 不再可用
}
```

## 泛型性能

泛型并不会使程序比具体类型运行得慢。因为 Rust 会将每种情况下的泛型代码编译为具体类型，使用泛型没有运行时开销。当代码运行时，它的执行效率就跟好像手写每个具体定义的重复代码一样。

# Trait

定义了特定类型拥有可能与其他类型共享的特定功能。

- 作用：将不同的方法签名合并成一个方法签名

## 定义

trait体中可以有多个方法：一行一个方法签名且都以分号结尾。

`src/lib.rs`

```rust
pub trait Summary {			
    fn summarize(&self) -> String;
}
```

## 实现

**为类型实现trait**

`src/lib.rs`

```rust
pub struct NewsArticle {
    pub headline: String,
    pub location: String,
    pub author: String,
    pub content: String,
}

impl Summary for NewsArticle {
    fn summarize(&self) -> String {
        format!("{}, by {} ({})", self.headline, self.author, self.location)
    }
}

pub struct SocialPost {
    pub username: String,
    pub content: String,
    pub reply: bool,
    pub repost: bool,
}

impl Summary for SocialPost {
    fn summarize(&self) -> String {
        format!("{}: {}", self.username, self.content)
    }
}
```

`main.rs`

```rust
use package_name::{SocialPost, Summary};

fn main() {
    let post = SocialPost {
        username: String::from("horse_ebooks"),
        content: String::from(
            "of course, as you probably already know, people",
        ),
        reply: false,
        repost: false,
    };

    println!("1 new post: {}", post.summarize());
}
```

输出

```shell
1 new post: horse_ebooks: of course, as you probably already know, people
```

注

1. 只有在 trait 或类型至少有一个属于当前 crate 时，才能对类型实现该 trait，确保了其他人编写的代码不会破坏你的代码
2. 不能为外部类型实现外部 trait，例如不能在 当前 crate 中为 `Vec<T>` 实现 `Display` trait，因为 `Display` 和 `Vec<T>` 都定义于标准库中，不位于当前 crate 本地作用域中

**默认实现**

有时可以为 trait 中的某些或全部方法提供默认的行为。

`src/lib.rs`

```rust
pub trait Summary {
    fn summarize(&self) -> String {			// 进行方法的实现
        String::from("(Read more...)")
    }
}

impl Summary for NewsArticle {}				// 使用默认实现
```

`main.rs`

```rust
fn main() {
    let article = NewsArticle {
        headline: String::from("Penguins win the Stanley Cup Championship!"),
        location: String::from("Pittsburgh, PA, USA"),
        author: String::from("Iceburgh"),
        content: String::from(
            "The Pittsburgh Penguins once again are the best \
             hockey team in the NHL.",
        ),
    };

    println!("New article available! {}", article.summarize());
}
```

输出

```shell
New article available! (Read more...)
```

默认实现允许调用相同 trait 中的其他方法，哪怕这些方法没有默认实现。如此，trait 可以提供很多有用的功能而只需要实现指定一小部分内容。

`src/lib.rs`

```rust
pub trait Summary {
    fn summarize_author(&self) -> String;

    fn summarize(&self) -> String {									// 默认实现
        format!("(Read more from {}...)", self.summarize_author())	// 调用其他方法
    }
}

impl Summary for SocialPost {
    fn summarize_author(&self) -> String {
        format!("@{}", self.username)
    }
}
```

`main.rs`

```rust
fn main() {
    let post = SocialPost {
        username: String::from("horse_ebooks"),
        content: String::from(
            "of course, as you probably already know, people",
        ),
        reply: false,
        repost: false,
    };

    println!("1 new social post: {}", post.summarize());		// 调用默认实现
}
```

输出

```shell
1 new post: (Read more from @horse_ebooks...)
```

## trait作为参数

```rust
pub fn notify(item: &impl Summary) {				// 实现了Summary trait的类型
    println!("Breaking news! {}", item.summarize());// 可调用任何来自 Summary trait 的方法
}
```

**Trait Bound**

```rust
// 上示例等价

pub fn notify<T: Summary>(item: &T) {
    println!("Breaking news! {}", item.summarize());
}
```

`impl Trait` 很方便，适用于短小的例子。更长的 trait bound 则适用于更复杂的场景。例如，可以获取两个实现了 `Summary` 的参数。

```rust
pub fn notify(item1: &impl Summary, item2: &impl Summary) {
pub fn notify<T: Summary>(item1: &T, item2: &T) {	
// 强制item1、item2是相同类型
// 泛型 T 被指定为 item1 和 item2 的参数限制，如此传递给参数 item1 和 item2 值的具体类型必须一致
```

**通过 + 指定多个 trait bound**

也可以指定多个 trait bound，通过 `+` 语法实现。

```rust
pub fn notify(item: &(impl Summary + Display)) {
pub fn notify<T: Summary + Display>(item: &T) {
```

**通过 where 简化 trait bound**

当有多个泛型参数的函数在名称和参数列表之间会有很长的 trait bound 信息，这使得函数签名难以阅读。为此，Rust 有另一个在函数签名之后的 `where` 从句中指定 trait bound 的语法。

```rust
fn some_function<T: Display + Clone, U: Clone + Debug>(t: &T, u: &U) -> i32 {

// or
    
fn some_function<T, U>(t: &T, u: &U) -> i32
where
    T: Display + Clone,
    U: Clone + Debug,
{
```

## trait作为返回值

也可以在返回值中使用 `impl Trait` 语法，来返回实现了某个 trait 的类型。

```rust
fn returns_summarizable() -> impl Summary {
    SocialPost {
        username: String::from("horse_ebooks"),
        content: String::from(
            "of course, as you probably already know, people",
        ),
        reply: false,
        repost: false,
    }
}
```

不过这只适用于返回单一类型的情况。

```rust
fn returns_summarizable(switch: bool) -> impl Summary {	  // ❌ 
    if switch {
        NewsArticle {
            headline: String::from(
                "Penguins win the Stanley Cup Championship!",
            ),
            location: String::from("Pittsburgh, PA, USA"),
            author: String::from("Iceburgh"),
            content: String::from(
                "The Pittsburgh Penguins once again are the best \
                 hockey team in the NHL.",
            ),
        }
    } else {
        SocialPost {
            username: String::from("horse_ebooks"),
            content: String::from(
                "of course, as you probably already know, people",
            ),
            reply: false,
            repost: false,
        }
    }
}
```

## 有条件地实现方法

通过使用带有 trait bound 的泛型参数的 `impl` 块，可以有条件地只为那些实现了特定 trait 的类型实现方法。

```rust
use std::fmt::Display;

struct Pair<T> {
    x: T,
    y: T,
}

impl<T> Pair<T> {					// 定义泛型方法
    fn new(x: T, y: T) -> Self {
        Self { x, y }
    }
}

/* 只有那些为 T 类型实现了 PartialOrd trait 和 Display trait 的 Pair<T> 才会实现 cmp_display 方法 */
impl<T: Display + PartialOrd> Pair<T> {
    fn cmp_display(&self) {
        if self.x >= self.y {
            println!("The largest member is x = {}", self.x);
        } else {
            println!("The largest member is y = {}", self.y);
        }
    }
}
```

也可以对任何实现了特定 trait 的类型有条件地实现 trait。

```rust
impl<T: Display> ToString for T {	// 为任何实现了 Display trait 的类型实现了 ToString trait
    // --snip--
}
```

能够使用泛型类型参数来减少重复，而且能够向编译器明确指定泛型类型需要拥有哪些行为。在动态类型语言中，如果调用了一个未定义的方法，会在运行时出现错误。Rust 将这些错误移动到了编译时，甚至在代码能够运行之前就强迫修复问题。

另外，也无需编写运行时检查行为的代码，因为在编译时就已经检查过了。这样既提升了性能又不必放弃泛型的灵活性。

# 生命周期

生命周期是另一类泛型。不同于确保类型有期望的行为，生命周期用于保证引用在需要的整个期间内都是有效的。

- 每一个引用都有一个生命周期
- 多数为隐式的，且可被推断出来
- 主要目的是为了防止悬垂引用

```rust
fn main() {					// Rust会无法通过编译 ❌
    let r;

    {
        let x = 5;
        r = &x;
    }
	// 此时 r 为已释放变量的一个引用
    println!("r: {r}");
}
```

**借用检查器**

Rust 编译器有一个借用检查器（borrow checker），它比较作用域来确保所有的借用都是有效的。

- 确保数据存活周期能够覆盖于其引用的生命周期
- 比较作用域，以确定所有借用是否有效

```rust
fn main() {
    let r;                // ---------+-- 'r
                          //          |
    {                     //          |
        let x = 5;        // -+-- 'x  |
        r = &x;           //  |       |
    }                     // -+       |
                          //          |
    println!("r: {r}");   //          |
}                         // ---------+

// x 的生命周期无法覆盖其引用（r）的生命周期 故无法通过编译 ❌
```

```rust
fn main() {
    let x = 5;            // ----------+-- 'b
                          //           |
    let r = &x;           // --+-- 'a  |
                          //   |       |
    println!("r: {r}");   //   |       |
                          // --+       |
}                         // ----------+

// 引用 r 的生命周期能够被 x 覆盖 ✅
```

**生命周期注解语法**

生命周期注解不会改变引用存活的时间，只是描述多个引用之间的生命周期关系。

与当函数签名中指定了泛型类型参数后就可以接受任何类型一样，当指定了泛型生命周期后函数也能接受任何生命周期的引用。

- 生命周期参数名称必须以撇号`'`开头
- 名称通常全是小写，类似于泛型其名称非常短

```rust
&i32        // 引用
&'a i32     // 带有显式生命周期的引用
&'a mut i32 // 带有显式生命周期的可变引用
```

**生命周期省略**

如果应用这些规则后，剩余引用的生命周期还不清楚，那么就需要显式标明剩余生命周期，否则编译器就会报错。

- 输入生命周期：函数或方法的参数的生命周期
- 输出生命周期：返回值的生命周期

生命周期省略规则

>  规则一：编译器为每一个引用参数都分配一个生命周期参数。
>
> 规则二：如果只有一个输入生命周期参数，那么将它赋予给所有输出生命周期参数。
>
> 规则三：如果方法有多个输入生命周期参数并且其中一个参数是 `&self` 或 `&mut self`，那么所有输出生命周期参数被赋予 `self` 的生命周期。

```rust
fn first_word(s: &str) -> &str {
// 等价
fn first_word<'a>(s: &'a str) -> &'a str {
```

```rust
fn longest(x: &str, y: &str) -> &str {
// 等价
fn longest<'a, 'b>(x: &'a str, y: &'b str) -> &str {
```

**静态生命周期**

`'static`，其生命周期能够存活于整个程序期间。

- 所有的字符串字面值都是 `'static` 的

```rust
let s: &'static str = "I have a static lifetime.";
```

**结合泛型类型参数、trait bounds 和生命周期**

```rust
use std::fmt::Display;

fn longest_with_an_announcement<'a, T>(
    x: &'a str,
    y: &'a str,
    ann: T,
) -> &'a str
where
    T: Display,
{
    println!("Announcement! {ann}");
    if x.len() > y.len() { x } else { y }
}
```

## 函数中的生命周期

```rust
fn longest(x: &str, y: &str) -> &str {			// ❌
    if x.len() > y.len() { x } else { y }		// 无法确定返回x的生命周期还是y的生命周期，所以无法比较生命周期存活情况
}

fn main() {				
    let string1 = String::from("abcd");
    let string2 = "xyz";

    let result = longest(string1.as_str(), string2);
    println!("The longest string is {result}");
}
```

为解决此问题，需添加泛型生命周期参数，来确定引用之间的关系，使得检查器能够分析。

```rust
fn longest<'a>(x: &'a str, y: &'a str) -> &'a str {	// 尖括号中声明泛型生命周期参数
    if x.len() > y.len() { x } else { y }			// 表明返回的引用的生命周期与函数参数所引用的值的生命周期的较小者一致
}

fn main() {
    let string1 = String::from("long string is long");

    {
        let string2 = String::from("xyz");
        let result = longest(string1.as_str(), string2.as_str());
        println!("The longest string is {result}");
    }
}
```

输出

```shell
The longest string is long string is long
```

注意生命周期范围

```rust
fn longest<'a>(x: &'a str, y: &'a str) -> &'a str {				
    if x.len() > y.len() { x } else { y }			
}

fn main() {														//  ❌
    let string1 = String::from("long string is long");
    let result;
    {
        let string2 = String::from("xyz");						// string2 存活不够长
        result = longest(string1.as_str(), string2.as_str());
    }
    println!("The longest string is {result}");
}
```

总是返回第一个参数而不是最长的字符串 slice 。

```rust
fn longest<'a>(x: &'a str, y: &str) -> &'a str {		// 无需为y指定生命周期
    x
}
```

当函数返回一个引用，返回值的生命周期参数必须与一个参数的生命周期参数相匹配。

```rust
fn longest<'a>(x: &str, y: &str) -> &'a str {			// ❌无法编译
    let result = String::from("really long string");
    result.as_str()
}
```

## 结构体中的生命周期

也可以定义包含引用的结构体，不过这需要为结构体定义中的每一个引用添加生命周期注解。

```rust
struct ImportantExcerpt<'a> {		// 声明泛型生命周期参数
    part: &'a str,					// 如果part引用字段无效，那么结构体实例也就失效 防止产生悬垂引用		
}

fn main() {
    let novel = String::from("Call me Ishmael. Some years ago...");
    let first_sentence = novel.split('.').next().unwrap();
    let i = ImportantExcerpt {
        part: first_sentence,
    };
}
```

## 方法中的生命周期

```rust
impl<'a> ImportantExcerpt<'a> {
    fn level(&self) -> i32 {										// 无需引用
        3
    }
   fn announce_and_return_part(&self, announcement: &str) -> &str {	// 规则
       println!("Attention please: {announcement}");
       self.part
}
```

# 自动化测试

Rust 中的测试就是一个带有 `test` 属性注解的函数。为了将一个函数变成测试函数，需要在 `fn` 行之前加上 `#[test]`。

当使用 `cargo test` 命令运行测试时，Rust 会构建一个测试执行程序用来调用被标注的函数，并报告每一个测试是通过还是失败。

## 测试模板与函数

**默认测试**

每次使用 Cargo 新建一个库项目时，会自动生成一个测试模块和一个测试函数。

新建一个库项目 `cargo new adder --lib`。

`src/lib.rs`

```rust
pub fn add(left: u64, right: u64) -> u64 {
    left + right
}

#[cfg(test)]					// 告诉 Rust 接下来的项只有在给定特定配置选项时，才会被包含
// 在这种情况下，配置选项是 test
// 通过使用 cfg 属性，Cargo 只会在主动使用 cargo test 运行测试时才编译测试代码
mod tests {
    use super::*;

    #[test]						// 这个属性表明这是一个测试函数 
    fn it_works() {				// 必须每次都标明哪些函数是测试
        let result = add(2, 2);
        assert_eq!(result, 4);	// 来断言 result 来验证代码是否符合编码人员的预期
    }
}
```

调用`cargo test`命令。

```shell
$ cargo test
   Compiling adder v0.1.0 (file:///projects/adder)
    Finished `test` profile [unoptimized + debuginfo] target(s) in 0.57s
     Running unittests src/lib.rs (target/debug/deps/adder-01ad14159ff659ab)

running 1 test
test tests::it_works ... ok

test result: ok. 1 passed; 0 failed; 0 ignored; 0 measured; 0 filtered out; finished in 0.00s			# 测试情况

   Doc-tests adder			# 所有文档测试的结果

running 0 tests

test result: ok. 0 passed; 0 failed; 0 ignored; 0 measured; 0 filtered out; finished in 0.00s
```

- ignored：将一个测试标记为忽略以便在特定情况下不会运行
- measured：针对性能测试
- filtered out：将参数传递给 `cargo test` 命令，以便只运行名称与字符串匹配的测试

**自定义测试**

`src/lib.rs`

```rust
pub fn add(left: u64, right: u64) -> u64 {
    left + right
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn exploration() {
        let result = add(2, 2);
        assert_eq!(result, 4);
    }

    #[test]
    fn another() {
        panic!("Make this test fail");
    }
}
```

```shell
$ cargo test
   Compiling adder v0.1.0 (file:///projects/adder)
    Finished `test` profile [unoptimized + debuginfo] target(s) in 0.72s
     Running unittests src/lib.rs (target/debug/deps/adder-92948b65e88960b4)

running 2 tests
test tests::another ... FAILED		# 测试失败
test tests::exploration ... ok		# 测试通过

failures:

---- tests::another stdout ----

thread 'tests::another' panicked at src/lib.rs:17:9:
Make this test fail
note: run with `RUST_BACKTRACE=1` environment variable to display a backtrace


failures:
    tests::another

test result: FAILED. 1 passed; 1 failed; 0 ignored; 0 measured; 0 filtered out; finished in 0.00s					# 本次测试情况

error: test failed, to rerun pass `--lib`

```

注

1. 当测试函数中出现 panic 时测试就失败了
2. 每一个测试都在一个新线程中运行，当主线程发现测试线程异常了，就将对应测试标记为失败

**测试中使用 Result<T, E>**

使用 `Result<T, E>` 重写，并在失败时返回 `Err` 而非 panic。

```rust
#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    // 不同于调用 assert_eq! 宏，而是在测试通过时返回 Ok(())，在测试失败时返回带有 String 的 Err
    fn it_works() -> Result<(), String> {
        let result = add(2, 2);

        if result == 4 {
            Ok(())
        } else {
            Err(String::from("two plus two does not equal four"))
        }
    }
}
```

注

1. 不能对这些使用 `Result<T, E>` 的测试使用 `#[should_panic]` 注解
2. 为了断言一个操作返回 `Err` 成员，不要对 `Result<T, E>` 值使用问号表达式（`?`）。而是使用 `assert!(value.is_err())`

## 测试相关宏与属性

**assert! 宏**

由标准库提供，需向 `assert!` 宏提供一个布尔值的参数。如果值是 `true`，测试通过。如果值为 `false`，`assert!` 调用 `panic!` 宏，这会导致测试失败。

`src/lib.rs`

```rust
#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn larger_can_hold_smaller() {
        let larger = Rectangle {
            width: 8,
            height: 7,
        };
        let smaller = Rectangle {
            width: 5,
            height: 1,
        };

        assert!(larger.can_hold(&smaller));
    }

    #[test]
    fn smaller_cannot_hold_larger() {
        let larger = Rectangle {
            width: 8,
            height: 7,
        };
        let smaller = Rectangle {
            width: 5,
            height: 1,
        };

        assert!(smaller.can_hold(&larger));
    }
}
```

```shell
$ cargo test
   Compiling rectangle v0.1.0 (file:///projects/rectangle)
    Finished `test` profile [unoptimized + debuginfo] target(s) in 0.66s
     Running unittests src/lib.rs (target/debug/deps/rectangle-6584c4561e48942e)

running 2 tests
test tests::larger_can_hold_smaller ... FAILED
test tests::smaller_cannot_hold_larger ... ok

failures:

---- tests::larger_can_hold_smaller stdout ----

thread 'tests::larger_can_hold_smaller' panicked at src/lib.rs:28:9:
assertion failed: larger.can_hold(&smaller)
note: run with `RUST_BACKTRACE=1` environment variable to display a backtrace


failures:
    tests::larger_can_hold_smaller

test result: FAILED. 1 passed; 1 failed; 0 ignored; 0 measured; 0 filtered out; finished in 0.00s

error: test failed, to rerun pass `--lib`
```

**assert_eq! 和 assert_ne! 宏**

分别比较两个值是相等还是不相等。当断言失败时它们也会打印出这两个值具体是什么，而 `assert!` 只会打印出它从 `==` 表达式中得到了 `false` 值，而不是打印导致 `false` 的具体值。

`src/lib.rs`

```rust
pub fn add_two(a: usize) -> usize {
    a + 3
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn it_adds_two() {
        let result = add_two(2);
        assert_eq!(result, 4);					// 断言相等
    }											
}
```

```shell
$ cargo test
   Compiling adder v0.1.0 (file:///projects/adder)
    Finished `test` profile [unoptimized + debuginfo] target(s) in 0.61s
     Running unittests src/lib.rs (target/debug/deps/adder-92948b65e88960b4)

running 1 test
test tests::it_adds_two ... FAILED

failures:

---- tests::it_adds_two stdout ----

thread 'tests::it_adds_two' panicked at src/lib.rs:12:9:
assertion `left == right` failed		# 返回具体数值
  left: 5
 right: 4
note: run with `RUST_BACKTRACE=1` environment variable to display a backtrace


failures:
    tests::it_adds_two

test result: FAILED. 0 passed; 1 failed; 0 ignored; 0 measured; 0 filtered out; finished in 0.00s

error: test failed, to rerun pass `--lib`

```

注

1. 对于自定义的结构体和枚举，需要实现 `PartialEq` 才能断言它们的值是否相等。需要实现 `Debug` 才能在断言失败时打印它们的值，通常可以直接在结构体或枚举上添加 `#[derive(PartialEq, Debug)]` 注解

**自定义失败信息**

也可以向 `assert!`、`assert_eq!` 和 `assert_ne!` 宏传递一个可选的失败信息参数，可以在测试失败时将自定义失败信息一同打印出来。

`src/lib.rs`

```rust
pub fn greeting(name: &str) -> String {
    String::from("Hello!")
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn greeting_contains_name() {
        let result = greeting("Carol");
        assert!(	// 增加一个自定义失败信息参数
            		// 带占位符的格式字符串，以及 greeting 函数的值
            result.contains("Carol"),
            "Greeting did not contain name, value was `{result}`"
        );
    }
}
```

```shell
$ cargo test
   Compiling greeter v0.1.0 (file:///projects/greeter)
    Finished `test` profile [unoptimized + debuginfo] target(s) in 0.93s
     Running unittests src/lib.rs (target/debug/deps/greeter-170b942eb5bf5e3a)

running 1 test
test tests::greeting_contains_name ... FAILED

failures:

---- tests::greeting_contains_name stdout ----

thread 'tests::greeting_contains_name' panicked at src/lib.rs:12:9:
Greeting did not contain name, value was `Hello!`	# 在测试输出中看到所取得的值
note: run with `RUST_BACKTRACE=1` environment variable to display a backtrace


failures:
    tests::greeting_contains_name

test result: FAILED. 0 passed; 1 failed; 0 ignored; 0 measured; 0 filtered out; finished in 0.00s

error: test failed, to rerun pass `--lib`
```

**should_panic**

除了检查返回值之外，检查代码是否按照期望处理错误也是很重要的。

可以通过对函数增加另一个属性 `should_panic` 来实现。这个属性在函数中的代码 panic 时会通过，而在其中的代码没有 panic 时失败。

`src/lib.rs`

```rust
pub struct Guess {
    value: i32,
}

impl Guess {
    pub fn new(value: i32) -> Guess {
        if value < 1 || value > 100 {
            panic!("Guess value must be between 1 and 100, got {value}.");
        }
        Guess { value }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    #[should_panic]					// 检查panic的属性
    fn greater_than_100() {
        Guess::new(200);
    }
}
```

```shell
$ cargo test
   Compiling guessing_game v0.1.0 (file:///projects/guessing_game)
    Finished `test` profile [unoptimized + debuginfo] target(s) in 0.58s
     Running unittests src/lib.rs (target/debug/deps/guessing_game-57d70c3acb738f4d)

running 1 test
test tests::greater_than_100 - should panic ... ok	# 测试通过，按期发生panic

test result: ok. 1 passed; 0 failed; 0 ignored; 0 measured; 0 filtered out; finished in 0.00s

   Doc-tests guessing_game

running 0 tests

test result: ok. 0 passed; 0 failed; 0 ignored; 0 measured; 0 filtered out; finished in 0.00s
```

`should_panic` 甚至在一些不是所期望的原因而导致 panic 时也会通过。

为了使 `should_panic` 测试结果更精确，可以给 `should_panic` 属性增加一个可选的 `expected` 参数。测试工具会确保错误信息中包含其提供的文本。

`src/lib.rs`

```rust
pub struct Guess {
    value: i32,
}

impl Guess {
    pub fn new(value: i32) -> Guess {
        if value < 1 {
            panic!(
                "Guess value must be greater than or equal to 1, got {value}."
            );
        } else if value > 100 {
            panic!(
                "Guess value must be less than or equal to 100, got {value}."
            );
        }
        Guess { value }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    #[should_panic(expected = "less than or equal to 100")]	// 增加限制
    fn greater_than_100() {
        Guess::new(200);									// 测试会通过
      // Guess::new(0);										// 测试不会通过
    }
}
```

## 控制测试如何运行

`cargo test` 在测试模式下编译代码并运行生成的测试二进制文件。指定命令行参数来改变 `cargo test` 的默认行为。

先列出传递给 `cargo test` 的参数，接着是分隔符 `--`，再之后是传递给测试二进制文件的参数。

- `cargo test --help`：提示 `cargo test` 的有关参数
- `cargo test -- --help`： 提示在分隔符之后使用的有关参数

**顺序运行测试**

当运行多个测试时，Rust 默认使用线程来并行运行。因为测试是在同时运行的，所以应该确保测试不能相互依赖。

如果不希望测试并行运行，或想要更加精确的控制线程的数量，可以传递 `--test-threads` 参数和希望使用线程的数量给测试二进制文件。

```shell
$ cargo test -- --test-threads=1			# 将测试线程设置为 1 
# 比并行运行花费更多时间，不过在有共享的状态时，测试就不会潜在的相互干扰了
```

**显示函数输出**

默认情况下，当测试通过时，Rust 的测试库会截获打印到标准输出的所有内容。

比如在测试中调用了 `println!` 而测试通过了，我们将不会在终端看到 `println!` 的输出。只会看到说明测试通过的提示行。如果测试失败了，则会看到所有标准输出和其他错误信息。

`src/lib.rs`

```rust
fn prints_and_returns_10(a: i32) -> i32 {
    println!("I got the value {a}");
    10
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn this_test_will_pass() {
        let value = prints_and_returns_10(4);
        assert_eq!(value, 10);
    }

    #[test]
    fn this_test_will_fail() {
        let value = prints_and_returns_10(8);
        assert_eq!(value, 5);
    }
}
```

```shell
$ cargo test
   Compiling silly-function v0.1.0 (file:///projects/silly-function)
    Finished `test` profile [unoptimized + debuginfo] target(s) in 0.58s
     Running unittests src/lib.rs (target/debug/deps/silly_function-160869f38cff9166)

running 2 tests
test tests::this_test_will_fail ... FAILED
test tests::this_test_will_pass ... ok

failures:

---- tests::this_test_will_fail stdout ----
I got the value 8						# 仅打印未通过的测试输出

thread 'tests::this_test_will_fail' panicked at src/lib.rs:19:9:
assertion `left == right` failed
  left: 10
 right: 5
note: run with `RUST_BACKTRACE=1` environment variable to display a backtrace


failures:
    tests::this_test_will_fail

test result: FAILED. 1 passed; 1 failed; 0 ignored; 0 measured; 0 filtered out; finished in 0.00s

error: test failed, to rerun pass `--lib`
```

如果希望也能看到通过的测试中打印的值，也可以在结尾加上 `--show-output` 告诉 Rust 显示成功测试的输出。

```shell
$ cargo test -- --show-output
   Compiling silly-function v0.1.0 (file:///projects/silly-function)
    Finished `test` profile [unoptimized + debuginfo] target(s) in 0.60s
     Running unittests src/lib.rs (target/debug/deps/silly_function-160869f38cff9166)

running 2 tests
test tests::this_test_will_fail ... FAILED
test tests::this_test_will_pass ... ok

successes:

---- tests::this_test_will_pass stdout ----
I got the value 4


successes:
    tests::this_test_will_pass

failures:

---- tests::this_test_will_fail stdout ----
I got the value 8

thread 'tests::this_test_will_fail' panicked at src/lib.rs:19:9:
assertion `left == right` failed
  left: 10
 right: 5
note: run with `RUST_BACKTRACE=1` environment variable to display a backtrace


failures:
    tests::this_test_will_fail

test result: FAILED. 1 passed; 1 failed; 0 ignored; 0 measured; 0 filtered out; finished in 0.00s

error: test failed, to rerun pass `--lib`
```

**通过名称运行部分测试**

有时运行整个测试集会耗费很长时间。如果负责特定位置的代码，可能会希望只运行与这些代码相关的测试。

`src/lib.rs`

```rust
pub fn add_two(a: usize) -> usize {
    a + 2
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn add_two_and_two() {
        let result = add_two(2);
        assert_eq!(result, 4);
    }

    #[test]
    fn add_three_and_two() {
        let result = add_two(3);
        assert_eq!(result, 5);
    }

    #[test]
    fn one_hundred() {
        let result = add_two(100);
        assert_eq!(result, 102);
    }
}
```

运行单个测试

```shell
$ cargo test one_hundred
   Compiling adder v0.1.0 (file:///projects/adder)
    Finished `test` profile [unoptimized + debuginfo] target(s) in 0.69s
     Running unittests src/lib.rs (target/debug/deps/adder-92948b65e88960b4)

running 1 test						# 只有名称为 one_hundred 的测试被运行了
test tests::one_hundred ... ok

test result: ok. 1 passed; 0 failed; 0 ignored; 0 measured; 2 filtered out; finished in 0.00s					# 其余两个测试被过滤，未运行
```

运行特定多个测试

```shell
$ cargo test add
   Compiling adder v0.1.0 (file:///projects/adder)
    Finished `test` profile [unoptimized + debuginfo] target(s) in 0.61s
     Running unittests src/lib.rs (target/debug/deps/adder-92948b65e88960b4)

running 2 tests								# 任何名称匹配 add 的测试会被运行
test tests::add_three_and_two ... ok
test tests::add_two_and_two ... ok

test result: ok. 2 passed; 0 failed; 0 ignored; 0 measured; 1 filtered out; finished in 0.00s
```

注

1. 可以通过过滤模块名来运行一个模块中的所有测试

**除非特别指定否则忽略某些测试**

可以使用 `ignore` 属性来标记耗时的测试并排除它们。

`src/lib.rs`

```rust
#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn it_works() {
        let result = add(2, 2);
        assert_eq!(result, 4);
    }

    #[test]
    #[ignore]				// 默认不会被运行
    fn expensive_test() {
        // code that takes an hour to run
    }
}
```

```shell
$ cargo test
   Compiling adder v0.1.0 (file:///projects/adder)
    Finished `test` profile [unoptimized + debuginfo] target(s) in 0.60s
     Running unittests src/lib.rs (target/debug/deps/adder-92948b65e88960b4)

running 2 tests
test tests::expensive_test ... ignored
test tests::it_works ... ok

test result: ok. 1 passed; 0 failed; 1 ignored; 0 measured; 0 filtered out; finished in 0.00s

   Doc-tests adder

running 0 tests

test result: ok. 0 passed; 0 failed; 0 ignored; 0 measured; 0 filtered out; finished in 0.00s
```

如果只希望运行被忽略的测试，可以使用 `cargo test -- --ignored`

```shell
$ cargo test -- --ignored
   Compiling adder v0.1.0 (file:///projects/adder)
    Finished `test` profile [unoptimized + debuginfo] target(s) in 0.61s
     Running unittests src/lib.rs (target/debug/deps/adder-92948b65e88960b4)

running 1 test
test expensive_test ... ok

test result: ok. 1 passed; 0 failed; 0 ignored; 0 measured; 1 filtered out; finished in 0.00s

   Doc-tests adder

running 0 tests

test result: ok. 0 passed; 0 failed; 0 ignored; 0 measured; 0 filtered out; finished in 0.00s
```

如果希望不管是否忽略都要运行全部测试，可以运行 `cargo test -- --include-ignored`。

## 测试的组织结构

**单元测试**

单元测试的目的是在与其他部分隔离的环境中测试每一个单元的代码，以便于快速而准确地验证某个单元的代码功能是否符合预期。

单元测试与它们要测试的代码共同存放在位于 src 目录下相同的文件中。规范是在每个文件中创建包含测试函数的 `tests` 模块，并使用 `cfg(test)` 标注模块。

测试私有函数

`src/lib.rs`

```rust
pub fn add_two(a: usize) -> usize {
    internal_adder(a, 2)
}

fn internal_adder(left: usize, right: usize) -> usize {	// 待测私有函数
    left + right
}

#[cfg(test)]
mod tests {
    use super::*; // 子模块的项可以使用其上级模块的项，故可以调用私有函数internal_adder

    #[test]
    fn internal() {
        let result = internal_adder(2, 2);
        assert_eq!(result, 4);
    }
}
```

**集成测试**

集成测试的目的是测试库的多个部分能否一起正常工作。一些单独能正确运行的代码单元集成在一起也可能会出现问题，所以集成测试的覆盖率也是很重要的。

为了创建集成测试，需要先创建一个 tests 目录。接着可以随意在这个目录中创建任意多的测试文件，Cargo 会将每一个文件当作单独的 crate 来编译。

```shell
package_name
├── Cargo.lock
├── Cargo.toml
├── src
│   └── lib.rs
└── tests
    └── integration_test.rs
```

`tests/integration_test.rs`

```rust
use adder::add_two;
// 因为每一个 tests 目录中的测试文件都是完全独立的 crate，所以需要将库引入到每个测试 crate 的作用域中

// 并不需要将 tests/integration_test.rs 中的任何代码标注为 #[cfg(test)]
// tests 文件夹在 Cargo 中是一个特殊的文件夹，Cargo 只会在运行 cargo test 时编译这个目录中的文件

#[test]
fn it_adds_two() {
    let result = add_two(2);
    assert_eq!(result, 4);
}
```

```shell
$ cargo test
   Compiling adder v0.1.0 (file:///projects/adder)
    Finished `test` profile [unoptimized + debuginfo] target(s) in 1.31s
     Running unittests src/lib.rs (target/debug/deps/adder-1082c4b063a8fbe6)

running 1 test					# 每个单元测试一行
test tests::internal ... ok	

test result: ok. 1 passed; 0 failed; 0 ignored; 0 measured; 0 filtered out; finished in 0.00s

     Running tests/integration_test.rs (target/debug/deps/integration_test-1082c4b063a8fbe6)				# 集成测试
 								 # 每一行是一个集成测试中的测试函数

running 1 test
test it_adds_two ... ok

test result: ok. 1 passed; 0 failed; 0 ignored; 0 measured; 0 filtered out; finished in 0.00s

   Doc-tests adder				# 文档测试

running 0 tests

test result: ok. 0 passed; 0 failed; 0 ignored; 0 measured; 0 filtered out; finished in 0.00ss
```

注

1. 如果一个单元测试失败，则不会有任何集成测试和文档测试的输出，因为这些测试只会在所有单元测试都通过后才会执行
2. 仍然可以通过指定测试函数的名称作为 `cargo test` 的参数来运行特定集成测试。也可以使用 `cargo test` 的 `--test` 后跟文件的名称来运行某个特定集成测试文件中的所有测试

```shell
$ cargo test --test integration_test		# integration_test.rs 文件
   Compiling adder v0.1.0 (file:///projects/adder)
    Finished `test` profile [unoptimized + debuginfo] target(s) in 0.64s
     Running tests/integration_test.rs (target/debug/deps/integration_test-82e7799c1bc62298)

running 1 test
test it_adds_two ... ok

test result: ok. 1 passed; 0 failed; 0 ignored; 0 measured; 0 filtered out; finished in 0.00s
```

如果集成测试中有公共函数部分，可以将其在`tests`目录中新建一个目录，并在其中创建文件`mod.rs`

文件结构

```shell
├── Cargo.lock
├── Cargo.toml
├── src
│   └── lib.rs
└── tests
    ├── common
    │   └── mod.rs
    └── integration_test.rs
```

``tests/integration_test.rs`

```rust
use adder::add_two;

mod common;

#[test]
fn it_adds_two() {
    common::setup();			// 调用公共函数

    let result = add_two(2);
    assert_eq!(result, 4);
}
```

**二进制crate的集成测试**

如果项目是二进制 crate 并且只包含 src/main.rs 而没有 src/lib.rs，这样就不可能在 tests 目录创建集成测试并也无法通过 `use` 语句将 src/main.rs 中定义的函数引入作用域。

只有库 crate 才会向其他 crate 暴露了可供调用和使用的函数，二进制 crate 只意在单独运行。

# I/O项目

- minigrep：在指定文件中搜索指定字符串并输出匹配的行

**函数分离**

- 每个函数只负责一个任务
- main.rs 处理程序运行、配置等，lib.rs 处理任务逻辑

**stdout与stderr**

- 标准输出（standard output，`stdout`）：对应一般信息，可重定向到文件中
- 标准错误（standard error，`stderr`）：用于错误信息，只显示在终端

```shell
$ cargo run > output.txt			# 将标准输出重定向到指定文件
```

`src/lib.rs`

```rust
use std::error::Error;
use std::fs;
use std::env;

pub struct Config {
    pub query: String,
    pub file_path: String,
    pub ignore_case: bool,
}

/* 命令行参数验证处理 */
impl Config {
    pub fn build(mut args: impl Iterator<Item = String>) -> Result<Config, &'static str> {
   		// 须调用next方法，因此迭代器为mut
        args.next();										// 迭代器首个为项目名称

        let query = match args.next() {
            Some(arg) => arg,
            None => return Err("Didn't get a query string"),
        };

        let file_path = match args.next() {
            Some(arg) => arg,
            None => return Err("Didn't get a file path"),
        };

        let ignore_case = env::var("IGNORE_CASE").is_ok();	// 根据环境变量设值	
        // is_ok 会根据是否设置环境变量来返回 true 或 false 

        Ok(Config {
            query,
            file_path,
            ignore_case,
        })
    }
}

/* 读取文件 */
pub fn run(config: Config) -> Result<(), Box<dyn Error>> {
    // Box<dyn Error> 会返回实现了 Error trait 的类型
    // 提供了在不同的错误场景可能有不同类型的错误返回值的灵活性
    let contents = fs::read_to_string(config.file_path)?;

    let results = if config.ignore_case {
        search_case_insensitive(&config.query, &contents)
    } else {
        search(&config.query, &contents)
    };

    for line in results {
        println!("{line}");
    }
    Ok(())
}

/* 大小写敏感 */    
pub fn search<'a>(query: &str, contents: &'a str) -> Vec<&'a str> {
    contents
        .lines()	// lines 方法返回一个迭代器		
        .filter(|line| line.contains(query))
        .collect()	// 迭代器结果返回一个集合
}



/* 大小写不敏感 */
pub fn search_case_insensitive<'a>(query: &str, contents: &'a str) -> Vec<&'a str> {
    let query = query.to_lowercase();    // 待搜寻字符串转为小写
    contents
        .lines()
        .filter(|line| line.to_lowercase().contains(&query))
        .collect()
}

/* 测试 */
#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn one_result() {
        let query = "duct";
        let contents = "\				// 不要在字符串字面值内容的开头加入换行符
Rust:
safe, fast, productive.
Pick three.";

        assert_eq!(vec!["safe, fast, productive."], search(query, contents));
    }
}
```

`src/main.rs`

```rust
use std::{env, process};
use package_name::Config;

fn main() {
    /* 接受命令行参数 */
    let config = Config::build(env::args()).unwrap_or_else(|err| {
        eprintln!("Problem parsing arguments: {err}");		// 打印到标准错误流
        process::exit(1);
      	// 立即停止程序并将传递给它的数字作为退出状态码
    });
    // env::args() 返回一个传递给程序的命令行参数的迭代器
    
    if let Err(e) = gussing_game::run(config) {
        eprintln!("Application error: {e}");
        process::exit(1);
    }
}
```

# 闭包

闭包是可以保存在变量中或作为参数传递给其他函数的匿名函数。

**基本使用**

- 通常不需要对参数和返回值进行类型注解
- 通常很短，只在有限的上下文中使用，以便编译器会为闭包的每个参数和返回值推断出一个具体类型
- 不会暴露给用户的接口中使用
- 如果闭包体只有一个表达式，那么大括号是可选的

```rust
fn  add_one_v1   (x: u32) -> u32 { x + 1 }		// ✅
let add_one_v2 = |x: u32| -> u32 { x + 1 };		// ✅
let add_one_v3 = |x|             { x + 1 };		// ✅
let add_one_v4 = |x|               x + 1  ;		// ✅


let expensive_closure = |num: u32| -> u32 {		// ✅
    println!("calculating slowly...");
    thread::sleep(Duration::from_secs(2));
    num
};
```

如果尝试对同一闭包使用不同类型则就会得到类型错误。

```rust
let example_closure = |x| x;						// 定义闭包

let s = example_closure(String::from("hello"));		// 类型被锁定进闭包
let n = example_closure(5);							// ❌
```

**捕获方式**

闭包可以通过三种方式捕获其环境中的值：不可变借用、可变借用和获取所有权。

闭包将根据函数体中对捕获值的操作来决定使用哪种方式。

捕获不可变引用

```rust
fn main() {
    let list = vec![1, 2, 3];
    println!("Before defining closure: {list:?}");

    let only_borrows = || println!("From closure: {list:?}");	// 只是打印，因此属于捕获不可变引用的闭包

    println!("Before calling closure: {list:?}");
    only_borrows();
    println!("After calling closure: {list:?}");
}
```

捕获可变引用

```rust
fn main() {
    let mut list = vec![1, 2, 3];
    println!("Before defining closure: {list:?}");

    let mut borrows_mutably = || list.push(7);					// 可变引用的闭包
	println!("After calling closure: {list:?}");				// ❌ 不允许有不可变引用
    borrows_mutably();
    // 闭包在被调用后就不再被使用，这时可变借用结束
    println!("After calling closure: {list:?}");
}
```

获取所有权

```rust
use std::thread;

fn main() {
    let list = vec![1, 2, 3];
    println!("Before defining closure: {list:?}");				// 不可变引用的闭包

    thread::spawn(move || println!("From thread: {list:?}"))	// move关键字，list所有权被移动到闭包中，在这里即新子线程中
    // 确保子线程中使用list是有效的
    .join()
    .unwrap();
}
```

**Fn trait**

闭包体可以对捕获的值进行以下操作

- 将捕获的值移出闭包
- 修改捕获的值
- 既不移动也不修改值
- 不从环境中捕获任何值

根据闭包体如何处理捕获的值，闭包会自动实现一个、两个或三个 `Fn` trait，它们是继承的关系。

- `FnOnce`： 适用于只能被调用一次的闭包，所有闭包至少都实现了这个 trait。一个会将捕获的值从闭包体中移出的闭包只会实现 `FnOnce` trait，而不会实现其他 `Fn` 相关的 trait，因为它只能被调用一次
- `FnMut` ：适用于不会移出值，但可能会修改捕获值的闭包，这类闭包可以被调用多次
- `Fn`： 适用于既不移出值，也不修改值的闭包，也包括不从环境中捕获任何值的闭包

`unwrap_or_else`定义

```rust
impl<T> Option<T> {
    /* 参数是Some返回值，如果是None则执行其中的闭包 */
    pub fn unwrap_or_else<F>(self, f: F) -> T	
    where							// f 是调用 unwrap_or_else 时提供的闭包
        F: FnOnce() -> T			// 所有闭包均可作为其参数
    {
        match self {
            Some(x) => x,
            None => f(),
        }
    }
}
```

`sort_by_key`举例

```rust
#[derive(Debug)]
struct Rectangle {
    width: u32,
    height: u32,
}

fn main() {
    let mut list = [
        Rectangle { width: 10, height: 1 },
        Rectangle { width: 3, height: 5 },
        Rectangle { width: 7, height: 12 },
    ];

    list.sort_by_key(|r| r.width);				// 按键进行排序，可被多次调用，闭包须实现 FnMut
    println!("{list:#?}");
}
```

输出

```shell
[
    Rectangle {
        width: 3,
        height: 5,
    },
    Rectangle {
        width: 7,
        height: 12,
    },
    Rectangle {
        width: 10,
        height: 1,
    },
]
```

```rust
#[derive(Debug)]
struct Rectangle {
    width: u32,
    height: u32,
}

fn main() {
    let mut list = [
        Rectangle { width: 10, height: 1 },
        Rectangle { width: 3, height: 5 },
        Rectangle { width: 7, height: 12 },
    ];

    let mut sort_operations = vec![];
    let value = String::from("closure called");

    list.sort_by_key(|r| {							// ❌ 只实现了FnOnce
        sort_operations.push(value);				// 将 value 所有权移出给 sort_operations
        r.width
    });
    println!("{list:#?}");
}
```

```rust
#[derive(Debug)]
struct Rectangle {
    width: u32,
    height: u32,
}

fn main() {
    let mut list = [
        Rectangle { width: 10, height: 1 },
        Rectangle { width: 3, height: 5 },
        Rectangle { width: 7, height: 12 },
    ];

    let mut num_sort_operations = 0;
    list.sort_by_key(|r| {							// ✅ 实现了FnMut
        num_sort_operations += 1;
        r.width
    });
    println!("{list:#?}, sorted in {num_sort_operations} operations");
}
```

# 迭代器

迭代器（iterator）负责遍历序列中的每一项并确定序列何时结束的逻辑。

迭代器并不引入运行时性能损失，且略优于for循环。

**基本使用**

```rust
fn main() {
    let v1 = vec![1, 2, 3];
    let v1_iter = v1.iter();
    for val in v1_iter {
        println!("Got: {val}");
    }
}
```

输出

```shell
Got: 1
Got: 2
Got: 3
```

**Iterator trait 和 next 方法**

迭代器都实现了名为 `Iterator` 的定义于标准库的 trait。

```rust
pub trait Iterator {
    type Item;	// Item 类型是迭代器返回元素的类型

    fn next(&mut self) -> Option<Self::Item>;	// 必须指明迭代器返回的类型

    // 此处省略了方法的默认实现
}
```

`Iterator` trait 要求实现者定义一个方法：`next` 方法，该方法每次返回迭代器中的一个项，封装在 `Some` 中，当迭代完成时，返回 `None`。

```rust
#[test]
fn iterator_demonstration() {
    let v1 = vec![1, 2, 3];

    let mut v1_iter = v1.iter();			// 在迭代器上调用 next 方法会改变迭代器内部的状态，因此为可变

    assert_eq!(v1_iter.next(), Some(&1));	// 从 next 调用中获取的值是对 vector 中值的不可变引用
    assert_eq!(v1_iter.next(), Some(&2));
    assert_eq!(v1_iter.next(), Some(&3));
    assert_eq!(v1_iter.next(), None);
}
```

注

1. iter()：不可变引用的迭代器
2. iter_mut ()：遍历可变引用
3. into_iter()：能获取`v1` 所有权，并返回拥有所有权的值

**消耗性适配器**

- Iterator trait 中一些方法在定义中调用 next 方法，被称为消耗性适配器

`sum`

这个方法获取迭代器的所有权并反复调用 `next` 来遍历迭代器，它将每个项累加到一个运行时总和中，并在迭代完成时返回这个总和。

```rust
#[test]
fn iterator_sum() {
    let v1 = vec![1, 2, 3];

    let v1_iter = v1.iter();

    let total: i32 = v1_iter.sum();
	// 调用 sum 之后不再允许使用 v1_iter 因为调用 sum 时它会获取迭代器的所有权
    assert_eq!(total, 6);	
}
```

**迭代器适配器**

- 迭代器适配器是定义在 Iterator trait 中不会消耗原始迭代器的方法
- 通过修改原始迭代器来产生新的迭代器

`map`

该方法使用一个闭包对每个元素进行操作，返回一个新的迭代器，该迭代器生成经过修改的元素。

```rust
fn main() {
    let v1: Vec<i32> = vec![1, 2, 3];

    let v2: Vec<_> = v1.iter().map(|x| x + 1).collect();
    // collect 方法消费迭代器并将结果收集到一个集合数据类型中

    assert_eq!(v2, vec![2, 3, 4]);

}
```

注

1. Rust中迭代器是惰性的（lazy），这意味着在调用消费迭代器的方法之前不会执行任何操作
2. 因为所有的迭代器都是惰性的，必须调用一个消费适配器方法，才能从这些迭代器方法的调用中获取结果

**使用捕获其环境的闭包**

很多迭代器适配器都接受闭包作为参数，而通常会指定捕获其环境的闭包作为迭代器适配器的参数。

`filter`

```rust
#[derive(PartialEq, Debug)]
struct Shoe {
    size: u32,
    style: String,
}

fn shoes_in_size(shoes: Vec<Shoe>, shoe_size: u32) -> Vec<Shoe> {
    shoes.into_iter().filter(|s| s.size == shoe_size).collect()
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn filters_by_size() {
        let shoes = vec![
            Shoe {
                size: 10,
                style: String::from("sneaker"),
            },
            Shoe {
                size: 13,
                style: String::from("sandal"),
            },
            Shoe {
                size: 10,
                style: String::from("boot"),
            },
        ];

        let in_my_size = shoes_in_size(shoes, 10);

        assert_eq!(
            in_my_size,
            vec![
                Shoe {
                    size: 10,
                    style: String::from("sneaker")
                },
                Shoe {
                    size: 10,
                    style: String::from("boot")
                },
            ]
        );
    }
}
```

# 智能指针

指针是一个包含内存地址的变量的通用概念，这个地址引用或指向一些其它数据。Rust 中最常见的指针是引用，引用以 `&` 符号为标志并借用了它们所指向的值，除了引用数据没有任何其他特殊功能，也没有额外开销。

- 智能指针（smart pointers）是一类数据结构，通常由结构体实现，它们类似指针，但拥有额外的元数据和功能
- 引用只是借用数据的指针，而智能指针大部分拥有它们指向的数据
- `Deref` trait：允许智能指针被当作引用对待
-  `Drop` trait ：当智能指针离开作用域时，允许用户自定义行为

## Deref trait

实现 `Deref` trait 允许定制解引用运算符 `*`，实现 `Deref` trait 的智能指针可以被当作常规引用来使用。

常规引用是一个指针类型。

```rust
fn main() {
    let x = 5;
    let y = &x;			// x 的一个引用

    assert_eq!(5, x);
    assert_eq!(5, *y);	// * 解引用
}
```

可以使用 `Box<T>` 代替引用来重写。

```rust
fn main() {
    let x = 5;
    let y = Box::new(x);	// 指向x的Box实例

    assert_eq!(5, x);
    assert_eq!(5, *y);
}
```

**显示解引用**

```rust
struct MyBox<T>(T);			// 包含一个元素的元组结构体

impl<T> MyBox<T> {
    fn new(x: T) -> MyBox<T> {
        MyBox(x)
    }
}

fn main() {
    let x = 5;
    let y = MyBox::new(x);	// ❌ Rust 不知道如何解引用 MyBox

    assert_eq!(5, x);
    assert_eq!(5, *y);
}
```

为了使用 `*` 运算符的解引用功能，需要实现 `Deref` trait 中的`deref` 方法，来为自定义类型实现解引用的能力。

```rust
use std::ops::Deref;

impl<T> Deref for MyBox<T> {
    type Target = T;
	// 要求实现名为 deref 的方法
    fn deref(&self) -> &Self::Target {	// 借用 self 并返回一个内部数据的引用
        &self.0							// 访问元组第一个元素
    }
}

fn main() {
    let x = 5;
    let y = MyBox::new(x);				// ✅

    assert_eq!(5, x);
    assert_eq!(5, *y);
    // Rust 在底层运行了如下代码
    // assert_eq!(5, *(y.deref()));			// 先调用 deref 方法再进行普通解引用的操作
}
```

**隐式解引用转换**

Deref 隐式解引用实现了将 `Deref` trait 的类型的引用转换为另一种类型的引用。例如，Deref 强制转换可以将 `&String` 转换为 `&str`。

```rust
fn hello(name: &str) {
    println!("Hello, {name}!");
}

fn main() {
    let m = MyBox::new(String::from("Rust"));	// m 为 MyBox<String> 的引用
    hello(&m);
    // Rust 通过 deref 调用将 &MyBox<String> 变为 &String
    // 标准库中提供了 String 上的 Deref 实现，Rust 再次调用 deref 将 &String 变为 &str
}
```

如果 Rust 没有实现 Deref 隐式解引用转换，则应改写为

```rust
fn main() {
    let m = MyBox::new(String::from("Rust"));
    hello(&(*m)[..]);
}
```

当类型定义了 `Deref` trait，Rust 会分析这些类型并使用任意多次 `Deref::deref` 调用以获得匹配参数的类型。这些解析都发生在编译时，所以利用 Deref 强制转换并没有运行时开销。

**DerefMut trait**

类似 `Deref` trait 重载不可变引用的 `*` 运算符，`DerefMut` trait 用于重载可变引用的 `*` 运算符。

三种解引用场景

- 当 `T: Deref<Target=U>` ： `&T` -> `&U`
- 当 `T: DerefMut<Target=U>` ： `&mut T` -> `&mut U`
- 当 `T: Deref<Target=U>` ： `&mut T` -> `&U`

注

1. 不可变引用永远也不能强转为可变引用，因为无法确保可变引用是否唯一

## Drop trait

Drop trait 允许在值要离开作用域时自定义要执行的操作。例如，当 `Box<T>` 被丢弃时会释放 Box 指向的堆空间。

编译器会自动插入 Drop实现的代码，避免泄漏资源。

`Drop` trait 要求实现 `drop` 方法，参数为 `self` 的可变引用。

```rust
struct CustomSmartPointer {
    data: String,
}

impl Drop for CustomSmartPointer {
    // 实现 drop 方法
    fn drop(&mut self) {
        println!("Dropping CustomSmartPointer with data `{}`!", self.data);
    }
}

fn main() {
    let c = CustomSmartPointer {
        data: String::from("my stuff"),
    };
    let d = CustomSmartPointer {
        data: String::from("other stuff"),
    };
    println!("CustomSmartPointers created.");
    // 无需显式调用 drop 方法
    // 变量以被创建时相反的顺序被丢弃
}
```

输出

```shell
CustomSmartPointers created.
Dropping CustomSmartPointer with data `other stuff`!
Dropping CustomSmartPointer with data `my stuff`!
```

Rust 不允许主动调用 `Drop` trait 的 `drop` 方法。

如果希望提前释放，可以使用 `std::mem::drop` 函数。

```rust
fn main() {
    let c = CustomSmartPointer {
        data: String::from("some data"),
    };
    println!("CustomSmartPointer created.");
    c.drop();		// ❌ 不允许显式调用 drop
    println!("CustomSmartPointer dropped before the end of main.");
}
```

```rust
fn main() {
    let c = CustomSmartPointer {
        data: String::from("some data"),
    };
    println!("CustomSmartPointer created.");
    drop(c);		// ✅ drop 方法被调用了并在此丢弃了 c
    // 此时 c 不具备所有权，因此无法调用 Drop trait 的 drop 方法
    println!("CustomSmartPointer dropped before the end of main.");
}
```

注

1. 通过 `Drop` trait 和 Rust 所有权系统，无需担心之后的代码清理，Rust 会自动完成
2. Rust 确保 `drop` 只会在值不再被使用（无所有权）时被调用一次

## Box\<T\>

Box 允许创建一个值，并将其放在堆上而不是栈上，留在栈上的则是指向堆数据的指针。

Box 只提供了间接存储和堆分配，并没有任何其他特殊的功能。

**适用场景**

- 当有一个在编译时未知大小的类型，而又想要在需要确切大小的上下文中使用这个类型值的时候
- 当有大量数据并希望在确保数据不被拷贝的情况下转移所有权的时候
- 当希望拥有一个值并只关心它的类型是否实现了特定 trait 而不是其具体类型的时候

**基本使用**

```rust
fn main() {
    let mut a = 1;
    let b = Box::new(a);							// 使用 box 在堆上储存一个 i32
    println!("a = {a} \nb = {b}",);
    let addr_a = &a as *const i32 as usize;
    let addr_b = &b as *const Box<i32> as usize;
    a += 1;
    println!("a:{addr_a} \nb:{addr_b}");
}
// b 被释放
```

输出

```shell
a = 2 
b = 1
a:11467476 
b:11467480
```

**创建递归类型**

递归类型（recursive type）的值可以拥有另一个同类型的值作为其自身的一部分，但 Rust 不知道递归类型需要多少空间。所以可以通过在循环类型定义中插入 box，就可以创建递归类型了。

`const list`

```shell
(1, (2, (3, Nil)))			#  cons list 伪代码表示
```

cons list 的每一项都包含两个元素：当前项的值和下一项，其最后一项值包含一个叫做 `Nil` 的值且没有下一项。

```rust
use crate::List::{Cons, Nil};

enum List {
    Cons(i32, List),
    Nil,
}

fn main() {
    let list = Cons(1, Cons(2, Cons(3, Nil)));		// ❌ Rust 无法计算为了存放 List 值需要多少空间
}
```

编译器为了检查占空间最大的变体会导致计算将无限进行下去

<img src=".\img\image-20250718142426213.png" alt="image-20250718142426213" style="zoom:63%;" />

使用Box进行修改

```rust
use crate::List::{Cons, Nil};

enum List {
    Cons(i32, Box<List>),
    Nil,
}
// 指针的大小并不会根据其指向的数据量而改变

fn main() {
    let list = Cons(1, Box::new(Cons(2, Box::new(Cons(3, Box::new(Nil))))));
    // ✅ Cons 变体将会需要一个 i32 的大小加上储存 box 指针数据的空间
}
```

<img src=".\img\image-20250718143512886.png" alt="image-20250718143512886" style="zoom:67%;" />

## Rc\<T\>

有些情况下，一个值可能有多个所有者。

Rc\<T\>可以开启多重所有权，通过记录一个值的引用数量来确定这个值是否被使用，如果有零个引用，就可以被清理。

`Rc<T>` 用于希望在堆上分配一些数据来供程序的多个部分读取，但在编译时无法确定程序哪一部分会最后结束使用它。

**基本使用**

不能用两个 `Box<T>` 的列表尝试共享第三个列表的所有权。

```rust
use crate::List::{Cons, Nil};

enum List {
    Cons(i32, Box<List>),
    Nil,
}

fn main() {
    let a = Cons(5, Box::new(Cons(10, Box::new(Nil))));
    let b = Cons(3, Box::new(a));		// a 被移动进了 b，此时 a 不再可用
    let c = Cons(4, Box::new(a));		// ❌ a 不可用
}
```

修改 `List` 的定义。

```rust
use crate::List::{Cons, Nil};
use std::rc::Rc;

enum List {
    Cons(i32, Rc<List>),
    Nil,
}

fn main() {
    let a = Rc::new(Cons(5, Rc::new(Cons(10, Rc::new(Nil)))));
    let b = Cons(3, Rc::clone(&a));	// 克隆a，引用计数从1增加到2，并允许a和b共享 Rc<List>中数据的所有权
    let c = Cons(4, Rc::clone(&a));	// ✅ 同样会克隆 a，这会将引用计数从2增加为3 
}
```

**克隆 Rc\<T\> 会增加引用计数**

```rust
fn main() {
    let a = Rc::new(Cons(5, Rc::new(Cons(10, Rc::new(Nil)))));
    println!("count after creating a = {}", Rc::strong_count(&a));
    let b = Cons(3, Rc::clone(&a));
    println!("count after creating b = {}", Rc::strong_count(&a));
    {
        let c = Cons(4, Rc::clone(&a));
        println!("count after creating c = {}", Rc::strong_count(&a));
    }
    // Drop trait 的实现当 Rc<T> 值离开作用域时自动减少引用计数
    println!("count after c goes out of scope = {}", Rc::strong_count(&a));
}
```

输出

```shell
count after creating a = 1
count after creating b = 2
count after creating c = 3
count after c goes out of scope = 2
```

注

1. 每次调用 `Rc::clone`，`Rc<List>` 中数据的引用计数都会增加，直到有零个引用之前其数据都不会被清理
2. `Rc::clone` 只会增加引用计数，不会进行深拷贝
3. 使用 `Rc<T>` 允许一个值有多个所有者，引用计数则确保只要任何所有者存在其值保持有效
4. 通过不可变引用， `Rc<T>` 允许在程序的多个部分之间只读地共享数据，为防止违反借用规则，`Rc<T>` 不允许多个可变引用

## RefCell\<T\>

对于引用，如果违反借用规则会得到一个编译错误。而对于 `RefCell<T>`，如果违反这些规则程序会 panic 并退出。

在编译时检查借用规则的优势是这些错误将在开发过程的早期被捕获，同时对运行时没有性能影响。相反在运行时检查借用规则的好处是允许出现特定内存安全的场景，而它们在编译时检查中是不允许的。

`RefCell<T>` 正是用于当确信代码遵守借用规则，而编译器不能理解和确定的时候。

**特点**

- 创建不可变和可变引用时分别使用 `&` 和 `&mut` 语法；对于 `RefCell<T>` 则是 `borrow` 和 `borrow_mut` 方法
- `borrow` 方法返回 `Ref<T>` 类型的智能指针，`borrow_mut` 方法返回 `RefMut<T>` 类型的智能指针
- `RefCell<T>` 记录当前有多少个活动的 `Ref<T>` 和 `RefMut<T>` 智能指针。每次调用 `borrow`，`RefCell<T>` 将活动的不可变借用计数加一，当 `Ref<T>` 值离开作用域时，不可变借用计数减一
- `RefCell<T>` 在任何时候只允许有多个不可变借用或一个可变借用

```rust
impl Messenger for MockMessenger {
    fn send(&self, message: &str) {
        let mut one_borrow = self.sent_messages.borrow_mut();
        let mut two_borrow = self.sent_messages.borrow_mut();	// ❌ 同作用域不允许创建两个可变借用

        one_borrow.push(String::from(message));
        two_borrow.push(String::from(message));
    }
}
// 当运行库的测试时，示例编译不会有错误，不过测试会失败
```

**`Box<T>`，`Rc<T>` 或 `RefCell<T>` 区别**

- `Rc<T>` 允许相同数据有多个所有者；`Box<T>` 和 `RefCell<T>` 则只有单一所有者
- `Box<T>` 允许在编译时执行不可变或可变借用检查；`Rc<T>` 仅允许在编译时执行不可变借用检查；`RefCell<T>` 允许在运行时执行不可变或可变借用检查
- `RefCell<T>` 允许在运行时执行可变借用检查，所以可以在即便 `RefCell<T>` 自身是不可变的情况下修改其内部的值

<img src=".\img\image-20250720130516783.png" alt="image-20250720130516783" style="zoom:50%;" />

**内部可变性**

内部可变性（Interior mutability）是 Rust 中的一个设计模式，允许在有不可变引用时也可以改变数据。为了改变数据，该模式在数据结构中使用 `unsafe` 代码来模糊 Rust 通常的可变性和借用规则。只有保证在运行时借用规则被遵守才能使用内部可变性模式的类型。

不可变值内部改变值就是内部可变性模式。

当有一个不可变值时，不能可变地借用它。

```rust
fn main() {
    let x = 5;
    let y = &mut x;		// ❌ x 不能为可变借用
}
```

特定情况下，令一个值在其方法内部能够修改自身，而在其他代码中仍视为不可变，是很有用的。`RefCell<T>` 是一个获得内部可变性的方法。

```rust
pub trait Messenger {
    fn send(&self, msg: &str);
}

pub struct LimitTracker<'a, T: Messenger> {
    messenger: &'a T,
    value: usize,
    max: usize,
}

impl<'a, T> LimitTracker<'a, T>
where
    T: Messenger,
{
    pub fn new(messenger: &'a T, max: usize) -> LimitTracker<'a, T> {
        LimitTracker {
            messenger,
            value: 0,
            max,
        }
    }

    pub fn set_value(&mut self, value: usize) {
        self.value = value;

        let percentage_of_max = self.value as f64 / self.max as f64;

        if percentage_of_max >= 1.0 {
            self.messenger.send("Error: You are over your quota!");
        } else if percentage_of_max >= 0.9 {
            self.messenger
                .send("Urgent warning: You've used up over 90% of your quota!");
        } else if percentage_of_max >= 0.75 {
            self.messenger
                .send("Warning: You've used up over 75% of your quota!");
        }
    }
}
```

```rust
#[cfg(test)]
mod tests {
    use super::*;

    struct MockMessenger {
        sent_messages: Vec<String>,
    }

    impl MockMessenger {
        fn new() -> MockMessenger {
            MockMessenger {
                sent_messages: vec![],
            }
        }
    }

    impl Messenger for MockMessenger {
        fn send(&self, message: &str) {
            self.sent_messages.push(String::from(message));
            // ❌ 不能修改 MockMessenger 来记录消息，因为 send 方法接收的是对 self 的不可变引用
        }
    }

    #[test]
    fn it_sends_an_over_75_percent_warning_message() {
        let mock_messenger = MockMessenger::new();
        let mut limit_tracker = LimitTracker::new(&mock_messenger, 100);

        limit_tracker.set_value(80);

        assert_eq!(mock_messenger.sent_messages.len(), 1);
    }
}
```

在不改变 `Messenger` trait 的情况下，通过 `RefCell` 来储存 `sent_messages`，然后 `send` 将能够修改 `sent_messages` 并储存消息。

```rust
use super::*;
use std::cell::RefCell;

#[cfg(test)]
mod tests {
    
    struct MockMessenger {
        sent_messages: RefCell<Vec<String>>,			// 声明RefCell类型
    }

    impl MockMessenger {
        fn new() -> MockMessenger {
            MockMessenger {
                sent_messages: RefCell::new(vec![]),	// 创建了一个 RefCell<Vec<String>> 实例
            }
        }
    }

    impl Messenger for MockMessenger {
        fn send(&self, message: &str) {
            self.sent_messages.borrow_mut().push(String::from(message));
            // ✅ borrow_mut 方法来获取 RefCell<Vec<String>> 中值的可变引用
        }
    }

    #[test]
    fn it_sends_an_over_75_percent_warning_message() {
        // --snip--

        assert_eq!(mock_messenger.sent_messages.borrow().len(), 1);
        // borrow 以获取 vector 的不可变引用
    }
}
```

**结合 Rc\<T\> 和 RefCell\<T\> 来拥有多个可变数据所有者**

如果有一个储存了 `RefCell<T>` 的 `Rc<T>` 的话，就可以得到有多个所有者并且可以修改的值了。

```rust
use crate::List::{Cons, Nil};
use std::cell::RefCell;
use std::rc::Rc;

#[derive(Debug)]
enum List {
    Cons(Rc<RefCell<i32>>, Rc<List>),	// RefCell<T> 中提供内部可变性的方法来在需要时修改数据
    Nil,
}

fn main() {
    let value = Rc::new(RefCell::new(5));

    let a = Rc::new(Cons(Rc::clone(&value), Rc::new(Nil)));

    let b = Cons(Rc::new(RefCell::new(3)), Rc::clone(&a));
    let c = Cons(Rc::new(RefCell::new(4)), Rc::clone(&a));

    *value.borrow_mut() += 10;
    // borrow_mut 方法返回 RefMut<T> 智能指针，可以对其使用解引用运算符并修改其内部值

    println!("a after = {a:?}");
    println!("b after = {b:?}");
    println!("c after = {c:?}");
    // 它们都拥有修改后的值15而不是5
}
```

输出

```shell
a after = Cons(RefCell { value: 15 }, Nil)
b after = Cons(RefCell { value: 3 }, Cons(RefCell { value: 15 }, Nil))
c after = Cons(RefCell { value: 4 }, Cons(RefCell { value: 15 }, Nil))
```

## 引用循环与内存泄漏

Rust 中创建引用循环是可能的，这会造成内存泄漏，因为每一项的引用计数永远也到不了 0，持有的数据也就永远不会被释放。

**引用循环例子**

```rust
use crate::List::{Cons, Nil};
use std::cell::RefCell;
use std::rc::Rc;

#[derive(Debug)]
enum List {
    Cons(i32, RefCell<Rc<List>>),
    Nil,
}

impl List {
    fn tail(&self) -> Option<&RefCell<Rc<List>>> {
        match self {
            Cons(_, item) => Some(item),
            Nil => None,
        }
    }
}

fn main() {
    let a = Rc::new(Cons(5, RefCell::new(Rc::new(Nil))));

    println!("a initial rc count = {}", Rc::strong_count(&a));
    println!("a next item = {:?}", a.tail());

    let b = Rc::new(Cons(10, RefCell::new(Rc::clone(&a))));

    println!("a rc count after b creation = {}", Rc::strong_count(&a));
    println!("b initial rc count = {}", Rc::strong_count(&b));
    println!("b next item = {:?}", b.tail());

    if let Some(link) = a.tail() {
        *link.borrow_mut() = Rc::clone(&b);		// 修改 a 使其指向 b 而不是 Nil
        // 将列表 a 修改为指向 b 之后， a 和 b 中的 Rc<List> 实例的引用计数都是 2
    }

    println!("b rc count after changing a = {}", Rc::strong_count(&b));
    println!("a rc count after changing a = {}", Rc::strong_count(&a));

    // Uncomment the next line to see that we have a cycle;
    // it will overflow the stack.
    // println!("a next item = {:?}", a.tail());
    // 如果取消 println! 的注释，Rust 会尝试打印出 a 指向 b 指向 a`这样的循环直到栈溢出
}
// Rust 丢弃 b，使 b Rc<List> 实例的引用计数从 2 减为 1
// 接下来 Rust 会丢弃 a ，由于另一个 Rc<List> 实例依然引用它， a 实例的引用计数从 2 减为 1
// a、b实例无法被回收，这些列表的内存将永远保持未被回收的状态
```

输出

```shell
a initial rc count = 1
a next item = Some(RefCell { value: Nil })
a rc count after b creation = 2
b initial rc count = 1
b next item = Some(RefCell { value: Cons(5, RefCell { value: Nil }) })
b rc count after changing a = 2
a rc count after changing a = 2
```

创建引用循环是一个程序上的逻辑 bug，应该使用自动化测试、代码评审等避免引用循环。另一个解决方案是重新组织数据结构，使得一部分引用拥有所有权而另一部分没有。

**使用Weak\<T\> 防止引用循环**

调用 `Rc::clone` 会增加 `Rc<T>` 实例的 `strong_count`，只有其 `strong_count` 为 0 时 `Rc<T>` 实例才会被清理。

调用 `Rc::downgrade` 时会得到 `Weak<T>` 类型的智能指针，增加 `weak_count` ， `weak_count` 无需计数为 0 就能使 `Rc<T>` 实例被清理。

因为 `Weak<T>` 引用的值可能已经被丢弃了，为了使用 `Weak<T>` 所指向的值，需调用`upgrade` 方法，如果 `Rc<T>` 值还未被丢弃，则结果是 `Some`；如果 `Rc<T>` 已被丢弃，则结果是 `None`。

```rust
use std::cell::RefCell;
use std::rc::{Rc, Weak};

#[derive(Debug)]
struct Node {
    value: i32,
    parent: RefCell<Weak<Node>>,		// 不拥有所有权
    // parent: RefCell<Node>,			// 会导致循环引用
    children: RefCell<Vec<Rc<Node>>>,	// 拥有所有权
}

fn main() {
    let leaf = Rc::new(Node {
        value: 3,
        parent: RefCell::new(Weak::new()),
        children: RefCell::new(vec![]),
    });

    println!("leaf parent = {:?}", leaf.parent.borrow().upgrade());

    let branch = Rc::new(Node {
        value: 5,
        parent: RefCell::new(Weak::new()),
        children: RefCell::new(vec![Rc::clone(&leaf)]),
    });

    *leaf.parent.borrow_mut() = Rc::downgrade(&branch);
    // 使用 Rc::downgrade 函数创建 branch 的 Weak<Node> 引用

    println!("leaf parent = {:?}", leaf.parent.borrow().upgrade());
}
```

输出

```shell
leaf parent = None
leaf parent = Some(Node { value: 5, parent: RefCell { value: (Weak) }, children: RefCell { value: [Node { value: 3, parent: RefCell { value: (Weak) }, children: RefCell { value: [] } }] } })
```

```rust
fn main() {
    let leaf = Rc::new(Node {
        value: 3,
        parent: RefCell::new(Weak::new()),
        children: RefCell::new(vec![]),
    });

    println!(
        "leaf strong = {}, weak = {}",
        Rc::strong_count(&leaf),
        Rc::weak_count(&leaf),
    );

    {
        let branch = Rc::new(Node {
            value: 5,
            parent: RefCell::new(Weak::new()),
            children: RefCell::new(vec![Rc::clone(&leaf)]),
        });

        *leaf.parent.borrow_mut() = Rc::downgrade(&branch);

        println!(
            "branch strong = {}, weak = {}",
            Rc::strong_count(&branch),
            Rc::weak_count(&branch),
        );
        
        // 弱引用即使不为0实例也可以被清除

        println!(
            "leaf strong = {}, weak = {}",
            Rc::strong_count(&leaf),
            Rc::weak_count(&leaf),
        );
    }

    println!("leaf parent = {:?}", leaf.parent.borrow().upgrade());
    println!(
        "leaf strong = {}, weak = {}",
        Rc::strong_count(&leaf),
        Rc::weak_count(&leaf),
    );
}
```

输出

```shell
leaf strong = 1, weak = 0
branch strong = 1, weak = 1
leaf strong = 2, weak = 0
leaf parent = None
leaf strong = 1, weak = 0
```

# 并发

- 并发：一个执行单元处理多个任务，通过任务切换实现，适用于单核，通过线程、进程和异步来实现
- 并行：多个执行单元同时处理不同任务，适用于多核

## 线程

将程序中的计算拆分进多个线程可以改善性能，因为程序可以同时进行多个任务，不过这也会增加复杂性。因为线程是同时运行的，所以无法保证不同线程中的代码的执行顺序。

**创建线程**

```rust
use std::thread;
use std::time::Duration;

fn main() {
    thread::spawn(|| {			// 创建一个新线程，并传递一个闭包
        for i in 1..10 {
            println!("hi number {i} from the spawned thread!");
            thread::sleep(Duration::from_millis(1));
        }
    });

    for i in 1..5 {				// 主线程
        println!("hi number {i} from the main thread!");
        thread::sleep(Duration::from_millis(1));
    }
}
// Rust 程序的主线程结束时，所有子线程也会结束，而不管其是否执行完毕
```

输出（顺序不一定）

```shell
hi number 1 from the main thread!
hi number 1 from the spawned thread!
hi number 2 from the main thread!
hi number 2 from the spawned thread!
hi number 3 from the main thread!
hi number 3 from the spawned thread!
hi number 4 from the main thread!
hi number 4 from the spawned thread!
hi number 5 from the spawned thread!
```

**join 等待线程结束**

可以通过将 `thread::spawn` 的返回值储存在变量中，来解决新建线程部分没有执行或者完全没有执行的问题。

```rust
use std::thread;
use std::time::Duration;

fn main() {
    let handle = thread::spawn(|| {	// thread::spawn 的返回值类型是 JoinHandle<T>，其拥有所有权
        for i in 1..10 {
            println!("hi number {i} from the spawned thread!");
            thread::sleep(Duration::from_millis(1));
        }
    });

    for i in 1..5 {
        println!("hi number {i} from the main thread!");
        thread::sleep(Duration::from_millis(1));
    }

    handle.join().unwrap();			// 等待 handle 线程结束
}
```

输出

```shell
hi number 1 from the main thread!
hi number 2 from the main thread!
hi number 1 from the spawned thread!
hi number 3 from the main thread!
hi number 2 from the spawned thread!
hi number 4 from the main thread!
hi number 3 from the spawned thread!
hi number 4 from the spawned thread!
hi number 5 from the spawned thread!
hi number 6 from the spawned thread!
hi number 7 from the spawned thread!
hi number 8 from the spawned thread!
hi number 9 from the spawned thread!
```

```rust
use std::thread;
use std::time::Duration;

fn main() {
    let handle = thread::spawn(|| {
        for i in 1..10 {
            println!("hi number {i} from the spawned thread!");
            thread::sleep(Duration::from_millis(1));
        }
    });

    handle.join().unwrap();			// 移动到主线程之前
    // 主线程会等待直到新建线程执行完毕之后才开始执行 for 循环

    for i in 1..5 {
        println!("hi number {i} from the main thread!");
        thread::sleep(Duration::from_millis(1));
    }
}
```

输出

```shell
hi number 1 from the spawned thread!
hi number 2 from the spawned thread!
hi number 3 from the spawned thread!
hi number 4 from the spawned thread!
hi number 5 from the spawned thread!
hi number 6 from the spawned thread!
hi number 7 from the spawned thread!
hi number 8 from the spawned thread!
hi number 9 from the spawned thread!
hi number 1 from the main thread!
hi number 2 from the main thread!
hi number 3 from the main thread!
hi number 4 from the main thread!
```

**move 闭包与线程**

move 会取得闭包从环境中取得的值的所有权，因此可以将这些值的所有权从一个线程传送到另一个线程。

```rust
use std::thread;

fn main() {
    let v = vec![1, 2, 3];

    let handle = thread::spawn(|| {
        println!("Here's a vector: {v:?}"); // ❌ v 在子线程中不一定一直有效 
    });
    drop(v);								// 此时子线程中v不再有效

    handle.join().unwrap();
}
```

```rust
use std::thread;

fn main() {
    let v = vec![1, 2, 3];

    let handle = thread::spawn(move || {	// ✅ 强制闭包获取其使用的值的所有权
        println!("Here's a vector: {v:?}");
    });
    
    v.push(4);								// ❌ 此时主线程中无法使用v

    handle.join().unwrap();
}
```

## 消息传递

为实现消息传递并发，Rust 标准库提供了一个信道（channel）实现，信道表示数据从一个线程发送到另一个线程。

**信道**

信道有两个组成部分：发送端（transmitter）和接收端（receiver）。代码中的一部分调用发送端的方法以及希望发送的数据，另一部分则检查接收端收到的消息。当发送端或接收端任一被丢弃时可以认为信道被关闭（closed）了。

```rust
use std::sync::mpsc;
use std::thread;

fn main() {
    let (tx, rx) = mpsc::channel();				// 创建信道 类型为元组
    // mpsc 是 多生产者单消费者（multiple producer, single consumer）的缩写
    // 简而言之，Rust 标准库实现信道的方式意味着一个信道可以有多个发送端，但只能有一个接收端

    thread::spawn(move || {						
        let val = String::from("hi");
        tx.send(val).unwrap();					// 将发送端移动到新建线程
    });

    let received = rx.recv().unwrap();			// 阻塞主线程执行直到从信道中接收一个值
    // try_recv 不会阻塞，它立刻返回一个 Result<T, E>
    // Ok 值包含可用的信息，而 Err 值代表此时没有任何消息
    println!("Got: {received}");
}
```

**信道与所有权转移**

所有权规则在消息传递中扮演了重要角色，有助于编写安全的并发代码。

```rust
use std::sync::mpsc;
use std::thread;

fn main() {
    let (tx, rx) = mpsc::channel();

    thread::spawn(move || {
        let val = String::from("hi");
        tx.send(val).unwrap();				// send 函数获取其参数的所有权并移动这个值归接收端所有
        println!("val is {val}");			// ❌ 此时 val 不再可用
    });

    let received = rx.recv().unwrap();
    println!("Got: {received}");
}
```

**发送多个者**

```rust
use std::sync::mpsc;
use std::thread;
use std::time::Duration;

fn main() {
    let (tx, rx) = mpsc::channel();

    thread::spawn(move || {
        let vals = vec![
            String::from("hi"),
            String::from("from"),
            String::from("the"),
            String::from("thread"),
        ];

        for val in vals {
            tx.send(val).unwrap();
            thread::sleep(Duration::from_secs(1));
        }
    });

    // 主线程等待从新建线程中接收值
    for received in rx {				// 打印接收到的值
        println!("Got: {received}");
    }
}
```

输出

```shell
Got: hi
Got: from
Got: the
Got: thread
```

**克隆发送端**

```rust
    // --snip--

	/* 模拟两个发送者一个接收者 */
    let (tx, rx) = mpsc::channel();

    let tx1 = tx.clone();			// 克隆发送端
    thread::spawn(move || {
        let vals = vec![
            String::from("hi"),
            String::from("from"),
            String::from("the"),
            String::from("thread"),
        ];

        for val in vals {
            tx1.send(val).unwrap();
            thread::sleep(Duration::from_secs(1));
        }
    });

    thread::spawn(move || {
        let vals = vec![
            String::from("more"),
            String::from("messages"),
            String::from("for"),
            String::from("you"),
        ];

        for val in vals {
            tx.send(val).unwrap();
            thread::sleep(Duration::from_secs(1));
        }
    });

    for received in rx {
        println!("Got: {received}");
    }

    // --snip--

```

输出（顺序不一定）

```shell
Got: hi
Got: more
Got: from
Got: messages
Got: for
Got: the
Got: thread
Got: you
```

## 共享状态

互斥锁（mutex）在同一时刻，只允许一个线程访问数据。

**互斥锁使用规则**

- 在使用数据之前，必须先获取锁
- 使用完被互斥器所保护的数据之后，必须解锁数据，这样其他线程才能够获取锁

正确的管理互斥器异常复杂，然而，在 Rust 中得益于类型系统和所有权，不可能在锁和解锁上出错。

**单线程中使用**

```rust
use std::sync::Mutex;

fn main() {
    let m = Mutex::new(5);

    {
        let mut num = m.lock().unwrap();	
        // 使用 lock 方法来获取锁，可以访问互斥器中的数据，这个调用会阻塞当前线程，直到拥有锁为止
        // 无法获取锁则会 panic
        *num = 6;
        // lock调用成功返回类型为 MutexGuard 的智能指针
        // 同时实现了 Drop trait，当 MutexGuard 离开作用域时，自动释放锁
    }

    println!("m = {m:?}");							// 此时m值为 6
}
```

**多线程中实现**

```rust
use std::sync::Mutex;
use std::thread;

fn main() {
    let counter = Mutex::new(0);
    let mut handles = vec![];

    for _ in 0..10 {
        let handle = thread::spawn(move || {
            let mut num = counter.lock().unwrap();	// ❌ 不能将 counter 锁的所有权移动到多个线程中

            *num += 1;
        });
        handles.push(handle);
    }

    for handle in handles {
        handle.join().unwrap();
    }

    println!("Result: {}", *counter.lock().unwrap());
}
```

```rust
use std::rc::Rc;
use std::sync::Mutex;
use std::thread;

fn main() {
    let counter = Rc::new(Mutex::new(0));			// 将 Mutex<T> 封装进 Rc<T> 中
    let mut handles = vec![];

    for _ in 0..10 {
        let counter = Rc::clone(&counter);			// 无法确保改变计数的操作不会被其他线程打断
        let handle = thread::spawn(move || {
            let mut num = counter.lock().unwrap();	// ❌ Rc<T> 没有实现并发原语 并不能安全的在线程间共享
			
            *num += 1;
        });
        handles.push(handle);
    }

    for handle in handles {
        handle.join().unwrap();
    }

    println!("Result: {}", *counter.lock().unwrap());
}
```

`Arc<T>` 是一个类似 `Rc<T>` 并可以安全地用于并发环境的原子类型。原子类型就像基本类型一样，可以安全地在线程间共享。

```rust
use std::sync::{Arc, Mutex};
use std::thread;

fn main() {
    let counter = Arc::new(Mutex::new(0));
    let mut handles = vec![];

    for _ in 0..10 {
        let counter = Arc::clone(&counter);				// 原子共享类型的互斥锁
        // 子线程
        let handle = thread::spawn(move || {
            let mut num = counter.lock().unwrap();		// 获取锁

            *num += 1;
        });
        // 主线程
        handles.push(handle);							
    }

    for handle in handles {
        handle.join().unwrap();							// 确保子线程执行完毕
    }

    println!("Result: {}", *counter.lock().unwrap());	// 主线程
}
```

输出

```shell
Result: 10
```

注

1. `Mutex<T>` 提供了内部可变性：使用 `RefCell<T>` 可以改变 `Rc<T>` 中内容，同样地，使用 `Mutex<T>` 也可以改变 `Arc<T>` 中的内容
2. `Mutex<T>` 也有造成死锁（deadlock）的风险：当某个操作需要锁住两个资源，而两个线程分别持有两个资源的其中一个锁时，它们会永远相互等待

## Send 和 Sync trait

处理并发的方案并不受标准库或语言所限，可以编写自己的或使用他人编写的并发特性。 `std::marker` 的 `Send` 和 `Sync` trait就是内嵌于语言本身的并发概念。

**Send trait**

`Send` trait 表明实现了 `Send` 的类型值的所有权可以在线程间传送。

几乎所有的 Rust 类型都是`Send` 的，任何完全由 `Send` 的类型组成的类型也会自动被标记为 `Send`。也有不能实现 `Send` 的，例如 `Rc<T>`和裸指针（raw pointer）。

**Sync trait**

`Sync` trait 表明实现了 `Sync` 的类型可以安全的在多个线程中拥有其值的引用。

类似于 `Send` 的情况，基本类型都实现了 `Sync`，完全由实现了 `Sync` 的类型组成的类型也实现了 `Sync`。智能指针 `Rc<T>`、`RefCell<T>`和 `Cell<T>` 系列类型 也没有实现 `Sync`。

注

1. 通常并不需要手动实现 `Send` 和 `Sync` trait，因为完全由实现了 `Send` 和 `Sync` 的类型组成的类型，自动实现了 `Send` 和 `Sync`
2. 在创建新的由不是 `Send` 和 `Sync` 的部分构成的并发类型时需要多加小心，以确保维持其安全保证
3. 线程安全会造成性能损失，如果只是在单线程中对值进行操作，不必强制原子性所提供的保证可以使代码运行得更快

# 异步

- 同步（Synchronous）：发起操作后必须等结果返回才能继续执行
- 异步（Asynchronous）：发起操作后无需等待结果，先继续执行，结果稍后通知或回调处理

Rust 的异步主要处理并发性，允许程序实现非阻塞式调用。

## 基础知识

Rust 异步编程的关键元素是 futures 和 `async` 与 `await` 关键字。

- future ：future 是一个现在可能还没有准备好但将在未来某个时刻准备好的值，实现了 `Future` trait 的类型为 future。每个 future 会维护自身的进度状态信息以及对 ready 的定义
- async：用于代码块和函数，表明它们可以被中断并恢复
- await：在一个 async 块或 async 函数中，使用 `await` 关键字来等待 future就绪，提供暂停和恢复执行的点

**异步程序举例**

```rust
use std::future::Future;
use trpl::Html;

/* 获取一个网页 URL 作为参数，请求该 URL 并返回标题元素的文本 */
// 当 Rust 遇到 async 关键字标记的代码块时，会将其编译为实现了 Future trait 的唯一的、匿名的数据类型
// 编写 async fn 就等同于编写一个返回类型为 future 的函数
// 等同于 
//fn page_title(url: &str) -> impl Future<Output = Option<String>> {
//    async move {
//        let text = trpl::get(url).await.text().await;
//        Html::parse(&text)
//            .select_first("title")
//            .map(|title| title.inner_html())
//    }
//}

async fn page_title(url: &str) -> Option<String> {
    let response = trpl::get(url).await;		// 获取传入的任意 URL，后使用 await 关键字来等待响应
    // Rust 的 await 关键字出现在需要等待的表达式之后而不是之前
    // 调用 await 后程序不会死等操作完成，而是将控制权交给异步运行时，让当前异步任务暂停执行一段指定的时间，但不会阻塞整个线程或其他任务
    let response_text = response.text().await;	// 获取响应的文本，并等待
    Html::parse(&response_text)
        .select_first("title")
        .map(|title_element| title_element.inner_html())
}

async fn main() {
    let args: Vec<String> = std::env::args().collect();
    let url = &args[1];
    match page_title(url).await {	
        // ❌ 只有在 async 函数或者代码块中才可以使用 await 关键字，并且不允许将 main 函数标记为 async
        // 异步代码至少需要一个运行时：即一个管理执行异步代码细节的 Rust crate，每种运行时针对场景不同，而main函数自身并不是一个运行时
        Some(title) => println!("The title for {url} was {title}"),
        None => println!("{url} had no title"),
    }
}

```

注

1. Rust 中的 futures 是 惰性（lazy），惰性使得 Rust 可以避免提前运行异步代码，直到真正需要时才执行
2. 在一个给定的 future 中的一切都是线性发生的，并发发生在 futures 之间

```rust
fn main() {
    let args: Vec<String> = std::env::args().collect();

    trpl::run(	// 设置一个运行时来运行传递的 future。一旦 future 完成，run 返回 future 返回的任何值
        async {	// ✅ 传递一个 async 块并显式地等待 page_title 调用的结果
            let url = &args[1];
            match page_title(url).await {
                Some(title) => println!("The title for {url} was {title}"),
                None => println!("{url} had no title"),
            }
        }
    )
}
```

注

1. 代码使用 `await` 关键字的地方，代表将控制权交还给运行时。为此 Rust 需要记录异步代码块中涉及的状态，这样运行时可以去执行其他工作，并在准备好时回来继续推进当前的任务
2. 如果 `main` 是一个异步函数，需要有其它组件来管理 `main` future 返回的状态机，但是 `main` 是程序的入口点

**两个 URL 竞争**

```rust
use trpl::{Either, Html};

fn main() {
    let args: Vec<String> = std::env::args().collect();

    trpl::run(
        async {
            let title_fut_1 = page_title(&args[1]);			// 因为 future 是惰性的，还没有进行任何工作
            let title_fut_2 = page_title(&args[2]);

            let (url, maybe_title) =
                match trpl::race(title_fut_1, title_fut_2).await {	// 哪个先执行完成
                    Either::Left(left) => left,
                    Either::Right(right) => right,
                };

            println!("{url} returned first");
            match maybe_title {
                Some(title) => println!("Its page title is: '{title}'"),
                None => println!("Its title could not be parsed."),
            }
        }
    )
}

async fn page_title(url: &str) -> (&str, Option<String>) {
    let text = trpl::get(url).await.text().await;
    let title = Html::parse(&text)
        .select_first("title")
        .map(|title| title.inner_html());
    (url, title)
}
```

## Async实现并发

**异步计数**

```rust
use std::time::Duration;

fn main() {
    trpl::run(
        async {
            trpl::spawn_task(
                async {
                    for i in 1..10 {
                        println!("hi number {i} from the first task!");
                        trpl::sleep(Duration::from_millis(500)).await;
                    }
                }
            );

            for i in 1..5 {
                println!("hi number {i} from the second task!");
                trpl::sleep(Duration::from_millis(500)).await;
            }
        }
    );
}
// 当 main 函数结束时 spawn_task 产生的任务就会关闭
```

输出（顺序不一定）

```shell
hi number 1 from the second task!
hi number 1 from the first task!
hi number 2 from the first task!
hi number 2 from the second task!
hi number 3 from the first task!
hi number 3 from the second task!
hi number 4 from the first task!
hi number 4 from the second task!
hi number 5 from the first task!
```

```rust
use std::time::Duration;

fn main() {
    trpl::run(
        async {
            let handle = trpl::spawn_task(
                async {
                    for i in 1..10 {
                        println!("hi number {i} from the first task!");
                        trpl::sleep(Duration::from_millis(500)).await;
                    }
                }
            );

            for i in 1..5 {
                println!("hi number {i} from the second task!");
                trpl::sleep(Duration::from_millis(500)).await;
            }

            handle.await.unwrap();  // 直到 spawn_task 任务结束
        }
    );
}
```

输出

```shell
hi number 1 from the second task!
hi number 1 from the first task!
hi number 2 from the first task!
hi number 2 from the second task!
hi number 3 from the first task!
hi number 3 from the second task!
hi number 4 from the first task!
hi number 4 from the second task!
hi number 5 from the first task!
hi number 6 from the first task!
hi number 7 from the first task!
hi number 8 from the first task!
hi number 9 from the first task!
```

异步与线程相比，无需再产生另一个操作系统线程来进行工作，甚至不需要产生一个任务。

```rust
use std::time::Duration;

fn main() {
    trpl::run(
        async {
            let fut1 = async {
                for i in 1..10 {
                    println!("hi number {i} from the first task!");
                    trpl::sleep(Duration::from_millis(500)).await;
                }
            };

            let fut2 = async {
                for i in 1..5 {
                    println!("hi number {i} from the second task!");
                    trpl::sleep(Duration::from_millis(500)).await;
                }
            };

            trpl::join(fut1, fut2).await;   
            // join 会等待 fut1 和 fut2 都结束后会产生一个新的 future
        }
    );
}
```

输出

```shell
hi number 1 from the first task!
hi number 1 from the second task!
hi number 2 from the first task!
hi number 2 from the second task!
hi number 3 from the first task!
hi number 3 from the second task!
hi number 4 from the first task!
hi number 4 from the second task!
hi number 5 from the first task!
hi number 6 from the first task!
hi number 7 from the first task!
hi number 8 from the first task!
hi number 9 from the first task!
```

注

1. 因为 `trpl::join` 函数是公平的（fair），它以相同的频率检查每一个 future，使它们交替执行，绝不会让一个任务在另一个任务准备好时抢先执行
2. 对于线程来说，操作系统会决定该检查哪个线程和会让它运行多长时间。对于异步 Rust 来说，运行时决定检查哪一个任务

**异步消息传递**

在 future 之间共享数据也与线程类似，使用消息传递。

```rust
use std::time::Duration;

// 消息立刻就会到达
// 并未实现并发
fn main() {
    trpl::run(
        async {
            let (tx, mut rx) = trpl::channel();     	// mpsc信道的异步版本，使用的是可变rx

            let val = String::from("hi");
            tx.send(val).unwrap();

            let received = rx.recv().await.unwrap();    // 等待 rx.recv() 调用，不会阻塞
            println!("Got: {received}");
        }
    );
}
```

```rust
use std::time::Duration;

// 消息在全部延迟后立刻一起到达，而不是逐个在延迟后到达
// 程序永远也不会退出
fn main() {
    trpl::run(
        async {
        // 只有一个异步代码块，所以所有的代码线性地执行。这里仍然没有并发
            let (tx, mut rx) = trpl::channel();

            let vals = vec![
                String::from("hi"),
                String::from("from"),
                String::from("the"),
                String::from("future"),
            ];

            for val in vals {
            // 所有 tx.send 调用与 trpl::sleep 调用是依次进行的
            // 如果顺序地 await 单个 future，则就又回到了一个顺序流
                tx.send(val).unwrap();
                trpl::sleep(Duration::from_millis(500)).await;
            }

            // while let 条件循环， if let 结构的循环版本
            while let Some(value) = rx.recv().await {
            // rx.recv 调用产生一个 Future，await 它
                println!("received '{value}'");
            }
        }
    );
}
```

```rust
use std::time::Duration;

// 消息会以 500 毫秒的间隔打印，而不是在两秒后就全部一起打印
// 程序仍然永远也不会退出
fn main() {
    trpl::run(
        async {
            let tx_fut = async {
                let vals = vec![
                    String::from("hi"),
                    String::from("from"),
                    String::from("the"),
                    String::from("future"),
                ];

                for val in vals {
                    tx.send(val).unwrap();
                    // tx 直到传递给 trpl::run 的外层异步代码块结束前都不会被丢弃
                    trpl::sleep(Duration::from_millis(500)).await;
                }
            };

            let rx_fut = async {
                while let Some(value) = rx.recv().await {	// 不会结束
                    println!("received '{value}'");
                }
                // 信道只有在调用 rx.close 或者发送端 tx 被丢弃时才会关闭
            };
            
            trpl::join(tx_fut, rx_fut).await;
            // trpl::join 需要两个任务都完成的时候才会结束
        }
    );
}
```

可以在代码的某处调用 `rx.close` 来手动关闭 `rx`，但是可能会丢失消息。可以让`tx` 在函数的结尾之前被丢弃。

```rust
use std::time::Duration;

fn main() {
    trpl::run(
        async {
            let (tx, mut rx) = trpl::channel();

            // move 关键字也能像闭包那样作用于异步代码块
            // tx 移动进异步代码块，它会在代码块结束后立刻被丢弃
            let tx_fut = async move {
                let vals = vec![
                    String::from("hi"),
                    String::from("from"),
                    String::from("the"),
                    String::from("future"),
                ];

                for val in vals {
                    tx.send(val).unwrap();
                    trpl::sleep(Duration::from_millis(500)).await;
                }
            };

            let rx_fut = async {
                while let Some(value) = rx.recv().await {
                    println!("received '{value}'");
                }
            };

            trpl::join(tx_fut, rx_fut).await;

        }
    );
}
```

多个发送者

```rust
use std::time::Duration;

fn main() {
    trpl::run(
        async {
            let (tx, mut rx) = trpl::channel();

            let tx1 = tx.clone();
            let tx1_fut = async move {
                let vals = vec![
                    String::from("hi"),
                    String::from("from"),
                    String::from("the"),
                    String::from("future"),
                ];

                for val in vals {
                    tx1.send(val).unwrap();
                    trpl::sleep(Duration::from_millis(500)).await;
                }
            };

            let rx_fut = async {
                while let Some(value) = rx.recv().await {
                    println!("received '{value}'");
                }
            };

            let tx_fut = async move {
                let vals = vec![
                    String::from("more"),
                    String::from("messages"),
                    String::from("for"),
                    String::from("you"),
                ];

                for val in vals {
                    tx.send(val).unwrap();
                    trpl::sleep(Duration::from_millis(1500)).await;
                }
            };
			
            // 关键在于 future 被 await 的顺序，而不是它们创建的顺序
            trpl::join3(tx1_fut, tx_fut, rx_fut).await;
        }
    );
}
```

输出

```shell
received 'hi'
received 'more'
received 'from'
received 'the'
received 'messages'
received 'future'
received 'for'
received 'you'
```

## 多futures

**join!**

宏版本的 `join` 可以传递任意数量的参数，会自行处理 await 这些 future。

```rust
trpl::join!(tx1_fut, tx_fut, rx_fut);
```

**future类型的集合**

然而，这种形式适用于提前知道 future 的数量的情况。在 Rust 中，常见的做法是将 futures 放进一个集合并等待集合中的一些或者全部 future 完成。

为了检查一些集合中的所有 future，需要遍历并 join 全部的 future。

```rust
let futures = vec![tx1_fut, rx_fut, tx_fut];		// ❌ 类型不同

trpl::join_all(futures).await;
```

使用 trait objects 允许将这些类型所产生的不同的匿名 future 视为相同的类型，因为它们都实现了 `Future` trait。同时用 `Pin` 来封装 `Box`，并使用 `Box::pin` 来 pin 住 futures 自身。

```rust
use std::pin::Pin;

// -- snip --

let futures: Vec<Pin<Box<dyn Future<Output = ()>>>> =
    vec![Box::pin(tx1_fut), Box::pin(rx_fut), Box::pin(tx_fut)];	// ✅ 显式标注 futures 的类型


// 最内层的类型是 future 本身,显式地指出 future 的输出类型是 ()，其编写为 Future<Output = ()>
// 使用 dyn 将 trait 标记为动态的
// 整个 trait 引用被封装进一个 Box，并用 Pin 来封装
// 最后，显式表明 futures 是一个包含这些项的 Vec

trpl::join_all(futures).await;
```

注

1. `Pin` 本身是一个封装类型，会带来性能开销

必须显式地知道被 pin 的引用的类型，否则 Rust 不知道如何将它们解释为动态 trait objects。可以在定义每个 future 的时候使用 `pin!`，并将 `futures` 定义为一个包含被 pin 的动态 `Future` 类型的可变引用的 `Vec`。

```rust
use std::pin::{Pin, pin};

// -- snip --

        let tx1_fut = pin!(async move {
            // --snip--
        });

        let rx_fut = pin!(async {
            // --snip--
        });

        let tx_fut = pin!(async move {
            // --snip--
        });

        let futures: Vec<Pin<&mut dyn Future<Output = ()>>> =
            vec![tx1_fut, rx_fut, tx_fut];

```

**不同类型的future**

```rust
let a = async { 1u32 };
let b = async { "Hello!" };
let c = async { true };

let (a_result, b_result, c_result) = trpl::join!(a, b, c);	// 允许传递多个 future 类型并产生一个这些类型的元组
println!("{a_result}, {b_result}, {c_result}");

// 不能使用 trpl::join_all，因为它要求传递的 future 都拥有相同的类型
```

注

1. `join_all`： 处理动态数量的 future，但要求类型相同
2. `join!` ：处理固定数量的 future，不要求类型相同

**future竞争**

想要要部分 future 结束就能继续，但 `join` 系列函数和宏要求全部结束才能继续。

```rust
use std::time::Duration;

fn main() {
    trpl::run(
        async {
            let slow = async {
                println!("'slow' started.");
                trpl::sleep(Duration::from_millis(100)).await;
                println!("'slow' finished.");
            };

            let fast = async {
                println!("'fast' started.");
                trpl::sleep(Duration::from_millis(50)).await;
                println!("'fast' finished.");
            };

            trpl::race(slow, fast).await;
            // 非公平地，第一个 future 会在另一个任务开始之前一直运行到异步代码块中第一个 await 为止
            // 在 a 完成后就立刻结束
        }
    );
}
```

输出

```shell
'slow' started.
'fast' started.
'fast' finished.
```

注

1. Rust 只会在一个 await point 暂停异步代码块并将控制权交还给运行时
2. 如果在异步代码块中做了一堆工作而没有一个 await point，则那个 future 会阻塞其它任何 future 继续进行

**Yielding**

模拟一个长时间运行的操作。

```rust
use std::{thread, time::Duration};

/* 模拟现实世界中的长时间运行并且会阻塞的操作 */
fn slow(name: &str, ms: u64) {
    thread::sleep(Duration::from_millis(ms));	// 阻塞当前线程
    println!("'{name}' ran for {ms}ms");
}

fn main() {
    trpl::run(
        async {
            // 每个 future 只会在进行了一系列缓慢操作之后在 await point 才将控制权交还给运行时
            let a = async {
                println!("'a' started.");
                slow("a", 30);				// 模拟CPU密集型操作
                slow("a", 10);
                slow("a", 20);
                trpl::sleep(Duration::from_millis(50)).await;
                println!("'a' finished.");
            };

            let b = async {
                println!("'b' started.");
                slow("b", 75);
                slow("b", 10);
                slow("b", 15);
                slow("b", 350);
                trpl::sleep(Duration::from_millis(50)).await;
                println!("'b' finished.");
            };

            trpl::race(a, b).await;
        }
    );
}
```

输出

```shell
'a' started.
'a' ran for 30ms
'a' ran for 10ms
'a' ran for 20ms
'b' started.
'b' ran for 75ms
'b' ran for 10ms
'b' ran for 15ms
'b' ran for 350ms
'a' finished.
```

```rust
use std::{thread, time::Duration};

fn main() {
    trpl::run(
        async {
            let one_ms = Duration::from_millis(1);
			
            // 现在两个 future 的工作会相互交替运行
            let a = async {
                println!("'a' started.");
                slow("a", 30);
                trpl::sleep(one_ms).await;
                slow("a", 10);
                trpl::sleep(one_ms).await;
                slow("a", 20);
                trpl::sleep(one_ms).await;
                println!("'a' finished.");
            };

            let b = async {
                println!("'b' started.");
                slow("b", 75);
                trpl::sleep(one_ms).await;
                slow("b", 10);
                trpl::sleep(one_ms).await;
                slow("b", 15);
                trpl::sleep(one_ms).await;
                slow("b", 350);
                trpl::sleep(one_ms).await;
                println!("'b' finished.");
            };

            trpl::race(a, b).await;
        }
    );
}
```

输出

```shell
'a' started.
'a' ran for 30ms
'b' started.
'b' ran for 75ms
'a' ran for 10ms
'b' ran for 10ms
'a' ran for 20ms
'b' ran for 15ms
'a' finished.
```

当需要交还控制权给运行时，可以使用 `yield_now` 函数让切换继续进行。

```rust
use std::{thread, time::Duration};

fn main() {
    trpl::run(
        async {
            let a = async {
                println!("'a' started.");
                slow("a", 30);
                trpl::yield_now().await;
                slow("a", 10);
                trpl::yield_now().await;
                slow("a", 20);
                trpl::yield_now().await;
                println!("'a' finished.");
            };

            let b = async {
                println!("'b' started.");
                slow("b", 75);
                trpl::yield_now().await;
                slow("b", 10);
                trpl::yield_now().await;
                slow("b", 15);
                trpl::yield_now().await;
                slow("b", 350);
                trpl::yield_now().await;
                println!("'b' finished.");
            };

            trpl::race(a, b).await;
        }
    );
}
```

```rust
use std::time::{Duration, Instant};

// 非并发，顺序执行
fn main() {
    trpl::run(
        async {
            let one_ns = Duration::from_nanos(1);
            let start = Instant::now();
            async {
                for _ in 1..1000 {
                    trpl::sleep(one_ns).await;
                }
            }.await;
            let time = Instant::now() - start;
            println!(
                "'sleep' version finished after {} seconds.",
                time.as_secs_f32()
            );

            let start = Instant::now();
            async {
                for _ in 1..1000 {
                    trpl::yield_now().await;
                }
            }.await;
            let time = Instant::now() - start;
            println!(
                "'yield' version finished after {} seconds.",
                time.as_secs_f32()
            );
        }
    );
}
```

输出

```shell
'sleep' version finished after 15.143559 seconds.
'yield' version finished after 0.0003077 seconds.
```

注

1. 相较于 `sleep`，使用 `yield_now`能更细粒度地控制，并且速度更快

**自定义异步抽象**

可以将 futures 组合起来形成一个新模式。

```rust
use std::time::Duration;
use trpl::Either;

async fn timeout<F: Future>(future_to_try: F, max_time: Duration)
                            -> Result<F::Output, Duration> {
    match trpl::race(future_to_try, trpl::sleep(max_time)).await {
        Either::Left(output) => Ok(output),
        Either::Right(_) => Err(max_time),
    }
}

fn main() {
    trpl::run(
        async {
            let slow = async {
                trpl::sleep(Duration::from_secs(5)).await;
                "Finally finished"
            };

            match timeout(slow, Duration::from_secs(2)).await {
                // 运行代码，它会在超时之后打印失败模式
                Ok(message) => println!("Succeeded with '{message}'"),
                Err(duration) => {
                    println!("Failed after {} seconds", duration.as_secs())
                }
            }
        }
    );
}
```

输出

```shell
Failed after 2 seconds
```

注

1. 由于 future 可以和其他 future 组合，可以使用更小的异步代码块来构建非常强力的工具

## 流

流类似于一种异步形式的迭代器，可以从任何迭代器上创建流。

流本身可以像使用迭代器一样使用它的所有工具方法，也可以将其看作 future 类型。

```rust
use trpl::StreamExt;     // 使用 StreamExt，注释掉则使用Stream

fn main() {
    trpl::run(
        async {
            let values = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10];
            let iter = values.iter().map(|n| n * 2);		// 将其所有值翻倍
            let mut stream = trpl::stream_from_iter(iter);	// 将迭代器转换为流

            while let Some(value) = stream.next().await {
                println!("The value was: {value}");
            }
        }
    );
}
```

注

1. `Stream` trait 定义了一个底层接口用于有效地结合 `Iterator` 与 `Future` trait
2. `StreamExt` trait 在 `Stream` 之上提供了一组高层 API，其中包括了 `next` 和其它类似于 `Iterator` trait 提供的工具方法

```rust
use trpl::StreamExt;

fn main() {
    trpl::run(
        async {
            let values = 1..101;
            let iter = values.map(|n| n * 2);
            let stream = trpl::stream_from_iter(iter);

            let mut filtered =
                stream.filter(|value| value % 3 == 0 || value % 5 == 0);
            // 使用 filter 方法来过滤出仅为 3 或 5 的倍数的项

            while let Some(value) = filtered.next().await {
                println!("The value was: {value}");
            }
        }
    );
}
```

适用场景

- 队列中逐渐可用的项目
- 文件系统中逐步拉取的数据块
- 网络上随时间到达的数据
- 实时通信

**组合流**

```rust
use trpl::{ReceiverStream, Stream, StreamExt};
use std::{pin::pin, time::Duration};

// 不能将 get_messages 函数本身变为异步函数，否则调用者则不得不 await get_messages 本身来获取流
// 这会要求其在返回接收端流之前发送所有的消息，包括消息之间的休眠延时，结果是超时将毫无用处
fn get_messages() -> impl Stream<Item = String> {
    let (tx, rx) = trpl::channel();

    trpl::spawn_task(
        async move {
            let messages = ["a", "b", "c", "d", "e", "f", "g", "h", "i", "j"];
            for (index, message) in messages.into_iter().enumerate() {
                // 使用 enumerate 迭代器方法以便能够同时获得项本身和其索引
                let time_to_sleep = if index % 2 == 0 { 100 } else { 300 };
                trpl::sleep(Duration::from_millis(time_to_sleep)).await;    // 实现消息间的延迟且不造成阻塞
                tx.send(format!("Message: '{message}'")).unwrap();
            }
        }
    );
    ReceiverStream::new(rx) 	//将 trpl::channel 的 rx 接收端转换为一个带有 next 方法的 Stream
}

fn main() {
    trpl::run(
        async {
            let mut messages =
                pin!(get_messages().timeout(Duration::from_millis(200)));
                // get_messages()当作 future 使用
                // 超时辅助函数产生了一个需要 pin 住才能轮询的流
            while let Some(result) = messages.next().await {
                match result {
                    Ok(message) => println!("{message}"),
                    Err(reason) => eprintln!("Problem: {reason:?}"),
                }
            }
        }
    );
}
```

输出

```shell
Message: 'a'
Problem: Elapsed(())
Message: 'b'
Message: 'c'
Problem: Elapsed(())
Message: 'd'
Message: 'e'
Problem: Elapsed(())
Message: 'f'
Message: 'g'
Problem: Elapsed(())
Message: 'h'
Message: 'i'
Problem: Elapsed(())
Message: 'j'
```

注

1. 超时最终并不会阻止消息到达，仍然能够得到所有原始的消息，因为信道是无限的，它可以存储内存所允许的所有消息
2. 如果消息在超时之前没有到达，流处理器会做出相应处理，不过当再次轮询流时，消息现在可能已经到达了

**合并流**

```rust
use trpl::{ReceiverStream, Stream, StreamExt};
use std::{pin::pin, time::Duration};

fn get_intervals() -> impl Stream<Item = u32> {
    let (tx, rx) = trpl::channel();

    trpl::spawn_task(
        async move {
            let mut count = 0;
            loop {                 // 在运行时被回收时才会结束的无限循环
// 很多程序需要无限地运行下去，通过异步编程，这不会阻塞任何其它内容，只要循环的每次迭代中有至少一个 await point
                trpl::sleep(Duration::from_millis(1)).await;
                count += 1;
                tx.send(count).unwrap();
            }
        }
    );
    ReceiverStream::new(rx)         //发送已经过去的间隔次数
}

fn main() {
    trpl::run(
        async {
            let messages = get_messages().timeout(Duration::from_millis(200));
            let intervals = get_intervals();
            let merged = messages.merge(intervals);     // 通过 merge 方法合并 messages 和 intervals 流
            // 将多个流合并为一个从任何一个来源流的项可用时返回项的流，并且不会保持任何特定顺序
            // 此时，messages 和 intervals 都不需要被 pin 住或是可变的，因为它们都会被合并进一个单一的 merged 流
            // ❌ 两个流有着不同的类型
            while let Some(result) = messages.next().await {
                match result {
                    Ok(message) => println!("{message}"),
                    Err(reason) => eprintln!("Problem: {reason:?}"),
                }
            }
        }
    );
}
```

为了合并这两个类型，需要将其中一个流转换以适配另一个流。

```rust
// 程序永远也不会停止
// 来自英文字母表的消息会淹没在所有的间隔计数消息之中
let messages = get_messages().timeout(Duration::from_millis(200));
let intervals = get_intervals()
    .map(|count| format!("Interval: {count}"))	// 格式化将 intervals 转换为字符串
    .timeout(Duration::from_secs(10));			// 需要匹配 messages 中的 Timeout，但不希望 intervals 有超时，因此创建一个十秒超时
let merged = messages.merge(intervals);
let mut stream = pin!(merged);					
// 需要将 stream 变为可变，这样 while let 循环的 next 调用可以遍历流，并且需要 pin 住它才能安全地执行
```

输出

```shell
--snip--
Interval: 38
Interval: 39
Interval: 40
Message: 'a'
Interval: 41
Interval: 42
Interval: 43
--snip--
```

```rust
// 流中轮询 20 个项后停止，同时间隔不会淹没消息
let messages = get_messages().timeout(Duration::from_millis(200));
let intervals = get_intervals()
    .map(|count| format!("Interval: {count}"))
    .throttle(Duration::from_millis(100))	
    .timeout(Duration::from_secs(10));
// 限流（Throttling）是一种限制函数被调用速率的方式，每 100 毫秒一次较为合适。因为这大概是消息到达的间隔
// throttle 调用产生了一个封装了原始流的新流，这样原始流只会在限制速率下而不是其原生速率下轮询
let merged = messages.merge(intervals).take(20);
// 在 merged 流上调用 take 方法，来限制最终输出
let mut stream = pin!(merged);
```

```shell
Interval: 1
Message: 'a'
Interval: 2
Interval: 3
Problem: Elapsed(())
Interval: 4
Message: 'b'
Interval: 5
Message: 'c'
Interval: 6
Interval: 7
Problem: Elapsed(())
Interval: 8
Message: 'd'
Interval: 9
Message: 'e'
Interval: 10
Interval: 11
Problem: Elapsed(())
Interval: 12
```

有了这两个基于信道的流，当信道的另一端关闭时 send 方法可能会失败，这取决于运行时如何执行组成流的 future。在一个行为良好的应用程序中，应明确地处理该错误，至少应终止循环，以避免继续尝试发送消息。

```rust
fn get_messages() -> impl Stream<Item = String> {
    let (tx, rx) = trpl::channel();

    trpl::spawn_task(
        async move {
            let messages = ["a", "b", "c", "d", "e", "f", "g", "h", "i", "j"];

            for (index, message) in messages.into_iter().enumerate() {
                let time_to_sleep = if index % 2 == 0 { 100 } else { 300 };
                trpl::sleep(Duration::from_millis(time_to_sleep)).await;

                if let Err(send_error) = tx.send(format!("Message: '{message}'")) {
                    eprintln!("Cannot send message '{message}': {send_error}");
                    break;
                }
            }
        }
    );
    ReceiverStream::new(rx)
}

fn get_intervals() -> impl Stream<Item = u32> {
    let (tx, rx) = trpl::channel();

    trpl::spawn_task(
        async move {
            let mut count = 0;
            loop {
                trpl::sleep(Duration::from_millis(1)).await;
                count += 1;

                if let Err(send_error) = tx.send(count) {
                    eprintln!("Could not send interval {count}: {send_error}");
                    break;
                };
            }
        }
    );
    ReceiverStream::new(rx)
}
```

## 异步traits

**Future trait**

定义

```rust
use std::pin::Pin;
use std::task::{Context, Poll};

pub trait Future {
    type Output;		// 最终解析出的类型

    fn poll(self: Pin<&mut Self>, cx: &mut Context<'_>) -> Poll<Self::Output>;
}
```

`Poll` 类型类似 `Option`，其中`Pending` 变体表明 future 仍然还有工作要进行，所有调用者稍后需要再次检查。`Ready` 变体表明 future 已经完成了其工作并且 `T` 的值是可用的。

```rust
enum Poll<T> {
    Ready(T),
    Pending,
}
```

当使用 `await` 的代码时，Rust 会在底层将其编译为调用 `poll` 的代码。

```rust
let mut page_title_fut = page_title(url);
loop {		// 类似循环 				
// 但为避免阻塞，Rust 确保循环可以将控制权交给一些可以暂停当前 future 转而去处理其它 future 并在之后再次检查当前 future 的内容，这就是异步运行时
    match page_title_fut.poll() {
        Ready(value) => match page_title {
            Some(title) => println!("The title for {url} was {title}"),
            None => println!("{url} had no title"),
        }
        Pending => {
            // continue
        }
    }
}
```

注

1. 运行时轮询其所负责的每一个 future，在它们还没有完成时使其休眠

**Pin 和 Unpin traits**

```rust
use std::pin::Pin;
use std::task::{Context, Poll};

pub trait Future {
    type Output;

    // Required method
    fn poll(self: Pin<&mut Self>, cx: &mut Context<'_>) -> Poll<Self::Output>;
	// self类型注解：告诉 Rust 在调用该方法时 self 必须具备的类型，它不能是任意类型
}
```

`Pin` 是一个类指针类型（`&`，`&mut`，`Box` 和 `Rc`）的封装，本身并不是一个指针，它单纯地是一个编译器可以用来约束指针使用的工具。

默认情况下移动任何拥有其自身引用的对象是不安全的，如果移动数据结构本身，这些内部引用会停留在指向老的地址。然而，这些内存地址现在是无效的。一方面，当修改这些数据结构时这些值不会被更新。另一方面，最终可能会读取到完全不相关的数据。

<img src=".\img\image-20250724165202749.png" alt="image-20250724165202749" style="zoom:70%;" />

理论上，Rust 编译器也可以在对象被移动时尝试更新其所有的引用，不过这会增加很多性能开销。相反如果确保相关的数据结构不会再内存中移动，就无需更新任何引用。而 `Pin` 提供了所需的保证。当通过 `Pin` 封装一个值的引用时，它就无法再移动了。也就是说，如果有 `Pin<Box<SomeType>>`，实际上 pin 住了 `SomeType` 的值，而不是 `Box` 指针。

<img src=".\img\image-20250724165641278.png" alt="image-20250724165641278" style="zoom:67%;" />

`Box` 指针仍然可以随意移动，而最终被引用的数据保持不动。

<img src=".\img\image-20250724165812849.png" alt="image-20250724165812849" style="zoom:67%;" />

只有当项中含有内部引用的时候才需要考虑 pin，而某个 `Future`（或 `async` 块）内部可能包含了自引用（即它内部某个字段引用了同结构的另一个字段）。

`Unpin` 告知编译器给定类型无需维护是否可以安全地移动的保证，默认不包含内部引用，移动是安全的。正如 `Send` 和 `Sync` 一样，编译器自动为所有被证明为安全的类型实现 `Unpin`。

**Stream trait**

`Stream`定义

```rust
use std::pin::Pin;
use std::task::{Context, Poll};

trait Stream {
    type Item;						// 流所产生项的类型，可有0个或多个项目

    fn poll_next(
        self: Pin<&mut Self>,
        cx: &mut Context<'_>
    ) -> Poll<Option<Self::Item>>;
}
```

`StreamExt`定义

```rust
trait StreamExt: Stream {
    async fn next(&mut self) -> Option<Self::Item>
    where
        Self: Unpin;

    // other methods...
}
```

## future、任务和线程

线程需要操作系统和硬件的支持。而在异步模型中，并发操作无需各自独立的线程，运行在任务上。

任务类似于线程，但不是由操作系统管理，而是由库级别的代码管理：也就是运行时。

即便一个函数在运行时上产生一个异步任务而另一个产生一个系统线程，其返回的流不受该区别的影响。

```rust
fn get_intervals() -> impl Stream<Item = u32> {
    let (tx, rx) = trpl::channel();

    // 这里不是 trpl::spawn，是 std::thread::spawn
    thread::spawn(move || {
        let mut count = 0;
        loop {
            // 同样，这里不是 trpl::sleep，是 std::thread::sleep
            thread::sleep(Duration::from_millis(1));
            count += 1;

            if let Err(send_error) = tx.send(count) {
                eprintln!("Could not send interval {count}: {send_error}");
                break;
            };
        }
    });

    ReceiverStream::new(rx)
}
```

可以在现代计算机中产生数以百万计的异步任务，但如果尝试用线程来这样做，则会耗尽内存。

future 是 Rust 中最细粒度的并发单位，其运行时管理任务，任务则管理 future。在这点上，任务类似于轻量的、运行时管理的线程，并由运行时而非操作系统管理。

**线程与异步**

- 如果工作是可并行的，例如处理大量数据其中每一部分数据都可以单独处理时，线程是更佳的选择
- 如果工作是并发的，例如处理大量不同来源的消息，它们可能有着不同的间隔或者速率，异步是更佳的选择

如果同时需要并行和并发，可以同时使用线程和异步。

```rust
use std::{thread, time::Duration};

fn main() {
    let (tx, mut rx) = trpl::channel();

    /* 产生一个线程来获取信道发送端的所有权 */
    thread::spawn(move || {
        for i in 1..11 {
            tx.send(i).unwrap();
            thread::sleep(Duration::from_secs(1));
        }
    });
	
    /* 异步代码块 */
    trpl::run(async {
        while let Some(message) = rx.recv().await {
            println!("{message}");
        }
    });
}
```

# 面对对象编程

## 面对对象语言特征

面向对象的语言有一些共同的特征，如对象、封装和继承。

**对象**

一种定义为：面向对象的程序由对象组成，一个对象包含数据和操作这些数据的过程，这些过程通常被称为方法或操作。

在这个定义下，Rust 是面向对象的，因为结构体和枚举包含数据而 `impl` 块提供了在结构体和枚举之上的方法。

**封装**

使用对象的代码不应能直接触及对象的内部并改变数据或行为，对象交互的唯一方式是通过其公有 API。这使得程序员能够更改和重构一个对象的内部实现，而无需改变使用该对象的代码。

Rust 可以使用 `pub` 关键字来决定代码中的哪些模块、类型、函数和方法是公有的，而默认情况下其他所有内容都是私有的。

```rust
pub struct AveragedCollection {			// 结构体内的字段仍保持私有
    list: Vec<i32>,				
    average: f64,
}

impl AveragedCollection {
	
    // 公有方法 add、remove 和 average 是访问或修改 AveragedCollection 实例中数据的唯一途径    
    pub fn add(&mut self, value: i32) {
        self.list.push(value);
        self.update_average();
    }

    pub fn remove(&mut self) -> Option<i32> {
        let result = self.list.pop();
        match result {
            Some(value) => {
                self.update_average();
                Some(value)
            }
            None => None,
        }
    }

    pub fn average(&self) -> f64 {
        self.average
    }

	// 私有的 update_average 方法来更新 average 字段
    fn update_average(&mut self) {
        let total: i32 = self.list.iter().sum();
        self.average = total as f64 / self.list.len() as f64;
    }
}
```

**继承**

继承（Inheritance）是一种机制：一个对象可以从另一个对象的定义中继承元素，从而获得父对象的数据和行为，无需再次定义。

Rust 不支持定义一个结构体时继承父结构体的字段和方法，除非使用宏。但根据运用继承的原因，Rust 提供了其他解决方案

- 代码复用：在 Rust 代码中可以使用默认 trait 方法实现来进行有限的代码复用
- 子类型可以用于父类型被使用的地方：也被称为多态，指的是可以处理多种子类数据的代码，Rust 使用泛型来抽象不同可能的类型，并通过 trait bound 来约束这些类型所必须提供的内容

继承在许多新的编程语言中逐渐不被青睐，因为它经常有分享过多代码的风险。子类不应总是共享父类的所有特征，这会降低程序设计的灵活性，或因为方法不适用于子类而导致错误。此外，一些语言只允许单一继承进一步限制了程序设计的灵活性。出于这些原因，Rust 使用 trait 对象而非继承。

## trait 对象

在拥有继承的语言中，可以定义一个名为 `Component` 的类，该类上有一个 `draw` 方法。其他的类比如 `Button`、`Image` 和 `SelectBox` 会从 `Component` 派生并因此继承 `draw` 方法。它们各自都可以重写 `draw` 方法来定义自己的行为。不过 Rust 并没有继承，需要寻找另一种方式来设计 `gui` 库。

**定义通用行为的trait**

trait 对象将数据和行为两者相结合，从这种意义上说更类似其他语言中的对象。不过 trait 对象不同于传统的对象，不能向 trait 对象添加数据，其具体的作用是允许对通用行为进行抽象。

`src/lib.rs`

```rust
// Draw trait 的定义
pub trait Draw {
    fn draw(&self);
}
// 定义含 Draw trait 对象
pub struct Screen {
    pub components: Vec<Box<dyn Draw>>,				// vector 的类型是 Box<dyn Draw>
    // 此为一个 trait 对象：它是 Box 中任何实现了 Draw trait 的类型的替身
}

impl Screen {
    pub fn run(&self) {
        for component in self.components.iter() {	// 对其 components 上的每一个组件调用 draw 方法
            component.draw();
        }
    }
}

/* 使用泛型和trait bound */

// 这限制了 Screen 实例必须拥有一个全是 Button 类型或者全是 TextField 类型的组件列表
pub struct Screen<T: Draw> {
    pub components: Vec<T>,
}

impl<T> Screen<T>
where
    T: Draw,
{
    pub fn run(&self) {
        for component in self.components.iter() {
            component.draw();
        }
    }
}
```

注

1. 与带有 trait bound 的泛型类型参数的结构体不同，泛型类型参数一次只能替代一个具体类型；而 trait 对象则允许在运行时替代多种具体类型
2. 如果只需要相同类型集合，则倾向于使用泛型和 trait bound

**实现trait**

`src/lib.rs`

```rust
// Button 结构体实现
pub struct Button {
    pub width: u32,
    pub height: u32,
    pub label: String,
}

impl Draw for Button {
    fn draw(&self) {
        // 实际绘制按钮的代码
    }
}
```

`src/main.rs`

```rust
use gui::Draw;
use gui::{Button, Screen};

struct SelectBox {
    width: u32,
    height: u32,
    options: Vec<String>,
}

impl Draw for SelectBox {
    fn draw(&self) {
        // code to actually draw a select box
    }
}


fn main() {
    // 通过将 SelectBox 和 Button 放入 Box<T> 转变为 trait 对象再放入 Screen 实例中
    let screen = Screen {
        components: vec![
            Box::new(SelectBox {
                width: 75,
                height: 10,
                options: vec![
                    String::from("Yes"),
                    String::from("Maybe"),
                    String::from("No"),
                ],
            }),
            Box::new(Button {
                width: 50,
                height: 10,
                label: String::from("OK"),
            }),
        ],
    };
	// 可以调用 Screen 的 run 方法，它会调用每个组件的 draw 方法
    screen.run();
}

```

使用 trait 对象和 Rust 类型系统无需在运行时检查一个值是否实现了特定方法或者担心在调用时因为值没有实现方法而产生错误，如果值没有实现 trait 对象所需的 trait 则 Rust 不会编译这些代码。

```rust
use gui::Screen;

fn main() {
    let screen = Screen {
        components: vec![Box::new(String::from("Hi"))],		// ❌ String 没有实现 Draw trait
    };

    screen.run();
}
```

**trait对象执行动态分发**

编译器为每一个被泛型类型参数代替的具体类型生成了函数和方法的非泛型实现，称之为静态分发，即编译器在编译时就知晓要调用什么方法。而在动态分发的场景下，编译器会生成负责在运行时确定该调用什么方法的代码。

当使用 trait 对象时，Rust 必须使用动态分发。编译器无法知晓所有可能用于 trait 对象代码的类型，所以它也不知道应该调用哪个类型的哪个方法实现。为此，Rust 在运行时使用 trait 对象中的指针来知晓需要调用哪个方法。这会带来运行时开销，同时也会阻止一些优化。

<img src=".\img\image-20250726222453756.png" alt="image-20250726222453756" style="zoom:47%;" />

## 面对对象设计模式

利用状态模式编写一个博客发布结构体的例子。

最终功能

- 博文从空白的草稿开始
- 一旦草稿完成，请求审核博文
- 一旦博文过审，它将被发表
- 只有被发表的博文的内容会被打印，这样就不会意外打印出没有被审核的博文的文本

`src/lib.rs`

```rust
pub struct Post {
    // 字段均是私有的
    state: Option<Box<dyn State>>,				// Option<T> 类型的 trait 对象
    content: String,
}

impl Post {
    pub fn new() -> Post {
        Post {
            state: Some(Box::new(Draft {})),	// 确保无论何时新建一个 Post 实例，它都会从草稿开始
            content: String::new(),
        }
    }
}

impl Post {
    /* 存放博文内容的文本 */
    pub fn add_text(&mut self, text: &str) {
        self.content.push_str(text);
    }

    /* 请求审核来改变博文的状态 */
    pub fn request_review(&mut self) {
        if let Some(s) = self.state.take() {
            // 调用 take 方法将 state 字段中的 Some 值取出并留下一个 None
            self.state = Some(s.request_review())
        }
    }

    /* 添加 approve 以改变 content 的行为 */
    pub fn approve(&mut self) {
        if let Some(s) = self.state.take() {
            self.state = Some(s.approve())
        }
    }

    /* content 根据 Post 的当前状态返回值 */
    pub fn content(&self) -> &str {
        self.state.as_ref().unwrap().content(self)
        // as_ref 方法是因为需要 Option 中值的引用而不是获取其所有权，返回一个 Option<&Box<dyn State>>
        // 当调用其 content 时，解引用强制转换会作用于 & 和 Box ，这样最终会调用实现了 State trait 的类型的 content 方法
    }
}


// 定义所有不同状态的博文所共享的行为
trait State {
    fn request_review(self: Box<Self>) -> Box<dyn State>;
    fn approve(self: Box<Self>) -> Box<dyn State>;
    // 该方法只可在持有这个类型的 Box 上被调用
    fn content<'a>(&self, post: &'a Post) -> &'a str {
        ""
    }
    // 默认（未发表前）返回空
}

// 草稿状态
struct Draft {}

impl State for Draft {
    fn request_review(self: Box<Self>) -> Box<dyn State> {
        Box::new(PendingReview {})
    }
    fn approve(self: Box<Self>) -> Box<dyn State> {
        self
    }
}

// 审核状态
struct PendingReview {}

impl State for PendingReview {
    fn request_review(self: Box<Self>) -> Box<dyn State> {
        self
    }
    fn approve(self: Box<Self>) -> Box<dyn State> {
        Box::new(Published {})
    }
}

// 发表状态
struct Published {}

impl State for Published {
    fn request_review(self: Box<Self>) -> Box<dyn State> {
        self
    }

    fn approve(self: Box<Self>) -> Box<dyn State> {
        self
    }
    fn content<'a>(&self, post: &'a Post) -> &'a str {			// 返回发表文章内容
        &post.content
    }
}
```

`src/main.rs`

```rust
use blog::Post;

fn main() {
    let mut post = Post::new();

    post.add_text("I ate a salad for lunch today");
    assert_eq!("", post.content());

    post.request_review();
    assert_eq!("", post.content());

    post.approve();
    assert_eq!("I ate a salad for lunch today", post.content());
}
```

**将状态和行为编码为类型**

在编译时发现错误。

`src/lib.rs`

```rust
// 未发布
pub struct Post {
    content: String,
}

// 已发表
pub struct DraftPost {
    content: String,
}

impl Post {
    pub fn new() -> DraftPost {
        DraftPost {
            content: String::new(),
        }
    }

    pub fn content(&self) -> &str {
        &self.content
    }
}

impl DraftPost {
    pub fn add_text(&mut self, text: &str) {
        self.content.push_str(text);
    }
    pub fn request_review(self) -> PendingReviewPost {
        PendingReviewPost {
            content: self.content,
        }
    }
}

pub struct PendingReviewPost {
    content: String,
}

impl PendingReviewPost {
    pub fn approve(self) -> Post {
        Post {
            content: self.content,
        }
    }
}
```

`src/main.rs`

```rust
use blog::Post;

fn main() {
    let mut post = Post::new();

    post.add_text("I ate a salad for lunch today");

    let post = post.request_review();

    let post = post.approve();

    assert_eq!("I ate a salad for lunch today", post.content());
}
```

在 Rust 中面向对象模式并不总是最好的解决方案，因为 Rust 拥有像所有权这样的面向对象语言所没有的特性。

# 模式与模式匹配

模式（Patterns）是用来匹配类型的结构。

## 可使用模式的位置

**match分支**

一个模式常用的位置是 `match` 表达式的分支。

```rust
match VALUE {
    PATTERN => EXPRESSION,
    PATTERN => EXPRESSION,
    PATTERN => EXPRESSION,
}
```

`match` 表达式要求模式是穷尽的， `_` 可以匹配所有情况，不过它从不绑定任何变量。

**if let条件表达式**

```rust
fn main() {
    let favorite_color: Option<&str> = None;
    let is_tuesday = false;
    let age: Result<u8, _> = "34".parse();

    if let Some(color) = favorite_color {
        println!("Using your favorite color, {color}, as the background");
    } else if is_tuesday {
        println!("Tuesday is green day!");
    } else if let Ok(age) = age {
        if age > 30 {
            println!("Using purple as the background color");
        } else {
            println!("Using orange as the background color");
        }
    } else {
        println!("Using blue as the background color");
    }
}
```

`if let` 表达式的缺点在于其穷尽性没有为编译器所检查，如果去掉最后的 `else` 块而遗漏处理一些情况，编译器也不会警告这类可能的逻辑错误。

**while let条件循环**

只要模式匹配就一直进行 `while` 循环。

```rust
    let (tx, rx) = std::sync::mpsc::channel();
    std::thread::spawn(move || {
        for val in [1, 2, 3] {
            tx.send(val).unwrap();
        }
    });

    while let Ok(value) = rx.recv() {
        println!("{value}");
    }
```

**for 循环**

在 `for` 循环中，模式是 `for` 关键字直接跟随的值。

```rust
    let v = vec!['a', 'b', 'c'];

    for (index, value) in v.iter().enumerate() {
        println!("{value} is at index {index}");
    }
```

**let**

```rust
let PATTERN = EXPRESSION;
```

**函数参数**

函数参数也可以是模式。

```rust
fn print_coordinates(&(x, y): &(i32, i32)) {
    println!("Current location: ({x}, {y})");
}

fn main() {
    let point = (3, 5);
    print_coordinates(&point);
}
```

闭包类似于函数，也可以在闭包参数列表中使用模式。

## 模式形式

模式有两种形式：refutable（可反驳的）和 irrefutable（不可反驳的）。

- refutable：对某些可能的值进行匹配会失败的模式
- irrefutable：能匹配任何传递的可能值的模式

函数参数、`let` 语句和 `for` 循环只能接受不可反驳的模式，`if let` 和 `while let` 表达式可以接受可反驳和不可反驳的模式，`match` 除最后一个分支可以使用能匹配任何剩余值的不可反驳模式，其他匹配分支必须使用可反驳模式。

```rust
let Some(x) = some_option_value;		// ❌ 在不可反驳模式的地方使用可反驳模式
```

为了修复在需要不可反驳模式的地方使用可反驳模式的情况，可以修改使用模式的代码。

```rust
if let Some(x) = some_option_value else {
    return;
};
```

然而，如果给 `if let` 提供一个不可反驳模式，编译器就会给出警告。

```rust
if let x = 5 {							// Rust 会警告将不可反驳模式用于 if let 是没有意义
    println!("{x}");
};
```

## 模式语法

**匹配字面值**

```rust
    let x = 1;

    match x {
        1 => println!("one"),
        2 => println!("two"),
        3 => println!("three"),
        _ => println!("anything"),
    }
```

**匹配命名变量**

命名变量（Named variables）是匹配任何值的不可反驳模式。

```rust
    let x = Some(5);
    let y = 10;

    match x {
        Some(50) => println!("Got 50"),
        Some(y) => println!("Matched, y = {y}"),	// 在 match 表达式的新作用域中y这是一个新变量
        _ => println!("Default case, x = {x:?}"),
    }

    println!("at the end: x = {x:?}, y = {y}");
```

输出

```shell
Matched, y = 5
at the end: x = Some(5), y = 10
```

**匹配多个模式**

在 `match` 表达式中，可以使用 `|` 语法匹配多个模式。

```rust
    let x = 1;

    match x {
        1 | 2 => println!("one or two"),		// 匹配任一个值
        3 => println!("three"),
        _ => println!("anything"),
    }
```

**..=匹配范围**

编译器会在编译时检查范围不为空，而 `char` 和数字值是 Rust 仅有的可以判断范围是否为空的类型，所以范围只允许用于数字或 `char` 值。

```rust
    let x = 5;

    match x {
        1..=5 => println!("one through five"),	//  匹配1、2、3、4或5
        _ => println!("something else"),
    }

    let y = 'c';

    match y {
        'a'..='j' => println!("early ASCII letter"),
        'k'..='z' => println!("late ASCII letter"),
        _ => println!("something else"),
    }
```

## 解构并分解值

也可以使用模式来解构结构体、枚举和元组，以便使用这些值的不同部分。

**解构结构体**

```rust
struct Point {
    x: i32,
    y: i32,
}

fn main() {
    let p = Point { x: 0, y: 7 };

    let Point { x: a, y: b } = p;		// 解析结构体
    assert_eq!(0, a);
    assert_eq!(7, b);
}

/* 只需列出结构体字段的名称，则模式创建的变量会有相同的名称 */

fn main() {
    let p = Point { x: 0, y: 7 };

    let Point { x, y } = p;
    assert_eq!(0, x);
    assert_eq!(7, y);
}
```

也可以使用结构体字面值来匹配模式。

```rust
fn main() {
    let p = Point { x: 0, y: 7 };

    match p {
        Point { x, y: 0 } => println!("On the x axis at {x}"),
        Point { x: 0, y } => println!("On the y axis at {y}"),
        Point { x, y } => {
            println!("On neither axis: ({x}, {y})");
        }
    }
}
```

输出

```shell
On the y axis at 7
```

**解构枚举**

```rust
enum Message {
    Quit,
    Move { x: i32, y: i32 },
    Write(String),
    ChangeColor(i32, i32, i32),
}

fn main() {
    let msg = Message::ChangeColor(0, 160, 255);

    match msg {
        Message::Quit => {
            println!("The Quit variant has no data to destructure.");
        }
        Message::Move { x, y } => {
            println!("Move in the x direction {x} and in the y direction {y}");
        }
        Message::Write(text) => {
            println!("Text message: {text}");
        }
        Message::ChangeColor(r, g, b) => {
            println!("Change color to red {r}, green {g}, and blue {b}");
        }
    }
}
```

**解构嵌套的结构体和枚举**

也可以匹配嵌套的项。

```rust
enum Color {
    Rgb(i32, i32, i32),
    Hsv(i32, i32, i32),
}

enum Message {
    Quit,
    Move { x: i32, y: i32 },
    Write(String),
    ChangeColor(Color),
}

fn main() {
    let msg = Message::ChangeColor(Color::Hsv(0, 160, 255));

    match msg {
        Message::ChangeColor(Color::Rgb(r, g, b)) => {
            println!("Change color to red {r}, green {g}, and blue {b}");
        }
        Message::ChangeColor(Color::Hsv(h, s, v)) => {
            println!("Change color to hue {h}, saturation {s}, value {v}");
        }
        _ => (),
    }
}
```

**解构结构体和元组**

可以用复杂的方式来混合、匹配和嵌套解构模式。

```rust
    let ((feet, inches), Point { x, y }) = ((3, 10), Point { x: 3, y: -10 });
```

**@绑定**

使用 `@` 可以在一个模式中同时测试和保存变量值。

```rust
enum Message {
    Hello { id: i32 },
}
fn main() {
    let msg = Message::Hello { id: 5 };

    match msg {
        Message::Hello {
            id: id_variable @ 3..=7,	// 捕获任何匹配此范围的值并同时测试其值匹配这个范围模式
        } => println!("Found an id in range: {id_variable}"),
        Message::Hello { id: 10..=12 } => {
            println!("Found an id in another range")
        }
        Message::Hello { id } => println!("Found some other id: {id}"),
    }
}
```

输出

```shell
Found an id in range: 5
```

## 忽略模式中的值

**使用 _ 忽略整个值**

```rust
fn foo(_: i32, y: i32) {
    println!("This code only uses the y parameter: {y}");
}

fn main() {
    foo(3, 4);	// 完全忽略作为第一个参数传递的值3
}
```

**使用嵌套的 _ 忽略部分值**

```rust
    let mut setting_value = Some(5);
    let new_setting_value = Some(10);

    match (setting_value, new_setting_value) {
        (Some(_), Some(_)) => {
            println!("Can't overwrite an existing customized value");
        }
        _ => {
            setting_value = new_setting_value;
        }
    }

    println!("setting is {setting_value:?}");
```

输出

```
Can't overwrite an existing customized value
setting is Some(5)
```

也可以在一个模式中的多处使用下划线来忽略特定值。

```rust
    let numbers = (2, 4, 8, 16, 32);

    match numbers {
        (first, _, third, _, fifth) => {
            println!("Some numbers: {first}, {third}, {fifth}");
        }
    }
```

**通过在变量名开头加 _ 来忽略未使用的变量**

如果创建了一个变量却不在任何地方使用它，Rust 通常会警告。但有时创建一个还未使用的变量是有用的，比如正在设计原型或刚刚开始一个项目，为此可以用下划线作为变量名的开头。

```rust
fn main() {
    let _x = 5;		// 暂时不使用x 
    let y = 10;
}
```

只使用 `_` 和使用以下划线开头的名称有些不同：比如 `_x` 仍会将值绑定到变量，而 `_` 则完全不会绑定。

```rust
    let s = Some(String::from("Hello!"));

    if let Some(_s) = s {		// ❌
     if let Some(_) = s {		// ✅
        println!("found a string");
    }

    println!("{s:?}");
```

**用 .. 忽略剩余值**

对于有多个部分的值，可以使用 `..` 语法来只使用特定部分并忽略其它值。

```rust
struct Point {
    x: i32,
    y: i32,
    z: i32,
}

fn main() {
    let origin = Point { x: 0, y: 0, z: 0 };

    match origin {
        Point { x, .. } => println!("x is {x}"),
    }
}
```

`..` 会扩展为所需要的值的数量。

```rust
fn main() {
    let numbers = (2, 4, 8, 16, 32);

    match numbers {
        (first, .., last) => {
            println!("Some numbers: {first}, {last}");
        }
    }
}
```

然而使用 `..` 必须是无歧义的，如果期望匹配和忽略的值是不明确的，Rust 会报错。

```rust
fn main() {
    let numbers = (2, 4, 8, 16, 32);

    match numbers {
        (.., second, ..) => {			// ❌ 不能决定在元组中匹配 second 值之前应该忽略多少个值
            println!("Some numbers: {second}")
        },
    }
}
```

## 匹配守卫

匹配守卫（match guard）是一个指定于 `match` 分支模式之后的额外 `if` 条件，它也必须被满足才能选择此分支。仅在 `match` 表达式中可用，不能用于 `if let` 或 `while let` 表达式。

```rust
fn main() {
    let num = Some(4);

    match num {
        Some(x) if x % 2 == 0 => println!("The number {x} is even"),
        Some(x) => println!("The number {x} is odd"),
        None => (),
    }
}
```

通过匹配守卫提供了更复杂的逻辑的能力，缺点是编译器不会尝试为包含匹配守卫的模式检查穷尽性。

```rust
fn main() {
    let x = Some(5);
    let y = 10;

    match x {
        Some(50) => println!("Got 50"),
        Some(n) if n == y => println!("Matched, n = {n}"),	// 这个 y 正是外部的 y
        _ => println!("Default case, x = {x:?}"),
    }

    println!("at the end: x = {x:?}, y = {y}");
}
```

输出

```shell
Default case, x = Some(5)
```

也可以在匹配守卫中使用`|` 来指定多个模式，同时匹配守卫的条件会作用于所有的模式。

```rust
fn main() {
    let x = 4;
    let y = false;

    match x {
        4 | 5 | 6 if y => println!("yes"),	// 等价于 (4 | 5 | 6) if y => ...
        _ => println!("no"),
    }
}
```

输出

```shell
no
```

# 高级特性

## 不安全Rust

 Rust默认保证内存安全，而不安全 Rust 不会强制执行内存安全保证，与常规 Rust 代码无异，但会提供额外的超能力，由程序员确保 `unsafe` 块中的代码以有效的方式访问内存。

有五类可以在不安全 Rust 中进行而不能用于安全 Rust 的操作，它们被称为不安全的超能力

- 解引用裸指针
- 调用不安全的函数或方法
- 访问或修改可变静态变量
- 实现不安全 trait
- 访问 `union` 的字段

`unsafe` 并不会关闭借用检查器或禁用任何其他 Rust 安全检查，只是提供了那五个不会被编译器检查内存安全的功能。通过要求不安全操作位于 `unsafe` 的块中，能够知道与内存安全相关的错误必定位于 `unsafe` 块内。

注

1. 保持 `unsafe` 块尽可能小
2. 为了尽可能隔离不安全代码，最好将不安全代码封装进一个安全的抽象并提供安全 API

**解引用裸指针**

不安全 Rust 有两个被称为裸指针（raw pointers）的类似于引用的新类型，裸指针是不可变或可变的，分别写作 `*const T` 和 `*mut T`。在裸指针的上下文中，不可变意味着指针解引用之后不能直接赋值。

裸指针与引用和智能指针的区别在于

- 允许忽略借用规则，可以同时拥有不可变和可变的指针，或多个指向相同位置的可变指针
- 不保证指向有效的内存
- 允许为空
- 不能实现任何自动清理功能

创建一个不可变裸指针和一个可变裸指针

```rust
fn main() {
    let mut num = 5;
	// 可以在安全代码中创建裸指针，只是不能在不安全块之外解引用裸指针
    let r1 = &raw const num;		// 不可变裸指针
    let r2 = &raw mut num;			// 可变裸指针
    
    unsafe {						// 不安全代码块中解引用裸指针
        println!("r1 is: {}", *r1);
        println!("r2 is: {}", *r2);
    }
}
```

创建一个指向任意内存地址的裸指针

```rust
fn main() {
    let address = 0x012345usize;
    let r = address as *const i32;
}
```

注

1. 创建一个指针不会造成任何危害，只有当访问其指向的值时才有可能遇到无效的值

**调用不安全的函数或方法**

不安全函数和方法与常规函数方法十分类似，除了其开头有一个额外的 `unsafe`。通过在 `unsafe` 块中调用不安全函数，表明已经阅读过此函数的文档并对其是否满足函数自身条件负责。

没有做任何操作的不安全函数

```rust
fn main() {
    unsafe fn dangerous() {}

    unsafe {
        dangerous();
    }
}
```

注

1. 在不安全函数的函数体内部执行不安全操作时，同样需要使用 `unsafe` 块
2. 出于尽量缩小 `unsafe` 块的动机，不再建议在不安全函数体中直接进行不安全操作

**创建不安全代码的安全抽象**

仅因为函数包含不安全代码并不意味着整个函数都需要标记为不安全的，将不安全代码封装进安全函数是一种常见的抽象方式。

```rust
fn main() {
    let mut v = vec![1, 2, 3, 4, 5, 6];

    let r = &mut v[..];

    let (a, b) = r.split_at_mut(3);		// 获取一个 slice 并从给定的索引参数开始将其分割为两个 slice

    assert_eq!(a, &mut [1, 2, 3]);
    assert_eq!(b, &mut [4, 5, 6]);		// ✅
}
```

自定义 `split_at_mut`函数

```rust
fn split_at_mut(values: &mut [i32], mid: usize) -> (&mut [i32], &mut [i32]) {
    let len = values.len();

    assert!(mid <= len);

    (&mut values[..mid], &mut values[mid..])
}

fn main() {
    let mut vector = vec![1, 2, 3, 4, 5, 6];
    let (left, right) = split_at_mut(&mut vector, 3);	
}
// ❌ Rust 的借用检查器无法理解要借用这个 slice 的两个不同部分：它只知道借用了同一个 slice 两次
```

本质上借用 slice 的不同部分是可以的，因为这两段 slice 不会重叠，不过 Rust 还没有智能到能够理解这些，这时可以使用不安全代码。

```rust
use std::slice;

fn split_at_mut(values: &mut [i32], mid: usize) -> (&mut [i32], &mut [i32]) {
    let len = values.len();
    let ptr = values.as_mut_ptr();					// 返回一个 *mut i32 类型的裸指针

    assert!(mid <= len);

    unsafe {
        (
            slice::from_raw_parts_mut(ptr, mid),	// 获取一个裸指针和一个长度来创建一个 slice
            slice::from_raw_parts_mut(ptr.add(mid), len - mid),
            // slice::from_raw_parts_mut 函数是不安全的因为它获取一个裸指针，并必须确信这个指针是有效的
            // 裸指针上的 add 方法也是不安全的，因为其必须确信此地址偏移量也是有效的指针
        )
    }
}

fn main() {
    let mut vector = vec![1, 2, 3, 4, 5, 6];
    let (left, right) = split_at_mut(&mut vector, 3);
}
```

无需将 `split_at_mut` 函数的结果标记为 `unsafe`，并可以在安全 Rust 中调用此函数。创建了一个不安全代码的安全抽象，其代码以一种安全的方式使用了 `unsafe` 代码。

**使用extern函数调用外部代码**

- 外部函数接口：一个编程语言用以定义函数的方式，其允许不同（外部）编程语言调用这些函数

使用`extern`可以创建和使用外部函数接口。因为其他语言不会强制执行 Rust 的规则，Rust 也无法检查这些约束，因此`extern` 块中声明的函数在 Rust 代码中通常是不安全的，所以 `extern` 块本身也必须标注 `unsafe`。

集成 C 标准库中的 `abs` 函数

```rust
// unsafe extern 中声明的任何项都隐式地是 unsafe 的
unsafe extern "C" {				// "C"部分定义了外部函数所使用的应用二进制接口
    fn abs(input: i32) -> i32;	// 另一个语言中的外部函数的签名和名称
}

fn main() {
    unsafe {
        println!("Absolute value of -3 according to C: {}", abs(-3));
    }
}
```

然而，一些函数可以安全地调用。例如，C 标准库中的 `abs` 函数没有任何内存安全方面并且知道它可以使用任何 `i32` 调用。此时可以使用 `safe` 关键字来表明这个特定的函数。

```rust
unsafe extern "C" {
    safe fn abs(input: i32) -> i32;
}

fn main() {
    println!("Absolute value of -3 according to C: {}", abs(-3));	// 调用不再需要 unsafe 块
}
```

也可以使用 `extern` 来创建一个允许其它语言调用 Rust 函数的接口。

```rust
fn main() {
    #[unsafe(no_mangle)]
    pub extern "C" fn call_from_c() {
        println!("Just called a Rust function from C!");
    }
}
// 一旦其编译为动态库并从 C 语言中链接，call_from_c 函数就能够在 C 代码中访问
```

**访问或修改可变静态变量**

全局变量在 Rust 中被称为静态变量。

- 静态变量只能储存拥有 `'static` 生命周期的引用
- 访问不可变静态变量是安全的
- 访问和修改可变静态变量都是不安全 

```rust
static HELLO_WORLD: &str = "Hello, world!";

fn main() {
    println!("name is: {HELLO_WORLD}");
}
```

常量与静态变量区别

- 静态变量中的值有一个固定的内存地址，常量则允许在任何被用到的时候复制其数据
- 静态变量可以是可变的

```rust
static mut COUNTER: u32 = 0;		// 任何读写 COUNTER 的代码都必须位于 unsafe 块中

// 拥有多个线程访问 COUNTER 则可能导致数据竞争，所以这是未定义行为
// 因此，需要将整个函数标记为 unsafe，并在文档注释中说明其安全性限制，以便调用者明确哪些操作是安全的、哪些是不安全的
/// SAFETY: 同时在多个线程调用这个方法是未定义的行为，所以你必须保证同一时间只有一个线程在调用它。
unsafe fn add_to_count(inc: u32) {
    unsafe {
        COUNTER += inc;
    }
}

fn main() {
    unsafe {
        // SAFETY: 它只在main这一个线程被调用
        add_to_count(3);
        println!("COUNTER: {}", *(&raw const COUNTER));	// 只能通过用裸指针解引用来访问可变静态变量的引用
    }
}
```

注

1. 拥有可以全局访问的可变数据，难以保证不存在数据竞争。在任何可能的情况下，优先使用并发技术和线程安全智能指针

**实现不安全trait**

当 trait 中至少有一个方法中包含编译器无法验证的不变式时该 trait 就是不安全的。可以在 `trait` 之前增加 `unsafe` 关键字将 trait 声明为 `unsafe`，同时 trait 的实现也必须标记为 `unsafe`。

```rust
unsafe trait Foo {
    // 方法在这里
}

unsafe impl Foo for i32 {
    // 方法实现在这里
}

fn main() {}
```

如果自定义的类型包含某些未实现 `Send` 或 `Sync` 的类型，例如裸指针，但又想将该类型标记为 `Send` 或 `Sync`，就必须使用 `unsafe`。Rust 不能验证自定义类型保证可以安全的跨线程发送或在多线程间访问，所以需要自己进行检查并通过 `unsafe` 表明。

**访问联合体中的字段**

`union` 和 `struct` 类似，但是在一个实例中同时只能使用一个已声明的字段。联合体主要用于和 C 代码中的联合体进行交互。访问联合体的字段是不安全的，只能在 `unsafe` 块中执行操作，因为 Rust 无法保证当前存储在联合体实例中数据的类型。

**miri**

 一个用来检测未定义行为的 Rust 官方工具。鉴于借用检查器是一个在编译时工作的静态工具，Miri 是一个在运行时工作的动态工具。它通过运行程序或者测试集来检查代码，并检测是否违反了 Rust 工作规则。

Miri 是一个动态分析工具，因此它只能捕获代码实际运行时出现的问题。这意味着需要将其与良好的测试技术相结合。

输入 `rustup +nightly component add miri`安装Miri，输入 `cargo +nightly miri run` or `cargo +nightly miri test` 在项目中使用 Miri。

## 高级trait

**关联类型**

将一个类型占位符与 trait 相关联，使得该 trait 的方法定义可以在签名中使用这些占位符类型，并在实现该 trait 时提供实际的类型。

`Iterator` trait 定义

```rust
pub trait Iterator {
    type Item;			// 关联类型 来替代遍历的值的类型

    fn next(&mut self) -> Option<Self::Item>;
    // 返回一个包含该具体类型值的 Option
}
```

关联类型与泛型

```rust
/* 关联类型 */
impl Iterator for Counter {
    type Item = u32;

    fn next(&mut self) -> Option<Self::Item> {
        // --snip--
    }
}
// 使用关联类型则无需标注类型
// 限定 trait 只能对一个类型实现一次

/* 泛型 */
pub trait Iterator<T> {
    fn next(&mut self) -> Option<T>;
}
// 可以对同一个类型多次实现 trait
// 如 Iterator<String> for Counte
// 每次调用方法时都需要明确类型
```

**默认泛型类型参数和运算符重载**

当使用泛型类型参数时，可以为泛型指定一个默认的具体类型，这种技术的一个很好的示例是运算符重载。Rust 并不允许创建自定义运算符或重载任意运算符，但可以通过实现 `std::ops` 中列出的运算符相关 trait 来重载它们。

重载 `+` 运算符

```rust
use std::ops::Add;

#[derive(Debug, Copy, Clone, PartialEq)]
struct Point {
    x: i32,
    y: i32,
}

impl Add for Point {
    type Output = Point;	// 关联类型 决定 add 方法的返回值类型

    fn add(self, other: Point) -> Point {
        Point {
            x: self.x + other.x,
            y: self.y + other.y,
        }
    }
}

fn main() {
    assert_eq!(
        Point { x: 1, y: 0 } + Point { x: 2, y: 3 },
        Point { x: 3, y: 3 }
    );
}
```

`Add` trait 定义

```rust
trait Add<Rhs=Self> {	// 默认类型参数，如果实现 Add trait 时不指定 Rhs 的具体类型，Rhs 的类型将默认为 Self
    type Output;

    fn add(self, rhs: Rhs) -> Self::Output;
}
```

通过自定义 `Rhs` 类型，让毫米值与米值相加，并让 `Add` 的实现正确处理单位转换。

```rust
use std::ops::Add;

struct Millimeters(u32);				// newtype模式 提供抽象、增强类型安全
struct Meters(u32);

impl Add<Meters> for Millimeters {		// 指定 Rhs 类型参数的值而不是使用默认的 Self
    type Output = Millimeters;

    fn add(self, other: Meters) -> Millimeters {
        Millimeters(self.0 + (other.0 * 1000))
    }
}
```

默认参数类型主要用于两个方面

- 扩展类型而不破坏现有代码
- 在特定情况下进行自定义

**在同名方法之间消除歧义**

定义了两个 trait，`Pilot` 和 `Wizard`，它们都拥有名为 `fly` 的方法。接着在一个本身已经实现了名为 `fly` 方法的类型 `Human` 上实现这两个 trait。

```rust
trait Pilot {
    fn fly(&self);
}

trait Wizard {
    fn fly(&self);
}

struct Human;

impl Pilot for Human {
    fn fly(&self) {
        println!("This is your captain speaking.");
    }
}

impl Wizard for Human {
    fn fly(&self) {
        println!("Up!");
    }
}

impl Human {
    fn fly(&self) {
        println!("*waving arms furiously*");
    }
}

fn main() {
    let person = Human;
    
  	Pilot::fly(&person);	// 调用 Pilot 的 fly
    Wizard::fly(&person);	// 调用 Wizard 的 fly
    person.fly();			// 调用 Human 实例的 fly 
    
}
```

 Rust 可以根据 self 的类型计算出应该使用哪一个 trait 实现，当存在多个类型或者 trait 定义了相同函数名的非方法函数时，Rust 就不总是能计算出期望的是哪一个类型，除非使用完全限定语法。

```rust
trait Animal {
    fn baby_name() -> String;
}

struct Dog;

impl Dog {
    fn baby_name() -> String {
        String::from("Spot")
    }
}

impl Animal for Dog {
    fn baby_name() -> String {
        String::from("puppy")
    }
}

fn main() {
    println!("A baby dog is called a {}", Dog::baby_name());	// 调用定义于 Dog 之上的关联函数
    println!("A baby dog is called a {}", Animal::baby_name()); // ❌ 无法确定
    println!("A baby dog is called a {}", <Dog as Animal>::baby_name()); // ✅ 使用完全限定语法
}
```

**使用超trait**

编写一个依赖另一个 trait 的 trait 定义。

```rust
use std::fmt;

trait OutlinePrint: fmt::Display {				// 指定 OutlinePrint 需要 Display trait
    fn outline_print(&self) {
        let output = self.to_string();
        let len = output.len();
        println!("{}", "*".repeat(len + 4));
        println!("*{}*", " ".repeat(len + 2));
        println!("* {output} *");
        println!("*{}*", " ".repeat(len + 2));
        println!("{}", "*".repeat(len + 4));
    }
}

struct Point {
    x: i32,
    y: i32,
}

impl OutlinePrint for Point {}					// ❌ Display 是必须的而未被实现
```

为了修复这个问题，需要在 `Point` 上实现 `Display` 并满足 `OutlinePrint` 要求的限制。

```rust
// ...
impl fmt::Display for Point {					// 超trait
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "({}, {})", self.x, self.y)
    }
}

fn main() {
    let p = Point { x: 1, y: 3 };
    p.outline_print();
}
```

**在外部类型上实现外部trait**

如果想要在 `Vec<T>` 上实现 `Display`，可以创建一个包含 `Vec<T>` 实例的 `Wrapper` 结构体。

```rust
use std::fmt;

struct Wrapper(Vec<String>);

impl fmt::Display for Wrapper {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "[{}]", self.0.join(", "))		// 使用 self.0 来访问其内部的 Vec<T>
    }
}

fn main() {
    let w = Wrapper(vec![String::from("hello"), String::from("world")]);
    println!("w = {w}");
}
```

## 高级类型

**使用类型别名创建类型同义词**

Rust 提供了声明类型别名的能力，使用 `type` 关键字为现有类型赋予另一个名字。

```rust
fn main() {
    type Kilometers = i32;			// 意味着 Kilometers 是 i32 的同义词
	
    let x: i32 = 5;
    let y: Kilometers = 5;

    println!("x + y = {}", x + y);	// Kilometers 类型的值将被完全当作 i32 类型值来对待
}
```

类型别名通过减少重复使代码更易于管理。

```rust
fn main() {
    type Thunk = Box<dyn Fn() + Send + 'static>;

    let f: Thunk = Box::new(|| println!("hi"));

    fn takes_long_type(f: Thunk) {
        // --snip--
    }

    fn returns_long_type() -> Thunk {
        // --snip--
        Box::new(|| ())
    }
}
```

**从不返回的never type**

Rust 有一个叫做 `!` 的特殊类型，称之为 never type，作用是在函数从不返回的时候充当返回值，从不返回的函数被称为发散函数。

```rust
fn bar() -> ! {				// 函数 bar 从不返回
    // --snip--
}
```

类型为 `!` 的表达式可以被强制转换为任意其他类型。

```rust
let guess: u32 = match guess.trim().parse() {
    Ok(num) => num,
    Err(_) => continue,		// continue 的值是 !
};
```

```rust
impl<T> Option<T> {
    pub fn unwrap(self) -> T {
        match self {			// 整个 match 表达式的结果是 T 类型
            Some(val) => val,
            None => panic!("called `Option::unwrap()` on a `None` value"),	// panic! 是 ! 类型，
        }
    }
}
```

最后一个有着 `!` 类型的表达式是 `loop`。

```rust
    print!("forever ");

    loop {
        print!("and ever ");
    }
```

循环永远也不结束，所以此表达式的值是 `!`。但是如果引入 `break` 就不是了，因为循环在执行到 `break` 后就会终止。

## 高级函数与闭包

**函数指针**

`fn` 被称为函数指针，通过函数指针允许使用函数作为其它函数的参数。

```rust
fn add_one(x: i32) -> i32 {
    x + 1
}
// 参数 f 被指定为一个接受一个 i32 参数并返回 i32 的 fn
fn do_twice(f: fn(i32) -> i32, arg: i32) -> i32 {	
    f(arg) + f(arg)
}

fn main() {
    let answer = do_twice(add_one, 5);

    println!("The answer is: {answer}");
}
```

函数指针实现了所有三个闭包 trait（`Fn`、`FnMut` 和 `FnOnce`），所以可以在需要闭包的地方使用函数指针。

例如，使用 `map` 函数将一个数字 vector 转换为一个字符串 vector，就可以使用闭包。

```rust
    let list_of_numbers = vec![1, 2, 3];
    let list_of_strings: Vec<String> =
        list_of_numbers.iter().map(|i| i.to_string()).collect();
```

或者可以将函数作为 `map` 的参数来代替闭包。

```rust
    let list_of_numbers = vec![1, 2, 3];
    let list_of_strings: Vec<String> =
        list_of_numbers.iter().map(ToString::to_string).collect();
```

**返回闭包**

可以使用 `Fn`、`FnOnce` 和 `FnMut` 返回任何函数类型。

```rust
fn returns_closure() -> impl Fn(i32) -> i32 {
    |x| x + 1
}
```

如果需要处理多个拥有相同签名但是不同实现的函数，就需要使用 trait 对象。

```rust
fn main() {
    let handlers = vec![returns_closure(), returns_initialized_closure(123)];
    for handler in handlers {	
        let output = handler(5);
        println!("{output}");
    }
}

fn returns_closure() -> Box<dyn Fn(i32) -> i32> {
    Box::new(|x| x + 1)
}

fn returns_initialized_closure(init: i32) -> Box<dyn Fn(i32) -> i32> {
    Box::new(move |x| x + init)
}
```

## 宏

宏是一种编写代码的代码，即元编程。

**宏和函数的区别**

- 宏能够接收可变数量的参数，而函数只能接受固定数量的参数
- 宏可以在编译器解析代码前展开，而函数是在运行时调用
- 在一个文件里调用宏之前必须定义它或将其引入作用域，而函数可以在任何地方定义和调用

**声明宏**

Rust 最常用的宏形式是声明宏，声明宏允许编写一些类似 Rust `match` 表达式的代码，匹配的是 Rust 源代码的结构本身，并且发生在编译期间。

可以使用 `macro_rules!` 来定义宏。

`vec!` 宏的简化定义

```rust
#[macro_export]				// 只要导入了定义这个宏的 crate，该宏就是可用的。如果没有该注解这个宏不能被引入作用域
macro_rules! vec {	
    ( $( $x:expr ),* ) => {
        {
            let mut temp_vec = Vec::new();
            $(
                temp_vec.push($x);
            )*
            temp_vec
        }
    };
}
```

**过程宏**

过程宏类似于函数，接收 Rust 代码作为输入，在这些代码上进行操作，然后产生另一些代码作为输出。

要定义过程宏，必须将它们写在一个单独的 crate 中，一个 crate 中可以定义多种类型的过程宏。

有三种类型的过程宏

- 自定义派生宏
- 属性宏
- 函数宏

# 多线程服务器

web server 中涉及到的两个主要协议是超文本传输协议（HTTP）和传输控制协议（TCP）。这两者都是请求 - 响应协议，也就是说，有客户端来初始化请求，并由服务端监听请求并向客户端提供响应，请求与响应的内容由协议本身定义。

TCP 是一个底层协议，它描述了信息如何从一个 server 到另一个的细节，不过其并不指定信息是什么。HTTP 构建于 TCP 之上，它定义了请求和响应的内容。绝大部分情况，HTTP 通过 TCP 传输数据。

`src/main.rs`

```rust
use std::{
    io::{BufReader, prelude::*},
    net::{TcpListener, TcpStream},
};

fn main() {
    let listener = TcpListener::bind("127.0.0.1:7878").unwrap();	// 监听 TCP 连接
	let pool = ThreadPool::new(4);
    
    // 只处理两个请求
    for stream in listener.incoming().take(2) {						
        let stream = stream.unwrap();
        
        pool.execute(|| {
            handle_connection(stream);								// 接收闭包并将其传递给线程池中的线程执行
        });
    }
    println!("Shutting down.");
}

/* 打印Http请求 */
fn handle_connection(mut stream: TcpStream) {
    let buf_reader = BufReader::new(&stream);							// 读取TCP的流
    let request_line = buf_reader.lines().next().unwrap().unwrap();		// 调用 next 从迭代器中获取第一项
    
    // 如果运行代码并请求 127.0.0.1:7878，就会得到 hello.html 中的 HTML
    // 如果进行任何其他请求，如 127.0.0.1:7878/something-else 几乎会得到 404.html 中的 HTML
    let (status_line, filename) = if request_line == "GET / HTTP/1.1" {
        ("HTTP/1.1 200 OK", "hello.html")
    } else {
        ("HTTP/1.1 404 NOT FOUND", "404.html")
    };
    
    let contents = fs::read_to_string(filename).unwrap();		// 读取html文件
    let length = contents.len();

    // 将文件内容加入到将要写入流的成功响应的 body 
    let response =
        format!("{status_line}\r\nContent-Length: {length}\r\n\r\n{contents}");

    stream.write_all(response.as_bytes()).unwrap();				// 发送给连接
    
    
}
```

`src/lib.rs`

```rust
use std::{
    sync::{Arc, Mutex, mpsc},
    thread,
};

pub struct ThreadPool {         // 线程池结构体
    workers: Vec<Worker>,
    sender: Option<mpsc::Sender<Job>>,
}

impl ThreadPool {
    /// Create a new ThreadPool.
    ///
    /// The size is the number of threads in the pool.
    ///
    /// # Panics
    ///
    /// The `new` function will panic if the size is zero.
    pub fn new(size: usize) -> ThreadPool {
        assert!(size > 0);

        let (sender, receiver) = mpsc::channel();

        let receiver = Arc::new(Mutex::new(receiver));

        // 预分配一个空间
        let mut workers = Vec::with_capacity(size);

        // 创建并存储线程
        for id in 0..size {
            workers.push(Worker::new(id, Arc::clone(&receiver)));
        }

        ThreadPool {
            workers,
            sender: Some(sender),
        }
    }

    // f 是调用方法时提供的闭包
    pub fn execute<F>(&self, f: F)
    where
        F: FnOnce() + Send + 'static,
    // 针对每个请求闭包只执行一次，并且需要跨线程传递，同时确保生命周期够长
    {
        let job = Box::new(f);

        self.sender.as_ref().unwrap().send(job).unwrap();
    }
}

impl Drop for ThreadPool {
    fn drop(&mut self) {

        drop(self.sender.take());                               // 关闭发送端

        for worker in self.workers.drain(..) {					// 指定哪些项要从 Vec 中移除
            println!("Shutting down worker {}", worker.id);

            worker.thread.join().unwrap();					    // 等待工作执行完毕
        }
    }
}



type Job = Box<dyn FnOnce() + Send + 'static>;

struct Worker {
    id: usize,
    thread: thread::JoinHandle<()>,
}

impl Worker {
    fn new(id: usize, receiver: Arc<Mutex<mpsc::Receiver<Job>>>) -> Worker {
        let thread = thread::spawn(move || {
            loop {
                // 确保同一时间只有worker能获取任务
                let message = receiver.lock().unwrap().recv();

                // 判断接受是否出错，错误直接退出
                match message {
                    Ok(job) => {
                        println!("Worker {id} got a job; executing.");

                        job();
                    }
                    Err(_) => {
                        println!("Worker {id} disconnected; shutting down.");
                        break;
                    }
                }
            }
        });

        Worker { id, thread }
    }
}
```

`hello.html`

```html
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <title>Hello!</title>
  </head>
  <body>
    <h1>Hello!</h1>
    <p>Hi from Rust</p>
  </body>
</html>
```

`404.html`

```html
<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <title>Hello!</title>
  </head>
  <body>
    <h1>Oops!</h1>
    <p>Sorry, I don't know what you're asking for.</p>
  </body>
</html>
```

输出

```shell
Worker 0 got a job; executing.
Shutting down.
Shutting down worker 0
Worker 3 got a job; executing.
Worker 1 disconnected; shutting down.
Worker 2 disconnected; shutting down.
Worker 3 disconnected; shutting down.
Worker 0 disconnected; shutting down.
Shutting down worker 1
Shutting down worker 2
Shutting down worker 3
```

