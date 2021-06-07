#include <bits/stdc++.h>
using namespace std;
using ld = long double;

/** 座標の構造体 */
typedef struct {
  ld x;
  ld y;
} coordinate_t;

void greedySearch(unordered_map<int, coordinate_t> data, vector<int>& path);
bool doTwoOpt(const unordered_map<int, coordinate_t>& data, vector<int>& path);
bool isPathCrossing(const coordinate_t from1, const coordinate_t to1, const coordinate_t from2, const coordinate_t to2);
void readInput(unordered_map<int, coordinate_t>& data, const string targetDataNum);
void outputCsv(const vector<int>& path, const string targetDataNum);

int main(int argc, char *argv[]) {
  string targetDataNum = argv[1];
  unordered_map<int, coordinate_t> data; // インデックスをキー、座標を値に持つ
  vector<int> path = {0}; // 訪れる座標のインデックスが順に保存されている配列
  readInput(data, targetDataNum);
  greedySearch(data, path);

  /* bool isTwoOptDone = true;
  int count = 0;
  while (isTwoOptDone && count < (int)(data.size() * 2)) {
    isTwoOptDone = doTwoOpt(data, path);
    count++;
  } */
  doTwoOpt(data, path);
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
  return;
}

/** 貪欲法で見つけた path に 2-opt をかける */
bool doTwoOpt(const unordered_map<int, coordinate_t>& data, vector<int>& path) {
  for (int i = 0; i < (int)path.size() - 1; i++) {
    coordinate_t from1 = data.at(path[i]);
    coordinate_t to1 = data.at(path[i + 1]);
    for (int j = i + 2; j < (int)path.size() - 1; j++) {
      coordinate_t from2 = data.at(path[j]);
      coordinate_t to2 = data.at(path[j + 1]);

      // パスがクロスしていたら path を再構築
      if (isPathCrossing(from1, to1, from2, to2)) {
        vector<int> newPath;
        std::copy(path.begin(), path.begin() + i + 1, back_inserter(newPath));
        newPath.push_back(path[j]);

        if (i + 2 != j) {
          vector<int> swapPath;
          std::copy(path.begin() + i + 2, path.begin() + j, back_inserter(swapPath));
          reverse(swapPath.begin(), swapPath.end());

          newPath.insert(newPath.end(), swapPath.begin(), swapPath.end());
        }

        newPath.push_back(path[i + 1]);
        newPath.push_back(path[j + 1]);

        std::copy(path.begin() + j + 2, path.end(), back_inserter(newPath));

        path = newPath;
        return true;
      }
    }
  }
  return false;
}

/** from1 と to1 を結ぶパスと from2 と to2 を結ぶパスがクロスしているかどうかを返す関数 */
bool isPathCrossing(const coordinate_t from1, const coordinate_t to1, const coordinate_t from2, const coordinate_t to2) {
  if (from1.x > from2.x && to1.x < to2.x) return true;
  if (from1.x < from2.x && to1.x > to2.x) return true;
  if (from1.y > from2.y && to1.y < to2.y) return true;
  if (from1.y < from2.y && to1.y > to2.y) return true;
  return false;
}

/** 座標データの読み込み */
void readInput(unordered_map<int, coordinate_t>& data, const string targetDataNum) {
  std::ifstream ifs("./input_" + targetDataNum + ".csv");
  if (ifs.fail()) {
    cerr<<"Failed to open file."<<endl;
    exit(1);
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
  return;
}

/** 見つけた経路の書き出し */
void outputCsv(const vector<int>& path, const string targetDataNum) {
  std::ofstream ofs("./output_" + targetDataNum + ".csv");
  if (ofs.fail()) {
    cerr<<"Failed to open file."<<endl;
    exit(1);
  }
  ofs<<"index"<<endl;
  for (int i = 0; i < (int)path.size(); i++) {
    ofs<<path[i]<<endl;
  }
  ofs.close();
  return;
}
