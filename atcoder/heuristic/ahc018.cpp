/*

問題 : https://atcoder.jp/contests/ahc018/tasks/ahc018_a

コンテスト中にやったこと
2/22
水源と家を頂点として見て最小全域木を作成、ただし水源同士の辺は追加しように処理した
移動方法はサンプルコードを流用した
得点 : 15083340

2/23
最小全域木上の辺をそのまま移動するため、硬い岩盤に当たった時に体力を無駄に消耗する
移動方法を指定(目的地に対してx軸, y軸の10マス先を指定した体力で掘り、先に削り切ることができた方向に移動する)
また、頂点間の移動に対して中継地点を計算することで長い辺でも硬い岩盤を避けるようにしようと試みた
得点 : 9950147

2/25-26
あらかじめ数カ所削っておくことでグリッドのどこが固いのか、柔らかいのかを推測したい
水源または家の周辺を削って見て、1回で削り切れるような箇所を新たな頂点として見た
その上で最小全域木＋頂点間の距離がある程度近い場合、辺を張ることで新たな全域グラフを作成
グラフ上で水源から最も近い家を求め、その経路をダイクストラ法で計算
家に水が供給されればその家は水源と見做せるため、同様の処理を水が供給されていない家が0になるまで繰り返した

得点 : 8137732

岩盤がとても柔らかい場合、使用する体力を指定したよりも少なくなるようにした

得点 : 7697583

最終順位 : 285 
最終得点 : 526307380

*/

#include <bits/stdc++.h>
using namespace std;
#define INF ((1LL<<62)-(1LL<<31))
typedef long long ll;
typedef pair<ll,ll> pl;

#define EPS (1e-10)
#define equals(a,b) (fabs((a)-(b))<EPS)

const double z = sqrt(1.0/2.0);
const double dy[] = {0, 0, 1, -1, z, z, -z, -z};
const double dx[] = {1, -1, 0, 0, z, -z, z, -z};

struct UnionFind {
    vector<ll> v;
    UnionFind (ll n=0): v(n, -1) {}
    int find (ll x) {
        if (v[x] < 0) return x;
        return v[x] = find(v[x]);
    }
    bool unite (ll x, ll y) {
        x = find(x);
        y = find(y);
        if (x == y) return false;
        if (v[x] > v[y]) swap(x, y);
        v[x] += v[y];
        v[y] = x;
        return true;
    }
    bool same (ll x, ll y) { return find(x) == find(y); }
    ll size (ll x) { return -v[find(x)]; }
};

struct Pos {
    int y, x;
    bool operator == (const Pos &p) const { return x == p.x && y == p.y; }
};

enum class Response {
    not_broken, broken, finish, invalid
};

struct Field {
    int N;
    int C;
    vector<vector<bool> > is_broken;
    vector<vector<int> > is_checked;
    vector<vector<int> > node_id;
    vector<vector<int> > sturdiness;
    int total_cost;

    Field(int N, int C) : 
        N(N), 
        C(C), 
        is_broken(N, vector<bool> (N, 0)), 
        is_checked(N, vector<int> (N, 0)), 
        node_id(N,vector<int> (N, -1)),
        sturdiness(N, vector<int> (N, 0)), 
        total_cost(0) {}

    Response query(int y, int x, int power) {
        total_cost += power + C;
        cout << y << " " << x << " " << power << endl; // endl does flush
        int r;
        cin >> r;
        switch (r) {
        case 0:
            return Response::not_broken;
        case 1:
            is_broken[y][x] = 1;
            return Response::broken;
        case 2:
            is_broken[y][x] = 1;
            return Response::finish;
        default:
            return Response::invalid;
        }
    }
};

struct Solver {
    int N;
    vector<Pos> source_pos;
    vector<Pos> house_pos;
    int C;
    Field field;

    Solver(int N, const vector<Pos>& source_pos, const vector<Pos>& house_pos, int C) : 
        N(N), source_pos(source_pos), house_pos(house_pos), C(C), field(N, C) {
    }

    void solve() {
        vector<Pos> object;
        for (Pos sp: source_pos) object.push_back(sp);
        for (Pos hp: house_pos) object.push_back(hp);

        vector<pair<Pos, Pos> > FromTo;
        if ((int)house_pos.size() <= 2) FromTo = make_MST(object);
        else FromTo = make_graph(object);
        
        for (int i = 0; i < (int)FromTo.size(); i++) {
            Pos from = FromTo[i].first;
            Pos to = FromTo[i].second;
            //Pos from = FromTo[(int)FromTo.size()-i-1].first;
            //Pos to = FromTo[(int)FromTo.size()-i-1].second;
            bool is_digged = false;
            for (int j = 0; j < i; j++) {
                if (from == FromTo[j].first && to == FromTo[j].second) is_digged = true;
                if (to == FromTo[j].first && from == FromTo[j].second) is_digged = true; 
            }
            if (!is_digged) move(from, to);
        }

        // should receive Response::finish and exit before entering here
        assert(false);
    }

