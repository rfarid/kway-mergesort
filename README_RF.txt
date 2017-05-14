This is a modified version

Source: https://github.com/arq5x/kway-mergesort

kwaymergesort_single_thread.h has the additional items as below:
    displaying a progress bar
    calling expensiveFunc() per each line read from the file

Memory Usage:
    maxBufferSize defined by user

Speed:
    This is an external memory sort-merge example.
    So, to analyse the speed and time complexity, we consider main/external memory exchange.
    General strategy
        Break the data into blocks about the size of the internal memory
        Sort these blocks
        Merge sorted blocks 
    Usually several passes are needed, creating larger sorted blocks until the whole file is sorted
    Time complexity: O(log (N/B)) in which N is the total size and B is the memory size

How to Compile/Run:
    Single Thread version:
        g++ StringProcessorSingleThread.cpp -o testsort
        ./testsort 0 english.txt
    MultiThreaded version:
        qmake sortTool.pro
        make
        ./sortTool 0 english.txt