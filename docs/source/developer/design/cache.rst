.. _cache_design:

############
Cache Design
############

As a consequence of our :ref:`memoization_design` strategy we have decided to
implement an associative container, called "the cache", to address the
remaining memoization considerations. As was briefly noted in the memoization
section, and explicitly called out in :ref:`design_checkpoint_restart`
checkpoint/restart (C/R) will also be handled by the cache.

.. note::

    We use the convention that "cache" or "the cache" refers to the concept and
    that ``Cache`` refers to the class.

********************
Cache Considerations
********************

The considerations for the cache are really the union of the memoization
considerations not considered in the :ref:`memoization_design` section with the
union of the checkpoint/restart (C/R) considerations not considered in
:ref:`design_checkpoint_restart`. The memoization considerations focus on using
the cache to avoid recomputing quantities, whereas the C/R considerations focus
on how we should save the cache so that it can be reloaded and resued at a later
time.

In the :ref:`memoization_design` section it was decided that at the core of the
cache is a high-performance associative array. The remainder of this section
focuses on enumerating considerations pertaining to how one can unify the
considerations in the :ref:`memoization_design` section with the associative
array concept.

- Since memoization occurs per type ``T`` (``T`` derived from ``ModuleBase``)
  the actual cache will be a nested associative array (``T`` maps to an
  associative array ``A`` and ``A`` actually maps the inputs to results)
- Will likely want to compress values and/or dump them to disk
- What are the keys? Input values? Hashes? Universally Unique IDs (UUIDs)?
- How many entries do we expect in a module's cache? 10s? 100s? 1000s?

  - Cache entries are ultimately calls to a module. In a given run, a module
    probably gets called less than 10 times, maybe in the 100s for an iterative
    process, or a very fundamental and frequently called module.
  - 1000+ calls is not unfathomable, but probably rare enough to justify
    treating it separately

- There's a number of associative array types; which one(s) do we want?

  - Certain structures excel at determining if an entry exists. Others excel
    at being able to add entries.

- Checksums for data validity?
- Needs to operate in a distributed environment

  - Each process gets its own cache?
  - For traditional memoization, distributed objects will stay distributed
  - For C/R less clear

    - Could do a checkpoint per process
    - Could do a single, sycnhronized, checkpoint
    - Could do something in between

*********************
Choosing the Key Type
*********************

Ultimately the cache is an associative array mapping inputs to results. To
determine if we have results for a set of inputs we ultimately need to map the
inputs to a key, and determine if the key exists. There's a number of possible
key types:

Using the Values as Keys
========================

Conceptually the simplest option, we use the set of inputs themselves as the key
(i.e., the mapping from inputs to keys is a trivial identity mapping). This has
the following advantages:

- human-readable keys (to the extent that the objects themselves are
  human-readable).
- Preservation of input values.
- Assuming a value comparison is implemented correctly, will never incorrectly
  memoize a call.

and the following disadvantages:

- May lead to very costly comparisons (think large distributed data structures).

  - Ultimately it falls to the object writer to optimize the comparisons for
    their object; however, it's not always feasible to count on developers to be
    willing to further optimize their objects.

- Memory consumption may be a large issue.

  - Storing the literal values will result in copies of the objects
  - Can store references/pointers, but lifetime management becomes an issue.

- Value comparisons are sensitive to floating-point values

  - Could allow users to provide custom comparison operators thereby allowing
    them to choose what to do in such cases.
  - Object developers may have already considered this in the design of the
    comparison operators.

Using Hashes as Keys
====================

Another option is to treat the associative array as a hash table and to use
hashes as keys.

Pros:

- Short look-up times (can be constant depending on the specific associative
  array implementation)
- Hash collisions aside, with the right hash algorithm the same object will
  always generate the same hash (i.e., we don't have to store the inputs, just
  the hashes)

Cons:

