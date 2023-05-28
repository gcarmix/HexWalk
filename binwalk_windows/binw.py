import binwalk
import sys
if len(sys.argv) == 2:
    binwalk.scan('--signature', sys.argv[1])
elif len(sys.argv) == 3 and sys.argv[1] == '-e':
    binwalk.scan('--signature','--extract', sys.argv[2])
