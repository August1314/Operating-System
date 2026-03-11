FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Asia/Shanghai

# 安装必要的工具
RUN apt-get update && apt-get install -y \
    build-essential \
    nasm \
    make \
    qemu-system-x86 \
    gdb \
    && rm -rf /var/lib/apt/lists/*

# 设置工作目录
WORKDIR /os-lab

# 复制源代码
COPY . /os-lab/

# 设置环境变量
ENV PATH="/os-lab:${PATH}"

# 设置入口点
CMD ["/bin/bash"] 