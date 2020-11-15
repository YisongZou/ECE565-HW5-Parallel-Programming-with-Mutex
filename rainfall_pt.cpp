#include "rainfall_pt.hpp"
#include <bits/stdc++.h>
// Thread Pool Library for c++
#include "ctpl_stl.h"

using namespace std;

mutex mtx; // locks access to counter

// Declaration of variables
int numThreads;
int timeSteps;
float absRate;
int N;
string elevation_file;
float runtime;
vector<vector<int>> elevation;
vector<vector<float>> absorb;
struct timespec start_time, end_time;
int wholeSteps;                // Store the whole timesteps
vector<vector<float>> rain;    // Store the current rain on the ground
vector<vector<float>> trickle; // Store the trickle of each step
vector<vector<float>>
    nextTrickle; // Store the trickle result to be added next round
vector<vector<float>>
    tempTrickle; // Store the trickle result to be added next round
vector<vector<float>> resetTrickle; // Used for resetting the tempTrickle
vector<vector<vector<vector<int>>>> neighborsToTrickle;
float isDrain;
int ID; // thread id

int main(int argc, char *argv[]) {
  if (argc != 6) {
    cout << "Syntax: ./rainfall_seq <P> <M> <A> <N> <elevation_file>" << endl;
    cout << "P = # of parallel threads to use." << endl;
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
  numThreads = stoi(argv[1]);
  timeSteps = stoi(argv[2]);
  absRate = stof(argv[3]);
  N = stoi(argv[4]);
  elevation_file = argv[5];
  elevation = vector<vector<int>>(N, vector<int>());
  absorb = vector<vector<float>>(N, vector<float>(N, 0));
  wholeSteps = 0; // Store the whole timesteps

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
  wholeSteps = calcRain();

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
