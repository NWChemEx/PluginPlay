.. Copyright 2026 NWChemEx-Project
..
.. Licensed under the Apache License, Version 2.0 (the "License");
.. you may not use this file except in compliance with the License.
.. You may obtain a copy of the License at
..
.. http://www.apache.org/licenses/LICENSE-2.0
..
.. Unless required by applicable law or agreed to in writing, software
.. distributed under the License is distributed on an "AS IS" BASIS,
.. WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
.. See the License for the specific language governing permissions and
.. limitations under the License.

.. _plugin_design:

##########################
Designing the Plugin Class
##########################

This page describes the design of the ``Plugin`` class, which represents a
plugin in PluginPlay. How plugins are used to initialize a ``ModuleManager``
is described in :ref:`pp_initialization_design`.

*****************
What is a Plugin?
*****************

A plugin is a named, distributable collection of :ref:`module` objects. A
plugin is also responsible for the default wiring among its own modules,
*e.g.*, telling its driver module which of the plugin's step modules to use.
A plugin is loaded into a ``ModuleManager`` as a single unit.

.. note::

   Plugins are ultimately C++ or Python classes. The examples on this page use
   three plugins. ``MyPlugin`` is the type of the plugin being written.
   ``ProviderA`` and ``ProviderB`` are the types of the other plugins that
   ``MyPlugin`` would like to get default submodules from. We will assume that
   ``ProviderA`` and ``ProviderB`` are already written and loaded into the
   ``ModuleManager`` under keys ``"provider_a"`` and ``"provider_b"``,
   respectively.

******************************
Why Do We Need a Plugin Class?
******************************

The original approach was a set of free functions. ``DECLARE_PLUGIN`` and
``DEFINE_PLUGIN`` (in ``pluginplay/plugin/plugin.hpp``) declare and define a
``load_modules(ModuleManager&)`` function. By convention, most plugins also
wrote a ``set_defaults(ModuleManager&)`` function for wiring that depended on
other plugins. This approach has several problems:

#. Plugins have no identity. PluginPlay cannot tell which plugins are loaded
   or which modules came from which plugin. This makes it cumbersome to know
   if a module came from a particular plugin, or if another plugin loaded a
   module with the same key.
#. Cross-plugin wiring was ad hoc. Every plugin reinvented "set this submodule
   if the provider happens to be loaded." The end user, *e.g.*, a package
   that combines several plugins, must call every ``load_modules`` and then
   every ``set_defaults`` in the right order.
#. Creating modules was tied to a live ``ModuleManager``. A plugin could not
   describe its modules without also inserting them into a particular
   ``ModuleManager``.
#. Nothing recorded which plugins a plugin expects to get defaults from.

Making plugins first-class objects allows PluginPlay to own the process of
loading and wiring plugins. This solves these problems and more.

*********************
Plugin Considerations
*********************

#. Identity

   - Each plugin has a name, *e.g.*, ``"my_plugin"``.
   - Plugin names must be unique within a ``ModuleManager``.

#. Module creation is separate from the ``ModuleManager``

   - A plugin creates its modules and stores them itself. It does not insert
     them into a ``ModuleManager``.
   - The plugin's modules already include its internal wiring. For example,
     ``MyPlugin``'s driver module will already link to any step modules
     provided by ``MyPlugin``.
   - Plugins can no longer accidentally modify other plugins' modules.

#. Declared dependencies on default providers

   - A plugin lists the plugins it would like to get default submodules from,
     *e.g.*, ``MyPlugin`` lists ``ProviderA`` and ``ProviderB``.
   - These are preferences, not requirements. The plugin will still load
     when a preferred provider is not loaded, but some of its submodules will
     remain in an unset state until the user provides the needed modules.

#. Cross-plugin defaults never overwrite existing choices

   - A default from a provider only fills a submodule that has not been set
     yet. It never replaces a choice made by the user or by a provider with
     higher priority.

#. Ease of use

   - Declaring a plugin should stay about as easy as it is with today's
     macros.
   - Writing a plugin should feel like writing a module: everything is set
     up in the constructor.
   - Plugins must be default constructible so the ``ModuleManager`` should be
     able to create them from their type alone, *i.e.*, ``load_plugin<T>()``.

#. Python

   - Plugins written in C++ must be loadable from Python.
   - It should also be possible to write a plugin in Python, the same way
     modules can be written in Python today.

#. Provenance

   - Plugins should know their versions, citations, etc.

#. Options

   - Plugins may have options, e.g., enabling or disabling features.
   - Options would need to be set before or while the plugin is loaded.
   - Not yet designed. Would conceptually be similar to module options, but at
     the plugin level.


***************
Plugin Strategy
***************

The design has two new classes: ``Plugin``, the base class for all plugins,
and ``ModuleMap``, the container a plugin stores its modules in.

The Plugin Class
================

