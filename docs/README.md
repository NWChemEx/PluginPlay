Building the Documentation
==========================

pluginplay's documentation has two parts:

- The Sphinx front-end which contains things like tutorials
- The Doxygen-generated C++ API documentation

The C++ API documentation is built as part of the build process and then moved
to `docs/source/pluginplay_cxx_api`. When the Sphinx front-end documentation is built
it will link to the C++ documentation. If the C++ documentation does not exist,
building the Sphinx front-end will still work, but the link to the C++ API won't
work.

Building the Sphinx Front-End
-----------------------------

The Sphinx front-end of the documentation requires the following Python packages
(all can be installed with pip):

- sphinx_rtd_theme (The Read-The-Docs theme for sphinx)
- sphinx (The thing that makes the documenation)

To build the Sphinx front-end of the documentation it is recommended that you
use Python's virtual environments. Assuming you want to call your virtual
environment `pluginplay_venv` the steps are (run from this directory):

```.bash
python3 -m venv pluginplay_venv
source pluginplay_venv/bin/activate
pip install -r requirements.txt
make html
```

You can then point your webbrowser at `build/html/index.html` to view the
documentation.
