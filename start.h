#!/usr/bin/env bash
set -euo pipefail

# ---- 基本路径 ----
PROJECT_ROOT="$(cd "$(dirname "$0")" && pwd)"
VENV_DIR="$PROJECT_ROOT/.venv"
LOG_DIR="$PROJECT_ROOT/logs"
PID_DIR="$PROJECT_ROOT/pids"

mkdir -p "$LOG_DIR" "$PID_DIR"

# ---- 选择 Python ----
if command -v python3.10 >/dev/null 2>&1; then
  PY=python3.10
elif command -v python3 >/dev/null 2>&1; then
  PY=python3
else
  echo "[ERR] 未找到 python3.10 / python3，请先安装 Python 3.10+"
  exit 1
fi

# ---- 创建虚拟环境 ----
if [ ! -d "$VENV_DIR" ]; then
  echo "[INFO] 创建虚拟环境: $VENV_DIR"
  "$PY" -m venv "$VENV_DIR"
fi
VENV_PY="$VENV_DIR/bin/python"
VENV_PIP="$VENV_DIR/bin/pip"

# ---- 升级 pip/基础构建工具 ----
"$VENV_PIP" install -U pip setuptools wheel -i https://pypi.tuna.tsinghua.edu.cn/simple
#
# ---- 安装 ONNX Runtime (GPU 优先，失败可手动换 CPU) ----
echo "[INFO] 安装 onnxruntime-gpu"
"$VENV_PIP" install onnxruntime-gpu -i https://pypi.tuna.tsinghua.edu.cn/simple

# ---- 安装 InsightFace 最小依赖 ----
echo "[INFO] 安装 InsightFace 及依赖"
"$VENV_PIP" install -r "$PROJECT_ROOT/requirements.txt" \
  -i https://pypi.tuna.tsinghua.edu.cn/simple

# ---- RabbitMQ 客户端 ----
"$VENV_PIP" install pika -i https://pypi.tuna.tsinghua.edu.cn/simple

# ---- 公共环境变量 ----
export PYTHONUNBUFFERED=1
export OMP_NUM_THREADS="${OMP_NUM_THREADS:-1}"

echo "[OK] InsightFace 环境准备完成"
