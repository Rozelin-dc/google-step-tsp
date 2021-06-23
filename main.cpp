#include <bits/stdc++.h>
using namespace std;

#define SHORT_ENOUGH_DISTANCE 12.0

/** 座標の構造体 */
typedef struct {
  int index;
  long double x;
  long double y;
} coordinate_t;

typedef struct {
  int from;
  int to;
  long double distance;
} edge_t;

long double calculateDistance(const coordinate_t from, const coordinate_t to);
void makeEdgeData(const unordered_map<int, coordinate_t>& data, vector<edge_t>& smallEdgeData, unordered_map<int, vector<edge_t>>& bigEdgeData);
void greedySearch(unordered_map<int, coordinate_t> data, vector<int>& path);
void doSearchByChristofidesAlgorithm(const unordered_map<int, coordinate_t>& data, const vector<edge_t>& smallEdgeData, const unordered_map<int, vector<edge_t>>& bigEdgeData, vector<int>& path);
bool doTwoOpt(const unordered_map<int, coordinate_t>& data, vector<int>& path);
bool swapFivePoint(const unordered_map<int, coordinate_t>& data, vector<int>& path, const int index);
bool nearLineSegmentPointPathChange(const unordered_map<int, coordinate_t>& data, vector<int>& path);
bool isPathCrossing(const coordinate_t from1, const coordinate_t to1, const coordinate_t from2, const coordinate_t to2);
bool isPointAndLineSegmentEnoughNear(const coordinate_t from, const coordinate_t to, const coordinate_t p);
void readInput(unordered_map<int, coordinate_t>& data, const string targetDataNum);
void outputCsv(const vector<int>& path, const string targetDataNum);

int main(int argc, char *argv[]) {
  string targetDataNum = argv[1];
  unordered_map<int, coordinate_t> data; // インデックスをキー、座標を値に持つ
  vector<edge_t> smallEdgeData; // 最小限の辺のデータを持つ
  unordered_map<int, vector<edge_t>> bigEdgeData; // インデックスをキー、その座標からのばせる全ての辺のデータを持つ
  vector<int> path = {0}; // 訪れる座標のインデックスが順に保存されている配列

  readInput(data, targetDataNum);
  makeEdgeData(data, smallEdgeData, bigEdgeData);
  doSearchByChristofidesAlgorithm(data, smallEdgeData, bigEdgeData, path);
  // greedySearch(data, path);

  bool isTwoOptDone = true;
  bool isSwapFivePointDone = true;
  bool isNearLineSegmentPointPathChange = true;
  for (int i = 0; i < (int)(data.size() * 2); i++) {
    isSwapFivePointDone = swapFivePoint(data, path, i % (int)data.size());
    if (isTwoOptDone || isSwapFivePointDone || isNearLineSegmentPointPathChange) {
      // isNearLineSegmentPointPathChange = nearLineSegmentPointPathChange(data, path);
      isTwoOptDone = doTwoOpt(data, path);
    }
  }

  outputCsv(path, targetDataNum);
}

/** from から to までの距離の二乗を計算 */
long double calculateDistance(const coordinate_t from, const coordinate_t to) {
  return ((from.x - to.x) * (from.x - to.x)) + ((from.y - to.y) * (from.y - to.y));
}

/** 全ての辺の長さの二乗を計算し edgeData に保存 */
void makeEdgeData(
  const unordered_map<int, coordinate_t>& data,
  vector<edge_t>& smallEdgeData,
  unordered_map<int, vector<edge_t>>& bigEdgeData
) {
  for (auto itr1 = data.begin(); itr1 != data.end(); ++itr1) {
    coordinate_t from = itr1->second;
    vector<edge_t> fromEdgeData(0);

    for (auto itr2 = data.begin(); itr2 != data.end(); ++itr2) {
      coordinate_t to = itr2->second;
      if (from.index == to.index) continue;

      long double distance = calculateDistance(from, to);
      edge_t edge = {from.index, to.index, distance};

      fromEdgeData.push_back(edge);
      if (from.index < to.index) smallEdgeData.push_back(edge);
    }

    // 距離で昇順にソート
    sort(fromEdgeData.begin(), fromEdgeData.end(),[](const edge_t &alpha, const edge_t &beta){return alpha.distance < beta.distance;});
    bigEdgeData[from.index] = fromEdgeData;
  }

  // 距離で昇順にソート
  sort(smallEdgeData.begin(), smallEdgeData.end(),[](const edge_t &alpha, const edge_t &beta){return alpha.distance < beta.distance;});

  return;
}