- All objects must be hashable. Places additional burden on object developers.
- Hashes can be very fragile

  - For a deterministic hash algorithm, one needs to prepare the input in the
    exact same state in order to guarantee the same hash. This can be difficult
    for a variety of reasons:

    - Operations which are equivalent in infinite precision arithmetic are in
      general not equivalent with finite precision arithmetic
    - Objects may have slightly different representations depending on the
      compiler, compiler settings, computing platform, etc.
    - Precision may be lost as a result of checkpointing (e.g. lossy
      compression, string to float conversions)

- Hash collisions are possible, albeit extremely unlikely with modern algorithms

  - Can perform a value comparison to guarantee the keys really are equal
    (requires storing the actual inputs too)
  - A notable exception to collision rarity occurs when two different objects
    have an unintentionally symmetric state. A common example is empty
    containers of different types; if one simply hashes the elements in the
    containers by looping over the container, and if the types of the containers
    are not hashed, the resulting hash value will be equal.

- Hashes are not human-readable (i.e. dumping a hash table is unlikely to be
  useful for anything other than memoization)

  - Combined with the fragility aspect, hashing is not suitable for long-term
    data archival

- Generally speaking, hashes can not be inverted, i.e., given just the hash it's
  not possible to determine what object was hashed.
- Distributed objects can be tricky

  - Can have each process hash its local part, requires no synchronization
  - Hashing the entire object requires synchrnoization


Universally Unique ID Considerations
====================================

Pros:

- Usage does not require the objects to be hashable
- Comparisons of UUIDs are quick
- Relatively straightforward to guarantee that the same UUID is not given to
  different objects.
- Could be potentially useful for data-archival purposes

Cons:

- Requires bookkeeping to maintain the association between the UUID and the
  object it was generated for.

  - Adding a member to the class avoids needing to store a map from instance to
    UUID, but complicates the object's semantics (the UUID needs updated when
    the data changes).
  - Easy enough to automate with a mix-in

- Not human-readable
- Can not be inverted
- Requires synchronization to assign the same UUID in all processes.

Digital Object Identifier
=========================

UUIDs are conceptually similar to DOIs. So another possibility is to use DOIs as
the keys:

Pros:

- Widely used to identify journal articles, research reports, and data sets
- Uniquely identifies an object
- Through the DOI site allows users to easily obtain the actual object

Cons:

- Object needs to be registered with the International DOI Foundation to be a
  true DOI
- Costs money to get a DOI


*********************
Cache Implementations
*********************

At its heart the cache involves associative arrays and conceptually we may be
able to use an existing high-performance C/C++ library to implement it. This
section explores the available options, their features, and ultimately their
suitability for our ``Cache`` class.

.. note::

   The notes on these libraries are primarily based off of documentation. I do
   not have experience with any of the libraries so the descriptions may be
   inaccurate. Activity, star, and watcher information was accurate as of March
   2022 and may have changed since then.

C++ Associative Container Libraries
===================================

First up are the C++ libraries on GitHub with the ``associative-array`` tag:

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

C Libraries for Assoicative Containers
======================================

Next are C libraries on GitHub with the ``associative-array`` tag:

- libdict

  - https://github.com/fmela/libdict
  - Looks like it's just implementations of common data structures with C APIs
    (no special features)
  - BSD
  - Likely abandoned (last commit May 2019). 255 stars and 28 watchers.

- thmap

  - https://github.com/rmind/thmap
  - Concurrent implementation of a trie-hash map
  - BSD
  - Likely abandoned (last commit April 2020). 63 stars and 7 watchers.

- rhashmap

  - https://github.com/rmind/rhashmap
  - Hash map that uses robin hood hashing
  - BSD
  - Last commit September 2021. 40 stars and 5 watchers.

- cdict

  - https://github.com/mkostoevr/cdict
  - Seems like a limited functionality replacement for std::map (in C)
  - MIT
  - Last commit November 2021. 2 stars and 1 watcher.

- core-array

  - https://github.com/Johns-Q/core-array
  - Appears to be a simple map class which can only have integer keys and
    integer values.
  - AGPLv3
  - Last commit September 2021. 1 star and 1 watcher.

