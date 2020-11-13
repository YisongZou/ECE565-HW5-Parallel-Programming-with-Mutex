#include <bits/stdc++.h>

using namespace std;

// Used to calculate the run time
double calc_time(struct timespec start, struct timespec end) {
  double start_sec =
      (double)start.tv_sec * 1000000000.0 + (double)start.tv_nsec;
  double end_sec = (double)end.tv_sec * 1000000000.0 + (double)end.tv_nsec;

  if (end_sec < start_sec) {
    return 0;
  } else {
    return end_sec - start_sec;
  }
}

// 1) Receive a new raindrop (if it is still raining) for each point.
// 2) If there are raindrops on a point, absorb water into the point
// 3a) Calculate the number of raindrops that will next trickle to the
// lowest neighbor(s)
void rainAbsorbTrickle(vector<vector<float>> &rain,
                       vector<vector<float>> &absorb,
                       vector<vector<float>> &trickle, int &timeSteps,
                       const float &absRate, float &isDrain) {
  for (int i = 0; i < rain.size(); ++i) {
    for (int j = 0; j < rain[0].size(); ++j) {
      if (timeSteps > 0) {
        // 1) Receive a new raindrop (if it is still raining) for each point.
        ++rain[i][j];
      }
      if (rain[i][j] == 0) {
        continue;
      }
      // 2) If there are raindrops on a point, absorb water into the point
      if (rain[i][j] >= absRate) {
        rain[i][j] -= absRate;
        absorb[i][j] += absRate;
      } else if (rain[i][j] > 0) {
        absorb[i][j] += rain[i][j];
        rain[i][j] = 0;
      }
      // 3a) Calculate the number of raindrops that will next trickle to the
      // lowest neighbor(s)
      trickle[i][j] = 0; // Reset Trickle array
      if (rain[i][j] >= 1) {
        trickle[i][j] = 1;
      } else if (rain[i][j] > 0) {
        trickle[i][j] = rain[i][j];
      }
      isDrain += trickle[i][j];
    }
  }
}

// Sort and determin which neighbor(s) to trickle
vector<vector<int>> countNeighbor(int i, int j,
                                  const vector<vector<int>> &elevation) {
  int N = elevation.size();
  vector<vector<int>> neighToTrickle; // <elevation, number>, number: Left: 0,
                                      // up: 1,right: 2, down:3
  vector<vector<int>> allNeigh;       // All neighbors that have less elevation
  if (j - 1 >= 0 && elevation[i][j - 1] < elevation[i][j]) { // left
    allNeigh.push_back({elevation[i][j - 1], 0});
  }
  if (i - 1 >= 0 && elevation[i - 1][j] < elevation[i][j]) { // up
    allNeigh.push_back({elevation[i - 1][j], 1});
  }
  if (j + 1 < N && elevation[i][j + 1] < elevation[i][j]) { // right
    allNeigh.push_back({elevation[i][j + 1], 2});
  }
  if (i + 1 < N && elevation[i + 1][j] < elevation[i][j]) { // down
    allNeigh.push_back({elevation[i + 1][j], 3});
  }
  sort(allNeigh.begin(), allNeigh.end());
  if (allNeigh.size() == 1) {
    neighToTrickle.push_back(allNeigh[0]);
  }
  for (int i = 0; allNeigh.size() != 0 && i < allNeigh.size() - 1; ++i) {
    if (allNeigh[i][0] == allNeigh[i + 1][0] && i == allNeigh.size() - 2) {
      neighToTrickle.push_back(allNeigh[i]);
      neighToTrickle.push_back(allNeigh[i + 1]);
      break;
    } else if (allNeigh[i][0] == allNeigh[i + 1][0]) {
      neighToTrickle.push_back(allNeigh[i]);
    } else {
      neighToTrickle.push_back(allNeigh[i]);
      break;
    }
  }
  return neighToTrickle;
}

// 3b) For each point, use the calculated number of raindrops that will
// trickle to the lowest neighbor(s) to update the number of raindrops at
// each lowest neighbor, if applicable.
void calcTrickle(vector<vector<float>> &rain,
                 const vector<vector<int>> &elevation,
                 vector<vector<float>> &trickle,
                 vector<vector<vector<vector<int>>>> &neighborsToTrickle) {
  vector<vector<int>> direct = {
      {0, -1}, {-1, 0}, {0, 1}, {1, 0}}; // Left, up, right, down
  for (int i = 0; i < rain.size(); ++i) {
    for (int j = 0; j < rain[0].size(); ++j) {
      vector<vector<int>> neighToTrickle = neighborsToTrickle[i][j];
      if (neighToTrickle.size() == 0) { // No neighbor to trickle
        trickle[i][j] = 0;
        continue;
      }
      float share = trickle[i][j] / neighToTrickle.size();
      rain[i][j] -= trickle[i][j];
      trickle[i][j] = 0;
      for (auto n : neighToTrickle) {
        rain[i + direct[n[1]][0]][j + direct[n[1]][1]] += share;
      }
    }
  }
}

// Calculate the whole time steps needed to drain
int calcRain(const vector<vector<int>> &elevation,
             vector<vector<float>> &absorb, int timeSteps, float absRate,
             struct timespec &start_time, struct timespec &end_time) {
  int N = elevation.size();
  vector<vector<float>> rain(
      N, vector<float>(N, 0)); // Store the current rain on the ground
  vector<vector<float>> trickle(
      N, vector<float>(N, 0)); // Store the trickle of each step
  vector<vector<vector<vector<int>>>> neighborsToTrickle(
      N, vector<vector<vector<int>>>(N, vector<vector<int>>()));
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      vector<vector<int>> neighToTrickle = countNeighbor(i, j, elevation);
      neighborsToTrickle[i][j] = neighToTrickle;
    }
  }
  int wholeSteps = 0;
  float isDrain = 1;

  clock_gettime(CLOCK_MONOTONIC, &start_time);

  while (isDrain != 0) {
    isDrain = 0;

    // 1) Receive a new raindrop (if it is still raining) for each point.
    // 2) If there are raindrops on a point, absorb water into the point
    // 3a) Calculate the number of raindrops that will next trickle to the
    // lowest neighbor(s)
    rainAbsorbTrickle(rain, absorb, trickle, timeSteps, absRate, isDrain);
    // 3b) For each point, use the calculated number of raindrops that will
    // trickle to the lowest neighbor(s) to update the number of raindrops
    // at each lowest neighbor, if applicable.
    calcTrickle(rain, elevation, trickle, neighborsToTrickle);
    --timeSteps;
    ++wholeSteps;
  }

  clock_gettime(CLOCK_MONOTONIC, &end_time);

  return wholeSteps;
}

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
