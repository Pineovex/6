# 三语对照 · 同一件事的第三种写法
# 任务与 arr.c / arr.cpp 完全一样：读 n 个整数，输出 个数/总和/最大值/最小值/去重升序
#
# 对照要点（这次看"少了什么"）：
#   1. 没有容量/长度两个变量 —— list 替你管
#   2. 没有 free/delete —— 引用计数在没人引用时自动回收
#   3. 没有类型声明 —— 但这不代表没类型，`x + 1` 在 int 和 str 上是两回事
#   4. 排序用 sorted()（返回新列表，原列表不变），C++ 的 sort 是原地改
#
# 运行：python arr.py
import sys


def main() -> int:
    tokens = sys.stdin.read().split()
    if not tokens:
        return 0

    n = int(tokens[0])
    nums = [int(t) for t in tokens[1:1 + n]]      # 切片天然处理"给的比 n 少"的情况
    if not nums:
        return 0

    uniq = sorted(set(nums))                       # set 去重 + sorted 排序，两行并一行

    print(f"count = {len(nums)}")
    print(f"sum   = {sum(nums)}")
    print(f"max   = {max(nums)}")
    print(f"min   = {min(nums)}")
    print("uniq  = " + " ".join(str(v) for v in uniq))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