/** 貪欲法で経路検索 */
void greedySearch(unordered_map<int, coordinate_t> data, vector<int>& path) {
  while (data.size() > 1) {
    long double distance; // 距離の二乗
    long double minDistance = -1; // 距離の二乗の最小値
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

      if (distance < minDistance) {
        minDistance = distance;
        minDistanceIndex = itr->first;
      }
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

/** クリストフィードのアルゴリズムによる探索 */
void doSearchByChristofidesAlgorithm(
  const unordered_map<int, coordinate_t>& data,
  const vector<edge_t>& smallEdgeData,
  const unordered_map<int, vector<edge_t>>& bigEdgeData,
  vector<int>& path
) {
  unordered_map<int, vector<edge_t>> visited; // 座標のインデックスをキー、繋がれている辺のデータを値に持つ(座標の最小全域木)
  int i = 0;

  // 最小全域木の構築
  while (visited.size() < data.size() && i < (int)smallEdgeData.size()) {
    edge_t edge = smallEdgeData[i];
    if (visited.find(edge.from) == visited.end() || visited.find(edge.to) == visited.end()) {
      visited[edge.from].push_back(edge);
      visited[edge.to].push_back(edge);
    }
    i++;
  }

  if (visited.size() != data.size()) {
    cerr<<"error!"<<endl;
    cout<<visited.size()<<", "<<data.size()<<", "<<smallEdgeData.size()<<endl;
    exit(1);
  }

  cout<<"finish saisyouzennikigi kouchiku"<<endl;
  
  unordered_map<int, bool> vertexSetNextOddNumber; // 奇数次の頂点集合(インデックスをキー、最小全域木と統合済みかどうかを値に持つ)
  // 奇数次の頂点集合を構築
  for (auto itr = visited.begin(); itr != visited.end(); ++itr) {
    if ((itr->second).size() % 2 != 0) {
      vertexSetNextOddNumber[itr->first] = false;
    }
  }

  cout<<"finish kusuuzi tyotensyugo kouchiku"<<endl;

  cout<<vertexSetNextOddNumber.size()<<endl;

  // 奇数次の頂点集合の最小重み最適マッチングを最小全域木と統合
  for (auto itr = vertexSetNextOddNumber.begin(); itr != vertexSetNextOddNumber.end(); ++itr) {
    int idx = itr->first;
    cout<<idx<<endl;
    vector<edge_t> edgeData = bigEdgeData.at(idx);
    for (i = 0; i < (int)edgeData.size(); i++) {
      edge_t edge = edgeData[i];
      if (
        vertexSetNextOddNumber.find(edge.from) != vertexSetNextOddNumber.end() &&
        vertexSetNextOddNumber.find(edge.to) != vertexSetNextOddNumber.end() && 
        vertexSetNextOddNumber.at(edge.from) == false &&
        vertexSetNextOddNumber.at(edge.to) == false
      ) {
        visited[edge.from].push_back(edge);
        visited[edge.to].push_back(edge);

        vertexSetNextOddNumber.at(edge.from) = true;
        vertexSetNextOddNumber.at(edge.to) = true;

        break;
      }
    }
  }

  cout<<"finish macthing"<<endl;

  for (i = 0; i < (int)data.size(); i++) {
    vector<edge_t> visitedEdgeData = visited[i];
    // 距離で昇順にソート
    sort(visitedEdgeData.begin(), visitedEdgeData.end(),[](const edge_t &alpha, const edge_t &beta){return alpha.distance < beta.distance;});

    visited[i] = visitedEdgeData;
  }

  cout<<"finish sort"<<endl;

  //  path 構築
  while (visited.size() > 1) {
    int fromIndex = path[path.size() - 1];
    vector<edge_t> edgeData = visited.at(fromIndex);

    for (i = 0; i < (int)edgeData.size(); i++) {
      edge_t edge = edgeData[i];
      int toIndex = edge.to;
      if (toIndex == fromIndex) toIndex = edge.from;

      if (visited.find(toIndex) != visited.end()) {
        path.push_back(toIndex);
        visited.erase(fromIndex);
        break;
      }
    }

    // 既に訪れた座標をとばす
    long double minDistance = -1;
    long double distance;
    int minDistanceIndex;
    for (auto itr = visited.begin(); itr != visited.end(); ++itr) {
      int toIndex = itr->first;
      if (fromIndex == toIndex) continue;

      distance = calculateDistance(data.at(fromIndex), data.at(toIndex));

      if (minDistance == -1 || distance < minDistance) {
        minDistance = distance;
        minDistanceIndex = toIndex;
      }
    }
    path.push_back(minDistanceIndex);
    visited.erase(fromIndex);
  }
  cout<<"finish"<<endl;
  cout<<path.size()<<endl;
  return;
}

/** path に 2-opt をかける */
bool doTwoOpt(const unordered_map<int, coordinate_t>& data, vector<int>& path) {
  for (int i = 0; i < (int)path.size() - 1; i++) {
    coordinate_t from1 = data.at(path[i]); // 1本目のパスの始点
    coordinate_t to1 = data.at(path[i + 1]); // 1本目のパスの終点

    for (int j = i + 2; j < (int)path.size(); j++) {
      coordinate_t from2 = data.at(path[j]); // 2本目のパスの始点
      coordinate_t to2; // 2本目のパスの終点

      if (i == 0 && j + 1 == (int)path.size()) break; // 1本目のパスの始点と2本目のパスの終点が同じだったら

      if (j + 1 == (int)path.size()) to2 = data.at(path.front());
      else to2 = data.at(path[j + 1]);

      // パスがクロスしていたら path を再構築
      if (isPathCrossing(from1, to1, from2, to2)) {
        vector<int> newPath;
        std::copy(path.begin(), path.begin() + i + 1, back_inserter(newPath));
        newPath.push_back(from2.index);

        if (i + 2 != j) {
          vector<int> swapPath;
          std::copy(path.begin() + i + 2, path.begin() + j, back_inserter(swapPath));
          reverse(swapPath.begin(), swapPath.end());

          newPath.insert(newPath.end(), swapPath.begin(), swapPath.end());
        }

        newPath.push_back(to1.index);
        if (to2.index != path.front()) newPath.push_back(to2.index);

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
  int firstIndex = index;
  int secondIndex = (index + 1) % (int)data.size();
  int thirdIndex = (index + 2) % (int)data.size();
  int forthIndex = (index + 3) % (int)data.size();
  int fifthIndex = (index + 4) % (int)data.size();
  coordinate_t first = data.at(path[firstIndex]);
  coordinate_t second = data.at(path[secondIndex]);
  coordinate_t third = data.at(path[thirdIndex]);
  coordinate_t forth = data.at(path[forthIndex]);
  coordinate_t fifth = data.at(path[fifthIndex]);

  long double currentDistance = sqrt(calculateDistance(first, second)) + sqrt(calculateDistance(second, third)) + sqrt(calculateDistance(third, forth)) + sqrt(calculateDistance(forth, fifth));
  long double anotherDistance = sqrt(calculateDistance(first, third)) + sqrt(calculateDistance(third, forth)) + sqrt(calculateDistance(forth, second)) + sqrt(calculateDistance(second, fifth));

  if (anotherDistance < currentDistance) {
    int swap = path[secondIndex];
    path[secondIndex] = path[thirdIndex];
    path[thirdIndex] = path[forthIndex];
    path[forthIndex] = swap;
    return true;
  }
  return false;
}

/** 線分の近くにある点のパスを変える */
bool nearLineSegmentPointPathChange(const unordered_map<int, coordinate_t>& data, vector<int>& path) {
  for (int i = 0; i < (int)data.size(); i++) {
    coordinate_t from = data.at(path[i]);
    coordinate_t to = data.at(path[(i + 1) % (int)data.size()]);

    for (int j = 0; j < (int)data.size(); j++) {
      if (j == i || j == i + 1) continue;

      coordinate_t p = data.at(path[j]);
      // p が from と to を結ぶ線分と十分に近かったら path 組み換え
      if (isPointAndLineSegmentEnoughNear(from, to, p)) {
        vector<int> newPath;
        if (to.index == path.front()) {
          std::copy(path.begin(), path.begin() + j, back_inserter(newPath));
          std::copy(path.begin() + j + 1, path.end(), back_inserter(newPath));
          newPath.push_back(p.index);
        } else if (j > i + 1) {
          std::copy(path.begin(), path.begin() + i + 1, back_inserter(newPath));

          newPath.push_back(p.index);

          std::copy(path.begin() + i + 1, path.begin() + j, back_inserter(newPath));
          std::copy(path.begin() + j + 1, path.end(), back_inserter(newPath));
        } else {
          std::copy(path.begin(), path.begin() + j , back_inserter(newPath));
          std::copy(path.begin() + j + 1, path.begin() + i + 1, back_inserter(newPath));

          newPath.push_back(p.index);

          std::copy(path.begin() + i + 1, path.end(), back_inserter(newPath));
        }
        path = newPath;
        return true;
      }
    }
  }
  return false;
}


/** from1 と to1 を結ぶパスと from2 と to2 を結ぶパスがクロスしているかどうかを返す関数 */
bool isPathCrossing(const coordinate_t from1, const coordinate_t to1, const coordinate_t from2, const coordinate_t to2) {
  long double s, t;
  s = (from1.x - to1.x) * (from2.y - from1.y) - (from1.y - to1.y) * (from2.x - from1.x);
  t = (from1.x - to1.x) * (to2.y - from1.y) - (from1.y - to1.y) * (to2.x - from1.x);
  if (s * t >= 0) return false;

  s = (from2.x - to2.x) * (from1.y - from2.y) - (from2.y - to2.y) * (from1.x - from2.x);
  t = (from2.x - to2.x) * (to1.y - from2.y) - (from2.y - to2.y) * (to1.x - from2.x);
  if (s * t >= 0) return false;
    
  return true;
}

/** from と to を結ぶ線分と p との距離が十分近いかどうか */
bool isPointAndLineSegmentEnoughNear(const coordinate_t from, const coordinate_t to, const coordinate_t p) {
  coordinate_t h; // 垂線の足

  if(from.x == to.x){ // 線分が垂直の場合
    h.x = from.x;
    h.y = p.y;
  } else if(from.y == to.y){ // 線分が水平の場合
    h.x = p.x;
    h.y = from.y;
  } else{ // それ以外
    long double m1, m2, b1, b2;

    m1 = (to.y - from.y) / (to.x - from.x); // 線分の傾き
    b1 = from.y - (m1 * from.x); // 線分のy切片

    m2 = -1.0 / m1; // 点 p を通り、線分に垂直な線の傾き
    b2 = p.y - (m2 * p.x); // 点 p を通り、線分に垂直な線のy切片

    // 交点算出
    h.x = (b2 - b1) / (m1 - m2);
    h.y = (b2 * m1 - b1 * m2) / (m1 - m2);
  }

  // h が線分上に無ければ
  if (h.y > from.y && h.y > to.y) return false;
  if (h.y < from.y && h.y < to.y) return false;
  if (h.x > from.x && h.x > to.x) return false;
  if (h.x < from.x && h.x < to.x) return false;

  if(calculateDistance(p, h) > SHORT_ENOUGH_DISTANCE) return false;
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
    data[i] = {i, (long double)std::stod(x), (long double)std::stod(y)};
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
