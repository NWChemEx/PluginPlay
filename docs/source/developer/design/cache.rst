.. _cache_design:

############
Cache Design
############

As a consequence of our :ref:`memoization_design` strategy we have decided to
implement an associative container, called "the cache", to address the
remaining memoization considerations. As was briefly noted in that section,
checkpoint/restart (C/R) also falls under the memoization and thus the cache
must be designed with C/R considerations in mind as well.

.. note::

    We use the convention that "cache" or "the cache" refers to the concept and
    that ``Cache`` refers to the class.

********************
Cache Considerations
********************

The considerations for the cache are really the union of the memoization
considerations not considered in the :ref:`memoization_design` section with the
checkpoint/restart (C/R) considerations laid out in :ref:`checkpoint_restart`.
These memoization and C/R considerations are somewhat orthogonal to each other
aside from the fact that the cache is related to both of them. More specifically
the memoization considerations focus on using the cache to avoid recomputing
quantities, whereas the C/R considerations focus on how we should save the cache
so that it can be reloaded and resued at a later ime.

In the :ref:`memoization_design` section it was decided that at the core of the
cache is a high-performance associative array. The remainder of this section
focuses on enumerating considerations pertaining to how one can unify the
considerations in the :ref:`memoization_design` section with the associative
array concept.

- Since memoization occurs per type ``T`` (``T`` derived from ``ModuleBase``)
  the actual cache will be a nested associative array (``T`` maps to an
  associative array ``A`` and ``A`` actually maps the inputs to results)
- At runtime hashes are likely to be the only feasible means of quickly looking
  up an object (an object's hash can be stored as a member so that it doesn't
  need to be recomputed each time).
- Will likely want to compress values and/or dump them to disk

Hash Considerations
===================

- Many objects are going to contain floating-point values which make hashes very
  sensitive.
- Hashes are great for quick comparisons, but they can collide

  - Choosing a hash algorithm with minimal collisions is ideal
  - Can perform a value comparison to ensure the values really are equal
    (requires storing the actual inputs too)

- Hashes are not human-readable (i.e. dumping a hash table is unlikely to be
  useful for anything other than memoization)

*********************
Cache Implementations
*********************

At its heart the cache involves associative arrays and conceptually we may be
able to use an existing high-performance C/C++ library to implement it. This
section explores the available options, their features, and ultimately their
suitability for our ``Cache`` class.

First up are the C++ libraries which explicitly market themselves as
implementing associative arrays:

- HArray

  - https://github.com/Bazist/HArray
  - Uses a trie (instead of hash table or binary tree)
  - Prefix compression
  - Save/load to disk
  - GPLv3
  - Appears to be abandoned (last commit July 2021). 88 stars and 12 watchers.

- AVL Array

  - https://github.com/mpaland/avl_array
  - Fixed size array (designed for embedded systems)
  - MIT License
  - Appears abandoned (last commit March 2020). 28 stars and 4 watchers.

- uCDB

  - https://github.com/JulStrat/uCDB
  - Designed for Arduino (apparently a competior of Raspberry Pi)
  - Seems like it can only read from existing databases
  - Not licensed.
  - Active development. 5 stars and 1 watcher.

- Vista

  - https://github.com/breese/vista
  - Implements various span classes (including an associative array one)
  - Operate on existing contiguous memory.
  - Does not appear to allow modification.
  - No license.
  - Likely abandoned (last commit May 2020). 4 stars and 3 watchers.

- ceres

  - https://github.com/ceresBakalite/ceres
  - It looks like it's an example project.
  - Appears to just wrap ``std::map``
  - GPLv3
  - Likely abandoned (last commit April 2021). 0 star and 1 watcher.

Next are C libraries for associative arrays:

- libdict

  - https://github.com/fmela/libdict
  - Looks like it's just implementations of common data structures with C APIs
    (no special features)
  - BSD
  - Likely abandoned (last commit May 2019). 255 stars and 28 watchers.

While we've focused on associative arrays up to this point what we're after can
also be considered a database.

- LevelDB

  - https://github.com/google/leveldb
  - Features:

    - key/value arbitrary byte data
    - automatic compression
    - support for threaded access
    - can get snapshots (immutable representations)
    - Disk based (using mulitple files)
    - Checksums for data integrity
  - Appears to be actively maintained. 28.5K stars and 1.3K watchers.

- RocksDB

  - https://github.com/facebook/rocksdb
  - Features:

    - Built on LevelDB
    - Designed for storing data on flash drives
    - Can adapt to pure memory
    - Supports backups and checkpoints
  - GPLv2 or Apache 2.0
  - Appears to be actively maintained. 22K stars and 1K watchers.

.. note::

   The notes on these libraries are primarily based off of documentation. I do
   not have experience with any of the libraries so the descriptions may be
   inaccurate. Activity, star, and watcher information was accurate as of March
   2022 and may have changed since then.
