<!--
  ~ Copyright 2022 NWChemEx-Project
  ~
  ~ Licensed under the Apache License, Version 2.0 (the "License");
  ~ you may not use this file except in compliance with the License.
  ~ You may obtain a copy of the License at
  ~
  ~ http://www.apache.org/licenses/LICENSE-2.0
  ~
  ~ Unless required by applicable law or agreed to in writing, software
  ~ distributed under the License is distributed on an "AS IS" BASIS,
  ~ WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  ~ See the License for the specific language governing permissions and
  ~ limitations under the License.
-->

![alt text](docs/source/assets/full_logo.png)

[![Actions](https://github.com/NWChemEx/PluginPlay/workflows/C_C++_CI/badge.svg)](https://github.com/NWChemEx/PluginPlay)

[![Citation Badge](https://api.juleskreuer.eu/citation-badge.php?doi=10.1063/5.0147903)](https://juleskreuer.eu/projekte/citation-badge/)

[Documentation](https://nwchemex.github.io/PluginPlay/)

PluginPlay is an inversion-of-control (IoC) framework to facilitate wrtiting
high-performance modular scientific software. More specifically, using
PluginPlay scientific software is written as a series of disconnected modules.
At runtime the modules are loaded into PluginPlay and then PluginPlay wires the
modules together to form a call graph. A more detailed overview can be found
[here](https://nwchemex.github.io/PluginPlay/background/overview.html).

# Features

From the perspective of high-performance scientific software development, the
main features enjoyed by a program built on top of PluginPlay are:

- Want to call your new optimizer everwhere an optimizer is needed without
  modifying those places, or even knowing where those places are? Runtime
  dependency injection lets you do just that!
- Memoization of the call graph. PluginPlay automatically records the execution
  of the call graph, which facilitates saving/loading a calcualtion.
- Plugin-based development. In C++ there is no need to recompile anything other
  than your plugin while developing it. Notably all other plugins and the
  program consuming the plugins, do not need to be recompiled!

A full feature list can be found
[here](https://nwchemex.github.io/PluginPlay/features.html).

# Install

PluginPlay ultimately uses CMake for configuring and building. This means that
installation is usually achieved via a variation on:

```.sh
git clone https://github.com/NWChemEx/PluginPlay
cd PluginPlay
cmake -H. -Bbuild -D...
cmake --build build --target install
```
More detailed install instructions can be found
[here](https://nwchemex.github.io/PluginPlay/install.html).

# Contributing

- [Contributor Guidelines](https://github.com/NWChemEx/.github/blob/1a883d64519f62da7c8ba2b28aabda7c6f196b2c/.github/CONTRIBUTING.md)
- [Contributor License Agreement](https://github.com/NWChemEx/.github/blob/master/.github/CONTRIBUTING.md#contributor-license-agreement-cla)
- [Developer Documentation](https://nwchemex.github.io/PluginPlay/developer/index.html)
- [Code of Conduct](https://github.com/NWChemEx/.github/blob/master/.github/CODE_OF_CONDUCT.md)

# Acknowledgments

This research was supported by the Exascale Computing Project (17-SC-20-SC), a collaborative effort of the U.S. Department of Energy Office of Science and the National Nuclear Security Administration.
