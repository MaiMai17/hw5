#include <cstdio>
#include <cerrno>
#include <cmath>
#include <cfloat>
#include <iostream>
#include <map>

#define INF 100000000
#define MIN 0.0000001

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
  float x, y;
  int i = 0;
  fscanf(fp, "%c,%c", &buf0, &buf1);
  while (fscanf(fp,"%f,%f\n", &x, &y) != EOF) {
    Point p;
    p.x = x;
    p.y = y;
    m.insert(pair <int, Point> (i, p));
    i++;
    num++;
  }
}

void write_csv_file(FILE* fp, int n, int* order) {
  fprintf(fp, "index\n");
  for (int i = 0; i < n; i++) {
    fprintf(fp, "%d\n", order[i]);
  }
  fclose(fp);
}

float calculate_distance(Point p, Point q) {
  return sqrt(pow(p.x - q.x, 2.0) + pow(p.y - q.y, 2.0));
}

int search_minP(map<int, Point> &m, int* order) {
  float d = INF, buf;
  int number;
  Point zero = {0.0, 0.0};
  for(int i = 0; i < num; i++) {
    buf = calculate_distance(zero, m[i]);
    if (buf < d) {
      d = buf;
      number = i;
    }
  }
  return number;
}

int search_maxP(map<int, Point> &m, int* order) {
  float d = MIN, buf;
  int number;
  Point zero = {0.0, 0.0};
  for(int i = 0; i < num; i++) {
    buf = calculate_distance(zero, m[i]);
    if (buf > d) {
      d = buf;
      number = i;
    }
  }
  return number;
}

bool evaluate_line (Point p, Point q, Point given) {
  float y = ((p.y - q.y)/(p.x - q.x)) * (given.x - p.x) + p.y;
  if (given.y > y) return true;
  else return false;
}

//直線よりも上にある点をg1
void divide_map(map <int, Point> &m, map <int, Point> &g1, map <int, Point> &g2, int num, int min, int max) {

  for(int i = 0; i < num; i++) {
    if (i == min || i == max) continue;
    if (evaluate_line(m[max], m[min], m[i])) {
      g1.insert(pair <int, Point> (i, m[i]));
    }
    else
      g2.insert(pair <int, Point> (i, m[i]));
  }
}

float calculate_tour(map<int, Point> &m, int* order) {
  float sum = 0;
  for(int i = 0; i < num-1; i++) {
    sum += calculate_distance(m[order[i]], m[order[i+1]]);
  }
  sum += calculate_distance(m[order[0]], m[order[num-1]]); //頭としっぽ
  return sum;
}

int search_nearest(map<int, Point> &m, char* visited, int given) {
  int key = -1;
  float min = INF;
  map <int, Point> :: iterator it;

  for(int i = 0; i < num; i++) {
    it = m.find(i);
    if(it != m.end()) {
      if(visited[i] != 1) {
        float buf = calculate_distance(m[given], m[i]);
        if(buf < min) {
          key = i;
          min = buf;
        }
      }
    }
  }
  return key;
}

int* make_order(int* order, map<int, Point> &m, int start, int start2) {
  char visited[num];
  int next;
  map <int, Point> group1, group2;

  std::cout << start << "," << start2 << '\n';
  divide_map(m, group1, group2, num, start, start2);

  std::cout << num << '\n';
  std::cout << group1.size() << '\n';
  std::cout << group2.size() << '\n';

  for (int i = 0; i < num-1; i++) {
    order[i] = start;
    visited[start] = 1;

    if (i < group1.size()-1) {
      next = search_nearest(group1, visited, start);
      start = next;
    }
    else if (i == group1.size()-1) {
      next = start2;
      start = next;
    }
    else {
      next = search_nearest(group2, visited, start);
      start  = next;
    }
  }
  order[num-1] = next;
  return order;
}



int main(int argc, char* argv[]) {

  map <int, Point> tour, group1, group2;

  FILE* ifp = open_input_file(argv[1]);
  read_csv_file(ifp, tour);

  int order[num];

  int start = search_minP(tour, order);
  int start2 = search_maxP(tour, order);

  float length = calculate_tour(tour, make_order(order, tour, start, start2));

  FILE* ofp = open_output_file(argv[2]);
  write_csv_file(ofp, num, order);

  cout<< length << endl;

  fclose(ifp);
  return 0;
}
