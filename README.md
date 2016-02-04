optimizing slot shift using SIMSO simulator

SLOT SHIFTING : 
Static scheduling has been shown to be appropriate for a variety of hard real-time systems, mainly due to the verifiable timing behavior of the system and the complex task models supported.
Its application is, however, impeded in systems with changing operational modes and critical activities, that arrive infrequently with unknown occurrence times. This thesis presents an approach to overcome these shortcomings. It addresses static scheduling, illustrated by an algorithm serving as case study, and provides concepts to extend the scope of statically scheduled systems to deal with mode changes properly and to provide for efficient handling of dynamic activities.

Mode changes are performed by switching from one periodically executing static schedule to another via a special schedule to prepare for the change. As all involved schedules are constructed statically, all actions executing the mode change do so deterministically: Given the current mode, time, and mode change request, the exact executions during the mode change and their completion time are known before run-time.

Dynamic activities are incorporated in to static schedules by making use of the unused resources and leeways in the schedule. We present mechanisms to effectively maintain information about the amount of dynamic activity that can be accommodated without impairing the feasible execution of statically scheduled tasks. On top of this service, aperiodic tasks can be handled in a very simple way. We furthermore present an on-line guarantee algorithm.

A combined approach integrates both methods.

CAPACITY SHIFTING: 
Slot shifting tries to make decision on a defined time frame called slots, This approach is basically used for convergence on schedulibity decision
in a distributed environment, but This is a overhead on a system.

Capacity Shifting is a methodical approach to remove slots but still provide the flexibility and guarantee provided by slot shifting.

