<font size = 6>Docker</font>

[toc]

# Docker概述

## 问题

不同的机器有不同的操作系统，以及不同的库和组件，将一个应用部署到多台机器上需要进行大量的环境配置操作。

而Docker主要解决环境配置问题，使用Docker可以不修改应用程序代码，就能够将现有的应用程序部署在其它机器上。

- 传统：开发发布项目，运维实现配置
- 现在：开发打包配置上线，一套流程做完

## 介绍

- 基于go语言开发的
- 开源

<img src="./img/截屏2024-12-24 下午9.38.34.png" alt="截屏2024-12-24 下午9.38.34" style="zoom:50%;" />

Docker通过镜像机制，将项目和环境一起打包。

本质是一种虚拟化技术，对进程进行隔离，被隔离的进程独立于宿主操作系统和其它隔离的进程。

## 与虚拟机比较

### 整体比较

相对虚拟机技术而言，十分轻巧。

虚拟机也是一种虚拟化技术，它与 Docker 最大的区别在于它是通过模拟硬件，并在硬件上安装操作系统来实现。

<img src="./img/截屏2024-12-24 下午9.39.20.png" alt="截屏2024-12-24 下午9.39.20" style="zoom:50%;" />

- VM：通过多个虚拟机来进行隔离 几个G 几分钟
- Docker：通过容器来隔离 十分小巧 几个Mb 秒即启动

### 启动速度

- 虚拟机：启动虚拟机需要先启动虚拟机的操作系统，再启动应用，这个过程非常慢
- Docker：启动Docker相当于启动宿主操作系统上的一个进程

### 占用资源

- 虚拟机：虚拟机是一个完整的操作系统，需要占用大量的磁盘、内存和 CPU 资源，一台机器只能开启几十个的虚拟机
- Docker：Docker只是一个进程，只需要将应用以及相关的组件打包，在运行时占用很少的资源，一台机器可以开启成千上万个 Docker

## 优势

除了**启动速度快**以及**占用资源少**之外，Docker具有以下优势

- 更容易迁移：提供一致性的运行环境，已经打包好的应用可以在不同的机器上进行迁移，而不用担心环境变化导致无法运行
- 更容易维护：在容器化后，开发、测试环境都是高度统一的，使用分层技术和镜像，使得应用可以更容易复用重复的部分。复用程度越高，维护工作也越容易
- 更容易扩展：可以使用基础镜像进一步扩展得到新的镜像，并且官方和开源社区提供了大量的镜像，通过扩展这些镜像可以非常容易得到我们想要的镜像
- 应用更快速的交付与部署
  - 传统：一堆帮助文档，安装程序
  - Docker：打包镜像，发布测试一键运行

- 更便捷的升级和扩缩容
- 更高效的计算资源利用：Docker是内核级的虚拟化，可以在一个物理机上运行很多容器实例

## 使用场景

### 持续集成

持续集成指的是频繁地将代码集成到主干上，这样能够更快地发现错误。

Docker具有轻量级以及隔离性的特点，在将代码集成到一个Docker中不会对其它Docker产生影响。

### 提供可伸缩的云服务

根据应用的负载情况，可以很容易地增加或者减少Docker。

### 搭建微服务架构

Docker轻量级的特点使得它很适合用于部署、维护、组合微服务。

# 容器化技术

## 虚拟机技术缺点

1. 资源占用十分多
2. 冗余步骤很多（如开机等）
3. 启动很慢

## 容器化技术

容器化技术不是模拟的一个完整的操作系统，容器包含运行环境与应用，并且容器相互隔离。

## 虚拟机与容器的区别

1. 传统虚拟机是虚拟出一个硬件，运行一个完整的OS，然后在这个系统上安装和运行软件
2. 容器内的应用直接运行在宿主机上，容器是没有自己的内核的，也没有虚拟出硬件，就轻便很多
3. 每个容器间相互隔离，每个容器内都有一个属于自己的文件系统，互不影响

# Docker基本组成

<img src="./img/截屏2024-12-25 下午8.13.03.png" alt="截屏2024-12-25 下午8.13.03" style="zoom:70%;" />

## 镜像

