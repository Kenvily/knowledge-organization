<font size = 6>Git</font>

[toc]

#  引言

## 何为版本控制

版本控制是一种记录文件内容变化，以便将来查阅特定版本修订情况的系统。

版本控制其实最重要的是可以记录文件修改历史记录，从而让用户能够查看历史版本，方便版本切换。

## 为什么需要版本控制

个人开发过渡到团队协作。

<img src="./img/截屏2024-12-11 下午8.43.35.png" alt="截屏2024-12-11 下午8.43.35" style="zoom:40%;" />

## 集中式与分布式

* 集中式版本控制工具：CVS、SVN(Subversion)、VSS等

* 分布式版本控制工具：Git、Mercurial、Bazaar、Darcs等

Git 属于分布式版本控制系统，而 SVN 属于集中式。

<img src="./img/截屏2024-12-10 下午8.37.16.png" alt="截屏2024-12-10 下午8.37.16" style="zoom:40%;" />

**区别**

1. 集中式版本控制只有中心服务器拥有一份代码，而分布式版本控制每个人的电脑上就有一份完整的代码
2. 集中式版本控制有安全性问题，当中心服务器挂了所有人都没办法工作
3. 集中式版本控制需要连网才能工作，如果网速过慢，那么提交一个文件会慢的无法让人忍受。而分布式版本控制不需要连网就能工作
4. 分布式版本控制新建分支、合并分支操作速度非常快，而集中式版本控制新建一个分支相当于复制一份完整代码

# Git

Git是一个免费的、开源的分布式版本控制系统，可以快速高效地处理从小型到大型的各种项目，默认编辑器为vim。

**特点**

- 速度快
- 设计简单
- 对非线性开发模式的强力支持（允许成千上万个并行开发的分支）
- 完全分布式
- 有能力高效管理类似 Linux 内核一样的超大规模项目（速度和数据量）

## Git与快照

不同于其他版本控制系统基于差异，Git更像是把数据看作是对小型文件系统的一系列快照。 在 Git 中，每当提交更新或保存项目状态时，它基本上就会对当时的全部文件创建一个快照并保存这个快照的索引。 为了效率，如果文件没有修改，Git 不再重新存储该文件，而是只保留一个链接指向之前存储的文件。 Git 对待数据更像是一个快照流。

<img src="./img/截屏2024-12-17 上午4.47.02.png" alt="截屏2024-12-17 上午4.47.02" style="zoom:30%;" />

## 完整性

Git 中所有的数据在存储前都计算校验和，然后以校验和来引用。这意味着不可能在 Git 不知情时更改任何文件内容或目录内容。 

Git 用以计算校验和的机制叫做 SHA-1 散列（hash，哈希）。 这是一个由 40 个十六进制字符（0-9 和 a-f）组成的字符串，基于 Git 中文件的内容或目录结构计算出来。

例如：`24b9da6552252987aa493b52f8696cd6d3b00373`

实际上，Git 数据库中保存的信息都是以文件内容的哈希值来索引，而不是文件名。

## 三种状态

Git 有三种状态，你的文件可能处于其中之一

- 已修改（modified）：表示修改了文件，但还没保存到数据库中
- 已暂存（staged）：表示对一个已修改文件的当前版本做了标记，使之包含在下次提交的快照中
- 已提交（committed）：表示数据已经安全地保存在本地数据库中

<img src="./img/截屏2024-12-17 上午4.57.15.png" alt="截屏2024-12-17 上午4.57.15" style="zoom:30%;" />

## 工作流

新建一个仓库之后，当前目录就成为了工作区，工作区下有一个隐藏目录 .git，它属于 Git 的版本库。

Git 的版本库有一个称为 Stage 的暂存区以及最后的 History 版本库，History 存储所有分支信息，使用一个 HEAD 指针指向当前分支。

<img src="./img/截屏2024-12-10 下午8.42.29.png" alt="截屏2024-12-10 下午8.42.29" style="zoom:40%;" />

> - git add files：把文件的修改添加到暂存区
> - git commit：把暂存区的修改提交到当前分支，提交之后暂存区就被清空了
> - git reset -- files：撤销提交
> - git checkout -- files：撤销文件修改，恢复到最后一次提交的状态

<img src="./img/截屏2024-12-10 下午8.45.03.png" alt="截屏2024-12-10 下午8.45.03" style="zoom:40%;" />