``Plugin`` is declared in ``pluginplay/plugin/plugin.hpp``. It follows the
same convention as ``ModuleBase``. A module's constructor calls base class
functions such as ``description``, ``add_input``, and ``add_submodule`` to
describe the module. In the same way, a plugin's constructor calls base class
functions to describe the plugin:

- ``set_name(name)`` sets the plugin's name, *e.g.*, ``"my_plugin"``.
- ``add_module<T>(key)`` and ``add_module(key, module_base_ptr)`` create a
  module and add it to the plugin's ``ModuleMap``.
- ``change_submod(module_key, callback_key, submod_key)`` links two of the
  plugin's own modules. It is used for intra-plugin defaults.
- ``set_preferred_providers(name0, name1, ...)`` lists the plugins to get
  defaults from, highest priority first.
- ``default_submod_from(provider, module_key, callback_key, provider_key)``
  records a cross-plugin default: "if ``provider`` is loaded, set submodule
  ``callback_key`` of my module ``module_key`` to the provider's module
  ``provider_key``." The ``ModuleManager`` applies these records during
  wiring (see below).
- ``modules()`` returns a non-const reference to the plugin's ``ModuleMap``,
  so the constructor can pass it to helper functions (see
  `Declaring a Plugin`_).

These functions are ``protected``, so only the plugin itself can call them.
After construction, the public interface is read-only: ``name()``,
``preferred_providers()``, ``default_submods_from()``, and ``get_modules()``.

Cross-Plugin Defaults
---------------------

Cross-plugin defaults are stored in the Plugin. The plugin does not have to
write any wiring logic.

- ``provider`` must already be listed in ``set_preferred_providers``.
  Otherwise ``default_submod_from`` throws. This catches typos in provider
  names, and it means priority always comes from one place: the order of
  ``preferred_providers``.
- ``module_key`` must be a module that the plugin has already added.
  Otherwise ``default_submod_from`` throws.
- ``provider_key`` cannot be checked in the constructor, because the
  provider is not available yet. If the provider is loaded but has no module
  ``provider_key``, the record is skipped during wiring. This matches how
  plugins' hand-written ``set_defaults`` functions behave today.

Wiring of the plugins is done when the user calls ``wire_defaults`` (see
:ref:`pp_initialization_design`).

The Class
---------

.. code-block:: c++

   class Plugin {
   public:
       using name_type          = std::string;
       using provider_list_type = std::vector<name_type>;

       /// One default_submod_from record
       struct DefaultSubmod {
           type::key module_key;
           type::key callback_key;
           type::key provider_key;
       };
       using default_list_type = std::vector<DefaultSubmod>;

       virtual ~Plugin() = default;

       /// The name of the plugin, e.g., "my_plugin"
       const name_type& name() const noexcept;

       /// Names of plugins to get defaults from, highest priority first
       const provider_list_type& preferred_providers() const noexcept;

       /// Recorded cross-plugin defaults
       const default_list_type& default_submods_from(
           const name_type& provider) const noexcept;

       /// The plugin's modules, in their current state
       const ModuleMap& get_modules() const noexcept;

   protected:
       // Called from the derived class's constructor
       void set_name(name_type name);

       template<typename T>
       void add_module(type::key key);

       void add_module(type::key key, module_base_ptr base);

       void change_submod(const type::key& module_key,
                          const type::key& callback_key,
                          const type::key& submod_key);

       template<typename... Args>
       void set_preferred_providers(Args&&... names);

       void default_submod_from(name_type provider, type::key module_key,
                                type::key callback_key,
                                type::key provider_key);

       ModuleMap& modules() noexcept;

   private:
       name_type m_name_;
       provider_list_type m_providers_;
       std::map<name_type, default_list_type> m_defaults_;
       ModuleMap m_modules_;
   };

Notes on the members:

- **Where modules live.** A plugin object owns its ``ModuleMap``.
  ``load_plugin`` takes ownership of the plugin object, and the plugin's map
  *is* the ``ModuleManager``'s map for that plugin (see
  :ref:`pp_initialization_design`). The ``ModuleManager`` changes the map
  through a non-const accessor that only it can use. ``get_modules()``
  therefore always returns the current state of the plugin's modules,
  including any changes made by the user or by wiring.
- **One instance per ModuleManager.** Since a plugin holds live modules, a
  plugin object must not be loaded into two ``ModuleManager`` objects.
  ``load_plugin<T>()`` creates a new ``T`` each time, so this only matters
  for the Python form, which takes an instance.
- **Provenance.** Setters for version and citations would follow the same
  convention, *e.g.*, ``set_version`` and ``add_citation`` in the
  constructor, mirroring ``ModuleBase::citation``.

The ModuleMap Class
===================

``ModuleMap`` is a thin wrapper around
``utilities::CaseInsensitiveMap<std::shared_ptr<Module>>``. This is the same
type as the current ``ModuleManager::module_map``. ``ModuleMap`` provides:

