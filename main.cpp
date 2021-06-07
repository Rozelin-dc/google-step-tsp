#include <bits/stdc++.h>
using namespace std;
using ld = long double;

/** 座標の構造体 */
typedef struct {
  int index;
  ld x;
  ld y;
} coordinate_t;

ld calculateDistance(const coordinate_t from, const coordinate_t to);
void greedySearch(unordered_map<int, coordinate_t> data, vector<int>& path);
bool doTwoOpt(const unordered_map<int, coordinate_t>& data, vector<int>& path);
bool swapFivePoint(const unordered_map<int, coordinate_t>& data, vector<int>& path, const int index);
bool isPathCrossing(const coordinate_t from1, const coordinate_t to1, const coordinate_t from2, const coordinate_t to2);
void readInput(unordered_map<int, coordinate_t>& data, const string targetDataNum);
void outputCsv(const vector<int>& path, const string targetDataNum);

int main(int argc, char *argv[]) {
  if (argc == 1) {
    cerr<<"Please input target data number."<<endl;
    return 1;
  }
  string targetDataNum = argv[1];

  unordered_map<int, coordinate_t> data; // インデックスをキー、座標を値に持つ
  vector<int> path = {0}; // 訪れる座標のインデックスが順に保存されている配列
  readInput(data, targetDataNum);
  greedySearch(data, path);

  default_random_engine eng((int)time(NULL));
  uniform_int_distribution<int> distr(0, (int)data.size() - 5);

  bool isTwoOptDone = true;
  bool isSwapFourPointDone = true;
  int count = 0;
  while ((isTwoOptDone || isSwapFourPointDone) && count < (int)(data.size() * 2)) {
    isTwoOptDone = doTwoOpt(data, path);
    isSwapFourPointDone = swapFivePoint(data, path, distr(eng));
    count++;
  }

  outputCsv(path, targetDataNum);
}

/** from から to までの距離の二乗を計算 */
ld calculateDistance(const coordinate_t from, const coordinate_t to) {
  return ((from.x - to.x) * (from.x - to.x)) + ((from.y - to.y) * (from.y - to.y));
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
      distance = calculateDistance(from, to);

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

/** path に 2-opt をかける */
bool doTwoOpt(const unordered_map<int, coordinate_t>& data, vector<int>& path) {
  for (int i = 0; i < (int)path.size() - 1; i++) {
    coordinate_t from1 = data.at(path[i]);
    coordinate_t to1 = data.at(path[i + 1]);
    for (int j = i + 2; j < (int)path.size(); j++) {
      coordinate_t from2 = data.at(path[j]);
      coordinate_t to2;

      if (i == 0 && j + 1 == (int)path.size()) break;
      if (j + 1 == (int)path.size()) to2 = data.at(path.front());
      else to2 = data.at(path[j + 1]);

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
        if (to2.index != 0) newPath.push_back(path[j + 1]);

        std::copy(path.begin() + j + 2, path.end(), back_inserter(newPath));

        path = newPath;
        return true;
      }
    }
  }
  return false;
}

/** 5 点を結ぶパスについて最適化 */
bool swapFivePoint(const unordered_map<int, coordinate_t>& data, vector<int>& path, const int index) {
  coordinate_t first = data.at(path[index]);
  coordinate_t second = data.at(path[index + 1]);
  coordinate_t third = data.at(path[index + 2]);
  coordinate_t forth = data.at(path[index + 3]);
  coordinate_t fifth = data.at(path[index + 4]);

  ld currentDistance = calculateDistance(first, second) + calculateDistance(second, third) + calculateDistance(third, forth) + calculateDistance(forth, fifth);
  ld anotherDistance = calculateDistance(first, third) + calculateDistance(third, forth) + calculateDistance(forth, second) + calculateDistance(second, fifth);

  if (anotherDistance < currentDistance) {
    int swap = path[index + 1];
    path[index + 1] = path[index + 2];
    path[index + 2] = path[index + 3];
    path[index + 3] = swap;
    return true;
  }
  return false;
}

/** from1 と to1 を結ぶパスと from2 と to2 を結ぶパスがクロスしているかどうかを返す関数 */
bool isPathCrossing(const coordinate_t from1, const coordinate_t to1, const coordinate_t from2, const coordinate_t to2) {
  ld s, t;
  s = (from1.x - to1.x) * (from2.y - from1.y) - (from1.y - to1.y) * (from2.x - from1.x);
  t = (from1.x - to1.x) * (to2.y - from1.y) - (from1.y - to1.y) * (to2.x - from1.x);
  if (s * t >= 0) return false;

  s = (from2.x - to2.x) * (from1.y - from2.y) - (from2.y - to2.y) * (from1.x - from2.x);
  t = (from2.x - to2.x) * (to1.y - from2.y) - (from2.y - to2.y) * (to1.x - from2.x);
  if (s * t >= 0) return false;
    
  return true;
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
    data[i] = {i, (ld)std::stod(x), (ld)std::stod(y)};
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
