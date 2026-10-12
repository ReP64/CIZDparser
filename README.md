send data from a CI200Sc to a zebra printer ZD420
the data from the CI200SC format is BP-DT4 Form0
the programming for de ZD420 is ZPL format

    CI200SC settings
    F27 0           8N1
    F28 4           9600
    F29 1   F33 0   0=prt 1=pc
    F30 0   F34 0   22byte
    F31 1   F35 1   1 stream
    F40 3           BP-DT4 format
    F41 0         ` FORM0
    F42 0           manual print
    F43 1           line feed

    BP-DT4 format
    V00   NETWEIGHT   7
    V01   UNIT        2
    V02   GROSS       7
    V03   TARE        7
    V04   DATE        10
    V05   TIME        8
    V06   ID          2
    V07   COUNT       3
    V08   NET:BARCODE 6
    V09   TOTAL NET   9
