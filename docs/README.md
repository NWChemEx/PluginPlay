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

Building the Documentation
==========================

PluginPlay's documentation has two parts:

- The Sphinx front-end which contains things like tutorials
- The Doxygen-generated C++ API documentation

The C++ API documentation is built as part of the build process and then moved
to `docs/source/PluginPlay_cxx_api`. When the Sphinx front-end documentation
is built it will link to the C++ documentation. If the C++ documentation does
not exist, building the Sphinx front-end will still work, but the link to the
C++ API won't work.

Building the C++ Documentation
------------------------------

The C++ documentation is built by Doxygen. You will need to have Doxygen
installed on your system (it is likely available in your operating system's
package manager, *e.g.*, try `apt-get install doxygen`). By default, when you
run CMake on PluginPlay, building documentation is disabled. To enable it,
either pass `BUILD_DOCS=TRUE` to the CMake command, or (preferred) set
`BUILD_DOCS` to true in your toolchain file. Optionally, you may set
`ONLY_BUILD_DOCS` to `TRUE` if you would like to skip building PluginPlay
(useful, for example, if you are iterating on the documentation locally). As
an example:

```.sh
# In the PluginPlay repository's root
cmake -H. -Bbuild -DBUILD_DOCS=TRUE -DONLY_BUILD_DOCS=TRUE
cmake --build build --target PluginPlay_cxx_api
```

When you are done building there should be a file `build/html/index.html`, this
is the entry point into the C++ API docs. Unfortunately, Sphinx does not allow
you to link to files which reside in parent directories of the `conf.py` file,
hence the link to the C++ API in the Sphinx documentation can not be made to
point to `build/html/index.html`. It is thus easier to view the C++ API
documentation separately by directing your web browser directly at
`build/html/index.html`.

Building the Sphinx Front-End
-----------------------------

The Sphinx front-end of the documentation requires several Python packages to
be installed (see the `requirements.txt` file). These packages are all available
from PIP. The workflow below shows how to setup a Python virtual environment,
download the dependencies, and build the Sphinx documentation. For the sake
of this tutorial we assume you want to call your virtual
environment `PluginPlay_venv`:

```.bash
# In the docs/ directory
python3 -m venv PluginPlay_venv
source PluginPlay_venv/bin/activate
pip install -r requirements.txt
make html
```

You can then point your webbrowser at `docs/build/html/index.html` to view the
documentation.
