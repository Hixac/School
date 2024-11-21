
def count_same(s: str, sub: str) -> int:
    ret = 0
    for i in s.split():
        if sub.lower() in i.lower(): ret += 1
    return ret
