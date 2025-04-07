

[toc]

# 序言

> [!important]
>
> ***不会算法的程序员大概率早被淘汰。***

# C/C++基础

## 输入/出

### String类型输入/出

```c++
int main(){
    string str1,str2;
    //第一种输入方式，遇到换行停止
    getline(cin,str1);
    //第二种输入方式，遇到空格停止
    cin >> str2;
    
    //输出方式
    printf("%s\n,str1.c_str()");
    //or
    cout << str2 << endl;
    return 0;
}
```

### cin.get()与cin

```c++
while(N--){
    int temp;
    while(cin >> temp){
        num[j] = temp;
        j++;
    }
    //遇到换行符则停止输入
    if(cin.get()=='\n'){ 
        break;
    }
}
```

> 输入：
>
> 2
>
> 5 6 8 11 9
>
> 10 12 9

> 输出：
>
> 7 9

### 常见数据类型及printf格式

#### 有符号整型

有符号整型可以用来定义正整数和负整数，主要有下面几种类型：

* int：C语言标准规定，int类型至少16位长，即**2字节**；printf中使用**%d**进行打印。

* short或short int：C语言规定，short的长度不应大于最大的int长度；printf中使用**%hd**打印。

* long或long int：长度不应小于int的最大长度，C语言要求至少有32位长，即**4字节**；printf打印使用**%ld**。

* long long或long long int：长度不应小于最大的long长度，至少为64位长，即**8字节**；printf使用**%lld**进行打印。

#### 无符号整型

无符号整型可以用来定义0和正整数，使用<font color = ff5809>***unsigned***</font>关键字修饰上面的有符号整型即可，如unsigned int代表无符号int类型。

无符号类型的长度与有符号类型的长度**相同**，但是由于无符号类型仅用来表示0和正整数，所以无符号整型可以表示**更大的正值范围**；printf打印只需要将上面有符号整型的**d**换为**u**即可，如unsigned long可以使用%lu。

#### 实浮点数

* float：基本浮点类型，至少精确表示**六位**有效数字；使用**%f**打印；
* double：更大范围的浮点数，至少精确表示**十位**有效数字；使用**%lf**打印；
* long double：更大范围的浮点数，比double有更多的有效数字位数；使用<font color = red>**%Lf**</font>打印。

## stack

栈(stack)是一种**后进先出（LIFO）**的数据结构，它**只允许在一端**（称为栈顶）进行插入和删除操作，收录在STL（C++标准库）里，使用时要先包含头文<font color = 6b8e23>***#include\<stack>***</font>。

### stack类型常用方法

1. 初始化

```c++
stack<Type> s;
```

2. push(item)——进栈

```c++
s.push(1);		//将1压入栈s的栈顶
```

3. pop()——出栈

```c++
s.pop();		//将栈s的栈顶元素出栈
```

4. top()——返回栈顶元素

```c++
s.top();
```

5. size()——返回栈中的元素个数

```c++
s.size();
```

6. empty()——检查栈是否为空，如果为空返回true，否则返回false

```c++
s.empty();
```

## queue

队列是一种**先进先出（FIFO）**的数据结构，它只允许在一端（称为队尾）进行插入操作，在另一端（称为队首）进行删除操作。收录在STL（C++标准库）里，使用时要先包含头文<font color = 6b8e23>***#include\<queue>***</font>。

### queue类型常用方法

1. 初始化

```c++
queue<Type> q;
```

2. push(item)——入队

```c++
q.push(1);		//将1压入队q的队尾
```

3. pop()——出队

```c++
q.pop();		//将队列q的首元素出队
```

4. front()——返回队首元素

```c++
q.top();
```

5. back()——返回队尾元素

```c++
q.back();
```

6. size()——返回队列中的元素个数

```c++
q.size();
```

6. empty()——检查队列是否为空，如果为空返回true，否则返回false

```c++
q.empty();
```

## vector

**动态数组**或不定长数组，有时也被译成**容器**，其实就是一种非静态数组，可以进行修改、增长。收录在STL（C++标准库）里。

### vector类型常用方法

1. 初始化

```c++
//	初始化5个值为0的元素
vector<int> v(5)
//or 直接使用花括号赋值
vector<int> v{0,0,0,0,0}
```

2. push_back(\*)——入队

v.push_back(x)，就是在容器v**后面**添加一个元素x，时间复杂度为O(1)。

```c++
int main(){
    vector<int> v;
    for(int i = 1; i<= 3;i++){
        //容器V后依次添加元素
        v.push_back(i);
    }
    return 0;
}
```

3. pop_back()——出队

pop_back()可以删除容器的**尾**元素，时间复杂度为O(1)。

```c++
int main(){
    vector<int> v;
    for(int i = 1; i<= 3;i++){
        //容器V后依次添加元素
        v.push_back(i);
    }
    v.pop_back();	//删除尾元素
    return 0;
}
```

4. size()——元素个数

size()用于获取容器中的个数，时间复杂度为O(1)，size()返回的类型为<font color = ff5809>***unsigned***</font>类型。

```c++
int main(){
    vector<int> v;
    for(int i = 1; i<= 3;i++){
        //容器V后依次添加元素
        v.push_back(i);
    }
    v.pop_back();	//删除尾元素
    cout << v.size();
    return 0;
}
```

> 输出：
>
> 2

5.clear()——清空

用来清空容器中**所有元素**，时间复杂度为O(N)，N为容器中元素个数。

```c++
int main(){
    vector<int> v;
    for(int i = 1; i<= 3;i++){
        //容器V后依次添加元素
        v.push_back(i);
    }
    v.clear();	//清空容器
    cout << v.size();
    return 0;
}
```

>  输出：
>
> 0

6. insert(\*，\*)——插入

