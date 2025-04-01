#!/bin/bash

# 定义要追加的内容
SUDOERS_ENTRY="orin ALL=(ALL) NOPASSWD: /sbin/ip link set can0 up type can bitrate 1000000, /sbin/ip link set can0 down"

# 使用 'echo' 和 'tee' 命令将内容追加到 sudoers 文件中
echo "$SUDOERS_ENTRY" | sudo tee -a /etc/sudoers > /dev/null

# 检查是否追加成功
if sudo grep -q "$SUDOERS_ENTRY" /etc/sudoers; then
  echo "Entry successfully added to sudoers file."
else
  echo "Failed to add entry to sudoers file."
fi
