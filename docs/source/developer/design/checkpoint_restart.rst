.. _design_checkpoint_restart:

##################
Checkpoint/Restart
##################

Checkpoint/restart (C/R)

************
What is C/R?
************

C/R is the high-performance computing equivalent of saving/loading. In
conventional usage C/R primarily targets fault resilience (being able to resume
a program when something goes wrong). In this typical usage, the time from the
checkpoint to the restart is usually short and long term data archival
considerations can usually be ignored. On the contrary, saving/loading usually
attempts to store data in such a way that it can be used to resume work after a
prolonged period of time.

C/R is typically handled in one of two ways: by the operating system (OS) or by
the application. While OS-based C/R is automatic (from the application
developer's perspective) its of limited utility in that its extremely expensive
(the OS more or less snapshots the state of all running programs and all
memory). The main application of OS-based C/R is suspending/hibernating the
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

- Fault rates are expected to increase substantially with exascale computing

******************
C/R Considerations
******************

Object Considerations
=====================

- The amount of data to checkpoint can be formidable
- Will need to checkpoint distributed objects
- May need to checkpoint different objects at different frequencies
- Serialization is the primary means of disassembling/assembling objects and
  thus intimately related

Hardware Considerations
=======================

- Exascale machines include memory hierarchies that need checkpointed (e.g.
  burst buffers, nonvolatile memory)
- Should be a system portable solution and not require access to the same system
  to restart.

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

***********************
Potential C/R Solutions
***********************

- disk based checkpointing

  - In the context of PluginPlay this amounts to dumping the cache to disk
  - Highly resilient, but very expensive for large objects
  - Will likely want a variety of possible file backends (e.g. JSON for making
    it human-readable, or HDF5 for performance)
  - Need to consider both serial and parallel file systems.

- in memory checkpointing

  - Primarily for resiliency since memory is volatile (although nonvolatile
    memory also exists)
  -
