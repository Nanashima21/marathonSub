/*

問題 : https://atcoder.jp/contests/ahc021/tasks/ahc021_a

コンテスト中にやったこと

ピラミッドをより少ない手数で完成させたい
とりあえずバブルソートと同じ方法で隣接しているボールを比較してswapすることを完成するまで繰り返した

得点 : 13023875

ここから何もできず...
隣同士の乱択swapを導入したりしてみたが大幅に改善することはなかった

パラメーターガチャして終了

最終順位 : 393
最終得点 : 13073725

感想 : ヒープで考える方法を思いつくことができなかったのはとても悔しい

*/

#include <bits/stdc++.h>
using namespace std;
#define rep(i,n) for(int i=0; i<(n); i++)
#define INF ((1LL<<62)-(1LL<<31))
#define all(a) (a).begin(),(a).end()
#define rall(a) (a).rbegin(),(a).rend()
typedef long long ll;
typedef pair<ll,ll> pl;

int dx[] = {-1, -1, 0, 0, 1, 1};
int dy[] = {-1, 0, -1, 1, 0, 1};

const double Limit_time=1.95*CLOCKS_PER_SEC;

const int N = 30;
const int M = 465;
const int lim = 10000;
vector<vector<int>> ball(N, vector<int> ());
vector<pair<int,int>> place(M);
vector<pair<pair<int,int>,pair<int,int>>> out(lim);

int Randint(int a,int b) {
    return a+rand()%(b-a+1);
}

double Randdouble() {
    return 1.0*rand()/RAND_MAX;
}

void output() {
    cout << (int)out.size() << endl;
    for(auto u:out) cout << u.first.first << " " << u.first.second << " " << u.second.first << " " << u.second.second << endl;
}

bool outpush(pair<int,int> p1,pair<int,int> p2,vector<pair<pair<int,int>,pair<int,int>>> &cur) {
    cur.push_back({p1,p2});
    if((int)cur.size()==lim) return false;
    return true;
}

int main() {
    srand(time(NULL));
    rep(i,N) {
        for(int j=0;j<i+1;j++) {
            int b;
            cin >> b;
            ball[i].push_back(b);
            place[b]={i,j};
        }
    }

    while(true) {
        if(clock()>Limit_time) break;
        vector<pair<pair<int,int>,pair<int,int>>> curout;
        vector<vector<int>> curball=ball;
        vector<pair<int,int>> curplace=place;
        rep(_,10) {
            int x=Randint(5, 28);
            int y=Randint(0, x-2);
            int l=curball[x][y];
            int r=curball[x][y+1];
            if(outpush(curplace[l],curplace[r],curout)) {
                swap(curball[x][y],curball[x][y+1]);
                swap(curplace[l],curplace[r]);
            }
        }
        bool flag=true;
        while(flag) {
            flag=false;
            for(int i=N-2;i>=0;i--) {
                for(int j=0;j<i+1;j++) {
                    int k=j;
                    if(i%2==0) j=i-j;
                    int cur=curball[i][j];
                    int l=curball[i+1][j];
                    int r=curball[i+1][j+1];
                    if(cur>l&&cur>r) {
                        if(l>r) {
                            if(outpush(curplace[cur],curplace[r],curout)) {
                                flag=true;
                                swap(curball[i][j],curball[i+1][j+1]);
                                swap(curplace[cur],curplace[r]);
                            }
                        } else {
                            if(outpush(curplace[cur],curplace[l],curout)) {
                                flag=true;
                                swap(curball[i][j],curball[i+1][j]);
                                swap(curplace[cur],curplace[l]);
                            }
                        }
                    } else if(cur>l) {
                        if(i%2==0&&j!=0) {
                            int nei=curball[i][j-1];
                            if(nei>cur&&nei>r) {
                                if(outpush(curplace[nei],curplace[l],curout)) {
                                    flag=true;
                                    swap(curball[i][j-1],curball[i+1][j]);
                                    swap(curplace[nei],curplace[l]);
                                }
                            } else {
                                if(outpush(curplace[cur],curplace[l],curout)) {
                                    flag=true;
                                    swap(curball[i][j],curball[i+1][j]);
                                    swap(curplace[cur],curplace[l]);
                                }
                            }
                        } else {
                            if(outpush(curplace[cur],curplace[l],curout)) {
                                flag=true;
                                swap(curball[i][j],curball[i+1][j]);
                                swap(curplace[cur],curplace[l]);
                            }
                        }
                    } else if(cur>r) {
                        if(i%2==1&&j!=i) {
                            int nei=curball[i][j+1];
                            if(nei>cur&&nei>r) {
                                if(outpush(curplace[nei],curplace[r],curout)) {
                                    flag=true;
                                    swap(curball[i][j+1],curball[i+1][j+1]);
                                    swap(curplace[nei],curplace[r]);
                                }
                            } else {
                                if(outpush(curplace[cur],curplace[r],curout)) {
                                    flag=true;
                                    swap(curball[i][j],curball[i+1][j+1]);
                                    swap(curplace[cur],curplace[r]);
                                }
                            }
                        } else {
                            if(outpush(curplace[cur],curplace[r],curout)) {
                                flag=true;
                                swap(curball[i][j],curball[i+1][j+1]);
                                swap(curplace[cur],curplace[r]);
                            }
                        }
                    } 
                    j=k;
                }
            }
        }
        if((int)curout.size()<(int)out.size()) swap(curout,out);
        // rep(i,N-1) {
        //     for(int j=0;j<i+1;j++) {
        //         int cur=curball[i][j];
        //         int l=curball[i+1][j];
        //         int r=curball[i+1][j+1];
        //         if(cur>l) cout << cur << " " << l << endl;
        //         if(cur>r) cout << cur << " " << r << endl;
        //         if(curball[i][j]>curball[i+1][j]) cout << place[cur].first << " " << place[cur].second << " " << place[l].first << " " << place[l].second << endl;
        //         if(curball[i][j]>curball[i+1][j+1]) cout << place[cur].first << " " << place[cur].second << " " << place[r].first << " " << place[r].second << endl;
        //     }
        // }
    }

    output();

    // 959100
    return 0;
}