类似于类，通过这个镜像可以创建多个容器，最终服务运行或者项目运行就是在容器中。

## 容器

通过镜像来创建，有启动、停止等命令，独立运行一个或一组应用。

## 镜像与容器小结

镜像是一种静态的结构，可以看成面向对象里面的类，而容器是镜像的一个实例。

镜像包含着容器运行时所需要的代码以及其它组件，它是一种分层结构，每一层都是只读的（read-only layers）。构建镜像时，会一层一层构建，前一层是后一层的基础。镜像的这种分层存储结构很适合镜像的复用以及定制。

构建容器时，通过在镜像的基础上添加一个可写层（writable layer），用来保存着容器运行过程中的修改。

<img src="./img/截屏2024-12-24 下午9.47.13.png" alt="截屏2024-12-24 下午9.47.13" style="zoom:40%;" />

## 仓库（repository）

仓库就是存放镜像的地方，分为公有仓库和私有仓库。

# 原理

## 框架

Docker是一个CS架构的系统，Docker的守护进程运行在主机上，通过Socket从客户端访问。

Docker Server接收到Docker Client指令，就会执行这个命令。

<img src="./img/截屏2024-12-25 下午9.19.06.png" alt="截屏2024-12-25 下午9.19.06" style="zoom:35%;" />

## Docker为什么比VM快

1. Docker有着比虚拟机更少的抽象层
2. Docker利用的是宿主机的内核，VM需要是Guset OS

<img src="./img/截屏2024-12-24 下午9.39.20.png" alt="截屏2024-12-24 下午9.39.20" style="zoom:50%;" />

所以说，新建一个容器时Docker不需要向虚拟机一样重新加载一个操作系统内核，避免引导。

而虚拟机时加载Guset OS，是分钟级别的。而Docker是利用宿主机的操作系统，省略了这个复杂的过程。

注

1. 守护进程：是一种在后台运行的特殊进程，它通常用于为其他程序或系统提供服务，而不直接与用户交互。通常在系统启动时启动，直到系统关闭
2. 抽象层：指在软件或系统的不同层次之间提供一个简化的接口，以隐藏底层的复杂性

## Docker镜像

### 镜像是什么

镜像是一种轻量级、可执行的独立软件包，用来打包软件运行环境和基于运行环境开发的软件，它包含运行某个软件所需的所有内容，包括代码、运行时、库、环境变量和配置文件。

### 如何获得镜像

- 从远程仓库下载
- 别人拷贝给你
- 自己制作一个镜像Dockerfile

## 镜像加载

### UnionFS（联合文件系统）

是一种分层、轻量级并且高性能的文件系统，支持对文件系统的修改作为一次提交来一层层叠加，同时可以将不同目录挂载到同一个虚拟文件系统下。

union文件系统是Docker镜像的基础，镜像可以通过分层来继承，基于基础镜像（没有父镜像），可以制作各种具体的应用镜像。

### 组成

- boot（boot file system）：在Docker镜像的最底层是bootfs，主要包含bootloader和kernel。bootloader主要引导加载kernel。当boot加载完成之后整个内核都在内存中了，此时内存的使用权已由bootfs转交给内核，此时系统也会卸载bootfs
- rootfs（root file system）：在bootfs之上，包含的就是典型的linux系统中的/dev、/proc、/bin、/etc等标准目录和文件。rootfs就是各种不同的操作系统的发行版，对于一个精简的OS，rootfs可以很小，只包含最基本的命令、工具和库就可以了。因为底层用的是宿主机的kernel，自己只负责提供rootfs就可以了

<img src="./img/截屏2024-12-28 下午4.17.38.png" alt="截屏2024-12-28 下午4.17.38" style="zoom:60%;" />

注

1. 不同linux发行版，bootfs基本一致，rootfs稍有差别，因此不同发行版可以公用bootfs

## 加载分层

### 为什么要分层

为了实现资源共享。

例如多个镜像都是从base镜像构建而来的，那么宿主机只需在磁盘保留一份base镜像，同时内存中也只需要加载一份base镜像，就可以为所有镜像服务了，而且镜像的每一层都可以被共享。

### 理解