- c_vector_map

  - https://github.com/michael105/c_vector_map
  - Maps strings to integers.
  - Documentation says it was designed/optimized for a highly-specific usecase.
  - BSD
  - Last commit April 2021. 0 star and 1 watcher.

- Data-Structures-C

  - https://github.com/chivington/Data-Structures-C
  - Appears to be someone's toy project to learn about data structures.
  - Documentation states it's not production ready
  - DO_WHATEVER_YOU_WANT license
  - Likely abandoned (last commit July 2019). 0 stars and 0 watchers.

Databases
=========

While we've focused on associative arrays up to this point, but what we're after
can also be considered a database. The advantage of moving to databases is that
we get some real heavy-hitters contributing software. Putting the cart before
the horse, our current cacheing strategy will rely on databases for a number of
the considerations. The full design discussion is lengthy and we defer that
conversation to :ref:`database_design`.

**************
Cache Strategy
**************

Each ``ModuleManager`` instance can have at most one ``Cache`` instance. This
ensures there's a single source of truth for the program. The ``Cache`` will be
hierarchical since memoization only makes sense for modules of the same C++
type. The ``Cache`` refers to the entire cache object, whereas ``ModuleCache``
refers to the cache for all modules of the same C++ type. Thus ``Cache`` can
be thought of as a map from C++ type to ``ModuleCache`` instances.

From a feature perspective most of the databases are capable of doing what we
want for ``ModuleCache``. Furthermore, many of the databases are heavily used
and widely supported. Whether the databases can do what we want, in a high-
performance computing setting is a different story. It is our opinion that
relying on a database for ``ModuleCache`` could potentially save a lot of
development time. Since the database will live under the hood of
``ModuleCache``, worst case scenario, we could swap out the database with
minimal disruptions to the remainder of the stack. We anticipate that reality
will be such that we can use the database for 90% of our needs and will need to
dispatch for the other 10%; assuming this is the case, using a database seems to
be a huge win. We defer the discussion of which database to use to (TODO: link
to discussion).

``std::type_index`` will suffice for the keys to ``Cache`` since we are only
trying to determine if modules have the same type. A major remaining decision is
shat to use for the keys to ``ModuleCache``? The financial burden of (properly)
assigning DOIs to inputs, and the need to synchronize those DOIs with the
International DOI Foundation, makes true DOIs impractical. UUIDs are more or
less DOIs without the central authority or financial burden. The biggest problem
with UUIDs is that if we wanted all processes to agree on the UUID we'd have to
synchronize the values. This could become burdensome as such a synchronization
would need to occur everytime the data changes. One seemingly promising solution
is to combine the value and hashing options to try to get the best of both.

ModuleCache Key Strategy
========================

Our solution is to maintain a map from input values to hashes, and then map the
hashes to the results. The use of the auxiliary map serves several purposes:

#. it preserves the input values in a way ammenable to human-readable-ness
#. it prevents memoization from incorrectly occuring (assuming the value
   comparisons are correctly implemented) since we have to do a value comparison
   to get the hash
#. the hashes can serve as checksums for data integrity (rehash key and compare
   it to the stored hash)
#. there is a mechanism for the user to decide when data is "close enough" for
   reuse.

   - e.g. say the input is a floating point value, the user can override the key
     comparison so that the floating point values compare equal as long as they
     agree to within some tolerance.

#. we "factor out" the value

   - If we just had the value to result map, the value-based keys could end up
     requiring us to store copies of the same input in multiple keys

Arguably the largest problem with using hashes is avoiding collisions between
objects with different types, but otherwise symmetric states. In our present
strategy note that:

- ``ModuleCache`` instances are per module ``std::type_index``
- Each ``std::type_index`` maps to a set input API
- The types of the inputs being hashed have already been checked against the
  input API (and an error raised if they had a wrong type)
- The auxiliary map allows us to map each input to a hash
- Results are stored under a tuple of hashes (the :math:`i`-th hash being
  the hash for the :math:`i`-th poisitonal argument)

