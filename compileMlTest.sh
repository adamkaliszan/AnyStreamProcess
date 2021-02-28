gcc mlTest.c `python3-config --cflags` -fPIE -lpython3.`python3-config --includes | grep -Eo '[0-9]+$'`