    void move(Pos start, Pos goal) {
        // you can output comment
        cout << "# move from (" << start.y << "," << start.x << ") to (" << goal.y << "," << goal.x << ")" << endl;

        Pos now = start;
        destruct_decided(now.y, now.x, now.y, now.x);

        while (now.y != goal.y || now.x != goal.x) { 
            int dy = 0;
            int dx = 0;
            // down or up
            if (now.y < goal.y) {
                dy = min(10, goal.y - now.y);
            } else {
                dy = max(-10, goal.y - now.y);
            }

            // right or left
            if (start.x < goal.x) {
                dx = min(10, goal.x - now.x);
            } else {
                dx = max(-10, goal.x - now.x);
            }
            //cerr << dy << " " << dx << endl;

            if (dx == 0) {
                if (true /*abs(goal.y - now.y) <= 10*/) {
                    if (dy > 0) {
                        destruct_decided(now.y+1, now.x, now.y, now.x);
                        now.y += 1;
                    } else {
                        destruct_decided(now.y-1, now.x, now.y, now.x);
                        now.y -= 1;
                    }
                } /*else {
                    int ddy[] = {11, 5, 5};
                    int ddx[] = {0, -5, 5};
                    if (dy > 0) {
                        while(true) {
                            bool is_moved = false;
                            for (int k = 0; k < 3; k++) {
                                int ny = now.y + ddy[k], nx = now.x + ddx[k];
                                if (0 > ny || ny >= N || 0 > nx || nx >= N) continue;
                                if (field.is_broken[ny][nx] || destruct_check(ny, nx) == Response::broken) {
                                    if (k == 1) {
                                        for (int y = now.y, x = now.x; y <= ny && x >= nx; y++, x--) {
                                            destruct_decided(y, x);
                                            if (x-1 >= nx) destruct_decided(y, x-1);
                                        }
                                    } else if (k == 2) {
                                        for (int y = now.y, x = now.x; y <= ny && x <= nx; y++, x++) {
                                            destruct_decided(y, x);
                                            if (x+1 <= nx) destruct_decided(y, x+1);
                                        }
                                    } else {
                                        for (int y = now.y; y <= ny; y++) destruct_decided(y, now.x);
                                    }
                                    now.y = ny, now.x = nx;
                                    is_moved = true;
                                    break;
                                }
                            }
                            if (is_moved) break;
                        }
                    } else {
                        while(true) {
                            bool is_moved = false;
                            for (int k = 0; k < 3; k++) {
                                int ny = now.y - ddy[k], nx = now.x + ddx[k];
                                if (0 > ny || ny >= N || 0 > nx || nx >= N) continue;
                                if (field.is_broken[ny][nx] || destruct_check(ny, nx) == Response::broken) {
                                    if (k == 1) {
                                        for (int y = now.y, x = now.x; y >= ny && x >= nx; y--, x--) {
                                            destruct_decided(y, x);
                                            if (x-1 >= nx) destruct_decided(y, x-1);
                                        }
                                    } else if (k == 2) {
                                        for (int y = now.y, x = now.x; y >= ny && x <= nx; y--, x++) {
                                            destruct_decided(y, x);
                                            if (x+1 <= nx) destruct_decided(y, x+1);
                                        }
                                    } else {
                                        for (int y = now.y; y >= ny; y--) destruct_decided(y, now.x);
                                    }
                                    now.y = ny, now.x = nx;
                                    is_moved = true;
                                    break;
                                }
                            }
                            if (is_moved) break;
                        }
                    }
                }*/
            } else if (dy == 0) {
                if (true /*abs(goal.x - now.x) <= 10*/) {
                    if (dx > 0) {
                        destruct_decided(now.y, now.x+1, now.y, now.x);
                        now.x += 1;
                    } else {
                        destruct_decided(now.y, now.x-1, now.y, now.x);
                        now.x -= 1;
                    }
                } /*else {
                    int ddy[] = {0, -5, 5};
                    int ddx[] = {11, 5, 5};
                    if (dx > 0) {
                        while(true) {
                            bool is_moved = false;
                            for (int k = 0; k < 3; k++) {
                                int ny = now.y + ddy[k], nx = now.x + ddx[k];
                                if (0 > ny || ny >= N || 0 > nx || nx >= N) continue;
                                if (field.is_broken[ny][nx] || destruct_check(ny, nx) == Response::broken) {
                                    if (k == 1) {
                                        for (int y = now.y, x = now.x; y >= ny && x <= nx; y--, x++) {
                                            destruct_decided(y, x);
                                            if (x+1 <= nx) destruct_decided(y, x+1);
                                        }
                                    } else if (k == 2) {
                                        for (int y = now.y, x = now.x; y <= ny && x <= nx; y++, x++) {
                                            destruct_decided(y, x);
                                            if (x+1 <= nx) destruct_decided(y, x+1);
                                        }
                                    } else {
                                        for (int x = now.x; x <= nx; x++) destruct_decided(now.y, x);
                                    }
                                    now.y = ny, now.x = nx;
                                    is_moved = true;
                                    break;
                                }
                            }
                            if (is_moved) break;
                        }
                    } else {
                        while(true) {
                            bool is_moved = false;
                            for (int k = 0; k < 3; k++) {
                                int ny = now.y + ddy[k], nx = now.x - ddx[k];
                                if (0 > ny || ny >= N || 0 > nx || nx >= N) continue;
                                if (field.is_broken[ny][nx] || destruct_check(ny, nx) == Response::broken) {
                                    if (k == 1) {
                                        for (int y = now.y, x = now.x; y >= ny && x >= nx; y--, x--) {
                                            destruct_decided(y, x);
                                            if (x-1 >= nx) destruct_decided(y, x-1);
                                        }
                                    } else if (k == 2) {
                                        for (int y = now.y, x = now.x; y <= ny && x >= nx; y++, x--) {
                                            destruct_decided(y, x);
                                            if (x-1 >= nx) destruct_decided(y, x-1);
                                        }
                                    } else {
                                        for (int x = now.x; x >= nx; x--) destruct_decided(now.y, x);
                                    }
                                    now.y = ny, now.x = nx;
                                    is_moved = true;
                                    break;
                                }
                            }
                            if (is_moved) break;
                        }
                    }
                }*/
            } else {
                int cx = now.x, cy = now.y;
                int nx = cx + dx, ny = cy + dy;
                while (true) {
                    if (field.is_broken[ny][cx] || destruct_check(ny, cx) == Response::broken) {
                        if (cy < ny) {
                            for (int y = cy; y <= ny; y++) destruct_decided(y, cx, max(y-1, cy), cx);
                        } else {
                            for (int y = cy; y >= ny; y--) destruct_decided(y, cx, min(y+1, cy), cx);
                        }
                        now.y = ny;
                        break;
                    }
                    if (field.is_broken[cy][nx] || destruct_check(cy, nx) == Response::broken) {
                        if (cx < nx) {
                            for (int x = cx; x <= nx; x++) destruct_decided(cy, x, cy, max(x-1, cx));
                        } else {
                            for (int x = cx; x >= nx; x--) destruct_decided(cy, x, cy, min(x+1, cx));
                        }
                        now.x = nx;
                        break;
                    }
                }
            }
        }
    }