Then we have established a one-to-one map from the original input values to the
tuple of hashes and can confidently return the results. There are two cavets to
this:

#. The input to hash map must be one-to-one, i.e., we must detect legitimate
   hash collisions (easily done by ensuring a hash is not already in the map
   before inserting it, if it is in the map we crash and tell the user to go
   buy a lottery ticket because odds are that won't happen again before the
   heat death of the universe...).
#. Since module instances set their APIs at runtime, it's entirely possible
   for two module instances to have different input APIs.

   - The default in PluginPlay is to make modules via the default ctor, in part
     to avoid this problem. So if the user wanted to use conditional logic to
     switch the API, they would have to do somewhat creatively. Point is, for
     now it probably suffices to just tell the user it's a big no-no to have
     different APIs for different instances of the same module.

Cache and Iterative Modules
===========================

This strategy does not address how iterative modules will be memoized yet, nor
will the database likely help on this issue. The primary problem to solve is how
to avoid needing to store all of the intermediate results. Our current strategy
is to more or less punt and make module developers do it. More specifically we
actually store two ``ModuleCache`` instances for each C++ module type. This
second instance is referred to as the module's internal cache. The module is
free to put whatever values it wants in the internal cache under whatever keys
it wants.

In practice iterative modules take the initial input and the current input.
Internally, before returning, iterative modules cache the result computed from
the current input under the initial input. When the initial input is the same as
the current input we know one of two things is happening: it's the true first
iteration (known becuase there is no value cached under the initial input) or we
are restarting (known because we have a partial result cached). The key
difference compared to normal memoization is that the module developer is
overriding the memoized value every iteration. N.B. that when a module developer
is going to do their own memoization they should turn off memoization for the
module otherwise PluginPlay will still memoize the outer call.

Cache Considerations Addressed
==============================

With respect to the cache considerations this strategy addresses the following
considerations:

- Each class ``T`` deriving from ``ModuleBase`` is assumed to wrap a unique
  algorithm. When memoizing it thus only makes sense to consider results
  computed by other ``T`` instances.

  - Addressed by ``Cache`` vs ``ModuleCache``

- Failing to memoize when appropriate will affect performance

  - There's only so much we can do here. The auxiliary value-to-hash map will
    allow the user to force memoziation to occur whenever they want.

- Memoizing when not appropriate will compromise the integrity of the scientific
  answer.

  - Assuming value comparison is implemented correctly for each object, the
    one-to-one mapping from input value to results established in this strategy
    should prevent this.

- Memoizing iterative (recursive) function requires special considerations

  - Addressed by having an internal cache.

- Comparing objects can be expensive (think about distributed tensors)

  - We still have value comparisons, but the majority of the comparisons will
    now be done by hashes. If the few value comparisons that remain become a
    bottleneck we assume this is indicative of the value comparisons needing
    further optimization.

- Module developers may want to do their own C/R.

  - Addresed by having an internal cache. Module developers can put whatever
    they want in that cache and have it be checkpointed.
  - Module developers are also free to ignore that cache and do something else
    if they would like.

- Since memoization occurs per type ``T`` (``T`` derived from ``ModuleBase``)
  the actual cache will be a nested associative array (``T`` maps to an
  associative array ``A`` and ``A`` actually maps the inputs to results)

  - The ``Cache`` nested ``CacheModule`` addresses this.

- What are the keys? Input values? Hashes? Universally Unique IDs (UUIDs)?

  - The final decision is a hybrid of values and hashes.

- How many entries do we expect in a module's cache? 10s? 100s? 1000s?

  - Cache entries are ultimately calls to a module. In a given run, a module
    probably gets called less than 10 times, maybe in the 100s for an iterative
    process, or a very fundamental and frequently called module.
  - 1000+ calls is not unfathomable, but probably rare enough to justify
    treating it separately

- Checksums for data validity?

  - The hashes can serve as checksums

*********************
Cache Class Hierarchy
*********************

TODO: Write me.
