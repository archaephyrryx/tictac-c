#!/usr/bin/awk -f
/^[+-]/ { opponent = $0; next; }
{ s[opponent":"$1] += 1; }
END { for (i in s) { print s[i], i } }