用来向容器任意位置**前**插入一个元素，时间复杂度为O(N)。

```c++
int main(){
    vector<int> v;
    for(int i = 1; i<= 3;i++){
        //容器V后依次添加元素
        v.push_back(i);
    }
    v.insert(v.begin()+1,-1);	//将-1插入v[1]位置前
    //此时v中元素为：1 -1 2 3
    return 0;
}
```

7. erase(\*)/erase（\*，\*）——删除单个/范围元素

> erase(it)，即删除it处元素。
>
> erase(first,last)，即删除[first,last)内所有元素。

## pair

将两个数据组合成一组数据。

### pair类型常用方法

1. 初始化

```c++
pair<T1, T2> p1;
pair<T1, T2> p1(v1, v2);
typedef pair<int, int> PII;
```

2.  访问

可以通过p.first or p.x和p.second or y分别访问第一个和第二个元素。

```c++
cin >> a[i].x >> a[i].y;
```

3.  比较大小

先比较**第一个元素**，而后再比较第二个元素。

4. 排序

pair默认是先对**第一个**关键字从小到大排序，如果第一关键字相同，再对第二关键字从小到大排序。

# C/C++常用函数

## memset

memset是一个**初始化函数**，作用是将某一块内存中的全部设置为指定的值。

```c
void *memset(void *s, int value, size_t n);
```

> * s指向要填充的内存块。
> * value是要被设置的值。
> * n是要被设置该值的字符数。
> * 返回类型是一个指向存储区s的指针。

用法：

1. 初始化数组。
2. 清空结构体类型的变量

注意：

1. **不能任意赋值**。
2. 注意赋值类型。