    void destruct_decided(int y, int x, int py, int px) {
        // excavate (y, x) with fixed power until destruction
        int power = 82;
        if (y != py || x != px) power = max(1, min(field.sturdiness[py][px] - 6, power));
        while (!field.is_broken[y][x]) {
            Response result = field.query(y, x, power);
            field.sturdiness[y][x] += power;
            if (result == Response::finish) {
                //cerr << "total_cost=" << field.total_cost << endl;
                exit(0);
            } else if (result == Response::invalid) {
                cerr << "invalid: y=" << y << " x=" << x << endl;
                exit(1);
            }
        }
    }

    Response destruct_check(int y, int x) {
        // excavate (y, x) with power for check
        const int power = 150;
        if (field.is_broken[y][x]) return Response::broken;
        Response result = field.query(y, x, power);
        field.sturdiness[y][x] += power;
        field.is_checked[y][x] += 1;
        if (result == Response::finish) {
            //cerr << "total_cost=" << field.total_cost << endl;
            exit(0);
        } else if (result == Response::invalid) {
            cerr << "invalid: y=" << y << " x=" << x << endl;
            exit(1);
        }
        return result;
    }

    double calc_dist(Pos p1, Pos p2) {
        return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
    }

    double calc_dist_m(Pos p1, Pos p2) {
        return abs(p1.x - p2.x) + abs(p1.y - p2.y);
    }