所有的Docker镜像都起始于一个基础镜像层，当进行修改或新增内容时，就会在当前镜像层之上创建新的镜像层。假设一些层是相同的，就可以实现复用。

Docker镜像都是只读的，当容器启动时，一个新的可写层被加载到镜像的顶部。这一层就是容器层，容器之下的都叫镜像层。

<img src="./img/截屏2024-12-29 下午8.35.57.png" alt="截屏2024-12-29 下午8.35.57" style="zoom:25%;" />

# 安装

## 步骤

参考官方文档

1. 找到对应OS
2. 卸载旧版本Docker
3. 下载安装包
4. 设置镜像的仓库（注意将仓库设为国内的，否则默认国外仓库下载十分慢）
5. 安装Docker
6. 启动Docker
7. 使用`docker version`检查是否安装成功

## 阿里云镜像服务加速

1. 登陆阿里云找到容器镜像服务
2. 找到镜像加速
3. 选择对应os，运行配置文件

# 基本命令

Linux中Docker的默认工作路径为`/var/lib/Docker`

<img src="./img/截屏2024-12-27 下午8.45.17.png" alt="截屏2024-12-27 下午8.45.17" style="zoom:50%;" />

## 帮助命令

```shell
docker version						# 显示Docker版本信息
docker info 							# 显示Docker系统信息 包括镜像和容器的数量
docker <command> --help		# 帮助命令
docker stats							# 查看cpu状态
```

