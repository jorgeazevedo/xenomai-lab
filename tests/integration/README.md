# Functional Tests

WARNING: These tests expect ~/.xenomailab/ to be installed on the system. The
reason is that due to some questionable architecture decisions, the blocks are
hardcoded to look in "~/.xenomailab/workspace" for their instance files. This
is more subtle than it seems, because the "PROJECT/NAME" argument is used as an
identifier for the shm and mutex instances shared with the GUI. So yes, this
should be fixed, but the fix will necessarily break the interface, which means
code changes thoroughout the stack, which means more solid testing, see the
next...

WARNING: Some of these tests may be subject to race conditions, meaning that
sometimes they fail even though the system is fine. Example: in diag4.2, sum
may detect that one of its input signals has been deleted, rather than timing
out and being cool. These tests are still usefull with this bug, so I'm keeping
them in. Consider yourself warned.

These tests explore the interaction between blocks.

## Usage

Run all tests with

    make check

To make it quicker, it's possible to compile the blocks manually with

    ./compileBlocks.sh "tick signal_generator sum gain Z debug"

Then comment the calls to make, and then run `make check`.

## Theory

The behavior we have tested thus far is how blocks react to timeouts on
rt_queue_read (i.e. when a block is left hanging on an input queue by its parent).

This happens in 4 scenarios.

1. Cyclical dependency

The input queue depends on our output.

-ETIMEDOUT on 1st sample. Throw ERROR() so XL can warn the user.

2. Parent received SIGSEV, SIGKILL or SIGFP

XL: Detects it with GDB. Warns the user ands shows the source line where
program received the signal. On OK, starts diagram::stop.

BLOCK: Detects -ETIMEDOUT. Fails gracefully.

3. Parent quit

In response to some valid condition, such as SIGTERM, diagram:stop or `running
= 0`, the parent has exited.

XL: If XL issued diagram:stop, then it must detect if all blocks answered
SIGTERM, if not SIGKILL them. If a cascading fail was trigged externally, XL
should probably warn the user. From the point of view of the library, it's all
the same, but XL needs to distinguish between these 2 situations.

BLOCKS: -ETIMEDOUT. Fail gracefully.

4. Parent Hung

The parent has not exited but has stopped normal operation. 100% CPU or eternal sleep.

XL: Presents a warning for every block that exits before the user clicked
stuff. The dialog box offers the change to continue operation or to
diagram:stop.

BLOCKS: -ETIMEDOUT. Should fail gracefully.


I had a theory that we could detect "Parent quit" with a sequential -ETIMEDOUT
and -EINVAL from rt_read_queue. However, there's a race condition here. If the
parent has already exited on the 2nd call to rt_read_queue, we'll get this
behavior. If the parent is in the process of exiting but has yet to delete the
queue, we'll get -ETIMEDOUT and -EWOULDBLOCk. So it's useless to test
rt_read_queue more than once.

## Diagram 1

The purpose of these tests is to guarantee the correct response to failed queue
reads. We begin with the simplest possible diagram, 

Tick -> Signal_generator

### diag1.1

Tick receives SIGTERM via "killall tick" and successfully terminates.
Signal_generator must time out and successfully terminate also.

	      finalize_block: 	Successfully terminated
	      finalize_block: 	Successfully terminated
	W: read_input_queues: The "root2" queue has timed out! Exiting gracefully. (-ETIMEDOUT -110 Connection timed out)


### diag1.2

Tick receives SIGKILL via "kill -9".
Signal_generator must time out and exit gracefully.

	          start_task: 	tick loop about to start
	      finalize_block: 	Successfully terminated
	W: read_input_queues: The "root2" queue has timed out! Exiting gracefully. (-ETIMEDOUT -110 Connection timed out)

## Diagram 2

Tick -> Signal_generator -> Gain

### diag2.1

Tick receives SIGTERM via "killall tick" and successfully terminates.

Signal_generator and Gain must time out and sucessfully terminate.

	      finalize_block:   Successfully terminated
	      finalize_block:   Successfully terminated
	W: read_input_queues: The "root2" queue has timed out! Exiting gracefully. (-ETIMEDOUT -110 Connection timed out)
	      finalize_block:   Successfully terminated
	W: read_input_queues: The "signal1" queue has timed out! Exiting gracefully. (-ETIMEDOUT -110 Connection timed out)

### diag2.2

Tick receives SIGKILL via "kill -9".

Signal_generator and Gain must time out and sucessfully terminate regardles.

	          start_task:   tick loop about to start
	      finalize_block:   Successfully terminated
	W: read_input_queues: The "root2" queue has timed out! Exiting gracefully. (-ETIMEDOUT -110 Connection timed out)
	      finalize_block:   Successfully terminated
	W: read_input_queues: The "signal1" queue has timed out! Exiting gracefully. (-ETIMEDOUT -110 Connection timed out)

## Diagram 3
                          _______
                         |       |
                         ->      |
Tick -> Signal_generator -> Sum -

### diag3.1

Sum throws ERROR() and detects a possible cyclical dependency.

Tick receives SIGTERM via "killall tick" and successfully terminates.

Signal_generator and times out and sucessfully terminate.

	      finalize_block: 	Successfully terminated
	      finalize_block: 	Successfully terminated
	W: read_input_queues: The "root2" queue has timed out! Exiting gracefully. (-ETIMEDOUT -110 Connection timed out)
	          start_task: 	sum loop about to start
	E: read_input_queues: The "signal1" queue has timed out on the first try! Cyclical dependency? (-ETIMEDOUT -110 Connection timed out)

## Diagram 4
                          _Z^-N_
                         |       |
                         ->      |
Tick -> Signal_generator -> Sum -


This suite of tests explores how we can solve cyclical dependencies by having a
Z^-N block in the loop with N=1. The diagram is terminated by sending SIGTERM
to Tick.

### diag4.1

With N=0, sum will have a cyclical dependency.

Sum and Z^-N will throw an ERROR() and accuse a cyclical dependency.
Tick and Signal_generator will sucessfully terminate.

      finalize_block: 	Successfully terminated
      finalize_block: 	Successfully terminated
W: read_input_queues: The "root2" queue has timed out! Exiting gracefully. (-ETIMEDOUT -110 Connection timed out)
          start_task: 	sum loop about to start
E: read_input_queues: The "signal2" queue has timed out on the first try! Cyclical dependency? (-ETIMEDOUT -110 Connection timed out)
          start_task: 	Z loop about to start
E: read_input_queues: The "signal1" queue has timed out on the first try! Cyclical dependency? (-ETIMEDOUT -110 Connection timed out)


### diag4.2

With N=1, sig/sum/Z will timeout but sucessfully terminate.

      finalize_block: 	Successfully terminated
      finalize_block: 	Successfully terminated
W: read_input_queues: The "root2" queue has timed out! Exiting gracefully. (-ETIMEDOUT -110 Connection timed out)
      finalize_block: 	Successfully terminated
W: read_input_queues: The "signal0" queue has timed out! Exiting gracefully. (-ETIMEDOUT -110 Connection timed out)
      finalize_block: 	Successfully terminated
W: read_input_queues: The "signal1" queue has timed out! Exiting gracefully. (-ETIMEDOUT -110 Connection timed out)



