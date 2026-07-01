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

Printing
========

This directory contains the source code related to printing out modules using
PluginPlay. The majority of the implementation is hidden in the detail_,
namespace to avoid propagating our dependencies. More specifically, the main API
is simply:

``` C++
std::ostream& operator<<(std::ostream&, const pluginplay::Module&);
```
