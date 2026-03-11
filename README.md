<div align="center">
<img src="[https://capsule-render.vercel.app/render?type=waving&color=auto&height=200&section=header&text=OS%20Lab%20Hub&fontSize=70](https://www.google.com/search?q=https://capsule-render.vercel.app/render%3Ftype%3Dwaving%26color%3Dauto%26height%3D200%26section%3Dheader%26text%3DOS%2520Lab%2520Hub%26fontSize%3D70)" />

<p><strong>🎓 从底层汇编到微内核开发：全栈操作系统实验指南</strong></p>

[🚀 快速开始](https://www.google.com/search?q=%23-%E5%BF%AB%E9%80%9F%E5%BC%80%E5%A7%8B) • [📁 目录结构](https://www.google.com/search?q=%23-%E7%9B%AE%E5%BD%95%E7%BB%93%E6%9E%84) • [📖 实验手册](https://www.google.com/search?q=%23-%E5%AE%9E%E9%AA%8C%E6%89%8B%E5%86%8C) • [🤝 贡献代码](https://www.google.com/search?q=%23-%E8%B4%A1%E7%8C%AE%E6%8C%87%E5%8D%97)

<div>
<a href="README_EN.md">English</a> | <b>中文简体</b>
</div>
</div>

---

## 📚 项目概述

本仓库是一套系统化的操作系统实验教学体系。我们不只是在写代码，而是在**“复刻”**计算机进化的史诗：从按下电源键后的第一条汇编指令，到多进程并发调度的现代内核。

### 🌟 核心特性

* **⚡ 全栈视角**：覆盖从 Bootloader、保护模式、内存管理到进程调度的全链路。
* **🛠️ 工业级工具链**：深度集成 GCC、NASM、QEMU 与 GDB，模拟真实内核开发环境。
* **🧩 模块化设计**：各实验解耦，既可循序渐进，也可针对特定模块（如中断、分页）专项突破。
* **💻 跨平台友好**：提供详细的 Docker 环境配置，确保在 Windows/macOS/Linux 下表现一致。

---

## 📁 实验地图

本项目的逻辑演进如下图所示：

| 阶段 | 实验模块 | 核心挑战 | 技术栈 |
| --- | --- | --- | --- |
| **Stage 1** | [lab1](https://www.google.com/search?q=./lab1) | **内核初探**：编译 Linux 源码与构建 Busybox 根文件系统 | C, Bash |
| **Stage 2** | [lab2](https://www.google.com/search?q=./lab2) | **唤醒硬件**：编写 MBR 引导程序，掌握实模式字符显示 | x86 ASM |
| **Stage 3** | [lab3](https://www.google.com/search?q=./lab3) | **打破枷锁**：突破 1MB 内存限制，跃迁至 **32位保护模式** | GDT, Segmentation |
| **Stage 4** | [lab4](https://www.google.com/search?q=./lab4) | **外设沟通**：构建 IDT 处理 8259A 硬件中断 | Interrupts, PIC |
| **Stage 5** | [lab5](https://www.google.com/search?q=./lab5) | **多维时空**：实现 PCB 与上下文切换，运行你的第一个进程 | Scheduling, Context Switch |

---

## 🚀 快速开始

### 1. 环境准备 (Ubuntu/Debian)

一行命令安装所有必要的底层开发工具：

```bash
sudo apt update && sudo apt install -y \
    binutils gcc-multilib g++-multilib nasm qemu-system-x86 \
    cmake make gdb libncurses5-dev bison flex libssl-dev

```

### 2. 编译与仿真 (以 Lab3 为例)

```bash
# 进入实验目录
cd lab3

# 使用 Make 构建磁盘镜像
make clean && make build

# 启动 QEMU 模拟器
make run

```

### 3. 远程调试模式

如果你需要调试内核启动过程，本项目预设了调试指令：

```bash
# 终端 A: 启动 QEMU 并挂起等待调试器
make debug

# 终端 B: 启动 GDB 并连接
gdb -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"

```

---

## 🔧 开发工具箱

| 工具 | 用途 | 备注 |
| --- | --- | --- |
| **VSCode** | 现代化的源码编辑 | 推荐安装 `C/C++` 和 `x86 and x86_64 Assembly` 插件 |
| **QEMU** | 裸机模拟 | 模拟 CPU、内存及 8259A 芯片 |
| **NASM** | 汇编编译 | 采用 Intel 语法，对初学者更友好 |
| **GDB** | 源码级调试 | 配合 QEMU 的 `-s -S` 参数实现内核单步调试 |

---

## 🤝 贡献指南

哪怕是一个错别字的修正，对项目来说也是巨大的帮助！

1. **Fork** 本仓库。
2. 创建特性分支：`git checkout -b feature/AmazingFeature`。
3. 提交更改：`git commit -m 'Add some AmazingFeature'`。
4. 发起 **Pull Request**。

---

## 🎓 学习资源库

### 📖 推荐书目

* 📘 **《x86汇编语言：从实模式到保护模式》** (李忠著) - 极佳的入门教材。
* 📙 **《操作系统真相还原》** (郑钢著) - 细致入微的内核实现过程。
* 📗 **《The Little OS Book》** - 轻量级的英文进阶手册。

### 🌐 实用链接

* [OSDev Wiki](https://www.google.com/search?q=https://wiki.osdev.org/) - 操作系统开发者的维基百科（圣经）。
* [Linux Kernel Archive](https://www.google.com/search?q=https://www.kernel.org/) - 官方内核源码。

---

## ✉️ 联系与反馈

* **Maintainer**: 梁力航
* **Email**: [1870616586@qq.com](mailto:1870616586@qq.com)
* **Issues**: [提交反馈](https://www.google.com/search?q=https://github.com/your-repo/issues)

<div align="center">
<p>💡 <b>"In order to understand recursion, one must first understand recursion."</b></p>
<p>构建属于你自己的操作系统吧！</p>
</div>
