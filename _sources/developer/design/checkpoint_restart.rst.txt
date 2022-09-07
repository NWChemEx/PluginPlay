.. Copyright 2022 NWChemEx-Project
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

.. _design_checkpoint_restart:

##################
Checkpoint/Restart
##################

The need for checkpoint/restart (C/R) was briefly touched upon in
:ref:`memoization_design` with a full discussion deferred to this section. From
the PluginPlay perspective C/R is a subset of the memoization needs, more
specifically C/R amounts to avoiding recomputation by using a previously created
checkpoint. In PluginPlay C/R is unified with memoization by using the
checkpoint to create a Cache instance.

************
What is C/R?
************

C/R is the high-performance computing equivalent of saving/loading. In
conventional usage C/R primarily targets fault resilience (being able to resume
a program when something goes wrong) and is typically done in the background of
the program's execution. Typical C/R usage usually has a short turn around time
from the checkpoint action to restarting. On the contrary, saving/loading is
typically a manual process (the chooses to save; although the program may also
maintain an autosave back-up as a convenience) and usually strives to store the
data in a manner ammenable to resuming work after a prolonged period of time. In
modern practice the exact distinction between save/load and C/R is fuzzy at
best and for our purposes we consider them together under the unified concept of
C/R.

C/R is typically handled in one of two ways: by the operating system (OS) or by
the application. While OS-based C/R is automatic (from the application
developer's perspective) it's of limited utility in that it's extremely
expensive (the OS more or less snapshots the state of all running programs and
all memory). The main application of OS-based C/R is suspending/hibernating the
current user's session. OS-based C/R is typically not designed for transferring
the checkpoint state to other devices and the next user action after a
checkpoint is expected to be resuming the session (albeit with possibly a
significant amount of time between the actions). On the contrary,
application-based C/R is not automatic (although tools help to facilitate it),
can be expensive or cheap (depends on what is being checkpointed and how the
developer does the checkpointing), and allows other actions to occur between the
checkpoint and the restart.

*******************
Why Do We Need C/R?
*******************

Historically many electronic structure packages have had limited or no C/R
capabilities. However, fault rates are expected to increase substantially with
exascale computing making it prudent for us to consider C/R if for no other
reason than resliancy. Arguably the more important reason to have C/R is for
scientific integrity. It should be possible to reload previous results and
verify/build upon them in some manner. Another reason is user friendliness.
Users have come to expect their state to be saved in most serious programs.
Having C/R means a user doesn't need to rerun the entire program just because
an error occurred.

******************
C/R Considerations
******************

There are a number of C/R considerations which we have grouped based on whether
they pertain to the objects being C/R, the hardware being used to C/R, or the
design of PluginPlay.

Object Considerations
=====================

- The amount of data to checkpoint can be formidable

   - Need ways to control what is checkpointed
   - Does the ability to turn on/off memoization suffice?

- Will need to checkpoint distributed objects

   - For traditional memoization want to keep the object distributed.
   - For C/R likely will want to pull the objects back to a single (or small
     number of files)

- May need to checkpoint different objects at different frequencies

   - To minimize checkpointing overhead larger objects may be checkpointed less
     frequently than smaller objects.

- Serialization is the primary means of disassembling/assembling objects and
  thus intimately related to C/R

Hardware Considerations
=======================

- Exascale machines include memory hierarchies that need checkpointed (e.g.
  burst buffers, nonvolatile memory)
- Should checkpoints be hardware-specific (e.g. checkpoint may assume access to
  a certain number of processes)

   - For immediate restart, hardware-specific checkpoints make sense, but for
     long-term archival the storage format needs to be platform agnostic


PluginPlay Specific Considerations
==================================

- Generally speaking, in PluginPlay most data that needs checkpointed will live
  in the cache.
- C/R is related to memoization; primarily differs in that C/R needs to remain
  valid for a much longer time frame than memoization
- True C/R requires built in resiliency (storing redundant data)
- Reloading a cache will allow PluginPlay to quickly regain parity with the
  previous calculation by relying on memoization.
- Converting from C/R format to memoized format may be necessary since the two
  have different target time ranges.
- Module developers may want to do their own C/R.

*******************
C/R Implementations
*******************

Saving/loading is an ubiquitous aspect of the modern user experience. For this
reason we expected to find a number of existing C/R candidates; however, we
were only able to locate a few existing libraries. It is possible that we are
not using the correct terminology (we tried both the C/R and save/load
terminology), or that C/R is just so intimately tied to a program's design that
no good general solution exists.

- DMTCP

  - https://github.com/dmtcp/dmtcp
  - Monitors a program's processes and threads to automatically checkpoint it
  - Not at all clear how this works (grabbing all memory?)
  - LGPLv3
  - Active development. 291 stars and 27 watchers.

- SCR

   - https://github.com/LLNL/scr
   - Appears to primarily be used to save to a parallel filesystem in the
     background.
   - Looks like user is still responsible for writing the files and interacting
     with MPI.
   - Appears to require user to call program through SCR wrapper
   - BSD License.
   - Active development. 76 stars and 21 watchers.

- VELOC

   - https://github.com/ECP-VeloC/VELOC
   - Looks like it's designed to replace SCR
   - Supports monitoring memory regions, will automatically C/R said regions
   - For more complex objects, has a file API. Like SCR users seem to be
     responsible for manually filling in/reading from the file.
   - MIT License
   - Active development. 35 stars and 11 watchers.


.. note::

   We do not have experience using any of these libraries. Notes are based on
   readmes and documentation, and may or may not actually reflect the features/
   state of the libraries. Stars and watchers were accurate as of March 2022.

************
C/R Strategy
************

The existing C/R libraries do not seem to be widely used. Perhaps the larger
problem with the existing C/R libraries is that it is not clear what they have
to offer (unless our program is so simple we can just tell it what memory
addresses to monitor). For this reason, we have opted to implement our own C/R
solution.

C/R is a form of memoization and we intend for it to occur in PluginPlay via the
cache concept. Thus we punt almost all of the C/R considerations to the cache
discussion in :ref:`cache_design`. The exceptions are the following
considerations which we address here:

- The amount of data to checkpoint can be formidable

  - With the current strategy turning on/off memoization also turns off C/R.

- Generally speaking, in PluginPlay most data that needs checkpointed will live
  in the cache.

  - This is the primary motivation behind unify the two.

- Reloading a cache will allow PluginPlay to quickly regain parity with the
  previous calculation by relying on memoization.

  - The current C/R strategy relies heavily on this consideration