    Pos make_relaypoint(Pos p1, Pos p2) {
        Pos midpoint;
        midpoint.x = (p1.x + p2.x) / 2;
        midpoint.y = (p1.y + p2.y) / 2;

        double a = 1.0;
        if (p1.x != p2.x) a = (p1.y - p2.y) / (p1.x - p2.x);
        
        const double dist = 15.0;
        Pos leftpoint, rightpoint;
        leftpoint.x = -a * dist / sqrt(a*a + 1) + midpoint.x;
        leftpoint.y = dist / sqrt(a*a + 1) + midpoint.y;
        rightpoint.x = a * dist / sqrt(a*a + 1) + midpoint.x;
        rightpoint.y = -dist / sqrt(a*a + 1) + midpoint.y;

        /*
        cerr << "(" << leftpoint.x << "," << leftpoint.y << "),(";
        cerr << midpoint.x << "," << midpoint.y << "),(";
        cerr << rightpoint.x << "," << rightpoint.y << ")" << endl;
        */

        vector<Pos> Points;
        Points.push_back(midpoint);
        if (0 <= leftpoint.x && leftpoint.x < N 
            && 0 <= leftpoint.y && leftpoint.y < N) Points.push_back(leftpoint);
        if (0 <= rightpoint.x && rightpoint.x < N 
            && 0 <= rightpoint.y && rightpoint.y < N) Points.push_back(rightpoint);

        int Pointidx = -1;
        while(Pointidx == -1) {
            for (int i = 0; i < (int)Points.size(); i++) {
                int x = Points[i].x;
                int y = Points[i].y;
                if (field.is_broken[y][x]) {
                    Pointidx = i;
                    break;
                }
                if (destruct_check(y, x) == Response::broken) {
                    Pointidx = i;
                    break;
                }
            }
        }

        return Points[Pointidx];
    }

    vector<pair<Pos, Pos> > make_MST(vector<Pos>& object) {
        int W = (int)source_pos.size();
        int M = (int)object.size();
        vector<pair<double, pair<int, int> > > edge;
        for (int i = 0; i < M; i++) {
            for (int j = i+1; j < M; j++) {
                if (i < W && j < W) continue;
                edge.push_back(make_pair(calc_dist(object[i], object[j]), make_pair(i, j)));
            }
        }
        sort(edge.begin(), edge.end());
        
        UnionFind uf(M);
        vector<pair<Pos, Pos> > MST_edge;
        for (int i = 0; i < (int)edge.size(); i++) {
            int u = edge[i].second.first;
            int v = edge[i].second.second;
            if (!uf.same(u, v)) {
                uf.unite(u, v);
                int dist_m = calc_dist_m(object[u], object[v]);
                
                if (dist_m <= 50) {
                    MST_edge.push_back(make_pair(object[u], object[v]));
                    continue;
                }

                Pos rp = make_relaypoint(object[u], object[v]);

                if (dist_m <= 100) {
                    MST_edge.push_back(make_pair(object[u], rp));
                    MST_edge.push_back(make_pair(rp, object[v]));
                    continue;
                }

                Pos rp2 = make_relaypoint(object[u], rp);
                Pos rp3 = make_relaypoint(rp, object[v]);
                MST_edge.push_back(make_pair(object[u], rp2));
                //MST_edge.push_back({rp2, rp});
                MST_edge.push_back(make_pair(rp2, rp3));
                //MST_edge.push_back({rp, rp3});
                MST_edge.push_back(make_pair(rp3, object[v]));
            }
        }
        
        return MST_edge;
    }

