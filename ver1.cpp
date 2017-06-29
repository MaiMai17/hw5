#include <cstdio>
#include <cerrno>
#include <cmath>
#include <cfloat>
#include <iostream>
#include <map>

#define INF 100000000

using namespace std;

struct  Point {
  float x;
  float y;
};

int num = 0;

FILE* open_input_file(char* filename) {
  FILE* fp = std::fopen(filename, "r");
  if(fp == NULL) {
    perror("input file open failed");
    exit(EXIT_FAILURE);
  }
  return fp;
}

FILE* open_output_file(char* filename) {
  FILE* fp = std::fopen(filename, "w");
  if(fp == NULL) {
    perror("output file open failed");
    exit(EXIT_FAILURE);
  }
  return fp;
}

void read_csv_file(FILE* fp, map <int, Point> &m) {
  char buf0, buf1;
  float x = 0, y = 0;
  int i = 0;

  fscanf(fp, "%c,%c", &buf0, &buf1); //1行目読み取り

  while (fscanf(fp,"%f,%f\n", &x, &y) != EOF) { //ファイル終わりまで読み取り、map挿入
    Point p;
    p.x = x;
    p.y = y;
    m.insert(pair<int, Point>(i, p));
    i++;
    num++;
  }
}

void write_csv_file(FILE* fp, int n, int* order) {
  int i;
  for (i = 0; i < n; i++) {
    fprintf(fp, "%d\n", order[i]);
  }
  fclose(fp);
}

float calculate_distance(Point p, Point q) {
  return sqrt(pow(p.x - q.x, 2.0) + pow(p.y - q.y, 2.0));
}

bool evaluate_point(Point p, Point q) {
  if(p.x < q.x && p.y < q.y) return true;
  else return false;
}

float calculate_tour_length(map<int, Point> &m, int* order) {
  float sum = 0;
  for(int i = 0; i < num-1; i++) {
    sum += calculate_distance(m[order[i]], m[order[i+1]]);
  }
  sum += calculate_distance(m[order[0]], m[order[num-1]]); //頭としっぽ
  return sum;
}

int search_nearest_point(map<int, Point> &m, char* visited, Point given) {
  int key = -1;
  float min = INF;

  for(int i = 0; i < num; i++) {
    if(visited[i] != 1){
      float buf = calculate_distance(given, m[i]);
      if(buf < min){
        key = i;
        min = buf;
      }
    }
  }
  return key;
}

int search_2nd_point(map<int, Point> &m, char* visited, Point given) {
  int key = -1;
  int secondkey = -1;
  float min = INF;
  float secondmin = INF;

  for(int i = 0; i < num; i++) {
    if(visited[i] != 1){
      float buf = calculate_distance(given, m[i]);
      if(buf < min){
        key = i;
        min = buf;
      }
      else if(buf < secondmin) {
        secondkey = i;
        secondmin = buf;
      }
    }
  }
  if(secondkey == -1) {
    return key;
  }
  else if((secondmin < min*1.17) && evaluate_point(m[secondkey], m[key])) {
    return secondkey;
  }
  else {
    return key;
  }
}

int* make_tour(int* order, map<int, Point> &m, int start) {
  char visited[num];
  int next;
  for (int i = 0; i < num-1; i++) {
    order[i] = start;
    visited[start] = 1;
    next = search_2nd_point(m, visited, m[start]);
    start = next;
    }
  order[num-1]= next;

  return order;
}

int main(int argc, char* argv[]) {

  map<int, Point> tour;
  int order[num];
  char dum[num];
  Point zero = {0.0, 0.0};

  FILE* ifp = open_input_file(argv[1]);
  read_csv_file(ifp, tour);

  int start = search_nearest_point(tour, dum, zero);
  float length = calculate_tour_length(tour, make_tour(order, tour, start));

  FILE* ofp = open_output_file(argv[2]);
  write_csv_file(ofp, num, order);

  cout<< length << endl;
  fclose(ifp);
  return 0;
}
