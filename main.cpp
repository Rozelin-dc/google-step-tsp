#include <bits/stdc++.h>
using namespace std;
using ld = long double;

/** 座標の構造体 */
typedef struct {
  ld x;
  ld y;
} coordinate_t;

void greedySearch(unordered_map<int, coordinate_t> data, vector<int>& path);
void readInput(unordered_map<int, coordinate_t>& data, const string targetDataNum);
void outputCsv(const vector<int>& path, const string targetDataNum);

int main(int argc, char *argv[]) {
  string targetDataNum = argv[1];
  unordered_map<int, coordinate_t> data; // インデックスをキー座標を値に持つ
  vector<int> path = {0}; // 訪れる座標のインデックスが順に保存されている配列
  readInput(data, targetDataNum);
  greedySearch(data, path);
  outputCsv(path, targetDataNum);
}

/** 貪欲法で経路検索 */
void greedySearch(unordered_map<int, coordinate_t> data, vector<int>& path) {
  while (data.size() > 1) {
    ld distance; // 距離の二乗
    ld minDistance = -1; // 距離の二乗の最小値
    int minDistanceIndex = -1; // 最小の距離となる座標のインデックス
    int fromIndex = path[path.size() - 1];
    coordinate_t from = data.at(fromIndex);
    // 距離の二乗が最小となる座標を探す
    for (auto itr = data.begin(); itr != data.end(); ++itr) {
      if (itr->first == fromIndex) continue;

      coordinate_t to = itr->second;
      distance = ((from.x - to.x) * (from.x - to.x)) + ((from.y - to.y) * (from.y - to.y));

      if (minDistance == -1) {
        minDistance = distance;
        minDistanceIndex = itr->first;
        continue;
      }

      minDistance = std::min(minDistance, distance);
      if (minDistance == distance) minDistanceIndex = itr->first;
    }

    if (minDistanceIndex == -1) {
      cerr<<"error!"<<endl;
      exit(1);
    }

    // 距離の二乗が最小となる座標を path に追加し、from を data から消去
    path.push_back(minDistanceIndex);
    data.erase(fromIndex);
  }
  cout<<"finish search"<<endl;
  return;
}

/** 座標データの読み込み */
void readInput(unordered_map<int, coordinate_t>& data, const string targetDataNum) {
  std::ifstream ifs("./input_" + targetDataNum + ".csv");
  if (ifs.fail()) {
    cerr<<"Failed to open file."<<endl;
    return;
  }
  string str;
  int i = -1;
  while (std::getline(ifs, str)) {
    if (i == -1) {
      i++;
      continue;
    }
    auto index = str.find(',');
    auto x = str.substr(0, index);
    auto y = str.substr(index + 1, str.size() - x.size() - 1);
    data[i] = {(ld)std::stod(x), (ld)std::stod(y)};
    i++;
  }
  ifs.close();
  cout<<"finish read"<<endl;
  return;
}

/** 見つけた経路の書き出し */
void outputCsv(const vector<int>& path, const string targetDataNum) {
  std::ofstream ofs("./output_" + targetDataNum + ".csv");
  if (ofs.fail()) {
    cerr<<"Failed to open file."<<endl;
    return;
  }
  ofs<<"index"<<endl;
  for (int i = 0; i < (int)path.size(); i++) {
    ofs<<path[i]<<endl;
  }
  ofs.close();
  cout<<"finish output"<<endl;
  return;
}