    vector<pair<Pos, Pos> > make_graph(vector<Pos>& object) {
        int M = (int)object.size();
        vector<Pos> node;
        for (int i = 0; i < M; i++) {
            node.push_back(object[i]);
            field.node_id[object[i].y][object[i].x] = i;
        } 

        const double dist = 20.0;
        int node_cnt = 0;
        while (node_cnt < (int)node.size()) {
            for (int i = 0; i < 8; i++) {
                int ny = floor(node[node_cnt].y + dy[i]*dist);
                int nx = floor(node[node_cnt].x + dx[i]*dist);
                if (ny < 0 || ny >= N || nx < 0 || nx >= N) continue;
                if (field.is_checked[ny][nx]) continue;
                bool is_isolated = true;
                for (int y = max(0, int(ny-dist*0.75)); y < min(N, int(ny+dist*0.75)); y++) {
                    for (int x = max(0, int(nx-dist*0.75)); x < min(N, int(nx+dist*0.75)); x++) {
                        if (field.node_id[y][x] != -1) is_isolated = false;
                    }
                }
                if (!is_isolated) continue;
                if (destruct_check(ny, nx) != Response::broken) continue;
                Pos nd = {ny, nx};
                node.push_back(nd);
                field.node_id[ny][nx] = (int)node.size() - 1;
                //cerr << "(" << ny << "," << nx << ")";
            }
            //cerr << endl;
            node_cnt += 1;
        }

        vector<pair<Pos, Pos> > graph_edge;
        
        int W = (int)source_pos.size();
        vector<pair<double, pair<int, int> > > edge;
        for (int i = 0; i < node_cnt; i++) {
            for (int j = i+1; j < node_cnt; j++) {
                if (i < W && j < W) continue;
                edge.push_back(make_pair(calc_dist_m(node[i], node[j]), make_pair(i, j)));
            }
        }
        sort(edge.begin(), edge.end());        
        
        UnionFind uf(node_cnt);
        vector<vector<int> > graph(node_cnt, vector<int> ());
        for (int i = 0; i < (int)edge.size(); i++) {
            int u = edge[i].second.first;
            int v = edge[i].second.second;
            if (!uf.same(u, v)) {
                uf.unite(u, v);
                graph[u].push_back(v);
                graph[v].push_back(u);
                //graph_edge.push_back({node[u], node[v]});
            }
        }

        for (int i = 0; i < node_cnt; i++) {
            int cy = node[i].y;
            int cx = node[i].x;
            const double delta = dist + 10.0; 
            for (int y = max(0, int(cy-delta)); y < min(N, int(cy+delta)); y++) {
                for (int x = max(0, int(cx-delta)); x < min(N, int(cx+delta)); x++) {
                    if (cy == y && cx == x) continue;
                    if (field.node_id[y][x] != -1) {
                        graph[i].push_back(field.node_id[y][x]);
                        graph[field.node_id[y][x]].push_back(i);
                        //graph_edge.push_back({node[i], node[field.node_id[y][x]]});
                    }
                }
            }
        }
        
        int connect_cnt = 0;
        vector<bool> has_water(M, false);
        for (int i = 0; i < W; i++) has_water[i] = true;

        while (connect_cnt < (int)house_pos.size()) {
            vector<pair<int, pair<int, int> > > object_dist;

            for (int os = 0; os < M; os++) {
                if (has_water[os]) continue;
                for (int og = 0; og < M; og++) {
                    if (!has_water[og]) continue;
                    object_dist.push_back(make_pair(calc_dist_m(node[os], node[og]), make_pair(os, og)));
                } 
            }

            sort(object_dist.begin() , object_dist.end());

            int from = object_dist[0].second.first;
            int to = object_dist[0].second.second;

            vector<int> cost(node_cnt, N*N);
            cost[from] = 0;
            priority_queue<pair<int, int>, vector<pair<int, int> >, greater<pair<int, int> > > heap;
            heap.push(make_pair(0, from)); 

            while(!heap.empty()) {
                int cd = heap.top().first;
                int cv = heap.top().second;
                heap.pop();
                if (cd > cost[cv]) continue;
                for (int nv: graph[cv]) {
                    int nd = cd + calc_dist_m(node[cv], node[nv]);
                    if (cost[nv] <= nd) continue;
                    cost[nv] = nd;
                    heap.push(make_pair(nd, nv));
                } 
            }
                    
            int cv = to;
            while(cv != from) {
                for (int nv: graph[cv]) {
                    if (cost[nv] + calc_dist_m(node[cv], node[nv]) == cost[cv]) {
                        graph_edge.push_back(make_pair(node[cv], node[nv]));
                        cv = nv;
                        break;
                    }
                }
            }
            
            has_water[from] = true;
            connect_cnt += 1;
        }

        return graph_edge;
    }
};

int main() {
    int N, W, K, C;
    cin >> N >> W >> K >> C;
    vector<Pos> source_pos(W);
    vector<Pos> house_pos(K);
    for (int i = 0; i < W; i++) {
        cin >> source_pos[i].y >> source_pos[i].x;
    }
    for (int i = 0; i < K; i++) {
        cin >> house_pos[i].y >> house_pos[i].x;
    }

    Solver solver(N, source_pos, house_pos, C);
    solver.solve();
}