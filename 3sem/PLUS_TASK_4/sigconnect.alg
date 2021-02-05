SIGUSR1 = 0
SIGUSR2 = 1

Parent:
Want to read a symbol
    Wait for a SIGUSR1/SIGUSR2

    Ignore SIGUSR1/SIGUSR2 here
        Consume
        Send SIGUSR1

Child:
Read symbol
    Ignore SIGUSR1
        Send SIGUSR1/SIGUSR2
        Wait for SIGUSR1 (confirm that parrent got the previous signal)