[帮助文档地址](https://docs.Docker.com/reference/cli/Docker/)

## 镜像命令

### 基本命令

```shell
docker images
docker images	-a		# 列出所有镜像
docker images	-q		# 只显示镜像的id
```

<img src="./img/截屏2024-12-26 下午8.51.45.png" alt="截屏2024-12-26 下午8.51.45" style="zoom:70%;" />

| 关键词 |解释 |
| :--: | :--: |
| REPOSITORY |镜像的仓库源 |
| TAG  |   镜像的标签 |
| IMAGE ID|  镜像的id |
| CREATED	 |镜像的创建时间   |
| SIZE	  |  镜像的大小|

### 搜索镜像

```shell
docker search <image_name>											# 搜索镜像
docker search <image_name> --filter=STARS=3000	# 搜索出stars为3000以上的
```

### 拉取镜像

```shell
docker pull	<image_name>[:tag]	# 下载指定版本为tag的镜像，默认下载最新版

#	下面二者等价
docker pull	<image_name>[:tag]
docker pull	Docker.io/libary/<image_name>[:tag]

#	指定版本
docker pull	mysql:5.7						# 版本一定要来源于官方
```

### 删除镜像

```shell
docker rmi -f <image_id>														# 删除指定的容器
docker rmi -f <image1_id> <image2_id> <image3_id>		# 删除多个容器
docker rmi -f $(docker images -aq)									# 删除全部容器	 
```

## 容器命令

有了镜像才可以创建容器。

### 创建并启动容器

```shell
docker run <image_name:[TAG]/id>														# 创建并启动容器
docker run --name <container_name> <image_name:[TAG]/id>		# 创建一个容器并命名
docker run -d <image_name:[TAG]/id>							# 后台方式运行，若无前台进程就会自动停止
docker run -it <image_name:[TAG]/id>						# 使用交互式方法运行，进入容器查看内容
docker run  -p 容器端口 <image_name:[TAG]/id>	   # 指定容器的端口	
						-P 主机端口:容器端口 
						-P ip:主机端口:容器端口
						-p 随机指定容器的端口
docker run  -e 所修改的配置												# 按指定配置运行容器

# 举例 #
docker run -it centos /bin/bash		# 当前主机下运行
ls																# 此时查看的是容器内的centos
exit															# 退出主机
```

Docker run运行流程

<img src="./img/截屏2024-12-25 下午9.14.08.png" alt="截屏2024-12-25 下午9.14.08" style="zoom:40%;" />

**端口暴露**

使用`docker run -p 主机端口:容器端口 <image_id/name>` 来实现端口暴露，使得可以通过外网来访问容器内地址。

<img src="./img/截屏2024-12-27 下午9.02.43.png" alt="截屏2024-12-27 下午9.02.43" style="zoom:60%;" />

### 查看容器

```shell
docker ps								# 查看正在运行的容器
docker ps -a						# 曾经以及当前运行过的容器
docker ps -n=<number>		# 查看number个当前运行的容器
docker ps -q						# 只显示正在运行容器的编号
```

### 退出容器

```shell
exit						# 直接容器停止并退出
Ctr + p + q			# 容器不停止退出
```

### 删除容器

```shell
docker rm <container_id>							# 删除指定容器，不能删除正在运行的容器
docker rm -f	<container_id> 					# 强制删除指定容器
docker rm -f $(docker ps -aq)					# 强制删除所有容器
docker ps -aq|xargs docker rm -f	 		# 强制删除所有容器
```

### 启动和停止容器

```shell
docker start <container_id>			# 启动容器
docker restart <container_id>		# 重启容器
docker stop <container_id>			# 停止当前正在运行的容器
docker kill <container_id>			# 强制停止当前容器
```

### 进入容器

容器通常都是使用后台方式运行的，需要进入容器，修改一些配置。

```shell
docker exec -it <container_id> /bin/bash		# 以交互式方式进入已启动的容器，开启一个新的终端
docker attach <container_id> /bin/bash			# 进入已启动容器正在运行的终端，不会启动新进程
```

### 查看容器日志

```shell
docker logs -tf <container_id>										# 显示容器日志
docker logs -tf --tail <number> <container_id>		# 显示指定条数的容器日志
```

### 查看容器中进程信息

```shell
docker top <container_id>					# 查看指定容器的进程信息
```

### 查看容器的元数据

```shell
 docker inspect <container_id>		# 查看容器的元数据
```

### 容器内文件拷贝到主机上

只要容器在，容器中的数据就在。

```shell
docker cp <container_id>:<container_file_dir> <host_dir>	# 将容器中指定文件拷贝到当前主机指定路径下
```

### 提交镜像

```shell
docker commit -a="作者" -m="提交的描述信息" <container_id> <target_image_name>:[TAG] 		# 将当前修改后的容器提交为镜像
```

# 数据卷

## 含义

一种持久化和同步操作，本质是共享目录。

## 产生原因

容器技术使用 rootfs 机制与 Namespace，构建出与宿主机隔离开的文件系统。在用户使用 Docker 容器的时候，会产生一系列的数据文件。这些数据文件在 Docker 容器关闭时就会消失，但是其中部分数据是用户希望能够保存的。

Docker 应用与运行环境打包成容器进行运行，用户希望在运行过程中产生的部分数据是可以持久化的，并且容器之间能够实现数据互通。这正是容器数据卷（Docker Volume）要解决的问题。

## 原理

在 Docker 中使用数据卷，就是在系统中挂载一个文件系统。

容器利用数据卷与宿主机进行数据共享，从而实现容器间的数据共享与交换。

在 Docker 中，一旦容器与数据卷绑定，没有直接的命令可以仅解除容器与数据卷的绑定而不删除数据卷。如果只是想从容器中移除数据卷的挂载，而保留数据卷以便其他容器使用，那么不需要删除数据卷，只需要移除容器即可。

## 命令

### 基本命令

```shell
docker volume ls												# 查看容器内所有卷的情况
docker volume inspect <volume_name>  		# 查看容器指定卷的情况
docker volume rm <volume_name>					# 删除指定数据卷，需先停止使用该卷的所有容器，此时宿主机对应文件也将删除
docker volume ls -qf dangling=true			# 删除未使用的数据卷
```

### 为容器挂载

```shell
-v 容器内路径							 # 匿名挂载
-v 卷名:容器内路径						# 具名挂载
-v /宿主机路径:容器内路径			# 指定路径挂载
```

### 举例

以数据卷方式运行容器。

```shell
docker run -it -v <host_dir>:<container_dir> <image_name:[TAG]/id>		# 将主机内路径与容器内路径进行同步映射

# 通过ro、rw改变读写权限
ro	readonly	
rw	readwrite

docker run -it -v <host_dir>:<container_dir>:ro <image_name:[TAG]/id>		# 只能从宿主机改变，不能从容器内改变
docker run -it -v <host_dir>:<container_dir>:rw <image_name:[TAG]/id>		# 默
```

使用inspect查看容器信息，出现`Mounts`即为挂载成功。

<img src="./img/截屏2024-12-28 下午8.53.15.png" alt="截屏2024-12-28 下午8.53.15" style="zoom:50%;" />

此后俩者实现同步，即使容器关闭后，两者依旧可以实现同步。

<img src="./img/截屏2024-12-28 下午8.55.49.png" alt="截屏2024-12-28 下午8.55.49" style="zoom:50%;" />

注

1. <host_dir> 是以`/`为开始
2. 在容器内删除文件，宿主机上的文件也是被删除的，但是删除容器，宿主机的文件就不会被删除
3. 数据卷的设计目的是为了持久化容器数据和实现数据共享，而不是复制数据，数据卷技术本身并不会导致宿主机的存储空间占用多一倍

## 好处

以后只需在本地修改即可，容器内会自动同步，且没有增加多余使用空间。

## 具名/匿名挂载

### 匿名挂载

 ```shell
 docker run -it -v <container_dir> <image_name:[TAG]/id>			# 不指定主机内路径与卷名
 ```

<img src="./img/截屏2024-12-28 下午9.26.28.png" alt="截屏2024-12-28 下午9.26.28" style="zoom:50%;" />

### 具名挂载

通过使用具名挂载可以很方便找到一个卷。

```shell
docker run -it -v <volume_name> <container_dir> <image_name:[TAG]/id>			# 不指定主机内路径但指定卷名
```

<img src="./img/截屏2024-12-28 下午9.35.09.png" alt="截屏2024-12-28 下午9.35.09" style="zoom:70%;" />

所有Docker容器内的卷，在没有指定主机内路径的情况下，默认都在主机的`/var/lib/Docker/volumes/xxx/_data`路径下。

<img src="./img/截屏2024-12-28 下午9.40.44.png" alt="截屏2024-12-28 下午9.40.44" style="zoom:50%;" />

注

1. <volume_name> ：不以`/`为开始

## 数据卷容器

### 含义

将已命名的容器挂载数据卷，其他容器通过挂载这个容器实现数据共享，挂载数据卷的容器叫做数据卷容器。

### 目的

实现容器间共享数据。

<img src="./img/截屏2024-12-29 下午2.15.46.png" alt="截屏2024-12-29 下午2.15.46" style="zoom:30%;" />

### 命令

```shell
docker run -it --name <sub_docker_name> --volumes-from <parent_docker_name> <image_name:[TAG]/id>		# 子容器挂载父 容器
```

只共享数据卷和绑定挂载，而不共享容器的临时文件系统层、临时目录、内部状态等。这意味着，容器的持久化数据（存储在数据卷中的数据）会被共享，而容器的临时数据和状态则不会。

<img src="./img/截屏2024-12-29 下午1.33.06.png" alt="截屏2024-12-29 下午1.33.06" style="zoom:50%;" />

### 效果

即使删除了初始的数据卷容器 volume-container 或其他容器，只要有容器在使用该数据卷，里面的数据就不会丢失。

即使删除了所有容器，数据卷也保留在宿主机中，这大大保证了数据的安全性。

### 删除容器

当容器被删除时，容器的可写层中的数据也会随之被删除。

然而，如果容器中的数据被存储在 Docker 数据卷中，那么即使容器被删除，这些数据也会被保留。

如果使用了宿主机上的目录作为数据卷（绑定挂载），那么容器中的数据实际上是存储在宿主机的文件系统中的。当容器被删除时，这些数据仍然存在于宿主机上。

如果使用了 `tmpfs` 作为数据存储，那么数据会存储在内存中，容器删除后，这些数据会立即丢失。

# Dockerfile

## 含义

就是用来构建Docker镜像的构建文件。

- Dockerfile是面向开发的，Dockerfile镜像逐渐成为企业交付的标准
- 通过Dockerfile在构建镜像时挂载数据卷，会在`root`路径下创建匿名卷

## 构建步骤

1. 编写一个Dockerfile文件
2. docker build 构建成为一个镜像
3. docker run 运行镜像
4. docker push 发布镜像（Dockerhub、阿里云镜像仓库）

## 基础知识

- 每个指令都必须是大写字母
- 执行都是从上到下顺序执行
- 每个指令都会创建提交一个新的镜像层

## 指令

```shell
FROM					# 基础镜像，一切从这里开始构建
MAINTAINER		# 镜像谁写的：姓名+邮箱
RUN						# 镜像构建的时候需要运行的命令
ADD						# 添加内容,自动解压
WORKDIR				# 镜像的工作目录
VOLUME				# 挂载的目录
RXPOSE				# 暴露端口配置
CMD						# 指定容器启动时运行的命令，只有最后一个会生效，原命令被覆盖，不可追加
ENTRYPOINT		# 指定容器启动时运行的命令，可以追加命令
ONBUILD				# 当构建一个被继承的 Dockerfile 这时就会触发ONBUILD指令
COPY					# 类似ADD，将文件拷贝到镜像中
ENV						# 构建时设置环境变量
```

## 举例

1. 简易版

```shell
FROM centos												# 基于centos镜像 
MAINTAINER Dkg<278196197@qq.com>	# 设置用户信息

ENV MYPATH /usr/local							# 配置环境变量
WORKDIR MYPATH										# 默认工作路径

RUN yum -y install vim						# 安装命令
RUN yum -y install net-tools

EXPOSE 80													# 暴露端口

CMD echo $MYPATH									# 打印信息
CMD echo "----end----"
CMD /bin/bash											# run镜像后启动bash shell
```

2. 稍复杂版

```shell
FROM centos
MAINTAINER Dkg<2788196197@qq.com>

COPY readme.md /usr/local/readme.md

ADD jdk-8ull-linux-x64.tar.gz /usr/local/			# 自动解压
ADD apache-tomcat-9.0.22.tar.gz /usr/local/

RUN yum -y install vim

ENV MYPATH /usr/local
WORKDIR &MYPATH

ENV JAVAHOME /usr/local/jdk1.8.0_11	
ENV CLASSPATH $JAVAHOME/lib/dt.jar:$JAVAHOME/lib/tools.jar
ENV CATALINA_HOME /usr/local/apache-tomcat-9.0.22
ENV CATALINA_BASH /usr/local/apache-tomcat-9.0.22
ENV PATH -$PATH:$JAVA_HOME/bin:$CATALINA_HOME/lib:$CATALINA_HOME/bin

EXPOSE 8080
CMD /usr/local/apache-tomcat-9.0.22/bin/startup.sh && tail -F /usr/local/apache-tomcat-9.0.22/bin/logs/catalina.out
```

## 构建

```shell
docker build -f <dockerfile_dir> -t <new_image_name>:[TAG] .		# 如果成功返回successfully build
```

Dockerfile文件命名为`Dockerfile`（官方推荐），`docker build`时会自动寻找这个文件，无需-f指定。

## 查看镜像构建过程

```SHELL
docker history <docker_name:[TAG]/id>		# 查看镜像构建过程
```

## CMD与ENTRYPOINT

```shell
FROM centos												# 基于centos镜像 
CMD ["ls","-l"]										# 无法执行，仅执行最后一条	
CMD ["ls","-a"]										# run镜像后自动执行 ls -a，但run时无法追加指令，如-l，因为此时相当于-l替换CMD ["ls","-a"]，要追加只能为ls -al		
ENTRYPOINT ["ls","-a"]										# run镜像后自动执行 ls -a，run时可以追加指令，如-l，此时相当于执行了ls -al	
```

# 发布镜像

## Dockerhub

### 流程

1. Dockerhub注册账户
2. 在本地服务器上登录该账户

```shell
docker login -u  <username>
```

3. push镜像

```shell
docker tag <image_id> <username/new_image_name>:[TAG]
docker push <username/new_image_name>:[TAG]
```

## Dockerhub

### 流程

1. 登录阿里云
2. 找到容器镜像服务
3. 创建命名空间
4. 创建容器镜像
5. 参考官方容器中cmd步骤

<img src="./img/截屏2024-12-30 上午7.07.08.png" alt="截屏2024-12-30 上午7.07.08" style="zoom:50%;" />

# Docker网络

## 命令

```shell
docker network ls													# 查看当前Docker网络
docker network inspect <network_id>				# 查看当前Docker网络信息
docker network creat											# 创建网络
docker network connect										# 连接容器到网络
```

## Docker0

<img src="./img/截屏2025-01-05 下午2.28.03.png" alt="截屏2025-01-05 下午2.28.03" style="zoom:50%;" />

服务器只要安装了Docker，就会得到一个Docker0，相当于路由器。所有容器在不指定网络的情况下，都是走Docker0路由的，Docker会给容器分配一个默认可用的ip。服务器可以通过该ip与容器交互。

不支持容器名连接访问。

## veth-pair

Docker网络中使用的技术。

一对虚拟设备接口，成对出现，一端连接协议，一端彼此相连。

veth-pair充当一个桥梁，连接各种虚拟网络设备。

使得容器之间可以相互ping通。

<img src="./img/截屏2025-01-05 下午2.49.29.png" alt="截屏2025-01-05 下午2.49.29" style="zoom:50%;" />

Docker中所有的网络接口都是虚拟的，虚拟转发的效率高。

容器移除后为其分配的网卡会自动释放。

<img src="./img/截屏2025-01-05 下午3.17.48.png" alt="截屏2025-01-05 下午3.17.48" style="zoom:50%;" />

## 通过别名替换ip

使用`-- link`

```shell
docker run -d -P --name tomcat03 --link tomcat02 tomcat
docker exec -it tomcat03 ping tomcat02		# 可以ping通
docker exec -it tomcat02 ping tomcat03		# 无法ping通，为单向，需对tomcat02进行配置
```

查看tomcat03本地的host配置文件

<img src="./img/截屏2025-01-05 下午3.41.07.png" alt="截屏2025-01-05 下午3.41.07" style="zoom:50%;" />

`--link`就是在容器host配置中增加了一个映射，但不建议使用--link，推荐使用自定义网络。

## 自定义网络

<img src="./img/截屏2025-01-05 下午3.52.24.png" alt="截屏2025-01-05 下午3.52.24" style="zoom:50%;" />

### 网络模式

- bridge：桥接
- none：不配置网络
- host：和宿主机共享网络
- container：容器网络连通

### 举例

1. 创建自定义网络

```shell
# drive： bridge
# subnet： 192.168.0.0/16
# getway： 192.168.0.1
# name： mynet
docker network creat --drive bridge --subnet 192.168.0.0/16 --getway 192.168.0.1 mynet
```

2. 查看mynet配置

<img src="./img/截屏2025-01-05 下午4.02.22.png" alt="截屏2025-01-05 下午4.02.22" style="zoom:50%;" />

3. 使用自定义网络

```shell
docker run -d -P --name tomcat01 tomcat									# 使用Docker0
docker run -d -P --name tomcat01 --net Docker0 tomcat		# 两者等价
docker run -d -P --name tomcat01 --net mynet tomcat			# 自定义网络
```

4. 此时可以不使用`--link`也可以通过名字ping对应ip

### 小结

自定义网络已经维护好了对应关系，推荐使用自定义网络。

不同集群使用不同的网络，保证集群是安全和健康的。

## 网络连通

### 问题

<img src="./img/截屏2025-01-05 下午4.12.04.png" alt="截屏2025-01-05 下午4.12.04" style="zoom:50%;" />

此时无法连通。

正常情况下网络之间无法打通，但可以将容器与网络之间打通。

### 解决方案

<img src="./img/截屏2025-01-05 下午4.17.31.png" alt="截屏2025-01-05 下午4.17.31" style="zoom:60%;" />

```shell
docker network connect mynet tomcat01		#  将容器tomcat01与网络mynet连通
```

连通后就是将tomcat01放到了mynet网络下，即一个容器两个ip地址。

# Docker项目流程

1. 构建项目
2. 打包应用
3. 编写Dockerfile
4. 构建镜像
5. 发布运行

# Docker进阶

## portainer

Docker图形界面管理工具，提供一个后台面板供操作。

## Docker Compose
### 定义

Docker Compose 是一个用于定义和运行多容器 Docker 应用程序的工具。它使用 YAML 文件来配置应用程序的服务、网络和卷。

### 功能

- 服务定义：在 `Docker-compose.yml` 文件中，可以定义多个服务及其配置，如镜像、端口映射、环境变量等
- 一键启动：通过一条命令 `Docker-compose up`，可以启动所有定义的服务，并自动处理服务间的依赖关系
- 管理便捷：提供 `Docker-compose down` 命令来停止并移除所有服务，以及 `Docker-compose restart` 等命令来管理服务

- 适用场景：适用于开发环境中的多容器应用部署和测试，简化了多服务的配置和启动过程

## Docker Swarm
### 定义

Docker Swarm 是 Docker 官方提供的容器编排工具，将多个 Docker 主机聚合成一个虚拟的单个主机，实现容器的集群管理。

### 功能

- 集群管理：通过 `Docker swarm init` 初始化 Swarm 集群，并使用 `Docker swarm join` 将其他节点加入集群
- 服务部署：使用 `Docker service create` 命令部署服务，Swarm 会自动在集群节点上调度容器实例
- 负载均衡：Swarm 内置负载均衡功能，可以将请求自动分配到多个容器实例上

- 适用场景：中小型容器集群的管理和部署

## Docker Stack
### 定义

Docker Stack 是基于 Docker Compose 文件格式的扩展，用于在 Docker Swarm 集群上部署和管理多服务应用。

### 功能

- 部署应用：使用 `Docker stack deploy` 命令将 `Docker-compose.yml` 文件中的服务部署到 Swarm 集群
- 服务管理：支持对服务的扩展、更新和删除操作，方便对集群中的服务进行集中管理

- 优势：简化了在 Swarm 集群中部署多服务应用的流程，使得应用的部署和管理更加灵活和高效

## Docker Secret

### 定义

Docker Secret 是 Docker Swarm 中用于管理敏感数据（如密码、证书等）的机制。

### 功能

- 数据加密：将敏感数据以加密形式存储在 Swarm 集群中，只有授权的服务才能访问
- 安全传输：在服务启动时，Docker Secret 会将加密数据安全地传输给容器，避免敏感数据在传输过程中被泄露。

- 适用场景：适用于需要保护敏感数据安全的容器化应用，确保数据在集群中的安全性和完整性

## Docker Config
### 定义

Docker Config 是 Docker Swarm 中用于管理配置文件的机制。

### 功能

- 配置共享：将配置文件存储在 Swarm 集群中，多个服务可以共享同一个配置文件
- 动态更新：支持配置文件的动态更新，当配置文件发生变化时，可以快速将更新推送到所有使用该配置的服务

- 应用场景：适用于需要统一管理和动态更新配置的应用场景，简化了配置文件的管理和分发过程

## Jenkins
### 定义

Jenkins 是一个开源的自动化服务器，用于持续集成和持续交付（CI/CD）。

### 功能

- 构建自动化：可以自动触发代码构建、测试和打包等任务，支持多种编程语言和构建工具
- 插件丰富：拥有大量的插件，可以与版本控制系统、测试框架、部署工具等集成，满足不同的自动化需求
- 可视化管理：提供直观的 Web 界面，方便用户配置和监控构建任务的状态

- 特点：灵活、可扩展性强，广泛应用于软件开发的持续集成和持续交付流程中，帮助提高开发效率和软件质量

## Kubernetes
### 定义

Kubernetes（简称 K8s）是一个开源的容器编排平台，用于自动化部署、扩展和管理容器化应用程序。

### 功能

- 资源管理：提供对容器资源的管理，包括 CPU、内存等资源的分配和限制
- 服务发现：支持服务发现机制，容器之间可以通过服务名称进行通信，无需关心具体的 IP 地址
- 负载均衡：内置负载均衡功能，可以将请求分发到多个容器实例，提高应用的可用性和扩展性
- 自动扩展：支持自动扩展和自动收缩容器实例，根据负载情况动态调整资源
- 自我修复：具备自我修复能力，当容器出现故障时，Kubernetes 可以自动重启容器，确保应用的持续运行

- 特点：功能强大、可扩展性好，支持多种容器运行时和存储系统，适用于大规模容器集群的管理和部署，是当前主流的容器编排工具之一
