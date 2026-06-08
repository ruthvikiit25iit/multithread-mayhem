# REPORT.md

## 1. race_counter

### Expected Output
The final counter value should be:

20000000

because two threads each increase the counter 10,000,000 times.

### Actual Outputs Seen
- 12471651
- 11971886
- 11468912
- 13284898
- 17272721

Range:
- Lowest value: 11468912
- Highest value: 17272721

### Was the Output Correct?
No.

The correct value was never obtained in the five runs.

### How Often Was it Correct?
- Correct runs: 0/5
- Incorrect runs: 5/5

### Reason
Both threads update the same counter at the same time without synchronization. Because of this, some increments are lost, causing wrong results.

---

### Extra Experiments for race_counter

#### Changing ITERS to 1000
When `ITERS` was changed to 1000, the program sometimes gave the correct output.

Reason:
Since the number of operations is very small, the chance of threads interfering with each other becomes lower. The bug still exists, but it may not appear every time.

---

#### Changing ITERS to 100000000
When `ITERS` was changed to 100000000, the wrong output appeared more often.

Reason:
With more operations, the threads interfere with each other more frequently, so the race condition becomes easier to observe.

---

#### Compiling with -O2
After compiling with `-O2`, the output became more unpredictable.

Reason:
Compiler optimizations change how instructions are executed. Since the program already has a race condition, optimization changes how often the bug appears.

---

## 2. parallel_sum

### Expected Output
The final sum should be:

10000000

### Actual Outputs Seen
- 6557908
- 5188380
- 5353841

Range:
- Lowest value: 5188380
- Highest value: 6557908

### Was the Output Correct?
No.

The correct sum was never obtained.

### How Often Was it Correct?
- Correct runs: 0/3
- Incorrect runs: 3/3

### Reason
Multiple threads update the shared variable `total` at the same time without synchronization, which causes incorrect sums.

---

## 3. bank_chaos

### Expected Output
- Total successful withdrawals should be 10000
- Final balance should be 0

### Actual Outputs Seen

Run 1:
- Alice successful withdrawals: 10000
- Bob successful withdrawals: 0
- Final balance: 0

Run 2:
- Alice successful withdrawals: 10000
- Bob successful withdrawals: 0
- Final balance: 0

Run 3:
- Alice successful withdrawals: 10000
- Bob successful withdrawals: 0
- Final balance: 0

### Was the Output Correct?
The output looked correct in these runs because the balance became 0.

### How Often Was it Correct?
- Correct-looking runs: 3/3

### Reason
Even though the output looked correct, the program still has a race condition because both threads access and update the balance without synchronization. On some systems or runs, wrong behavior may still happen.

## PROBLEM 2

To make the race condition easier to observe, the `counter++` operation was expanded into multiple steps using a temporary variable.

A new variable `temp` was used to first store the value of `counter`, then increment it, and finally assign it back to `counter`. This increased the number of operations involved in updating the counter.

An extra `for` loop was also added between reading and writing the value. This created a small delay and increased the chance that both threads would access the shared variable at the same time.

Because of these changes, thread overlap happened more frequently, making the race condition appear more reliably.

## PROBLEM 3

The file used for testing was a text file containing multiple words.  
The actual total number of words counted from the file was printed using the normal count variable.

The program was tested using different thread counts.

When 2 threads were used, the total words counted by the threads was sometimes closer to the actual value.

When 4 threads were used, the counted value became much lower and inconsistent across runs.

The output changed in different executions because all threads were updating the shared variable `partial_count` at the same time without synchronization.

The expected output was that both values:
- `Total words counted`
- `total words actual`

should be equal.

But the counted value was usually smaller than the actual value because some updates were lost due to race conditions.

As the number of threads increased, the error became more noticeable because more threads were trying to modify the same variable simultaneously.

---

## PROBLEM 4

This simulation is similar to the `bank_chaos` example.

The program simulates ticket booking where:
- Total available seats = 800
- Total customers = 800
- Each customer tries to book 4 tickets

The expected behavior is that tickets should not be overbooked and the available seats should never become negative.

However, multiple threads check and update `avl_seats` at the same time without synchronization.

A delay loop was also added before reducing the seat count. This increased the chance of thread overlap and made the race condition easier to observe.

Because of this race condition, multiple customers may successfully book tickets using the same available seats.

The output was inconsistent across runs and sometimes produced incorrect remaining seat values.

The problem happens because checking the available seats and updating the value are done in separate steps without any locking mechanism.