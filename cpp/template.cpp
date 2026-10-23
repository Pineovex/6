/*
 * template.cpp —— 算法竞赛模板骨架
 *
 * 用法：每场比赛开场先复制这个文件，删掉用不到的部分，再开始写题。
 * 编译：g++ -std=c++17 -O2 -Wall -Wextra -o a.exe template.cpp
 * 本地调试可以加 -DLOCAL 打开 dbg 输出：g++ -DLOCAL ...
 *
 * 约定：数组从 0 开始，图用 vector<vector<Edge>>，答案用 ll。
 * 宁可多写 typedef，也不要在赛场上纠结类型溢出。
 */

#include <bits/stdc++.h>
using namespace std;

typedef long long ll;
typedef unsigned long long ull;
typedef pair<int, int> pii;
typedef pair<ll, ll> pll;

#define rep(i, a, b) for (int i = (a); i < (b); ++i)
#define rrep(i, a, b) for (int i = (b) - 1; i >= (a); --i)
#define all(x) (x).begin(), (x).end()
#define sz(x) (int)((x).size())
#define pb push_back
#define eb emplace_back
#define fi first
#define se second

#ifdef LOCAL
#define dbg(x) cerr << "[" << #x << "] = " << (x) << "\n"
#else
#define dbg(x) ((void)0)
#endif

const ll INF = numeric_limits<ll>::max() / 4;
const int MOD = 1000000007;

/* ---------------- 快读（数据量大时用，比 cin 快 3~5 倍） ---------------- */

static char ibuf[1 << 20];
static char *ip = ibuf, *ie = ibuf;

static inline int read_ch(void)
{
    if (ip == ie) {
        ie = ibuf + fread(ibuf, 1, sizeof(ibuf), stdin);
        ip = ibuf;
        if (ip == ie) {
            return -1;
        }
    }
    return *ip++;
}

template <class T>
static inline bool read_int(T &out)
{
    int c = read_ch();
    T sign = 1;
    T num = 0;
    while (c != -1 && c != '-' && (c < '0' || c > '9')) {
        c = read_ch();
    }
    if (c == -1) {
        return false;
    }
    if (c == '-') {
        sign = -1;
        c = read_ch();
    }
    while (c >= '0' && c <= '9') {
        num = num * 10 + (c - '0');
        c = read_ch();
    }
    out = num * sign;
    return true;
}

/* ---------------- 数学 ---------------- */

ll gcd_ll(ll a, ll b)
{
    if (a < 0) a = -a;
    if (b < 0) b = -b;
    while (b) {
        ll t = a % b;
        a = b;
        b = t;
    }
    return a;
}

ll lcm_ll(ll a, ll b)
{
    if (a == 0 || b == 0) return 0;
    return a / gcd_ll(a, b) * b;
}

/* 快速幂：qpow(a, b, mod)，注意 b 是非负整数 */
ll qpow(ll a, ll b, ll mod)
{
    ll r = 1 % mod;
    a %= mod;
    while (b) {
        if (b & 1) r = r * a % mod;
        a = a * a % mod;
        b >>= 1;
    }
    return r;
}

/* 线性筛：返回 n 以内的所有素数 */
vector<int> sieve(int n)
{
    vector<int> primes;
    vector<char> comp(n + 1, 0);
    for (int i = 2; i <= n; ++i) {
        if (!comp[i]) primes.pb(i);
        for (int p : primes) {
            if (1LL * i * p > n) break;
            comp[i * p] = 1;
            if (i % p == 0) break;
        }
    }
    return primes;
}

/* ---------------- 并查集（按秩合并 + 路径压缩） ---------------- */

struct DSU {
    vector<int> parent, rank_;
    explicit DSU(int n) : parent(n), rank_(n, 0) { iota(all(parent), 0); }

    int find(int x)
    {
        while (parent[x] != x) {
            parent[x] = parent[parent[x]];
            x = parent[x];
        }
        return x;
    }

    bool unite(int a, int b)
    {
        a = find(a);
        b = find(b);
        if (a == b) return false;
        if (rank_[a] < rank_[b]) swap(a, b);
        parent[b] = a;
        if (rank_[a] == rank_[b]) ++rank_[a];
        return true;
    }
};

/* ---------------- 图：邻接表 + 堆优化 Dijkstra ---------------- */

struct Edge {
    int to;
    ll w;
};

vector<ll> dijkstra(int n, const vector<vector<Edge>> &g, int src)
{
    vector<ll> dist(n, INF);
    priority_queue<pair<ll, int>, vector<pair<ll, int>>, greater<pair<ll, int>>> pq;
    dist[src] = 0;
    pq.push({0, src});
    while (!pq.empty()) {
        pair<ll, int> top = pq.top();
        pq.pop();
        ll d = top.fi;
        int u = top.se;
        if (d != dist[u]) continue;
        for (const Edge &e : g[u]) {
            if (dist[u] + e.w < dist[e.to]) {
                dist[e.to] = dist[u] + e.w;
                pq.push({dist[e.to], e.to});
            }
        }
    }
    return dist;
}

/* ---------------- 常用工具 ---------------- */

/* 二分查找：在有序 vector 里找第一个 >= x 的位置（手写版，便于改条件） */
int lower_bound_idx(const vector<int> &v, int x)
{
    int lo = 0, hi = sz(v);
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (v[mid] < x) lo = mid + 1;
        else hi = mid;
    }
    return lo;
}

/* 众数 / 频率 */
int mode_of(const vector<int> &v)
{
    map<int, int> cnt;
    for (int x : v) ++cnt[x];
    int best = 0, ans = 0;
    for (auto &kv : cnt) {
        if (kv.se > best) {
            best = kv.se;
            ans = kv.fi;
        }
    }
    return ans;
}

/* ---------------- 主函数 ---------------- */

int main()
{
    /* 用快读时不要再用 cin；用 cin 时注释掉下面这段改用 cin.tie 加速 */
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    ll a = 0, b = 0;
    if (read_int(a) && read_int(b)) {
        cout << a + b << "\n";
    }
    return 0;
}
