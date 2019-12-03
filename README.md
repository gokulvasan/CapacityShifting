OPTIMIZING slot shift using SIMSO simulator [Capacity Shifting]
===============================================================

Optimisation:
-------------
* Desynchronize the slots and let the scheduler exhibit natural EDF behaviour.
* Reduce time complexity of Guarantee Algorithm from O(N^2) to O(N).

SLOT SHIFTING :
----------------

  * What  is slotshifting?
    * One among the few scheduling algorithms that guarantees aperiodics on admittance.
    * [Algorithmic View of slot shifting]( https://github.com/gokulvasan/Slot-shifting-in-LITMUS-RT-Kernel-2.6/blob/master/documentations/SlotShifting.pdf )
    * [original paper](https://www.slideshare.net/slideshow/embed_code/key/PJt8vhtGcHvKQ)

CAPACITY SHIFTING:
------------------
* **Goal:** Making slot shifting scheduling  algorithm 60% efficient  by addressing the following problems:

* **Problem1:** Updating Spare Capacity
  * The function update-sc is applied for every slot.
  * The problem arises when job that does not belong to the current interval is executing in current interval and spare capacity of the job’s Interval is negative then we traverse Backwards through all the consecutive negative intervals and increment the spare capacity of the intervals till either we reach positive interval or current interval, which we presume is the lender to these backward consecutive intervals.
  * The function update-sc becomes an consistent Overhead when
  * current executing job’s Interval is not Current interval and job’s interval’s spare capacity is a BIG negative number, which intuitively means the borrow from the lender interval is also big.
  * Interval to which task belongs is far away from lender,i.e. there are many intervals in between the lender and job’s interval.
  * overhead further complicates when current job is the only task being selected for the next consecutive slots of Current Interval, which might be a normal scenario in EDF. 

  * **Solution:** Deferred update.

* **Problem2:** Slots
  * Notion of slots might be a good approach in distributed systems but the system that has centralised clock trigger, slot adds
following overhead.
  *  Increases the number of time scheduling decision needs to be made.
  * Might make aperiodic miss deadline.
  
  * **Solution:** Desynchronization.

* **Problem3:**  Acceptance and Guarantee Algorithm
  * Additional space of reference between job and interval is needed.
  * Increases runtime complexity in the calculation of online spare capacity.
  * Worst and average case time complexity is  O(N.M), where 
       * N is the number of intervals.
       * M is the job per interval.
       * if(M == N), the time complexity is O(N^2).

  * **Solution:** New Algorithm that runs O(N).

**Conclusion:**
Capacity Shifting is a methodical approach to remove slots but still provide the flexibility and guarantee provided by slot shifting.

SIMSO Modification:
-------------------
* slot shifting needs a seperate data structure called interval which needs to be associated with job of the task to make this happen simso itself needs modification to adapt to different DS.
* In this implementation I tried to make this modification as generic as possible so that the modification
can be used for other schedulers as well. 

REPORT :
--------
[Capacity Shifting ](https://github.com/gokulvasan/CapacityShifting/blob/master/capacityshifting_finalReport.pdf)