[memset用法详解](https://blog.csdn.net/weixin_44162361/article/details/115790452?ops_request_misc=%7B%22request%5Fid%22%3A%22170221025216800225536878%22%2C%22scm%22%3A%2220140713.130102334..%22%7D&request_id=170221025216800225536878&biz_id=0&utm_medium=distribute.pc_search_result.none-task-blog-2~all~top_positive~default-1-115790452-null-null.142%5ev96%5epc_search_result_base8&utm_term=memset%E7%94%A8%E6%B3%95&spm=1018.2226.3001.4187)

## 进制转换

### strtol

将任意2-36进制数（char *）转化为10进制数（int）。

```c
long int strtol(const char *nptr, char **endptr, int base)
```

> * nptr是要转换的字符
> * 非法字符赋值给endptr
> * base是转换之前的进制

例如：

```c++
int main(){
    char buffer[20] = "102354486as4d";
    char *stop;
    //将十六进制的buffer转换为十进制
    int ans = strtol(buffer, &stop, 16);
    return 0;
}
```

注意：

若返回类型为unsigned long long 类型对应函数为 <font color = ff5809>***strtoull***</font>

### _itoa/itoa

将十进制数转换为任意的n进制数。

```C++
char *itoa(int value, char *str, int radix);
```

> * value是int 类型的待转换的十进制数。
> * str是转换后的结果。
> * radix是转换后的目标进制。

例如：

```C++
int main(){
    int num = 10;
    char str[100];
    itoa(num, str, 2);	//c++中一般用_itoa
    //此时str为二进制表示的十进制10
    return 0;
}
```

例题：

> 奇怪的捐赠
>
> 题目描述
>
> 本题为填空题，只需要算出结果后，在代码中使用输出语句将所填结果输出即可。
>
> 地产大亨 Q 先生临终的遗愿是：拿出 100 万元给 X 社区的居民抽奖，以稍慰藉心中愧疚。
>
> 麻烦的是，他有个很奇怪的要求：
>
> 1. 100 万元必须被正好分成若干份（不能剩余）。每份必须是 7 的若干次方元。比如：1 元, 7 元， 49 元，343 元，...
> 2. 相同金额的份数不能超过 5 份。
> 3. 在满足上述要求的情况下，分成的份数越多越好！
>
> 请你帮忙计算一下，最多可以分为多少份？
>
> 运行限制
>
> - 最大运行时间：1s
> - 最大运行内存: 128M
>
> ```C++
> # include<bits/stdc++.h>
> using namespace std;
> typedef unsigned long long ull;
> 
> int main(){
>     char str[20];
>     int num = null(1e6);
>     itoa(num, str, 7);	//转换为7进制
>     puts(str);
>     //11333311
>     //因为相同份额为不超过五份，各位均小于等于5，所以答案就是各位数之和
>     cout << 16;
>     return 0;
> }
> ```

## 格式转换

### int -> char *

```C++
int i = 1;
string str_temp = to_string(i);
char * str = const_cast<char*>(str_temp.c_str());
//str[] = [最高位 -> 最低位]
```

## max_element/min_element

获取最大值和最小值的地址

```C++
int main(){
    int a[] = {3, 6, 8, 2};
    int len = sizeof(a)/sizeof(int);
    cout << max_element(a, a+len) << endl;	//输出最大值的地址值
    cout << *min_element(a, a+len) << endl;	//输出最小值 即2
    return 0;
}
```

## sort

```C++
sort(起始地址, 末尾地址+1, cmp);
```

> 默认为升序，要实现降序有两种方法：
>
> 1. 需传入第三个参数——比较函数，即greater\<type>()。
> 2. 也可以使用自定义比较函数，如：
>
> ```C++
> bool cmp(int num1, int num2){
>     return num1 > num2;	//可以简单理解为 > 降序排列; < 升序排列
> }
> ```

## 字符串

### 字符数组——字符串

#### strrev

反转字符串。

```C++
char s[] = "hello";
strrev(s);
```

### string

#### substr

字符截取函数。

```C++
string A,B;
getline(ciin,A);
getline(ciin,B);
if(A[0] == '-')A = A.substr(1);
if(B[0] == '-')A = A.substr(1,3);
cout << A << B;
```

> 输入：
>
> -100
>
> -123456

> 输出：
>
> 100
>
> 123

注意：

substr()函数只string有效，对**字符串数组类型是无效的**。

#### reverse

反转字符串。

```C++
string s = "hello";
reverse(s.begin(),s.end());
```

#### length

字符串个数。

```C++
string s = "hello";
unsigned long long length = s.length(); //返回类型为unsigned long long
    
```

## max/min

计算多个数的最大值、最小值。

```c++
// 输出三者结果最大值
cout << max(c,max(a,b));
```

## accumulate

计算数组元素之和（vctor或int型）。

```C++
int sum = accumulate(vec.begin(),vec.end(),init);
```

> * 前两个表示累加元素范围，第三个形参为累加初始值。
> * 返回类型为第三个形参的类型

# 算法

## 枚举

描述：**一一列举，逐一检验**，又称穷举法或者暴力法。

三要素：枚举对象、枚举范围与判断条件。

## 排序

### 冒泡排序

思想：每次循环让最大（小）的”冒“上去。

```C++
void maopao(int *s, int n){
    while(n){
        bool flag = false;
        for(int i = 1; i < n; i++){
            if(s[i-1] > s[i]){
                //交换
                int temp = s[i-1];
                s[i-1] = s[i];
                flag = true;
            }
        }
        //判断是否需要提前终止
        if(!flag){
            break;
        }
        n--;
    }
}
```

## 搜索

方位：

![png](.\img\fangwei-1.png)

![png](.\img\sousuo-1.png)

### DFS

思想：**一条道走到黑，然后掉头**。

适用场景：

1. 暴力搜索结果或者进行减枝优化。

2. 排列组合，结合***递归***。

3. 正常情况下需要回溯。

例题：

> 1. 全球变暖
>
> 题目描述
>
> 你有一张某海域 NxN 像素的照片，"."表示海洋、"#"表示陆地，如下所示：
>
> .......
>
> .##....
>
> .##....
>
> ....##.
>
> ..####.
>
> ...###.
>
> .......
>
> 其中"上下左右"四个方向上连在一起的一片陆地组成一座岛屿。例如上图就有 2 座岛屿。
>
> 由于全球变暖导致了海面上升，科学家预测未来几十年，岛屿边缘一个像素的范围会被海水淹没。具体来说如果一块陆地像素与海洋相邻(上下左右四个相邻像素中有海洋)，它就会被淹没。
>
> 例如上图中的海域未来会变成如下样子：
>
> .......
>
> .......
>
> .......
>
> .......
>
> ....#..
>
> .......
>
> .......
>
> 请你计算：依照科学家的预测，照片中有多少岛屿会被完全淹没。
>
> 输入描述
>
> 第一行包含一个整数 N (1≤N≤1000)。
>
> 以下 N 行 N 列代表一张海域照片。
>
> 照片保证第 1 行、第 1 列、第 N 行、第 N 列的像素都是海洋。
>
> 输出一个整数表示答案。
>
> ```C++
> #include<bits/stdc++.h>
> using namespace std;
> 
> int N;           //海域像素大小
> int res = 0;
> int flag = 0;   //判断岛屿是否被整个淹没
> char sea[1005][1005];
> 
> //深度优先搜索算法
> void DFS(int x, int y){
>     //当像素超出范围或为海洋时退出当前搜索
>     if(x < 0 || x >= N || y <0 || y>=N || sea[x][y]=='.'){
>         return;
>     }
>     if(sea[x][y]=='#'){
>         //如果搜索到陆地表示该陆地未被完全淹没，此时判断标志为1
>         flag = 1;
>     }
>     sea[x][y]='.'; //表示已经搜索过
>     DFS(x+1,y);
>     DFS(x-1,y);
>     DFS(x,y+1);
>     DFS(x,y-1);
> }
> 
> //预测后的岛屿
> void preSea(){
>     for(int i=0; i<N; i++){
>         for(int j=0; j<N; j++){
>             if(sea[i][j]=='#'){
>                 //如果临近海洋则该陆地将会变为海洋用 , 暂时表示
>                 if(sea[i-1][j]=='.'|| sea[i+1][j]=='.'|| sea[i][j-1]=='.'|| sea[i][j+1]=='.'){
>                     sea[i][j]=',';
>                 }
>             }
>         }
>     }
> }
> 
> 
> int main(){
>     ios::sync_with_stdio(false);
>     cin.tie(nullptr);
>     cout.tie(nullptr);
> 
>     cin >> N;
>     for(int i=0; i<N; i++){
>         for(int j=0; j<N; j++){
>             cin >> sea[i][j];
>         }
>     }
> 
>     //预测后的海域像素图
>     preSea();
>     //利用DFS计算出未来预测被完全淹没的岛屿个数
>     for(int i=0; i<N; i++){
>         for(int j=0; j<N; j++){
>             if(sea[i][j]==','){
>               flag = 0;
>                 //进行搜索
>                 DFS(i,j);
>                 if(flag!=1){
>                     //表示该陆地被完全淹没
>                     res++;
>                 }
>             }
>         }
>     }
>     cout << res;
>     return 0;
> }
> ```

> 2. 分糖果
>
> 问题描述
>
> 两种糖果分别有 9 个和 16 个，要全部分给 7 个小朋友，每个小朋友得到的糖果总数最少为 2 个最多为 5 个，问有多少种不同的分法。糖果必须全部分完。
>
> 只要有其中一个小朋友在两种方案中分到的糖果不完全相同，这两种方案就算作不同的方案。
>
> 答案提交
>
> 这是一道结果填空的题，你只需要算出结果后提交即可。本题的结果为一个整数，在提交答案时只填写这个整数，填写多余的内容将无法得分。
>
> ```C++
> #include<bits/stdc++.h>
> using namespace std;
> 
> long res = 0;
> // 利用DFS搜索所有的排列组合数
> /* 描述：第people+1个小朋友分配r1、r2两种糖果所有结果
>  * 参数说明：people 当前分配的第people+1个小朋友，r1、r2当前两种糖果数
>  * */
> void seek(int people,int r1,int r2){
>     if(people >= 7 ){            //为7个小朋友分配完
>         if(r1==0 && r2==0){      //正好分完
>             res++;
>         }
>         return ;
>     }
>     for(int i = 0; i <= r1; i++){      //枚举所有排列组合情况，可以不选其中一种糖果
>         for(int j = 0; j <= r2; j++){
>             if(i+j>=2 && i+j<=5){    //为每个小朋友分配糖果不少于2且不大于5
>                 seek(people+1,r1-i,r2-j);
>             }
>         }
>     }
> }
> 
> int main(){
>     ios::sync_with_stdio(false);
>     cin.tie(nullptr);
>     cout.tie(nullptr);
>     seek(0,9,16);
>     cout << res;
>     return 0;
> }
> ```

### BFS

思想：**以点散面**。

适用场景：

1. 最短/最少步数等最优解。

2. 最短路径/最小生成树。

3. 结合***队列***（入队列->队列不为空下出队列->队列不为空下且条件判断入队列）

例题：

> 大胖子走迷宫
>
> 题目描述
>
> 小明是个大胖子，或者说是个大大胖子，如果说正常人占用 1×1 的面积，小明要占用 5×5 的面积。
>
> 由于小明太胖了，所以他行动起来很不方便。当玩一些游戏时，小明相比小伙伴就吃亏很多。
>
> 小明的朋友们制定了一个计划，帮助小明减肥。计划的主要内容是带小明玩一些游戏，让小明在游戏中运动消耗脂肪。走迷宫是计划中的重要环节。
>
> 朋友们设计了一个迷宫，迷宫可以看成是一个由 n×n 个方阵组成的方阵，正常人每次占用方阵中 1×1 的区域，而小明要占用 5×5 的区域。小明的位置定义为小明最正中的一个方格。迷宫四周都有障碍物。
>
> 为了方便小明，朋友们把迷宫的起点设置在了第 33 行第 33 列，终点设置在 了第 n−2 行第 n−2 列。
>
> 小明在时刻 0 出发，每单位时间可以向当前位置的上、下、左、右移动单 位 1 的距离，也可以停留在原地不动。小明走迷宫走得很辛苦，如果他在迷宫里面待的时间很长，则由于消耗了很多脂肪，他会在时刻 k 变成一个胖子，只占用 3×3 的区域。如果待的时间更长，他会在时刻 2k 变成一个正常人，只占用 1×1 的区域。注意，当小明变瘦时迷宫的起点和终点不变。
>
> 请问，小明最少多长时间能走到迷宫的终点。注意，小明走到终点时可能瘦了也可能没有变瘦。
>
> 输入描述
>
> 输入的第一行包含两个整数 n,k （1≤n≤300，1≤k≤1000）。
>
> 接下来 n 行，每行一个由 n 个字符组成的字符串，字符为 + 表示为空地， 字符为 * 表示为阻碍物。
>
> 输出描述
>
> 输出一个整数，表示答案。
>
> ```C++
> #include<bits/stdc++.h>
> #include<queue>
> using namespace std;
> 
> char myMap[305][305];          //存储地图
> int weight;                     //体重
> int n,k;                        //行数、变瘦时间
> int visited[305][305];          //访问标志
> int xMove[4]={0,0,-1,1};   //移动方向：上下左右
> int yMove[4]={1,-1,0,0};
> 
> //BFS数据结构
> struct node{
>     int x;
>     int y;
>     int time; //单位时间
>     node(int x=0,int y=0,int time=0):x(x),y(y),time(time){}//利用列表＋构造函数初始化节点
> };
> /*= 判断是否出界 =*/
> bool outside(int x, int y){
>     if(x-weight/2<0||x+weight/2>=n||y-weight/2<0||y+weight/2>=n)return true;//表明BFS出界
>     return false;
> }
> /*= 判断是否能通过 =*/
> bool check(int x,int y){
>     for(int i=x-weight/2; i<=x+weight/2; i++){
>         for(int j=y-weight/2; j<=y+weight/2; j++){
>             if(myMap[i][j]=='*') return false;
>         }
>     }
>     return true;
> }
> 
> void BFS(){
>     queue<node> q;
>     node u(2,2,0);      		  //初始化小明位置
>     visited[u.x][u.y]= 1;         //表示访问过
>     q.push(u);                    //入队
>     while(!q.empty()){
>         node v = q.front();
>         q.pop();                    //出队
>         bool freeze = true;         //无法移动判断位
>         if(v.x==n-3 && v.y==n-3){   //走出迷宫
>             cout << v.time;
>             return ;
>         }
>         if(v.time<k)weight = 5;                     //大胖子
>         else if(v.time>=k && v.time<2*k)weight = 3; //胖子
>         else weight = 1;                            //正常人
> 
>         //四周进行搜索
>         for(int i=0; i<4; i++){
>             u = node(v.x+xMove[i],v.y+yMove[i],v.time+1);
>             if(!outside(u.x,u.y) && check(u.x,u.y) && visited[u.x][u.y]!=1){
>                 visited[u.x][u.y] = 1;
>                 q.push(u);
>                 freeze = false;
>             }
>         }
>         if(freeze)q.push(node(v.x,v.y,v.time+1));//小明暂时无法移动
>     }
> }
> 
> 
> int main(){
>     ios::sync_with_stdio(false);
>     cin.tie(nullptr);
>     cout.tie(nullptr);
> 
>     cin >> n;
>     cin >> k;
>     for(int i=0; i<n; i++){
>         cin >> myMap[i];
>     }
>     BFS();
>     return 0;
> }
> ```

## 贪心

思想：由每个阶段的局部最优推出全局最优。

关键：找出局部最优、全局最优，并将**两者联系起来**。

例题：

> 题目描述
>
> 几个人一起出去吃饭是常有的事。但在结帐的时候，常常会出现一些争执。
>
> 现在有 n 个人出去吃饭，他们总共消费了 S 元。其中第 i 个人带了 $a_i$元。幸运的是，所有人带的钱的总数是足够付账的，但现在问题来了：每个人分别要出多少钱呢？
>
> 为了公平起见，我们希望在总付钱量恰好为 S 的前提下，最后每个人付的钱的标准差最小。这里我们约定，每个人支付的钱数可以是任意非负实数，即可以不是 1 分钱的整数倍。你需要输出最小的标准差是多少。
>
> 标准差的介绍：标准差是多个数与它们平均数差值的平方平均数，一般用于刻画这些数之间的"偏差有多大"。形式化地说，设第 i 个人付的钱为 $b_i$ 元，那么标准差为 :
> $$
> S=\sqrt{\frac{1}{n}\sum_{i=1}^{n}(b_i-\frac{1}{n}\sum_{i=1}^{n}b_i)^2}
> $$
> 输入描述
>
> 第一行包含两个整数 n、S；
>
> 第二行包含 n 个非负整数 $a_1, ⋯, a_n$。
>
> 其中，$n≤5×10^5,0≤a_i≤10^9 $。
>
> 输出描述
>
> 输出最小的标准差，四舍五入保留 4 位小数。
>
> 保证正确答案在加上或减去 $10^−9 $后不会导致四舍五入的结果发生变化。
>
> ```c++
> #include<bits/stdc++.h>
> using namespace std;
> 
> int main(){
>     ios::sync_with_stdio(false);
>     cin.tie(nullptr);
>     cout.tie(nullptr);
> 
>     int n;                  //人数
>     long double S;          //总价钱
>     cin >> n;
>     cin >> S;
>     long double money[n];           // 存储每个人的钱
>     long double aboveSum = 0;      // 超出部分的总钱
>     long double above = 0;          // 超过平均值的人数
>     long double res = 0;
>     for(int i=0; i<n; i++){         //与平均值作比较
>         cin >> money[i];
>         money[i] -= (S/n);
>     }
>     sort(money,money+n);//排序
>     //局部最优：越靠近平均值越好
>     for(int i=0; i<n; i++){  //小于平均值直接平方求和
>         if(money[i]<0){
>             res += pow(money[i],2);
>             aboveSum +=  - money[i];       //超出部分需要补的钱数
>         }
>         else if(money[i]>0){
>             above = n-i;                   	 	//计算出超出人数
>             if(money[i]<(aboveSum/above)){		//若比超出部分平均值小
>                 res += pow(money[i],2);
>                 aboveSum -= money[i];      	//更新超出须补钱数
>             }
>             else{
>                 if(above == 0){             //特殊情况 above作除数不能为0
>                     printf("0.0000");
>                     return 0;
>                 }
>                 res += above * pow((aboveSum/above),2);//超出部分平方求和
>                 break;
>             }
>         }
>     }
>     printf("%0.4Lf",sqrt(res/n));      //计算标准差
>     return 0;
> }
> ```

## 模拟

思想：“照着葫芦画瓢”，根据题目表述进行筛选提取关键要素（或**一条判别准则**），按需求书写代码解决实际问题。

例题：

> 计算两个矩形面积
>
> ![png](.\img\juxing-1.png)
>
> ```c++
> cin >> x1 >> y1 >> x2 >> y2 >> x3 >> y3 >> x4 >> y4;
> a1 = max(x1,x3);			//用于查看x轴是否相交
> a2 = min(x2,x4);
> b1 = max(y1,y3);			//用于查看y轴是否相交
> b2 = min(y2,y4);
> //两矩形面积 = 两矩形各自面积 - 重合区域面积
> if(a2 > a1 && b2 > b1){		//存在重合区域
>     s = (x2  -x1) * (y2 - y1) + (x4 -x3) * (y4 - y3) - (a2 - a1) *(b2 -b1);    
> }
> else{
>     s = (x2  -x1) * (y2 - y1) + (x4 -x3) * (y4 - y3);
> }
> cout << s;
> ```

## 二分

思想：将问题规模不断缩小为原来的**二分之一**，从而快速找到问题的解。

关键：根据区间规则（左闭右闭、左闭右开）编写代码。

1. 左闭右开型

```c++
//前提：有序
left = 0;
right = numSize;
while(left < right){			//左闭右开
    middle = (left + right) / 2;
    if(num[middle] > target){	//更新右边界
        right = middle;
    }
    else if(num[middle] < target){	//更新左边界
        left = middle + 1;
    }
    else return middle;
}
return -1;						//未找到
```

2. 左闭右闭型

```c++
//前提：有序
left = 0;
right = numSize - 1;
while(left <= right){			//左闭右闭
    middle = (left + right) / 2;
    if(num[middle] > target){	//更新右边界
        right = middle - 1;
    }
    else if(num[middle] < target){	//更新左边界
        left = middle + 1;
    }
    else return middle;
}
return -1;						//未找到
```

## 动态规划DP

思想：这种算法通过拆分问题，定义问题状态和状态之间的关系，使得问题能够以递推（或者说分治）的方式去解决。基本思想是将待求解的问题分解为若干个子问题（阶段），按顺序求解子阶段。

题型：基础题、背包问题、打家劫舍、股票问题、子序列问题。

DP五部曲：

1. DP数组（一维、二维）以及下标的含义，即数组的定义

2. DP数组如何初始化

3. 递推公式

4. 遍历顺序

5. 打印DP数组（用于检验）

例题：

> 1. 01背包问题
>
>    题目描述
>
>    辰辰是个天资聪颖的孩子，他的梦想是成为世界上最伟大的医师。为此，他想拜附近最有威望的医师为师。医师为了判断他的资质，给他出了一个难题。医师把他带到一个到处都是草药的山洞里对他说：“孩子，这个山洞里有一些不同的草药，采每一株都需要一些时间，每一株也有它自身的价值。我会给你一段时间，在这段时间里，你可以采到一些草药。如果你是一个聪明的孩子，你应该可以让采到的草药的总价值最大。”
>
>    如果你是辰辰，你能完成这个任务吗？
>
>    输入描述
>
>    第一行有两个整数 T（1≤T≤1000 和 M（1≤M≤100，用一个空格隔开，T 代表总共能够用来采药的时间，M代表山洞里的草药的数目。
>
>    接下来的 M 行每行包括两个在 1 到 100 之间（包括 1 和 100 ）的整数，分别表示采摘某株草药的时间和这株草药的价值。
>
>    输出描述
>
>    输出一个整数，表示在规定的时间内，可以采到的草药的最大总价值。
>
>
>
> ```C++
> //二维解法：（矩阵中）当前值来自于左上角或者正上方的值。
> 	int T;			//总采药时间
> 	int M;			//总草药数
>    	int times[101]; //草药所需时间
>    	int values[101];//草药价值
>    	int dp[101][1001];
>    /*定义：dp[i][j]表明在时间j内，采集i个药草的最大价值
>    * 初始化：0行、0列均初始化为0
>      **/
>      memset(dp,0,sizeof(dp));
>      cin >> T;
>      cin >> M;
>      for(int i = 1 ; i<= M; i++){
>          cin >> times[i];
>          cin >> values[i];
>      }
> 
>      for(int i = 1; i <= M; i++){  			//可先循环药草后循环时间，反之亦可
>          for(int j = 1; j <=T ; j++){		//注意遍历顺序
>              if(j < times[i]){				//不采时的递归公式		
>                  dp[i][j] = dp[i-1][j];
>              }
>              else{       					//采时的递归公式
>                  dp[i][j] = max(dp[i-1][j-times[i]] + values[i],dp[i-1][j]);
>              }
>          }
>      }
>      cout << dp[M][T];
> ```
>    ```c++
> //一维解法：滚动数组，将矩阵进行压缩。
>        int T;              //总采药时间
>        int M;              //总草药数
>        int times[101];    //草药所需时间
>        int values[101];   //草药价值
>        int dp[1001];
>        /*定义: dp[j]表明在时间j内所能采药的最大价值
>         * 初始化为0
>         * */
>        memset(dp,0,sizeof(dp));
>        cin >> T;
>        cin >> M;
>        for(int i = 0 ; i< M; i++){
>            cin >> times[i];
>            cin >> values[i];
>        }
>        for(int i = 0; i < M; i++){                              //必须先循环草药再循环时间
>            for(int j = T; j >= times[i]; j--){                  //注意遍历顺序，循环时间必须倒序（矩阵中当前值的更新仅依赖于左上角或上角）
>                dp[j] = max(dp[j-times[i]] + values[i],dp[j]);   //递推公式
>            }
>        }
>        cout << dp[T];
>    }
>    ```
>
> 2. 最长递增（减）子序列问题
>
> 问题描述
>
> 小椒是个摄影爱好者。恰逢班级合照，他受邀帮忙拍照（**站成一排**）。这本是一件简单的事，但由于啾啾是个完美主义者，他希望他拍的照片必须符合美学，即存在一个身高较大值，使得较大值无论是往左还是往右身高都是递减的（数学表示应为：a[1]≤...≤a[i]≥a[i+1]≥...≥a[n]）。同学们已经站好了，但站位不符合美学，你需要找出尽可能少的同学出队进行重新排列。请问最少需要出队多少个同学？
>
> 输入格式
>
> 第一行输入 n，表示有 n个同学。
>
> 接下来的 n行输入校友身高，其中第 i 行输入 a[i]，表示编号为 i 的校友的身高（单位：毫米）。
>
> (1≤n≤100,1500≤a[i]≤1900)。
>
> 输出描述
>
> 输出一个整数，表示最少需要出队多少个同学。
>
> ```c++
>     int n;
>     int stu[101];
>     int dpRise[101];  //（左）最长递增子序列
>     /* 定义：dp_rise[i] 从0——i,以stu[i]为结尾的最长递增子序列的长度
>      * 初始化：均初始化为1
>      * */
>     int dpDown[101];  //（右）最长递减子序列
>     /* 定义：dp_down[i] 从i——n-1,以stu[i]为结尾的最长递减子序列的长度
>     * 初始化：均初始化为1
>     * */
>     int sumMax = 0;
>     cin >> n;
>     for(int i = 0 ; i < n; i++){
>         cin >> stu[i];
>         //dp数组初始化
>         dpRise[i] = 1;
>         dpDown[i] = 1;
>     }
> 
>     //计算最长递增子序列长度
>     for(int i = 1; i < n; i++){     //注意从stu[1]开始 stu[0]确定为1
>         for(int j = 0 ; j < i ;j++){//stu[i]之前的元素
>             if(stu[i] >= stu[j]){   //确认更新dp_rise[i]
>                 dpRise[i] = max(dpRise[i], dpRise[j]+1); //递推公式
>             }
>         }
>     }
>     //计算最长递减子序列长度
>     for(int i = n-2; i >= 0; i--){      //注意从stu[n-2]开始 stu[n-1]确定为1
>         for(int j = n-1 ; j > i ;j--){  //stu[i]之后的元素
>             if(stu[i] >= stu[j]){       //确认更新dp_down[i]
>                 dpDown[i] = max(dpDown[i], dpDown[j]+1); //递推公式
>             }
>         }
>     }
> 
>     for(int i=0; i<n; i++){                             //注意需找到递增子序列和递减子序列之和最大的值
>         sumMax = max(dpRise[i]+dpDown[i]-1,sumMax); //减去最高点自身
>     }
>     cout << n - sumMax;        //输出最小须重新调整的人数
> ```
>
> 3. 蜗牛问题
>
> 问题描述
>
> 这天，一只蜗牛来到了二维坐标系的原点。
>
> 在 x轴上长有 n 根竹竿。它们平行于 y轴，底部纵坐标为 0，横坐标分别为 x1,x2,...,xn 。竹竿的高度均为无限高，宽度可忽略。蜗牛想要从原点走到第 n个竹竿的底部也就是坐标 (xn,0)。它只能在 x 轴上或者竹竿上爬行，在 x轴上爬行速度为 1单位每秒；由于受到引力影响，蜗牛在竹竿上向上和向下爬行的速度分别为 0.7 单位每秒和 1.3单位每秒。
>
> 为了快速到达目的地，它施展了魔法，在第 i和 i+1根竹竿之间建立了传送门（0<i<n），如果蜗牛位于第 i根竹竿的高度为 ai 的位置 (xi,ai)，就可以瞬间到达第 i+1 根竹竿的高度为 bi+1 的位置 (xi+1,bi+1)，请计算蜗牛最少需要多少秒才能到达目的地。
>
> 输入格式
>
> 输入共 1+n 行，第一行为一个正整数 n；
>
> 第二行为 n个正整数 x1,x2,...,xn ；
>
> 后面 n−1 行，每行两个正整数 ai,bi+1。
>
> 输出格式
>
> 输出共一行，一个浮点数表示答案（四舍五入保留两位小数）。
>
> ```c++
>     int n;
>     cin >> n;
>     long double bamboo[n];      //竹竿位置
>     long double dp[n][2];       //ddp[i][0]表示到节点i+1用时最短时间，dp[i][1]表示到第i+1个传送门所需最短时间
>     long double a[n],b[n];      //分别表示第i+1个传送门初始位置、传送门终点位置
>     for(int i = 0; i < n; i++){
>         cin >> bamboo[i];
>     }
>     for(int i = 0; i < n-1; i++){ //初始化传送门
>         cin >> a[i];
>         cin >> b[i];
>     }
>     //dp数组初始化
>     dp[0][0] = bamboo[0]/1.0;
>     dp[0][1] = dp[0][0] + a[0] / 0.7;
> 
>     for(int i =1; i < n - 1; i++){
>         if(b[i-1] < a[i]){          //向上爬
>             dp[i][1] = min(dp[i-1][1] + (a[i] - b[i-1]) / 0.7,dp[i-1][0]+(bamboo[i]-bamboo[i-1])/1.0+a[i]/0.7);
>         }else{                     //向下爬
>             dp[i][1] = min(dp[i-1][1] + (b[i-1]-a[i]) / 1.3,dp[i-1][0]+(bamboo[i]-bamboo[i-1])/1.0+a[i]/0.7);
>         }                           //更新节点最短时间
>         dp[i][0] = min(dp[i-1][0]+(bamboo[i]-bamboo[i-1])/1.0,dp[i-1][1]+b[i-1]/1.3);
>     }
>     //计算终点最短时间
>     dp[n-1][0] = min(dp[n-2][0]+(bamboo[n-1]-bamboo[n-2])/1.0,dp[n-2][1]+b[n-2]/1.3);
>     printf("%0.2Lf",dp[n-1][0]);
> ```

## 高精度

思想：计算时表达式的操作数范围超出了标准数据类型可以表示的数据范围，此时将其**转为字符串**进行操作，然后将字符串转为数组按照人工计算方式类比即可。

> 问题描述
>
> 输入两个整数 A 和 B，输出 $A^2−B^2$ 的值。
>
> 输入格式
>
> 第一行输入一个整数，表示 A。
>
> 第二行输入一个整数，表示 B。
>
> 输出格式
>
> 输出仅一行，包含一个整数，表示答案。
>
> 评测数据规模
>
> 对于所有评测数据，$−10^{100}≤A,B≤10^{100} $。
>
> ```c++
> int main() {
>     string A;
>     string B;
>     int a[210] = {0};   //对A数组化
>     int b[210] = {0};   //对B数组化
>     int res[210] = {0}; //保存最终结果
>     int res_a[105][210] = {0}; //用于计算A*A
>     int res_b[105][210] = {0}; //用于计算B*B
>     getline(cin, A);
>     getline(cin, B);
> 
>     /*===常见输入处理===*/
>     //首先去负号
>     if (A[0] == '-')A = A.substr(1);
>     if (B[0] == '-')B = B.substr(1);
>     for (int i = 0; i < A.length(); i++) {      //将字符转为数字
>         a[i] = A[A.length() - i - 1] - '0';
>     }
>     for (int i = 0; i < B.length(); i++) {
>         b[i] = B[B.length() - i - 1] - '0';
>     }
>     
>      /*===常见乘法处理===*/
>     //计算A*A
>     int carry = 0;  //进位
>     for (int i = 0; i < A.length(); i++) {
>         for (int j = 0; j < A.length(); j++) {
>             res_a[i][j + i] += a[j] * a[i];
>             carry = res_a[i][j + i] / 10;
>             res_a[i][j + i] %= 10;
>             res_a[i][j + i + 1] += carry;
>         }
>     }
>     int aWei = 0;              //进行乘法后A的位数
>     if (res_a[A.length() - 1][2 * A.length() - 1] != 0) {//获取运算后的位数
>         aWei = 2 * int(A.length());
>     } else aWei = 2 * int(A.length()) - 1;
>     carry = 0;
>     for (int i = 0; i < aWei; i++) {           //进行求和操作得出最终结果
>         a[i] = 0;
>         a[i] += carry;
>         for (int j = 0; j < A.length(); j++) {
>             a[i] += res_a[j][i];
>         }
>         carry = a[i] / 10;
>         a[i] %= 10;
>     }
>     if (carry != 0) {   //最高位是否需要进位
>         aWei += 1;
>         a[aWei] = carry;
>     }
>     //同理计算B*B
>     carry = 0;
>     for (int i = 0; i < B.length(); i++) {
>         for (int j = 0; j < B.length(); j++) {
>             res_b[i][j + i] += b[j] * b[i];
>             carry = res_b[i][j + i] / 10;
>             res_b[i][j + i] %= 10;
>             res_b[i][j + i + 1] += carry;
>         }
>     }
>     int bWei = 0;              //进行乘法后的位数
>     if (res_b[B.length() - 1][2 * B.length() - 1] != 0) {
>         bWei = 2 * int(B.length());
>     } else bWei = 2 * int(B.length()) - 1;
>     carry = 0;
>     for (int i = 0; i < bWei; i++) {
>         b[i] = 0;
>         b[i] += carry;
>         for (int j = 0; j < B.length(); j++) {
>             b[i] += res_b[j][i];
>         }
>         carry = b[i] / 10;
>         b[i] %= 10;
>     }
>     if (carry != 0) {
>         bWei += 1;
>         b[bWei] = carry;
>     }
>     
>      /*===常见减法处理===*/
>     //计算A*A-B*B               //一定是大数 - 小数
>     int resWei = 0;           //最终相减的结果位数
>     if (aWei > bWei || (aWei == bWei && a[aWei - 1] >= b[bWei - 1])) {//若前者大
>         for (int i = 0; i < min(aWei, bWei); i++) {
>             res[i] = a[i] - b[i];
>             resWei++;
>             if (res[i] < 0 && i != min(aWei, bWei) - 1) {
>                 int j = i + 1;
>                 while (true) {              //进行借位操作
>                     if (a[j] - 1 >= 0) {    //可以借位
>                         a[j] -= 1;
>                         res[i] += 10;
>                         break;
>                     }
>                     a[j] = 9;               //无法借位
>                     j++;
>                 }
>             }
>         }
>     }
>     if (aWei > bWei && res[min(aWei, bWei) - 1] < 0) {//若前者位数大于后者位数且中间位存在符号
>         int j = min(aWei, bWei);
>         while (true) {              //进行借位操作
>             if (a[j] - 1 >= 0) {    //可以借位
>                 a[j] -= 1;
>                 res[min(aWei, bWei) - 1] += 10;
>                 break;
>             }
>             a[j] = 9;
>             j++;
>         }
>         if(a[aWei-1]==0)aWei--;                      //更新较大者的位数 
>         for(int i = min(aWei, bWei); i<aWei; i++){  //将后续结果赋值给res数组
>             resWei++;
>             res[i] = a[i];
>         }
>     }
>     //同上，此时后者小前者大
>     if (aWei < bWei || (aWei == bWei && a[aWei - 1] < b[bWei - 1])) {
>         for (int i = 0; i < min(aWei, bWei); i++) {
>             res[i] = b[i] - a[i];
>             resWei++;
>             if (res[i] < 0 && i != min(aWei, bWei) - 1) {
>                 int j = i + 1;
>                 while (true) {              //进行借位操作
>                     if (b[j] - 1 >= 0) {    //可以借位
>                         b[j] -= 1;
>                         res[i] += 10;
>                         break;
>                     }
>                     b[j] = 9;
>                     j++;
>                 }
>             }
>         }
>         cout << "-";        //此时注意要加负号
>     }
>     if (aWei < bWei && res[min(aWei, bWei) - 1] < 0) {
>         int j = min(aWei, bWei);
>         while (true) {              //进行借位操作
>             if (b[j] - 1 >= 0) {    //可以借位
>                 b[j] -= 1;
>                 res[min(aWei, bWei) - 1] += 10;
>                 break;
>             }
>             b[j] = 9;
>             j++;
>         }
>         if(b[bWei-1]==0)bWei--;
>         for(int i = min(aWei, bWei); i<bWei; i++){
>             resWei++;
>             res[i] = b[i];
>         }
>     }
>     for (int i = 0; i < resWei; i++) {
>         cout << res[resWei -1 - i];
>     }
>     return 0;
> }
> ```

## 图论

### 生成树与最小生成树

一个连通图的生成树是一个极小的连通子图，其中边的权值之和最小的生成树为最小生成树。

![png](.\img\shu-1.png)

求解最小生成树的方法：

1. Kruskal算法

思想：一种贪心算法，将边从小到大排序后，按顺序若该边符合生成树（不构成环），则添加进去，直至遍历完成。

>  主要时间花在排序上，适用于稀疏图（边的数量接近于与顶点的数量呈线性关系）。

![png](.\img\kruskal-1.png)

2. Prim算法

思想：基于贪心，每次总是选出一个离生成树距离最小的点去加入生成树，最后实现最小生成树。

> 由于不断读取任意两点之间边的权值，故适应于稠密图（边的数量接近于与顶点的数量呈平方关系）。

![png](.\img\prim-1.png)
