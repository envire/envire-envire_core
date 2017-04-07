//
// Copyright (c) 2015, Deutsches Forschungszentrum für Künstliche Intelligenz GmbH.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#ifndef ENVIREGRAPH2DSTRUCTUREVISUALIZATION_HPP
#define ENVIREGRAPH2DSTRUCTUREVISUALIZATION_HPP
#include "EnvireGraph2DStructurWidget.hpp"

/**The qt designer lib is installed into a different folder where it is not
 * found by most applications. In order to use the 2d visualizer widget 
 * in other applications as well, the whole functionallity is implemented in 
 * EnvireGraph2DStructurWidget.hpp. This file is just a wrapper for the qt 
 * designer stuff */
class EnvireGraph2DStructureVisualization : public envire::viz::EnvireGraph2DStructurWidget
{
    Q_OBJECT
public:
    EnvireGraph2DStructureVisualization(QWidget *parent = 0) : 
        EnvireGraph2DStructurWidget(parent) {}
};

#endif /* ENVIREGRAPH2DSTRUCTUREVISUALIZATION_HPP */