也可以跳过暂存区域直接从分支中取出修改，或者直接提交修改到分支中

> - git commit -a 直接把所有文件的修改添加到暂存区然后执行提交
> - git checkout HEAD -- files 取出最后一次修改

<img src="./img/截屏2024-12-10 下午8.49.15.png" alt="截屏2024-12-10 下午8.49.15" style="zoom:40%;" />

## 中心服务器

中心服务器用来交换每个用户的修改，没有中心服务器也能工作，但是中心服务器能够 24 小时保持开机状态，这样就能更方便的交换修改。Github 就是一个中心服务器。

* 局域网：GitLab

* 互联网：GitHub（外网）、Gitee码云（国内网站）


## 特殊文件

- .gitignore 文件：提交时可以忽略里面的文件，如`.log、.jar、.o、缩略图、敏感信息`等与项目无关的文件

```tex
# Compiled Object files
*.o

# Compiled Static libraries
*.a
*.lib

# Executables
*.exe
*.out
*.app
```

不需要全部自己编写，可以到[this](https://github.com/github/gitignore)中进行查询。

- LICENSE文件：开源许可证
- README文件：会在Github项目的首页渲染出来

# Git 命令

## 配置命令

### 全局范围签名设置

签名的作用是区分不同操作者身份，用户的签名信息在每一个版本的提交信息中能够看到，以此确认本次提交是谁做的。

Git首次安装必须设置一下用户签名，否则无法提交代码。

```shell
git config --global user.name "your_username"
git config --global user.email "your_email"
git config --list 				# 查看配置
```

## 基础命令

### 初始化本地库

```shell
git init					# 当前目录下初始化本地仓库 
git init newDir 	# 在newDir目录下生成一个.git目录 
```

生成.git隐藏文件夹（Git 仓库的骨干），且会默认生成一个master分支。

### 克隆仓库

```shell
git clone <repository_url> 														# 将Git仓库master分支拷贝到本地
git clone -b <repository_branch> <repository_url> 		# 将Git仓库指定分支拷贝到本地
```

如

```shell
git clone git://github.com/schacon/grit newgit	# 克隆远程仓库，目标目录名设为newgit
```

注

1. 远程 Git 仓库中的每一个文件的每一个版本都将被拉取下来
2. 自动将其添加为远程仓库并默认以 “origin” 为简写
3. 与直接下载zip压缩文件相比，多了.Git的配置管理文件，文件大小更大

### 查看当前状态

```shell
git status			# 查看当前状态（所在分支、进行的修改提交）
git status -s 	# 更为紧凑的输出
```

工作目录下的每一个文件都不外乎这两种状态：**已跟踪**或**未跟踪(untracked)**。 已跟踪的文件是指那些被纳入了版本控制的文件；工作目录中除已跟踪文件外的其它所有文件都属于未跟踪文件，它们既不存在于上次快照的记录中，也没有被放入暂存区。

### 查看差异

比较工作目录中当前文件和暂存区域快照之间的差异，也就是修改之后还没有暂存起来的变化内容。

```shell
git diff						# 查看详细状态
git diff --staged 	# 比对已暂存文件与最后一次提交的文件差异
```

> - 尚未缓存的改动：git diff
> - 查看已缓存的改动： git diff --cached
> - 查看已缓存的与未缓存的所有改动：git diff HEAD
> - 显示摘要而非整个 diff：git diff --stat

### 将工作区的文件添加到暂存区

```shell
git add <fileName>
git add.						# 全部文件添加到暂存区
git add *.c 				# 将全部c文件添加到暂存区
```

### 暂存区文件提交到本地库

```shell
git commit -m "log_info" fileName
git commit -m "log_info"	# 暂存区中文件全部提交
git commit -am "log_info" # 跳过add,直接提交

# 最终只会有一个提交——第二次提交将代替第一次提交的结果
git commit -m 'initial commit'
git add forgotten_file
git commit --amend
```

注

1. git关注的是文件，无法提交空目录

### 移除文件

```shell
git rm <fileName>						# 移除文件 不允许已经放到暂存区域
git rm -f <fileName>				# 强制移除 允许已经放到暂存区域
git rm –r <fileDir>					# 递归删除
git rm --cached <fileName>	# 把文件从暂存区删除，但在工作区保留
```

### 取消暂存区内容

 ```shell
 git reset HEAD <cached_file>	# 取消之前 git add 添加到缓存区的cached_file文件
 ```

### 撤消对文件的修改

```shell
git checkout -- <fileName> 	# 撤消修改——将它还原成上次提交时的样子
```

注

1. 除非确实清楚不想要对哪个文件的本地修改，否则请不要使用这个命令
2. 在 Git 中任何已提交的东西几乎总是可以恢复的，然而，任何未提交的东西丢失后很可能再也找不到了

### 移动/重命名

```shell
git mv README.md README			# 重命名
git mv README.md ./newDir		# 移动
```

其实，运行 `git mv` 就相当于运行了下面三条命令

```shell
mv README.md README
git rm README.md
git add README
```

## 历史版本

### 查看历史版本

```shell
git reflog					# 显示所有的操作记录，包括提交，回退的操作
git log  						# 按时间先后顺序列出所有的提交，最近的更新排在最上面
git log -p -number	# 显示每次提交所引入的差异并限制显示的日志条目数量
git log --stat  		# 附带一系列的总结性选项
```

关于`git log`的几种选项

> * --oneline ：查看历史记录的简洁版本
> * --graph ：查看历史中什么时候出现了分支、合并
> * --reverse ：逆向显示所有日志
> * --author ：查找指定用户的提交日志
> * --since、--before、 --until、--after： 指定筛选日期
> * --no-merges ：选项以隐藏合并提交

作者指的是实际作出修改的人，提交者指的是最后将此工作成果提交到仓库的人。

### 版本穿梭

```shell
git checkout <commit_hash>			# 转到该版本，不重置头指针，进入头分离指针状态 常用于查看代码
git reset <commit_hash>					# 重置版本，重置头指针，重置暂存区，不重置工作区 不常用
git reset --hard <version_num>	# 重置版本，重置头指针，重置暂存区和工作区
```

注

1. 开发过程尽量保持线性，不要有过多分支

2. 回滚只是为了查看，尽量不要在历史版本中修改文件的内容，否则容易导致项目难以管理，应该返回之前的最新版下进行开发

## 分支管理

Git 的分支，其实本质上仅仅是指向提交对象的可变指针。使用指针将每个提交连接成一条时间线，HEAD 指针指向当前所在的分支。

<img src="./img/截屏2024-12-10 下午8.50.20.png" alt="截屏2024-12-10 下午8.50.20" style="zoom:50%;" />

新建分支是新建一个指针指向时间线的最后一个节点，并让 HEAD 指针指向新分支，表示新分支成为当前分支。

<img src="./img/截屏2024-12-10 下午8.51.41.png" alt="截屏2024-12-10 下午8.51.41" style="zoom:50%;" />

每次提交只会让当前分支指针向前移动，而其它分支指针不会移动。

<img src="./img/截屏2024-12-10 下午8.52.55.png" alt="截屏2024-12-10 下午8.52.55" style="zoom:50%;" />

合并分支也只需要改变指针即可。

<img src="./img/截屏2024-12-10 下午8.53.28.png" alt="截屏2024-12-10 下午8.53.28" style="zoom:50%;" />

### 查看/创建分支

```shell
git branch							# 查看本地分支
git branch <branchName>		# 在当前所在的提交对象上创建一个指针
git branch -v 					# 查看每一个分支的最后一次提交
git branch --merged 		# 查看哪些分支已经合并到当前分支
git branch --no-merged	# 查看所有未合并工作的分支
```

注

1. git的分支必须指向一个commit，没有commit就没有任何分支

### 切换/创建分支

```shell
git checkout <branchName>
git checkout -b <branchName> 	# 创建新分支并立即切换到该分支下
```

### 合并分支

```shell
git merge <branchName>		# 将branchName分支合并到到当前分支中
```

注

1. 要合并的分支必须有一个共同的祖先提交
2. 工作目录应该是干净的，即没有未提交的更改
3. 两个分支的更改没有重叠

### Fast forward

尝试合并一个分支到当前分支，而这两个分支之间没有发生任何冲突，并且当前分支直接指向要合并分支的最新提交的直接前驱，Git 可以简单地通过移动指针来合并它们，而不需要创建一个新的合并提交。

但由于会直接将 master 分支指向合并的分支，这种模式下进行分支合并会丢失分支信息，也就不能在分支历史上看出分支信息。

可以在合并时加上 --no-ff 参数来禁用 Fast forward 模式，并且加上 -m 参数让合并时产生一个新的 commit。

```shell
git merge --no-ff -m "merge with no-ff" dev
```

<img src="./img/截屏2024-12-10 下午9.00.16.png" alt="截屏2024-12-10 下午9.00.16" style="zoom:50%;" />

### 冲突

当两个分支都对同一个文件的同一行进行了修改，在分支合并时就会产生冲突。

<img src="./img/截屏2024-12-10 下午8.55.08.png" alt="截屏2024-12-10 下午8.55.08" style="zoom:50%;" />

Git 会使用 <<<<<<< ，\=\=\=\=\=\=\= ，>>>>>>> 标记出不同分支的内容，只需要把不同分支中冲突部分修改成一样就能解决冲突。

```shell
<<<<<<< HEAD
Creating a new branch is quick & simple.
=======
Creating a new branch is quick AND simple.
>>>>>>> feature1
```

### 删除分支

 ```shell
git branch -d <branchName>
 ```

注

1. merge记得删除多余分支

### Stashing

在一个分支上操作之后，如果还没有将修改提交到分支上，此时进行切换分支，那么另一个分支上也能看到新的修改。这是因为所有分支都共用一个工作区的缘故。

可以使用 git stash 将当前分支的修改储藏起来，此时当前工作区的所有修改都会被存到栈中，也就是说当前工作区是干净的，没有任何未提交的修改。此时就可以安全的切换到其它分支上了。

```shell
git stash
```

该功能可以用于 bug 分支的实现。如果当前正在 dev 分支上进行开发，但是此时 master 上有个 bug 需要修复，但是 dev 分支上的开发还未完成，不想立即提交。在新建 bug 分支并切换到 bug 分支之前就需要使用 git stash 将 dev 分支的未提交修改储藏起来。

## Git标签

使用标签可以很方便的永远的记住那个特别的提交快照，我们发一个新的版本时，可以给它加一个“vx.x”版本

### 查看标签

```shell
git tag		# 查看所有标签
```

### 创建标签|追加标签

```shell
git tag <vx.x>									# 创建一个不带注解的标签（轻量级标签）
git tag -a <vx.x>								# 创建一个带注解的标签
git tag -a <vx.x> <version_num> # 对已提交的version_num追加标签
git tag -a <vx.x> -m "tag_info"	# 包含额外信息的注解标签，可修改
git tag -s <vx.x> -m "tag_info"	# 包含额外信息的签名的注解标签，不可修改	
```

### 查看标签与对应提交信息

```shell
git show <vx.x>				# 显示了打标签者的信息、打标签的日期时间、附注信息，然后显示具体的提交信息。
```

### 删除标签

```shell
git tag -d <tag_name>												# 删除一个轻量标签
git push <remote_name> --delete <tag_name>	# 删除远程仓库的标签
```

## Git 远程仓库

### 添加远程仓库

```shell
git remote add [shortname] <url>	 # shortname为别名， url为远程仓库的地址

git remote add [shortname] https://<Token>@github.com/<用户名>/<仓库名>.git
git remote add [shortname]  https://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@github.com/Kenvily/knowledge-organization 
```

注

1. 远程仓库名称默认为 origin

### 查看当前远程仓库

```shell
git remote							# 查看当前有哪些远程仓库
git remote -v 					# 显示需要读写远程仓库使用的 Git保存的简写与其对应的URL
git remote show remote	# 查看某一个远程仓库的更多信息
```

### 提取远程仓库数据

```shell
git fetch <remote_name>								# 拉取remote_name仓库中有但你没有的信息
```

该命令执行完后需要执行git merge远程分支到你所在的分支，如

```shell
git merge <remote_name/branchName>		# 将 remote_name/branchName 分支上的更改合并到当前检出（checked out）的本地分支
```

### 拉取远程仓库数据并合并

```shell
git pull <remote_name> <branchName>										# 将远程仓库 remote_name 的 branchName 分支的更改拉取到本地分支
git pull <remote_name> <branchName>:<localBranchName>		# 从 remote_name 远程仓库的 branchName 分支拉取代码，并尝试合并到本地的 localBranchName 分支。如果本地分支不存在，Git 会创建它。
```

注

1. 无需执行git merge
2. 使用 `git pull` 时，如果远程仓库没有但本地有的文件，执行完后这些文件并不会直接消失，但可能会遇到以下几种情况
   - 文件未被跟踪（Untracked Files）： 如果本地有的文件在远程仓库中不存在，并且这些文件也没有被 `git` 跟踪（即没有被添加到暂存区或提交过），那么这些文件会保持不变，它们不会被 `git pull` 影响
   - 文件被忽略（Ignored Files）： 如果这些文件被 `.gitignore` 文件忽略，那么 `git pull` 也不会影响它们
   - 文件被删除（Deleted Files）： 如果远程分支中的文件被删除了，而你的本地分支中还有这个文件，执行 `git pull` 后，Git 会提示有冲突。你可以选择保留本地的文件（即不删除），或者覆盖本地的文件以匹配远程仓库的状态
   - 文件冲突（Conflicts）： 如果远程仓库中的文件被修改并提交了，而你的本地分支中也有这个文件，那么 `git pull` 合并时可能会产生冲突。你需要手动解决这些冲突，决定是保留本地的更改、远程的更改，还是两者的混合

### 推送数据到远程仓库

```shell
git push <remote_name> <branchName>			# 推送分支与数据到远端仓库指定分支（若分支不存在则自动创建）
git push -f <remote_name> <branchName>	# 强制推送分支与数据到远端仓库(确保本地为想要版本)
git push <remote_name> HEAD  						# 将本地的任何分支推送到远程仓库的同名分支 
git push <remote_name> <tag_name> 			# 显式地推送标签到共享服务器上
git push <remote_name> --tags						# 一次性推送很多标签
```

注

1. 此命令容易造成“删库跑路”的情况，应该谨慎使用，或使用成熟的软件（gitlab）或插件来避免
2. 只有当有所克隆服务器的写入权限，并且之前没有人推送过时，这条命令才能生效。 当和其他人在同一时间克隆，他们先推送到上游然后你再推送到上游，你的推送就会毫无疑问地被拒绝。 你必须先抓取他们的工作并将其合并进你的工作后才能推送
3. 默认情况下，git push 命令并不会传送标签到远程仓库服务器上

### 重命名远程仓库

```shell
git remote rename <pb> <paul>		# 将pb重命名为 paul
```

同样也会修改所有远程跟踪的分支名字。

### 删除远程仓库

```shell
git remote rm <remote_name>		# remote_name为别名
```

# GitHub

GitHub 是一个基于 Git 的代码托管平台，提供了一个基于 Web 的图形界面（GUI），使得用户可以更容易地使用 Git。

## SSH 传输设置

Git 仓库和 Github 中心仓库之间的传输是通过 SSH 加密。

如果工作区下没有 .ssh 目录，或者该目录下没有 id_rsa 和 id_rsa.pub 这两个文件，可以通过以下命令来创建 SSH Key

```shell
ssh-keygen -t rsa -C "youremail@example.com"
```

然后把公钥 id_rsa.pub 的内容复制到 Github "Account settings" 的 SSH Keys 中。

## fork

fork不是git操作，而是github操作，实现代码仓库的克隆。

fork之后会在自己github账户下创建一个新仓库，包含元仓库所有内容，如分支、tag、提交历史等。

## pull request

Pull Request是一种代码审查和代码合并的机制，用于将一个分支的更改合并到另一个分支。

## git交互

1. 使用git clone 仓库到本地
2. 从 `master` 分支创建一个新分支
3. 修改并提交到本地仓库
4. 使用git push到远程仓库
5. 使用 pull request选择对应分支进行合并

## Gitee

GitHub的网站有时候会连接不上，无法登录。可以使用阿里提供的Git远程仓库网站Gitee来代替。

# GitLab  

GitLab 也是一个基于 Git 的代码托管和协作平台，类似于 GitHub，但它提供了一个自托管的选项。这意味着用户可以在自己的服务器上安装和运行 GitLab，从而拥有更多的控制权和隐私。

使用git，还需要一个远程代码仓库。常见的github、gitee这种远程代码仓库，公司中一般不会使用，因为他们是使用外网的，不够安全，易泄露数据。一般企业都会搭建一个仅内网使用的远程代码仓库，最常见就是 GitLab。

## 安装部署

GitLab一般由公司的运维人员安装部署，开发人员只需要申请账号和相应权限即可。

## 团队管理

去到一家公司，应该是已经有了GitLab平台，运维人员拥有root管理员账号。而作为一名普通的开发人员，你的leader和同事都拥有各自的GitLab账号和不同权限。入职后，你只需要申请开通GitLab账号和对应权限，不需要你来操作。

## 群组

在公司里会有前端、后端、大数据等一系列组，组中还有下属组。

对于人员权限以及角色的控制有如下五种

- Owner：最高权限，谁去创建组，这个组就被谁拥有，可以开除管理员，但管理员无法操作owner的角色

- Maintainer：管理员——只是具备sudo权限的用户，一般是给小组的组长或是产品线的总监

- Developer：读写权限——程序员

- Repoter：只读权限

- guest：匿名，直接去掉，例如原成员离职

## GitLab功能拓展

- code review：代码审查是指在软件开发过程中，对源代码的系统性检查。通常的目的是查找系统缺陷，保证软件总体质量和提高开发者自身水平
- CICD部署程序：自动化构建、测试和部署

# 企业项目构建与开发分支

- 在实际开发中，一般成员只负责自己那部分的feature分支，开发完成后合并到上一层开发分支，经审查测试通过后完成合并
- 完成develop分支后会在release分支进行测试
- （2-3反复多次）测试完成后release分支会合并到master并上线
- 对于上线版本中出现的bug，通过创建hotfix分支来进行修改，完成后进行测试同时合并到master分支

## 工作流介绍

### 集中式工作流

所有修改都提交到 Master 这个分支。比较适合极小团队或单人维护的项目，不建议使用这种方式

<img src=".\img\wps1111.jpg" alt="img" style="zoom:80%;" />

### 功能开发工作流

功能开发应该在一个专门的分支，而不是在 master 分支上，适用于小团队开发

<img src=".\img\wps21111.jpg" alt="img" style="zoom:80%;" />

### GitFlow工作流

公司中最常用于管理大型项目。为功能开发、发布准备和维护设立了独立的分支，让发布迭代过程更流畅

<img src=".\img\wps13111.jpg" alt="img" style="zoom:80%;" />

### Forking工作流

在 GitFlow 基础上，充分利用 Git 的 Fork 和 pull request 的功能以达到代码审核的目的（合并到master时需要经过审查通过）。一般用于跨团队协作、网上开源项目

<img src=".\img\wps41111.jpg" alt="img" style="zoom:67%;" />

## 各分支功能介绍

<img src=".\img\wps13111.jpg" alt="img" style="zoom:80%;" />

- 主干分支 master：主要负责管理正在运行的生产环境代码，永远保持与正在运行的生产环境完全一致。为了保持稳定性一般不会直接在这个分支上修改代码，都是通过其他分支合并过来的
- 热修分支 hotfix：主要负责管理生产环境下出现的紧急修复的代码， 从主干分支分出，修复完毕并测试上线后，并回主干分支和开发分支
- 开发分支 develop：主要负责管理正在开发过程中的代码
- 功能分支 feature：开发模块，会从开发分支中独立出来,，完成后会合并到开发分支
- 准发布分支 release：较大的版本上线前，会从开发分支中分出准生产分支，进行最后阶段的集成测试。该版本上线后，会合并到主干分支，生产环境运行一段阶段较稳定后可以视情况删除

# 冲突提交

实际单个模块的开发往往不是单独一个人来进行操作，当多个人协同开发相同的一个项目时，就会涉及到提交冲突的问题，以下均以同一分支下，本地push时版本已修改为前提。

## 不同人修改不同文件

1. 先pull后push

2. 直接merge

## 不同人修改同文件的不同区域

1. 先pull后push

2. 直接merge

## 不同人修改同文件的相同区域

先merge，后人为判断进行合并或删除等操作

## 不同人变更文件名和文件内容

1. 先pull，后 `git status` 查看状态，根据提示 `git rm` 删除对应修改文件
1.  `git commit` 之后人为判断代码保留部分，再pull

# Reference

1. [Git官方文档](https://git-scm.com/book/zh/v2)

2. [命令集](./git-cheat-sheet.pdf)   
