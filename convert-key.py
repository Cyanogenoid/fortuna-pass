import sys

s = sys.stdin.read().replace('\n', '').strip()
words = [s[i:i+2] for i in range(0, len(s), 2)]
little_endian_words = reversed(words)
print(''.join('\\x{}'.format(word) for word in little_endian_words))
