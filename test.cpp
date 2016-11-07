#include "myheader.h"
#include "vector"

int main(){

  int a;

  std::vector<double> x[100];
  std::vector<double> y[100];

  for (int i = 0; i < 100; ++i){
    x[i] = y[i];
  }

  return 0;
}
