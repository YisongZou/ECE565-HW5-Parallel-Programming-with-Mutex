#include "rainfall_seq.hpp"
#include <bits/stdc++.h>

using namespace std;

int main(int argc, char *argv[]) {
  if (argc != 5) {
    cout << "Syntax: ./rainfall_seq <M> <A> <N> <elevation_file>" << endl;
    cout << "M = # of simulation time steps during which a rain drop will "
            "fall "
            "on each landscape"
            "point. In other words, 1 rain drop falls on each point during "
            "the "
            "first M steps of the"
            "simulation."
         << endl;
    cout << "A = absorption rate (specified as a floating point number). "
            "The "
            "amount of raindrops"
            "that are absorbed into the ground at a point during a"
            "timestep."
         << endl;
    cout << "N = dimension of the landscape (NxN)" << endl;
    cout << "elevation_file = name of input file that specifies the "
            "elevation "
            "of each point."
         << endl;
    return EXIT_FAILURE;
  }

  // Initialization of variables
  int timeSteps = stoi(argv[1]);
  float absRate = stof(argv[2]);
  int N = stoi(argv[3]);
  string elevation_file = argv[4];
  float runtime;
  vector<vector<int>> elevation(N, vector<int>());
  vector<vector<float>> absorb(N, vector<float>(N, 0));
  struct timespec start_time, end_time;
  int wholeSteps = 0; // Store the whole timesteps

  // open the file to read.
  fstream in("./" + elevation_file);
  string line;
  int ele; // Store elevation for each input unit
  if (in)  // file exists
  {
    int row = 0;
    while (getline(in, line)) // line does not contain newline of each line
    {
      stringstream ss(line);
      while (ss >> ele) {
        elevation[row].push_back(ele);
      }
      ++row;
    }
  } else // No such file
  {
    cout << "No such elevation_file" << endl;
    return EXIT_FAILURE;
  }

  // Calculate the whole time steps needed to drain
  wholeSteps =
      calcRain(elevation, absorb, timeSteps, absRate, start_time, end_time);

  float elapsed_ns = calc_time(start_time, end_time);
  cout << "Rainfall simulation took " << wholeSteps
       << " time steps to complete." << endl;
  cout << "Runtime = " << elapsed_ns / 1000000000 << " seconds" << endl;
  cout << endl;
  cout << "The following grid shows the number of raindrops absorbed at each "
          "point:"
       << endl;
  for (auto r : absorb) {
    for (auto c : r) {
      cout << setw(8) << setprecision(6) << c;
    }
    cout << endl;
  }
  return 0;
}
