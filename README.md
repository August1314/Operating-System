# 操作系统实验仓库

<div align="center">
  <img src="https://trae-api-cn.mchost.guru/api/ide/v1/text_to_image?prompt=operating%20system%20kernel%20development%20with%20assembly%20and%20C%2C%20educational%20computer%20science%20project%2C%20professional%20technical%20illustration&image_size=landscape_16_9" alt="Operating System Development" width="800">
  <p>🎉 一个全面的操作系统实验教学仓库，从底层汇编到内核开发</p>
</div>

## 📚 项目概述

本仓库是一个操作系统实验教学项目，旨在帮助学生通过实践深入理解操作系统的工作原理。从最底层的汇编语言到高级的内核功能，涵盖了操作系统开发的各个方面，为计算机科学专业学生提供了一个系统化的学习路径。

### 🌟 特点

- **循序渐进**：从基础的汇编编程到完整的内核开发
- **实践导向**：每个实验都有详细的步骤和代码示例
- **跨平台支持**：支持Windows、Linux和MacOS等多种平台
- **多架构兼容**：支持ARM、Intel和RISC-V等多种CPU架构
- **丰富的资源**：包含详细的实验指导、代码示例和参考资料

## 📁 目录结构

```text
├── docs/             # 文档和参考资料
├── images/           # 通用图片资源
├── lab1/             # 实验1：编译内核/利用已有内核构建OS
├── lab2/             # 实验2：汇编语言与计算机启动过程
├── lab3/             # 实验3：保护模式与硬盘操作
├── lab4/             # 实验4：中断处理
├── lab5/             # 实验5：进程管理
├── LICENSE           # 许可证文件
└── README.md         # 项目说明文档
```

### 实验内容概览

| 实验 | 主题 | 主要内容 |
|------|------|----------|
| lab1 | 内核编译与构建 | 编译Linux内核、制作Initramfs、编译Busybox |
| lab2 | 汇编与启动过程 | x86汇编、实模式编程、MBR编写、字符显示 |
| lab3 | 保护模式 | 段描述符、GDTR、保护模式切换、硬盘访问 |
| lab4 | 中断处理 | 8259A PIC、中断描述符表、中断处理程序 |
| lab5 | 进程管理 | 进程切换、上下文保存、调度算法 |

## 🚀 快速开始

### 环境要求

- **操作系统**：Ubuntu 18.04+（推荐）或其他Linux发行版
- **工具链**：
  - GCC/G++
  - NASM汇编器
  - QEMU模拟器
  - GDB调试器
  - CMake
  - 其他依赖：binutils、libncurses5-dev、bison、flex、libssl-dev等

### 环境配置

```bash
# 更新包管理器
sudo apt update

# 安装基本工具
sudo apt install binutils gcc nasm qemu cmake libncurses5-dev bison flex libssl-dev libc6-dev-i386

# 安装VSCode（可选）
sudo snap install code --classic
```

### 运行实验

1. **进入实验目录**
   ```bash
   cd lab1  # 或其他实验目录
   ```

2. **按照实验指导执行**
   每个实验目录都有详细的README.md文件，包含完整的实验步骤和代码示例。

3. **运行QEMU模拟**
   ```bash
   # 例如，运行lab2的MBR
   qemu-system-i386 -hda hd.img -serial null -parallel stdio
   ```

4. **使用GDB调试**
   ```bash
   # 启动带调试功能的QEMU
   qemu-system-i386 -hda hd.img -s -S -parallel stdio -serial null
   
   # 在另一个终端启动GDB
   gdb
   (gdb) target remote:1234
   ```

## 📖 实验指南

### Lab 1: 编译内核/利用已有内核构建OS

- **目标**：熟悉Linux内核的编译过程和启动过程，构建简单的操作系统
- **内容**：
  - 环境配置和工具安装
  - 下载并编译Linux内核
  - 使用QEMU启动内核并开启远程调试
  - 制作Initramfs并编写简单应用
  - 编译并启动Busybox

### Lab 2: 汇编语言与计算机启动过程

- **目标**：学习x86汇编、计算机启动过程、IA-32处理器架构和字符显存原理
- **内容**：
  - IA-32处理器体系结构
  - x86汇编基础
  - 实地址模式编程
  - 编写MBR（主引导记录）
  - 字符显示原理与实现

### Lab 3: 保护模式与硬盘操作

- **目标**：理解保护模式的工作原理和硬盘访问方法
- **内容**：
  - 保护模式的段机制
  - 段描述符和GDTR
  - 从实模式切换到保护模式
  - 硬盘的抽象结构
  - 硬盘访问编程

### Lab 4: 中断处理

- **目标**：掌握中断处理的原理和实现方法
- **内容**：
  - 8259A PIC（可编程中断控制器）
  - 中断描述符表（IDT）
  - 中断处理程序编写
  - 中断向量和中断服务例程

### Lab 5: 进程管理

- **目标**：理解进程管理的基本概念和实现方法
- **内容**：
  - 进程控制块（PCB）
  - 进程切换机制
  - 上下文保存和恢复
  - 进程调度算法

## 🔧 开发工具

- **代码编辑器**：VSCode、Vim、Gedit
- **汇编器**：NASM
- **编译器**：GCC/G++
- **模拟器**：QEMU
- **调试器**：GDB
- **构建工具**：Make、CMake

## 🤝 贡献指南

我们欢迎并感谢任何形式的贡献！如果你有兴趣参与本项目，请：

1. **Fork** 本仓库
2. **创建** 你的特性分支 (`git checkout -b feature/AmazingFeature`)
3. **提交** 你的更改 (`git commit -m 'Add some AmazingFeature'`)
4. **推送到** 分支 (`git push origin feature/AmazingFeature`)
5. **开启** 一个 Pull Request

### 贡献内容

- 修复代码中的bug
- 改进实验指导文档
- 添加新的实验内容
- 优化现有代码
- 提供新的工具和资源

## 📄 许可证

本项目采用 **GNU General Public License v3.0** 许可证。详见 [LICENSE](LICENSE) 文件。

## 📞 联系方式

- **项目维护者**：梁力航
- **实验课程邮箱**：os_sysu_lab@163.com
- **问题反馈**：请在GitHub上提交Issue

## 🎓 学习资源

### 参考书籍
- 《x86汇编语言 从实模式到保护模式》
- 《操作系统原理》
- 《Linux内核设计与实现》

### 在线资源
- [Linux命令查询](https://wangchujiang.com/linux-command/)
- [QEMU文档](https://qemu-project.gitlab.io/qemu/system/invocation.html)
- [GDB教程](http://c.biancheng.net/view/8123.html)
- [Matrix镜像源](https://mirrors.matrix.moe)
- [清华Tuna镜像源](https://mirrors.tuna.tsinghua.edu.cn)

## 🙏 致谢

感谢所有为这个项目做出贡献的同学和老师！特别感谢实验助教组的辛勤工作，为我们提供了详细的实验指导和技术支持。

---

<div align="center">
  <p>✨ 开始你的操作系统开发之旅吧！ ✨</p>
</div>