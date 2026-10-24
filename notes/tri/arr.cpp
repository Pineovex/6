// 三语对照 · 同一件事的第二种写法
// 任务与 arr.c 完全一样：读 n 个整数，输出 个数/总和/最大值/最小值/去重升序
//
// 对照要点（和 C 版逐行对比着看）：
//   1. std::vector 自己管 len + cap + realloc，你不用碰指针
//   2. 传参用 const std::vector<int>& —— & 避免拷贝，const 保证不改原数据
//   3. 排序用 std::sort，去重用 std::unique（必须先排序，且 unique 只搬移不删除）
//   4. 没有任何 free —— 析构函数在作用域结束时自动跑（RAII）
//
// 编译：g++ -std=c++17 -g -O0 -Wall -Wextra -static -o arr_cpp.exe arr.cpp
#include <algorithm>
#include <iostream>
#include <vector>

int main() {
    int n = 0;
    if (!(std::cin >> n) || n <= 0) return 0;

    std::vector<int> data;      // 不需要 reserve，push_back 会自己扩容
    data.reserve(static_cast<std::size_t>(n));   // 但知道总个数时 reserve 能少搬几次
    for (int i = 0; i < n; ++i) {
        int v = 0;
        if (!(std::cin >> v)) break;
        data.push_back(v);
    }

    const std::size_t count = data.size();       // erase 会改 size，先存下来

    long long sum = 0;
    for (int v : data) sum += v;                 // range-for，等价于 C 的 for (i=0..len)

    // minmax_element 一趟同时拿到最大和最小，比手写两个 if 更清晰
    // 注意：必须在 sort **之前**取值。迭代器跟着元素位置走，
    // sort 把元素搬走了，排序后再 *mx_it 读到的是别人 —— 这个坑我第一次就踩了。
    auto [mn_it, mx_it] = std::minmax_element(data.begin(), data.end());
    const int mn = *mn_it;
    const int mx = *mx_it;

    std::sort(data.begin(), data.end());
    auto last = std::unique(data.begin(), data.end());
    data.erase(last, data.end());                // unique 只是把不重复的把往前挪，得自己 erase

    std::cout << "count = " << count << "\n"
              << "sum   = " << sum << "\n"
              << "max   = " << mx << "\n"
              << "min   = " << mn << "\n"
              << "uniq  =";
    for (int v : data) std::cout << ' ' << v;
    std::cout << '\n';

    return 0;                                   // 没有 delete / free，vector 自己收尾
}
