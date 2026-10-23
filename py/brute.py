#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
brute.py —— 对拍器（竞赛最重要的 30 行工具）

作用：用随机数据同时跑「正解」和「暴力」，输出不一样就停，把反例存下来。

典型用法：
    python brute.py --gen "python gen.py" --a "a.exe" --b "brute.exe" -n 500

    --gen  造数据的命令（输出写到 stdout，它的输出就是下一轮的标准输入）
    --a    你的正解（可以是一条命令，比如 "python sol.py"）
    --b    暴力/朴素实现（用来验证正解）
    -n     对拍轮数

发现反例时：
    反例输入保存为 bad_input.txt
    两边输出保存为 bad_a.txt / bad_b.txt
    退出码 1（方便脚本判断）
"""

import argparse
import random
import shlex
import subprocess
import sys


def run_cmd(cmd, stdin_data="", timeout=10):
    """执行一条命令，返回 (成功?, stdout文本, stderr文本)"""
    try:
        p = subprocess.run(
            shlex.split(cmd) if isinstance(cmd, str) else cmd,
            input=stdin_data.encode("utf-8", errors="replace"),
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            timeout=timeout,
        )
    except subprocess.TimeoutExpired:
        return False, "", "TIMEOUT"
    except FileNotFoundError:
        return False, "", "找不到可执行文件"
    ok = p.returncode == 0
    out = p.stdout.decode("utf-8", errors="replace").strip()
    err = p.stderr.decode("utf-8", errors="replace").strip()
    return ok, out, err


def normalize(text):
    """比较前的归一化：去掉行尾空白、合并连续空行"""
    lines = [ln.rstrip() for ln in text.splitlines()]
    while lines and lines[-1] == "":
        lines.pop()
    return "\n".join(lines)


def main():
    ap = argparse.ArgumentParser(description="对拍器：随机数据 + 正解 vs 暴力")
    ap.add_argument("--gen", required=True, help='造数据命令，如 "python gen.py"')
    ap.add_argument("--a", required=True, help='正解命令，如 "a.exe"')
    ap.add_argument("--b", required=True, help='暴力命令，如 "brute.exe"')
    ap.add_argument("-n", "--rounds", type=int, default=100, help="对拍轮数，默认 100")
    ap.add_argument("--timeout", type=int, default=10, help="单次运行超时秒数，默认 10")
    args = ap.parse_args()

    for i in range(1, args.rounds + 1):
        ok, data, err = run_cmd(args.gen, timeout=args.timeout)
        if not ok:
            print(f"[{i}] 造数据失败：{err}")
            return 2

        ok_a, out_a, err_a = run_cmd(args.a, data, timeout=args.timeout)
        ok_b, out_b, err_b = run_cmd(args.b, data, timeout=args.timeout)

        if not ok_a:
            print(f"[{i}] 正解崩溃/超时：{err_a}")
            _save(data, "", out_b)
            return 1
        if not ok_b:
            print(f"[{i}] 暴力崩溃/超时：{err_b}")
            _save(data, out_a, "")
            return 1

        if normalize(out_a) != normalize(out_b):
            print(f"[{i}] 发现反例！")
            print("--- 输入 ---")
            print(data[:2000])
            print("--- 正解输出 ---")
            print(out_a[:2000])
            print("--- 暴力输出 ---")
            print(out_b[:2000])
            _save(data, out_a, out_b)
            return 1

        if i % 20 == 0 or i == args.rounds:
            print(f"  已对拍 {i}/{args.rounds} 轮，全部一致")

    print(f"对拍通过：{args.rounds} 轮全部一致 ✅")
    return 0


def _save(data, out_a, out_b):
    with open("bad_input.txt", "w", encoding="utf-8") as f:
        f.write(data)
    with open("bad_a.txt", "w", encoding="utf-8") as f:
        f.write(out_a)
    with open("bad_b.txt", "w", encoding="utf-8") as f:
        f.write(out_b)
    print("已保存 bad_input.txt / bad_a.txt / bad_b.txt")


if __name__ == "__main__":
    sys.exit(main())
