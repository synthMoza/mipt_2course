{Old version}

Writer:
1) Create and open target fifo (O_RDWR, O_NONBLOCK)
2) Open (create if needed) cross_proc fifo (O_WRONLY), wait for the reader
3) Open input file
4) Write the pid into the cross_proc fifo
5) Remove the O_NONBLOCK flag from target fifo
6) Write into the target fifo
7) Close

Reader:
1) Open (and creates if needed) cross_proc fifo (O_NONBLOCK, O_RDONLY) [doesn't block as it is O_NONBLOCK]
2) Wait for pid ('select' on cross_proc fifo) [permitted single timeout]
3) Read the pid from cross_proc fifo [as 'read' follows 'select' and this fifo is nonblocking, works correctly]
4) Open target fifo (O_NONBLOCK, O_RDONLY)  [doesn't block as it is O_NONBLOCK]
5) Remove the O_NONBLOCK flag from target fifo  [no comments]
6) Read from target fifo ['read' is correct - see Situations R1211 and R1212]
7) Write into STDOUT_FILENO [doesn't block anyway]
8) Close [succesful exit]

Situations: ('0' - expected behaviour, '-1' - bad behaviour )
1) Reader:
    1) Open (and create if needed) cross_proc fifo nonblocked, wait for pid with select
        1) No pid in the cross_proc fifo - finish the program (0)
            (!) Select has gone wrong, but then writer puts the pid into fifo - doesn't break, 
                but needs to be restarted (0)
        2) There is a pid - read it, then open it with O_NONBLOCK:
            1) Writer didn't manage to write something yet
                1) Writer is aborted - finish the program (0)
                2) Writer is alive - wait for it, read until writer finishes 
                    (aborts or finishes corrrectly), finish the programm (0)
2) Writer:
    1) Opens target fifo (O_RDWR, O_NONBLOCK), open cross proc fifo (O_WRONLY), open input file, 
    write the pid, remove the O_NONBLOCK flag, try to write into target fifo
        1) Reader stopped at open and haven't got any extra time - 'write' will 
            fail and exit, reader will freeze forever (-1)
            (!) Difficult to emulate, but if reader sleeps after openning target fifo, both will finish 
            with return code 0, but it isn't expected behaviour
        2) Reader has been given time to proceed after 'open' - writer writes data into fifo and finishes correctly (0)
    
Try to solve unexpected behaviour W11:
1) Reader must indicate it has opened the fifo and is ready to proceed - for example, through cross_proc fifo,
but writer must wait for it for some time (insert 'select' here)

{New version}

SWITCH WRITER AND READER
Reader:
1) Create and open target fifo (O_RDONLY, O_NONBLOCK)
2) Open (create if needed) cross_proc fifo (O_WRONLY), wait for the writer
3) Write the pid into the cross_proc fifo
4) Remove the O_NONBLOCK flag from target fifo
5) Use 'select' on target_fifo. If it fails - finish the program.
5) Read from target fifo
6) Write into STDOUT_FILENO [doesn't block anyway]
7) Close

Writer:
1) Open (and create if needed) cross_proc fifo (O_RDONLY), wait for the reader
2) Read the pid from cross_proc fifo
3) Open target fifo (O_WRONLY | O_NONBLOCK) [if reader is still alive, it won't fail]
4) Open input file
4) Remove the O_NONBLOCK flag from target fifo
5) Write into target fifo [target fifo is opened by reader, if not - reader is dead]
6) Close

Situations:
1) Reader
    1) Create and open target fifo, open (create if needed) cross_proc fifo,
         wait for the writer, write the pid into fifo
         1) Write fails - writer has already died, as they synced by 'open'
         2) Write succedes - writer is alive, remove the O_NONBLOCK from target_fifo,
            read from target_fifo
            1) Writer hasn't opened target_fifo yet - select fails (0)?
            2) Writer has already opened target_fifo - read is succesful, 
                write into STDOUT_FILENO, finish the program (0)

2) Writer:
    1) Open (create if needed) cross_proc, wait for the reader, read the pid
        1) Read fails - reader has already died, as they are synced with 'open'
        2) Read succedes - open target fifo
            1) Open fails - reader has already died, finish the program (0)
            2) Opens succedes - write data into target_fifo until end or reader dies (0)

Critical sections:
Reader:
1)  Lines: 59-59 ('read' syscall)
    Resource: target_fifo
    Who: reader and writer
Writer:
1)  Lines: 31-31 ('read' syscall)
    Resource: cross_proc FIFO
    Who: several writers
2)  Lines: 60-60
    Resource: target_fifo
    Who: reader and writer
