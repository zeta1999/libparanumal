/*

The MIT License (MIT)

Copyright (c) 2017 Tim Warburton, Noel Chalmers, Jesse Chan, Ali Karakus

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#ifndef GRADIENT_HPP
#define GRADIENT_HPP 1

#include "core.hpp"
#include "mesh.hpp"
#include "solver.hpp"

#define DGRADIENT LIBP_DIR"/solvers/gradient/"

class gradientSettings_t: public settings_t {
public:
  gradientSettings_t(MPI_Comm& _comm);
  void report();
  void parseFromFile(occaSettings_t& occaSettings,
                     meshSettings_t& meshSettings,
                     const string filename);
};

class gradient_t: public solver_t {
public:
  int Nfields;

  dfloat *q;
  occa::memory o_q;

  dfloat *gradq;
  occa::memory o_gradq;

  occa::memory o_Mgradq;

  occa::kernel volumeKernel;

  occa::kernel MassMatrixKernel;

  occa::kernel initialConditionKernel;

  gradient_t() = delete;
  gradient_t(mesh_t& _mesh, linAlg_t& _linAlg, settings_t& _settings):
    solver_t(_mesh, _linAlg, _settings) {}

  ~gradient_t();

  //setup
  static gradient_t& Setup(mesh_t& mesh, linAlg_t& linAlg,
                          gradientSettings_t& settings);

  void Run();

  void Report();

  void PlotFields();
};

#endif