- ``add_module<T>(key)`` and ``add_module(key, module_base_ptr)``, which
  mirror the existing ``ModuleManager`` functions. ``Plugin::add_module``
  forwards to these.
- ``count``, ``at``, ``size``, and iteration.
- ``change_submod(module_key, callback_key, submod_key)``, which links two
  modules in the same map. ``Plugin::change_submod`` forwards to this.
- ``change_submod(module_key, callback_key, std::shared_ptr<Module>)``, which
  links a module in this map to a module from another map. It is used for
  cross-plugin defaults.
- ``has_submod(module_key, callback_key)``, which returns whether that
  submodule has been set. It wraps ``SubmoduleRequest::has_module``.
- ``default_submod(...)``, a convenience function with the same overloads
  as ``change_submod``. It sets the submodule only if it is unset. The
  ``ModuleManager`` uses it to apply ``default_submod_from`` records.

``ModuleMap`` enforces that module keys are unique within the map, which
means they are unique within a plugin. Uniqueness across plugins is the
``ModuleManager``'s job (see :ref:`pp_initialization_design`).

Declaring a Plugin
==================

The plugin macros mirror the module macros (``DECLARE_MODULE``,
``MODULE_CTOR``, and ``MODULE_RUN``):

- ``DECLARE_PLUGIN(plugin_name)`` declares a class ``plugin_name`` that
  derives from ``Plugin`` and has a default constructor.
- ``PLUGIN_CTOR(plugin_name)`` starts the definition of the constructor.

A plugin's constructor can also pass ``modules()`` to helper functions that
take a ``ModuleMap&``. This lets a large plugin split module registration
across files, *e.g.*, one helper per subdirectory. Each helper adds its
modules and sets intra-plugin defaults with ``ModuleMap``'s ``add_module``
and ``change_submod``.

Example: Declarative Defaults
-----------------------------

``MyPlugin`` has two modules. ``"My driver"`` has a ``"Step"`` submodule,
which ``MyPlugin`` fills itself, and a ``"Data"`` submodule, which it would
like a provider to fill. ``"My step"`` has a ``"Helper"`` submodule, which it
would also like a provider to fill. ``ProviderA`` provides the modules
``"A data"`` and ``"A helper"``. ``ProviderB`` provides ``"B data"``.

.. code-block:: c++

   // my_plugin/my_plugin.hpp
   namespace my_plugin {
   DECLARE_PLUGIN(MyPlugin);
   }

   // my_plugin/driver/driver.hpp
   namespace my_plugin {
   inline void load_driver_modules(pluginplay::ModuleMap& modules) {
       modules.add_module<MyDriver>("My driver");
       modules.add_module<MyStep>("My step");

       // Intra-plugin default
       modules.change_submod("My driver", "Step", "My step");
   }
   } // namespace my_plugin

   // my_plugin/my_plugin.cpp
   namespace my_plugin {

   PLUGIN_CTOR(MyPlugin) {
       set_name("my_plugin");
       set_preferred_providers("provider_a", "provider_b");

       load_driver_modules(modules());

       // Cross-plugin defaults
       default_submod_from("provider_a", "My driver", "Data", "A data");
       default_submod_from("provider_a", "My step", "Helper", "A helper");
       default_submod_from("provider_b", "My driver", "Data", "B data");
   }

   } // namespace my_plugin

Both providers can fill ``"My driver"``'s ``"Data"`` submodule. Since
``"provider_a"`` comes first in the preferred providers, ``"A data"`` is used
when both are loaded, and ``"B data"`` is used when only ``ProviderB`` is
loaded. :ref:`pp_initialization_design` walks through these cases.

Python
======

Python support follows the pattern used for modules:

- A Python plugin subclasses ``pluginplay.Plugin``. Its ``__init__`` calls
  ``set_name``, ``add_module``, ``change_submod``,
  ``set_preferred_providers``, and ``default_submod_from``, just as a Python
  module's ``__init__`` calls ``description`` and ``add_input``.
- ``EXPORT_PLUGIN(plugin_name, m)`` exports the C++ plugin class to Python
  (instead of a ``load_modules`` function), so a Python user can pass an
  instance to ``ModuleManager.load_plugin``.
- ``ModuleMap`` is exported so Python plugins can use ``default_submod`` and
  related functions.

**********************
Further Considerations
**********************

- **Missing provider modules.** A ``default_submod_from`` record whose
  provider is loaded but has no module ``provider_key`` is skipped, which
  matches today's behavior. This can hide a typo or a module that a newer
  version of the provider renamed. A warning through the ``ModuleManager``'s
  logger may be useful.
- **Plugins that are loaded later.** ``default_submod_from`` records are
  kept after wiring, so calling ``wire_defaults`` again after loading
  another plugin applies the records for the new plugin.
